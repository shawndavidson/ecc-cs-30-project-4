#include <string>
#include <typeinfo>
#include <exception>
#include <assert.h>
#include <algorithm>
#include <thread>
#include <sstream>
#include <iomanip>

#include "StudentWorld.h"
#include "Actor.h"


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
			if (y > 3 && x >= 30 && x <= 33)
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
 
#if TEST_WORKER_MULTITHREADS
	// TODO: make shared data thread safe
	startWorkerThreads();
#endif

	return GWSTATUS_CONTINUE_GAME;
}

#if TEST_WORKER_MULTITHREADS
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
#endif

// Handle movement for all game objects within our world
int StudentWorld::move()
{
	addNewActors();

	// Compute the distance between all Actors
	computeDistancesBetweenActors();

	// Compute shortest path to exit (used by Protesters)
	m_shortestPathToExit.compute(60, 60);

	// Compute shortest path to IceMan (also used by Protesters)
	{
		auto pIceMan = m_pIceMan.lock();
		m_shortestPathToIceMan.compute(pIceMan->getX(), pIceMan->getY());
	}

	setGameStatText(getGameStatText());

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
	bool foundIce = false;

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
					m_ice[finalX][finalY].reset();
					foundIce = true;
				}
			}
		}
	}

	if (foundIce) {
		playSound(SOUND_DIG);
	}
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

	// Clearing ice as we go in digUpIce()
}

string StudentWorld::getGameStatText() {
	auto iceMan = m_pIceMan.lock();
	stringstream ss;
	ss << setfill(' ') << left;
	ss <<
		"Lvl: " << setw(2) << to_string(getLevel()) <<
		"  Lives: " << setw(1) << to_string(getLives()) <<
		"  Hlth: " << setw(3) << to_string(100 * iceMan->getHitPoints() / 10) + "%" <<
		"  Wtr: " << setw(2) << to_string(iceMan->getWater()) <<
		"  Gld: " << to_string(iceMan->getGold()) <<
		"  Oil Left: " << to_string(getNumBarrels()) <<
		"  Sonar: " << to_string(iceMan->getSonarKits()) <<
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
				// Allows for multiple protesters to be hit by the same squirt
				hitProtester = true; 
				// Prevents an annoyed protester from awarding points mutliple times
				if (!static_pointer_cast<Protester>(actor)->isAnnoyed())
					static_pointer_cast<Protester>(actor)->squirtedByIceMan();
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
				static_pointer_cast<Protester>(actor)->bonkedByBoulder();
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
	auto iceMan = m_pIceMan.lock();
	if (iceMan == nullptr)
		return;
	iceMan->annoy(2);
	playSound(SOUND_PROTESTER_YELL);
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
			// Do we have a line of sight with him by looking up?
			for (int j = y; j < iceY && hasLineOfSight; j++) {
				hasLineOfSight = !isBlocked(x, j, GraphObject::Direction::up);
			}
			// If we have a line of sight, then face IceMan above
			if (hasLineOfSight)
				direction = GraphObject::Direction::up;
		}
		else if (y > iceY) { // Are we above IceMan?
			// Do we have a line of sight with by looking down?
			for (int j = y; j > iceY && hasLineOfSight; j--) {
				hasLineOfSight = !isBlocked(x, j, GraphObject::Direction::down);
			}
			// If we have a line of sight, then face IceMan below
			if (hasLineOfSight)
				direction = GraphObject::Direction::down;
		}
		else {
			direction = GraphObject::Direction::none;
		}
	}
	else if (y == iceY) {
		hasLineOfSight = true;

		// Check for ice or boulders in the way on the vertical axis
		// Are we on IceMan's left?
		if (x < iceX) {
			// Do we have a line of sight with by looking to the right?
			for (int i = x; i < iceX && hasLineOfSight; i++) {
				hasLineOfSight = !isBlocked(i, y, GraphObject::Direction::right);
			}
			// If we have a line of sight, then face IceMan on our right
			if (hasLineOfSight)
				direction = GraphObject::Direction::right;
		}
		else if (x > iceX) { // Are we on IceMan's right?
			// Do we have a line of sight with by looking to the left?
			for (int i = x; i > iceX && hasLineOfSight; i--) {
				hasLineOfSight = !isBlocked(i, y, GraphObject::Direction::left);
			}
			// If we have a line of sight, then face IceMan on our left
			if (hasLineOfSight)
				direction = GraphObject::Direction::left;
		}
		else {
			direction = GraphObject::Direction::none;
		}
	}

	return hasLineOfSight;
}

