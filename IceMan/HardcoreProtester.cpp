#include "HardcoreProtester.h"

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
}

// Annoy the Protester
void HardcoreProtester::annoy() {
    // TODO
}
