#include <string>
#include <typeinfo>
#include <exception>
#include <assert.h>

#include "StudentWorld.h"
#include "IceMan.h"
#include "Actor.h"
#include "Ice.h"
#include "OilBarrel.h"
#include "Gold.h"
#include "SonarKit.h"
#include "WaterPool.h"
#include "RegularProtester.h"
#include "HardcoreProtester.h"
#include "Event.h"

using namespace std;

const int ICEMAN_TO_ICE_SIZE_RATIO = (int)(ICEMAN_SIZE / ICE_SIZE);

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
// Constructor
StudentWorld::StudentWorld(std::string assetDir)
:	GameWorld(assetDir), 
	m_nTick(0), 
	m_actors(), 
	m_pIceMan(),
	m_distances(),
	m_events(),
	m_eventListeners(),
	m_distanceCalc()
{
}

// Destructor
StudentWorld::~StudentWorld() {
}

// Perform initializion
int StudentWorld::init()
{
	// Reset time
	m_nTick = 0;

	// Initialize ice field - only uses lower 60 squares of screen
	for (int x = 0; x < ICE_WIDTH; x++) {
		for (int y = 0; y < ICE_HEIGHT; y++) {
			// If we're within the tunnel, skip laying ice
			if (y > 0 && x >= 30 && x <= 33)
				continue;

			try {
				// Instantiate an ice block
				m_ice[x][y] = make_shared<Ice>(this, x, y);
			}
			catch (bad_alloc& /*ex*/) {
				cout << "Unable to allocate memory for an ice block" << endl;
			}
		}
	}

	// Initialize IceMan
	shared_ptr<IceMan> pIceMan;

	try {
		// Instantiate IceMan
		pIceMan = make_shared<IceMan>(this);

		// Retain a weak pointer so we can easily reach IceMan when necessary
		m_pIceMan = pIceMan;

		m_actors.push_back(pIceMan);
	}
	catch (bad_alloc& /*ex*/) {
		cout << "Unable to allocate memory for IceMan" << endl;
	}

	// Initialize OilBarrels
	// Maybe handle this with a helper function?
	shared_ptr<OilBarrel> pOilBarrel;
	int NUM_OIL_BARRELS = 5; // TODO: how many should spawn at the start?
	setNumBarrels(NUM_OIL_BARRELS);
	for (int i = 0; i < NUM_OIL_BARRELS; ++i) {
		try {
			pOilBarrel = make_shared<OilBarrel>(this, getRandomX(), getRandomY());
			m_actors.push_back(pOilBarrel);
		}
		catch (bad_alloc&) {
			cout << "Unable to allocate memory for Oil Barrel" << endl;
		}
	}

	// Initialize Gold Nuggets
	shared_ptr<Gold> pGold;
	int NUM_GOLD_NUGGETS = 1; // TODO: how many should spawn at the start?
	for (int i = 0; i < NUM_GOLD_NUGGETS; ++i) {
		try {
			pGold = make_shared<Gold>(this, getRandomX(), getRandomY(), true, true, false, true);

			m_actors.push_back(pGold);
		}
		catch (bad_alloc&) {
			cout << "Unable to allocate memory for Gold Nugget" << endl;
		}
	}
	
	// Initialize Sonar Kits
	shared_ptr<SonarKit> pSonar;
	int NUM_SONAR_KITS = 1; // TODO: how many should spawn at the start?
	for (int i = 0; i < NUM_SONAR_KITS; ++i) {
		try {
			pSonar = make_shared<SonarKit>(this, getRandomX(), getRandomY(), true);

			m_actors.push_back(pSonar);
		}
		catch (bad_alloc&) {
			cout << "Unable to allocate memory for Sonar Kit" << endl;
		}
	}

	// Initialize Water Pools
	shared_ptr<WaterPool> pWaterPool;
	int NUM_WATER_POOLS = 1; // FIXME how many should spawn at the start?
	for (int i = 0; i < NUM_SONAR_KITS; ++i) {
		try {
			pWaterPool = make_shared<WaterPool>(this, getRandomX(), getRandomY());

			m_actors.push_back(pWaterPool);
		}
		catch (bad_alloc&) {
			cout << "Unable to allocate memory for Water Pool" << endl;
		}
	}
	// TODO: Remove
	// Initialize a Regular and Hardcore Protester 
	try {
		for (int i = 0; i < 50; i++) {
			m_actors.push_back(make_shared<RegularProtester>(this, rand() % ICE_WIDTH, ICE_HEIGHT));
			m_actors.push_back(make_shared<HardcoreProtester>(this, rand() % ICE_WIDTH, ICE_HEIGHT));
		}
	}
	catch (bad_alloc& /*ex*/) {
		cout << "Unable to allocate memory for Regular Protester" << endl;
	}

	return GWSTATUS_CONTINUE_GAME;
}