// Is this location occupied by Ice, Boulder, or is out of bounds
bool StudentWorld::isBlocked(int x, int y, GraphObject::Direction direction) const {
	// Is this location outside of the ice field's boundries?
	switch (direction) {
	case GraphObject::Direction::up:
		{
			if (x < 0 || x > (ICE_WIDTH - PERSON_SIZE) ||
				y < 0 || y + 1 > ICE_HEIGHT) {
				return true;
			}
		}
		break;
	case GraphObject::Direction::down:
		{
			if (x < 0 || x > (ICE_WIDTH - PERSON_SIZE) ||
				y - 1 < 0 || y > ICE_HEIGHT) {
				return true;
			}
		}
		break;
	case GraphObject::Direction::left:
		{
			if (x - 1 < 0 || x > (ICE_WIDTH - PERSON_SIZE) ||
				y < 0 || y > ICE_HEIGHT) {
				return true;
			}
		}
		break;
	case GraphObject::Direction::right:
		{
			if (x < 0 || x + 1 > (ICE_WIDTH - PERSON_SIZE) ||
				y < 0 || y > ICE_HEIGHT) {
				return true;
			}
		}
		break;
	};

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
			yEnd	= yBegin;			
			yBegin  -= 1;
			break;
		case GraphObject::Direction::left:
			xEnd	= xBegin;			
			xBegin  -= 1;
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

	// Is this location occupied by a boulder?
	if (isBlockedByBoulder(x, y, direction)) {
		return true;
	}

	return false;
}

