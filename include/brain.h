#ifndef BRAIN_H
#define BRAIN_H

#include "ears.h"
#include "mouth.h"

class Brain : public EarsListeners
{
private:
	Ears* ears;
	Mouth* mouth;
public:
	Brain(Ears* ears, Mouth* mouth);
	virtual ~Brain();
	void notifyHeardText(const std::string& text);
};

#endif
