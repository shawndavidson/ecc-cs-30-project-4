#include "Goodie.h"


Goodie::Goodie(
	StudentWorld* pStudentWorld,
	int imageID,
	int startX,
	int startY,
	Direction dir /*= right*/,
	double size /*= 1.0*/,
	unsigned int depth /*= 0*/,
	bool visible /*= true*/,
	bool canAnnoy /*= false*/,
	bool canPickup /*= false*/,
	bool isPermanent /*= true*/)
	: Actor(pStudentWorld, imageID, startX, startY, dir, size, depth)
{
};

Goodie::~Goodie() {
}

void Goodie::annoy() {
	return;
}

void Goodie::setPermanentStatus(bool isPermanent) {
	m_permanent = isPermanent;
}

bool Goodie::getPermanentStatus() {
	return m_permanent;
}

void Goodie::setTicksAlive(int ticks) {
	m_ticks_alive = ticks;
}

int Goodie::getTicksAlive() {
	return m_ticks_alive;
}





