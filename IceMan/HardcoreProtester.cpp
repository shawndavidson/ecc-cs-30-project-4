#include "HardcoreProtester.h"
#include "StudentWorld.h"

// Calculate the # of ticks to stare at a Gold Nugget,
// where N is the current game level
#define CALCULATE_TICKS_TO_STARE(N) std::max<int>(50, 100 - N * 10)

// Constructor
HardcoreProtester::HardcoreProtester(
    StudentWorld* pStudentWorld,
    int startX,
    int startY)
    : Protester(pStudentWorld,
        IID_HARD_CORE_PROTESTER,
        startX,
        startY,
        20 /* nHitPoints */),
    m_nTicksToStare(0)
{
}

// Destructor
HardcoreProtester::~HardcoreProtester() {
}

// Handle a tick
void HardcoreProtester::doSomething() {
    // Are we staring at a Gold Nugget?
    if (m_nTicksToStare > 0) {
        m_nTicksToStare--;
        return;
    }
    else {
        m_nTicksToStare = 0;
    }

    // If we're dead or resting, then do nothing...
    // TODO: fix resting tick calculation
    const bool isRestingTick = getStudentWorld()->getTick() % 1000 == 0;

    if (!isAlive() || isRestingTick) {
        return;
    }

    Direction dir = getStudentWorld()->getShortestPathToIceMan(getX(), getY());
    if (!takeOneStep(dir)) {
        cout << "HardcoreProtester unable to take one step towards IceMan" << endl;
    }
}

// Annoy the Protester
void HardcoreProtester::annoy(int nHitPoints) {
    Protester::annoy(nHitPoints);
}

// Step on a Gold Nugget 

void HardcoreProtester::pickUpGold() {
    StudentWorld* pStudentWorld = getStudentWorld();

    pStudentWorld->playSound(SOUND_PROTESTER_FOUND_GOLD);
    pStudentWorld->increaseScore(50);
    
    
    // Stare at Nugget for N ticks
    m_nTicksToStare = CALCULATE_TICKS_TO_STARE(getStudentWorld()->getLevel());
}
