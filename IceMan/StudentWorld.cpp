#include <string>
#include <exception>
#include <assert.h>

#include "StudentWorld.h"
#include "IceMan.h"
#include "Actor.h"
#include "Ice.h"
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

	// TODO: Remove
	// Initialize a Regular and Hardcore Protester 
	try {
		for (int i = 0; i < 1; i++) {
			m_actors.push_back(make_shared<RegularProtester>(this, rand() % ICE_WIDTH, ICE_HEIGHT));
			//m_actors.push_back(make_shared<HardcoreProtester>(this, rand() % ICE_WIDTH, ICE_HEIGHT));
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
	computeDistancesBetweenActors();

	// This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
	// Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
	//decLives();

	// Handle the next event from the min heap
	processNextEvent();

	// Give ALL Actors a chance to do something during this tick
	for (auto actor : m_actors) {
		if (actor != nullptr) {
			actor->doSomething();
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

	return m_distanceCalc.getDistance(x, y, pIceMan->getX(), pIceMan->getY());
}

// Check if these coordinates and direction are facing IceMan
bool StudentWorld::isFacingIceMan(int x, int y, GraphObject::Direction direction) const {
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

// Do we have a direct line of sight with IceMan, i.e. we're on 
// the same horizontal or vertical axis and no ice or boulders are
// between us. Return true, if so and set direction (by reference)
// to face IceMan. Otherwise, false.
bool StudentWorld::hasPathToIceMan(int x, int y, GraphObject::Direction& direction) const {
	bool hasLineOfSight = false;

	shared_ptr<IceMan> pIceMan = m_pIceMan.lock();
	
	// Check for ice or boulders in the way on the vertical and
	// horizontal axis
	const int iceX = pIceMan->getX();
	const int iceY = pIceMan->getY();	
		 
	if (x == iceX) {
		// Are we below IceMan's?
		if (y < iceY) {
			for (int j = y; j < iceY && !hasLineOfSight; j++) {
				if (isBlocked(x, j)) 
					hasLineOfSight = false;
			}
			if (hasLineOfSight)
				direction = GraphObject::Direction::up;
		}
		else if (y > iceY) { // Are we above IceMan?
			for (int j = y; j > iceY && !hasLineOfSight; j--) {
				if (isBlocked(x, j))
					hasLineOfSight = false;
			}
			if (hasLineOfSight)
				direction = GraphObject::Direction::down;
		}
		else {
			direction = GraphObject::Direction::none;
		}

		hasLineOfSight = true;
	}
	else if (y == iceY) {
		// Check for ice or boulders in the way on the vertical axis
		// Are we on IceMan's left?
		if (x < iceX) {
			for (int i = x; i < iceX && !hasLineOfSight; i++) {
				if (isBlocked(i, y))
					hasLineOfSight = false;
			}
			if (hasLineOfSight)
				direction = GraphObject::Direction::left;
		}
		else if (x > iceX) { // Are we on IceMan's right?
			for (int i = x; i > iceX && !hasLineOfSight; i--) {
				if (isBlocked(i, y))
					hasLineOfSight = false;
			}
			if (hasLineOfSight)
				direction = GraphObject::Direction::right;
		}
		else {
			direction = GraphObject::Direction::none;
		}

		hasLineOfSight = true;
	}

	return hasLineOfSight;
}

// Is this location occupied by Ice, Boulder, or is out of bounds
bool StudentWorld::isBlocked(int x, int y) const {
	// Check boundries
	if (x < 0 || x >= VIEW_WIDTH || y < 0 || y >= ICE_HEIGHT)
		return false;

	// TODO: Check for Boulders
	return m_ice[x][y] != nullptr && m_ice[x][y]->isAlive();
}

// Compute distances between all actors
void StudentWorld::computeDistancesBetweenActors() {
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