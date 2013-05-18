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
	virtual ~EarsException() throw();
	virtual const char* what() const throw();
};

#endif
