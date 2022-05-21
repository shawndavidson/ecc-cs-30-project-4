#include "RegularProtester.h"
#include "StudentWorld.h"
#include "Event.h"

// Constructor
RegularProtester::RegularProtester(
    StudentWorld* pStudentWorld,
    int startX,
    int startY)
    : Protester(pStudentWorld,
        IID_PROTESTER,
        startX,
        startY)
{
    // TODO: Remove - for testing
    getStudentWorld()->listenForEvent(
        Event::Types::EVENT_TEST,
        [&]() { 
            // TODO: Is capturing "this" safe or could it create a memory leak? Consider
            // using a shared_pointer
            this->handleEvent(); 
        });
}

// Destructor
RegularProtester::~RegularProtester() {
}

// Handle a tick
void RegularProtester::doSomething() {
    // TODO
}

// Annoy the Protester
void RegularProtester::annoy() {
    // TODO
}

// Handle an Event
void RegularProtester::handleEvent() {
    std::cout << "Reached RegularProtester::handleEvent() at tick "
        << getStudentWorld()->getTick() << "!" << std::endl;
}