#include "brain.h"

Brain::Brain(Ears* ears, Mouth* mouth) : ears(ears), mouth(mouth)
{
	this->ears->setListener(dynamic_cast<EarsListener*>(this));
}

Brain::~Brain()
{
	
}

void Brain::notifyHeardText(const std::string& message)
{
}

Ears* Brain::getEars() const
{
	return this->ears;
}

Mouth* Brain::getMouth() const
{
	return this->mouth;
}
