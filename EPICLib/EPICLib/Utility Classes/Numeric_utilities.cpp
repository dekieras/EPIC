#include "Numeric_utilities.h"

#include <string>
#include <sstream>
#include <cmath>

using std::string;
using std::ostringstream;
using std::log;

// return a double rounded to the nearest integer value
double round_to_integer(double x)
{
	int i = int(x + 0.5);
	return double(i);
}

string int_to_string(int i)
{
	ostringstream ss;
	ss << i;
	return ss.str();
}

// convert hours, minutes, and seconds to milliseconds (note long integer returned)
long time_convert(long hours, long minutes, long seconds)
{
	 return (seconds*1000 + minutes*60000 + hours*3600000);
}

// convert milliseconds into hours, minutes, seconds, and milliseconds
void time_convert(long time_ms, int& hours, int& minutes, int& seconds, int& milliseconds)
{
	hours = int(time_ms / 3600000);
	time_ms = time_ms % 3600000;
	minutes = int(time_ms / 60000);
	time_ms = time_ms % 60000;
	seconds = int(time_ms / 1000);
	time_ms = time_ms % 1000;
	milliseconds = int(time_ms);
}

// convert milliseconds into hours, minutes, and double seconds
void time_convert(long time_ms, int& hours, int& minutes, double& seconds)
{
	hours = int(time_ms / 3600000);
	time_ms = time_ms % 3600000;
	minutes = int(time_ms / 60000);
	time_ms = time_ms % 60000;
	int int_seconds = int(time_ms / 1000);
	time_ms = time_ms % 1000;
	int milliseconds = int(time_ms);
	seconds = double(int_seconds) + double(milliseconds) / 1000.;
}



// compute the base 2 log of the supplied value
double logb2(double x) 
{
	return (log(x) / log(2.0));
}


double pitch_to_semitones(double pitch)
{
    return logb2(pitch) * 12.;
}

