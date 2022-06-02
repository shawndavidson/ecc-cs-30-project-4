#ifndef DISTANCECALCULATOR_H_
#define DISTANCECALCULATOR_H_

#include <cstdint>
#include "GameConstants.h"

// Set to 1 to ensure optimizations generate the correct results.
#define TEST_DISTANCECALCULATOR 0


class DistanceCalculator
{
public:
    /*************************************************************************/
    /* Construction														     */
    /*************************************************************************/
    // Constructor
    DistanceCalculator();

    // Copy Constructor (deleted - too expensive)
    DistanceCalculator(const DistanceCalculator& rhs) = delete;

    // Destructor
    ~DistanceCalculator();

    /*************************************************************************/
    /* Operators														     */
    /*************************************************************************/
    // Assignment Operator (deleted - too expensive)
    DistanceCalculator& operator=(const DistanceCalculator&) = delete;

    /*************************************************************************/
    /* Operations														     */
    /*************************************************************************/
#if TEST_DISTANCECALCULATOR
    // Test optimized table - debug only
    void Test();
#endif //UNIT_TEST

    // Get distance between two squares
    inline uint8_t getDistance(int x0, int y0, int x1, int y1) const 
    {
        return m_table[x0][y0][x1][y1];
    }

private:
    uint8_t m_table[VIEW_WIDTH][VIEW_HEIGHT][VIEW_WIDTH][VIEW_HEIGHT];
};

#endif // DISTANCECALCULATOR_H_

