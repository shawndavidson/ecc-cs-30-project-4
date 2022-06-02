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
	bool canPickupP		/*false*/,
	bool isPermanent	/*= true*/)
	: Actor(pStudentWorld, imageID, startX, startY, dir, size, depth, visible),
	m_bCollected(false),
	m_bPermanent(isPermanent)
{
	if (!isPermanent)
		m_iTicksRemaining = 0;
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
bool Goodie::isPermanent() {
	return m_bPermanent;
}

// Sets the number of ticks a non-permanent Goodie
// must wait before doing something
void Goodie::setTicksRemaining(int ticks) {
	m_iTicksRemaining = ticks;
}

// Returns the number of ticks a Goodie has left
// before doing something
int Goodie::getTicksRemaining() {
	return m_iTicksRemaining;
}

// Decrements the number of ticks remaining
void Goodie::decTicksRemaining() {
	--m_iTicksRemaining;
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










