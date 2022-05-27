#ifndef ICEMAN_H_
#define ICEMAN_H_

#include "Actor.h"
#include <iostream> // FIXME, just for test prints
// TODO: REMOVE TEST PRINTS

#define TEST_ICEMAN 1

// Forward Declaration
class StudentWorld;

const double ICEMAN_SIZE = 1.0;

class IceMan : public Actor
{
public:
    /*************************************************************************/
    /* Construction														     */
    /*************************************************************************/
    // Constructor
    IceMan(StudentWorld* pStudentWorld);

    // Prevent copying 
    IceMan(const IceMan&) = delete;

    // Destructor
    virtual ~IceMan();

    /*************************************************************************/
    /* Operators													     */
    /*************************************************************************/
    // Prevent assignment 
    IceMan& operator=(const IceMan&) = delete;

    /*************************************************************************/
    /* Operations													     */
    /*************************************************************************/
    virtual void doSomething();
    virtual void annoy();

    #if TEST_ICEMAN
    // Handle an Event
    void handleTestEvent(int num, const char* text);
    #endif // TEST_ICEMAN

    // Increases the number of gold nuggets by 1
    void incGold();

    // Increases the number of sonar kits by 1
    void incSonarKits();

    // Increases the number of water squirts by 5
    void incWater();

    // Decreases the number of gold nuggets by 1
    void decGold();

    // Decreases the number of sonar kits by 1
    void decSonarKits();

    // Decreases the number of water squirts by 1
    void decWater();

private:
    // Inventory counters
    int m_iGold;
    int m_iSonarKits;
    int m_iWater;


};

#endif ICEMAN_H_

