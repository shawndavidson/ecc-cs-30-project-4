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
class IceMan;
class Ice;

/*************************************************************************/
/* Type Declaration(s)														     */
/*************************************************************************/
typedef shared_ptr<Actor>	ActorPtr;
typedef shared_ptr<Ice>		IcePtr;

/*************************************************************************/
/* Construction														     */
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

private:
	/*************************************************************************/
	/* Data Members														     */
	/*************************************************************************/

	// Container of Actors
	std::vector<ActorPtr>	m_actors;
	std::weak_ptr<IceMan>   m_pIceMan;

	// 2D Array for ice blocks 
	IcePtr m_ice[ICE_WIDTH][ICE_HEIGHT];
};

#endif // STUDENTWORLD_H_
