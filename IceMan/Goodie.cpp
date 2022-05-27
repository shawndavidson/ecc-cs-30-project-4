#include "Goodie.h"


Goodie::Goodie(
	StudentWorld* pStudentWorld,
	int imageID,
	int startX,
	int startY,
	Direction dir		/*= right*/,
	double size			/*= 1.0*/,
	unsigned int depth	/*= 0*/,
	bool visible		/*= false*/,
	bool canAnnoy		/*= false*/,
	bool canPickupIM	/*= true*/,
	bool canPikcupP		/*false*/,
	bool isPermanent	/*= true*/)
	: Actor(pStudentWorld, imageID, startX, startY, dir, size, depth),
	m_bCollected(false),
	m_bPermanent(isPermanent)
{
	setCollected(false);
};

Goodie::~Goodie() {
	// ToDo: how to handle this?
}

// Goodies cannot be annoyed
void Goodie::annoy() {
}

// Set the permanent status of a Goodie
void Goodie::setPermanentStatus(bool isPermanent) {
	m_bPermanent = isPermanent;
}

// Returns the permanent status of a Goodie
bool Goodie::getPermanentStatus() {
	return m_bPermanent;
}

// Sets the number of ticks a non-permanent Goodie will be alive
void Goodie::setTicksAlive(int ticks) {
	m_iTicksAlive = ticks;
}

// Returns the number of ticks a Goodie will be alive
// This is NOT how many ticks it has left to live,
// but is the total number of ticks it will live
int Goodie::getTicksAlive() {
	return m_iTicksAlive;
}

// Sets the collected status
// If a Goodie IS collected, then it is CANNOT be picked up anymore
void Goodie::setCollected(bool isCollected) {
	m_bCollected = isCollected;
	setCanPickupIM(!isCollected);
}

// Returns if a goodie has been collected
bool Goodie::isCollected() {
	return m_bCollected;
}










