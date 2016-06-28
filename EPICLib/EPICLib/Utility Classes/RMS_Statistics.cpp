#include "RMS_Statistics.h"

#include <cmath>

using namespace std;


double Accumulate_rms_error::update(GU::Point p1, GU::Point p2)
{
	double error = GU::cartesian_distance(p1, p2);
	total += (error * error);
	n++;
	rms = sqrt(total / n);
	return rms;
}
		
