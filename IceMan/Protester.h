#ifndef PROTESTOR_H_
#define PROTESTOR_H_

#include <vector>
#include <random>

#include "Person.h"

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
	Protester&		operator=(const Protester&) = delete;

	/*************************************************************************/
	/* Operations													     */
	/*************************************************************************/
	virtual void	doSomething();
	virtual void	annoy(int nHitPoints);

	// Step on a Gold Nugget
	virtual void	pickUpGold()  = 0;

	// Protester is bonked by a Boulder
	virtual void	bonkedByBoulder();


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
	void			leave()				{ m_nLeaveTheOilField = true;  }

	// Check if we're leaving the oil field
	bool			isLeaving() const	{ return m_nLeaveTheOilField;  }

	// Pause the Protester for N ticks
	void			pause(int nTicks)	{ m_nTicksStunned = nTicks; }

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



#endif // PROTESTOR_H_
