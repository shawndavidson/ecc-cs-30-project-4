#include "Gold.h"


// Constructor
Gold::Gold(StudentWorld* pStudentWorld,
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
};

// Destructor
Gold::~Gold() {
}

// Handle tick
void Gold::doSomething() {
	// Page 35 has details
	// If gold is not alive, return immediately
	if (!isAlive()) {
		return;
	}
	// If within a radius of 4 to IceMan, become visible (using radius squared)
	if (!isVisible() && getRadiusIceMan() <= 16) {
		setVisible(true);
		return;
	}
	// Collected by Iceman
	// If can be picked up by IceMan and within a radius of 3 (squared), get collected
	if (canPickupIM() && getRadiusIceMan() <= 9) {
		collectIM();
		return;
	}
	
	// TODO: handle being picked up by a protester

	// TODO: Check if lifetime expired

}

void Gold::collect() {
	setAlive(false);
	setCollected(true);
	setVisible(false);
}

void Gold::collectIM() {
	collect();
	setCanPickupIM(false);
	setSoundEffect(SOUND_GOT_GOODIE);
	setPoints(GOLD_ICEMAN_POINTS);
}

void Gold::collectP() {
	setCanPickupP(false);
	setSoundEffect(SOUND_PROTESTER_FOUND_GOLD);
	setPoints(GOLD_PROTESTER_POINTS);
}

void Gold::dropGold() {
	// TODO: this may need to be handled by StudentWorld
}




