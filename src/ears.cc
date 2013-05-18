#include "ears.h"

Ears::Ears() throw(EarsException)
{
}

Ears::~Ears()
{
	delete this->listener;
}

EarsListener* Ears::getListener() const
{
	return this->listener;
}

void Ears::setListener(EarsListener* listener)
{
	this->listener = listener;
}
