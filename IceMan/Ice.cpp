#include "Ice.h"

// Constructor
Ice::Ice(StudentWorld* pStudentWorld, int startX, int startY)
	: Actor(pStudentWorld, 
			IID_ICE, 
		    startX, startY, 
		    Direction::right, 
			ICE_SIZE /*size*/,
		    3 /*depth*/,
		    true /*visible*/,
		    false /*canAnnoy*/,
		    false /*canPickup*/)
{
};

// Destructor
Ice::~Ice() {
}

// Handle tick
void Ice::doSomething() {
	// Is we're no longer alive, make ourselves invisible
	if (!isAlive()) {
		setVisible(false);
	}
}

// Handle Annoy
void Ice::annoy() {
	// You can't annoy ice
}

