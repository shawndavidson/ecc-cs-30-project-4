#include "Actor.h"
#include "StudentWorld.h"

/************************************************************************************************/
/*																								*/
/* ACTOR																					    */
/*																								*/
/************************************************************************************************/

// Constructor
Actor::Actor(
	StudentWorld* pStudentWorld,
	int imageID,
	int startX,
	int startY,
	Direction dir		/*= right*/,
	double size			/*= 1.0*/,
	unsigned int depth	/*= 0*/,
	bool visible		/*= true*/,
	bool canAnnoy		/*= false*/,
	bool canPickupIM	/*= false*/,
	bool canPickupP		/*= false*/)
	: GraphObject(imageID, startX, startY, dir, size, depth),
	m_bAlive(true),
	m_bCanAnnoy(canAnnoy),
	m_bCanPickupIM(canPickupIM),
	m_bCanPickupP(canPickupP),
	m_pStudentWorld(pStudentWorld)
{
	setAlive(true);
	setVisible(visible);
}

// Destructor
Actor::~Actor()
{
}

// Check if we can be picked up by another Actor? 
bool Actor::canPickupIM() {
	return m_bCanPickupIM;
}

void Actor::setCanPickupIM(bool collectable) {
	m_bCanPickupIM = collectable;
}

bool Actor::canPickupP() {
	return m_bCanPickupP;
}

void Actor::setCanPickupP(bool collectable) {
	m_bCanPickupP = collectable;
}

// Check if we can be annoyed by another Actor? 
bool Actor::canAnnoy() const {
	return m_bCanAnnoy;
}

// Check if we are still alive
bool Actor::isAlive() const {
	return m_bAlive;
}

// Update our alive status
void Actor::setAlive(bool alive, bool changeVisibility /*= true*/) {
	m_bAlive = alive;

	// Update the visibility so dead objects aren't visible, by default
	if (changeVisibility) 
		setVisible(alive);
}

void Actor::setPoints(int points) {
	m_iPoints = points;
}

int Actor::getPoints() {
	return m_iPoints;
}

void Actor::setSoundEffect(int s_e) {
	m_iSoundEffect = s_e;
}

int Actor::getSoundEffect() {
	return m_iSoundEffect;
}

int Actor::getDistanceToIceman() {
	return getStudentWorld()->getDistanceToIceMan(getX(), getY());
}



/************************************************************************************************/
/*																								*/
/* ICE																						    */
/*																								*/
/************************************************************************************************/

// Constructor
Ice::Ice(StudentWorld* pStudentWorld, int startX, int startY)
	: Actor(pStudentWorld,
		IID_ICE,
		startX, startY,
		Direction::right,
		ICE_SIZE /*size*/,
		3 /*depth*/,
		true /*visible*/,
		false /*canAnnoy*/,
		false /*canPickup*/)
{
};

// Destructor
Ice::~Ice() {
}


/************************************************************************************************/
/*																								*/
/* BOULDER																					    */
/*																								*/
/************************************************************************************************/

// Constructor
Boulder::Boulder(StudentWorld* pStudentWorld, int startX, int startY)
	: Actor(pStudentWorld,
		IID_BOULDER,
		startX, startY,
		Direction::right,
		BOULDER_SIZE	/*size*/,
		BOULDER_DEPTH	/*depth*/,
		true			/*visible*/,
		false			/*canAnnoy*/,
		false			/*canPickupIM*/,
		false			/*canPickupP*/),
	m_bStable(true),	// Boulders start out stable
	m_bWaiting(false),
	m_bFalling(false)
{
	setSoundEffect(SOUND_FALLING_ROCK);
};

// Destructor
Boulder::~Boulder() {
}

// Handle tick
void Boulder::doSomething() {
	// If Bouder is dead, return immediately
	if (!isAlive())
		return;

	// Else, check the state of the Boulder
	StudentWorld* pStudentWorld = getStudentWorld();
	bool goundUnderBoulder = pStudentWorld->StudentWorld::isGroundUnderBoulder(getX(), getY());

	// Check for Ice below Boulder
	if (isStable()) {
		if (!goundUnderBoulder) {
			setStable(false);
			setWaiting(true);
			setTicksRemaining(30);
		}

	}

	// If 30 ticks have elapsed,
	// set state to falling
	if (isWaiting()) {

		if (getTicksRemaining() > 0)
			decTicksRemaining();
		else {
			setFalling(true);
			setWaiting(false);
			pStudentWorld->playSound(getSoundEffect());
		}
	}

	if (isFalling()) {
		// If within 3 of Protesters or Iceman,
		// Cause 100 points of annoyance
		// If collides with another Boulder, return
		if (pStudentWorld->StudentWorld::hitByBoulder(getX(), getY())) {
			setAlive(false);
			return;
		}

		// Move down 1 tick until
		// a. hits bottom of ice field
		// b. hits top of another boulder OR
		// c. hits Ice
		// Then set state dead
		if (!goundUnderBoulder)
			moveTo(getX(), getY() - 1);
		else {
			setAlive(false);
		}
	}
}


