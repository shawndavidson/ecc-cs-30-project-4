#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include <string>
#include <vector>
#include <memory>
#include <queue>
#include <map>
#include <unordered_map>

#include "GameWorld.h"
#include "GameConstants.h"
#include "Event.h"
#include "DistanceCalculator.h"

#define TEST_STUDENTWORLD 0

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

using namespace std;

// Forward Declarations
class Actor;
class IceMan;
class Ice;

/*************************************************************************/
/* Type Declaration(s)														     */
/*************************************************************************/
typedef shared_ptr<Actor>					ActorPtr;
typedef shared_ptr<Ice>						IcePtr;
typedef std::function<void(SharedEventPtr)>	EventCallback;


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

	// Prevent copying 
	StudentWorld(const StudentWorld&) = delete;

	virtual ~StudentWorld();

	/*************************************************************************/
	/* Operators													     */
	/*************************************************************************/
	// Prevent assignment 
	StudentWorld& operator=(const StudentWorld&) = delete;

	/*************************************************************************/
	/* Getters/Setters													     */
	/*************************************************************************/
	// Get the tick which represents time
	unsigned long getTick()	const { return m_nTick; }

	/*************************************************************************/
	/* Operations														     */
	/*************************************************************************/
    // Perform initializion
	virtual int init();

	// Handle movement for all game objects within our world
	virtual int move();

	// Cleanup game objects (deallocates memory)
	virtual void cleanUp();

	// Compute distance to IceMan
	int getDistanceToIceMan(int x, int y) const;

	// Check if these coordinates and direction are facing IceMan
	bool isFacingIceMan(int x, int y, int direction) const;

	bool hasPathToIceMan(int x, int y, unsigned int& direction) const;

	// Schedule a new Event
	void pushEvent(SharedEventPtr e)			{ m_events.push(e); }

	// Register for an Event
	void listenForEvent(EventTypes type, EventCallback callback);

private:
	// Get the Event with the smallest tick
	SharedEventPtr topEvent()					{ return m_events.top(); }

	// Is there another Event in the queue?
	bool isEventQueueEmpty() const				{ return m_events.empty(); }

private:
	/*************************************************************************/
	/* Helpers																 */
	/*************************************************************************/

	// Process the next Event
	void processNextEvent();

	void computeDistances();

private:
	/*************************************************************************/
	/* Data Members														     */
	/*************************************************************************/

	// Time
	unsigned long m_nTick;

	// Container of Actors
	std::vector<ActorPtr>	m_actors;

	// Pointer to IceMan
	std::weak_ptr<IceMan>   m_pIceMan;

	// Distances between Actors
	std::unordered_map<ActorPtr, std::unordered_map<ActorPtr, int>> m_distances;

	// 2D Array for ice blocks 
	IcePtr m_ice[ICE_WIDTH][ICE_HEIGHT];


	// Declaration for a Function Object to compare two Events 
	// in descending order by their tick (time)
	class EventComparator
	{
	public:
		// Function operator
		int operator() (const SharedEventPtr& e1, const SharedEventPtr& e2)
		{
			return e1->getTick() > e2->getTick();
		}
	};

	// Event Queue (Min Heap)
	// TODO: Make it thread-safe
	std::priority_queue<SharedEventPtr, vector<SharedEventPtr>, StudentWorld::EventComparator> m_events;

	// Registry of Event Listeners
	std::multimap<EventTypes, EventCallback> m_eventListeners;

	// Tool for fast distance calculations between cells
	DistanceCalculator m_distanceCalc;
};

#endif // STUDENTWORLD_H_
