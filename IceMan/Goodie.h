#ifndef GOODIE_H_
#define GOODIE_H_

#include "Actor.h"
#include "StudentWorld.h"

class Goodie : public Actor {
public:
	/*************************************************************************/
	/* Construction														     */
	/*************************************************************************/
	// Constructor
	Goodie(
		StudentWorld* pStudentWorld,
		int imageID,
		int startX,
		int startY,
		Direction dir /*= right*/,
		double size /*= 1.0*/,
		unsigned int depth /*= 0*/,
		bool visible = false,
		bool canAnnoy = false,
		bool canPickupIM = true,
		bool canPickupP = false,
		bool isPermanent = true);

	// Destructor
	virtual ~Goodie();

	/*************************************************************************/
	/* Operations													     */
	/*************************************************************************/
	// Handles one tick
	virtual void doSomething() = 0;

	// Goodies cannot be annoyed
	void annoy();

	/*************************************************************************/
	/* Getters/Setters													     */
	/*************************************************************************/
	// Sets whether a goodie is permanent or not
	void setPermanentStatus(bool);

	// Gets whether a goodie is permanent or not
	bool isPermanent();

	// Sets the number of ticks a temporary Goodie
	// must wait to do something
	void setTicksRemaining(int);

	// Gets the number of ticks a temporary Goodie
	// must wait before doing an action
	int getTicksRemaining();

	// Decrements the number of ticks remaining
	void decTicksRemaining();

	// Sets whether a Goodie is collected
	void setCollected(bool);

	// Returns true if goodie was collected
	bool isCollected();

	/*************************************************************************/
	/* Pure Virtual														     */
	/*************************************************************************/

	virtual void collect() = 0;
	
private:
	/*************************************************************************/
	/* Data Members													     */
	/*************************************************************************/

	// Some goodies are not permanent and must disappear after a certain number of ticks
	bool m_bPermanent;

	// The number of ticks a temporary Goodie will remain alive
	unsigned int m_iTicksRemaining;

	// True if the Goodie has been collected
	bool m_bCollected;

};

#endif GOODIE_H_