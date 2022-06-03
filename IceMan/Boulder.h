#ifndef BOULDER_H_
#define BOULDER_H_

#include "Actor.h"

const double	BOULDER_SIZE	= 1.00;
const int		BOULDER_DEPTH	= 1;
const int		BOULDER_DAMAGE	= 100;

class Boulder : public Actor
{
public:
	/*************************************************************************/
	/* Construction														     */
	/*************************************************************************/
	// Constructor
	Boulder(StudentWorld* pStudentWorld, int startX, int startY);

	// Destructor
	~Boulder();

	/*************************************************************************/
	/* Operations													     */
	/*************************************************************************/
	void doSomething();
	// Boulders cannot be annoyed
	void annoy() {};

	/*************************************************************************/
	/* Getters/Setters													     */
	/*************************************************************************/
	// Set stable state
	void setStable(bool);
	// Get stable state
	bool isStable();
	
	// Set waiting state
	void setWaiting(bool);
	// Get waiting state
	bool isWaiting();

	// Set falling state
	void setFalling(bool);
	// Get falling state
	bool isFalling();

	// Sets the number of ticks a Boulder
	// must wait to do something
	void setTicksRemaining(int);

	// Gets the number of ticks a Boulder
	// must wait before doing an action
	int getTicksRemaining();

	// Decrements the number of ticks remaining
	void decTicksRemaining();

private:
	/*************************************************************************/
	/* Data Members													     */
	/*************************************************************************/
	// Boulder states
	bool m_bStable;
	bool m_bWaiting;
	bool m_bFalling;

	unsigned int m_iTicksRemaining;
};

#endif BOULDER_H_