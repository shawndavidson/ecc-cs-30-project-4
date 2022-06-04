#ifndef SHORTESTPATHFINDER_H_
#define SHORTESTPATHFINDER_H_

#include "GameConstants.h"
#include "GraphObject.h"

#define TEST_SHORTESTPATHFINDER 0

// Forward Declaration
class StudentWorld;

struct DirectionDistance {
    // Constructor
    DirectionDistance(GraphObject::Direction direction = GraphObject::Direction::none, size_t distance = 0)
        : direction(direction), distance(distance) {}

    GraphObject::Direction  direction;
    size_t                  distance;
};

/*************************************************************************/
/* The purpose of this class is to help us find the distance from a given*/
/* location to all other locations in a maze                             */      
/*************************************************************************/
class ShortestPathFinder
{
public:
    /*************************************************************************/
    /* Construction															 */
    /*************************************************************************/
    // Constructor
    ShortestPathFinder(const StudentWorld* pStudentWorld);

    // Disallow the Copy Constructor (expensive)
    ShortestPathFinder(const ShortestPathFinder& rhs) = delete;

    // Destructor
    ~ShortestPathFinder() {};

    /*************************************************************************/
    /* Getters/Setters															 */
    /*************************************************************************/
    // Get a pointer to StudentWorld
    const StudentWorld* getStudentWorld()   { return m_pStudentWorld;  }

    // Get the current X location
    int getX() const                        { return m_originX;  }

    // Get the current Y location
    int getY() const                        { return m_originY; }


    /*************************************************************************/
    /* Operations															 */
    /*************************************************************************/
    // Disallow the Assignment Operator (expensive)
    ShortestPathFinder& operator=(const ShortestPathFinder&) = delete;

    // Compute distances from all squares relative to location (x,y)
    bool compute(int x, int y);

    // Get the direction that has the shortest direction
    bool getShortestPath(int x, int y, DirectionDistance& result) const;

    void dump() const;

    /*************************************************************************/
    /* Data Members															 */
    /*************************************************************************/
private:
    struct Coordinates {
        // Constructor
        Coordinates(uint8_t x, uint8_t y, size_t distance) : x(x), y(y), distance(distance) {}

        uint8_t x, y;
        size_t  distance;
    };

    const StudentWorld*   m_pStudentWorld;

    // The location from which we want to calculate distances
    uint8_t         m_originX;
    uint8_t         m_originY;

    size_t          m_distances[VIEW_WIDTH][VIEW_HEIGHT];
};

#endif // SHORTESTPATHFINDER_H_

