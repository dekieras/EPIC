#ifndef HUMAN_SUBPROCESSOR_H
#define HUMAN_SUBPROCESSOR_H

/*
This class is a subclass of Processor to be used for the components of a Human Processor. It has
facilities for maintaining lists of parameters and communications with a containing Human processor.
It intercepts no Events.
*/

#include "Human_processor.h"
#include "Output_tee.h"
#include "Processor.h"
#include "Parameter.h"

#include <list>
#include <string>


class Human_subprocessor : public Processor {
public:
	Human_subprocessor(const std::string& name_, Human_processor * human_ptr_) :
		Processor (name_), human_ptr(human_ptr_)
		{}
	virtual ~Human_subprocessor() {}
	
	virtual void initialize();
	
	virtual std::string processor_info() const;	// produce string containing output info

	// parameter services
	// these functions set all of the parameters in the processor accordingly
	virtual void set_parameter(const Parameter_specification& param_spec);
	virtual void set_distribution_type(Parameter::Distribution_type distribution_type);
	virtual void set_randomization_time(Parameter::Randomization_time randomization_time);
	virtual void randomize_value();
	virtual void describe_parameters(Output_tee& ot) const;
	// access a parameter from outside the class (e.g. for Motor_actions)
	Parameter* get_parameter_ptr(const std::string& parameter_name) const;

	// connect to another processor via this pointer
	Human_processor * get_human_ptr() const
		{return human_ptr;}
		
	// View maintenance functions
	void add_view(View_base * view_ptr);
	void remove_view(View_base * view_ptr);
	void clear_views();	// tell all of the views to clear themselves

protected:
	// services to derived classes
//	typedef std::list<View_base *> EPIC_View_list_t;
//	const EPIC_View_list_t& get_EPIC_view_list() const
//		{return views;}
		
	// call these functions to notify the views; func is a member function of View_base
	template <typename F, typename A1>
	void notify_views(F func, A1 a1) const
	{
		for(EPIC_View_list_t::const_iterator it = views.begin(); it != views.end(); it++)
			((*it)->*func)(a1);
	}

	template <typename F, typename A1, typename A2>
	void notify_views(F func, A1 a1, A2 a2) const
	{
		for(EPIC_View_list_t::const_iterator it = views.begin(); it != views.end(); it++)
			((*it)->*func)(a1, a2);
	}

	template <typename F, typename A1, typename A2, typename A3>
	void notify_views(F func, A1 a1, A2 a2, A3 a3) const
	{
		for(EPIC_View_list_t::const_iterator it = views.begin(); it != views.end(); it++)
			((*it)->*func)(a1, a2, a3);
	}

	template <typename F, typename A1, typename A2, typename A3, typename A4>
	void notify_views(F func, A1 a1, A2 a2, A3 a3, A4 a4) const
	{
		for(EPIC_View_list_t::const_iterator it = views.begin(); it != views.end(); it++)
			((*it)->*func)(a1, a2, a3, a4);
	}

	template <typename F, typename A1, typename A2, typename A3, typename A4, typename A5>
	void notify_views(F func, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const
	{
		for(EPIC_View_list_t::const_iterator it = views.begin(); it != views.end(); it++)
			((*it)->*func)(a1, a2, a3, a4, a5);
	}

	template <typename F, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
	void notify_views(F func, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) const
	{
		for(EPIC_View_list_t::const_iterator it = views.begin(); it != views.end(); it++)
			((*it)->*func)(a1, a2, a3, a4, a5, a6);
	}

	template <typename F, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
	void notify_views(F func, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) const
	{
		for(EPIC_View_list_t::const_iterator it = views.begin(); it != views.end(); it++)
			((*it)->*func)(a1, a2, a3, a4, a5, a6, a7);
	}

	void broadcast_to_recorders(const Event *
		 event_ptr)
		{human_ptr->broadcast_to_recorders(event_ptr);}

	void add_parameter(Parameter& parameter)
		{parameter_ptrs[parameter.get_name()] = &parameter;}
	void remove_parameter(Parameter& parameter)
		{parameter_ptrs.erase(parameter.get_name());}

private:
	// connnections
	Human_processor * human_ptr;

	// parameters
	typedef std::map<std::string, Parameter *> Parameter_ptrs_t;
	Parameter_ptrs_t parameter_ptrs;

	typedef std::list<View_base *> EPIC_View_list_t;		
	EPIC_View_list_t views;

	// no default copy, assignment
	Human_subprocessor(const Human_subprocessor&);
	Human_subprocessor& operator= (const Human_subprocessor&);
};

#endif
