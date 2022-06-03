#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

// Constructor
Actor::Actor(
	StudentWorld* pStudentWorld,
	int imageID,
	int startX,
	int startY,
	Direction dir		/*= right*/,
	double size			/*= 1.0*/,
	unsigned int depth	/*= 0*/,
	bool visible		/*= true*/,
	bool canAnnoy		/*= false*/,
	bool canPickupIM	/*= false*/,
	bool canPickupP		/*= false*/)
	: GraphObject(imageID, startX, startY, dir, size, depth),
	m_bAlive(true),
	m_bCanAnnoy(canAnnoy),
	m_bCanPickupIM(canPickupIM),
	m_bCanPickupP(canPickupP),
	m_pStudentWorld(pStudentWorld)
{
	setAlive(true);
	setVisible(visible);
}

// Destructor
Actor::~Actor()
{
}

// Check if we can be picked up by another Actor? 
bool Actor::canPickupIM() {
	return m_bCanPickupIM;
}

void Actor::setCanPickupIM(bool collectable) {
	m_bCanPickupIM = collectable;
}

bool Actor::canPickupP() {
	return m_bCanPickupP;
}

void Actor::setCanPickupP(bool collectable) {
	m_bCanPickupP = collectable;
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

<<<<<<< HEAD
// Get a pointer to StudentWorld
StudentWorld* Actor::getStudentWorld() 
{ 
	return m_pStudentWorld; 
}

void Actor::setPoints(int points) {
	m_iPoints = points;
}

int Actor::getPoints() {
	return m_iPoints;
}

void Actor::setSoundEffect(int s_e) {
	m_iSoundEffect = s_e;
}

int Actor::getSoundEffect() {
	return m_iSoundEffect;
}

=======
>>>>>>> main
int Actor::getDistanceToIceman() {
	return getStudentWorld()->getDistanceToIceMan(getX(), getY());
}
