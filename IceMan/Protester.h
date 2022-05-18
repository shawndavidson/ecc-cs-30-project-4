#ifndef PROTESTOR_H_
#define PROTESTOR_H_

#include "Actor.h"

class Protester : public Actor
{
public:
	/*************************************************************************/
    /* Construction														     */
    /*************************************************************************/
	// Construction
    Protester(
		StudentWorld* pStudentWorld, 
		int imageID,
		int startX,
		int startY);

	// Prevent copying 
	Protester(const Protester&) = delete;

	// Destruction
    virtual ~Protester();

	/*************************************************************************/
	/* Operators													     */
	/*************************************************************************/
	// Prevent assignment 
	Protester& operator=(const Protester&) = delete;

	/*************************************************************************/
	/* Operations													     */
	/*************************************************************************/
	virtual void doSomething();
	virtual void annoy();

private:
};



#endif // PROTESTOR_H_
