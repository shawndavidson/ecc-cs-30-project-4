#ifndef EVENT_H_
#define EVENT_H_

#include <functional>
#include <iostream>
#include <memory>

/*************************************************************************/
/* Types														         */
/*************************************************************************/
enum EventTypes {
    EVENT_TEST,         // See EventTestData
    EVENT_ALWAYS_LAST
};

// TODO: Remove - for testing
// For use with EventTypes::EVENT_TEST 
struct EventTestData {
    int         num;
    const char* text; 
};

class EventBase 
{
public:
    /*************************************************************************/
    /* Construction														     */
    /*************************************************************************/
    // Constructor
    EventBase(int tick, EventTypes nType)
    : m_nTick(tick), m_nType(nType) 
    {
    }
    
    // Copy Constructor
    EventBase(const EventBase& rhs)
    {
        if (this == &rhs)
            return;

        m_nTick = rhs.m_nTick;
        m_nType = rhs.m_nType;
    }

    // Destructor
    virtual ~EventBase() {}


    /*************************************************************************/
    /* Getters/Setters														 */
    /*************************************************************************/
    // Get Tick
    unsigned long getTick() const { return m_nTick; }
    
    // Get Type
    EventTypes getType() const { return m_nType; }

    /*************************************************************************/
    /* Operators														     */
    /*************************************************************************/
    // Assignment Operator
    EventBase& operator=(const EventBase& rhs) {
        if (this == &rhs)
            return *this;

        // Use copy-swap idiom 
        EventBase e(rhs);
        e.swap(*this);

        return *this;
    }

    /*************************************************************************/
    /* Operations														     */
    /*************************************************************************/
    // Swap state with another instance of Event
    void swap(EventBase& rhs) {
        // Swap ticks
        auto nTickTemp  = m_nTick;
        m_nTick         = rhs.m_nTick;
        rhs.m_nTick     = nTickTemp;

        // Swap types
        auto nTypeTemp  = m_nType;
        m_nType         = rhs.m_nType;
        rhs.m_nType     = nTypeTemp;
    }

private:
    /*************************************************************************/
    /* Data Members														     */
    /*************************************************************************/
    unsigned long   m_nTick;
    EventTypes      m_nType;
};

/*************************************************************************/
/* Types														 */
/*************************************************************************/
typedef std::shared_ptr<EventBase> SharedEventPtr;

// Stream operator
std::ostream& operator<<(std::ostream& out, const EventBase& e);

template <typename T>
class Event : public EventBase
{
public:
    /*************************************************************************/
    /* Construction														     */
    /*************************************************************************/
    // Constructor
    Event(int tick, EventTypes nType, const T& data)
    :   EventBase(tick, nType),
        m_data(data)
    {
    }

    // Copy Constructor
    Event(const Event& rhs)
    {
        if (this == &rhs)
            return;

        m_nTick = rhs.m_nTick;
        m_nType = rhs.m_nType;
        m_data  = rhs.m_data;
    }

    // Destructor
    virtual ~Event() {}

    /*************************************************************************/
    /* Getters/Setters														 */
    /*************************************************************************/
    // Get Tick
    unsigned long   getTick() const     { return m_nTick; }
    
    // Get Type
    EventTypes      getType() const     { return m_nType; }

    // Get Data 
    const T&        getData() const     { return m_data;  }

    /*************************************************************************/
    /* Operators														     */
    /*************************************************************************/
    // Assignment Operator
    Event& operator=(const Event& rhs) {
        if (this == &rhs)
            return *this;

        // Use copy-swap idiom 
        Event e(rhs);
        e.swap(*this);

        return *this;
    }

    /*************************************************************************/
    /* Operations														     */
    /*************************************************************************/
    // Swap state with another instance of Event
    void swap(Event& rhs) {
        EventBase::swap(rhs);

        // Swap Data
        auto pDataTemp  = m_data;
        m_data          = rhs.m_data;
        rhs.m_data      = pDataTemp;
    }

private:
    /*************************************************************************/
    /* Data Members														     */
    /*************************************************************************/
    const T m_data;
};

#endif // EVENT_H_

