#ifndef PROTESTOR_H_
#define PROTESTOR_H_

#include <vector>
#include <random>

#include "Actor.h"

class Protester : public Actor
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
		int startY);

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
	virtual void	annoy();

protected:
	// Take one step towards the exit 
	void			moveTowardsExit();

	// Take one step towards IceMan 
	void			moveTowardsIceMan();

	// Take one step in the given direction
	bool			takeOneStep(GraphObject::Direction direction);

	// Get number of squares to move before turning
	int				getNumSquaresToMoveInCurrentDirection() const;

	// Get possible perpendicular turns relative to the current direction
	bool			getPossiblePerpendicularDirections(std::vector<GraphObject::Direction>& directions);

	// Can we shout at IceMan
	bool			canShoutAtIceMan();

	// Shout at IceMan
	void			shout();

	// Leave the oil field
	void			leave()				{ m_nLeaveTheOilField = true;  }

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
};



#endif // PROTESTOR_H_
