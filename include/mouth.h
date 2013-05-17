#ifndef MOUTH_H
#define MOUTH_H

#include <string>

class Mouth
{
public:
	Mouth();
	virtual ~Mouth();
	virtual void speak(const std::string& text) = 0;
};

#endif
