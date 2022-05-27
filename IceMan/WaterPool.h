#ifndef WATER_POOL_H_
#define WATER_POOL_H_

#include "Goodie.h"

const double	WATER_POOL_SIZE		= 1.00;
const int		WATER_POOL_DEPTH	= 2;
const int		WATER_POOL_POINTS	= 100;

class WaterPool : public Goodie
{
public:
	/*************************************************************************/
	/* Construction														     */
	/*************************************************************************/
	// Constructor
	WaterPool(StudentWorld* pStudentWorld, int startX, int startY);

	// Destructor
	~WaterPool();

	/*************************************************************************/
	/* Operations													     */
	/*************************************************************************/
	void doSomething();

	// Handles when a Water Pool is collected
	void collect();

	/*************************************************************************/
	/* Getters/Setters													     */
	/*************************************************************************/
private:
	/*************************************************************************/
	/* Data Members													     */
	/*************************************************************************/
};

#endif WATER_POOL_H_