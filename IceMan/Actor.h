#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Actor {
public:
	Actor() {}
	virtual ~Actor() {}

	virtual void doSomething() = 0;
};

#endif // ACTOR_H_
