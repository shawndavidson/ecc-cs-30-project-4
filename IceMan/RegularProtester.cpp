#include "RegularProtester.h"
#include "StudentWorld.h"
#include "Event.h"

#include <algorithm>


// Constructor
RegularProtester::RegularProtester(
    StudentWorld* pStudentWorld,
    int startX,
    int startY)
    : Protester(pStudentWorld,
        IID_PROTESTER,
        startX,
        startY,
        5 /* nHitPoints */,
        false /* bCanTrackIceMansCell */)
{
#if TEST_REGULARPROTESTER
    getStudentWorld()->listenForEvent(
        EventTypes::EVENT_TEST,
        [&](SharedEventPtr pEvent) {
            // TODO: Is capturing "this" safe or could it cause an access violation? 
            // happens if this object is removed before the callback is invoked?

            // This is a little dangerous but it works. Try to find a better way! 
            Event<EventTestData>* pData = (Event<EventTestData>*)pEvent.get();

            this->annoy(5);
            cout << "Event received: RegularProtester leaving the oil field." << endl;
        });
#endif
}

// Destructor
RegularProtester::~RegularProtester() {
}

// Handle a tick
void RegularProtester::doSomething() {
    Protester::doSomething();
}

// Annoy the Protester
void RegularProtester::annoy(int nHitPoints) {
    Protester::annoy(nHitPoints);
}
// Get squirted by IceMan
void RegularProtester::squirtedByIceMan() {
    Protester::squirtedByIceMan();
    if (isAnnoyed())
        getStudentWorld()->increaseScore(SQUIRT_ANNOYED_POINTS_R);
    
}

void RegularProtester::pickUpGold() {
    StudentWorld* pStudentWorld = getStudentWorld();
    pStudentWorld->playSound(SOUND_PROTESTER_FOUND_GOLD);
    pStudentWorld->increaseScore(PICK_UP_GOLD_POINTS_R);
    leave();
}