// Set stable state
void Boulder::setStable(bool stable) {
	m_bStable = stable;
}
// Get stable state
bool Boulder::isStable() {
	return m_bStable;
}

// Set waiting state
void Boulder::setWaiting(bool waiting) {
	m_bWaiting = waiting;
}
// Get waiting state
bool Boulder::isWaiting() {
	return m_bWaiting;
}

// Set falling state
void Boulder::setFalling(bool falling) {
	m_bFalling = falling;
}
// Get falling state
bool Boulder::isFalling() {
	return m_bFalling;
}

// Sets the number of ticks a Boulder
// must wait to do something
void Boulder::setTicksRemaining(int ticks) {
	m_iTicksRemaining = ticks;
}

// Gets the number of ticks a Boulder
// must wait before doing an action
int Boulder::getTicksRemaining() {
	return m_iTicksRemaining;
}

// Decrements the number of ticks remaining
void Boulder::decTicksRemaining() {
	--m_iTicksRemaining;
}


/************************************************************************************************/
/*																								*/
/* SQUIRT																					    */
/*																								*/
/************************************************************************************************/

// Constructor
Squirt::Squirt(StudentWorld* pStudentWorld,
	int startX,			/*IceMan's X*/
	int startY,			/*IceMan's Y*/
	Direction dir)		/*IceMan's direction*/
	: Actor(pStudentWorld,
		IID_WATER_SPURT,
		startX, startY,
		dir,
		SQUIRT_SIZE		/*size*/,
		SQUIRT_DEPTH	/*depth*/,
		true,			/*visible*/
		false			/*canAnnoy*/,
		false			/*canPickupIM*/,
		false			/*canPickupP*/),
	m_iTravelDist(4)
{
	setSoundEffect(SOUND_PLAYER_SQUIRT);
};

// Destructor
Squirt::~Squirt() {
}

// Handle tick
void Squirt::doSomething() {
	// If squirt is not alive, return immediately
	if (!isAlive()) {
		return;
	}

	// Handles if Squirt hit something
	if (getStudentWorld()->StudentWorld::hitBySquirt(getX(), getY())) {
		setAlive(false);
		return;
	}

	// Else, squirt moves forward
	if (m_iTravelDist >= 1)
	{
		--m_iTravelDist;
		const int x = getX();
		const int y = getY();
		const Direction dir = getDirection();
		if (dir == Direction::left && x > 0) {
			moveTo(x - 1, y);
		}
		else if (dir == Direction::right && x < 60) {
			moveTo(x + 1, y);
		}
		else if (dir == Direction::down && y > 0) {
			moveTo(x, y - 1);
		}
		else if (dir == Direction::up && y < 60) {
			moveTo(x, y + 1);
		}
		return;
	}
	else {
		setAlive(false);
	}
}


/************************************************************************************************/
/*																								*/
/* PERSON																					    */
/*																								*/
/************************************************************************************************/

// Constructor
Person::Person(
	StudentWorld* pStudentWorld,
	int imageID,
	int startX,
	int startY,
	Direction dir,
	int nHitPoints)
	: Actor(pStudentWorld,
		imageID,
		startX,
		startY,
		dir,
		1.0     /*size*/,
		0       /*depth*/,
		true    /*visible*/,
		true    /*canAnnoy*/,
		false   /*canPickup*/),
	m_nHitPoints(nHitPoints)
{
}

// Destructor
Person::~Person() {
}


/************************************************************************************************/
/*																								*/
/* ICEMAN																					    */
/*																								*/
/************************************************************************************************/

// Constructor
IceMan::IceMan(StudentWorld* pStudentWorld)
	: Person(pStudentWorld,
		IID_PLAYER,
		30 /*startX*/,
		60 /*startY*/,
		Direction::right,
		10 /* nHitPoints */),
	m_iGold(0),         // start with 0
	m_iSonarKits(1),    // start with 1
	m_iWater(5)         // start with 5
{

	// TODO: Remove - for testing
#if TEST_EVENTS
	getStudentWorld()->listenForEvent(
		EventTypes::EVENT_TEST,
		[&](SharedEventPtr pEvent) {
			// TODO: Is capturing "this" safe or could it cause an access violation? 
			// happens if this object is removed before the callback is invoked?

			// This is a little dangerous but it works. Try to find a better way! 
			Event<EventTestData>* pData = (Event<EventTestData>*)pEvent.get();

			this->handleTestEvent(pData->getData().num, pData->getData().text);
		});
#endif
}

// Destructor
IceMan::~IceMan() {
}

