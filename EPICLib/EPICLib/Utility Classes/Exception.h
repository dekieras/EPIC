/* 
Exception is a base class for exceptions that contains a std::string
with the std::exception interface for accessing its contents. Note that 
the value returned by what() is valid only as long as the exception object 
exists.  The std::string member is protected to allow subclasses to access
it directly for more flexibility in initialization.
*/

#ifndef MY_EXCEPTION_H
#define MY_EXCEPTION_H

#include <exception>
#include <string>

class Exception : public std::exception {
public:
	Exception() : msg("Unspecified Exception")
		{}
	Exception(const std::string& msg_) :
		msg(msg_)
		{}
	~Exception() throw()
		{}
	const char * what() const noexcept override
		{return msg.c_str();}
protected:
	std::string msg;
};

#endif
