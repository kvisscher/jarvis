#ifndef EARS_EXCEPTION_H
#define EARS_EXCEPTION_H

#include <string>
#include <exception>

class EarsException : public std::exception
{
private:
	std::string message;
public:
	EarsException(std::string message);
};

#endif
