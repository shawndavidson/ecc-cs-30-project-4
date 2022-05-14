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
	StudentWorld(std::string assetDir)
		: GameWorld(assetDir)
	{
	}

	virtual int init()
	{
		return GWSTATUS_CONTINUE_GAME;
	}

	virtual int move()
	{
		// This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
		// Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}

	virtual void cleanUp()
	{
		// Release memory for all Actors
		for (auto actor : m_pActors) {
			actor.reset();
		}

		// Release memory for all Ice blocks
		for (int i = 0; i < VIEW_WIDTH; i++) {
			for (int j = 0; j < VIEW_HEIGHT; j++) {
				if (m_ice[i][j] != nullptr) {
					m_ice[i][j].reset();
				}
			}
		}
	}

private:
	std::vector<ActorPtr>	m_pActors;
	IcePtr					m_ice[VIEW_WIDTH][VIEW_HEIGHT];
};

#endif // STUDENTWORLD_H_
