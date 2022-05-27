#ifndef SQUIRT_H_
#define SQUIRT_H_

#include "Actor.h"

const double	SQUIRT_SIZE = 1.00;
const int		SQUIRT_DEPTH = 1;
const int		SQUIRT_DAMAGE = 2;		// Amount that squirts annoy Protesters

class Squirt : public Actor
{
public:
	/*************************************************************************/
	/* Construction														     */
	/*************************************************************************/
	// Constructor
	Squirt(
		StudentWorld* pStudentWorld,
		int imageID,
		int startX,		// X is IceMan's X
		int startY,		// Y is Iceman's Y
		Direction dir); // Direction is IceMan's direction

	// Destructor
	~Squirt();

	/*************************************************************************/
	/* Operations													     */
	/*************************************************************************/
	void doSomething();

	/*************************************************************************/
	/* Getters/Setters													     */
	/*************************************************************************/
private:
	/*************************************************************************/
	/* Data Members													     */
	/*************************************************************************/


};

#endif GOLD_H_