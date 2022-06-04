#ifndef SQUIRT_H_
#define SQUIRT_H_

#include "Actor.h"

const double	SQUIRT_SIZE = 1.00;
const int		SQUIRT_DEPTH = 1;

class Squirt : public Actor
{
public:
	/*************************************************************************/
	/* Construction														     */
	/*************************************************************************/
	// Constructor
	Squirt(
		StudentWorld* pStudentWorld,
		int startX,		// X is IceMan's X
		int startY,		// Y is Iceman's Y
		Direction dir); // Direction is IceMan's direction

	// Destructor
	~Squirt();

	/*************************************************************************/
	/* Operations													     */
	/*************************************************************************/
	void doSomething();

	// Squirts cannot be annoyed
	void annoy() {};

	/*************************************************************************/
	/* Getters/Setters													     */
	/*************************************************************************/
private:
	/*************************************************************************/
	/* Data Members													     */
	/*************************************************************************/
	// Distance a water squirt has left to travel
	int m_iTravelDist;

};

#endif SQUIRT_H_