// Is this location occupied by a Boulder?
bool StudentWorld::isBlockedByBoulder(int x, int y, GraphObject::Direction direction) const {
	// Check for Boulders
	for (const auto& pBoulder : m_actors) {
		// Is this a boulder?
		if (pBoulder == nullptr || pBoulder->getID() != IID_BOULDER)
			continue;

		int boulderX = pBoulder->getX();
		int boulderY = pBoulder->getY();

		switch (direction) {
		case GraphObject::Direction::none:
			{
			}
			break;
		case GraphObject::Direction::up:
			{
				if (std::abs(boulderX - x) <= 3 && boulderY == y + 4)
					return true;
			}
			break;
		case GraphObject::Direction::down:
			{
				if (std::abs(boulderX - x) <= 3 && boulderY == y - 4)
					return true;
			}
			break;
		case GraphObject::Direction::left:
			{
				if (boulderX == x - 4 && std::abs(boulderY - y) <= 3)
					return true;
			}
			break;
		case GraphObject::Direction::right:
			{
				if (boulderX == x + 4 && std::abs(boulderY - y) <= 3)
					return true;
			}
			break;
		default:
			break;
		};
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


/************************************************************************************************/
/*																								*/
/* DISTANCE CALCULATOR																		    */
/*																								*/
/************************************************************************************************/

#include <cmath>
#include <assert.h>

// Constructor
DistanceCalculator::DistanceCalculator()
{
	// TODO: Optimize - the field is symmetric (square) so I think we can cut this by 50%
	//       The distance from square a to b is the same as b to as.
	for (int x0 = 0; x0 < VIEW_WIDTH; x0++) {
		for (int y0 = 0; y0 < VIEW_HEIGHT; y0++) {
			for (int x1 = 0; x1 < VIEW_WIDTH; x1++) {
				for (int y1 = 0; y1 < VIEW_HEIGHT; y1++) {
					// Use Pythagorean Theorm to calculate distance between locations
					m_table[x0][y0][x1][y1] = (uint8_t)ceil(sqrt(pow((x1 - x0), 2) + pow((y1 - y0), 2)));
				}
			}
		}
	}

#if TEST_DISTANCECALCULATOR
	Test();
#endif //UNIT_TEST
}

// Destructor
DistanceCalculator::~DistanceCalculator() {
}

#if TEST_DISTANCECALCULATOR
// Test optimized table 
void DistanceCalculator::Test() {
	uint8_t distance;
	uint8_t answer;

	for (int x0 = 0; x0 < VIEW_WIDTH; x0++) {
		for (int y0 = 0; y0 < VIEW_HEIGHT; y0++) {
			for (int x1 = 0; x1 < VIEW_WIDTH; x1++) {
				for (int y1 = 0; y1 < VIEW_HEIGHT; y1++) {
					distance = getDistance(x0, y0, x1, y1);
					answer = (uint8_t)ceil(sqrt(pow((x1 - x0), 2) + pow((y1 - y0), 2)));
					assert(distance == answer);
				}
			}
		}
	}
}
#endif //UNIT_TEST


/************************************************************************************************/
/*																								*/
/* SHORTEST PATH FINDER																		    */
/*																								*/
/************************************************************************************************/

#include <queue>
#include <iomanip>

// Constructor
ShortestPathFinder::ShortestPathFinder(StudentWorld* pStudentWorld)
	: m_pStudentWorld(pStudentWorld),
	m_originX(0),
	m_originY(0),
	m_distances{ UCHAR_MAX }
{
}

// Compute distances from all units to a specific location
bool ShortestPathFinder::compute(int x, int y) {
	if (x < 0 || x >= VIEW_WIDTH || y < 0 || y >= VIEW_HEIGHT) {
		return false;
	}

	m_originX = x;
	m_originY = y;

	// Reset distances to our representation of infinity
	memset(m_distances, UINT_MAX, VIEW_WIDTH * VIEW_HEIGHT * sizeof(size_t));

	std::queue<Coordinates> queue;

	// Add the initial location which is our reference when measuring distance
	queue.emplace(x, y, 0);

#if TEST_SHORTESTPATHFINDER
	size_t maxQueueSize = 0;
	size_t minX{}, maxX{}, minY{}, maxY{};
#endif

	// Perform a BFS (Breadth First Search)
	while (!queue.empty()) {
		Coordinates unit = queue.front();
		queue.pop();

#if TEST_SHORTESTPATHFINDER
		minX = std::min<uint8_t>(minX, unit.x);
		maxX = std::max<uint8_t>(maxX, unit.x);
		minY = std::min<uint8_t>(minY, unit.y);
		maxY = std::max<uint8_t>(maxY, unit.y);
#endif
		// KLUDGE: Without this statement, the queue grows larger than it should 
		// >4096 (64*64) causing a progressive slow down to game play that seems
		// to be correlated to how many tunnels IceMan digs.
		if (m_distances[unit.x][unit.y] != UINT_MAX)
			continue;

		// Store distance
		m_distances[unit.x][unit.y] = unit.distance;

		// Explore adjacent units, if valid, unoccupied, and unexplored
		for (int direction = GraphObject::Direction::up;
			direction <= GraphObject::Direction::right;
			direction++) {

			switch (direction) {
			case GraphObject::Direction::up:
			{
				if (x < 0 || x >(ICE_WIDTH - PERSON_SIZE) ||
					y < 0 || y + 1 > ICE_HEIGHT) {
					continue;
				}
			}
			break;
			case GraphObject::Direction::down:
			{
				if (x < 0 || x >(ICE_WIDTH - PERSON_SIZE) ||
					y - 1 < 0 || y > ICE_HEIGHT) {
					continue;
				}
			}
			break;
			case GraphObject::Direction::left:
			{
				if (x - 1 < 0 || x > (ICE_WIDTH - PERSON_SIZE) ||
					y < 0 || y > ICE_HEIGHT) {
					continue;
				}
			}
			break;
			case GraphObject::Direction::right:
			{
				if (x < 0 || x + 1 >(ICE_WIDTH - PERSON_SIZE) ||
					y < 0 || y > ICE_HEIGHT) {
					continue;
				}
			}
			break;
			};

			switch (direction) {
			case GraphObject::Direction::up:
				// If there's a path above that isn't blocked AND the distance is still unknown
				if (!getStudentWorld()->isBlocked(unit.x, unit.y, (GraphObject::Direction)direction) &&
					unit.distance + 1 < m_distances[unit.x][unit.y + 1]) {
					queue.emplace(unit.x, unit.y + 1, unit.distance + 1);
				}
				break;
			case GraphObject::Direction::down:
				// If there's a path below that isn't blocked AND the distance is still unknown
				if (!getStudentWorld()->isBlocked(unit.x, unit.y, (GraphObject::Direction)direction) &&
					unit.distance + 1 < m_distances[unit.x][unit.y - 1]) {
					queue.emplace(unit.x, unit.y - 1, unit.distance + 1);
				}
				break;
			case GraphObject::Direction::left:
				// If there's a path on the left that isn't blocked AND the distance is still unknown
				if (!getStudentWorld()->isBlocked(unit.x, unit.y, (GraphObject::Direction)direction) &&
					unit.distance + 1 < m_distances[unit.x - 1][unit.y]) {
					queue.emplace(unit.x - 1, unit.y, unit.distance + 1);
				}
				break;
			case GraphObject::Direction::right:
				// If there's a path on the right that isn't blocked AND the distance is still unknown
				if (!getStudentWorld()->isBlocked(unit.x, unit.y, (GraphObject::Direction)direction) &&
					unit.distance + 1 < m_distances[unit.x + 1][unit.y]) {
					queue.emplace(unit.x + 1, unit.y, unit.distance + 1);
				}
				break;
			}
		}

#if TEST_SHORTESTPATHFINDER
		maxQueueSize = std::max<unsigned int>(maxQueueSize, queue.size());
#endif
	}

	return true;
}

// Get the direction and distance of the shortest path to the location previously 
// set by compute()
bool ShortestPathFinder::getShortestPath(int x, int y, DirectionDistance& result) const {
	std::vector<DirectionDistance> directions;

	// Grab the squares around our the location x,y
	for (int d = GraphObject::Direction::up; d <= GraphObject::Direction::right; d++) {
		GraphObject::Direction dir = (GraphObject::Direction)d;

		switch (dir) {
		case GraphObject::Direction::up:
			if (y + 1 < VIEW_HEIGHT)
				directions.emplace_back(dir, m_distances[x][y + 1]);
			break;
		case GraphObject::Direction::down:
			if (y - 1 >= 0)
				directions.emplace_back(dir, m_distances[x][y - 1]);
			break;
		case GraphObject::Direction::left:
			if (x - 1 >= 0)
				directions.emplace_back(dir, m_distances[x - 1][y]);
			break;
		case GraphObject::Direction::right:
			if (x + 1 < VIEW_WIDTH)
				directions.emplace_back(dir, m_distances[x + 1][y]);
			break;
		}
	}

	// Sort them in descending order of distance
	sort(begin(directions), end(directions), [](const DirectionDistance& dd1, const DirectionDistance& dd2) {
		return dd1.distance < dd2.distance;
		});

	// Return the object with the shortest distance
	if (!directions.empty()) {
		result = directions[0];
		return true;
	}

	// Unable to move in any direction
	return false;
}

// Dump the distance matrix to the console
void ShortestPathFinder::dump() const {
	cout << "ShortestPathFinder::dump()" << endl;

	for (int y = VIEW_HEIGHT - 1; y >= 0; y--) {
		for (int x = 0; x < VIEW_WIDTH; x++) {
			cout << std::setw(2) << std::hex << (int)m_distances[x][y] << " ";
		}
		cout << endl;
	}
	cout << endl << endl;
}


