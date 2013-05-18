#ifndef EARS_LISTENER_H
#define EARS_LISTENER_H

#include <string>

class EarsListener
{
public:
	EarsListener();
	virtual ~EarsListener();
	virtual void notifyHeardText(const std::string& text) = 0;
};

#endif
