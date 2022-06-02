#include "Gold.h"

#include <iostream> // FIXME - for test prints


// Constructor
Gold::Gold(
	StudentWorld* pStudentWorld,
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
	if (!isPermanent)
		setTicksRemaining(100);
	
	// TODO: a bug appears when these lines aren't here,
	// but they shouldn't be necessary
	setCanPickupIM(canPickupIM);
	setCanPickupP(canPickupP);

};

// Destructor
Gold::~Gold() {
}

// Handle tick
void Gold::doSomething() {
	// Page 35 has details
	// If gold is not alive, return immediately
	if (!isAlive())
		return;

	if (!isPermanent()) {
		if (getTicksRemaining() > 0)
			decTicksRemaining();
		else {
			setAlive(false);
			setVisible(false);
		}
	}

	// If within a radius of 4 to IceMan, become visible
	if (!isVisible() && getDistanceToIceman() <= 4) {
		setVisible(true);
		return;
	}
	
	// Collected by Iceman
	// If can be picked up by IceMan and within a radius of 3
	if (canPickupIM() && getDistanceToIceman() <= 3) {
		collectIM();
		getStudentWorld()->pickupGoodieIM(getID(), getPoints(), getSoundEffect());
		return;
	}

	// TODO: implement when a Protester picks up Gold
	// Maybe use a StudentWorld function
	if (canPickupP()) {
		return;
	}
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





