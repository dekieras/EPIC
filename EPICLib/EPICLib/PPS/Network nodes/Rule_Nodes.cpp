#include "Rule_Nodes.h"
#include "Symbol.h"
#include "Symbol_utilities.h"
#include "Variables.h"
#include "Binding_set.h"
#include "Binding_set_list.h"
#include "Combining_Nodes.h"
#include "Predicate.h"
//#include "Command_Action.h"
#include "Output_tee.h"
#include "PPS_globals.h"
#include "Assert_throw.h"

#include "Production_System.h"
#include "Command_interface.h"
#include "PPS_exception.h"

#include <string>
#include <iostream>

using std::string;
using std::ostream;	using std::cout; using std::endl; using std::cerr; 


using namespace Parsimonious_Production_System;

//std::ostream& operator<< (std::ostream& os, const Parsimonious_Production_System::Binding_set_list_t& bsl);
//ostream& operator<< (ostream& os, const Parsimonious_Production_System::Command_Action_list_t& command_list);

namespace Parsimonious_Production_System {
//using ::operator<<;	// bring in other declarations


// define static member variables

bool Rule_Node::debug_flag = false;

// create the appropriate rule node based on variables & predicates
Combining_Node * Rule_Node::create(Production_System * production_system_ptr, 
		const Symbol& rule_name, Combining_Node * source, 
		const Variable_list& var_names, const Predicate_ptr_list_t& predicates,
		const list_Symbol_list_t& add_action_list, const list_Symbol_list_t& delete_action_list,
		const list_Symbol_list_t& command_action_list,
        const list_Symbol_list_t& add_command_action_list, const list_Symbol_list_t& add_command_pattern_list)
{
//	PPS_out << "create Rule_Node using " << rule_name << ", " << var_names << " and " 
//		 << predicates.size() << " predicates, " 
//		 << add_action_list.size() << " add actions, " 
//		 << delete_action_list.size() << " delete actions, " 
//		 << command_action_list.size() << " command actions" 
//		 << endl;
	// if no variables, no predicates
	if (var_names.size() == 0)
		return new Simple_Rule_Node(production_system_ptr, 
			Combining_Node::get_next_node_number(), rule_name, source, add_action_list, delete_action_list, command_action_list,
                add_command_action_list, add_command_pattern_list);
	else if(var_names.size() > 0 && predicates.size() == 0)
		return new Bindings_Rule_Node(production_system_ptr, 
			Combining_Node::get_next_node_number(), rule_name, source, add_action_list, delete_action_list, command_action_list,
            add_command_action_list, add_command_pattern_list);
	//	(var_names.size() > 0 && predicates.size() > 0)
	return new Bindings_Predicate_Rule_Node(production_system_ptr, 
		Combining_Node::get_next_node_number(), rule_name, source, predicates, add_action_list, delete_action_list, command_action_list,
            add_command_action_list, add_command_pattern_list);
}


void Rule_Node::instantiate_pattern(const Symbol_list_t& pattern, const Binding_set& bs, Symbol_list_t& result) const
{	
	const Symbol no_value;	// contains default for no value
	
	for(Symbol_list_t::const_iterator it = pattern.begin(); it != pattern.end(); it++) {
		if (is_variable(*it)) {
			Symbol var_value(bs.get_value(*it));
			if (var_value == no_value) {
				string msg = string("In rule ") + rule_name.str() + string(": variable ") + (*it).str() + string(" has no value");
				throw PPS_exception(msg);
//				cerr << "in instantiate_pattern: variable " << *it << " has no value" << endl;
//				result.push_back(Symbol("no_valid_binding"));
				}
			else {
				result.push_back(var_value);
				}
			}
		else
			result.push_back(*it);
		}
}


// input has turned on
bool Simple_Rule_Node::positive_update(const Combining_Node * source_node)
{	
	Assert(source_node != 0);
		
	if (Rule_Node::debug_flag) {
		PPS_out << "positive update from: ";
		source_node->display_state();
		}
		 
	if (!state) {
		state = true;
		if (Rule_Node::debug_flag) {
			PPS_out << "turned on: ";
			display_state();
			}
		// put this rule on the fired list - it was not already present
		production_system_ptr->add_to_fired_rules(this);
		
		return true;
		}

	return false;
}

// input has turned off
bool Simple_Rule_Node::negative_update(const Combining_Node * source_node)
{	
	Assert(source_node != 0);
		
	if (Rule_Node::debug_flag) {
		PPS_out << "negative update from: ";
		source_node->display_state();
		}
		 
	if (state) {
		state = false;
		if (Rule_Node::debug_flag) {
			PPS_out << "turned off: ";
			display_state();
			}
		// remove this rule from the fired list if present
		production_system_ptr->remove_from_fired_rules(this);
		return true;
		}

	return false;
}

// display the static contents of the node
void Simple_Rule_Node::display_contents(const char * label, bool dynamic, int indent_level) const
{
	PPS_out << endl;
	PPS_out << rule_name << ' ';
	Combining_Node::display_contents("", dynamic, indent_level);
	if (!add_action_patterns.empty())
		PPS_out << "Add:\n" << add_action_patterns << endl;
	if (!delete_action_patterns.empty())
		PPS_out << "Delete:\n" << delete_action_patterns << endl;
	if (!command_actions.empty()) {
		PPS_out << "Actions:\n" << command_actions << endl;
//		PPS_out << "Actions:\n";
//		PPS_out << command_actions;
//		Parsimonious_Production_System::operator<< (PPS_out, command_actions);
//		::operator<< (PPS_out, command_actions);
//		operator<< (PPS_out, command_actions);
//		Parsimonious_Production_System::operator<< (cout, command_actions);
		PPS_out << endl;
		}
	PPS_out << "Matching net:" << endl;
	indent_level++;
	input->display_contents("", dynamic, indent_level);
}

void Simple_Rule_Node::execute(Command_interface& command_interface) const
{
	bool output = production_system_ptr->get_output_run_messages() && PPS_out;
	
	if(output)
		PPS_out << "*** Fire: " << rule_name << endl;

	for(list_Symbol_list_t::const_iterator it1 = add_action_patterns.begin(); it1 != add_action_patterns.end(); it1++) {
		if(output)
			PPS_out << "  Add: " << *it1 << endl;
		production_system_ptr->add_to_add_clause_list(Clause(*it1));
		}
	
	for(list_Symbol_list_t::const_iterator it2 = delete_action_patterns.begin(); it2 != delete_action_patterns.end(); it2++) {
		if(output)
			PPS_out << "  Delete: " << *it2 << endl;
		production_system_ptr->add_to_delete_clause_list(Clause(*it2));
		}
	
	for(list_Symbol_list_t::const_iterator it3 = command_actions.begin(); it3 != command_actions.end(); it3++) {
		command_interface.dispatch(*it3);
		if(output)
			PPS_out << "  Command: " << (*it3) << endl;		// Changed for temporal.  Was "  Motor Command: "
		}

    // only one at most add_command action and pattern should be present and used
    if(!add_command_actions.empty()) {
        Symbol_list_t command = add_command_actions.front();
        Symbol_list_t pattern = add_command_patterns.front();
        if(output)
            PPS_out << ' ' << command.front() << ' ' << command.back() << ' ' << pattern;
		bool to_add = command_interface.dispatch(command, pattern);
        if(to_add)
            production_system_ptr->add_to_add_clause_list(Clause(pattern));
        if(output) {
            if(to_add)
                PPS_out << ": Added" << endl;
            else
                PPS_out << ": Not added" << endl;
            }
        }
    
	/* for(list_Symbol_list_t::const_iterator it = add_command_actions.begin(); it != add_command_actions.end(); it++) {
		command_interface.dispatch(*it, add_command_patterns.front());
		if(output)
			PPS_out << "  Add_command: " << (*it) << ' ' << add_command_patterns << endl;	
        }
        */
}



// the binding set list comes from the pointed-to predecessor
bool Bindings_Rule_Node::positive_update(const Combining_Node * source_node)
{	
	Assert(source_node != 0);
		
	if (Rule_Node::debug_flag) {
		PPS_out << "positive update from: ";
		source_node->display_state();
		}
		 
	bool changed = binding_sets.add_binding_sets(source_node->get_delta_binding_sets());
	if (!changed)
		return false;
	
	// changing by adding a binding set automatically means this node is on,
	// but check for change of state
	if (!state) {
		state = true;
		if (Rule_Node::debug_flag) {
			PPS_out << "turned on: ";
			display_state();
			}
		// put this rule on the fired list - it was not already present
		production_system_ptr->add_to_fired_rules(this);
		}

	return true;
}

bool Bindings_Rule_Node::negative_update(const Combining_Node * source_node)
{	
	Assert(source_node != 0);
			
	if (Rule_Node::debug_flag) {
		PPS_out << "negative update from: ";
		source_node->display_state();
		}

	bool changed = binding_sets.remove_binding_sets(source_node->get_delta_binding_sets());
	if (!changed)
		return false;
	
	// a binding set was removed, if now empty, state becomes off, 
	// but check for change of state
	if (state && binding_sets.empty()) {
		state = false;
		if (Rule_Node::debug_flag) {
			PPS_out << "turned off: ";
			display_state();
			}
		// remove this rule from the fired list
		production_system_ptr->remove_from_fired_rules(this);
		}


	return true;
}

// display the static contents of the node
void Bindings_Rule_Node::display_contents(const char * label, bool dynamic, int indent_level) const
{
	PPS_out << endl;
	PPS_out << rule_name << ' ';
	Combining_Node::display_contents("", dynamic, indent_level);
	if (!add_action_patterns.empty())
		PPS_out << "Add:\n" << add_action_patterns << endl;
	if (!delete_action_patterns.empty())
		PPS_out << "Delete:\n" << delete_action_patterns << endl;
	if (!command_actions.empty())
		PPS_out << "Actions:\n" << command_actions << endl;
	PPS_out << "Matching net:" << endl;
	indent_level++;
	input->display_contents("", dynamic, indent_level);
}

void Bindings_Rule_Node::execute(Command_interface& command_interface) const
{
	// use this node's binding set list
	execute_with_bindings(binding_sets.get_binding_set_list(), command_interface);
}

void Bindings_Rule_Node::execute_with_bindings(const Binding_set_list_t& binding_set_list, Command_interface& command_interface) const
{
	bool output = production_system_ptr->get_output_run_messages() && PPS_out;
	if(output)
		PPS_out << "*** Fire: " << rule_name << endl;

	// for each binding set, and for each pattern in add patterns, add the add clauses, ditto with delete patterns and commands
	for(Binding_set_list_t::const_iterator bs_it = binding_set_list.begin(); bs_it != binding_set_list.end(); bs_it++) {
		if(output)
			PPS_out << "  Bindings: " << *bs_it << endl;
		for(list_Symbol_list_t::const_iterator it1 = add_action_patterns.begin(); it1 != add_action_patterns.end(); it1++) {
			Symbol_list_t result;
			instantiate_pattern(*it1, *bs_it, result);
			Clause clause(result);
			if(output)
				PPS_out << "  Add: " << clause << endl;
			production_system_ptr->add_to_add_clause_list(clause);
			}
			
		for(list_Symbol_list_t::const_iterator it2 = delete_action_patterns.begin(); it2 != delete_action_patterns.end(); it2++) {
			Symbol_list_t result;
			instantiate_pattern(*it2, *bs_it, result);
			Clause clause(result);
			if(output)
				PPS_out << "  Delete: " << clause << endl;
			production_system_ptr->add_to_delete_clause_list(clause);
			}


		for(list_Symbol_list_t::const_iterator it3 = command_actions.begin(); it3 != command_actions.end(); it3++) {
			Symbol_list_t result;
			instantiate_pattern((*it3), *bs_it, result);
			command_interface.dispatch(result);
			if(output)
				PPS_out << "  Command: " << (result) << endl;	// Changed for temporal.  Was "  Motor Command: "
			}
        
        // only one at most add_command action and pattern should be present and used
        if(!add_command_actions.empty()) {
            Symbol_list_t command = add_command_actions.front();
            Symbol_list_t pattern = add_command_patterns.front();
            Symbol_list_t result;
            instantiate_pattern(pattern, *bs_it, result);
            if(output)
                PPS_out << ' ' << command.front() << ' ' << command.back() << ' ' << result;
            bool to_add = command_interface.dispatch(command, result);
            if(to_add)
                production_system_ptr->add_to_add_clause_list(Clause(result));
            if(output) {
                if(to_add)
                    PPS_out << ": Added" << endl;
                else
                    PPS_out << ": Not added" << endl;
                }
            }


       /*
        for(list_Symbol_list_t::const_iterator it = add_command_actions.begin(); it != add_command_actions.end(); it++) {
            Symbol_list_t result;
            instantiate_pattern(add_command_patterns.front(), *bs_it, result);
            command_interface.dispatch(*it, result);
            if(output)
                PPS_out << "  Add_command: " << (*it) << ' ' << add_command_patterns << ' ' << result << endl;
            }
        */
		/*
		for(Command_Action_list_t::const_iterator it3 = command_actions.begin(); it3 != command_actions.end(); it3++) {

			Symbol_list_t result;
			if(output)
				PPS_out << "  Execute command: " << (*it3).command;
			instantiate_pattern((*it3).parameters, *bs_it, result);
			if(output)
				PPS_out << ", parameters: " << result;
			command_interface.dispatch((*it3).command, result);
				 
			for(list_Symbol_list_t::const_iterator it4 = (*it3).patterns.begin(); it4 != (*it3).patterns.end(); it4++) {
				instantiate_pattern(*it4, *bs_it, result);
				if(output)
					PPS_out << ", pattern: " << result;
				}
			if(output)
				PPS_out << endl;
			}
		*/
		}
}

// The dtor must deallocate the predicates
Bindings_Predicate_Rule_Node::~Bindings_Predicate_Rule_Node()
{
//	PPS_out << "Bindings_Predicate_Rule_Node " << get_id() << " dtor" << endl;
	for (Predicate_ptr_list_t::const_iterator it = predicate_ptrs.begin(); it != predicate_ptrs.end(); it++)
		delete (*it);
}


void Bindings_Predicate_Rule_Node::reset() 
{
	Bindings_Rule_Node::reset();
	execution_binding_sets.clear();
}

void Bindings_Predicate_Rule_Node::display_state(int indent_level) const
{
	Bindings_Rule_Node::display_state(indent_level + 2);
	indent_output(indent_level + 2);
	PPS_out << "execution bindings: " << execution_binding_sets << endl;
}


// the binding set list comes from the pointed-to predecessor
bool Bindings_Predicate_Rule_Node::positive_update(const Combining_Node * source_node)
{	
	Assert(source_node != 0);
		
	if (Rule_Node::debug_flag) {
		PPS_out << "positive update from: ";
		source_node->display_state();
		}
		 
	bool changed = binding_sets.add_binding_sets(source_node->get_delta_binding_sets());
	// if no change, nothing further needs to be done
	if (!changed)
		return false;
		
	// A predicate might result (conceivably) in turning this rule off as a result of adding a binding set.
	apply_predicates_and_check_for_changed_rule_state();
	// but the execution bindings are different, so return true - there has been a change.
	return true;
}

bool Bindings_Predicate_Rule_Node::negative_update(const Combining_Node * source_node)
{	
	Assert(source_node != 0);
			
	if (Rule_Node::debug_flag) {
		PPS_out << "negative update from: ";
		source_node->display_state();
		}

	bool changed = binding_sets.remove_binding_sets(source_node->get_delta_binding_sets());
	if (!changed)
		return false;

	// A predicate might result (conceivably) in turning this rule off as a result of adding a binding set.
	apply_predicates_and_check_for_changed_rule_state();
	// but the execution bindings are different, so return true - there has been a change.
	return true;
}

void Bindings_Predicate_Rule_Node::apply_predicates_and_check_for_changed_rule_state()
{
	// apply the predicates to see if this node has execution bindings
	// recompute starting with the current binding_sets
	execution_binding_sets = binding_sets.get_binding_set_list();
	for (Predicate_ptr_list_t::const_iterator it = predicate_ptrs.begin(); it != predicate_ptrs.end(); it++)
		execution_binding_sets = (*it)->apply(execution_binding_sets);

	// If this rule was on before, and the execution binding sets are empty, it is now off.
	if (state && execution_binding_sets.empty()) {
		state = false;
		if (Rule_Node::debug_flag) {
			PPS_out << "turned off: ";
			display_state();
			}
		// remove this rule from the fired list
		production_system_ptr->remove_from_fired_rules(this);
		}
	// If it was not on before, but there are now execution bindings, it is now on.
	else if (!state && !execution_binding_sets.empty()) {
		state = true;
		if (Rule_Node::debug_flag) {
			PPS_out << "turned on: ";
			display_state();
			}
		// put this rule on the fired list - it was not already present
		production_system_ptr->add_to_fired_rules(this);
		}
	// otherwise, the node was either already on or not, and it hasn't changed state -
}

void Bindings_Predicate_Rule_Node::execute(Command_interface& command_interface) const
{
	// use this node's execution binding set list
	execute_with_bindings(execution_binding_sets, command_interface);
}

// display the static contents of the node
void Bindings_Predicate_Rule_Node::display_contents(const char * label, bool dynamic, int indent_level) const
{
	PPS_out << endl;
	PPS_out << rule_name << ' ';
	Combining_Node::display_contents("", dynamic, indent_level);
	if (!add_action_patterns.empty())
		PPS_out << "Add:\n" << add_action_patterns << endl;
	if (!delete_action_patterns.empty())
		PPS_out << "Delete:\n" << delete_action_patterns << endl;
	if (!command_actions.empty())
		PPS_out << "Actions:\n" << command_actions << endl;
	PPS_out << "Predicates:" << endl;
	indent_level++;
	for (Predicate_ptr_list_t::const_iterator it = predicate_ptrs.begin(); it != predicate_ptrs.end(); it++)
		(*it)->display_contents(indent_level);
	PPS_out << "Matching net:" << endl;
	input->display_contents("", dynamic, indent_level);
}

} // end namespace
