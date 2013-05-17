#ifndef DEFAULT_MOUTH_H
#define DEFAULT_MOUTH_H

#include "mouth.h"

class DefaultMouth : public Mouth
{
public:
	DefaultMouth();
	virtual ~DefaultMouth();
	virtual void speak(const std::string& text);
};

#endif
