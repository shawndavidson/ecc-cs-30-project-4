#ifndef GOODIE_H_
#define GOODIE_H_

#include "Actor.h"

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
		bool canPickup = false,
		bool isPermanent = true);

	// Destructor
	virtual ~Goodie();

	/*************************************************************************/
	/* Operations													     */
	/*************************************************************************/
	// Handles one tick
	virtual void doSomething()			= 0;
	// Goodies cannot be annoyed
	void annoy();


	/*************************************************************************/
	/* Getters/Setters													     */
	/*************************************************************************/
	// Sets the length of the radius between a Goodie and IceMan
	void  setRadius(int IceMan_x, int IceMan_y);
	// Gets the length of the radius between a Goodie and Iceman
	double getRadius();
	// Sets whether a goodie is permanent or not
	void setPermanentStatus(bool);
	// Gets whether a goodie is permanent or not
	bool getPermanentStatus();
	// Sets the number of ticks a temporary Goodie will remain alive
	void setTicksAlive(int);
	// Gets the number of ticks a temporary Goodie has left to live
	int getTicksAlive();


	/*************************************************************************/
	/* Pure Virtual														     */
	/*************************************************************************/
	// Handles when a Goodie is collected
	virtual void collect()				 = 0;
private:
	/*************************************************************************/
	/* Data Members													     */
	/*************************************************************************/
	// Some goodies are not permanent and must disappear after a certain number of ticks
	bool m_permanent;
	// The number of ticks a temporary Goodie will remain alive
	int m_ticks_alive;
};




#endif GOODIE_H_