#ifndef NUMERIC_UTILITIES_H
#define NUMERIC_UTILITIES_H

#include <string>

// rounds x to an integer value returned in a double
double round_to_integer(double x);

// writes an integer value to a string
std::string int_to_string(int i);

// convert hours, minutes, and seconds to milliseconds (note long integer returned)
long time_convert(long hours, long minutes, long seconds);

// convert milliseconds into hours, minutes, seconds, and milliseconds
void time_convert(long time_ms, int& hours, int& minutes, int& seconds, int& milliseconds);

// convert milliseconds into hours, minutes, and double seconds
void time_convert(long time_ms, int& hours, int& minutes, double& seconds);

// compute the base 2 log of the supplied value
double logb2(double x);

double pitch_to_semitones(double pitch);



#endif
