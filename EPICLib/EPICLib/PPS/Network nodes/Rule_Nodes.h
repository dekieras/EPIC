#ifndef RULE_NODES_H
#define RULE_NODES_H

#include <iostream>
#include <string>
#include <list>

#include "Assert_throw.h"
#include "Output_tee.h"
#include "PPS_globals.h"
#include "Symbol.h"
#include "Symbol_utilities.h"
#include "Variables.h"
#include "Binding_set.h"
#include "Binding_set_list.h"
#include "Combining_Nodes.h"
#include "Predicate.h"
//#include "Command_Action.h"
#include "Motor_action.h"

namespace Parsimonious_Production_System {
using ::operator<<;	// bring global declarations into scope
using std::operator<<;	// bring global declarations into scope

//forward declaration
class Production_System;


// A mixin class for shared functions and variables
class Rule_Node {
public:
	
	Rule_Node(Production_System * in_production_system_ptr, const Symbol& in_rule_name, Combining_Node * in_input,
			const list_Symbol_list_t& add_action_list, const list_Symbol_list_t& delete_action_list, const list_Symbol_list_t& command_action_list,
            const list_Symbol_list_t& add_command_action_list, const list_Symbol_list_t& add_command_pattern_list) :
		production_system_ptr(in_production_system_ptr), input(in_input), rule_name(in_rule_name),
		faulted_flag(false), break_flag(false), add_action_patterns(add_action_list), delete_action_patterns(delete_action_list),
		command_actions(command_action_list), add_command_actions(add_command_action_list), add_command_patterns(add_command_pattern_list)
		{}
	
	virtual ~Rule_Node()
		{}
	static bool debug_flag;

	static Combining_Node * create(Production_System * production_system_ptr, 
		const Symbol& rule_name, Combining_Node * source, 
		const Variable_list& var_names, const Predicate_ptr_list_t& predicates,
		const list_Symbol_list_t& add_action_list, const list_Symbol_list_t& delete_action_list,
		const list_Symbol_list_t& command_action_list,
        const list_Symbol_list_t& add_command_action_list, const list_Symbol_list_t& add_command_pattern_list);
		
	const Symbol& get_name() const
		{return rule_name;}
	// accessors for faulted and break flags
	bool get_fault_state() const {return faulted_flag;}
	void set_fault_state(bool faulted_flag_) {faulted_flag = faulted_flag_;}
	bool get_break_state() const {return break_flag;}
	void set_break_state(bool break_flag_) {break_flag = break_flag_;}
	
	// execute the rule actions
	virtual void execute(Command_interface&) const = 0;	// pure virtuals


protected:
	Production_System * production_system_ptr;
	Combining_Node * input;
	const Symbol rule_name;
	bool faulted_flag;	// flag that this rule has been faulted
	bool break_flag;		// flag to break when this rule is executed
	const list_Symbol_list_t add_action_patterns;
	const list_Symbol_list_t delete_action_patterns;
	const list_Symbol_list_t command_actions;
	const list_Symbol_list_t add_command_actions;
	const list_Symbol_list_t add_command_patterns;

//	static void instantiate_pattern(const Symbol_list_t& pattern, const Binding_set& bs, Symbol_list_t& result);
	void instantiate_pattern(const Symbol_list_t& pattern, const Binding_set& bs, Symbol_list_t& result) const;
	
private:
	Rule_Node();
};

typedef std::list<Rule_Node *> Rule_Node_ptr_list_t;

// A Simple_Rule_Node has no bindings
class Simple_Rule_Node : public Combining_Node, public Rule_Node {
public:
	Simple_Rule_Node(Production_System * in_production_system_ptr, int in_id, const Symbol& in_rule_name, Combining_Node * in_input,
			const list_Symbol_list_t& add_action_list, const list_Symbol_list_t& delete_action_list, const list_Symbol_list_t& command_action_list,
            const list_Symbol_list_t& add_command_action_list, const list_Symbol_list_t& add_command_pattern_list) :
		Combining_Node(in_id), Rule_Node (in_production_system_ptr, in_rule_name, in_input, add_action_list, delete_action_list, command_action_list, add_command_action_list, add_command_pattern_list)
		{}

//	~Simple_Rule_Node()
//		{cout << "Simple_Rule_Node " << get_id() << " dtor" << endl;}
	// source has turned on
	virtual bool positive_update(const Combining_Node * source_node);
	// source has turned off
	virtual bool negative_update(const Combining_Node * source_node);

