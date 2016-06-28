#include "Processor.h"
#include "Coordinator.h"

#include <typeinfo>
#include <string>
#include <sstream>

using std::string;
using std::ostringstream;

Processor::Processor(const std::string& processor_name_, bool add_processor) : 
	processor_name(processor_name_), trace(false), added(add_processor)
{	
	// tell the coordinator about this processor
	if(added)
		Coordinator::get_instance().add_processor(this);
}

Processor::~Processor()
{
	// remove the processor if it was auto-added
	if(added)
		Coordinator::get_instance().remove_processor(this);
}

// services for external and derived classes
string Processor::processor_info() const
{
	ostringstream oss;
	oss << get_time() << ':' << processor_name << ':';
	return oss.str();
}

void Processor::stop_simulation()
{
	Coordinator::get_instance().stop();
}

long Processor::get_time() 
{
	return Coordinator::get_time();
}

void Processor::schedule_event(const Event * p) 
{
	Coordinator::get_instance().schedule_event(p);
}
