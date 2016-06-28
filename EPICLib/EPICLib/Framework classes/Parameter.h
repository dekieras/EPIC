#ifndef PARAMETER_H
#define PARAMETER_H

#include <string>
#include <list>

/* Parameter class used to specify possibly fluctuation processor parameter values.
Three orthogonal properties of parameters, which can be changed during run-time:
	Whether randomized
		Either not at all, or as otherwise specified
	When randomized
		Either when specifically commanded with randomize(), or 
		Every time the value is accessed with get_value() or get_double_value()
	How randomized - Fluctuation type
		fixed (never fluctuates)
		uniform - uniform about mean value +/- the deviation
		normal - normally distributed about N(mean, deviation).
Inheritance is not used for the fluctuation type because it might be changed at run time
rather than being a structural feature. Function pointer is set instead.
*/

struct Parameter_specification {
	std::string processor_name;
	std::string parameter_name;
	std::string specification;
};


/* specification string is in the form:
Fixed value
Uniform (When_used | On_command) mean_value {deviation} // if deviation not specified, set to mean/3
Normal (When_used | On_command) mean_value deviation
Log_normal (When_used | On_command) median spread
*/

typedef std::list<Parameter_specification> Parameter_specification_list_t;

class Parameter {
public:
	typedef enum fluctuation_e {Fixed, Uniform, Normal, Log_normal} Distribution_type;
	typedef enum when_randomized_e {When_used, On_command, Never} Randomization_time;

	// to set an initially fixed parameter value
	Parameter(const std::string& name_, double constant_value);

	// to set a uniformly distributed parameter with conventional deviation = mean/3
	Parameter(const std::string& name_, double mean_value_, Randomization_time randomization_time_);
	
	// to set all properties - note no default values, parameters in same order as specifications
	Parameter(const std::string& name_, Distribution_type distribution_type_, Randomization_time randomization_time_, double mean_value_, double deviation_);


	// set to change the values during execution
	void set_mean_value(double mean_value_);
	void set_deviation(double deviation_);
	void set_distribution_type(Distribution_type distribution_type_);
	void set_randomization_time(Randomization_time randomization_time_);

	virtual void set_parameter_values(const Parameter_specification& input_spec);
			
	// accessors for values, randomization control
	const std::string& get_name() const
		{return name;}
	double get_constant_value() const
		{return mean_value;}
	double get_deviation() const
		{return deviation;}
	std::string get_description() const;
	
	void randomize_value()
		{if(randomization_time == On_command) parameter_value = fp(mean_value, deviation);}

	double get_double_value() const
		{
		if(randomization_time == When_used)
			parameter_value = fp(mean_value, deviation);
		return parameter_value;
		}
    // return the parameter value converted to a long
	long get_long_value() const
		{return long(get_double_value());}
		
	static void	throw_parameter_error(const char * msg, const Parameter_specification& input_spec);

private:
	typedef double (*Parameter_distribution_fp_t)(double, double);
	const std::string name;
	double mean_value;
	double deviation;
	Distribution_type distribution_type;
	Randomization_time randomization_time;
	Parameter_distribution_fp_t fp;
	mutable double parameter_value;

	void setup_distribution_function();
	static double uniform_fluctuation(double mean, double deviation);
	static double normal_fluctuation(double mean, double deviation);
	static double log_normal_fluctuation(double median, double spread);
	static double no_fluctuation(double mean, double deviation);
};


#endif
