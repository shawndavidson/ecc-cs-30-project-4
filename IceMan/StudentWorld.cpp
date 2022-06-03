#include <string>
#include <exception>
#include <assert.h>
#include <thread>

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
			m_actors.push_back(make_shared<RegularProtester>(this, rand() % (ICE_WIDTH - PERSON_SIZE), ICE_HEIGHT));
			m_actors.push_back(make_shared<HardcoreProtester>(this, rand() % (ICE_WIDTH - PERSON_SIZE), ICE_HEIGHT));
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
	computeDistancesBetweenActors();

	m_shortestPathToExit.compute(60, 60);

	{
		auto pIceMan = m_pIceMan.lock();
		m_shortestPathToIceMan.compute(pIceMan->getX(), pIceMan->getY());
	}

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

	playSound(SOUND_DIG);
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

	m_distances.clear();

	// Terminate the worker threads and reclaim their resources
	for_each(begin(m_pWorkerThreads), end(m_pWorkerThreads), [](ThreadPtr pThread) {
			pThread->join();
		});
	m_pWorkerThreads.resize(0);
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

// Compute distance to IceMan. Returns the units to reach IceMan through the maze, avoiding
// ice and boulder. If there isn't a path, it returns UINT_MAX.
unsigned int StudentWorld::getDistanceToIceMan(int x, int y) const {
	shared_ptr<IceMan> pIceMan = m_pIceMan.lock();

	DirectionDistance result;

	return m_shortestPathToIceMan.getShortestPath(x, y, result) ? result.distance : UINT_MAX;
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
// between us.
// Must pass in the current direction.  
// Returns true and changes direction (by reference), if needed towards IceMan
// to face IceMan. Otherwise, false.
bool StudentWorld::hasLineOfSightToIceMan(int x, int y, GraphObject::Direction& direction) const {
	bool hasLineOfSight = true;

	shared_ptr<IceMan> pIceMan = m_pIceMan.lock();
	
	// Check for ice or boulders in the way on the vertical and
	// horizontal axis
	const int iceX = pIceMan->getX();
	const int iceY = pIceMan->getY();	
		 
	if (x == iceX) {
		// Are we below IceMan's?
		if (y < iceY) {
			for (int j = y; j < iceY && hasLineOfSight; j++) {
				hasLineOfSight = !isBlocked(x, j);
			}
			// If we have a line of sight, then face IceMan
			if (hasLineOfSight)
				direction = GraphObject::Direction::up;
		}
		else if (y > iceY) { // Are we above IceMan?
			for (int j = y; j > iceY && hasLineOfSight; j--) {
				hasLineOfSight = !isBlocked(x, j);
			}
			// If we have a line of sight, then face IceMan
			if (hasLineOfSight)
				direction = GraphObject::Direction::down;
		}
		else {
			// We're in the same spot as IceMan so just face the default direction
			direction = GraphObject::Direction::left;
		}
	}
	else if (y == iceY) {
		// Check for ice or boulders in the way on the vertical axis
		// Are we on IceMan's left?
		if (x < iceX) {
			for (int i = x; i < iceX && hasLineOfSight; i++) {
				hasLineOfSight = !isBlocked(i, y);
			}
			// If we have a line of sight, then face IceMan
			if (hasLineOfSight)
				direction = GraphObject::Direction::right;
		}
		else if (x > iceX) { // Are we on IceMan's right?
			for (int i = x; i > iceX && hasLineOfSight; i--) {
				hasLineOfSight = !isBlocked(i, y);
			}
			// If we have a line of sight, then face IceMan
			if (hasLineOfSight)
				direction = GraphObject::Direction::left;
		}
		else {
			// We're in the same spot as IceMan so just face the default direction
			direction = GraphObject::Direction::left;
		}
	}

	return hasLineOfSight;
}

// Is this location occupied by Ice, Boulder, or is out of bounds
bool StudentWorld::isBlocked(int x, int y) const {
	// Check boundries
	if (x < 0 || x > (VIEW_WIDTH - PERSON_SIZE) || y < 0 || y > ICE_HEIGHT)
		return true;

	// TODO: Check for Boulders

	// If we're on top of the ice, no need to check for ice
	if (y == ICE_HEIGHT) {
		return false;
	}

	//const int xStop = std::max<int>(x + PERSON_SIZE, ICE_WIDTH);
	//const int yStop = std::max<int>(y + PERSON_SIZE, ICE_HEIGHT);

	//for (int i = x; i < xStop; i++) {
	//	for (int j = y; j < yStop; j--) {
	//		if (m_ice[i][j] != nullptr && m_ice[i][j]->isAlive())
	//			return true;
	//	}
	//}

	return m_ice[x][y] != nullptr && m_ice[x][y]->isAlive();
}

// Compute distances between all actors
void StudentWorld::computeDistancesBetweenActors() {
	// TODO: make thread-safe
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