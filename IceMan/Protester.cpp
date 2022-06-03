#include <algorithm>
#include <assert.h>

#include "Protester.h"
#include "StudentWorld.h"
#include "Ice.h"

/*************************************************************************/
/* Macros														        */
/*************************************************************************/
// Get the random # of squares to move in the current direction
#define GET_RANDOM_NUM_SQUARES_TO_MOVE()    ((int)8 + (rand() % (60 - 8 + 1)))

// Calculated # of ticks to be stunned for when annoyed
#define CALCULATE_STUNNED_TICKS(GAME_LEVEL) (std::max<size_t>({ 50, 100 - GAME_LEVEL * 10 }))

/*************************************************************************/
/* Types														     */
/*************************************************************************/
const int           EXIT_POSITION_X                     = 60;
const int           EXIT_POSITION_Y                     = 60;
const double        PROTESTER_SIZE                      = 1.0;
const unsigned int  NON_RESTING_TICKS_BETWEEN_SHOUTS    = 15;
const int           MAX_SHOUTING_RANGE_UNITS            = 4; 
const int           NON_RESTING_TICKS_BETWEEN_TURNING   = 200;

/*************************************************************************/
/* Static Members														 */
/*************************************************************************/
std::random_device  Protester::m_randomDevice;
std::mt19937        Protester::m_randomGenerator = std::mt19937(m_randomDevice());

// Constructor
Protester::Protester(
    StudentWorld* pStudentWorld,
    int imageID,
	int startX,
	int startY,
    int nHitPoints,
    bool bCanTrackIceMansCell)
: Person(pStudentWorld,
    imageID,
    startX,
    startY,
    Direction::left,
    nHitPoints),
    m_nTicksToWaitBetweenMoves(std::max<unsigned int>(0, 3 - getStudentWorld()->getLevel() / 4)),
    m_nLeaveTheOilField(false),
    m_nLastShoutedTick(0),
    m_nNumSquaresToMoveInCurrentDirection(GET_RANDOM_NUM_SQUARES_TO_MOVE()),
    m_nTickOfLastPerpendicularTurn(0),
    m_allDirections{ /*Direction::none,*/ Direction::up, Direction::down, Direction::left, Direction::right},
    m_nTicksStunned(0),
    m_nIceManCellRange(16 + getStudentWorld()->getLevel() * 2), // know as M on pg. 45
    m_bCanTrackIceMansCell(bCanTrackIceMansCell)
{
}

// Destructor
Protester::~Protester() {
}