// Handle Tick
void IceMan::doSomething() {
	// If we're not alive, there's nothing to do
	if (!isAlive()) {
		return;
	}

	// Cache a pointer to StudentWorld, for the sake of convenience
	StudentWorld* const pStudentWorld = getStudentWorld();
	Direction direction = getDirection();

	int key;

	// Did the user press a key?
	if (pStudentWorld->getKey(key)) {
		const int x = getX();
		const int y = getY();
		const double size = getSize();

		// Handle user keyboard input
		switch (key) {
			// Check if there is a Boulder before moving in any direction
		case KEY_PRESS_LEFT:
			// If we're already facing left, move left. Otherwise, just face left;
			if (!pStudentWorld->isBlockedByBoulder(x, y, direction) &&
				direction == Direction::left && x > 0)
				takeOneStep(x - 1, y);
			else
				setDirection(Direction::left);
			break;
		case KEY_PRESS_RIGHT:
			// If we're already facing right, move right. Otherwise, just face right;
			if (!pStudentWorld->isBlockedByBoulder(x, y, direction) &&
				direction == Direction::right && x < (VIEW_WIDTH - (size * ICEMAN_SIZE / ICE_SIZE)))
				takeOneStep(x + 1, y);
			else
				setDirection(Direction::right);
			break;
		case KEY_PRESS_UP:
			// If we're already facing up, move up. Otherwise, just face up;
			if (!pStudentWorld->isBlockedByBoulder(x, y, direction) &&
				direction == Direction::up && y < ICE_HEIGHT)
				takeOneStep(x, y + 1);
			else
				setDirection(Direction::up);
			break;
		case KEY_PRESS_DOWN:
			// If we're already facing down, move down. Otherwise, just face down;
			if (!pStudentWorld->isBlockedByBoulder(x, y, direction) &&
				direction == Direction::down && y > 0)
				takeOneStep(x, y - 1);
			else
				setDirection(Direction::down);
			break;
		case KEY_PRESS_SPACE:
			// Fire a water squirt
			if (m_iWater > 0) {
				pStudentWorld->StudentWorld::fireSquirt(getX(), getY(), getDirection());
				--m_iWater;
				return;
			}
			break;
		case KEY_PRESS_ESCAPE:
			// Restart level. Lose a life
			setAlive(false);
			break;
		case KEY_PRESS_TAB:
			// Drop Gold Nugget
			if (m_iGold > 0) {
				pStudentWorld->StudentWorld::dropGold();
				--m_iGold;
			}
			break;
		case 'z':
		case 'Z':
			// Use SonarKits
			if (m_iSonarKits > 0) {
				pStudentWorld->StudentWorld::useSonarKit();
				--m_iSonarKits;
			}
			break;

#if TEST_ICEMAN
		case 'p':
		{
			getStudentWorld()->dump();
		}
		break;
#endif
#if TEST_EVENTS
		case 'e':
		{
			const int tick = getStudentWorld()->getTick() + 100;

			// Pass along this data with the Event
			struct EventTestData data { 1234, "Hello World!" };

			getStudentWorld()->pushEvent(make_shared<Event<EventTestData>>(tick, EventTypes::EVENT_TEST, data));
		}
		break;
#endif
		case 'q':
			// Quit game
			pStudentWorld->cleanUp();
			break;
		default:
			// Do nothing if input key is invalid
			break;
		};
	}
}

// Handle Annoy
void IceMan::annoy(int nHitPoints) {
	Person::annoy(nHitPoints);
	if (isAnnoyed()) {
		setAlive(false);
	}
}

// Increments the gold counter by 1
void IceMan::incGold() {
	++m_iGold;
}

// Increments the sonar kits counter by 1
void IceMan::incSonarKits() {
	++m_iSonarKits;
}

// Increments the water counter by 5 (since picking up a Water Pool gives 5)
void IceMan::incWater() {
	m_iWater += 5;
}

// Decrements the gold counter by 1 if there is at least 1 left
void IceMan::decGold() {
	--m_iGold;
}

// Decrements the sonar kits counter by 1 if there is at least one left
void IceMan::decSonarKits() {
	--m_iSonarKits;
}

// Decrements the water counter by 1 if there is at least one left
void IceMan::decWater() {
	--m_iWater;
}

// Return Gold
int IceMan::getGold() {
	return m_iGold;
}

// Return Sonar Kits
int IceMan::getSonarKits() {
	return m_iSonarKits;
}

// Return Water Squirts
int IceMan::getWater() {
	return m_iWater;
}

#if TEST_ICEMAN
// Handle an Event
void IceMan::handleTestEvent(int num, const char* text) {
	std::cout << "Tick: " << getStudentWorld()->getTick()
		<< ", RegularProtester::handleTestEvent, num: " << num
		<< ", text: " << text
		<< std::endl;


}
#endif // TEST_ICEMAN

