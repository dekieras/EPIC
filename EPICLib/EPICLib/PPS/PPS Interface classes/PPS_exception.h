#ifndef PPS_EXCEPTION_H
#define PPS_EXCEPTION_H

#include <string>
#include "Exception.h"

// The base class for PPS exceptions
class PPS_exception : public Exception {
public:
	PPS_exception(): Exception("Unspecified PPS exception") {}
	PPS_exception(const std::string& msg_) : Exception(msg_) {}
};

#endif
