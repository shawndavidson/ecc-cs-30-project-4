#ifndef REGULARPROTESTER_H_
#define REGULARPROTESTER_H_

#include "Protester.h"
#include <iostream>

#define TEST_REGULARPROTESTER 0

class RegularProtester : public Protester
{
public:
	/*************************************************************************/
	/* Construction														     */
	/*************************************************************************/
	// Constructor
	RegularProtester(
		StudentWorld* pStudentWorld,
		int startX,
		int startY);

	// Copy Constructor (explicitly deleted to prevent copying) 
	RegularProtester(const RegularProtester&) = delete;

	// Destruction
	virtual ~RegularProtester();

	/*************************************************************************/
	/* Operators													     */
	/*************************************************************************/
	// Prevent assignment 
	RegularProtester& operator=(const RegularProtester&) = delete;

	/*************************************************************************/
	/* Operations													     */
	/*************************************************************************/
	virtual void doSomething();
	virtual void annoy(int nHitPoints);

	// TODO: Resolve in merge
	// Step on a Gold Nugget 
	//virtual void stepOnGoldNugget();

	// TODO: Resolve in merge
	// Squirt the Protester
	//virtual void waterSquirt(int nHitPoints);

private:
};

#endif // REGULARPROTESTER_H_

