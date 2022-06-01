#include <algorithm>
#include <assert.h>

#include "Protester.h"
#include "StudentWorld.h"
#include "Ice.h"

/*************************************************************************/
/* Types														     */
/*************************************************************************/
const int           EXIT_POSITION_X                     = 60;
const int           EXIT_POSITION_Y                     = 60;
const double        PROTESTER_SIZE                      = 1.0;
const unsigned int  NON_RESTING_TICKS_BETWEEN_SHOUTS    = 15;
const int           MAX_SHOUTING_RANGE_UNITS            = 4*4; // 4 units * ICEMAN_SIZE
const int           NON_RESTING_TICKS_BETWEEN_TURNING   = 200;

/*************************************************************************/
/* Static Members														     */
/*************************************************************************/
std::random_device Protester::m_randomDevice;
std::mt19937 Protester::m_randomGenerator = std::mt19937(m_randomDevice());

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
    m_nLastShoutedTick(0),
    m_nNumSquaresToMoveInCurrentDirection(getNumSquaresToMoveInCurrentDirection()),
    m_nTickOfLastPerpendicularTurn(0),
    m_allDirections{ Direction::none, Direction::up, Direction::down, Direction::left, Direction::right }
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
    
    // If we within shouting distance, facing IceMan, and haven't shouted recently then
    // shout at him!
    if (canShoutAtIceMan()) {
        shout();
        return;
    }
    
    // If we have a line of sight with IceMan, move towards him.
    {
        Direction direction = this->getDirection();

        if (getStudentWorld()->hasPathToIceMan(getX(), getY(), direction) &&
            getStudentWorld()->getDistanceToIceMan(getX(), getY()) <= MAX_SHOUTING_RANGE_UNITS) {
            // Move towards IceMan
            if (!takeOneStep(direction)) {
                cout << "Protester unable to take step in direction " << direction << endl;
            }

            // This will cause us to change direction on subsequent ticks
            // when IceMan is no longer in sight. Otherwise, we'll keep
            // taking a step towards him.
            m_nNumSquaresToMoveInCurrentDirection = 0;
            return;
        }
    }

    // Check if it's time to change direction
    m_nNumSquaresToMoveInCurrentDirection--;

    if (m_nNumSquaresToMoveInCurrentDirection <= 0) {
        // Move in a new random direction. If we're not able to, then pick another direction.
        std::shuffle(begin(m_allDirections), end(m_allDirections), m_randomGenerator);

        for (auto direction : m_allDirections) {
            if (takeOneStep(direction)) {
                // Randomly select a new number of moves [8, 60] to make in this direction 
                m_nNumSquaresToMoveInCurrentDirection = getNumSquaresToMoveInCurrentDirection();
                return;
            }
        }

        cout << "We have a protester that is stuck due to bounds checking or is blocked on all sides by ice and/or boulders!" << endl;
        return;
    }

    // If we haven't made a perpendicular turn in the last 200 ticks, let's make a turn
    if (m_nTickOfLastPerpendicularTurn > 0 &&
        m_nTickOfLastPerpendicularTurn - getStudentWorld()->getTick() >= NON_RESTING_TICKS_BETWEEN_TURNING * m_nTicksToWaitBetweenMoves) {
        std::vector<Direction> directions;

        if (getPossiblePerpendicularDirections(directions)) {
            // Pick one at random then go in that direction
            Direction newDirection = directions[rand() % directions.size()];

            setDirection(newDirection);

            m_nNumSquaresToMoveInCurrentDirection = getNumSquaresToMoveInCurrentDirection();

            // Keep track of when we made our last turn
            m_nTickOfLastPerpendicularTurn = getStudentWorld()->getTick();
        }
    }

    // Try to take a step in our current direction
    if (!takeOneStep(getDirection())) {
        // Unable to take step so change direction on the next non-resting tick
        m_nNumSquaresToMoveInCurrentDirection = 0;
        return;
    }

    return;
}

// Get the number of squares to move in the current direction
int Protester::getNumSquaresToMoveInCurrentDirection() const {
    return 8 + (rand() % (60 - 8 + 1));
}