// Take a step and dig up ice, if necessary
void IceMan::takeOneStep(int x, int y) {
	moveTo(x, y);
	getStudentWorld()->digUpIce(x, y);
}


/************************************************************************************************/
/*																								*/
/* PROTESTER																				    */
/*																								*/
/************************************************************************************************/

/*************************************************************************/
/* Macros														         */
/*************************************************************************/
// Get the random # of squares to move in the current direction
#define GET_RANDOM_NUM_SQUARES_TO_MOVE()    ((int)8 + (rand() % (60 - 8 + 1)))

// Calculated # of ticks to be stunned for when annoyed
#define CALCULATE_STUNNED_TICKS(GAME_LEVEL) (std::max<size_t>({ 50, 100 - GAME_LEVEL * 10 }))

/*************************************************************************/
/* Types															     */
/*************************************************************************/
const int           EXIT_POSITION_X = 60;
const int           EXIT_POSITION_Y = 60;
const double        PROTESTER_SIZE = 1.0;
const unsigned int  NON_RESTING_TICKS_BETWEEN_SHOUTS = 15;
const int           MAX_SHOUTING_RANGE_UNITS = 4;
const int           NON_RESTING_TICKS_BETWEEN_TURNING = 200;

/*************************************************************************/
/* Static Members														 */
/*************************************************************************/
std::random_device  Protester::m_randomDevice;
std::mt19937        Protester::m_randomGenerator = std::mt19937(m_randomDevice());

// Constructor
Protester::Protester(
	StudentWorld* pStudentWorld,
	int imageID,
	int startX,
	int startY,
	int nHitPoints,
	bool bCanTrackIceMansCell)
	: Person(pStudentWorld,
		imageID,
		startX,
		startY,
		Direction::left,
		nHitPoints),
	m_nTicksToWaitBetweenMoves(std::max<unsigned int>(0, 3 - getStudentWorld()->getLevel() / 4)),
	m_nLeaveTheOilField(false),
	m_nLastShoutedTick(0),
	m_nNumSquaresToMoveInCurrentDirection(GET_RANDOM_NUM_SQUARES_TO_MOVE()),
	m_nTickOfLastPerpendicularTurn(0),
	m_allDirections{ Direction::up, Direction::down, Direction::left, Direction::right },
	m_nTicksStunned(0),
	m_nIceManCellRange(16 + getStudentWorld()->getLevel() * 2), // know as M on pg. 45
	m_bCanTrackIceMansCell(bCanTrackIceMansCell)
{
}

// Destructor
Protester::~Protester() {
}

// Handle a tick
void Protester::doSomething() {
	if (!isAlive())
		return;

	// If we're fully annoyed, give up by exiting the oil field
	if (!m_nLeaveTheOilField && isAnnoyed()) {
		leave();

		getStudentWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
		m_nTicksStunned = 0;
	}

	// If we're stunned, then rest for N ticks...
	if (m_nTicksStunned > 0) {
		m_nTicksStunned--;
		return;
	}

	// Rest to give the user a chance to react in human-speed
	if (!m_nLeaveTheOilField &&
		getStudentWorld()->getTick() % m_nTicksToWaitBetweenMoves == 0) {
		return;
	}

	// Are we leaving the oil field?
	if (m_nLeaveTheOilField) {
		// Have we reached the exit?
		if (getX() == EXIT_POSITION_X && getY() == EXIT_POSITION_Y) {
			setAlive(false);
		}
		else {
			moveTowardsExit();
		}
		return;
	}

	// If we are within shouting distance, facing IceMan, and haven't shouted recently then
	// shout at him!
	if (canShoutAtIceMan()) {
		shout();
		return;
	}

	// Can we track IceMan's location from his cell phone?
	if (m_bCanTrackIceMansCell) {
		unsigned int distance = getStudentWorld()->getPathDistanceToIceMan(getX(), getY());

		if (distance <= m_nIceManCellRange && distance > MAX_SHOUTING_RANGE_UNITS - 1) {
			moveTowardsIceMan();
			return;
		}
	}

	// If we have a line of sight with IceMan, move towards him.
	GraphObject::Direction direction = getDirection();

	if (getStudentWorld()->hasLineOfSightToIceMan(getX(), getY(), direction)) {
		if (getStudentWorld()->getDistanceToIceMan(getX(), getY()) > MAX_SHOUTING_RANGE_UNITS) {
			// Move towards IceMan
			if (!takeOneStep(direction)) {
				cout << "Protester unable to take step in direction " << direction << endl;
			}

			// This will cause us to change direction on subsequent ticks
			// when IceMan is no longer in sight. Otherwise, we'll keep
			// taking a step towards him.
			m_nNumSquaresToMoveInCurrentDirection = 0;
		}

		return;
	}

	m_nNumSquaresToMoveInCurrentDirection--;

	// Check if it's time to change direction
	if (m_nNumSquaresToMoveInCurrentDirection <= 0) {
		// Move in a new random direction. If we're not able to, then pick another direction.
		std::shuffle(begin(m_allDirections), end(m_allDirections), m_randomGenerator);

		for (auto direction : m_allDirections) {
			if (takeOneStep(direction)) {
				// Randomly select a new number of moves [8, 60] to make in this direction 
				m_nNumSquaresToMoveInCurrentDirection = GET_RANDOM_NUM_SQUARES_TO_MOVE();
				return;
			}
		}

		cout << "We have a protester that is stuck due to bounds checking or is blocked on all sides by ice and/or boulders!" << endl;
		return;
	}

	// If we haven't made a perpendicular turn in the last 200 ticks, let's make a turn
	if (getStudentWorld()->getTick() - m_nTickOfLastPerpendicularTurn >= NON_RESTING_TICKS_BETWEEN_TURNING * m_nTicksToWaitBetweenMoves) {
		std::vector<GraphObject::Direction> directions;

		if (getPossiblePerpendicularDirections(directions)) {
			// Pick one at random then go in that direction
			GraphObject::Direction newDirection = directions[rand() % directions.size()];

			setDirection(newDirection);

			m_nNumSquaresToMoveInCurrentDirection = GET_RANDOM_NUM_SQUARES_TO_MOVE();

			// Keep track of when we made our last turn
			m_nTickOfLastPerpendicularTurn = getStudentWorld()->getTick();
		}
	}

	// Try to take a step in our current direction
	if (!takeOneStep(getDirection())) {
		// Unable to take step so change direction on the next non-resting tick
		m_nNumSquaresToMoveInCurrentDirection = 0;
	}

	return;
}