// Handle a tick
void Protester::doSomething() {
    if (!isAlive())
        return;

    // If we're fully annoyed, give up by exiting the oil field
    if (!m_nLeaveTheOilField && isAnnoyed()) {
        leave();

        getStudentWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
        m_nTicksStunned = 0;
    }

    // If we're stunned, then rest for N ticks...
    if (m_nTicksStunned > 0) {
        m_nTicksStunned--;
        return;
    }

    // Rest to give the user a chance to react in human-speed
    if (!m_nLeaveTheOilField &&
        getStudentWorld()->getTick() % m_nTicksToWaitBetweenMoves == 0) {
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

    // If we are within shouting distance, facing IceMan, and haven't shouted recently then
    // shout at him!
    if (canShoutAtIceMan()) {
        shout();
        return;
    }

    // If we have a line of sight with IceMan, move towards him.
    Direction direction = getDirection();

    if (getStudentWorld()->hasLineOfSightToIceMan(getX(), getY(), direction)) {
        if (getStudentWorld()->getDistanceToIceMan(getX(), getY()) > MAX_SHOUTING_RANGE_UNITS) {
            // Move towards IceMan
            if (!takeOneStep(direction)) {
                cout << "Protester unable to take step in direction " << direction << endl;
            }

            // This will cause us to change direction on subsequent ticks
            // when IceMan is no longer in sight. Otherwise, we'll keep
            // taking a step towards him.
            m_nNumSquaresToMoveInCurrentDirection = 0;
        }

        return;
    }

    // Can we track IceMan's location from his cell phone?
    if (m_bCanTrackIceMansCell) {
        if (getStudentWorld()->getPathDistanceToIceMan(getX(), getY()) <= m_nIceManCellRange) {
            moveTowardsIceMan();
            return;
        }
    }

    m_nNumSquaresToMoveInCurrentDirection--;

    // Check if it's time to change direction
    if (m_nNumSquaresToMoveInCurrentDirection <= 0) {
        // Move in a new random direction. If we're not able to, then pick another direction.
        std::shuffle(begin(m_allDirections), end(m_allDirections), m_randomGenerator);

        for (auto direction : m_allDirections) {
            if (takeOneStep(direction)) {
                // Randomly select a new number of moves [8, 60] to make in this direction 
                m_nNumSquaresToMoveInCurrentDirection = GET_RANDOM_NUM_SQUARES_TO_MOVE();
                return;
            }
        }

        cout << "We have a protester that is stuck due to bounds checking or is blocked on all sides by ice and/or boulders!" << endl;
        return;
    }

    // If we haven't made a perpendicular turn in the last 200 ticks, let's make a turn
    if (getStudentWorld()->getTick() - m_nTickOfLastPerpendicularTurn >= NON_RESTING_TICKS_BETWEEN_TURNING * m_nTicksToWaitBetweenMoves) {
        std::vector<Direction> directions;

        if (getPossiblePerpendicularDirections(directions)) {
            // Pick one at random then go in that direction
            Direction newDirection = directions[rand() % directions.size()];

            setDirection(newDirection);

            m_nNumSquaresToMoveInCurrentDirection = GET_RANDOM_NUM_SQUARES_TO_MOVE();

            // Keep track of when we made our last turn
            m_nTickOfLastPerpendicularTurn = getStudentWorld()->getTick();
        }
    }

    // Try to take a step in our current direction
    if (!takeOneStep(getDirection())) {
        // Unable to take step so change direction on the next non-resting tick
        m_nNumSquaresToMoveInCurrentDirection = 0;
    }

    return;
}

// Determine what perpendicular directions, relative to our current direction, that we could move here?
// This will return 0-2 possible directions by reference depending on bounds checking and if spaces are occupied by
// ice or boulders
bool Protester::getPossiblePerpendicularDirections(std::vector<GraphObject::Direction>& directions) {
    directions.clear();

    const int x = getX();
    const int y = getY();
    const Direction direction = getDirection();

    if (direction == Direction::up    || 
        direction == Direction::down  || 
        direction == Direction::none) {
        // Check if we can move left or right
        if (!getStudentWorld()->isBlocked(x - 1, y, direction))
            directions.push_back(Direction::left);
        if (!getStudentWorld()->isBlocked(x + 1, y, direction))
            directions.push_back(Direction::right);
    }
    if (direction == Direction::left  || 
        direction == Direction::right || 
        direction == Direction::none) {

        // Check if we can move up or down?
        if (!getStudentWorld()->isBlocked(x, y - 1, direction))
            directions.push_back(Direction::down);
        if (!getStudentWorld()->isBlocked(x, y + 1, direction))
            directions.push_back(Direction::up);
    }

    return directions.size() > 0;
}

// Annoy the Protester
void Protester::annoy(int nHitPoints) {
    // If we're leaving, we can't be annoyed any further
    if (isLeaving()) {
        return;
    }

    Person::annoy(nHitPoints);

    // Sound annoyed and stunned for N ticks, where N = m_nStunnedTicks
    getStudentWorld()->playSound(SOUND_PROTESTER_ANNOYED);

    m_nTicksStunned = CALCULATE_STUNNED_TICKS(getStudentWorld()->getLevel());
}

// Bonk the Protester with a Boulder
// TODO: resolve in merge
/*
void Protester::boulderBonk(int nHitPoints) {
    annoy(nHitPoints);
    getStudentWorld()->increaseScore(500);
}
*/

// Figure out which direction to move towards the exit 
void Protester::moveTowardsExit() {
    DirectionDistance directionDistance;

    if (!getStudentWorld()->getShortestPathToExit(getX(), getY(), directionDistance) || 
        !takeOneStep(directionDistance.direction)) {
        cout << "Unable to move the Protester in the direction towards the exit" << endl;
        // throw?
    }
}

// Figure out which direction to move towards IceMan 
void Protester::moveTowardsIceMan() {
    DirectionDistance directionDistance;

    if (!getStudentWorld()->getShortestPathToIceMan(getX(), getY(), directionDistance) ||
        !takeOneStep(directionDistance.direction)) {
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
            cout << "Protester::takeOneStep Direction is none" << endl;
            break;
        case Direction::up:
            if (y < ICE_HEIGHT && !getStudentWorld()->isBlocked(x, y + 1, direction))
                moveTo(x, y + 1);
            else
                result = false;
            break;
        case Direction::down:
            if (y > 0 && !getStudentWorld()->isBlocked(x, y - 1, direction))
                moveTo(x, y - 1);
            else
                result = false;
            break;
        case Direction::left:
            if (x > 0 && !getStudentWorld()->isBlocked(x - 1, y, direction))
                moveTo(x - 1, y);
            else
                result = false;
            break;
        case Direction::right:
            if (x < (VIEW_WIDTH - PERSON_SIZE) && !getStudentWorld()->isBlocked(x + 1, y, direction))
                moveTo(x + 1, y);
            else
                result = false;
            break;
        default:
            cout << "Protester attempted to move in an invalid direction" << endl;
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

