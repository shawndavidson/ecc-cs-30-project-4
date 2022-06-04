#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"


/************************************************************************************************/
/*																								*/
/* ACTOR																					    */
/*																								*/
/************************************************************************************************/

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
	/* Operators														     */
	/*************************************************************************/
	// Prevent assignment 
	Actor& operator=(const Actor&) = delete;


	/*************************************************************************/
	/* Interface (pure virtual)											     */
	/*************************************************************************/
	// Handle a game tick
	virtual void doSomething()			= 0;

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
	void setAlive(bool alive, bool syncVisibility = true);

	// Get a pointer to StudentWorld
	StudentWorld* getStudentWorld() { return m_pStudentWorld; }

	// Sets the number of points an Actor awards for any given action
	void setPoints(int);

	// Returns the number of points an Actor awards for any given action
	int getPoints();

	// Sets the sound effect an Actor should play
	void setSoundEffect(int);

	// Returns the sound effect an Actor plays
	int getSoundEffect();

	// Returns the distance of an Actor to IceMan
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

	// How many points an actor should award for any given action
	int m_iPoints;

	// Sound effect ID when Goodie is picked up
	int m_iSoundEffect;

};

#endif // ACTOR_H_

/************************************************************************************************/
/*																								*/
/* ICE																						    */
/*																								*/
/************************************************************************************************/

#ifndef ICE_H_
#define ICE_H_

const double ICE_SIZE = 0.25;
const int ICE_DEPTH = 3;

class Ice : public Actor
{
public:
	/*************************************************************************/
	/* Construction														     */
	/*************************************************************************/
	// Constructor
	Ice(StudentWorld* pStudentWorld, int startX, int startY);

	// Prevent copying 
	Ice(const Ice&) = delete;

	// Destructor
	virtual ~Ice();

	/*************************************************************************/
	/* Operators															 */
	/*************************************************************************/
	// Prevent assignment 
	Ice& operator=(const Ice&) = delete;

	/*************************************************************************/
	/* Operations															 */
	/*************************************************************************/
	// Ice doesn't do anything
	virtual void doSomething() {};

};

#endif ICE_H_



/************************************************************************************************/
/*																								*/
/* BOULDER																						    */
/*																								*/
/************************************************************************************************/

#ifndef BOULDER_H_
#define BOULDER_H_

const double	BOULDER_SIZE = 1.00;
const int		BOULDER_DEPTH = 1;
const int		BOULDER_DAMAGE = 100;

class Boulder : public Actor
{
public:
	/*************************************************************************/
	/* Construction														     */
	/*************************************************************************/
	// Constructor
	Boulder(StudentWorld* pStudentWorld, int startX, int startY);

	// Destructor
	~Boulder();

	/*************************************************************************/
	/* Operations													     */
	/*************************************************************************/
	void doSomething();
	// Boulders cannot be annoyed

	/*************************************************************************/
	/* Getters/Setters													     */
	/*************************************************************************/
	// Set stable state
	void setStable(bool);
	// Get stable state
	bool isStable();

	// Set waiting state
	void setWaiting(bool);
	// Get waiting state
	bool isWaiting();

	// Set falling state
	void setFalling(bool);
	// Get falling state
	bool isFalling();

	// Sets the number of ticks a Boulder
	// must wait to do something
	void setTicksRemaining(int);

	// Gets the number of ticks a Boulder
	// must wait before doing an action
	int getTicksRemaining();

	// Decrements the number of ticks remaining
	void decTicksRemaining();

private:
	/*************************************************************************/
	/* Data Members													     */
	/*************************************************************************/
	// Boulder states
	bool m_bStable;
	bool m_bWaiting;
	bool m_bFalling;

	unsigned int m_iTicksRemaining;
};

#endif BOULDER_H_


/************************************************************************************************/
/*																								*/
/* SQUIRT																					    */
/*																								*/
/************************************************************************************************/

#ifndef SQUIRT_H_
#define SQUIRT_H_

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
	/* Operations															 */
	/*************************************************************************/
	void doSomething();

private:
	/*************************************************************************/
	/* Data Members															 */
	/*************************************************************************/
	// Distance a water squirt has left to travel
	int m_iTravelDist;

};

#endif SQUIRT_H_


/************************************************************************************************/
/*																								*/
/* PERSON																					    */
/*																								*/
/************************************************************************************************/

#ifndef PERSON_H_
#define PERSON_H_

//#define PERSON_SIZE     4


class Person : public Actor
{
public:
	/*************************************************************************/
	/* Construction														     */
	/*************************************************************************/
	// Constructor
	Person(
		StudentWorld* pStudentWorld,
		int imageID,
		int startX,
		int startY,
		Direction dir,
		int nHitPoints);

	virtual ~Person();

