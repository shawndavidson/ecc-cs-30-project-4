#include <string>
#include <typeinfo>
#include <exception>
#include <assert.h>
#include <algorithm>
#include <thread>
#include <sstream>
#include <iomanip>

#include "StudentWorld.h"
#include "IceMan.h"
#include "Actor.h"
#include "Person.h"
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
	const int level = getLevel();

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
	int NUM_BOULDERS = min(level / 2 + 2, 9);
	for (int i = 0; i < NUM_BOULDERS; ++i) {

		pair<int, int> coordinates = getRandCoordinates(20);
		int x = coordinates.first;
		int y = coordinates.second;

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
	int NUM_OIL_BARRELS = min(2 + level, 21);
	setNumBarrels(NUM_OIL_BARRELS);
	for (int i = 0; i < NUM_OIL_BARRELS; ++i) {
		try {
			pair<int, int> coordinates = getRandCoordinates(0);
			int x = coordinates.first;
			int y = coordinates.second;
			m_actors.push_back(make_shared<OilBarrel>(this, x, y));
		}
		catch (bad_alloc&) {
			cout << "Unable to allocate memory for Oil Barrel" << endl;
		}
	}

	// Initialize Gold Nuggets
	shared_ptr<Gold> pGold;
	int NUM_GOLD_NUGGETS = max(5 - level / 2, 2);
	for (int i = 0; i < NUM_GOLD_NUGGETS; ++i) {
		try {
			pair<int, int> coordinates = getRandCoordinates(0);
			int x = coordinates.first;
			int y = coordinates.second;
			m_actors.push_back(make_shared<Gold>(this, x, y, false, true, false, true));
		}
		catch (bad_alloc&) {
			cout << "Unable to allocate memory for Gold Nugget" << endl;
		}
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
	addNewActors();

	// Compute the distance between all Actors
	computeDistancesBetweenActors();

	m_shortestPathToExit.compute(60, 60);

	{
		auto pIceMan = m_pIceMan.lock();
		m_shortestPathToIceMan.compute(pIceMan->getX(), pIceMan->getY());
	}

	setGameStatText(getGameStatText());


	// Handle the next event from the min heap
	processNextEvent();

	// Give ALL Actors a chance to do something during this tick
	for_each(begin(m_actors), end(m_actors), [](ActorPtr& actor) { 
		if (actor) 
			actor->doSomething();  
	});
	
	// Go to next level if all Oil Barrels Collected
	if (getNumBarrels() == 0) {
		playSound(SOUND_FINISHED_LEVEL);
		return GWSTATUS_FINISHED_LEVEL;
	}
	// Restart the level if IceMan dies
	if (!m_pIceMan.lock()->isAlive()) {
		playSound(SOUND_PLAYER_GIVE_UP);
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}
	
	removeDeadGameObjects();

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
			if (finalY >= ICE_HEIGHT)
				continue;

			for (int xOffset = 0; xOffset < ICEMAN_TO_ICE_SIZE_RATIO; xOffset++) {
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
	m_actors.clear();
	m_distances.clear();

	m_nNumProtesters = 0;

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

	auto endIter = remove_if(begin(m_actors), end(m_actors), [](ActorPtr pActor) {
		if (pActor == nullptr || !pActor->isAlive())
		{
			pActor.reset();
			return true;
		}
		return false;
	});

	// Clear distances, this will be regenerated on the next tick
	m_distances.clear();

	// Clear dead ice
	for (int x = 0; x < ICE_WIDTH; x++) {
		for (int y = 0; y < ICE_HEIGHT; y++) {
			if (m_ice[x][y] && !m_ice[x][y]->isAlive())
				m_ice[x][y].reset();
		}
	}
}

string StudentWorld::getGameStatText() {
	stringstream ss;
	ss << setfill(' ') << left;
	ss <<
		"Lvl: " << setw(2) << to_string(getLevel()) <<
		"  Lives: " << setw(1) << to_string(getLives()) <<
		"  Hlth: " << setw(3) << to_string(100 * m_pIceMan.lock()->getHitPoints() / 10) + "%" <<
		"  Wtr: " << setw(2) << to_string(m_pIceMan.lock()->getWater()) <<
		"  Gld: " << to_string(m_pIceMan.lock()->getGold()) <<
		"  Oil Left: " << to_string(getNumBarrels()) <<
		"  Sonar: " << to_string(m_pIceMan.lock()->getSonarKits()) <<
		"  Scr: " << setfill('0') << right << setw(6) << to_string(getScore());
	return ss.str();
}

// Creates random x coordinate for actors to spawn in
int StudentWorld::getRandomX() {
	return rand() % (ICE_WIDTH - 4);
}

// Creates random y coordinate for actors to spawn in
int StudentWorld::getRandomY(int minHeight) {
	return minHeight + rand() % (ICE_HEIGHT - 4 - minHeight);
}

// Not within 6 of other actors and not in tunnel
pair<int, int> StudentWorld::getRandCoordinates(int minHeight) {
	int x = getRandomX();
	if (x >= 26 && x < 35)
		x += 10;
	int y = getRandomY(minHeight);

	for (auto actor : m_actors) {
		if (actor == nullptr)
			continue;
		while (m_distanceCalc.getDistance(x, y, actor->getX(), actor->getY()) <= 6) {
			y = getRandomY(minHeight);
			x = getRandomX();
			if (x >= 26 && x < 35)
				x += 10;
		}
	}
	return pair<int, int>(x, y);
}

/*************************************************************************/
/* Functions that add actors										     */
/*************************************************************************/

// Adds new actors during each tick
void StudentWorld::addNewActors() {
	// All values are specified on page 20 and 21
	const unsigned int level = getLevel();
	// Random chance if a new goodie should be added
	bool addNewGoodie = (0 == rand() % (level * 25 + 300));
	if (addNewGoodie) {
		// 1/5 chance it is a SonarKit
		if (0 == rand() % 5)
			addSonarKit();
		// 4/5 chance it is a WaterPool
		else
			addWaterPool();
	}
	
	// Only add a Protester if there are fewer than the max already in the field
	// And enough ticks have passed since the last Protester was added
	bool addNewProtester = getTick() - m_nTickLastProtesterAdded >= std::max<unsigned int>(25, 200 - level);
	if (addNewProtester) {
		if (m_nNumProtesters < std::min<unsigned int>(15, int(2 + level * 1.5))) {
			// Random if it is Hardcore or Regular, gives probability as percentage [30...90]
			unsigned int probabilityOfHardcore = std::min<unsigned int>(90, level * 10 + 30);

			// TODO: remove
			probabilityOfHardcore = 100;

			// Keep in mind, rand() doesn't give a uniform distribution but it's good enough.
			// https://stackoverflow.com/questions/12885356/random-numbers-with-different-probabilities
			int ID = (rand() % (100+1) < (int)probabilityOfHardcore) ? IID_HARD_CORE_PROTESTER : IID_PROTESTER;
			addProtester(ID);
		}
	}

	for (auto newActor : m_newActors) {
		m_actors.push_back(newActor);
	}
	m_newActors.clear();
}

// Adds new Protesters, either Regular or Hardcore
void StudentWorld::addProtester(int ID) {
	
	try {
		if (ID == IID_HARD_CORE_PROTESTER)
			m_newActors.emplace_back(make_shared<HardcoreProtester>(this, 60, 60));
		else
			m_newActors.emplace_back(make_shared<RegularProtester>(this, 60, 60));
	}
	catch (bad_alloc& /*ex*/) {
		cout << "Unable to allocate memory for Protester" << endl;
	}

	++m_nNumProtesters;
	m_nTickLastProtesterAdded = getTick();
}

// Initializes and places new WaterPool
void StudentWorld::addWaterPool() {
	int x = getRandomX();
	int y = getRandomY(0);
	bool overlaps = true;
	while (overlaps == true) {
		x = getRandomX();
		y = getRandomY(0);
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
			if (m_distanceCalc.getDistance(x, y, actor->getX(), actor->getY()) <= 6)
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

// Initializes and places new SonarKits
void StudentWorld::addSonarKit() {
	try {
		m_newActors.emplace_back(make_shared<SonarKit>(this, 0, 60, true));
	}
	catch (bad_alloc&) {
		cout << "Unable to allocate memory for Sonar Kit" << endl;
	}
}


/*************************************************************************/
/* Non-Person Functions												     */
/*************************************************************************/

// Sets the number of oil barrels in the oil field
void StudentWorld::setNumBarrels(int barrels) {
	m_iNumBarrels = barrels;
}

// Returns the number of oil barrels remaining in the field
int StudentWorld::getNumBarrels() {
	return m_iNumBarrels;
}

// Decrements the number of oil barrels remaining in the field
void StudentWorld::decNumBarrels() {
	--m_iNumBarrels;
}

// Handles the case where any Goodie is picked up
void StudentWorld::pickupGoodieIM(int ID, int points, int soundEffect) {
	// If there is a collision (distance <= 3), increase the score and play a sound
	// Depending on the goodie, a different action occurs
	// Uses the image ID to identify the goodie

	switch (ID) {
	case IID_BARREL:
		decNumBarrels();
		break;
	case IID_GOLD:
		m_pIceMan.lock()->incGold();
		break;
	case IID_SONAR:
		m_pIceMan.lock()->incSonarKits();
		break;
	case IID_WATER_POOL:
		m_pIceMan.lock()->incWater();
		break;
	}
	increaseScore(points);
	playSound(soundEffect);
}

// Handles when a Protester picks up Gold
bool StudentWorld::pickupGoldP(int x, int y) {
	for (auto actor : m_actors) {
		if (actor == nullptr)
			continue;
		if (m_distanceCalc.getDistance(actor->getX(), actor->getY(), x, y) <= 3) {
			if (actor->getID() == IID_PROTESTER || actor->getID() == IID_HARD_CORE_PROTESTER) {
				static_pointer_cast<Protester>(actor)->pickUpGold();
				return true;
			}
		}
	}
	return false;
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
				hitProtester = true; // Allows for multiple protesters to be hit by the same squirt
				static_pointer_cast<Protester>(actor)->annoy(2);
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
		if (actor == nullptr)
			continue;
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
bool StudentWorld::hitByBoulder(int x, int y) {

	// Did the boulder hit IceMan, a Protester or another Boulder?
	for (auto actor : m_actors) {
		if (actor == nullptr)
			continue;
		if ((actor->getX() >= x) && (actor->getX() <= x + 4) && (actor->getY() + 4 == y)) {
			int actorID = actor->getID();
			if (actorID == IID_PROTESTER || actorID == IID_HARD_CORE_PROTESTER) {
				static_pointer_cast<Protester>(actor)->annoy(100);
				increaseScore(500);
			}
			if (actorID == IID_PLAYER) {
				static_pointer_cast<IceMan>(actor)->annoy(100);
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

// Handles when a Protester shouts at IceMan
void StudentWorld::iceManShoutedAt() {
	m_pIceMan.lock()->annoy(2);
	playSound(SOUND_PROTESTER_YELL);
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

// Compute distance to IceMan. Returns the units to reach IceMan through the maze, avoiding
// ice and boulder. If there isn't a path, it returns UINT_MAX.
unsigned int StudentWorld::getPathDistanceToIceMan(int x, int y) const {
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
	bool hasLineOfSight = false;

	shared_ptr<IceMan> pIceMan = m_pIceMan.lock();
	
	// Check for ice or boulders in the way on the vertical and
	// horizontal axis
	const int iceX = pIceMan->getX();
	const int iceY = pIceMan->getY();	
		 
	if (x == iceX) {
		hasLineOfSight = true;

		// Are we below IceMan's?
		if (y < iceY) {
			for (int j = y; j < iceY && hasLineOfSight; j++) {
				hasLineOfSight = !isBlocked(x, j, direction);
			}
			// If we have a line of sight, then face IceMan
			if (hasLineOfSight)
				direction = GraphObject::Direction::up;
		}
		else if (y > iceY) { // Are we above IceMan?
			for (int j = y; j > iceY && hasLineOfSight; j--) {
				hasLineOfSight = !isBlocked(x, j, direction);
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
		hasLineOfSight = true;

		// Check for ice or boulders in the way on the vertical axis
		// Are we on IceMan's left?
		if (x < iceX) {
			for (int i = x; i < iceX && hasLineOfSight; i++) {
				hasLineOfSight = !isBlocked(i, y, direction);
			}
			// If we have a line of sight, then face IceMan
			if (hasLineOfSight)
				direction = GraphObject::Direction::right;
		}
		else if (x > iceX) { // Are we on IceMan's right?
			for (int i = x; i > iceX && hasLineOfSight; i--) {
				hasLineOfSight = !isBlocked(i, y, direction);
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
bool StudentWorld::isBlocked(int x, int y, GraphObject::Direction direction) const {
	// Is this location outside of the ice field's boundries?
	if (x < 0 || x > (ICE_WIDTH - PERSON_SIZE) || y < 0 || y > ICE_HEIGHT)
		return true;

	// Is this location occupied by a boulder?
	if (isBlockedByBoulder(x, y)) {
		return true;
	}

	// Is this location occupied by ice?
	int xBegin{ x }, xEnd{ x };
	int yBegin{ y }, yEnd{ y };
	
	// Check the strip of ice on the far side of the box surround the sprite
	// with respect to the direction we're facing. 
	// TODO: Keep in mind that the (x,y) values we're receiving are already being offset by
	// 1 depending on the direction. A cleaner approach would be to handle it all here, i.e. 
	// x,y would always represent the lower left coordinate of the sprite.
	switch (direction) {
		case GraphObject::Direction::up:
			xEnd	+= PERSON_SIZE;
			yBegin	+= PERSON_SIZE;
			yEnd	= yBegin + 1;
			break;
		case GraphObject::Direction::down:
			xEnd	+= PERSON_SIZE;
			yEnd	= yBegin + 1;
			break;
		case GraphObject::Direction::left:
			xEnd	= xBegin + 1;
			yEnd	+= PERSON_SIZE;
			break;
		case GraphObject::Direction::right:
			xBegin	+= PERSON_SIZE;
			xEnd	= xBegin + 1;
			yEnd	+= PERSON_SIZE;
			break;
	};

	
	xBegin	= std::max<int>(0, std::min<int>(ICE_WIDTH, xBegin));
	xEnd	= std::max<int>(0, std::min<int>(ICE_WIDTH, xEnd));
	yBegin	= std::max<int>(0, std::min<int>(ICE_HEIGHT, yBegin));
	yEnd	= std::max<int>(0, std::min<int>(ICE_HEIGHT, yEnd));

	for (int x_ = xBegin; x_ < xEnd; x_++) {
		for (int y_ = yBegin; y_ < yEnd; y_++) {
			if (m_ice[x_][y_] != nullptr && m_ice[x_][y_]->isAlive()) {
				return true;
			}
		}
	}

	return false;
}

// Is this location occupied by a Boulder?
bool StudentWorld::isBlockedByBoulder(int x, int y) const {
	// Check for Boulders
	for (const auto& actor : m_actors) {
		if (actor == nullptr)
			continue;

		if (actor->getID() == IID_BOULDER) {
			if (x > actor->getX() - 4 && x < actor->getX() + 4 && y > actor->getY() - 4 && y < actor->getY() + 4)
				return true;
		}
	}
	return false;
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

void StudentWorld::dump() const {
	m_shortestPathToIceMan.dump();
}