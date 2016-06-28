//#define NDEBUG	// turn off asserts

#include "Cognitive_processor.h"
#include "Cognitive_event_types.h"
#include "Coordinator.h"
#include "Production_System.h"
#include "Symbol.h"
#include "Output_tee.h"
#include "Output_tee_globals.h"
#include "Epic_standard_symbols.h"
#include "Numeric_utilities.h"
#include "PPS_exception.h"
#include "Epic_exception.h"
#include "PPS_globals.h"
#include "Assert_throw.h"


#include <string>
#include <ctime>
#include <iostream>
//#include <cstdlib>
//#include <typeinfo>

//using Parsimonious_Production_System::Clause;
using Parsimonious_Production_System::Production_System;

//using namespace std;
using std::string;
using std::clock_t; using std::clock;
using std::endl;	using std::cerr;

const int Cognitive_processor::idle_step_limit = 1000;

bool Cognitive_processor::compile(const string& filename)
{
	try {
//		ps.clear();	// destroy any existing network
		ready_to_run = ps.compile_file(filename);
		ps.display_static_contents();
		}
	catch (...) {
		ready_to_run = false;
		throw;
		}
	return ready_to_run;		
}

void Cognitive_processor::initialize()
{
	if(!ready_to_run)
		throw Epic_internal_error(this, "Attempt to initialize when production rules not compiled.");
	Human_subprocessor::initialize();
//	cycle_number = 0;
	idle_step_counter = 0;
	advance_next_step = true;	

	// this empties all the data
	ps.reset();

	// this prepares it to run
	ps.initialize();
		
}


// Message interface to rest of the system

void Cognitive_processor::accept_event(const Start_event *)
{
	step_end_time = get_time() + cycle_time.get_long_value();
	schedule_event(new Cognitive_Cycle_event(step_end_time, this)); 	
}

void Cognitive_processor::accept_event(const Stop_event *)
{
//	psychological_space.display_contents(Normal_out);
}

void Cognitive_processor::accept_event(const Cognitive_event * event_ptr)
{
//	if(get_trace() && Trace_out)
//		Trace_out << processor_info() << typeid(*event_ptr).name() << " received" << endl;
	broadcast_to_recorders(event_ptr);
	// tell the event to handle itself with itself!
	event_ptr->handle_self(this);
}


void Cognitive_processor::handle_event(const Cognitive_Cycle_event *)
{
	// the step_end_time is one less than the start time of the next cycle
	// to allow motor commands to start execution before the beginning of the next cycle
	step_end_time = get_time() + cycle_time.get_long_value() - 1;
	// if we are about to halt, turn on the output
	if(idle_step_counter > idle_step_limit) {
		Normal_out << processor_info() << " Halting because " << idle_step_limit 
					<< " idle steps have elapsed; memory contents follow:" << endl;
		ps.display_memory_contents();
		throw Epic_exception(this, int_to_string(idle_step_limit) + " idle steps have elapsed");
		}
// here execute a step; advance idle_step_counter if nothing happens
// set advance_next_step to false if it is not time to advance to another cycle.	
	
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << " Cycle " << ps.get_cycle_counter() << endl;
	// ps.display_memory_contents();
	bool rule_fired = ps.run_cycle(get_time(), command_interface);
	if(rule_fired)
		idle_step_counter = 0;
	else
		idle_step_counter++;


	if(advance_next_step) {
		schedule_event(new Cognitive_Cycle_event(step_end_time + 1, this));
		}
	
	// decide that we have to pause the simulation here ...
	if(ps.get_break_on_this_cycle())
		Coordinator::get_instance().pause();
}

void Cognitive_processor::handle_event(const Cognitive_Update_event * event_ptr)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << event_ptr->tag_name << " update: " << event_ptr->object_name 
			<< " " << event_ptr->property_name << " from " << event_ptr->old_value
			<< " to " << event_ptr->new_value << endl;

	
	if(event_ptr->old_value != Nil_c)
		ps.update_delete_clause(Clause(event_ptr->tag_name, event_ptr->object_name, event_ptr->property_name, event_ptr->old_value));
	if(event_ptr->new_value != Nil_c)
		ps.update_add_clause(Clause(event_ptr->tag_name, event_ptr->object_name, event_ptr->property_name, event_ptr->new_value));
	
}
			
void Cognitive_processor::handle_event(const Cognitive_Add_Clause_event * event_ptr)
{
	Clause clause(event_ptr->clause);
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "add clause: " << clause << endl; 

	ps.update_add_clause(clause);
}

void Cognitive_processor::handle_event(const Cognitive_Delete_Clause_event * event_ptr)
{
	Clause clause(event_ptr->clause);
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "delete clause: " << clause << endl; 

	ps.update_delete_clause(clause);
}

// direct call update inputs
void Cognitive_processor::update_object_property(const Symbol& type_name, const Symbol& object_name, const Symbol& property_name, const Symbol& old_value, const Symbol& new_value)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << type_name << " update: " << object_name 
			<< " " << property_name << " from " << old_value
			<< " to " << new_value << endl;

	if(old_value != Nil_c)
		ps.update_delete_clause(Clause(type_name, object_name, property_name, old_value));
	if(new_value != Nil_c)
		ps.update_add_clause(Clause(type_name, object_name, property_name, new_value));
}

void Cognitive_processor::add_clause(const Clause& clause)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "add clause: " << clause << endl; 

	ps.update_add_clause(clause);
}

void Cognitive_processor::delete_clause(const Clause& clause)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "delete clause: " << clause << endl; 

	ps.update_delete_clause(clause);
}