	/*************************************************************************/
	/* Getters/Setters														     */
	/*************************************************************************/
	bool    isAnnoyed()     const { return m_nHitPoints <= 0; }

	// Return remaining Hit Points
	int     getHitPoints()  const { return m_nHitPoints; }

public:
	virtual void annoy(int hitPoints) { m_nHitPoints -= hitPoints; }

private:
	/*************************************************************************/
	/* Data Members														     */
	/*************************************************************************/
	int m_nHitPoints;
};

#endif PERSON_H_



/************************************************************************************************/
/*																								*/
/* ICEMAN																					    */
/*																								*/
/************************************************************************************************/

#ifndef ICEMAN_H_
#define ICEMAN_H_

#define TEST_ICEMAN 1
#define TEST_EVENTS 0 

// Forward Declaration
class StudentWorld;

const double ICEMAN_SIZE = 1.0;

class IceMan : public Person
{
public:
	/*************************************************************************/
	/* Construction														     */
	/*************************************************************************/
	// Constructor
	IceMan(StudentWorld* pStudentWorld);

	// Prevent copying 
	IceMan(const IceMan&) = delete;

	// Destructor
	virtual ~IceMan();

	/*************************************************************************/
	/* Operators													         */
	/*************************************************************************/
	// Prevent assignment 
	IceMan& operator=(const IceMan&) = delete;

	/*************************************************************************/
	/* Operations													         */
	/*************************************************************************/
	// Hanlde Tick
	virtual void doSomething();

	// Annoy IceMan
	virtual void annoy(int);

	// Take a step and dig up ice, if necessary
	void takeOneStep(int x, int y);

	// Handle an Event
	void handleTestEvent(int num, const char* text);

	// Increases the number of gold nuggets by 1
	void incGold();

	// Increases the number of sonar kits by 1
	void incSonarKits();

	// Increases the number of water squirts by 5
	void incWater();

	// Decreases the number of gold nuggets by 1
	void decGold();

	// Decreases the number of sonar kits by 1
	void decSonarKits();

	// Decreases the number of water squirts by 1
	void decWater();

	// Return Gold
	int getGold();

	// Return Sonar Kits
	int getSonarKits();

	// Return Water Squirts
	int getWater();

private:
	// Inventory counters
	unsigned int m_iGold;
	unsigned int m_iSonarKits;
	unsigned int m_iWater;
};

#endif ICEMAN_H_


/************************************************************************************************/
/*																								*/
/* PROTESTER																				    */
/*																								*/
/************************************************************************************************/

#ifndef PROTESTOR_H_
#define PROTESTOR_H_

#include <vector>
#include <random>

const int SQUIRT_DAMAGE = 2;

class Protester : public Person
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
		int startY,
		int nHitPoints,
		bool bCanTrackIceMansCell);

	// Prevent copying 
	Protester(const Protester&) = delete;

	// Destruction
	virtual			~Protester();

	/*************************************************************************/
	/* Operators													     */
	/*************************************************************************/
	// Prevent assignment 
	Protester& operator=(const Protester&) = delete;

	/*************************************************************************/
	/* Operations													     */
	/*************************************************************************/
	virtual void	doSomething();
	virtual void	annoy(int nHitPoints);

	// Step on a Gold Nugget
	virtual void	pickUpGold() = 0;

	// Protester is bonked by a Boulder
	virtual void	bonkedByBoulder();

	// Protester is hit by a squirt
	virtual void	squirtedByIceMan();


protected:
	// Take one step towards the exit 
	void			moveTowardsExit();

	// Take one step towards IceMan 
	void			moveTowardsIceMan();

	// Take one step in the given direction
	bool			takeOneStep(GraphObject::Direction direction);

	// Get possible perpendicular turns relative to the current direction
	bool			getPossiblePerpendicularDirections(std::vector<GraphObject::Direction>& directions);

	// Can we shout at IceMan
	bool			canShoutAtIceMan();

	// Shout at IceMan
	void			shout();

	// Leave the oil field
	void			leave() { m_nLeaveTheOilField = true; }

	// Check if we're leaving the oil field
	bool			isLeaving() const { return m_nLeaveTheOilField; }

	// Pause the Protester for N ticks
	void			pause(int nTicks) { m_nTicksStunned = nTicks; }

private:
	/*************************************************************************/
	/* Data Members													     */
	/*************************************************************************/

	size_t			m_nTicksToWaitBetweenMoves;
	bool			m_nLeaveTheOilField;
	unsigned long   m_nLastShoutedTick;
	int				m_nNumSquaresToMoveInCurrentDirection;
	size_t			m_nTickOfLastPerpendicularTurn;
	std::vector<Direction> m_allDirections;

	static std::random_device	m_randomDevice;
	static std::mt19937			m_randomGenerator;

	size_t			m_nTicksStunned;
	const size_t	m_nIceManCellRange;
	const bool		m_bCanTrackIceMansCell;
};

