#ifndef EARS_H
#define EARS_H

#include "ears_exception.h"
#include "ears_listener.h"

class Ears
{
private:
	EarsListener* listener;
protected:
	EarsListener* getListener() const;
public:
	Ears();
	virtual ~Ears();
	void setListener(EarsListener* listener);
};

#endif
