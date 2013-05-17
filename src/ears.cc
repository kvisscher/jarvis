#include "ears.h"

Ears::Ears()
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
