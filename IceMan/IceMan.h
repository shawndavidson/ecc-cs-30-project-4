#ifndef ICEMAN_H_
#define ICEMAN_H_

#include "Actor.h"

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
};

#endif ICEMAN_H_

