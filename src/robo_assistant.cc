#include "robo_assistant.h"

RoboAssistant::RoboAssistant(Brain* brain) : brain(brain)
{
}

RoboAssistant::~RoboAssistants()
{
	delete this->brain;
}
