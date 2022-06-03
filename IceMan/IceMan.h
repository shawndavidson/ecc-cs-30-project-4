#ifndef ICEMAN_H_
#define ICEMAN_H_

#include "Person.h"

#define TEST_ICEMAN 0

// Forward Declaration
class StudentWorld;

const double ICEMAN_SIZE = 1.0;

class IceMan : public Person
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
    /* Operators													         */
    /*************************************************************************/
    // Prevent assignment 
    IceMan& operator=(const IceMan&) = delete;

    /*************************************************************************/
    /* Operations													         */
    /*************************************************************************/
    // Hanlde Tick
    virtual void doSomething();

    // Annoy IceMan
    virtual void annoy(int);

    // Take a step and dig up ice, if necessary
    void takeOneStep(int x, int y);

    // Handle an Event
    void handleTestEvent(int num, const char* text);

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

    // Return Gold
    int getGold();

    // Return Sonar Kits
    int getSonarKits();

    // Return Water Squirts
    int getWater();


private:
    // Inventory counters
    unsigned int m_iGold;
    unsigned int m_iSonarKits;
    unsigned int m_iWater;
};

#endif ICEMAN_H_

