#include "Pattern_Nodes.h"
#include "PPS_globals.h"
#include "Binding_set.h"
#include "Binding_set_list.h"
#include "Assert_throw.h"

#include <iostream>

using std::cout; using std::endl;
//#include <typeinfo>

namespace Parsimonious_Production_System {

// ** Pattern Nodes ***
bool Pattern_Node::debug_flag = false;

Combining_Node * Pattern_Node::create(const Clause& pattern, const Variable_list& var_names, bool wildcard_present)
{
//	PPS_out << "create Pattern_Node using " << pattern << ", " << var_names << " and " << wildcard_present << endl;
	// build the appropriate kind of Clause node depending on presence of vars and wildcards
	if (var_names.size() == 0 && !wildcard_present)
		return new Simple_Pattern_Node(Combining_Node::get_next_node_number(), pattern.get_clause_term_list());
	else if(var_names.size() > 0 && !wildcard_present)
		return new Bindings_Pattern_Node(Combining_Node::get_next_node_number(),pattern.get_clause_term_list(), var_names);
	else if(var_names.size() == 0 && wildcard_present)
		return new Wildcard_Pattern_Node(Combining_Node::get_next_node_number(), pattern.get_clause_term_list());
	else
		return new Bindings_Wildcard_Pattern_Node(Combining_Node::get_next_node_number(), pattern.get_clause_term_list(), var_names);
}

// Simple_Pattern_Node represents a condition clause in a production rule
// return true for changed, false for unchanged
// input for this node has turned on
bool Simple_Pattern_Node::add_clause(const Binding_set& bs)
{	
	// binding set should be empty
	Assert(bs.empty());
	
	if (Pattern_Node::debug_flag) {
		display_state();
		PPS_out << "add_clause" << endl;
		}

	// this node turns on, reports change if wasn't on before
	if(state)
		return false;	// no change
	
	state = true;
			
	propagate_positive();
	return true;	
}

bool Simple_Pattern_Node::remove_clause(const Binding_set& bs)
{
	// binding set should be empty
	Assert(bs.empty());
	
	if (Pattern_Node::debug_flag) {
		display_state();
		PPS_out << "remove_clause" << endl;
		}

	// this node turns off, reports change if wasn't off before
	if(!state)
		return false;	// no change
	
	state = false;
	
	propagate_negative();
	return true;	
}

void Simple_Pattern_Node::display_contents(const char * label, bool dynamic, int indent_level) const
{
	indent_output(indent_level);
//	PPS_out << label << "Simple_Pattern_Node: " << id << ' ' << pattern << endl;
	PPS_out << label << "Simple_Pattern_Node: " << id << ' ' << pattern;
	if(dynamic) {
		PPS_out << " is " << ((state) ? "on" : "off") << endl;
		display_state(indent_level + 2);
		}
	else
		PPS_out << endl;
}

// return true for changed, false for unchanged
bool Bindings_Pattern_Node::add_clause(const Binding_set& bs)
{
	Assert(!bs.empty());	// input has to be non-empty
	
	if (Pattern_Node::debug_flag) {
		display_state();
		PPS_out << "add_clause input:\n"
			 << bs << endl;
		}

	bool changed = binding_sets.add_binding_set(bs);
	
	// if binding sets didn't change, nothing further to do.
	if (!changed)
		return false;

	// changing by adding a binding set automatically means this node is on
	state = true;
	
	// output delta is a list of the input binding set
	delta_binding_sets.clear();
	delta_binding_sets.add_binding_set(bs);	// output must be in the form of a list

	propagate_positive();

	// for debugging
	delta_binding_sets.clear();

	return true;	
}


bool Bindings_Pattern_Node::remove_clause(const Binding_set& bs)
{	
	Assert(!bs.empty());	// input has to be non-empty

	if (Pattern_Node::debug_flag) {
		display_state();
		PPS_out << "remove_clause input:\n"
			 << bs << endl;
		}
		
	bool changed = binding_sets.remove_binding_set(bs);

	// if binding sets didn't change, nothing further to do.
	if (!changed)
		return false;
	
	// a binding set was removed, if now empty, state becomes off
	if (binding_sets.empty())
		state = false;
		
	// if changed, output delta is a list of the input binding set
	delta_binding_sets.clear();
	delta_binding_sets.add_binding_set(bs);	// output must be in the form of a list

	propagate_negative();

	// for debugging
	delta_binding_sets.clear();

	return true;
}

void Bindings_Pattern_Node::display_contents(const char * label, bool dynamic, int indent_level) const
{
	indent_output(indent_level);
//	PPS_out << label << "Bindings_Pattern_Node: " << id << ' ' << pattern << ' ' << varlist << endl;
	PPS_out << label << "Bindings_Pattern_Node: " << id << ' ' << pattern << ' ' << varlist;
	if(dynamic) {
		PPS_out << " is " << ((state) ? "on" : "off") << endl;
		display_state(indent_level + 2);
		}
	else
		PPS_out << endl;
}


void Wildcard_Pattern_Node::reset() 
{
	Bindings_Pattern_Node::reset();
	wildcard_binding_sets.clear();
}

void Wildcard_Pattern_Node::display_state(int indent_level) const
{
	Bindings_Pattern_Node::display_state(indent_level + 2);
	indent_output(indent_level + 2);
	PPS_out << "wildcard bindings: " << wildcard_binding_sets << endl;
}


// A Wildcards_Pattern_Node stores state for a single clause
// in a rule condition containing only wildcards
// it behaves like a Bindings_Pattern_Node but propagates no bindings
// return true for changed, false for unchanged
bool Wildcard_Pattern_Node::add_clause(const Binding_set& bs)
{
	Assert(!bs.empty());	// input has to be non-empty
	
	if (Pattern_Node::debug_flag) {
		display_state();
		PPS_out << "add_clause input:\n"
			 << bs << endl;
		}

	bool changed = wildcard_binding_sets.add_binding_set(bs);
	// bindings_sets and delta_binding_sets are never changed
	
	// if binding sets didn't change, nothing further to do.
	if (!changed)
		return false;

	// if this node was already on, nothing further to do
	if (state)
		return false;
		
	// if not, changing by adding a binding set automatically means this node is on
	state = true;
	
	// since this node holds wildcards only, no bindings are propagated.
	
	propagate_positive();

	return true;	
}


bool Wildcard_Pattern_Node::remove_clause(const Binding_set& bs)
{	
	Assert(!bs.empty());	// input has to be non-empty

	if (Pattern_Node::debug_flag) {
		display_state();
		PPS_out << "remove_clause input:\n"
			 << bs << endl;
		}
		
	bool changed = wildcard_binding_sets.remove_binding_set(bs);
	// bindings_sets and delta_binding_sets are never changed

	// if binding sets didn't change, nothing further to do.
	if (!changed)
		return false;
	
	// compute new state:	
	// a binding set was removed, if now empty, state will be off
	bool new_state = !wildcard_binding_sets.empty();
		
	// if node was on, and is still on, nothing further to do
	if (state && new_state)
		return false;
	
	// otherwise, node has turned off, propagate the change
		state = new_state;
		
	// since this node holds wildcards only, no bindings are propagated.

	propagate_negative();

	return true;
}

void Wildcard_Pattern_Node::display_contents(const char * label, bool dynamic, int indent_level) const
{
	indent_output(indent_level);
	PPS_out << label << "Wildcard_Pattern_Node: " << id << ' ' << pattern << endl;
	if(dynamic)
		display_state(indent_level + 2);
}


// A Bindings_Wildcard_Pattern_Node stores state for a single clause
// in a rule condition containing both variables and wildcards
// it behaves like a Bindings_Pattern_Node but propagates only variable bindings

bool Bindings_Wildcard_Pattern_Node::add_clause(const Binding_set& bs)
{
	Assert(!bs.empty());	// input has to be non-empty
	
	if (Pattern_Node::debug_flag) {
		display_state();
		PPS_out << "add_clause input:\n"
			 << bs << endl;
		}

	bool changed = wildcard_binding_sets.add_binding_set(bs);
	
	// if binding sets didn't change, nothing further to do.
	if (!changed)
		return false;

	// changing by adding a binding set automatically means this node is on
	state = true;
	
	// current binding sets updated by remove wildcards of bs
	binding_sets.add_binding_set(bs.remove_wildcards());
	
	// output delta is a list of the input binding set but without any wildcards in it
	delta_binding_sets.clear();
	delta_binding_sets.add_binding_set(bs.remove_wildcards());	// output must be in the form of a list

	propagate_positive();

	// for debugging
	delta_binding_sets.clear();

	return true;	
}


bool Bindings_Wildcard_Pattern_Node::remove_clause(const Binding_set& bs)
{	
	Assert(!bs.empty());	// input has to be non-empty

	if (Pattern_Node::debug_flag) {
		display_state();
		PPS_out << "remove_clause input:\n"
			 << bs << endl;
		}
		
	bool changed = wildcard_binding_sets.remove_binding_set(bs);

	// if binding sets didn't change, nothing further to do.
	if (!changed)
		return false;
	
	// a binding set was removed, if now empty, state becomes off
	if (wildcard_binding_sets.empty())
		state = false;
		
	// current binding sets updated by remove wildcards of bs
	binding_sets.remove_binding_set(bs.remove_wildcards());

	// if changed, output delta is a list of the input binding set but with wildcards removed
	delta_binding_sets.clear();
	delta_binding_sets.add_binding_set(bs.remove_wildcards());	// output must be in the form of a list

	propagate_negative();

	// for debugging
	delta_binding_sets.clear();

	return true;
}

void Bindings_Wildcard_Pattern_Node::display_contents(const char * label, bool dynamic, int indent_level) const
{
	indent_output(indent_level);
	PPS_out << label << "Bindings_Wildcard_Pattern_Node: " << id << ' ' << pattern << ' ' << varlist << endl;
	if(dynamic)
		display_state(indent_level + 2);
}

} // end namespace




