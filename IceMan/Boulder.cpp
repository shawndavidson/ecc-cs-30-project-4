#include "Boulder.h"
#include "StudentWorld.h"

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
			setVisible(false);
			return;
		}
		
		// Move down 1 tick until
		// a. hits bottom of ice field
		// b. hits top of another boulder or
		// c. hits Ice
		// Then set state dead
		if (!goundUnderBoulder)
			moveTo(getX(), getY() - 1);
		else {
			setAlive(false);
			setVisible(false);
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

