#ifndef ICE_H_
#define ICE_H_

#include "Actor.h"

const double ICE_SIZE = 0.25;
const int ICE_DEPTH = 3;

class Ice : public Actor
{
public:
	/*************************************************************************/
	/* Construction														     */
	/*************************************************************************/
	// Constructor
	Ice(StudentWorld* pStudentWorld, int startX, int startY);

	// Prevent copying 
	Ice(const Ice&) = delete;

	// Destructor
	virtual ~Ice();

	/*************************************************************************/
	/* Operators															 */
	/*************************************************************************/
	// Prevent assignment 
	Ice& operator=(const Ice&) = delete;

	/*************************************************************************/
	/* Operations															 */
	/*************************************************************************/
	// Ice doesn't do anything
	virtual void doSomething() {};
	// Ice cannot be annoyed
	virtual void annoy() {};

};

#endif ICE_H_