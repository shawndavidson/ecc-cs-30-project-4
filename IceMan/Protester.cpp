#include "Protester.h"

// Constructor
Protester::Protester(
    StudentWorld* pStudentWorld,
    int imageID,
	int startX,
	int startY)
    : Actor(pStudentWorld,
        imageID,
        startX,
        startY,
        Direction::left,
        1.0 /*size*/,
        0 /*depth*/,
        true /*visible*/,
        true /*canAnnoy*/,
        false /*canPickup*/)
{
}

// Destructor
Protester::~Protester() {
}

// Handle a tick
void Protester::doSomething() {
    // TODO
}

// Annoy the Protester
void Protester::annoy() {
    // TODO
}