	// display the contents of the node
	virtual void display_contents(const char * label = "", bool dynamic = false, int indent_level = 0) const;

	// execute the rule actions
	virtual void execute(Command_interface&) const;

private:
	Simple_Rule_Node();
};

// A Bindings_Rule_Node has bindings and no predicates
class Bindings_Rule_Node : public Bindings_Node, public Rule_Node {
public:
	Bindings_Rule_Node(Production_System * in_production_system_ptr, int in_id, const Symbol& in_rule_name, Combining_Node * in_input,
			const list_Symbol_list_t& add_action_list, const list_Symbol_list_t& delete_action_list, const list_Symbol_list_t& command_action_list,
            const list_Symbol_list_t& add_command_action_list, const list_Symbol_list_t& add_command_pattern_list) :
		Bindings_Node(in_id), Rule_Node (in_production_system_ptr, in_rule_name, in_input, add_action_list, delete_action_list, command_action_list, add_command_action_list, add_command_pattern_list)
		{}

//	~Bindings_Rule_Node()
//		{cout << "Bindings_Rule_Node " << get_id() << " dtor" << endl;}
	// source has turned on
	virtual bool positive_update(const Combining_Node * source_node);
	// source has turned off
	virtual bool negative_update(const Combining_Node * source_node);

	// display the contents of the node
	virtual void display_contents(const char * label = "", bool dynamic = false, int indent_level = 0) const;

	// execute the rule actions
	virtual void execute(Command_interface&) const;
	void execute_with_bindings(const Binding_set_list_t& binding_set_list, Command_interface& command_interface) const;

private:

	Bindings_Rule_Node();
};

// A Bindings_Predicate_Rule_Node has bindings and predicates
class Bindings_Predicate_Rule_Node : public Bindings_Rule_Node {
public:
	Bindings_Predicate_Rule_Node(Production_System * in_production_system_ptr, int in_id, const Symbol& in_rule_name, 
		Combining_Node * in_input, const Predicate_ptr_list_t& in_predicate_ptrs,
		const list_Symbol_list_t& add_action_list, const list_Symbol_list_t& delete_action_list, const list_Symbol_list_t& command_action_list,
        const list_Symbol_list_t& add_command_action_list, const list_Symbol_list_t& add_command_pattern_list) :
		Bindings_Rule_Node (in_production_system_ptr, in_id, in_rule_name, in_input, add_action_list, delete_action_list, 
		command_action_list, add_command_action_list, add_command_pattern_list), predicate_ptrs(in_predicate_ptrs)
		{}

	// The dtor must deallocate the predicates
	~Bindings_Predicate_Rule_Node();
	virtual void reset(); 
	virtual void display_state(int indent_level = 0) const;

	// source has turned on
	virtual bool positive_update(const Combining_Node * source_node);
	// source has turned off
	virtual bool negative_update(const Combining_Node * source_node);

	// display the contents of the node
	virtual void display_contents(const char * label = "", bool dynamic = false, int indent_level = 0) const;

	// execute the rule actions
	virtual void execute(Command_interface&) const;

private:
	const Predicate_ptr_list_t predicate_ptrs;
	
	// the binding_sets from Bindings_Nodes reflect the input state;
	// these binding_sets represents the binding sets after predicates are applied
	Binding_set_list_t execution_binding_sets;

	void apply_predicates_and_check_for_changed_rule_state();

	Bindings_Predicate_Rule_Node();
};

} // end namespace

#endif