#endif PROTESTOR_H_


/************************************************************************************************/
/*																								*/
/* REGULAR PROTESTER																		    */
/*																								*/
/************************************************************************************************/

#ifndef REGULARPROTESTER_H_
#define REGULARPROTESTER_H_

#include <iostream>

const int PICK_UP_GOLD_POINTS_R = 25;
const int SQUIRT_ANNOYED_POINTS_R = 100;

#define TEST_REGULARPROTESTER	0

class RegularProtester : public Protester
{
public:
	/*************************************************************************/
	/* Construction														     */
	/*************************************************************************/
	// Constructor
	RegularProtester(
		StudentWorld* pStudentWorld,
		int startX,
		int startY);

	// Copy Constructor (explicitly deleted to prevent copying) 
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
	virtual void annoy(int nHitPoints);
	// Get squirted by IceMan
	virtual void squirtedByIceMan();

	// Step on a Gold Nugget
	virtual void pickUpGold();

private:
};

#endif // REGULARPROTESTER_H_


/************************************************************************************************/
/*																								*/
/* HARDCORE PROTESTER																		    */
/*																								*/
/************************************************************************************************/

#ifndef HARDCOREPROTESTER_H_
#define HARDCOREPROTESTER_H_

#define TEST_HARDCOREPROTESTER 1

const int PICK_UP_GOLD_POINTS_HC = 50;
const int SQUIRT_ANNOYED_POINTS_HC = 250;

class HardcoreProtester : public Protester
{
public:
	/*************************************************************************/
	/* Construction														     */
	/*************************************************************************/
	// Construction
	HardcoreProtester(
		StudentWorld* pStudentWorld,
		int startX,
		int startY);

	// Prevent copying 
	HardcoreProtester(const HardcoreProtester&) = delete;

	// Destruction
	virtual ~HardcoreProtester();

	/*************************************************************************/
	/* Operators													     */
	/*************************************************************************/
	// Prevent assignment 
	HardcoreProtester& operator=(const HardcoreProtester&) = delete;

	/*************************************************************************/
	/* Operations													     */
	/*************************************************************************/
	virtual void doSomething();

	virtual void annoy(int nHitPoints);

	// Get squirted by IceMan
	virtual void squirtedByIceMan();

	// Step on a Gold Nugget
	void pickUpGold();

};

#endif // HARDCOREPROTESTER_H_



/************************************************************************************************/
/*																								*/
/* GOODIE																					    */
/*																								*/
/************************************************************************************************/

#ifndef GOODIE_H_
#define GOODIE_H_

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



/************************************************************************************************/
/*																								*/
/* GOLD																						    */
/*																								*/
/************************************************************************************************/

#ifndef GOLD_H_
#define GOLD_H_

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

};

#endif GOLD_H_



/************************************************************************************************/
/*																								*/
/* OIL BARREL																				    */
/*																								*/
/************************************************************************************************/

#ifndef OIL_BARREL_H_
#define OIL_BARREL_H_

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

};

#endif OIL_BARREL_H_


/************************************************************************************************/
/*																								*/
/* SONAR KIT																				    */
/*																								*/
/************************************************************************************************/

#ifndef SONAR_KIT_H_
#define SONAR_KIT_H_

const double	SONAR_KIT_SIZE = 1.00;
const int		SONAR_KIT_DEPTH = 2;
const int		SONAR_KIT_POINTS = 75;

class SonarKit : public Goodie
{
public:
	/*************************************************************************/
	/* Construction														     */
	/*************************************************************************/
	// Constructor
	SonarKit(StudentWorld* pStudentWorld,
		int startX,
		int startY,
		bool isVisible);

	// Destructor
	~SonarKit();

	/*************************************************************************/
	/* Operations													     */
	/*************************************************************************/
	void doSomething();

	// Handles when a Sonar Kit is collected
	void collect();

};

#endif SONAR_KIT_H_


/************************************************************************************************/
/*																								*/
/* WATER POOL																				    */
/*																								*/
/************************************************************************************************/

#ifndef WATER_POOL_H_
#define WATER_POOL_H_

const double	WATER_POOL_SIZE		= 1.00;
const int		WATER_POOL_DEPTH	= 2;
const int		WATER_POOL_POINTS	= 100;

class WaterPool : public Goodie
{
public:
	/*************************************************************************/
	/* Construction														     */
	/*************************************************************************/
	// Constructor
	WaterPool(StudentWorld* pStudentWorld, int startX, int startY);

	// Destructor
	~WaterPool();

	/*************************************************************************/
	/* Operations														     */
	/*************************************************************************/
	void doSomething();

	// Handles when a Water Pool is collected
	void collect();

};

#endif WATER_POOL_H_