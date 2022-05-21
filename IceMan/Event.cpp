#include "Event.h"

using namespace std;

// Stream Operator
ostream& operator<<(ostream& out, const Event& e) {
    out << "Event @" << e.getTick() << "of type " << e.getType() << endl;
    return out;
}


// Constructor
Event::Event(int tick, Event::Types nType) 
:   m_nTick(tick), 
    m_nType(nType)
{
}

// Destructor
Event::Event(const Event& rhs)
{
    if (this == &rhs)
        return;

    m_nTick     = rhs.m_nTick;
    m_nType     = rhs.m_nType;
}

// Destructor
Event::~Event() {
}

// Assignment Operator
Event& Event::operator=(const Event& rhs) {
    if (this == &rhs)
        return *this;

    // Use copy-swap idiom 
    Event e(rhs);
    e.swap(*this);

    return *this;
}

// Swap state with another instance of Event
void Event::swap(Event& rhs) {
    // Swap ticks
    auto nTickTemp = m_nTick;
    m_nTick = rhs.m_nTick;
    rhs.m_nTick = nTickTemp;

    // Swap types
    auto nTypeTemp = m_nType;
    m_nType = rhs.m_nType;
    rhs.m_nType = nTypeTemp;
}

