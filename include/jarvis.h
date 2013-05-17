#ifndef JARVIS_H
#define JARVIS_H

#include "brain.h"

class Jarvis
{
private:
	Brain* brain;
public:
	Jarvis(Brain* brain);
	~Jarvis();
};

#endif
