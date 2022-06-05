#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include <string>
#include <vector>
#include <memory>
#include <queue>
#include <map>
#include <unordered_map>
#include <thread>

#include <functional>
#include <iostream>

#include "GameWorld.h"
#include "GameConstants.h"
#include "GraphObject.h"


#define TEST_STUDENTWORLD			0
#define TEST_WORKER_MULTITHREADS	0

#define PERSON_SIZE					4

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

using namespace std;

// Forward Declarations
class Actor;
class Goodie;
class IceMan;
class Ice;
class OilBarrel;
class Squirt;
class Boulder;

class DistanceCalculator;

/*************************************************************************/
/* Type Declaration(s)													 */
/*************************************************************************/
typedef shared_ptr<Actor>					ActorPtr;
typedef shared_ptr<Ice>						IcePtr;
typedef shared_ptr<thread>					ThreadPtr;

/*************************************************************************/
/* Constants														     */
/*************************************************************************/
const int ICE_WIDTH		= VIEW_WIDTH;
const int ICE_HEIGHT	= VIEW_HEIGHT - PERSON_SIZE;









/************************************************************************************************/
/*																								*/
/* DISTANCE CALCULATOR																		    */
/*																								*/
/************************************************************************************************/

#ifndef DISTANCECALCULATOR_H_
#define DISTANCECALCULATOR_H_

#include <cstdint>

// Set to 1 to ensure optimizations generate the correct results.
#define TEST_DISTANCECALCULATOR 0


class DistanceCalculator
{
public:
	/*************************************************************************/
	/* Construction														     */
	/*************************************************************************/
	// Constructor
	DistanceCalculator();

	// Copy Constructor (deleted - too expensive)
	DistanceCalculator(const DistanceCalculator& rhs) = delete;

	// Destructor
	~DistanceCalculator();

	/*************************************************************************/
	/* Operators														     */
	/*************************************************************************/
	// Assignment Operator (deleted - too expensive)
	DistanceCalculator& operator=(const DistanceCalculator&) = delete;

	/*************************************************************************/
	/* Operations														     */
	/*************************************************************************/
#if TEST_DISTANCECALCULATOR
	// Test optimized table - debug only
	void Test();
#endif //UNIT_TEST

	// Get distance between two squares
	inline uint8_t getDistance(int x0, int y0, int x1, int y1) const
	{
		return m_table[x0][y0][x1][y1];
	}

private:
	uint8_t m_table[VIEW_WIDTH][VIEW_HEIGHT][VIEW_WIDTH][VIEW_HEIGHT];
};

#endif // DISTANCECALCULATOR_H_


/************************************************************************************************/
/*																								*/
/*SHORTEST PATH FINDER																		    */
/*																								*/
/************************************************************************************************/

#ifndef SHORTESTPATHFINDER_H_
#define SHORTESTPATHFINDER_H_

#include "GameConstants.h"
#include "GraphObject.h"

#define TEST_SHORTESTPATHFINDER 0

// Forward Declaration
class StudentWorld;

struct DirectionDistance {
	// Constructor
	DirectionDistance(GraphObject::Direction direction = GraphObject::Direction::none, size_t distance = 0)
		: direction(direction), distance(distance) {}

	GraphObject::Direction  direction;
	size_t                  distance;
};

class ShortestPathFinder
{
public:
	/*************************************************************************/
	/* Construction															 */
	/*************************************************************************/
	// Constructor
	ShortestPathFinder(StudentWorld* pStudentWorld);

	// Destructor
	~ShortestPathFinder() {};

	/*************************************************************************/
	/* Getters/Setters															 */
	/*************************************************************************/
	// Get a pointer to StudentWorld
	StudentWorld* getStudentWorld() { return m_pStudentWorld; }

	// Get the current X location
	int getX() const { return m_originX; }

	// Get the current Y location
	int getY() const { return m_originY; }


	/*************************************************************************/
	/* Operations															 */
	/*************************************************************************/

	// Compute distances from all squares relative to location (x,y)
	bool compute(int x, int y);

	// Get the direction that has the shortest direction
	bool getShortestPath(int x, int y, DirectionDistance& result) const;

	void dump() const;

	/*************************************************************************/
	/* Data Members															 */
	/*************************************************************************/
private:
	struct Coordinates {
		// Constructor
		Coordinates(uint8_t x, uint8_t y, size_t distance) : x(x), y(y), distance(distance) {}

		uint8_t x, y;
		size_t  distance;
	};

	StudentWorld* m_pStudentWorld;

	// The location from which we want to calculate distances
	uint8_t m_originX;
	uint8_t m_originY;

	size_t m_distances[VIEW_WIDTH][VIEW_HEIGHT];
};

#endif // SHORTESTPATHFINDER_H_







/************************************************************************************************************/
/*																											*/
/************************************************************************************************************/
/*																											*/
/* STUDENT WORLD																						    */
/*																											*/
/************************************************************************************************************/
/*																											*/
/************************************************************************************************************/

class StudentWorld : public GameWorld
{
public:
	/*************************************************************************/
	/* Construction														     */
	/*************************************************************************/
	StudentWorld(std::string assetDir);

	// Prevent copying 
	StudentWorld(const StudentWorld&) = delete;

	virtual ~StudentWorld();

	/*************************************************************************/
	/* Operators															 */
	/*************************************************************************/
	// Prevent assignment 
	StudentWorld& operator=(const StudentWorld&) = delete;

