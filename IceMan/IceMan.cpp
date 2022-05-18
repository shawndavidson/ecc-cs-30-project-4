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
                // TODO
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
