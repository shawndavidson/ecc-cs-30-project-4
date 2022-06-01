#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

// Constructor
Actor::Actor(
	StudentWorld* pStudentWorld,
	int imageID,
	int startX,
	int startY,
	Direction dir /*= right*/,
	double size /*= 1.0*/,
	unsigned int depth /*= 0*/,
	bool visible /*= true*/,
	bool canAnnoy /*= false*/,
	bool canPickup /*= false*/)
	: GraphObject(imageID, startX, startY, dir, size, depth),
	m_bAlive(true),
	m_bCanAnnoy(canAnnoy),
	m_bCanPickup(canPickup),
	m_pStudentWorld(pStudentWorld)
{
	setVisible(visible);
}

// Destructor
Actor::~Actor()
{
}

// Check if we can be picked up by another Actor? 
bool Actor::canPickup() const {
	return m_bCanPickup;
}

// Check if we can be annoyed by another Actor? 
bool Actor::canAnnoy() const {
	return m_bCanAnnoy;
}

// Check if we are still alive
bool Actor::isAlive() const {
	return m_bAlive;
}

// Update our alive status
void Actor::setAlive(bool alive) {
	m_bAlive = alive;
}

int Actor::getDistanceToIceman() {
	return getStudentWorld()->getDistanceToIceMan(getX(), getY());
}
