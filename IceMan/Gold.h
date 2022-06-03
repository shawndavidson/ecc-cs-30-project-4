#ifndef GOLD_H_
#define GOLD_H_

#include "Goodie.h"

const double	GOLD_SIZE				= 1.00;
const int		GOLD_DEPTH				= 2;
const int		GOLD_ICEMAN_POINTS		= 10;	// Points awarded for being picked up by IceMan
const int		GOLD_PROTESTER_POINTS	= 25;	// Points awarded for being picked up by Protester

class Gold : public Goodie
{
public:
	/*************************************************************************/
	/* Construction														     */
	/*************************************************************************/
	// Constructor
	Gold(StudentWorld* pStudentWorld,
		int startX,
		int startY,
		bool visible,
		bool canPickupIM,
		bool canPickupProtester,
		bool isPermanent);

	// Destructor
	~Gold();

	/*************************************************************************/
	/* Operations													     */
	/*************************************************************************/
	void doSomething();

	// Base for collectIM and collectP
	void collect();

	// Handles when gold is collected by IceMan
	void collectIM();

	// Handles when gold is collected by Protester
	void collectP();


	/*************************************************************************/
	/* Getters/Setters													     */
	/*************************************************************************/
private:
	/*************************************************************************/
	/* Data Members													     */
	/*************************************************************************/
};

#endif GOLD_H_