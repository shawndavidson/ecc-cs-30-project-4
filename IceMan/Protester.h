#ifndef PROTESTOR_H_
#define PROTESTOR_H_

#include "Actor.h"

class Protester : public Actor
{
public:
	/*************************************************************************/
    /* Construction														     */
    /*************************************************************************/
	// Construction
    Protester(
		StudentWorld* pStudentWorld, 
		int imageID,
		int startX,
		int startY);

	// Prevent copying 
	Protester(const Protester&) = delete;

	// Destruction
    virtual ~Protester();

	/*************************************************************************/
	/* Operators													     */
	/*************************************************************************/
	// Prevent assignment 
	Protester& operator=(const Protester&) = delete;

	/*************************************************************************/
	/* Operations													     */
	/*************************************************************************/
	virtual void doSomething();
	virtual void annoy();

protected:
	// Take one step towards the exit 
	void moveTowardsExit();
	bool takeOneStep(GraphObject::Direction direction);
	Direction getShortestPathToExit() const;

	// Can we shout at IceMan
	bool canShoutAtIceMan();

	// Shout at IceMan
	void shout();

	// Leave the oil field
	void leave()				{ m_nLeaveTheOilField = true;  }

private:
	/*************************************************************************/
	/* Data Members													     */
	/*************************************************************************/

	unsigned int	m_nTicksToWaitBetweenMoves;
	bool			m_nLeaveTheOilField;
	unsigned long   m_nLastShoutedTick;
	unsigned int    m_nNumSquaresToMoveInCurrentDirection;
};



#endif // PROTESTOR_H_