// Determine what perpendicular directions, relative to our current direction, that we could move here?
// This will return 0-2 possible directions by reference depending on bounds checking and if spaces are occupied by
// ice or boulders
bool Protester::getPossiblePerpendicularDirections(std::vector<GraphObject::Direction>& directions) {
	directions.clear();

	const int x = getX();
	const int y = getY();
	const GraphObject::Direction direction = getDirection();

	if (direction == GraphObject::Direction::up ||
		direction == GraphObject::Direction::down ||
		direction == GraphObject::Direction::none) {
		// Check if we can move left or right
		if (!getStudentWorld()->isBlocked(x, y, direction))
			directions.push_back(GraphObject::Direction::left);
		if (!getStudentWorld()->isBlocked(x, y, direction))
			directions.push_back(GraphObject::Direction::right);
	}
	if (direction == GraphObject::Direction::left ||
		direction == GraphObject::Direction::right ||
		direction == GraphObject::Direction::none) {

		// Check if we can move up or down?
		if (!getStudentWorld()->isBlocked(x, y, direction))
			directions.push_back(GraphObject::Direction::down);
		if (!getStudentWorld()->isBlocked(x, y, direction))
			directions.push_back(GraphObject::Direction::up);
	}

	return directions.size() > 0;
}

// Annoy the Protester
void Protester::annoy(int nHitPoints) {
	// If we're leaving, we can't be annoyed any further
	if (isLeaving()) {
		return;
	}

	Person::annoy(nHitPoints);

	// Sound annoyed and stunned for N ticks, where N = m_nStunnedTicks
	getStudentWorld()->playSound(SOUND_PROTESTER_ANNOYED);

	m_nTicksStunned = CALCULATE_STUNNED_TICKS(getStudentWorld()->getLevel());
}

// Protester is bonked by a Boulder
void Protester::bonkedByBoulder() {
	annoy(100);
	// Being hit by a boulder always kills the Protester
	getStudentWorld()->increaseScore(500);
}

void Protester::squirtedByIceMan() {
	annoy(SQUIRT_DAMAGE);
}


// Figure out which direction to move towards the exit 
void Protester::moveTowardsExit() {
	DirectionDistance directionDistance;

	if (!getStudentWorld()->getShortestPathToExit(getX(), getY(), directionDistance) ||
		!takeOneStep(directionDistance.direction)) {
		return;
	}
}

// Figure out which direction to move towards IceMan 
void Protester::moveTowardsIceMan() {
	DirectionDistance directionDistance;

	if (!getStudentWorld()->getShortestPathToIceMan(getX(), getY(), directionDistance) ||
		!takeOneStep(directionDistance.direction)) {
		return;
	}
}

