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
		false /*canPickup*/,
		false /*isPermanent*/)
{
	// Must set ticksAlive to
	// T = max(100, 300 - 10 * current_level_number)
};

// Destructor
SonarKit::~SonarKit() {
	getGraphObjects(SONAR_KIT_DEPTH).erase(this);
}

// Handle tick
void SonarKit::doSomething() {
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
	// Check page 36
	//if (!isVisible() && getRadius() <= 4.0) {
	//	setVisible(true);
	//	return;
	//}
	//if (getRadius() <= 3.0) {
	//	
	//}

}

void SonarKit::collect() {
	setAlive(false);
	//playSound(SOUND_FOUND_OIL);
	//increaseScore(OIL_BARREL_POINTS);
	//displayMessage(OIL_BARREL_MESSAGE);
	// Inform StudentWorld that the barrel was picked up?
	// Decrement the number of OilBarrels
}





