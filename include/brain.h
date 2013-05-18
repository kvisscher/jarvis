#ifndef BRAIN_H
#define BRAIN_H

#include "ears.h"
#include "ears_listener.h"
#include "mouth.h"

class Brain : public EarsListener
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
