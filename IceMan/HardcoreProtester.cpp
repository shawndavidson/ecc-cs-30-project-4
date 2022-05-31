#include "HardcoreProtester.h"
#include "StudentWorld.h"

// Constructor
HardcoreProtester::HardcoreProtester(
    StudentWorld* pStudentWorld,
    int startX,
    int startY)
    : Protester(pStudentWorld,
        IID_HARD_CORE_PROTESTER,
        startX,
        startY)
{
}

// Destructor
HardcoreProtester::~HardcoreProtester() {
}

// Handle a tick
void HardcoreProtester::doSomething() {
    // TODO
    // If we're dead or resting, then do nothing...
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
void HardcoreProtester::annoy() {
    Protester::annoy();
    // TODO
}
