#ifndef SHORTESTPATHFINDER_H_
#define SHORTESTPATHFINDER_H_

#include "GameConstants.h"
#include "GraphObject.h"

// Forward Declaration
class StudentWorld;

class ShortestPathFinder
{
public:
    /*************************************************************************/
    /* Construction															 */
    /*************************************************************************/
    // Constructor
    ShortestPathFinder(StudentWorld* pStudentWorld);

    // Destructor
    ~ShortestPathFinder() {};

    /*************************************************************************/
    /* Getters/Setters															 */
    /*************************************************************************/
    // Get a pointer to StudentWorld
    StudentWorld* getStudentWorld() { return m_pStudentWorld;  }

    /*************************************************************************/
    /* Operations															 */
    /*************************************************************************/

    // Compute distances from all units to a specific location
    bool compute(int x, int y);

    // Get the direction that has the shortest direction
    GraphObject::Direction getShortestPath(int x, int y);

    /*************************************************************************/
    /* Data Members															 */
    /*************************************************************************/
private:
    struct Coordinates {
        // Constructor
        Coordinates(byte x, byte y, byte distance) : x(x), y(y), distance(distance) {}

        byte x, y;
        byte distance;
    };

    byte m_distances[VIEW_WIDTH][VIEW_HEIGHT];

    StudentWorld* m_pStudentWorld;
};

#endif // SHORTESTPATHFINDER_H_

