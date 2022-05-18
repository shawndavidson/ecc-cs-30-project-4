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

};

#endif ICEMAN_H_

