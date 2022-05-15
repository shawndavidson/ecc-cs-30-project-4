#include "Ice.h"

// Constructor
Ice::Ice(StudentWorld* pStudentWorld, int startX, int startY)
	: Actor(pStudentWorld, 
			IID_ICE, 
		    startX, startY, 
		    Direction::right, 
		    0.25 /*size*/,
		    3 /*depth*/,
		    true /*visible*/,
		    false /*canAnnoy*/,
		    false /*canPickup*/)
{
};

// Destructor
Ice::~Ice() {
}

// Handle Annoy
void Ice::doSomething() {
	// Ice doesn't do anything
}

// Handle Annoy
void Ice::annoy() {
	// You can't annoy ice
}