// Determine what perpendicular directions, relative to our current direction, that we could move here?
// This will return 0-2 possible directions by reference depending on bounds checking and if spaces are occupied by
// ice or boulders
bool Protester::getPossiblePerpendicularDirections(std::vector<GraphObject::Direction>& directions) {
    directions.clear();

    const int x = getX();
    const int y = getY();

    switch (getDirection()) {
        case Direction::up:
        case Direction::down:
            // Check if we can move left or right
            if (!getStudentWorld()->isBlocked(x - 1, y))
                directions.push_back(Direction::left);
            if (!getStudentWorld()->isBlocked(x + 1, y))
                directions.push_back(Direction::right);
            break;
        case Direction::left:
        case Direction::right:
            // Check if we can move up or down?
            if (!getStudentWorld()->isBlocked(x, y - 1))
                directions.push_back(Direction::down);
            if (!getStudentWorld()->isBlocked(x, y + 1))
                directions.push_back(Direction::up);
            break;
        case Direction::none:
        default:
            break;
    }

    return directions.size() > 0;
}

// Annoy the Protester
void Protester::annoy() {
    // TODO
    cout << "Protester was annoyed" << endl;
}

// Figure out which direction to move towards the exit 
void Protester::moveTowardsExit() {
    Direction direction = getStudentWorld()->getShortestPathToExit(getX(), getY());

    if (!takeOneStep(direction)) {
        cout << "Unable to move the Protester in the direction towards the exit" << endl;
        // throw?
    }
}

// Figure out which direction to move towards IceMan 
void Protester::moveTowardsIceMan() {
    Direction direction = getStudentWorld()->getShortestPathToIceMan(getX(), getY());

    if (!takeOneStep(direction)) {
        cout << "Unable to move the Protester in the direction towards IceMan" << endl;
        // throw?
    }
}

// Take one step in a specified direction
bool Protester::takeOneStep(Direction direction) {
    bool result = true;

    const int x         = getX();
    const int y         = getY();
    const double size   = getSize();

    switch (direction) {
        case Direction::none:
            break;
        case Direction::up:
            if (y < ICE_HEIGHT && !getStudentWorld()->isBlocked(x, y + 1))
                moveTo(x, y + 1);
            else
                result = false;
            break;
        case Direction::down:
            if (y > 0 && !getStudentWorld()->isBlocked(x, y - 1))
                moveTo(x, y - 1);
            else
                result = false;
            break;
        case Direction::left:
            if (x > 0 && !getStudentWorld()->isBlocked(x - 1, y))
                moveTo(x - 1, y);
            else
                result = false;
            break;
        case Direction::right:
            if (x < (VIEW_WIDTH - (size * PROTESTER_SIZE / ICE_SIZE)) && !getStudentWorld()->isBlocked(x + 1, y))
                moveTo(x + 1, y);
            else
                result = false;
            break;
        default:
            assert(!"Invalid direction");
            result = false;
    }

    // If we were able to make a move, then face that direction 
    if (result) {
        setDirection(direction);
    }

    return result;
}

// Can we shout at IceMan?
bool Protester::canShoutAtIceMan() {
    const unsigned long nTick = getStudentWorld()->getTick();

    // We're skipping the last shouted tick when it's zero so we can keep it as an
    // unsigned and use 0 as an initial value.
    return  getStudentWorld()->isFacingIceMan(getX(), getY(), getDirection()) &&
            (m_nLastShoutedTick == 0 || nTick - m_nLastShoutedTick > (NON_RESTING_TICKS_BETWEEN_SHOUTS * m_nTicksToWaitBetweenMoves)) &&
            getDistanceToIceman() <= MAX_SHOUTING_RANGE_UNITS;
}

// Shout at IceMan
void Protester::shout() {
    getStudentWorld()->playSound(SOUND_PROTESTER_YELL);

    // TODO: Annoy IceMan

    m_nLastShoutedTick = getStudentWorld()->getTick();
}

