#ifndef ROBO_ASSISTANT_H
#define ROBO_ASSISTANT_H

#include "brain.h"

class RoboAssistant
{
private:
	Brain* brain;
public:
	RoboAssistant(Brain* brain);
	virtual ~RoboAssistant();
};

#endif
