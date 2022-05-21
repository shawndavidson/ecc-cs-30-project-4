#ifndef EVENT_H_
#define EVENT_H_

#include <functional>
#include <iostream>


class Event
{
public:
    /*************************************************************************/
    /* Types														 */
    /*************************************************************************/
    enum Types {
        EVENT_TEST,
        EVENT_ALWAYS_LAST
    };

    /*************************************************************************/
    /* Construction														     */
    /*************************************************************************/
    // Constructor
    Event(int tick, Event::Types type);
    
    // Copy Constructor
    Event(const Event& rhs);

    // Destructor
    virtual ~Event();


    /*************************************************************************/
    /* Getters/Setters														 */
    /*************************************************************************/
    // Get Tick
    unsigned long getTick() const                   { return m_nTick; }
    
    // Get Type
    Event::Types getType() const                    { return m_nType; }

    /*************************************************************************/
    /* Operators														     */
    /*************************************************************************/
    // Assignment Operator
    Event& operator=(const Event& rhs);

    /*************************************************************************/
    /* Operations														     */
    /*************************************************************************/
    // Swap internal state
    void swap(Event& rhs);

private:
    /*************************************************************************/
    /* Data Members														     */
    /*************************************************************************/
    unsigned long   m_nTick;
    Event::Types    m_nType;
};

std::ostream& operator<<(std::ostream& out, const Event& e);

#endif // EVENT_H_

