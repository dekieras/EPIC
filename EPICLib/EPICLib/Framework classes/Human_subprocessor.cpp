#include "Human_subprocessor.h"
#include "Assert_throw.h"
#include "Output_tee.h"
#include "Output_tee_globals.h"
#include "Processor.h"
#include "Parameter.h"

#include <iostream>
#include <string>
#include <list>
#include <algorithm>
#include <functional>

using std::endl;
using std::list;
using std::string;
using std::for_each;
using std::mem_fun; using std::bind2nd;
//#include <typeinfo>



void Human_subprocessor::initialize()
{
	Assert(human_ptr);
	clear_views();
//	describe_parameters(Normal_out);
}

/* View maintenance and notification functions */


void Human_subprocessor::add_view(View_base * view_ptr)
{
	views.push_back(view_ptr);
}
	
void Human_subprocessor::remove_view(View_base * view_ptr)
{
	views.remove(view_ptr);
}

void Human_subprocessor::clear_views()
{
	// tell all the associated views to clear themselves
	for(EPIC_View_list_t::const_iterator it = views.begin(); it != views.end(); it++)
		(*it)->clear();
}

string Human_subprocessor::processor_info() const
{
	string result = human_ptr->processor_info();
	result += get_name();
	result += ':';
	return result;
}



// parameter services


void Human_subprocessor::set_parameter(const Parameter_specification& param_spec)
{
	Parameter_ptrs_t::const_iterator param_ptr_it = parameter_ptrs.find(param_spec.parameter_name);
	if(param_ptr_it == parameter_ptrs.end())
		Parameter::throw_parameter_error("Unrecognized parameter name in parameter specification", param_spec);
	param_ptr_it->second->set_parameter_values(param_spec);
}

Parameter* Human_subprocessor::get_parameter_ptr(const std::string& parameter_name) const
{
	Parameter_ptrs_t::const_iterator param_ptr_it = parameter_ptrs.find(parameter_name);
	Assert(param_ptr_it != parameter_ptrs.end());	// programming error if this is not true
	return(param_ptr_it->second);
}


/*
void Human_subprocessor::set_randomize(bool randomize)
{
	for_each(parameter_ptrs.begin(), parameter_ptrs.end(), 
		bind2nd(mem_fun(&Parameter::set_randomize), randomize));
}
*/

void Human_subprocessor::set_distribution_type(Parameter::Distribution_type distribution_type)
{
	for(Parameter_ptrs_t::iterator it = parameter_ptrs.begin(); it != parameter_ptrs.end(); ++it)
		it->second->set_distribution_type(distribution_type);
}

void Human_subprocessor::set_randomization_time(Parameter::Randomization_time randomization_time)
{
	for(Parameter_ptrs_t::iterator it = parameter_ptrs.begin(); it != parameter_ptrs.end(); ++it)
		it->second->set_randomization_time(randomization_time);
}

void Human_subprocessor::randomize_value()
{
	for(Parameter_ptrs_t::iterator it = parameter_ptrs.begin(); it != parameter_ptrs.end(); ++it)
		it->second->randomize_value();
}

void Human_subprocessor::describe_parameters(Output_tee& ot) const
{
	if(parameter_ptrs.empty()) {
		ot << '\n' << get_name() << " Parameters: None " << endl;
		return;
		}
	ot << '\n' << get_name() << " Parameters:" << endl;
	for(Parameter_ptrs_t::const_iterator it = parameter_ptrs.begin(); it != parameter_ptrs.end(); ++it)
		ot << it->second->get_description() << endl;
}


