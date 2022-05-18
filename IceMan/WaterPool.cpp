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
		false /*canPickup*/)
{
};

// Destructor
WaterPool::~WaterPool() {
	getGraphObjects(WATER_POOL_DEPTH).erase(this);
}

// Handle tick
void WaterPool::doSomething() {
	//setRadius(); need to figure out how to get the distance from ice man
	// This code just forces the barrel to be visible
	// Delete later
	if (isAlive())
		setVisible(true);
	return;

	// Is we're no longer alive, make ourselves invisible
	if (!isAlive()) {
		setVisible(false);
		return;
	}
	// Check if IceMan is within a radius of 4
	// Check page 34
	//if (!isVisible() && getRadius() <= 4.0) {
	//	setVisible(true);
	//	return;
	//}
	//if (getRadius() <= 3.0) {
	//	
	//}

}

void WaterPool::collect() {
	setAlive(false);
	//playSound(SOUND_FOUND_OIL);
	//increaseScore(OIL_BARREL_POINTS);
	//displayMessage(OIL_BARREL_MESSAGE);
	// Inform StudentWorld that the barrel was picked up?
	// Decrement the number of OilBarrels
}





