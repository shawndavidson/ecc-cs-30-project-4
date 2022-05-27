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
	// TODO: water pools are temporary and only stay on screen for a certain number of ticks
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
	// If IceMan is within a radius of 3 (squared)
	if (getRadiusIceMan() <= 9) {
		collect();
	}
}

// Handles when a water pool is collected
void WaterPool::collect() {
	setAlive(false);
	setCollected(true);
	setVisible(false);
	setSoundEffect(SOUND_GOT_GOODIE);
	setPoints(WATER_POOL_POINTS);
}





