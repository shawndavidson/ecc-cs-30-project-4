#include <queue>

#include "StudentWorld.h"
#include "ShortestPathFinder.h"

// Constructor
ShortestPathFinder::ShortestPathFinder(StudentWorld* pStudentWorld)
: m_pStudentWorld(pStudentWorld),
  m_originX(0), 
  m_originY(0),
  m_distances{UCHAR_MAX}
{
}

// Compute distances from all units to a specific location
bool ShortestPathFinder::compute(int x, int y) {
    if (x < 0 || x >= VIEW_WIDTH || y < 0 || y >= VIEW_HEIGHT) {
        return false;
    }

    m_originX = x;
    m_originY = y;

    // Reset distances to our representation of infinity
    memset(m_distances, UCHAR_MAX, VIEW_WIDTH * VIEW_HEIGHT * sizeof(uint8_t));

    std::queue<Coordinates> queue;

    // Add the initial location which is our reference when measuring distance
    queue.emplace(x, y, 0);

    // Perform a BFS (Breadth First Search)
    // TODO: Make multithreaded
    while (!queue.empty()) {
        Coordinates unit = queue.front();
        queue.pop();

        // TODO: remove kludge - this never should have made it into the queue
        if (m_distances[unit.x][unit.y] != UCHAR_MAX)
            continue;

        // Store distance
        m_distances[unit.x][unit.y] = unit.distance;

        // Explore adjacent units, if valid, unoccupied, and unexplored
        for (int dir = GraphObject::Direction::up; dir <= GraphObject::Direction::right; dir++) {
            switch (dir) {
                case GraphObject::Direction::up:
                    // If there's a path above that isn't blocked AND the distance is still unknown
                    if (y + 1 < ICE_HEIGHT && 
                        m_distances[unit.x][unit.y + 1] == UCHAR_MAX &&
                        !getStudentWorld()->isBlocked(unit.x, unit.y + 1)) {
                        queue.emplace(unit.x, unit.y + 1, unit.distance + 1);
                    }
                    break; 
                case GraphObject::Direction::down:
                    // If there's a path below that isn't blocked AND the distance is still unknown
                    if (y - 1 >= 0 && 
                        m_distances[unit.x][unit.y - 1] == UCHAR_MAX &&
                        !getStudentWorld()->isBlocked(unit.x, unit.y - 1)) {
                        queue.emplace(unit.x, unit.y - 1, unit.distance + 1);
                    }
                    break;
                case GraphObject::Direction::left:
                    // If there's a path on the left that isn't blocked AND the distance is still unknown
                    if (x - 1 >= 0 &&
                        m_distances[unit.x - 1][unit.y] == UCHAR_MAX &&
                        !getStudentWorld()->isBlocked(unit.x - 1, unit.y)) {
                        queue.emplace(unit.x - 1, unit.y, unit.distance + 1);
                    }
                    break;
                case GraphObject::Direction::right:
                    // If there's a path on the right that isn't blocked AND the distance is still unknown
                    if (x + 1 < VIEW_WIDTH &&
                        m_distances[unit.x + 1][unit.y] == UCHAR_MAX &&
                        !getStudentWorld()->isBlocked(unit.x + 1, unit.y)) {
                        queue.emplace(unit.x + 1, unit.y, unit.distance + 1);
                    }
                    break;
            }
        }
    }

    return true;
}

// Get the direction and distance of the shortest path to the location previously 
// set by compute()
bool ShortestPathFinder::getShortestPath(int x, int y, DirectionDistance& result) const {
    std::vector<DirectionDistance> directions;

    // Grab the squares around our the location x,y
    for (int d = GraphObject::Direction::up; d <= GraphObject::Direction::right; d++) {
        GraphObject::Direction dir = (GraphObject::Direction)d;

        switch (dir) {
            case GraphObject::Direction::up:
                if (y + 1 < VIEW_HEIGHT)
                    directions.emplace_back(dir, m_distances[x][y + 1]);
                break;
            case GraphObject::Direction::down:
                if (y - 1 >= 0)
                    directions.emplace_back(dir, m_distances[x][y - 1]);
                break;
            case GraphObject::Direction::left:
                if (x - 1 >= 0)
                    directions.emplace_back(dir, m_distances[x - 1][y]);
                break;
            case GraphObject::Direction::right:
                if (x + 1 < VIEW_WIDTH)
                    directions.emplace_back(dir, m_distances[x + 1][y]);
                break;
        }
    }

    // Sort them in descending order of distance
    sort(begin(directions), end(directions), [](const DirectionDistance& dd1, const DirectionDistance& dd2) {
        return dd1.distance < dd2.distance;
    });

    // Return the object with the shortest distance
    if (!directions.empty()) {
        result = directions[0];
        return true;
    }

    // Unable to move in any direction
    return false;
}
