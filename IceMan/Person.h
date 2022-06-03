#ifndef PERSON_H_
#define PERSON_H_

#include "Actor.h"


class Person : public Actor
{
public:
    /*************************************************************************/
    /* Construction														     */
    /*************************************************************************/
    // Constructor
    Person(
        StudentWorld* pStudentWorld,
        int imageID,
        int startX,
        int startY,
        Direction dir,
        int nHitPoints);

    virtual ~Person();

    /*************************************************************************/
    /* Getters/Setters														     */
    /*************************************************************************/
    bool    isAnnoyed()     const           { return m_nHitPoints <= 0; }

    // Return remaining Hit Points
    int     getHitPoints()  const           { return m_nHitPoints; }

    /*************************************************************************/
    /* Operations														     */
    /*************************************************************************/
public:
    virtual void annoy(int hitPoints)       { m_nHitPoints -= hitPoints;  }

private:
    /*************************************************************************/
    /* Data Members														     */
    /*************************************************************************/
    int m_nHitPoints;
};

#endif // PERSON_H_

