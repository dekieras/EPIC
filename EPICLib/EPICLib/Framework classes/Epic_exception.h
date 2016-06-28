#ifndef EPIC_EXCEPTION_H
#define EPIC_EXCEPTION_H

#include "Exception.h"
#include <string>

class Processor;

// The base class for Epic exceptions - this has a constructor to automatically
// prepend information about the processor throwing the exception
class Epic_exception : public Exception {
public:
	Epic_exception(): Exception("Unspecified Epic_exception") {}
	Epic_exception(const std::string& msg_) : Exception(msg_) {}
	Epic_exception(Processor * proc_ptr, const std::string& msg_);
};

class Epic_internal_error : public Epic_exception {
public:
	Epic_internal_error(): Epic_exception("Unspecified Epic_internal_error") {}
	Epic_internal_error(const std::string& msg_) : Epic_exception(msg_) {}
	Epic_internal_error(Processor * proc_ptr, const std::string& msg_) :
		Epic_exception(proc_ptr, std::string("*** Epic Internal Error ") + msg_)
		{}
};


#endif
