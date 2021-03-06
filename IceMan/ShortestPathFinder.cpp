#include <queue>
#include <iomanip>

#include "StudentWorld.h"
#include "ShortestPathFinder.h"

// Constructor
ShortestPathFinder::ShortestPathFinder(const StudentWorld* pStudentWorld)
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
    memset(m_distances, UINT_MAX, VIEW_WIDTH * VIEW_HEIGHT * sizeof(size_t));

    std::queue<Coordinates> queue;

    // Add the initial location which is our reference when measuring distance
    queue.emplace(x, y, 0);

#if TEST_SHORTESTPATHFINDER
    size_t maxQueueSize = 0;
    size_t minX{}, maxX{}, minY{}, maxY{};
#endif

    // Perform a BFS (Breadth First Search)
    while (!queue.empty()) {
        Coordinates unit = queue.front();
        queue.pop();

#if TEST_SHORTESTPATHFINDER
        minX = std::min<uint8_t>(minX, unit.x);
        maxX = std::max<uint8_t>(maxX, unit.x);
        minY = std::min<uint8_t>(minY, unit.y);
        maxY = std::max<uint8_t>(maxY, unit.y);
#endif
        // KLUDGE: Without this statement, the queue grows larger than it should 
        // >4096 (64*64) causing a progressive slow down to game play that seems
        // to be correlated to how many tunnels IceMan digs.
        if (m_distances[unit.x][unit.y] != UINT_MAX)
            continue;

        // Store distance
        m_distances[unit.x][unit.y] = unit.distance;

        // Explore adjacent units, if valid, unoccupied, and unexplored
        for (int direction = GraphObject::Direction::up; 
            direction <= GraphObject::Direction::right; 
            direction++) {

            switch (direction) {
            case GraphObject::Direction::up:
            {
                if (x < 0 || x >(ICE_WIDTH - PERSON_SIZE) ||
                    y < 0 || y + 1 > ICE_HEIGHT) {
                    continue;
                }
            }
            break;
            case GraphObject::Direction::down:
            {
                if (x < 0 || x >(ICE_WIDTH - PERSON_SIZE) ||
                    y - 1 < 0 || y > ICE_HEIGHT) {
                    continue;
                }
            }
            break;
            case GraphObject::Direction::left:
            {
                if (x - 1 < 0 || x > (ICE_WIDTH - PERSON_SIZE) ||
                    y < 0 || y > ICE_HEIGHT) {
                    continue;
                }
            }
            break;
            case GraphObject::Direction::right:
            {
                if (x < 0 || x + 1 >(ICE_WIDTH - PERSON_SIZE) ||
                    y < 0 || y > ICE_HEIGHT) {
                    continue;
                }
            }
            break;
            };

            switch (direction) {
                case GraphObject::Direction::up:
                    // If there's a path above that isn't blocked AND the distance is still unknown
                    if (!getStudentWorld()->isBlocked(unit.x, unit.y, (GraphObject::Direction)direction) &&
                        unit.distance + 1 < m_distances[unit.x][unit.y + 1]) {
                        queue.emplace(unit.x, unit.y + 1, unit.distance + 1);
                    }
                    break; 
                case GraphObject::Direction::down:
                    // If there's a path below that isn't blocked AND the distance is still unknown
                    if (!getStudentWorld()->isBlocked(unit.x, unit.y, (GraphObject::Direction)direction) &&
                        unit.distance + 1 < m_distances[unit.x][unit.y - 1]) {
                        queue.emplace(unit.x, unit.y - 1, unit.distance + 1);
                    }
                    break;
                case GraphObject::Direction::left:
                    // If there's a path on the left that isn't blocked AND the distance is still unknown
                    if (!getStudentWorld()->isBlocked(unit.x, unit.y, (GraphObject::Direction)direction) &&
                        unit.distance + 1 < m_distances[unit.x - 1][unit.y]) {
                        queue.emplace(unit.x - 1, unit.y, unit.distance + 1);
                    }
                    break;
                case GraphObject::Direction::right:
                    // If there's a path on the right that isn't blocked AND the distance is still unknown
                    if (!getStudentWorld()->isBlocked(unit.x, unit.y, (GraphObject::Direction)direction) &&
                        unit.distance + 1 < m_distances[unit.x + 1][unit.y]) {
                        queue.emplace(unit.x + 1, unit.y, unit.distance + 1);
                    }
                    break;
            }
        }

#if TEST_SHORTESTPATHFINDER
        maxQueueSize = std::max<unsigned int>(maxQueueSize, queue.size());
#endif
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

// Dump the distance matrix to the console
void ShortestPathFinder::dump() const {
    cout << "ShortestPathFinder::dump()" << endl;

    for (int y = VIEW_HEIGHT - 1; y >= 0; y--) {
        for (int x = 0; x < VIEW_WIDTH; x++) {
            cout << std::setw(2) << std::hex << (int)m_distances[x][y] << " ";
        }
        cout << endl;
    }
    cout << endl << endl;
}
