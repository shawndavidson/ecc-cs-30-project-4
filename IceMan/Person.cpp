#include "Person.h"
#include "StudentWorld.h"

// Constructor
Person::Person(
    StudentWorld* pStudentWorld,
    int imageID,
    int startX,
    int startY,
    Direction dir,
    int nHitPoints)
: Actor(pStudentWorld,
    imageID,
    startX,
    startY,
    dir,
    1.0     /*size*/,
    0       /*depth*/,
    true    /*visible*/,
    true    /*canAnnoy*/,
    false   /*canPickup*/),
  m_nHitPoints(0) 
{
}

// Destructor
Person::~Person() {
}