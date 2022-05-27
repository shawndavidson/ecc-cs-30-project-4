#include <string>
#include <typeinfo>

#include "StudentWorld.h"
#include "IceMan.h"
#include "Actor.h"
#include "Ice.h"
#include "OilBarrel.h"
#include "Gold.h"
#include "SonarKit.h"
#include "WaterPool.h"

using namespace std;

const int ICEMAN_TO_ICE_SIZE_RATIO = (int)(ICEMAN_SIZE / ICE_SIZE);

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
// Constructor
StudentWorld::StudentWorld(std::string assetDir)
	: GameWorld(assetDir)
{
}

// Perform initializion
int StudentWorld::init()
{
	// Initialize ice field - only uses lower 60 squares of screen
	for (int x = 0; x < ICE_WIDTH; x++) {
		for (int y = 0; y < ICE_HEIGHT; y++) {
			// If we're within the tunnel, skip laying ice
			if (y > 0 && x >= 30 && x <= 33)
				continue;

			try {
				// Instantiate an ice block
				m_ice[x][y] = make_shared<Ice>(this, x, y);
			}
			catch (bad_alloc& /*ex*/) {
				cout << "Unable to allocate memory for an ice block" << endl;
			}
		}
	}

	// Initialize IceMan
	shared_ptr<IceMan> pIceMan;

	try {
		// Instantiate IceMan
		pIceMan = make_shared<IceMan>(this);

		// Retain a weak pointer so we can easily reach IceMan when necessary
		m_pIceMan = pIceMan;

		m_actors.push_back(pIceMan);
	}
	catch (bad_alloc& /*ex*/) {
		cout << "Unable to allocate memory for IceMan" << endl;
	}

	// Initialize OilBarrels
	// Maybe handle this with a helper function?
	shared_ptr<OilBarrel> pOilBarrel;
	int NUM_OIL_BARRELS = 5; // TODO: how many should spawn at the start?
	setNumBarrels(NUM_OIL_BARRELS);
	for (int i = 0; i < NUM_OIL_BARRELS; ++i) {
		try {
			pOilBarrel = make_shared<OilBarrel>(this, getRandomX(), getRandomY());
			m_actors.push_back(pOilBarrel);
		}
		catch (bad_alloc&) {
			cout << "Unable to allocate memory for Oil Barrel" << endl;
		}
	}

	// Initialize Gold Nuggets
	shared_ptr<Gold> pGold;
	int NUM_GOLD_NUGGETS = 1; // TODO: how many should spawn at the start?
	for (int i = 0; i < NUM_GOLD_NUGGETS; ++i) {
		try {
			pGold = make_shared<Gold>(this, getRandomX(), getRandomY(), true, true, false, true);

			m_actors.push_back(pGold);
		}
		catch (bad_alloc&) {
			cout << "Unable to allocate memory for Gold Nugget" << endl;
		}
	}
	
	// Initialize Sonar Kits
	shared_ptr<SonarKit> pSonar;
	int NUM_SONAR_KITS = 1; // TODO: how many should spawn at the start?
	for (int i = 0; i < NUM_SONAR_KITS; ++i) {
		try {
			pSonar = make_shared<SonarKit>(this, getRandomX(), getRandomY(), true);

			m_actors.push_back(pSonar);
		}
		catch (bad_alloc&) {
			cout << "Unable to allocate memory for Sonar Kit" << endl;
		}
	}

	// Initialize Water Pools
	shared_ptr<WaterPool> pWaterPool;
	int NUM_WATER_POOLS = 1; // FIXME how many should spawn at the start?
	for (int i = 0; i < NUM_SONAR_KITS; ++i) {
		try {
			pWaterPool = make_shared<WaterPool>(this, getRandomX(), getRandomY());

			m_actors.push_back(pWaterPool);
		}
		catch (bad_alloc&) {
			cout << "Unable to allocate memory for Water Pool" << endl;
		}
	}

	return GWSTATUS_CONTINUE_GAME;
}

