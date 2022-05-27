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
		Direction dir		/*= right*/,
		double size			/*= 1.0*/,
		unsigned int depth	/*= 0*/,
		bool visible = true,
		bool canAnnoy = false,
		bool canPickupIM = false,
		bool canPickupP = false);

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

	// Check if we can be picked up by Iceman 
	bool canPickupIM();

	// Check if we can be picke dup by a Protester
	bool canPickupP();

	// Set canPickupIM
	void setCanPickupIM(bool);

	// Set canPickupP
	void setCanPickupP(bool);

	// Check if we can be picked up by IceMan
	bool canAnnoy()	const;

	// Check if we are still alive
	bool isAlive() const;

	// Update our alive status
	void setAlive(bool alive);

	// Get a pointer to StudentWorld
	StudentWorld* getStudentWorld();
	
	// Set the radius between a Goodie and IceMan
	void setRadiusIceMan(double);

	// Get the radius between a Goodie and IceMan
	double getRadiusIceMan();

	// Sets the number of points an Actor awards for any given action
	void setPoints(int);

	// Returns the number of points an Actor awards for any given action
	int getPoints();

	// Sets the sound effect an Actor should play
	void setSoundEffect(int);

	// Returns the sound effect an Actor plays
	int getSoundEffect();

	int getDistanceToIceman();

private:
	/*************************************************************************/
	/* Data Members														     */
	/*************************************************************************/
	
	// If we're alive - true. Otherwise, false.
	bool m_bAlive;

	// If we're capable of being annoyed - true. Otherwise, false.
	const bool m_bCanAnnoy;

	// If we're capable of being picked up by IceMan- true. Otherwise, false.
	bool m_bCanPickupIM;
	
	// If we're capable of being picked up by Protestors
	bool m_bCanPickupP;

	// Pointer to our world so we can interact with other Actors 
	StudentWorld* m_pStudentWorld;

	// Distance between this actor and IceMan
	double m_dRadiusIceMan;

	// How many points an actor should award for any given action
	int m_iPoints;

	// Sound effect ID when Goodie is picked up
	int m_iSoundEffect;

};

#endif // ACTOR_H_
