#include "SonarKit.h"


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
	// Must set ticksAlive to
	// Number of ticks to live T = max(100, 300 - 10 * current_level_number)
};

// Destructor
SonarKit::~SonarKit() {
}

// Handle tick
void SonarKit::doSomething() {
	// TODO: if tick counter has run out, set state to dead

	// Is we're no longer alive, return
	if (!isAlive()) {
		return;
	}
	// Check if IceMan is within a radius of 3 (using squared radius so 9)
	// Check page 37 for guidlines
	if (getRadiusIceMan() <= 9) {
		collect();
		return;
	}

}

// Handles when a sonar kit is collected
void SonarKit::collect() {
	setAlive(false);
	setCollected(true);
	setVisible(false);
	setSoundEffect(SOUND_GOT_GOODIE);
}





