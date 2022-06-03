#include "WaterPool.h"


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
	if (getTicksRemaining() > 0) {
		decTicksRemaining();
	}
	else {
		setAlive(false);
		return;
	}
	// If IceMan is within a radius of 3
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





