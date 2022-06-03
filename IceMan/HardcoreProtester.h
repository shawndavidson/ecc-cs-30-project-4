#ifndef HARDCOREPROTESTER_H_
#define HARDCOREPROTESTER_H_

#include "Protester.h"

#define TEST_HARDCOREPROTESTER 1

const int PICK_UP_GOLD_POINTS_HC = 50;
const int SQUIRT_ANNOYED_POINTS_HC = 250;

class HardcoreProtester : public Protester
{
public:
	/*************************************************************************/
	/* Construction														     */
	/*************************************************************************/
	// Construction
	HardcoreProtester(
		StudentWorld* pStudentWorld,
		int startX,
		int startY);

	// Prevent copying 
	HardcoreProtester(const HardcoreProtester&) = delete;

	// Destruction
	virtual ~HardcoreProtester();

	/*************************************************************************/
	/* Operators													     */
	/*************************************************************************/
	// Prevent assignment 
	HardcoreProtester& operator=(const HardcoreProtester&) = delete;

	/*************************************************************************/
	/* Operations													     */
	/*************************************************************************/
	virtual void doSomething();

	virtual void annoy(int nHitPoints);

	// Step on a Gold Nugget
	void pickUpGold();

private:
};

#endif // HARDCOREPROTESTER_H_