// Handle movement for all game objects within our world
int StudentWorld::move()
{
	// Compute the distance between all Actors
	computeDistances();

	// This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
	// Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
	//decLives();

	// Handle the next event from the min heap
	processNextEvent();



	// Give ALL Actors a chance to do something during this tick
	for (auto actor : m_actors) {
		// TODO: Do we call doSomething() if it's not alive?
		if (actor != nullptr) {
			// Check if IceMan can pick up the object
			// If yes, then this is a Goodie
			if (actor->canPickupIM()) {
				actor->doSomething();
				pickupGoodieIM(actor, *(m_pIceMan.lock()));
			}
			// Else, this is not a Goodie
			else {
				actor->doSomething();
			}
		}
	}

	{
		shared_ptr<IceMan> pIceManShared = m_pIceMan.lock();

		const int x = pIceManShared->getX();
		const int y = pIceManShared->getY();

		// Is IceMan standing on ice?
		if (x < ICE_WIDTH && y < ICE_HEIGHT) {

			// Dig through the 4x4 matrix of ice that we're standing on 
			for (int yOffset = 0; yOffset < ICEMAN_TO_ICE_SIZE_RATIO; yOffset++) {
				int finalY = y + yOffset;

				for (int xOffset = 0; xOffset < ICEMAN_TO_ICE_SIZE_RATIO && finalY < ICE_HEIGHT; xOffset++) {
					int finalX = x + xOffset;
					// If ice is present, kill it
					if (finalX < ICE_WIDTH && m_ice[finalX][finalY]) {
						m_ice[finalX][finalY]->setAlive(false);
					}
				}
			}

		}

	}

	// Give the ice a chance to do something during this tick
	// TODO: Does ice need to do anything?
	for (int x = 0; x < ICE_WIDTH; x++) {
		for (int y = 0; y < ICE_HEIGHT; y++) {
			if (m_ice[x][y] != nullptr) {
				m_ice[x][y]->doSomething();
			}
		}
	}

	// Increment time. Keep this at the end of this method.
	m_nTick++;
	
	return GWSTATUS_CONTINUE_GAME;
}

// Cleanup game objects (deallocates memory)
void StudentWorld::cleanUp()
{
	// Release memory for all Actors
	for (auto actor : m_actors) {
		actor.reset();
	}

	// Release memory for all Ice blocks
	for (int x = 0; x < ICE_WIDTH; x++) {
		for (int y = 0; y < ICE_HEIGHT; y++) {
			if (m_ice[x][y] != nullptr) {
				m_ice[x][y].reset();
			}
		}
	}
}

void StudentWorld::removeDeadGameObjects() {
	// TODO
}

// Creates random x coordinate for actors to spawn in
// Will not spawn in tunnel
int StudentWorld::getRandomX() {
	int x = rand() % 60;
	while (x >= 27 && x <= 33)
		x = rand() % 60;
	return x;
}

// Creates random y coordinate for actors to spawn in
int StudentWorld::getRandomY() {
	return rand() % 56;
}


// GOODIE FUNCTIONS

// Sets the number of oil barrels in the oil field
void StudentWorld::setNumBarrels(int barrels) {
	m_num_barrels = barrels;
}

// Returns the number of oil barrels remaining in the field
int StudentWorld::getNumBarrels() {
	return m_num_barrels;
}

// Decrements the number of oil barrels remaining in the field
void StudentWorld::decNumBarrels() {
	--m_num_barrels;
}

