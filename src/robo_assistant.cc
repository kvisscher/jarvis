#include "robo_assistant.h"

RoboAssistant::RoboAssistant(Brain* brain) : brain(brain)
{
	brain->getMouth()->speak("Hello world");
}

RoboAssistant::~RoboAssistant()
{
	delete this->brain;
}
