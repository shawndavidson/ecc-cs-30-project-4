#include <cmath>
#include <assert.h>

#include "DistanceCalculator.h"

// Constructor
DistanceCalculator::DistanceCalculator() 
{
    // TODO: Optimize - the field is symmetric (square) so I think we can cut this by 50%
    //       The distance from square a to b is the same as b to as.
    for (int x0 = 0; x0 < VIEW_WIDTH; x0++) {
        for (int y0 = 0; y0 < VIEW_HEIGHT; y0++) {
            for (int x1 = 0; x1 < VIEW_WIDTH; x1++) {
                for (int y1 = 0; y1 < VIEW_HEIGHT; y1++) {
                    // Use Pythagorean Theorm to calculate distance between locations
                    m_table[x0][y0][x1][y1] = (byte)ceil(sqrt(pow((x1 - x0), 2) + pow((y1 - y0), 2)));
                }
            }
        }
    }

#if TEST_DISTANCECALCULATOR
    Test();
#endif //UNIT_TEST
}

// Destructor
DistanceCalculator::~DistanceCalculator() {
}

#if TEST_DISTANCECALCULATOR
// Test optimized table 
void DistanceCalculator::Test() {
    byte distance;
    byte answer;

    for (int x0 = 0; x0 < VIEW_WIDTH; x0++) {
        for (int y0 = 0; y0 < VIEW_HEIGHT; y0++) {
            for (int x1 = 0; x1 < VIEW_WIDTH; x1++) {
                for (int y1 = 0; y1 < VIEW_HEIGHT; y1++) {
                    distance = getDistance(x0, y0, x1, y1);
                    answer = (byte)ceil(sqrt(pow((x1 - x0), 2) + pow((y1 - y0), 2)));
                    assert(distance == answer);
                }
            }
        }
    }
}
#endif //UNIT_TEST

