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

private:
    // Perform initialization
    void init();

    /*************************************************************************/
    /* Data Members															 */
    /*************************************************************************/
private:
    struct Coordinates {
        // Constructor
        Coordinates(int x, int y, int distance) : x(x), y(y), distance(distance) {}

        int x, y;
        int distance;
    };

    byte m_distances[VIEW_WIDTH][VIEW_HEIGHT];
    
    StudentWorld* m_pStudentWorld;
};

#endif // SHORTESTPATHFINDER_H_

