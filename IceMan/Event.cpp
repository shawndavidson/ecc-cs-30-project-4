#include "Event.h"

std::ostream& operator<<(std::ostream& out, const EventBase& e) {
    out << "Event @" << e.getTick() << "of type " << e.getType() << std::endl;
    return out;
}


