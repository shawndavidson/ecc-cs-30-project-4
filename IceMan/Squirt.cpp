#include "Squirt.h"
#include "StudentWorld.h"


// Constructor
Squirt::Squirt(StudentWorld* pStudentWorld,
	int startX,			/*IceMan's X*/
	int startY,			/*IceMan's Y*/
	Direction dir)		/*IceMan's direction*/
	: Actor(pStudentWorld,
		IID_WATER_SPURT,
		startX, startY,
		dir,
		SQUIRT_SIZE		/*size*/,
		SQUIRT_DEPTH	/*depth*/,
		true,			/*visible*/
		false			/*canAnnoy*/,
		false			/*canPickupIM*/,
		false			/*canPickupP*/),
	m_iTravelDist(4)
{
	setSoundEffect(SOUND_PLAYER_SQUIRT);
};

// Destructor
Squirt::~Squirt() {
}

// Handle tick
void Squirt::doSomething() {
	// Page 33 has details
	// If squirt is not alive, return immediately
	if (!isAlive()) {
		return;
	}
	
	// Handles if Squirt hit something
	if (getStudentWorld()->StudentWorld::hitBySquirt(getX(), getY())) {
		setAlive(false);
		return;
	}

	// Else, squirt moves forward
	if (m_iTravelDist >= 1)
	{
		--m_iTravelDist;
		const int x = getX();
		const int y = getY();
		const Direction dir = getDirection();
		if (dir == Direction::left && x > 0) {
			moveTo(x - 1, y);
		}
		else if (dir == Direction::right && x < 60) {
			moveTo(x + 1, y);
		}
		else if (dir == Direction::down && y > 0) {
			moveTo(x, y - 1);
		}
		else if (dir == Direction::up && y < 60) {
			moveTo(x, y + 1);
		}
		return;
	}
	else {
		setAlive(false);
	}
}







