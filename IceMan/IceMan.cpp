#include "IceMan.h"
#include "Ice.h"
#include "StudentWorld.h"
#include "Squirt.h"

// Constructor
IceMan::IceMan(StudentWorld* pStudentWorld)
    : Person(pStudentWorld, 
            IID_PLAYER, 
            30 /*startX*/,
            60 /*startY*/,
            Direction::right,
            10 /* nHitPoints */),
    m_iGold(0),         // start with 0
    m_iSonarKits(1),    // start with 1
    m_iWater(5)         // start with 5
{
    
    // TODO: Remove - for testing
#if TEST_EVENTS
    getStudentWorld()->listenForEvent(
        EventTypes::EVENT_TEST,
        [&](SharedEventPtr pEvent) {
            // TODO: Is capturing "this" safe or could it cause an access violation? 
            // happens if this object is removed before the callback is invoked?

            // This is a little dangerous but it works. Try to find a better way! 
            Event<EventTestData>* pData = (Event<EventTestData>*)pEvent.get();

            this->handleTestEvent(pData->getData().num, pData->getData().text);
        });
#endif
}

// Destructor
IceMan::~IceMan() {
}

// Handle Tick
void IceMan::doSomething() {
    // If we're not alive, there's nothing to do
    if (!isAlive()) {
        return;
    }

    // Cache a pointer to StudentWorld, for the sake of convenience
    StudentWorld* const pStudentWorld = getStudentWorld();
    Direction direction = getDirection();

    int key;

    // Did the user press a key?
    if (pStudentWorld->getKey(key)) {
        const int x       = getX();
        const int y       = getY();
        const double size = getSize();

        // Handle user keyboard input
        switch (key) {
            // Check if there is a Boulder before moving in any direction
            case KEY_PRESS_LEFT:
                // If we're already facing left, move left. Otherwise, just face left;
                if (!pStudentWorld->isBlockedByBoulder(x, y, direction) &&
                    direction == Direction::left && x > 0)
                    takeOneStep(x - 1, y);
                else
                    setDirection(Direction::left);
                break;
            case KEY_PRESS_RIGHT:
                // If we're already facing right, move right. Otherwise, just face right;
                if (!pStudentWorld->isBlockedByBoulder(x, y, direction) &&
                    direction == Direction::right && x < (VIEW_WIDTH-(size*ICEMAN_SIZE/ICE_SIZE)))
                    takeOneStep(x + 1, y);
                else
                    setDirection(Direction::right);
                break;
            case KEY_PRESS_UP:
                // If we're already facing up, move up. Otherwise, just face up;
                if (!pStudentWorld->isBlockedByBoulder(x, y, direction) &&
                    direction == Direction::up && y < ICE_HEIGHT)
                    takeOneStep(x, y + 1);
                else
                    setDirection(Direction::up);
                break;
            case KEY_PRESS_DOWN:
                // If we're already facing down, move down. Otherwise, just face down;
                if (!pStudentWorld->isBlockedByBoulder(x, y, direction) &&
                    direction == Direction::down && y > 0)
                    takeOneStep(x, y - 1);
                else
                    setDirection(Direction::down);
                break;
            case KEY_PRESS_SPACE:
                // Fire a water squirt
                if (m_iWater > 0) {
                    pStudentWorld->StudentWorld::fireSquirt(getX(), getY(), getDirection());
                    --m_iWater;
                    return;
                }
                break;
            case KEY_PRESS_ESCAPE:
                // Restart level. Lose a life
                setAlive(false);
                break;
            case KEY_PRESS_TAB:
                // Drop Gold Nugget
                if (m_iGold > 0) {
                    pStudentWorld->StudentWorld::dropGold();
                    --m_iGold;
                }
                break;
            case 'z':
            case 'Z': 
                // Use SonarKits
                if (m_iSonarKits > 0) {
                    pStudentWorld->StudentWorld::useSonarKit();
                    --m_iSonarKits;
                }
                break;
                
#if TEST_ICEMAN
            case 'p':
                {
                    getStudentWorld()->dump();
                }
                break;
#endif
#if TEST_EVENTS
            case 'e': 
                {
                    const int tick = getStudentWorld()->getTick() + 100;
                    
                    // Pass along this data with the Event
                    struct EventTestData data { 1234, "Hello World!" };

                    getStudentWorld()->pushEvent(make_shared<Event<EventTestData>>(tick, EventTypes::EVENT_TEST, data));
                }
                break;
#endif
            case 'q':
                // Quit game
                pStudentWorld->cleanUp();
                break;
            default:
                // Do nothing if input key is invalid
                break;
        };
    }
}

// Handle Annoy
void IceMan::annoy(int nHitPoints) {
    Person::annoy(nHitPoints);
    if (isAnnoyed()) {
        setAlive(false);
    }
}

// Increments the gold counter by 1
void IceMan::incGold() {
    ++m_iGold;
}

// Increments the sonar kits counter by 1
void IceMan::incSonarKits(){
    ++m_iSonarKits;
}

// Increments the water counter by 5 (since picking up a Water Pool gives 5)
void IceMan::incWater() {
    m_iWater += 5;
}

// Decrements the gold counter by 1 if there is at least 1 left
void IceMan::decGold() {
   --m_iGold;
}

// Decrements the sonar kits counter by 1 if there is at least one left
void IceMan::decSonarKits() {
   --m_iSonarKits;
}

// Decrements the water counter by 1 if there is at least one left
void IceMan::decWater() {
    --m_iWater;
}

// Return Gold
int IceMan:: getGold() {
    return m_iGold;
}

// Return Sonar Kits
int IceMan::getSonarKits() {
    return m_iSonarKits;
}

// Return Water Squirts
int IceMan::getWater() {
    return m_iWater;
}

#if TEST_ICEMAN
// Handle an Event
void IceMan::handleTestEvent(int num, const char* text) {
    std::cout << "Tick: " << getStudentWorld()->getTick()
        << ", RegularProtester::handleTestEvent, num: " << num
        << ", text: " << text
        << std::endl;


}
#endif // TEST_ICEMAN

// Take a step and dig up ice, if necessary
void IceMan::takeOneStep(int x, int y) {
    moveTo(x, y);
    getStudentWorld()->digUpIce(x, y);
}