// Take one step in a specified direction
bool Protester::takeOneStep(Direction direction) {
	bool result = true;

	const int x = getX();
	const int y = getY();
	const double size = getSize();

	switch (direction) {
	case Direction::none:
		break;
	case Direction::up:
		if (y < ICE_HEIGHT && !getStudentWorld()->isBlocked(x, y, direction))
			moveTo(x, y + 1);
		else
			result = false;
		break;
	case Direction::down:
		if (y > 0 && !getStudentWorld()->isBlocked(x, y, direction))
			moveTo(x, y - 1);
		else
			result = false;
		break;
	case Direction::left:
		if (x > 0 && !getStudentWorld()->isBlocked(x, y, direction))
			moveTo(x - 1, y);
		else
			result = false;
		break;
	case Direction::right:
		if (x < (VIEW_WIDTH - PERSON_SIZE) && !getStudentWorld()->isBlocked(x, y, direction))
			moveTo(x + 1, y);
		else
			result = false;
		break;
	default:
		result = false;
	}

	// If we were able to make a move, then face that direction 
	if (result) {
		setDirection(direction);
	}

	return result;
}

// Can we shout at IceMan?
bool Protester::canShoutAtIceMan() {
	const unsigned long nTick = getStudentWorld()->getTick();

	// We're skipping the last shouted tick when it's zero so we can keep it as an
	// unsigned and use 0 as an initial value.
	return  getStudentWorld()->isFacingIceMan(getX(), getY(), getDirection()) &&
		(m_nLastShoutedTick == 0 || nTick - m_nLastShoutedTick > (NON_RESTING_TICKS_BETWEEN_SHOUTS * m_nTicksToWaitBetweenMoves)) &&
		getDistanceToIceman() <= MAX_SHOUTING_RANGE_UNITS;
}

// Shout at IceMan
void Protester::shout() {
	// Play sound effect and annoy IceMan
	getStudentWorld()->iceManShoutedAt();

	m_nLastShoutedTick = getStudentWorld()->getTick();
}


/************************************************************************************************/
/*																								*/
/* REGULAR PROTESTER																		    */
/*																								*/
/************************************************************************************************/

// Constructor
RegularProtester::RegularProtester(
	StudentWorld* pStudentWorld,
	int startX,
	int startY)
	: Protester(pStudentWorld,
		IID_PROTESTER,
		startX,
		startY,
		5 /* nHitPoints */,
		false /* bCanTrackIceMansCell */)
{
#if TEST_REGULARPROTESTER
	getStudentWorld()->listenForEvent(
		EventTypes::EVENT_TEST,
		[&](SharedEventPtr pEvent) {
			// TODO: Is capturing "this" safe or could it cause an access violation? 
			// happens if this object is removed before the callback is invoked?

			// This is a little dangerous but it works. Try to find a better way! 
			Event<EventTestData>* pData = (Event<EventTestData>*)pEvent.get();

			this->annoy(5);
			cout << "Event received: RegularProtester leaving the oil field." << endl;RegularProtester
		});
#endif
}

// Destructor
RegularProtester::~RegularProtester() {
}

// Handle a tick
void RegularProtester::doSomething() {
	Protester::doSomething();
}

// Annoy the Protester
void RegularProtester::annoy(int nHitPoints) {
	Protester::annoy(nHitPoints);
}
// Get squirted by IceMan
void RegularProtester::squirtedByIceMan() {
	Protester::squirtedByIceMan();
	if (isAnnoyed())
		getStudentWorld()->increaseScore(SQUIRT_ANNOYED_POINTS_R);

}

void RegularProtester::pickUpGold() {
	StudentWorld* pStudentWorld = getStudentWorld();
	pStudentWorld->playSound(SOUND_PROTESTER_FOUND_GOLD);
	pStudentWorld->increaseScore(PICK_UP_GOLD_POINTS_R);
	leave();
}


/************************************************************************************************/
/*																								*/
/* HARDCORE PROTESTER																		    */
/*																								*/
/************************************************************************************************/

// Calculate the # of ticks to stare at a Gold Nugget,
// where N is the current game level
#define CALCULATE_TICKS_TO_STARE(N) std::max<int>(50, 100 - N * 10)

// Constructor
HardcoreProtester::HardcoreProtester(
	StudentWorld* pStudentWorld,
	int startX,
	int startY)
	: Protester(pStudentWorld,
		IID_HARD_CORE_PROTESTER,
		startX,
		startY,
		20 /* nHitPoints */,
		true /* bCanTrackIceMansCell */)
{
#if TEST_HARDCOREPROTESTER
	getStudentWorld()->listenForEvent(
		EventTypes::EVENT_TEST,
		[&](SharedEventPtr pEvent) {
			// TODO: Is capturing "this" safe or could it cause an access violation? 
			// happens if this object is removed before the callback is invoked?

			// This is a little dangerous but it works. Try to find a better way! 
			Event<EventTestData>* pData = (Event<EventTestData>*)pEvent.get();

			this->annoy(10);
			cout << "Event received: HardcoreProtester leaving the oil field." << endl;
		});
#endif
}

// Destructor
HardcoreProtester::~HardcoreProtester() {
}

// Handle a tick
void HardcoreProtester::doSomething() {
	Protester::doSomething();
}

