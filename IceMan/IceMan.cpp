#include "IceMan.h"
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
        false /*canPickup*/),
    m_iGold(0),
    m_iSonarKits(1),
    m_iWater(5)
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
                if (getDirection() == Direction::right && x < (VIEW_WIDTH-size-1))
                    moveTo(x + 1, y);
                else
                    setDirection(Direction::right);
                break;
            case KEY_PRESS_UP:
                // If we're already facing up, move up. Otherwise, just face up;
                if (getDirection() == Direction::up && y < (VIEW_HEIGHT-size-ICE_ROW_BEGIN))
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

// Increments the gold counter by 1
void IceMan::incGold() {
    ++m_iGold;
    // FIXME - print confirmation, remove when score interface is functional
    std::cout << "GOLD " << m_iGold << std::endl;
}

// Increments the sonar kits counter by 1
void IceMan::incSonarKits(){
    ++m_iSonarKits;
    // FIXME - print confirmation, remove when score interface is functional
    std::cout << "SONAR " << m_iSonarKits << std::endl;
}

// Increments the water counter by 1
void IceMan::incWater() {
    m_iWater += 5;
    // FIXME - print confirmation, remove when score interface is functional
    std::cout << "WATER " << m_iWater << std::endl;
}

// Decrements the gold counter by 1 if there is at least 1 left
void IceMan::decGold() {
    if (m_iGold >=1)
        --m_iGold;
    // FIXME - print confirmation, remove when score interface is functional
    std::cout << "GOLD " << m_iGold << std::endl;
}

// Decrements the sonar kits counter by 1 if there is at least one left
void IceMan::decSonarKits() {
   if (m_iSonarKits >= 1)
       --m_iSonarKits;
    // FIXME - print confirmation, remove when score interface is functional
    std::cout << "SONAR " << m_iSonarKits << std::endl;
}

// Decrements the water counter by 1 if there is at least one left
void IceMan::decWater() {
    if (m_iWater >= 1)
        --m_iWater;
    // FIXME - print confirmation, remove when score interface is functional
    std::cout << "WATER " << m_iWater << std::endl;
}
