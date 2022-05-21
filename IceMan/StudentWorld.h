#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Event.h"
#include <string>
#include <vector>
#include <memory>
#include <queue>
#include <map>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

using namespace std;

// Forward Declarations
class Actor;
class IceMan;
class Ice;

/*************************************************************************/
/* Type Declaration(s)														     */
/*************************************************************************/
typedef shared_ptr<Actor>			ActorPtr;
typedef shared_ptr<Ice>				IcePtr;
typedef std::function<void(void)>	EventCallback;

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

	// Schedule a new Event
	void pushEvent(const Event& e)			{ m_events.push(e); }

	// Get the Event with the smallest tick
	Event topEvent()						{ return m_events.top(); }

	// Is there another Event in the queue?
	bool isEventQueueEmpty() const			{ return m_events.empty(); }

	// Register for an Event
	void listenForEvent(Event::Types type, EventCallback callback);

private:
	/*************************************************************************/
	/* Helpers																 */
	/*************************************************************************/

	// Process the next Event
	void processNextEvent();

private:
	/*************************************************************************/
	/* Data Members														     */
	/*************************************************************************/

	// Time
	unsigned long m_nTick;

	// Container of Actors
	std::vector<ActorPtr>	m_actors;
	std::weak_ptr<IceMan>   m_pIceMan;

	// 2D Array for ice blocks 
	IcePtr m_ice[ICE_WIDTH][ICE_HEIGHT];


	// Declaration for a Function Object to compare two Events 
	// in descending order by their tick (time)
	class EventComparator
	{
	public:
		// Function operator
		int operator() (const Event& e1, const Event& e2)
		{
			return e1.getTick() > e2.getTick();
		}
	};

	// Event Queue (Min Heap)
	std::priority_queue<Event, vector<Event>, StudentWorld::EventComparator> m_events;

	// Registry of Event Listeners
	std::multimap<Event::Types, EventCallback> m_eventListeners;
};

#endif // STUDENTWORLD_H_
