#ifndef ICEMAN_H_
#define ICEMAN_H_

#include "Actor.h"

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

    // Destructor
    virtual ~IceMan();

    /*************************************************************************/
    /* Operations													     */
    /*************************************************************************/
    virtual void doSomething();
    virtual void annoy();

};

#endif ICEMAN_H_

