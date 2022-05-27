#include "IceMan.h"
#include "Ice.h"
#include "StudentWorld.h"

// Constructor
IceMan::IceMan(StudentWorld* pStudentWorld)
    : Actor(pStudentWorld, 
            IID_PLAYER, 
            30 /*startX*/,
            60 /*startY*/,
            Direction::right,
            ICEMAN_SIZE /*size*/,
            0 /*depth*/,
            true /*visible*/,
            true /*canAnnoy*/,
            false /*canPickup*/)
{
    // TODO: Remove - for testing
#if TEST_ICEMAN
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
    StudentWorld* pStudentWorld = getStudentWorld();

    int key;

    // Did the user press a key?
    if (pStudentWorld->getKey(key)) {
        const int x       = getX();
        const int y       = getY();
        const double size = getSize();

        // Handle user keyboard input
        switch (key) {
            case KEY_PRESS_LEFT:
                // If we're already facing left, move left. Otherwise, just face left;
                if (getDirection() == Direction::left && x > 0)
                    moveTo(x - 1, y);
                else
                    setDirection(Direction::left);
                break;
            case KEY_PRESS_RIGHT:
                // If we're already facing right, move right. Otherwise, just face right;
                if (getDirection() == Direction::right && x < (VIEW_WIDTH-(size*ICEMAN_SIZE/ICE_SIZE)))
                    moveTo(x + 1, y);
                else
                    setDirection(Direction::right);
                break;
            case KEY_PRESS_UP:
                // If we're already facing up, move up. Otherwise, just face up;
                if (getDirection() == Direction::up && y < ICE_HEIGHT)
                    moveTo(x, y + 1);
                else
                    setDirection(Direction::up);
                break;
            case KEY_PRESS_DOWN:
                // If we're already facing down, move down. Otherwise, just face down;
                if (getDirection() == Direction::down && y > 0)
                    moveTo(x, y - 1);
                else
                    setDirection(Direction::down);
                break;
            case KEY_PRESS_SPACE:
                {
                    // TODO: Remove - for testing
                    const int tick = getStudentWorld()->getTick() + 100;

                    // Pass along this data with the Event
                    struct EventTestData data { 1234, "Hello World!" };

                    getStudentWorld()->pushEvent(make_shared<Event<EventTestData>>( tick, EventTypes::EVENT_TEST, data ));

                    cout << "Pushing event in IceMan::doSomething() at tick " << getStudentWorld()->getTick() << endl;
                }
                break;
            case KEY_PRESS_ESCAPE:
                // TODO
                break;
            case KEY_PRESS_TAB:
                // TODO
                break;
            case 'z':
            case 'Z':
                // TODO: Are 'z' and 'Z' the same?
                break;
            default:
                // TODO: what is the behavior if the key is invalid?
                break;
        };
    }


    // TODO
}

// Handle Annoy
void IceMan::annoy() {
    // TODO
}

// Handle an Event
void IceMan::handleTestEvent(int num, const char* text) {
#if TEST_ICEMAN
    std::cout << "Tick: " << getStudentWorld()->getTick()
        << ", RegularProtester::handleTestEvent, num: " << num
        << ", text: " << text
        << std::endl;
#endif // TEST_ICEMAN
}