// Annoy the Protester
void HardcoreProtester::annoy(int nHitPoints) {
	Protester::annoy(nHitPoints);
}

void HardcoreProtester::squirtedByIceMan() {
	Protester::squirtedByIceMan();
	if (isAnnoyed())
		getStudentWorld()->increaseScore(SQUIRT_ANNOYED_POINTS_HC);
}

// Step on a Gold Nugget 
void HardcoreProtester::pickUpGold() {
	StudentWorld* pStudentWorld = getStudentWorld();

	pStudentWorld->playSound(SOUND_PROTESTER_FOUND_GOLD);
	pStudentWorld->increaseScore(PICK_UP_GOLD_POINTS_HC);


	// Stare at Nugget for N ticks
	pause(CALCULATE_TICKS_TO_STARE(getStudentWorld()->getLevel()));
}



/************************************************************************************************/
/*																								*/
/* GOODIE																					    */
/*																								*/
/************************************************************************************************/

Goodie::Goodie(
	StudentWorld* pStudentWorld,
	int imageID,
	int startX,
	int startY,
	Direction dir		/*= right*/,
	double size			/*= 1.0*/,
	unsigned int depth	/*= 0*/,
	bool visible		/*= false*/,
	bool canAnnoy		/*= false*/,
	bool canPickupIM	/*= true*/,
	bool canPickupP		/*false*/,
	bool isPermanent	/*= true*/)
	: Actor(pStudentWorld, imageID, startX, startY, dir, size, depth, visible),
	m_bCollected(false),
	m_bPermanent(isPermanent)
{
	if (!isPermanent)
		m_iTicksRemaining = 0;
};

Goodie::~Goodie() {
}

// Set the permanent status of a Goodie
void Goodie::setPermanentStatus(bool isPermanent) {
	m_bPermanent = isPermanent;
}

// Returns the permanent status of a Goodie
bool Goodie::isPermanent() {
	return m_bPermanent;
}

// Sets the number of ticks a non-permanent Goodie
// must wait before doing something
void Goodie::setTicksRemaining(int ticks) {
	m_iTicksRemaining = ticks;
}

// Returns the number of ticks a Goodie has left
// before doing something
int Goodie::getTicksRemaining() {
	return m_iTicksRemaining;
}

// Decrements the number of ticks remaining
void Goodie::decTicksRemaining() {
	--m_iTicksRemaining;
}

// Sets the collected status
// If a Goodie IS collected, then it is CANNOT be picked up anymore
void Goodie::setCollected(bool isCollected) {
	m_bCollected = isCollected;
	setCanPickupIM(!isCollected);
}

// Returns if a goodie has been collected
bool Goodie::isCollected() {
	return m_bCollected;
}


/************************************************************************************************/
/*																								*/
/* GOLD																						    */
/*																								*/
/************************************************************************************************/

// Constructor
Gold::Gold(
	StudentWorld* pStudentWorld,
	int startX,
	int startY,
	bool visible,
	bool canPickupIM,
	bool canPickupP,
	bool isPermanent)
	: Goodie(pStudentWorld,
		IID_GOLD,
		startX, startY,
		Direction::right,
		GOLD_SIZE	/*size*/,
		GOLD_DEPTH	/*depth*/,
		visible,
		false		/*canAnnoy*/,
		canPickupIM,
		canPickupP,
		isPermanent)

{
	// Permanent status is true when initialized in the ice field
	// Permanent status is false when the IceMan drops a gold nugget
	if (!isPermanent)
		setTicksRemaining(100);

	setCanPickupIM(canPickupIM);
	setCanPickupP(canPickupP);
};

// Destructor
Gold::~Gold() {
}

// Handle tick
void Gold::doSomething() {
	// If gold is not alive, return immediately
	if (!isAlive())
		return;

	// If gold is not permanent, count down its ticks remaining
	// Remove gold if it has expired
	if (!isPermanent()) {
		if (getTicksRemaining() > 0)
			decTicksRemaining();
		else {
			setAlive(false);
		}
	}

	// If within a radius of 4 to IceMan, become visible
	if (!isVisible() && getDistanceToIceman() <= 4) {
		setVisible(true);
		return;
	}

	// Collected by Iceman
	// If can be picked up by IceMan and within a radius of 3
	if (canPickupIM() && getDistanceToIceman() <= 3) {
		collectIM();
		getStudentWorld()->pickupGoodieIM(getID(), getPoints(), getSoundEffect());
		return;
	}

	// Handles when Protester picks up Gold
	if (canPickupP() && getStudentWorld()->pickupGoldP(getX(), getY())) {
		collectP();
		return;
	}
}

void Gold::collect() {
	setAlive(false);
	setCollected(true);
}

void Gold::collectIM() {
	collect();
	setCanPickupIM(false);
	setSoundEffect(SOUND_GOT_GOODIE);
	setPoints(GOLD_ICEMAN_POINTS);
}

