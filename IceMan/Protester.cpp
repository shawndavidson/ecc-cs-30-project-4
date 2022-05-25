#include <algorithm>

#include "Protester.h"
#include "StudentWorld.h"
#include "Ice.h"

/*************************************************************************/
/* Types														     */
/*************************************************************************/
const int           EXIT_POSITION_X = 60;
const int           EXIT_POSITION_Y = 60;
const double        PROTESTER_SIZE = 1.0;
const unsigned int  NON_RESTING_TICKS_BETWEEN_SHOUTS = 15;


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
        PROTESTER_SIZE,
        0 /*depth*/,
        true /*visible*/,
        true /*canAnnoy*/,
        false /*canPickup*/),
    m_nTicksToWaitBetweenMoves(std::max<unsigned int>(0, 3 - getStudentWorld()->getLevel() / 4)),
    m_nLeaveTheOilField(false),
    m_nLastShoutTick(0)
{

}

// Destructor
Protester::~Protester() {
}

// Handle a tick
void Protester::doSomething() {
    // If we're dead or resting, then do nothing...
    const bool isRestingTick = getStudentWorld()->getTick() % m_nTicksToWaitBetweenMoves == 0;
    
    if (!isAlive() || isRestingTick) {
        return;
    }

    // Are we leaving the oil field?
    if (m_nLeaveTheOilField) {
        // Have we reached the exit?
        if (getX() == EXIT_POSITION_X && getY() == EXIT_POSITION_Y) {
            setAlive(false);
        }
        else {
            moveTowardsExit();
        }
        return;
    }

    if (canShout()) {
        shout();
    }
    

    // TODO
}

// Annoy the Protester
void Protester::annoy() {
    // TODO
    cout << "Protester was annoyed" << endl;
}

// Figure out which direction to move towards the exit 
void Protester::moveTowardsExit() {
    Direction direction = getShortestPathToExit();

    if (!move(direction)) {
        cout << "Unable to move the Protester in the direction towards the exit" << endl;
        // throw?
    }
}

// Get direction to move towards exit in shortest path
GraphObject::Direction Protester::getShortestPathToExit() const {
    // TODO: Use maze searching algorithm to move towards exit, 
    // using concurrency, avoiding Ice and Boulders
    return (Direction)(rand() % Direction::right+1);
}

// Take one step in a specified direction
bool Protester::move(Direction direction) {
    bool result = true;

    const int x         = getX();
    const int y         = getY();
    const double size   = getSize();

    switch (direction) {
        case Direction::none:
            break;
        case Direction::up:
            if (y < ICE_HEIGHT)
                moveTo(x, y + 1);
            else
                result = false;
            break;
        case Direction::down:
            if (y > 0)
                moveTo(x, y - 1);
            else
                result = false;
            break;
        case Direction::left:
            if (x > 0)
                moveTo(x - 1, y);
            else
                result = false;
            break;
        case Direction::right:
            if (x < (VIEW_WIDTH - (size * PROTESTER_SIZE / ICE_SIZE)))
                moveTo(x + 1, y);
            else
                result = false;
            break;
        default:
            return false;
    }

    return result;
}

// Can we shout at IceMan?
bool Protester::canShout() {
    // TODO: Return true if we're within 4 units AND we're facing Iceman
    bool isIceManInRange    = true; // TODO
    bool isFacingIceMan     = true; // TODO

    const unsigned long nTick = getStudentWorld()->getTick();

    return  isFacingIceMan &&
            (m_nLastShoutTick > 0 && nTick - m_nLastShoutTick > (NON_RESTING_TICKS_BETWEEN_SHOUTS * m_nTicksToWaitBetweenMoves)) &&
            isIceManInRange;
}

// Shout at IceMan
void Protester::shout() {
    getStudentWorld()->playSound(SOUND_PROTESTER_YELL);
    m_nLastShoutTick = getStudentWorld()->getTick();
}