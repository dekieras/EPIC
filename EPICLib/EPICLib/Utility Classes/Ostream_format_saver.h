#ifndef OSTREAM_FORMAT_SAVER_H
#define OSTREAM_FORMAT_SAVER_H
#include <iostream>

// An RAII-concept object for saving ostream formatting states

class Ostream_format_saver {
public:
Ostream_format_saver(std::ostream& os_) :
	os(os_), old_flags(os.flags()), old_precision(os.precision()) {}
~Ostream_format_saver() 
	{
		os.flags(old_flags);
		os.precision(old_precision);
	}
private:
	std::ostream& os;
	std::ios::fmtflags old_flags;
	int old_precision;
};
#endif
