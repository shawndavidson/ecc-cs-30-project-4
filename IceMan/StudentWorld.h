#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>
#include <memory>

//#include "Actor.h"
//#include "Ice.h"

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

using namespace std;

// Forward Declarations
class Actor;
class Goodie;
class IceMan;
class Ice;
class OilBarrel;

/*************************************************************************/
/* Type Declaration(s)														     */
/*************************************************************************/
typedef shared_ptr<Actor>	ActorPtr;
typedef shared_ptr<Ice>		IcePtr;

/*************************************************************************/
/* Constants														     */
/*************************************************************************/
const int ICE_ROW_BEGIN = 4;
const int ICE_WIDTH		= VIEW_WIDTH;
const int ICE_HEIGHT	= VIEW_HEIGHT - ICE_ROW_BEGIN;

class StudentWorld : public GameWorld
{
public:
	/*************************************************************************/
	/* Construction														     */
	/*************************************************************************/
	StudentWorld(std::string assetDir);

	/*************************************************************************/
	/* Operations														     */
	/*************************************************************************/
    // Perform initializion
	virtual int init();

	// Handle movement for all game objects within our world
	virtual int move();

	// Cleanup game objects (deallocates memory)
	virtual void cleanUp();

	// Remove all dead game objects (deallocate memory)
	virtual void removeDeadGameObjects();

	// Creates random x coordinate for actors to spawn in
	// Will not spawn in tunnel
	int getRandomX();

	// Creates random y coordinate for actors to spawn in
	int getRandomY();

	// Handles collisions between all game objects
	bool detectCollison(ActorPtr, ActorPtr, double);

	// Returns the distance between two actors squared
	double calcDistanceSq(ActorPtr, ActorPtr);

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
	void pickupGoodieIM(ActorPtr, IceMan&);
private:
	/*************************************************************************/
	/* Data Members														     */
	/*************************************************************************/

	// Container of Actors
	std::vector<ActorPtr>	m_actors;
	std::weak_ptr<IceMan>   m_pIceMan;

	// 2D Array for ice blocks 
	IcePtr m_ice[ICE_WIDTH][ICE_HEIGHT];
	
	// Number of Oil Barrels in the current level
	int m_num_barrels;
};

#endif // STUDENTWORLD_H_
