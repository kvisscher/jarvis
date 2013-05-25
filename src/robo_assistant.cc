#include "robo_assistant.h"

RoboAssistant::RoboAssistant(Brain* brain) : brain(brain)
{
}

RoboAssistant::~RoboAssistant()
{
	delete this->brain;
}