// Handles the case where any Goodie is picked up
void StudentWorld::pickupGoodieIM(ActorPtr actor, IceMan& iceman) {
	// If there is a collision (distance <= 3), increase the score and play a sound
	// Depending on the goodie, a different action occurs
	// Uses the image ID to identify the goodie
	if (getDistanceToIceMan(actor->getX(), actor->getY()) <= 3) {

		switch (actor->getID()) {
		case IID_BARREL:
			decNumBarrels();
			if (getNumBarrels() == 0) {
				// FIXME level increases when all barrels are collected
				// TODO: Where should the level be incremented?
				cout << "LEVEL UP" << endl; // FIXME
			}
			break;
		case IID_GOLD:
			iceman.incGold();
			break;
		case IID_SONAR:
			iceman.incSonarKits();
			break;
		case IID_WATER_POOL:
			iceman.incWater();
			break;
		}
		increaseScore(actor->getPoints());
		playSound(actor->getSoundEffect());
	}
}

// Process the next Event
void StudentWorld::processNextEvent() {
	// Iterate through all of the events for this tick
	while (!m_events.empty() && m_nTick >= m_events.top()->getTick()) {
		SharedEventPtr e = m_events.top();

		// Skip any missed events but print an error message
		if (m_nTick > e->getTick()) {
			cout << "Uh oh! StudentWorld::processNextEvent() is late to process this event: " << *e;
		}

		try {
			auto iterBegin	= m_eventListeners.lower_bound(e->getType());
			auto iterEnd	= m_eventListeners.upper_bound(e->getType());

			// Iterate through all pairs where the key matches our Event type
			for (auto it = iterBegin; it != iterEnd; it++) {
				EventCallback& callback = it->second;

				callback(e);
			}
		}
		catch (exception& ex) {
			cout << "An exception occured within a callback associated with an Event of type: " << *e << endl;
			cout << ex.what() << endl;
		}

		m_events.pop();
	}
}
 
// Register for an Event
void StudentWorld::listenForEvent(EventTypes type, EventCallback callback) {
	m_eventListeners.insert({ type, callback });
}

// Compute distance to IceMan
int StudentWorld::getDistanceToIceMan(int x, int y) const {
	shared_ptr<IceMan> pIceMan = m_pIceMan.lock();

	// TODO: Should use m_distances
	return m_distanceCalc.getDistance(x, y, pIceMan->getX(), pIceMan->getY());
}

// Check if these coordinates and direction are facing IceMan
bool StudentWorld::isFacingIceMan(int x, int y, int direction) const {
	shared_ptr<IceMan> pIceMan = m_pIceMan.lock();

	bool isFacing = false;

	switch (direction) {
	case GraphObject::Direction::up:
		isFacing = pIceMan->getY() > y;
		break;
	case GraphObject::Direction::down:
		isFacing = pIceMan->getY() < y;
		break;
	case GraphObject::Direction::left:
		isFacing = pIceMan->getX() < x;
		break;
	case GraphObject::Direction::right:
		isFacing = pIceMan->getX() > x;
		break;
	case GraphObject::Direction::none:
	default:
		break;
	}

	return isFacing; 
}

// Compute distances betwen actors
void StudentWorld::computeDistances() {
	m_distances.clear();
	
	// Compute distances between each Actor and every other Actor O(n^2)
	for (auto it1 = cbegin(m_actors); it1 != cend(m_actors); it1++) {
		for (auto it2 = cbegin(m_actors); it2 <= it1; it2++) {
			// If we're looking at the distance from ourself, it's always zero
			if (it1 == it2) {
				m_distances[(*it1)][(*it2)] = 0;
				continue;
			}

			m_distances[(*it1)][(*it2)] = m_distanceCalc.getDistance(
				(*it1)->getX(), (*it1)->getY(),
				(*it2)->getX(), (*it2)->getY());
			// Also add the transpose for distance from actor a to b, since it's
			// the same from b to a
			m_distances[(*it2)][(*it1)] = m_distances[(*it1)][(*it2)];
		} 
	}

#if TEST_STUDENTWORLD
	if (getTick() % 10) {
		for (auto it1 = cbegin(m_actors); it1 != cend(m_actors); it1++) {
			for (auto it2 = cbegin(m_actors); it2 != cend(m_actors); it2++) {
				int calcDistance = m_distanceCalc.getDistance(
					(*it1)->getX(), (*it1)->getY(),
					(*it2)->getX(), (*it2)->getY());

				auto it3 = m_distances.find(*it1);
				if (it3 != end(m_distances)) {
					auto it4 = it3->second.find(*it2);
					if (it4 != end(it3->second)) {
						int distance = (*it4).second;

						assert(calcDistance == distance);
					}
				}

			}
		}
	}
#endif // TEST_STUDENTWORLD
}