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
        20 /* nHitPoints */,
        true /* bCanTrackIceMansCell */)
{
#if TEST_HARDCOREPROTESTER
    getStudentWorld()->listenForEvent(
        EventTypes::EVENT_TEST,
        [&](SharedEventPtr pEvent) {
            // TODO: Is capturing "this" safe or could it cause an access violation? 
            // happens if this object is removed before the callback is invoked?

            // This is a little dangerous but it works. Try to find a better way! 
            Event<EventTestData>* pData = (Event<EventTestData>*)pEvent.get();

            this->annoy(10);
            cout << "Event received: HardcoreProtester leaving the oil field." << endl;
        });
#endif
}

// Destructor
HardcoreProtester::~HardcoreProtester() {
}

// Handle a tick
void HardcoreProtester::doSomething() {
    Protester::doSomething();
}

// Annoy the Protester
void HardcoreProtester::annoy(int nHitPoints) {
    Protester::annoy(nHitPoints);
}

void HardcoreProtester::squirtedByIceMan() {
    Protester::squirtedByIceMan();
    if (isAnnoyed())
        getStudentWorld()->increaseScore(SQUIRT_ANNOYED_POINTS_HC);
}

// Step on a Gold Nugget 

void HardcoreProtester::pickUpGold() {
    StudentWorld* pStudentWorld = getStudentWorld();

    pStudentWorld->playSound(SOUND_PROTESTER_FOUND_GOLD);
    pStudentWorld->increaseScore(PICK_UP_GOLD_POINTS_HC);
    
    
    // Stare at Nugget for N ticks
    pause(CALCULATE_TICKS_TO_STARE(getStudentWorld()->getLevel()));
}
