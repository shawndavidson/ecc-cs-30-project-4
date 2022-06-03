#ifndef HARDCOREPROTESTER_H_
#define HARDCOREPROTESTER_H_

#include "Protester.h"

#define TEST_HARDCOREPROTESTER 1

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

	// TODO: Resolve in merge
	// Step on a Gold Nugget 
	//virtual void stepOnGoldNugget();

	// TODO: Resolve in merge
	// Squirt the Protester
	//virtual void waterSquirt(int nHitPoints);

private:
};

#endif // HARDCOREPROTESTER_H_

