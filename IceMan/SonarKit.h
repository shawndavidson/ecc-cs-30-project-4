#ifndef SONAR_KIT_H_
#define SONAR_KIT_H_

#include "Goodie.h"

const double	SONAR_KIT_SIZE		= 1.00;
const int		SONAR_KIT_DEPTH		= 2;
const int		SONAR_KIT_POINTS	= 75;

class SonarKit : public Goodie
{
public:
	/*************************************************************************/
	/* Construction														     */
	/*************************************************************************/
	// Constructor
	SonarKit(StudentWorld* pStudentWorld,
		int startX,
		int startY,
		bool isVisible);

	// Destructor
	~SonarKit();

	/*************************************************************************/
	/* Operations													     */
	/*************************************************************************/
	void doSomething();

	// Handles when a Sonar Kit is collected
	void collect();

	/*************************************************************************/
	/* Getters/Setters													     */
	/*************************************************************************/
private:
	/*************************************************************************/
	/* Data Members													     */
	/*************************************************************************/
};

#endif SONAR_KIT_H_
