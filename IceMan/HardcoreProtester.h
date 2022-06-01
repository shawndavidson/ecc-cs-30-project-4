#ifndef HARDCOREPROTESTER_H_
#define HARDCOREPROTESTER_H_

#include "Protester.h"

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
	virtual void stepOnGoldNugget();

	// Squirt the Protester
	virtual void waterSquirt(int nHitPoints);

private:
	// Number of ticks to stare at Gold Nugget when found
	int m_nTicksToStare;
};

#endif // HARDCOREPROTESTER_H_