	/*************************************************************************/
	/* Getters/Setters													     */
	/*************************************************************************/
	// Get the tick which represents time
	unsigned long getTick()	const { return m_nTick; }

	string getGameStatText();

	/*************************************************************************/
	/* Operations														     */
	/*************************************************************************/
    // Perform initializion
	virtual int init();

	// Handle movement for all game objects within our world
	virtual int move();

	// Dig up the ice at this location
	void digUpIce(int x, int y);

	// Cleanup game objects (deallocates memory)
	virtual void cleanUp();

	// Remove all dead game objects (deallocate memory)
	virtual void removeDeadGameObjects();

	// Creates random x coordinate for actors to spawn in
	int getRandomX();

	// Creates random y coordinate for actors to spawn in
	int getRandomY(int minHeight);

	// Generates x and y coordinates that fall within ice field
	// Not within 6 of other actors and not in tunnel
	pair<int, int> getRandCoordinates(int minHeight);

	// Adds new actors during each tick
	void addNewActors();

	// Adds new Protesters, either Regular or Hardcore
	void addProtester(int ID);

	// Initializes and places new WaterPool
	void addWaterPool();

	// Initializes and places new SonarKit
	void addSonarKit();
		
	// Compute straight-line distance to IceMan
	int getDistanceToIceMan(int x, int y) const;

	// Compute shortest path distance to IceMan, i.e. through the maze avoiding
	// ice and boulders
	unsigned int getPathDistanceToIceMan(int x, int y) const;

	// Check if these coordinates and direction are facing IceMan
	bool isFacingIceMan(int x, int y, GraphObject::Direction direction) const;

	// Check if a Protester standing in this location and facing this direction has
	// a direct line of sight to IceMan 
	bool hasLineOfSightToIceMan(int x, int y, GraphObject::Direction& direction) const;

	// Is this location occupied by Ice or a Boulder
	inline bool isBlocked(int x, int y, GraphObject::Direction direction) const;

	// Is this location occupied by a Boulder
	inline bool StudentWorld::isBlockedByBoulder(int x, int y, GraphObject::Direction direction) const;

	// Get the direction that has the shortest path to the exit
	bool getShortestPathToExit(int x, int y, DirectionDistance& result)		{ return m_shortestPathToExit.getShortestPath(x, y, result); }

	// Get the direction that has the shortest path to IceMan
	bool getShortestPathToIceMan(int x, int y, DirectionDistance& result)	{ return m_shortestPathToIceMan.getShortestPath(x, y, result); }

	// Dump debugging out to the console
	void dump() const;

	/*************************************************************************/
	/* Goodie Operations												     */
	/*************************************************************************/
	// Sets the number of Oil Barrels in the field
	void setNumBarrels(int);

	// Gets the number of Oil Barrels remaining in the field
	int getNumBarrels();

	// Decrements the number of Oil Barrels remaining in the field
	void decNumBarrels();

	// Handles when a Goodie is picked up by IceMan
	void pickupGoodieIM(int ID, int points, int soundEffect);

	// Handles when a Gold nugget is picked up by a Protester
	bool pickupGoldP(int x, int y);

	// Lets IceMan use a Sonar Kit
	void useSonarKit();

	// Lets IceMan place a gold nugget
	void dropGold();



	/*************************************************************************/
	/* Squirt, Boulder, and Shout Operations							     */
	/*************************************************************************/

	// Handles when a Squirt is fired by IceMan
	void fireSquirt(int x, int y, GraphObject::Direction);

	// Handles when a Squirt hits Ice, Boulder, or Protester
	// Pass in the coordinates of the squirt
	bool hitBySquirt(int x, int y);

	// Checks if there is ice under a Boulder,
	// or if the Boulder is at the bottom of the ice field
	bool isGroundUnderBoulder(int x, int y);

	// Handles if a boulder hits IceMan, Protester, or another Boulder
	// Return true if Boulder hits another Boulder
	bool hitByBoulder(int x, int y);

	// Handles when a Protester shouts at IceMan
	void iceManShoutedAt();

private:

	// Start calculations in worker threads
	void startWorkerThreads();

private:
	/*************************************************************************/
	/* Helpers																 */
	/*************************************************************************/

	// Compute distances between all Actors
	void computeDistancesBetweenActors();

private:
	/*************************************************************************/
	/* Data Members														     */
	/*************************************************************************/

	// Time
	unsigned long			m_nTick;

	unsigned long			m_nTickLastProtesterAdded = 0;

	unsigned int			m_nNumProtesters = 0;

	// Container of Actors
	std::vector<ActorPtr>	m_actors;

	std::vector<ActorPtr>	m_newActors;

	// Pointer to IceMan
	std::weak_ptr<IceMan>   m_pIceMan;

	// Distances between Actors
	std::unordered_map<ActorPtr, std::unordered_map<ActorPtr, int>> m_distances;

	// 2D Array for ice blocks 
	IcePtr m_ice[ICE_WIDTH][ICE_HEIGHT];
	
	// Number of Oil Barrels in the current level
	int m_iNumBarrels;

	// Tool for fast distance calculations between units
	DistanceCalculator m_distanceCalc;

	// Tools for computing shortest paths to the Exit and IceMan
	ShortestPathFinder m_shortestPathToExit;
	ShortestPathFinder m_shortestPathToIceMan;

	// Threads for performing computations 
	std::vector<ThreadPtr> m_pWorkerThreads;
};

#endif // STUDENTWORLD_H_