void Gold::collectP() {
	collect();
	setCanPickupP(false);
	setSoundEffect(SOUND_PROTESTER_FOUND_GOLD);
}


/************************************************************************************************/
/*																								*/
/* OIL BARREL																				    */
/*																								*/
/************************************************************************************************/

// Constructor
OilBarrel::OilBarrel(StudentWorld* pStudentWorld, int startX, int startY)
	: Goodie(pStudentWorld,
		IID_BARREL,
		startX, startY,
		Direction::right,
		OIL_BARREL_SIZE /*size*/,
		OIL_BARREL_DEPTH /*depth*/,
		false /*visible*/,
		false /*canAnnoy*/,
		true /*canPickupIM*/,
		false /*canPickupP*/,
		true /*isPermanent*/)
{
	setCanPickupIM(true);
};

// Destructor
OilBarrel::~OilBarrel() {
}

// Handle tick
void OilBarrel::doSomething() {
	// Is we're no longer alive, return
	if (!isAlive()) {
		return;
	}
	// Make visible if IceMan is within a radius of 4
	if (!isVisible() && getDistanceToIceman() <= 4) {
		setVisible(true);
		return;
	}
	// Collect if IceMan is within a radius of 3
	if (getDistanceToIceman() <= 3) {
		collect();
		getStudentWorld()->pickupGoodieIM(getID(), getPoints(), getSoundEffect());
		return;
	}
}

// Handles when an oil barrel is collected
void OilBarrel::collect() {
	setAlive(false);
	setCollected(true);
	setSoundEffect(SOUND_FOUND_OIL);
	setPoints(OIL_BARREL_POINTS);
}


/************************************************************************************************/
/*																								*/
/* SONAR KIT																				    */
/*																								*/
/************************************************************************************************/

// Constructor
SonarKit::SonarKit(StudentWorld* pStudentWorld, int startX, int startY, bool isVisible)
	: Goodie(pStudentWorld,
		IID_SONAR,
		startX, startY,
		Direction::right,
		SONAR_KIT_SIZE /*size*/,
		SONAR_KIT_DEPTH /*depth*/,
		true /*visible*/,
		false /*canAnnoy*/,
		true /*canPickupIM*/,
		false /*canPickupP*/,
		false /*isPermanent*/)
{
	// Number of ticks to live T = max(100, 300 - 10 * current_level_number)
	const int level = getStudentWorld()->getLevel();
	setTicksRemaining(max(100, 300 - 10 * level));
};

// Destructor
SonarKit::~SonarKit() {
}

// Handle tick
void SonarKit::doSomething() {
	// Is we're no longer alive, return
	if (!isAlive()) {
		return;
	}
	if (getTicksRemaining() > 0) {
		decTicksRemaining();
	}
	else {
		setAlive(false);
		return;
	}
	// Get collected if if IceMan is within a radius of 3
	if (getDistanceToIceman() <= 3) {
		collect();
		getStudentWorld()->pickupGoodieIM(getID(), getPoints(), getSoundEffect());
		return;
	}
}

// Handles when a sonar kit is collected
void SonarKit::collect() {
	setAlive(false);
	setCollected(true);
	setSoundEffect(SOUND_GOT_GOODIE);
	setPoints(SONAR_KIT_POINTS);
}


/************************************************************************************************/
/*																								*/
/* WATER POOL																				    */
/*																								*/
/************************************************************************************************/

// Constructor
WaterPool::WaterPool(StudentWorld* pStudentWorld, int startX, int startY)
	: Goodie(pStudentWorld,
		IID_WATER_POOL,
		startX, startY,
		Direction::right,
		WATER_POOL_SIZE /*size*/,
		WATER_POOL_DEPTH /*depth*/,
		true /*visible*/,
		false /*canAnnoy*/,
		true /*canPickupIM*/,
		false /*canPickupP*/,
		false /*isPermanent*/)
{
	const int level = getStudentWorld()->getLevel();
	setTicksRemaining(max(100, 300 - 10 * level));
};

// Destructor
WaterPool::~WaterPool() {
}

// Handle tick
void WaterPool::doSomething() {
	// Is we're no longer alive, return
	if (!isAlive()) {
		return;
	}
	// Decrement ticks remaining
	// Remove if ticks have run out
	if (getTicksRemaining() > 0) {
		decTicksRemaining();
	}
	else {
		setAlive(false);
		return;
	}
	// Collect if IceMan is within a radius of 3
	if (getDistanceToIceman() <= 3) {
		collect();
		getStudentWorld()->pickupGoodieIM(getID(), getPoints(), getSoundEffect());
	}
}

// Handles when a water pool is collected
void WaterPool::collect() {
	setAlive(false);
	setCollected(true);
	setSoundEffect(SOUND_GOT_GOODIE);
	setPoints(WATER_POOL_POINTS);
}