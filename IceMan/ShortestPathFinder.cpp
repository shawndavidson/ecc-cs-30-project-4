#include <queue>

#include "StudentWorld.h"
#include "ShortestPathFinder.h"

// Constructor
ShortestPathFinder::ShortestPathFinder(StudentWorld* pStudentWorld)
: m_pStudentWorld(pStudentWorld)
{
}

// Perform initialization
void ShortestPathFinder::init() {
    for (int x = 0; x < VIEW_WIDTH; x++) {
        for (int y = 0; y < VIEW_HEIGHT; y++) {
            m_distances[x][y] = UCHAR_MAX;
        }
    }
}

// Compute distances from all units to a specific location
bool ShortestPathFinder::compute(int x, int y) {
    if (x < 0 || x >= VIEW_WIDTH || y < 0 || y >= VIEW_HEIGHT) {
        return false;
    }

    init();

    std::queue<Coordinates> queue;

    // Add the initial location which is our reference when measuring distance
    queue.emplace(x, y, 0);

    // Perform a BFS (Breadth First Search)
    // TODO: Make multithreaded
    while (!queue.empty()) {
        const Coordinates& unit = queue.front();
        queue.pop();

        // Store distance
        m_distances[unit.x][unit.y] = unit.distance;

        // Explore adjacent units, if valid, unoccupied, and unexplored
        for (int dir = GraphObject::Direction::up; dir <= GraphObject::Direction::right; dir++) {
            switch (dir) {
                case GraphObject::Direction::up:
                    // If there's a path above that isn't blocked AND the distance is still unknown
                    if (!getStudentWorld()->isBlocked(unit.x, unit.y + 1) && m_distances[unit.x][unit.y + 1] == INT_MAX) {
                        queue.emplace(unit.x, unit.y + 1, unit.distance + 1);
                    }
                    break; 
                case GraphObject::Direction::down:
                    // If there's a path below that isn't blocked AND the distance is still unknown
                    if (!getStudentWorld()->isBlocked(unit.x, unit.y - 1) && m_distances[unit.x][unit.y - 1] == INT_MAX) {
                        queue.emplace(unit.x, unit.y - 1, unit.distance + 1);
                    }
                    break;
                case GraphObject::Direction::left:
                    // If there's a path on the left that isn't blocked AND the distance is still unknown
                    if (!getStudentWorld()->isBlocked(unit.x - 1, unit.y) && m_distances[unit.x - 1][unit.y] == INT_MAX) {
                        queue.emplace(unit.x - 1, unit.y, unit.distance + 1);
                    }
                    break;
                case GraphObject::Direction::right:
                    // If there's a path on the right that isn't blocked AND the distance is still unknown
                    if (!getStudentWorld()->isBlocked(unit.x + 1, unit.y) && m_distances[unit.x + 1][unit.y] == INT_MAX) {
                        queue.emplace(unit.x + 1, unit.y, unit.distance + 1);
                    }
                    break;
            }
        }
    }

    return true;
}

// Get the direction that has the shortest direction
GraphObject::Direction ShortestPathFinder::getShortestPath(int x, int y) {

    struct DirectionDistance {
        DirectionDistance(GraphObject::Direction direction, byte distance)
            : direction(direction), distance(distance) {}

        GraphObject::Direction  direction;
        byte                    distance;
    };

    std::vector<DirectionDistance> directions;

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

    sort(begin(directions), end(directions), [](const DirectionDistance& dd1, const DirectionDistance& dd2) {
        return dd1.distance < dd2.distance;
    });

    return directions[0].direction;
}