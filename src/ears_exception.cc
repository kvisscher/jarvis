#include "ears_exception.h"

EarsException::EarsException(std::string message) : message(message)
{
}

const char* EarsException::what() const throw()
{
	return this->message.c_str();
} 
