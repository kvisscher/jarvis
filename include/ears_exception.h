#ifndef EARS_EXCEPTION_H
#define EARS_EXCEPTION_H

#include <string>
#include <stdexcept>

class EarsException : public std::runtime_error
{
public:
	EarsException(const std::string& message);
	~EarsException() throw();
};

#endif
