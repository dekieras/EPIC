#include "Assert_throw.h"

#include <string>
#include <sstream>

using std::string;
using std::ostringstream;

Assertion_exception::Assertion_exception (const char * condition_, const char * filename_, int lineno_)
{
	ostringstream ss;
	ss << "Assertion exception: " << condition_ << " failed in " << filename_ << " line " << lineno_;
	msg = ss.str();
}
