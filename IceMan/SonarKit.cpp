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
	// Number of ticks to live T = max(100, 300 - 10 * current_level_number)
	const int level = getStudentWorld()->getLevel();
	setTicksRemaining(max(100, 300 - 10 * level));
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





