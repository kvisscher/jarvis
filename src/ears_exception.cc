#include "ears_exception.h"

EarsException::EarsException(const std::string& message) : std::runtime_error(message)
{
}

EarsException::~EarsException() throw()
{
}
