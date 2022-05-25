#ifndef REGULARPROTESTER_H_
#define REGULARPROTESTER_H_

#include "Protester.h"
#include <iostream>

#define TEST_REGULARPROTESTER 1

class RegularProtester : public Protester
{
public:
	/*************************************************************************/
	/* Construction														     */
	/*************************************************************************/
	// Construction
	RegularProtester(
		StudentWorld* pStudentWorld,
		int startX,
		int startY);

	// Prevent copying 
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
	virtual void annoy();

	// Handle an Event
	void handleTestEvent(int num, const char* text);

private:
};

#endif // REGULARPROTESTER_H_

