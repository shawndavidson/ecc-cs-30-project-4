#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>
#include <memory>

#include "Actor.h"
#include "Ice.h"

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

using namespace std;

// Type Definition(s)
typedef shared_ptr<Actor>	ActorPtr;
typedef shared_ptr<Ice>		IcePtr;

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
	std::vector<ActorPtr>	m_pActors;

	// 2D Array for ice blocks 
	IcePtr					m_ice[VIEW_WIDTH][VIEW_HEIGHT];
};

#endif // STUDENTWORLD_H_
