#ifndef ASSERT_THROW_H
#define ASSERT_THROW_H

#include "Exception.h"
#include <string>

// this class always declared
class Assertion_exception : public Exception {
public:
	Assertion_exception (const char * condition_, const char * filename_, int lineno_);
	~Assertion_exception() throw()
		{}
	const char * what() const throw() override
		{return msg.c_str();}
private:
	std::string msg;
};

// turn macro on or off depending on NDEBUG

#ifdef NDEBUG
#define Assert(condition) ((void) 0)
#else
#define Assert(condition) ((condition) ? ((void) 0) : throw Assertion_exception(#condition,__FILE__, __LINE__))
#endif

#endif
