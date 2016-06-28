#include "Discrimination_Nodes.h"
#include "PPS_globals.h"
#include "Assert_throw.h"

#include <iostream>
using std::cout; using std::endl;

// 2/04/04 corrections to start_add_clause and start_remove_clause - DK

namespace Parsimonious_Production_System {

// add this node and its descendants to the set of nodes
void Discrimination_Node::accumulate_nodes(Discrimination_Node_ptr_set_t& nodes)
{
	nodes.insert(this);
	for (Discrimination_Node_ptr_list_t::const_iterator it = successors.begin(); it != successors.end(); it++)
		(*it)->accumulate_nodes(nodes);
}

void Root_Node::display_contents() const
{
	PPS_out << "Root: " << net_name << endl;
	for (Discrimination_Node_ptr_list_t::const_iterator it = successors.begin(); it != successors.end(); it++)
		(*it)->display_contents(1);	
}


Combining_Node * Root_Node::start_build_pattern(const Clause& pattern)
{
	Variable_list var_names;
	bool wildcard_present = false;
	int wildcard_number = 0;
	Combining_Node * result = 0;
	
	// start building the pattern from this node
	result = build_pattern(pattern, var_names, wildcard_present, wildcard_number);
	
	return result;
}
/*
Combining_Node * Root_Node::start_find_pattern(const Clause& pattern)
{
	Variable_list var_names;
	bool wildcard_present = false;
	int wildcard_number = 0;
	Combining_Node * result = 0;
	
	// start finding the pattern from this node
	result = find_pattern(pattern, var_names, wildcard_present, wildcard_number);
	
	return result;
}
*/

bool Root_Node::start_add_clause(const Clause& clause) const
{
	if (Discrimination_Node::debug_flag) {
		PPS_out << "start_add_clause input:\n";
		clause.display(PPS_out);
		PPS_out << endl;
		}
		
	Binding_set bindings;
	bool return_status = false;
	Clause_state saved_state = clause.get_state(); // somehow omitted earlier - DK, 2/4/04

	for (Discrimination_Node_ptr_list_t::const_iterator it = successors.begin(); it != successors.end(); it++) {
		clause.set_state(saved_state); // somehow omitted earlier - DK, 2/4/04
		return_status = (*it)->add_clause(clause, bindings) || return_status;
		}

	return return_status;
}

bool Root_Node::start_remove_clause(const Clause& clause) const
{
	
	if (Discrimination_Node::debug_flag) {
		PPS_out << "start_remove_clause input:\n";
		clause.display(PPS_out);
		PPS_out << endl;
		}
		
	Binding_set bindings;
	bool return_status = false;
	Clause_state saved_state = clause.get_state(); // somehow omitted earlier - DK, 2/4/04

	for (Discrimination_Node_ptr_list_t::const_iterator it = successors.begin(); it != successors.end(); it++) {
		clause.set_state(saved_state); // somehow omitted earlier - DK, 2/4/04
		return_status = (*it)->remove_clause(clause, bindings) || return_status;
		}

	return return_status;
}


bool Discrimination_Node::debug_flag = false;

bool Discrimination_Node::add_clause(const Clause&, const Binding_set&) const
		{return false;}
bool Discrimination_Node::remove_clause(const Clause&, const Binding_set&) const
		{return false;}

void Discrimination_Node::display_contents(int indent_level) const
{
	// output the required number of leading spaces - simple, brute force here
	for(int n = 0; n < indent_level*2; n++)
		PPS_out << ' ';
	PPS_out << term << endl;
	indent_level++;
	
	if (pattern_node_ptr) {
		pattern_node_ptr->display_contents("", false, indent_level);
		}
		
	for (Discrimination_Node_ptr_list_t::const_iterator it = successors.begin(); it != successors.end(); it++)
		(*it)->display_contents(indent_level);	
}


Discrimination_Node * Discrimination_Node::create(const Symbol& current_term)
{
//	PPS_out << "create Discrimination_Node using " << current_term << endl;

	// create and return the appropriate node
	if (is_wildcard(current_term))
		return new Variable_Discrimination_Node(current_term);
	else if (is_variable(current_term))
		return new Variable_Discrimination_Node(current_term);
	else
		return new Constant_Discrimination_Node(current_term);
}


Combining_Node * Discrimination_Node::build_pattern(
	const Clause& pattern, Variable_list& var_names, bool wildcard_present, int wildcard_number)
{
//	PPS_out << "build_pattern with " << pattern << ", " << var_names << ", " << wildcard_present << " and " << wildcard_number << endl;

	// at the end of the pattern, return the pattern node, creating it if necessary
	if(pattern.at_end()) {
		if (!pattern_node_ptr)
			pattern_node_ptr = Pattern_Node::create(pattern, var_names, wildcard_present);
		return pattern_node_ptr;
		}
	
	Symbol current_term = pattern.get_current_term();
	if (is_variable(current_term)) {
		var_names.add(current_term);
		}
	else if (is_wildcard(current_term)) {
		current_term = full_wildcard_name(wildcard_number);
		wildcard_present = true;
		}
	pattern.advance();
	wildcard_number++;	// always advance the wildcard number
	
	// find the successor, if any, that matches this term; build it if it doesn't exist;
	Discrimination_Node * successor = 0;
	for (Discrimination_Node_ptr_list_t::const_iterator it = successors.begin(); it != successors.end(); it++) {
		if ((*it)->term == current_term) {
			successor = (*it);
			break;
			}
		}

	if(!successor) {
		successor = create(current_term);
		add_successor(successor);
		}
	
	// call the successor
	
	Combining_Node * result = successor->build_pattern(pattern, var_names, wildcard_present, wildcard_number);
	return result;
}


/*
// if the build flag is on,
// 	nodes are constructed and final pattern node returned
// otherwise, 
//	the pattern is search for, and final node return if present, zero otherwise
Combining_Node * Discrimination_Node::build_or_find_pattern(
	const Clause& pattern, Variable_list& var_names, bool wildcard_present, int wildcard_number, bool build_flag)
{
//	PPS_out << "build_pattern with " << pattern << ", " << var_names << ", " << wildcard_present << " and " << wildcard_number << endl;

	// at the end of the pattern, return the pattern node, creating it if necessary if the build flag is on
	if(pattern.at_end()) {
		if (!pattern_node_ptr) {
			if (build_flag)
				pattern_node_ptr = Pattern_Node::create(pattern, var_names, wildcard_present);
			else
				return 0;
			}
		return pattern_node_ptr;
		}
	
	Symbol current_term = pattern.get_current_term();
	if (is_variable(current_term)) {
		var_names.add(current_term);
		}
	else if (is_wildcard(current_term)) {
		current_term = full_wildcard_name(wildcard_number);
		wildcard_present = true;
		}
	pattern.advance();
	wildcard_number++;	// always advance the wildcard number
	
	// find the successor, if any, that matches this term; build it if it doesn't exist if the build flag is on
	Discrimination_Node * successor = 0;
	for (Discrimination_Node_ptr_list_t::const_iterator it = successors.begin(); it != successors.end(); it++) {
		if ((*it)->term == current_term) {
			successor = (*it);
			break;
			}
		}

	if(!successor) {
		if (build_flag) {
			successor = create(current_term);
			add_successor(successor);
			}
		else
			return 0;	// no successor
		}
	
	// call the successor
	
	Combining_Node * result = successor->build_or_find_pattern(
		pattern, var_names, wildcard_present, wildcard_number, build_flag);
	return result;
}
*/

bool Constant_Discrimination_Node::add_clause(const Clause& clause, const Binding_set& binding_set) const
{
	if (Discrimination_Node::debug_flag) {
		PPS_out << "add_clause input:\n";
		clause.display(PPS_out);
		PPS_out << endl;
		PPS_out << "compare to " << term << endl;
		}
		
	// if at the end of the clause, or the current term does not match this node's term, do nothing further
	if(clause.at_end() || clause.get_current_term() != term)
		return false;
		
	// here if the current term matches the term in the node
	
	// advance the term
	clause.advance();
	// if at the end of the clause, but there is no pointer to a pattern node here,
	// the match failed, do nothing further here
	if (clause.at_end() && !pattern_node_ptr)
		return false;
	
	// if we are at a pattern node, and at the clause end, add the binding set to that node and return true
	// (modified dk 8/4/04)
	if (clause.at_end() && pattern_node_ptr) {
		pattern_node_ptr->add_clause(binding_set);
		return true;
		}
	
	// we are still looking for the pattern node
	// save the state, and call each successor with the current state; 
	// return true if a true comes back
	
	Clause_state saved_state = clause.get_state();
	bool return_status = false;
	
	for (Discrimination_Node_ptr_list_t::const_iterator it = successors.begin(); it != successors.end(); it++) {
		clause.set_state(saved_state);
		return_status = (*it)->add_clause(clause, binding_set) || return_status;
		}

	return return_status;
}

bool Constant_Discrimination_Node::remove_clause(const Clause& clause, const Binding_set& binding_set) const
{
	if (Discrimination_Node::debug_flag) {
		PPS_out << "remove_clause input:\n";
		clause.display(PPS_out);
		PPS_out << endl;
		PPS_out << "compare to " << term << endl;
		}
		
	// if at the end of the clause, or the current term does not match this node's term, do nothing further
	if(clause.at_end() || clause.get_current_term() != term)
		return false;
		
	// here if the current term matches the term in the node
	
	// advance the term
	clause.advance();
	// if at the end of the clause, but there is no pointer to a pattern node here,
	// the match failed, do nothing further here
	if (clause.at_end() && !pattern_node_ptr)
		return false;
	
	// if we are at a pattern node, and at the clause end, add the binding set to that node and return true
	// (modified dk 8/4/04)
	if (clause.at_end() && pattern_node_ptr) {
		pattern_node_ptr->remove_clause(binding_set);
		return true;
		}
	
	// we are still looking for the pattern node
	// save the state, and call each successor with the current state; 
	// return true if a true comes back
	
	Clause_state saved_state = clause.get_state();
	bool return_status = false;
	
	for (Discrimination_Node_ptr_list_t::const_iterator it = successors.begin(); it != successors.end(); it++) {
		clause.set_state(saved_state);
		return_status = (*it)->remove_clause(clause, binding_set) || return_status;
		}

	return return_status;
}

bool Variable_Discrimination_Node::add_clause(const Clause& clause, const Binding_set& current_binding_set) const
{
	if (Discrimination_Node::debug_flag) {
		PPS_out << "add_clause input:\n";
		clause.display(PPS_out);
		PPS_out << endl;
		PPS_out << "bind to " << term << endl;
		}
		
	// if at the end of the clause, do nothing further
	if(clause.at_end())
		return false;

	// create a binding_pair with the current term and the variable name (this node's term)
	Binding_pair new_pair(term, clause.get_current_term());
	
	// advance the term
	clause.advance();
	// if at the end of the clause, but there is no pointer to a pattern node here,
	// the match failed, do nothing further here
	if (clause.at_end() && !pattern_node_ptr)
		return false;
	
	// add the new pair to a copy of the binding_set.
	Binding_set binding_set = current_binding_set;
	binding_set.add_binding_pair(new_pair);
	
	// if we are at a pattern node, and at the clause end, add the binding set to that node and return true
	// (modified dk 8/4/04)
	if (clause.at_end() && pattern_node_ptr) {
		pattern_node_ptr->add_clause(binding_set);
		return true;
		}
	
	// we are still looking for the pattern node
	// save the state, and call each successor with the current state; 
	// return true if a true comes back
	
	Clause_state saved_state = clause.get_state();
	bool return_status = false;
	
	for (Discrimination_Node_ptr_list_t::const_iterator it = successors.begin(); it != successors.end(); it++) {
		clause.set_state(saved_state);
		return_status = (*it)->add_clause(clause, binding_set) || return_status;
		}

	return return_status;
}

bool Variable_Discrimination_Node::remove_clause(const Clause& clause, const Binding_set& current_binding_set) const
{
	if (Discrimination_Node::debug_flag) {
		PPS_out << "remove_clause input:\n";
		clause.display(PPS_out);
		PPS_out << endl;
		PPS_out << "bind to " << term << endl;
		}
		
	// if at the end of the clause, do nothing further
	if(clause.at_end())
		return false;

	// create a binding_pair with the current term and the variable name (this node's term)
	Binding_pair new_pair(term, clause.get_current_term());
		
	// advance the term
	clause.advance();
	// if at the end of the clause, but there is no pointer to a pattern node here,
	// the match failed, do nothing further here
	if (clause.at_end() && !pattern_node_ptr)
		return false;
	
	// add the new pair to a copy of the binding_set.
	Binding_set binding_set = current_binding_set;
	binding_set.add_binding_pair(new_pair);
	
	// if we are at a pattern node, and at the clause end, add the binding set to that node and return true
	// (modified dk 8/4/04)
	if (clause.at_end() && pattern_node_ptr) {
		pattern_node_ptr->remove_clause(binding_set);
		return true;
		}
	
	// we are still looking for the pattern node
	// save the state, and call each successor with the current state; 
	// return true if a true comes back
	
	Clause_state saved_state = clause.get_state();
	bool return_status = false;
	
	for (Discrimination_Node_ptr_list_t::const_iterator it = successors.begin(); it != successors.end(); it++) {
		clause.set_state(saved_state);
		return_status = (*it)->remove_clause(clause, binding_set) || return_status;
		}

	return return_status;
}

} // end namespace

