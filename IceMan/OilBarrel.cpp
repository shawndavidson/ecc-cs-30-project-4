#include "OilBarrel.h"


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
	// Check if IceMan is within a radius of 4 (using radius squared, so 16)
	// Check page 34 for guidlines
	if (!isVisible() && getRadiusIceMan() <= 16) {
		setVisible(true);
		return;
	}
	// Collect if IceMan is within a radius of 3 (using radius sqaured, so 9)
	if (getRadiusIceMan() <= 9) {
		collect();
		return;
	}
}

// Handles when an oil barrel is collected
void OilBarrel::collect() {
	setAlive(false);
	setCollected(true);
	setVisible(false);
	setSoundEffect(SOUND_FOUND_OIL);
	setPoints(OIL_BARREL_POINTS);
}





