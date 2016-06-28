#ifndef RMS_STATISTICS_H
#define RMS_STATISTICS_H

#include "Geometry.h"
namespace GU = Geometry_Utilities;


/*
Use these classes to accumulate running statistics values easily.
reset() - zero the internal variables
update() - add a new data value, updating current average
get_n, get_rms/mean - return the current values
*/

class Accumulate_rms_error {
public:
	Accumulate_rms_error()
		{
			reset();
		}
	void reset()
		{
			n = 0;
			total = 0.;
			rms = 0.;
		}
	
	double update(GU::Point p1, GU::Point p2);
			
	long get_n() const
		{return n;}
	double get_rms() const
		{return rms;}
			
private:
	long n;
	double total;
	double rms;
};


#endif
