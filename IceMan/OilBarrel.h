#ifndef OIL_BARREL_H_
#define OIL_BARREL_H_

#include "Goodie.h"

const double	OIL_BARREL_SIZE		= 1.00;
const int		OIL_BARREL_DEPTH	= 2;
const int		OIL_BARREL_POINTS	= 1000;

class OilBarrel : public Goodie
{
public:
	/*************************************************************************/
	/* Construction														     */
	/*************************************************************************/
	// Constructor
	OilBarrel(StudentWorld* pStudentWorld, int startX, int startY);

	// Destructor
	~OilBarrel();

	/*************************************************************************/
	/* Operations													     */
	/*************************************************************************/
	void doSomething();
	void collect();

	/*************************************************************************/
	/* Getters/Setters													     */
	/*************************************************************************/

private:
	/*************************************************************************/
	/* Data Members													     */
	/*************************************************************************/
};

#endif OIL_BARREL_H_