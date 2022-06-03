#ifndef SHORTESTPATHFINDER_H_
#define SHORTESTPATHFINDER_H_

#include "GameConstants.h"
#include "GraphObject.h"

// Forward Declaration
class StudentWorld;

struct DirectionDistance {
    // Constructor
    DirectionDistance(GraphObject::Direction direction = GraphObject::Direction::none, uint8_t distance = 0)
        : direction(direction), distance(distance) {}

    GraphObject::Direction  direction;
    uint8_t                 distance;
};

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

    // Get the current X location
    int getX() const    { return m_originX;  }

    // Get the current Y location
    int getY() const    { return m_originY; }


    /*************************************************************************/
    /* Operations															 */
    /*************************************************************************/

    // Compute distances from all squares relative to location (x,y)
    bool compute(int x, int y);

    // Get the direction that has the shortest direction
    bool getShortestPath(int x, int y, DirectionDistance& result) const;

    /*************************************************************************/
    /* Data Members															 */
    /*************************************************************************/
private:
    struct Coordinates {
        // Constructor
        Coordinates(uint8_t x, uint8_t y, uint8_t distance) : x(x), y(y), distance(distance) {}

        uint8_t x, y;
        uint8_t distance;
    };

    StudentWorld* m_pStudentWorld;

    // The location from which we want to calculate distances
    uint8_t m_originX;
    uint8_t m_originY;

    uint8_t m_distances[VIEW_WIDTH][VIEW_HEIGHT];
};

#endif // SHORTESTPATHFINDER_H_

