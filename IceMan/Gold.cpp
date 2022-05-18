#include "Gold.h"


// Constructor
Gold::Gold(StudentWorld* pStudentWorld, int startX, int startY, bool visible, bool isPermanent)
	: Goodie(pStudentWorld,
		IID_GOLD,
		startX, startY,
		Direction::right,
		GOLD_SIZE /*size*/,
		GOLD_DEPTH /*depth*/,
		visible /*visible*/,
		false /*canAnnoy*/,
		false /*canPickup*/,
		isPermanent)
{
	// Permanent status is true when initialized in the ice field
	// Permanent status is false when the IceMan drops a gold nugget
};

// Destructor
Gold::~Gold() {
	getGraphObjects(GOLD_DEPTH).erase(this);
}

// Handle tick
void Gold::doSomething() {
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
	 //Check if IceMan is within a radius of 4
	 //Check page 35

	/*
	if (!isVisible() && getRadius() <= 4.0) {
		setVisible(true);
		return;
	}
	// If IceMan can collect
	if (iceManCanCollect() && getRadius() <= 3.0) {
		setAlive(false);
		// play SOUND_GOT_GOODIE
		//increaseScore(GOLD_POINTS);
		// Must update IceMan's gold inventory by 1
		// Inform StudentWorld that the barrel was picked up?
	}
	// If Protestor can collect
	if (protesterCanCollect() && getRadius() <= 3.0) {
		setAlive(false);
		// play SOUND_PROTESTER_FOUND_GOLD
		// increase score by 25
	}
	*/
	




}

void Gold::collect() {
	setAlive(false);
	//playSound(SOUND_GOT_GOODIE);
	
}




