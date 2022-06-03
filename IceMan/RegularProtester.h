#ifndef REGULARPROTESTER_H_
#define REGULARPROTESTER_H_

#include "Protester.h"
#include <iostream>

const int PICK_UP_GOLD_POINTS_R	= 25;
const int SQUIRT_ANNOYED_POINTS_R = 100;

#define TEST_REGULARPROTESTER 1

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
	// Get squirted by IceMan
	virtual void squirtedByIceMan();

	// Step on a Gold Nugget
	virtual void pickUpGold();

	

private:
};

#endif // REGULARPROTESTER_H_

