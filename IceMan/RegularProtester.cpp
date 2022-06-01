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
        5 /* nHitPoints */)
{
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

// Step on a Gold Nugget 
void RegularProtester::stepOnGoldNugget() {
    StudentWorld* pStudentWorld = getStudentWorld();
    
    pStudentWorld->playSound(SOUND_PROTESTER_FOUND_GOLD);
    pStudentWorld->increaseScore(25);
    leave();
}

// Squirt the Protester
void RegularProtester::waterSquirt(int nHitPoints) {
    annoy(nHitPoints);
    getStudentWorld()->increaseScore(100);
}

