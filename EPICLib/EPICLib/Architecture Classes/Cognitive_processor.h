#ifndef COGNITIVE_PROCESSOR_H
#define COGNITIVE_PROCESSOR_H

#include "Human_subprocessor.h"
#include "Cognitive_event_types.h"
#include "Production_System.h"
#include "Command_interface.h"
#include "Parameter.h"

//#include <string>

//class Clause;

// THIS LACKS INSULATION - Production_System should be hidden behind an incomplete-declaration
// pointer!

// A cognitive processor receives inputs that are updates to its working memories.
// The update event type indicates which working memory is to be updated.
// The updates are ALWAYS in the form of (object name, property name, old value, new value).
// This means that any information about (object name, property name, old value) is to be
// discarded, and replaced by (object name, property name, new value)
// If the old value is Symbol(), nothing is discarded - assumed nothing there already.
// If the new_value is Symbol(), the new value is not stored (i.e. erase the old value).



// This class defines the common interface between a cognitive processor and the rest of the
// system.  A cognitive processor runs in steps of fixed duration, and accepts messages about
// whether operations are complete, and generate a message to advance to the next step.
// Member functions are used to control whether the advance happens or not.
class Cognitive_processor : public Human_subprocessor {
public:
	Cognitive_processor(Human_processor * human_ptr_) :
		Human_subprocessor("Cognitive", human_ptr_),
		cycle_time("Cycle_time", 50),
        probability1("Probability1", 0.5), probability2("Probability2", 0.5), probability3("Probability3", 0.5),
		probability4("Probability4", 0.5), probability5("Probability5", 0.5),
		command_interface(human_ptr_), ready_to_run(false)
		{
			add_parameter(cycle_time);
			add_parameter(probability1);
			add_parameter(probability2);
			add_parameter(probability3);
			add_parameter(probability4);
			add_parameter(probability5);
		}

	virtual bool compile(const std::string& filename);
	virtual void initialize();
		
	// Message interface to rest of the system
	virtual void accept_event(const Start_event *);
	virtual void accept_event(const Stop_event *);
	virtual void accept_event(const Cognitive_event *);
	
	virtual void handle_event(const Cognitive_Cycle_event *);
	virtual void handle_event(const Cognitive_Update_event *);
	virtual void handle_event(const Cognitive_Add_Clause_event *);
	virtual void handle_event(const Cognitive_Delete_Clause_event *);
	
	// direct inputs
	void update_object_property(const Symbol& type_name, const Symbol& object_name, const Symbol& property_name, 
		const Symbol& old_value, const Symbol& new_value);
	void add_clause(const Clause& clause);
	void delete_clause(const Clause& clause);
	
	// services to the rest of the system
	Geometry_Utilities::Point get_named_location(const Symbol& name, bool& found) const
		{return ps.get_named_location(name, found);}
	const Parameter_specification_list_t& get_parameter_specifications() const
		{return ps.get_parameter_specifications();}
    
    void display_memory_contents()
        {ps.display_memory_contents();}
		
	// functions to get/set output flags in Production System
	bool get_output_compiler_messages() const
		{return ps.get_output_compiler_messages();}
	void set_output_compiler_messages(bool flag)
		{ps.set_output_compiler_messages(flag);}

	bool get_output_compiler_details() const
		{return ps.get_output_compiler_details();}
	void set_output_compiler_details(bool flag)
		{ps.set_output_compiler_details(flag);}

	bool get_output_run_messages() const
		{return ps.get_output_run_messages();}
	void set_output_run_messages(bool flag)
		{ps.set_output_run_messages(flag);}

	bool get_output_run_details() const
		{return ps.get_output_run_details();}
	void set_output_run_details(bool flag)
		{ps.set_output_run_details(flag);}

	bool get_output_run_memory_contents() const
		{return ps.get_output_run_memory_contents();}
	void set_output_run_memory_contents(bool flag)
		{ps.set_output_run_memory_contents(flag);}
		
	long get_step_end_time() const
		{return step_end_time;}
		
	// functions to set rule-level flags in production system
	const Symbol_list_t& get_rule_names() const
		{return ps.get_rule_names();}
	bool is_rule_name_valid(const Symbol& name) const
		{return ps.is_rule_name_valid(name);}

	bool get_break_enabled() const
		{return ps.get_break_enabled();}
	void set_break_enabled(bool state)
		{ps.set_break_enabled(state);}
	void set_rule_break_state(const Symbol& name, bool state)
		{ps.set_rule_break_state(name, state);}
	bool get_rule_break_state(const Symbol& name)
		{return ps.get_rule_break_state(name);}
	const Symbol_list_t& get_break_rule_names() const
		{return ps.get_break_rule_names();}
		
	bool get_fault_enabled() const
		{return ps.get_fault_enabled();}
	void set_fault_enabled(bool state)
		{ps.set_fault_enabled(state);}
	void set_rule_fault_state(const Symbol& name, bool state)
		{ps.set_rule_fault_state(name, state);}
	bool get_rule_fault_state(const Symbol& name)
		{return ps.get_rule_fault_state(name);}
	const Symbol_list_t& get_fault_rule_names() const
		{return ps.get_fault_rule_names();}

	// no copy, move
	Cognitive_processor(const Cognitive_processor&) = delete;
	Cognitive_processor& operator= (const Cognitive_processor&) = delete;
	Cognitive_processor(Cognitive_processor&&) = delete;
	Cognitive_processor& operator= (Cognitive_processor&&) = delete;




private:
	// parameters
	Parameter cycle_time;
	Parameter probability1;
	Parameter probability2;
	Parameter probability3;
	Parameter probability4;
	Parameter probability5;
	// constants
	static const int idle_step_limit;
	
//	long cycle_number;
	long step_end_time;
	long last_step_time;
	int operations_pending;
	int idle_step_counter;	// counts steps with no action
	bool advance_next_step;

//	friend class Command_interface;
	Command_interface command_interface;
	
	bool ready_to_run;	// true if have successfully compiled a rule set
	Parsimonious_Production_System::Production_System ps;

};

#endif
