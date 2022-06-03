#include <string>
#include <typeinfo>
#include <exception>
#include <assert.h>
#include <algorithm>
#include <thread>

#include "StudentWorld.h"
#include "IceMan.h"
#include "Actor.h"
#include "Ice.h"
#include "OilBarrel.h"
#include "Gold.h"
#include "SonarKit.h"
#include "WaterPool.h"
#include "Squirt.h"
#include "Boulder.h"
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
	m_distanceCalc(),
	m_shortestPathToExit(this),
	m_shortestPathToIceMan(this),
	m_pWorkerThreads{}
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

	// Initialize Boulders
	int NUM_BOULDERS = 20; // TODO: how many should spawn at the start?
	for (int i = 0; i < NUM_BOULDERS; ++i) {
		
		int y = getRandomY();
		if (y < 4)
			y += 4;
		int x = getCoordinatesWithIce(getRandomX(), y);
		// Delete ice where the boulder spawns
		for (int xOffset = x; xOffset < x + 4; ++xOffset) {
			for (int yOffset = y; yOffset < y + 4; ++yOffset) {
				if (m_ice[xOffset][yOffset])
					m_ice[xOffset][yOffset].reset();
			}
		}
		try {
			m_actors.push_back(make_shared<Boulder>(this, x, y));
		}
		catch (bad_alloc&) {
			cout << "Unable to allocate memory for Boulder" << endl;
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
	int NUM_OIL_BARRELS = 5; // TODO: how many should spawn at the start?
	setNumBarrels(NUM_OIL_BARRELS);
	for (int i = 0; i < NUM_OIL_BARRELS; ++i) {
		try {
			int y = getRandomY();
			int x = getCoordinatesWithIce(getRandomX(), y);
			m_actors.push_back(make_shared<OilBarrel>(this, x, y));
		}
		catch (bad_alloc&) {
			cout << "Unable to allocate memory for Oil Barrel" << endl;
		}
	}

	// Initialize Gold Nuggets
	shared_ptr<Gold> pGold;
	int NUM_GOLD_NUGGETS = 10; // TODO: how many should spawn at the start?
	for (int i = 0; i < NUM_GOLD_NUGGETS; ++i) {
		try {
			int y = getRandomY();
			int x = getCoordinatesWithIce(getRandomX(), y);
			m_actors.push_back(make_shared<Gold>(this, x, y, false, true, false, true));
		}
		catch (bad_alloc&) {
			cout << "Unable to allocate memory for Gold Nugget" << endl;
		}
	}
	
	// Initialize Sonar Kits
	int NUM_SONAR_KITS = 1; // TODO: how many should spawn at the start?
	for (int i = 0; i < NUM_SONAR_KITS; ++i) {
		try {
			int y = getRandomY();
			int x = getCoordinatesWithIce(getRandomX(), y);
			m_actors.push_back(make_shared<SonarKit>(this, x, y, true));
		}
		catch (bad_alloc&) {
			cout << "Unable to allocate memory for Sonar Kit" << endl;
		}
	}

	// TODO: Remove
	// Initialize a Regular and Hardcore Protester 
	try {
		for (int i = 0; i < 1; i++) {
			m_actors.push_back(make_shared<RegularProtester>(this, rand() % ICE_WIDTH, ICE_HEIGHT));
			m_actors.push_back(make_shared<HardcoreProtester>(this, rand() % ICE_WIDTH, ICE_HEIGHT));
		}
	}
	catch (bad_alloc& /*ex*/) {
		cout << "Unable to allocate memory for Regular Protester" << endl;
	}
 
	// TODO: make shared data thread safe
	//startWorkerThreads();

	return GWSTATUS_CONTINUE_GAME;
}

// Start worker threads to perform expensive calculations
void StudentWorld::startWorkerThreads()
{
	// Create worker to computer distances between actors
	m_pWorkerThreads.push_back(make_shared<thread>([&]() {
		while (true) {
			// TODO: must make thread-safe
			computeDistancesBetweenActors();
		}
	}));

	// Create worker to map shortest path to exit 
	m_pWorkerThreads.push_back(make_shared<thread>([&]() {
		while (true) {
			// TODO: must make thread-safe
			m_shortestPathToExit.compute(60, 60);
		}
	}));

	// Create worker to map shortest path to IceMan
	m_pWorkerThreads.push_back(make_shared<thread>([&]() {
		while (true) {
			auto pIceMan = m_pIceMan.lock();
			// TODO: must make thread-safe
			m_shortestPathToIceMan.compute(pIceMan->getX(), pIceMan->getY());
		}
	}));
}

// Handle movement for all game objects within our world
int StudentWorld::move()
{
	for (auto newActor : m_newActors) {
		m_actors.push_back(newActor);
	}
	m_newActors.clear();

	// Compute the distance between all Actors
	computeDistancesBetweenActors();

	m_shortestPathToExit.compute(60, 60);

	{
		auto pIceMan = m_pIceMan.lock();
		m_shortestPathToIceMan.compute(pIceMan->getX(), pIceMan->getY());
	}

	setGameStatText(getGameStatText());
	
	// REMOVE: for testing adding water pools. None will spawn without this
	if (getTick() % 50 == 0)
		addWaterPool();

	// This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
	// Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
	//decLives();


	// Handle the next event from the min heap
	processNextEvent();

	// Give ALL Actors a chance to do something during this tick
	for_each(begin(m_actors), end(m_actors), [](ActorPtr& actor) { 
		if (actor) 
			actor->doSomething();  
	});

	// Give the ice a chance to do something during this tick
	// TODO: check that the Ice is properly destructed
	// TODO: play sound effect when Ice is dug away
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


// Dig up the ice at this location
void StudentWorld::digUpIce(int x, int y)
{
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

// Cleanup game objects (deallocates memory)
void StudentWorld::cleanUp()
{
	// Release memory for all Actors
	m_actors.clear();
	m_distances.clear();

	// Release memory for all Ice blocks
	for (int x = 0; x < ICE_WIDTH; x++) {
		for (int y = 0; y < ICE_HEIGHT; y++) {
			if (m_ice[x][y] != nullptr) {
				m_ice[x][y].reset();
			}
		}
	}

	m_distances.clear();

	// Terminate the worker threads and reclaim their resources
	for_each(begin(m_pWorkerThreads), end(m_pWorkerThreads), [](ThreadPtr pThread) {
			pThread->join();
		});
	m_pWorkerThreads.resize(0);
}

void StudentWorld::removeDeadGameObjects() {
	// TODO: test
	for (auto actor : m_actors) {
		if (actor == nullptr)
			continue;
		if (actor == nullptr || !actor->isAlive()) {
			actor.reset();
		}
	}
	remove_if(begin(m_actors), end(m_actors), [](ActorPtr pActor) {
		return pActor == nullptr || !pActor->isAlive();
		});
	m_distances.clear();
}

string StudentWorld::getGameStatText() {
	// TODO: fix functions that should be called here
	return
		"Lvl: " + to_string(getLevel()) +
		" Lives: " + to_string(getLives()) +
		" Hlth: " + to_string(0) + // TODO - should be IceMan Hit Points
		" Wtr: " + //to_string(m_pIceMan.lock()->getWater()) +
		" Gld : " + //to_string(m_pIceMan.lock()->getGold()) +
		" Oil Left: " + to_string(getNumBarrels()) +
		" Sonar: " + //to_string(m_pIceMan.lock()->getSonarKits()) +
		" Scr: " + to_string(getScore());
}

// Creates random x coordinate for actors to spawn in
// Will not spawn in tunnel
int StudentWorld::getRandomX() {
	return rand() % (ICE_WIDTH - 4);
}

// Creates random y coordinate for actors to spawn in
int StudentWorld::getRandomY() {
	return rand() % (ICE_HEIGHT - 4);
}

// Regenerates the x value until the object is buried in ice
int StudentWorld::getCoordinatesWithIce(int x, int y) {
	bool overlapsIce = false;
	while (overlapsIce == false) {
		overlapsIce = true;
		for (int xOffset = x; xOffset < x + 4; ++xOffset) {
			for (int yOffset = y; yOffset < y + 4; ++yOffset) {
				if (!m_ice[xOffset][yOffset]) {
					overlapsIce = false;
				}
			}
		}
		if (!overlapsIce)
			x = getRandomX();
	}
	return x;
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
void StudentWorld::pickupGoodieIM(int ID, int points, int SE) {
	// If there is a collision (distance <= 3), increase the score and play a sound
	// Depending on the goodie, a different action occurs
	// Uses the image ID to identify the goodie
	
	// TODO: fix functions called here

	switch (ID) {
	case IID_BARREL:
		decNumBarrels();
		break;
	case IID_GOLD:
		// m_pIceMan.lock()->incGold();
		break;
	case IID_SONAR:
		// m_pIceMan.lock()->incSonarKits();
		break;
	case IID_WATER_POOL:
		// m_pIceMan.lock()->incWater();
		break;
	}
	increaseScore(points);
	playSound(SE);
}

// Handles when a Protester picks up Gold
void StudentWorld::pickupGoldP(ActorPtr gold) {
	// TODO: implement this
}
// Handles when a Squirt is fired by IceMan
void StudentWorld::fireSquirt(int x, int y, GraphObject::Direction dir) {
	int xOffset = 0;
	int yOffset = 0;

	switch (dir) {
	case GraphObject::Direction::left:
		x+= -4;
		break;
	case GraphObject::Direction::right:
		x+= 4;
		break;
	case GraphObject::Direction::up:
		y+= 4;
		break;
	case GraphObject::Direction::down:
		y+= -4;
		break;
	}
	playSound(SOUND_PLAYER_SQUIRT);

	// Dont' make a squirt if it goes outside the edge of the screen or ice
	for (int xOffset = x; xOffset < x + 4; ++xOffset) {
		for (int yOffset = y; yOffset < y + 4; ++yOffset) {
			if (x >= ICE_WIDTH || x < 0 || y >= VIEW_HEIGHT || y < 0 || (y < ICE_HEIGHT && m_ice[xOffset][yOffset]))
				return;
		}
	}
	m_newActors.emplace_back(make_shared<Squirt>(this, x, y, dir));

}

// Handles when a Squirt hits Ice, Boulder, or Protester
// TODO: finish Protester case (annoy by 2 points)
bool StudentWorld::hitBySquirt(int x, int y) {

	// Did the squirt hit ice or the edge of the screen?
	for (int xOffset = x; xOffset < x + 4; ++xOffset) {
		for (int yOffset = y; yOffset < y + 4; ++yOffset) {
			if (x >= ICE_WIDTH || x < 0 || y >= VIEW_HEIGHT || y < 0 || (y < ICE_HEIGHT && m_ice[xOffset][yOffset]))
				return true;
		}
	}

	// Did the squirt hit a protester or boulder?
	bool hitProtester = false;
	for (auto actor : m_actors) {
		if (actor == nullptr)
			continue;
		if (m_distanceCalc.getDistance(actor->getX(), actor->getY(), x, y) <= 3) {
			if (actor->getID() == IID_PROTESTER || actor->getID() == IID_HARD_CORE_PROTESTER) {
				// TODO: should annoy them by 2 points
				hitProtester = true; // Allows for multiple protesters to be hit by the same squirt
			}
			else if (actor->getID() == IID_BOULDER) {
				return true;
			}
		}
	}
	if (hitProtester)
		return true;
	// The squirt did not hit any other actors
	return false;
}

// Handles when IceMan uses a SonarKit
void StudentWorld::useSonarKit() {
	playSound(SOUND_SONAR);
	for (auto actor : m_actors) {
		if (actor->getDistanceToIceman() <= 12 && actor->canPickupIM()) {
			actor->setVisible(true);
		}
	}
}

// Handles when IceMan drops a Gold Nugget
void StudentWorld::dropGold() {
	try {
		m_newActors.push_back(make_shared<Gold>(
			this,
			m_pIceMan.lock()->getX(),
			m_pIceMan.lock()->getY(),
			true, false, true, false));
	}
	catch (bad_alloc&) {
		cout << "Unable to allocate memory for Gold Nugget" << endl;
	}
}

// Initializes and places new WaterPools
void StudentWorld::addWaterPool() {
	int x = getRandomX();
	int y = getRandomY();
	bool overlaps = true;
	while (overlaps == true) {
		x = getRandomX();
		y = getRandomY();
		overlaps = false;
		for (int xOffset = x; xOffset < x + 4; ++xOffset) {
			for (int yOffset = y; yOffset < y + 4; ++yOffset) {
				if (m_ice[xOffset][yOffset]) {
					overlaps = true;
				}
			}
		}
		for (auto actor : m_actors) {
			if (actor == nullptr)
				continue;
			if (m_distanceCalc.getDistance(x, y, actor->getX(), actor->getY()) <= 3)
				overlaps = true;
		}
	}
	try {
		m_newActors.emplace_back(make_shared<WaterPool>(this, x, y));
	}
	catch (bad_alloc&) {
		cout << "Unable to allocate memory for Water Pool" << endl;
	}
}

// Checks if there is ice under a Boulder,
// or if the Boulder is at the bottom of the ice field
bool StudentWorld::isGroundUnderBoulder(int x, int y) {
	// Check gound 1 block below the Boulder
	y -= 1;
	if (y < 0)
		return true;
	for (int xOffset = x; xOffset < x + 4; ++xOffset) {
		if (m_ice[xOffset][y]) {
			return true;
		}
	}
	return false;
}

// Handles if a boulder hits IceMan, Protester, or another Boulder
// Pass in coordinates of the Boulder
// TODO: annoy Protesters by 100 points
bool StudentWorld::hitByBoulder(int x, int y) {

	// Did the boulder hit IceMan, a Protester or another Boulder?
	for (auto actor : m_actors) {
		if (actor == nullptr)
			continue;
		if ((actor->getX() >= x) && (actor->getX() <= x + 4) && (actor->getY() + 4 == y - 1)) {
			int actorID = actor->getID();
			if (actorID == IID_PROTESTER || actorID == IID_HARD_CORE_PROTESTER) {
				// TODO: should annoy them by 100 points
				increaseScore(500);
			}
			if (actorID == IID_PLAYER) {
				// TODO: should annoy player by 100 points
			}
			// Stop the boulder if it hits another Boulder
			else if (actor->getID() == IID_BOULDER) {
				return true;
			}
		}
	}
	// Boulder did not hit anything that stops it
	return false;
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
	if (x < 0 || x >= VIEW_WIDTH || y < 0 || y > ICE_HEIGHT)
		return true;

	// TODO: Check for Boulders
	return y < ICE_HEIGHT && m_ice[x][y] != nullptr && m_ice[x][y]->isAlive();
}

// Compute distances between all actors
void StudentWorld::computeDistancesBetweenActors() {
	// TODO: make thread-safe
	m_distances.clear();
	
	// Compute distances between each Actor and every other Actor O(n^2)
	for (auto it1 = cbegin(m_actors); it1 != cend(m_actors); it1++) {
		if ((*it1) == nullptr)
			continue;
		for (auto it2 = cbegin(m_actors); it2 <= it1; it2++) {
			if ((*it2) == nullptr)
				continue;
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