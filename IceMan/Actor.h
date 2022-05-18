#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

// Forward Declaration
class StudentWorld;

class Actor : public GraphObject {
public:
	/*************************************************************************/
	/* Construction														     */
	/*************************************************************************/

	// Constructor
	Actor(
		StudentWorld* pStudentWorld,
		int imageID,
		int startX,
		int startY,
		Direction dir /*= right*/,
		double size /*= 1.0*/,
		unsigned int depth /*= 0*/,
		bool visible = true,
		bool canAnnoy = false,
		bool canPickup = false);

	// Prevent copying 
	Actor(const Actor&) = delete;

	// Destructor
	virtual ~Actor();

	/*************************************************************************/
	/* Operators													     */
	/*************************************************************************/
	// Prevent assignment 
	Actor& operator=(const Actor&) = delete;


	/*************************************************************************/
	/* Interface (pure virtual)														     */
	/*************************************************************************/
	// Handle a game tick
	virtual void doSomething()			= 0;

	// Handle annoying this Actor
	virtual void annoy()				= 0;

	/*************************************************************************/
	/* Operations														     */
	/*************************************************************************/

	// Check if we can be picked up by another Actor? 
	bool canPickup() const;

	// Check if we can be annoyed by another Actor? 
	bool canAnnoy()	const;

	// Check if we are still alive
	bool isAlive() const;

	// Update our alive status
	void setAlive(bool alive);

	// Get a pointer to StudentWorld
	StudentWorld* getStudentWorld();

private:
	/*************************************************************************/
	/* Data Members														     */
	/*************************************************************************/
	
	// If we're alive - true. Otherwise, false.
	bool m_bAlive;

	// If we're capable of being annoyed - true. Otherwise, false.
	const bool m_bCanAnnoy;

	// If we're capable of being picked up - true. Otherwise, false.
	const bool m_bCanPickup;

	// Pointer to our world so we can interact with other Actors 
	StudentWorld* m_pStudentWorld;
};

#endif // ACTOR_H_
