#include "Parameter.h"
#include "Random_utilities.h"
#include "Epic_exception.h"

#include <string>
#include <sstream>

using std::string;
using std::istringstream;
using std::ostringstream;

// to set an initially fixed parameter value
Parameter::Parameter(const std::string& name_, double constant_value) :
	name(name_)
{
	set_distribution_type(Fixed);
	set_mean_value(constant_value);
	set_deviation(0.);
	set_randomization_time(Never);
}

// to set a uniformly distributed parameter with conventional deviation = mean/3

Parameter::Parameter(const std::string& name_, double mean_value_, Randomization_time randomization_time_) :
	name(name_)
{
	set_distribution_type(Uniform);
	set_mean_value(mean_value_);
	set_deviation(0.);
	set_randomization_time(randomization_time_);
}


// to set all properties - note no default values, parameters in same order as specifications
Parameter::Parameter(const std::string& name_, Distribution_type distribution_type_, 
	Randomization_time randomization_time_, double mean_value_, double deviation_) :
	name(name_)
{
	set_distribution_type(distribution_type_);
	set_mean_value(mean_value_);
	set_deviation(deviation_);
	set_randomization_time(randomization_time_);
}


void Parameter::set_mean_value(double mean_value_)
{
	mean_value = mean_value_;
	parameter_value = mean_value;
}

void Parameter::set_deviation(double deviation_)
{
	deviation = deviation_;
	if (deviation == 0.0 && distribution_type == Uniform)
		deviation = mean_value/ 3.;
}

void Parameter::set_distribution_type(Distribution_type distribution_type_)
{
	distribution_type = distribution_type_;
	setup_distribution_function();
}

void Parameter::set_randomization_time(Randomization_time randomization_time_)
{
	randomization_time = randomization_time_;
}

void Parameter::setup_distribution_function()
{	
	// set distribution function pointer to appropriate value
	if(distribution_type == Normal)
		fp = normal_fluctuation;
	else if(distribution_type == Uniform)
		fp = uniform_fluctuation;
	else if(distribution_type == Log_normal)
		fp = log_normal_fluctuation;
	else
		fp = no_fluctuation;
}

// negative values are not allowed - resample until result is >= 0
double Parameter::uniform_fluctuation(double mean, double deviation)
{
	double result;
	do {
		result = uniform_random_variable(mean, deviation);
		} while (result < 0);
	return result;
}

double Parameter::normal_fluctuation(double mean, double deviation)
{
	double result;
	do {
		result = normal_random_variable(mean, deviation);
		} while (result < 0);
	return result;
}

double Parameter::log_normal_fluctuation(double median, double spread)
{
	double result;
	do {
		result = log_normal_random_variable(median, spread);
		} while (result < 0);
	return result;
}

double Parameter::no_fluctuation(double mean, double)
{
	return mean;
}

// this function describes each parameter in its current state
string Parameter::get_description() const
{
	ostringstream oss;
	oss << name;
	switch(distribution_type) {
		case Fixed:
			oss << ": fixed at " << mean_value;
			return oss.str();
		case Normal:
			oss << ": N(" << mean_value << ", " << deviation << ")";
			break;
		case Log_normal:
			oss << ": logN(" << mean_value << ", " << deviation << ")";
			break;
		case Uniform:
			oss << ": U(" << mean_value << ", " << deviation << ")";
			break;
		}
	switch(randomization_time) {
		case When_used:
			oss << "; sampled every use.";
			break;
		case On_command:
			oss << "; sampled on command.";
			break;
		case Never:
			oss << "; never sampled.";
			break;
		}
					
	return oss.str();
}


/*
Fixed value
Uniform (When_used | On_command) mean_value {deviation} // deviation not specified, set to mean/3
Normal (When_used | On_command) mean_value deviation
Log_normal (When_used | On_command) median spread
*/

// this defines the default for setting parameter values from an input source
void Parameter::set_parameter_values(const Parameter_specification& input_spec)
{
	istringstream iss(input_spec.specification);
	// first item is distribution type
	string dist_type;
	iss >> dist_type;
	if(!iss)
		throw_parameter_error("Cannot read distribution type from parameter string", input_spec);
	if(dist_type == "Fixed") {
		set_distribution_type(Fixed);
		set_randomization_time(Never);
		// in this case, second item is the mean (fixed) value
		double mean_value_in;
		iss >> mean_value_in;
		if(!iss)
			throw_parameter_error("Cannot read parameter value from parameter string", input_spec);
		set_mean_value(mean_value_in);
		return;
		}
	else if(dist_type == "Uniform")
		set_distribution_type(Uniform);
	else if(dist_type == "Normal")
		set_distribution_type(Normal);
	else if(dist_type == "Log_normal")
		set_distribution_type(Log_normal);
	else 
		throw_parameter_error("Unrecognized distribution type in parameter string", input_spec);

	// second item is randomization time
	string rand_time;
	iss >> rand_time;
	if(!iss)
		throw_parameter_error("Cannot read randomization time from parameter string", input_spec);
	if(rand_time == "When_used")
		set_randomization_time(When_used);
	else if(rand_time == "On_command")
		set_randomization_time(On_command);
	else 
		throw_parameter_error("Unrecognized randomization time in parameter string", input_spec);

	// third item is mean value
	double mean_value_in;
	iss >> mean_value_in;
	if(!iss)
		throw_parameter_error("Cannot read mean_value from parameter string", input_spec);
	set_mean_value(mean_value_in);
	// fourth item is deviation, but it is optional if Uniform is specified
	iss >> deviation;
	if(!iss && distribution_type == Uniform) {
		deviation = 0;
		}
	else if(!iss)
		throw_parameter_error("Cannot read deviation from parameter string", input_spec);

	set_deviation(deviation);
}

void Parameter::throw_parameter_error(const char * msg, const Parameter_specification& input_spec)
{
	throw Epic_exception(
			string(msg) + " \"" + 
			input_spec.processor_name + " " + input_spec.parameter_name + " " + 
			input_spec.specification + "\"");
}