// Handle movement for all game objects within our world
int StudentWorld::move()
{
	// This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
	// Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
	decLives();



	// Give ALL Actors a chance to do something during this tick
	for (auto actor : m_actors) {
		// TODO: Do we call doSomething() if it's not alive?
		if (actor != nullptr) {
			// Calculate distance to IceMan, send to Actor
			actor->setRadiusIceMan(calcDistanceSq(actor, m_pIceMan.lock()));
			// Check if IceMan can pick up the object
			// If yes, then this is a Goodie
			if (actor->canPickupIM()) {
				actor->doSomething();
				pickupGoodieIM(actor, *(m_pIceMan.lock()));
			}
			// Else, this is not a Goodie
			else {
				actor->doSomething();
			}
		}
	}

	{
		shared_ptr<IceMan> pIceManShared = m_pIceMan.lock();

		const int x = pIceManShared->getX();
		const int y = pIceManShared->getY();

		// Is IceMan standing on ice?
		if (x < ICE_WIDTH && y < ICE_HEIGHT) {

			// Dig through the 4x4 matrix of ice that we're standing on 
			for (int yOffset = 0; yOffset < ICEMAN_TO_ICE_SIZE_RATIO; yOffset++) {
				int finalY = y + yOffset;

				for (int xOffset = 0; xOffset < ICEMAN_TO_ICE_SIZE_RATIO && finalY < ICE_HEIGHT; xOffset++) {
					int finalX = x + xOffset;
					// If ice is present, kill it
					if (finalX < ICE_WIDTH && m_ice[finalX][finalY]) {
						m_ice[finalX][finalY]->setAlive(false);
					}
				}
			}

		}

	}

	// Give the ice a chance to do something during this tick
	// TODO: Does ice need to do anything?
	for (int x = 0; x < ICE_WIDTH; x++) {
		for (int y = 0; y < ICE_HEIGHT; y++) {
			if (m_ice[x][y] != nullptr) {
				m_ice[x][y]->doSomething();
			}
		}
	}


	return GWSTATUS_CONTINUE_GAME;
}

// Cleanup game objects (deallocates memory)
void StudentWorld::cleanUp()
{
	// Release memory for all Actors
	for (auto actor : m_actors) {
		actor.reset();
	}

	// Release memory for all Ice blocks
	for (int x = 0; x < ICE_WIDTH; x++) {
		for (int y = 0; y < ICE_HEIGHT; y++) {
			if (m_ice[x][y] != nullptr) {
				m_ice[x][y].reset();
			}
		}
	}
}

void StudentWorld::removeDeadGameObjects() {
	// TODO
}

// Creates random x coordinate for actors to spawn in
// Will not spawn in tunnel
int StudentWorld::getRandomX() {
	int x = rand() % 60;
	while (x >= 27 && x <= 33)
		x = rand() % 60;
	return x;
}

// Creates random y coordinate for actors to spawn in
int StudentWorld::getRandomY() {
	return rand() % 56;
}

// Detects collision between two actors
bool StudentWorld::detectCollison(ActorPtr pA1, ActorPtr pA2, double distance) {
	// If the distance is less than 9, there has been a collision
	if (distance <= 9 && pA1 != pA2)
		return true;
	return false;
}

// Calculates the distance squared between any two actors
// Using distance squared avoid an extra operation
double StudentWorld::calcDistanceSq(ActorPtr pA1, ActorPtr pA2) {
	if (pA1 == pA2)
		return NULL;
	return pow(pA1->getX() - pA2->getX(), 2) + pow(pA1->getY() - pA2->getY(), 2);
}


// GOODIE FUNCTIONS

// Sets the number of oil barrels in the oil field
void StudentWorld::setNumBarrels(int barrels) {
	m_num_barrels = barrels;
}

// Returns the number of oil barrels remaining in the field
int StudentWorld::getNumBarrels() {
	return m_num_barrels;
}

// Decrements the number of oil barrels remaining in the field
void StudentWorld::decNumBarrels() {
	--m_num_barrels;
}

// Handles the case where any Goodie is picked up
void StudentWorld::pickupGoodieIM(ActorPtr actor, IceMan& iceman) {
	// If there is a collision, increase the score and play a sound
	// Depending on the goodie, a different action occurs
	// Uses the image ID to identify the goodie
	if (detectCollison(actor, m_pIceMan.lock(), actor->getRadiusIceMan())) {
		
		switch (actor->getID()) {
		case IID_BARREL:
			decNumBarrels();
			if (getNumBarrels() == 0) {
				// FIXME level increases when all barrels are collected
				// TODO: Where should the level be incremented?
				cout << "LEVEL UP" << endl; // FIXME
			}
			break;
		case IID_GOLD:
			iceman.incGold();
			break;
		case IID_SONAR:
			iceman.incSonarKits();
			break;
		case IID_WATER_POOL:
			iceman.incWater();
			break; 
		}
		increaseScore(actor->getPoints());
		playSound(actor->getSoundEffect());
	}
}