#include "Ice.h"

// Constructor
Ice::Ice(int startX, int startY)
	: Actor(IID_ICE, 
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

void Ice::doSomething() {
	// Ice doesn't do anything
}

void Ice::annoy() {
	// You can't annoy ice
}

