#include "jarvis.h"

Jarvis::Jarvis(Brain* brain) : brain(brain)
{
}

Jarvis::~Jarvis()
{
	delete this->brain;
}
