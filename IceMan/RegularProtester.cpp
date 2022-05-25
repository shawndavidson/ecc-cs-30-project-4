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
        EventTypes::EVENT_TEST,
        [&](SharedEventPtr pEvent) {
            // TODO: Is capturing "this" safe or could it cause an access violation? 
            // happens if this object is removed before the callback is invoked?
 
            // This is a little dangerous but it works. Try to find a better way! 
            Event<Data>* pData = (Event<Data>*)pEvent.get();

            this->handleTestEvent(pData->getData().num, pData->getData().text); 
        });
}

// Destructor
RegularProtester::~RegularProtester() {
}

// Handle a tick
void RegularProtester::doSomething() {
    Protester::doSomething();

    // TODO
}

// Annoy the Protester
void RegularProtester::annoy() {
    Protester::annoy();
    // TODO
}

// Handle an Event
void RegularProtester::handleTestEvent(int num, const char* text) {
    std::cout << "Tick: " << getStudentWorld()->getTick() 
        << ", RegularProtester::handleTestEvent, num: " << num 
        << ", text: " << text 
        << std::endl;
}