#include "Combining_Nodes.h"
#include "Binding_set.h"
#include "Binding_set_list.h"
#include "PPS_globals.h"
#include "Assert_throw.h"

#include <iostream>
#include <typeinfo>
#include <string>

using std::cout; using std::endl; using std::cerr;
using std::string;
//	using std::typeid;

namespace Parsimonious_Production_System {

const Symbol_list_t c_empty_symbol_list;
const Variable_list c_empty_variable_list;
const Binding_set_list c_empty_binding_set_list;


// define static member variables
int Combining_Node::next_node_number = 0;

bool Combining_Node::debug_flag = false;

// display the contents of the node
void Combining_Node::display_contents(const char * label, bool dynamic, int indent_level) const
{
	indent_output(indent_level);
//	PPS_out << label << clean_typeid_name() << ' ' << id << endl;
	PPS_out << label << clean_typeid_name() << ' ' << id;
	if(dynamic) {
		PPS_out << " is " << ((state) ? "on" : "off") << endl;
		display_state(indent_level + 2);
		}
	else
		PPS_out << endl;
}

// scan the input from the end to the first colon (if any) and 
// return a string containing the contents from that point to the end
// this code is implementation specific - doesn't work properly for g++!
string Combining_Node::clean_typeid_name() const
{
	string typeid_name(typeid(*this).name());
	string::size_type colon_pos = typeid_name.rfind(":");
	if(colon_pos == string::npos)
		return typeid_name;
	return typeid_name.substr(colon_pos + 1);
}

// assumes it is called from display_contents with dynamic on
void Combining_Node::display_state(int indent_level) const 
{
//	indent_output(indent_level);
//	PPS_out << clean_typeid_name() << ' ' << id << " is " << ((state) ? "on" : "off") << endl;
}

// Bindings_Node override display_state
// assumes it is called from display_contents with dynamic on
void Bindings_Node::display_state(int indent_level) const 
{
//	indent_output(indent_level);
//	PPS_out << clean_typeid_name() << ' ' << id << " is " << ((state) ? "on" : "off") << endl;
	indent_output(indent_level);
	PPS_out << "bindings: "<< binding_sets << endl;
	indent_output(indent_level);
	PPS_out << "delta: " << delta_binding_sets << endl;
}


// Fat Interface for all possible subclasses of Combining_Node
// There are dummy definitions of these to catch erroneous calls


// reset this node - done before starting a new execution run
// note that multiple resets are possible if a node has multiple predecessors
void Combining_Node::reset()
{
	// reset successors
	for (Combining_Node_ptr_list_t::const_iterator it = successors.begin(); it != successors.end(); it++)
		(*it)->reset();
	// reset this node
	state = false;
//	PPS_out << "reset of node " << id << endl;
}

// add this node and its descendants to the set of nodes
void Combining_Node::accumulate_nodes(Combining_Node_ptr_set_t& nodes)
{
	nodes.insert(this);
	for (Combining_Node_ptr_list_t::const_iterator it = successors.begin(); it != successors.end(); it++)
		(*it)->accumulate_nodes(nodes);
}

bool Combining_Node::add_clause(const Binding_set&)
{
	Assert("add_clause called on improper node ");
	return false;
}

bool Combining_Node::remove_clause(const Binding_set&)
{
	Assert("remove_clause called on improper node");
	return false;
}

// access pattern and variable list data (implemented by Pattern_Nodes)
const Symbol_list_t& Combining_Node::get_pattern() const
{
	Assert("get_pattern called on improper node");
	return c_empty_symbol_list;
}

const Variable_list& Combining_Node::get_variables() const
{
	Assert("get_variables called on improper node");
	return c_empty_variable_list; 
}

bool Combining_Node::positive_update(const Combining_Node * source_node)
{
	Assert("positive_update called on improper node");
	if (source_node)
		cerr << "source: " << source_node->get_id() << endl;
	return false;
}

bool Combining_Node::negative_update(const Combining_Node * source_node)
{
	Assert("negative_update called on improper node");
	if (source_node)
		cerr << "source: " << source_node->get_id() << endl;
	return false;
}

const Binding_set_list& Combining_Node::get_binding_sets() const
{
	Assert("get_binding_sets called on improper node");
	return c_empty_binding_set_list;
}

const Binding_set_list& Combining_Node::get_delta_binding_sets() const
{
	Assert("get_delta_binding_sets called on improper node");
	return c_empty_binding_set_list;
}

// ** Test Nodes ***
// Used for debugging purposes

// input has turned on
bool Simple_Test_Node::positive_update(const Combining_Node * source_node)
{	
	Assert(source_node != 0);
		
	display_state();
	PPS_out << "positive_update from " << source_node->get_id() << endl;
		 
	if (!state) {
		PPS_out << "changed:\n";
		state = true;
		display_state();
		return true;
		}

	return false;
}

// input has turned off
bool Simple_Test_Node::negative_update(const Combining_Node * source_node)
{	
	Assert(source_node != 0);
		
	display_state();
	PPS_out << "negative_update from " << source_node->get_id() << endl;
		 
	if (state) {
		PPS_out << "changed:\n";
		state = false;
		display_state();
		return true;
		}

	return false;
}


// the binding set list comes from the pointed-to predecessor
bool Bindings_Test_Node::positive_update(const Combining_Node * source_node)
{	
	Assert(source_node != 0);
		
	display_state();
	PPS_out << "positive_update from ";
	source_node->display_state();
		 
	bool changed = binding_sets.add_binding_sets(source_node->get_delta_binding_sets());
	if (!changed)
		return false;
	
	// changing by adding a binding set automatically means this node is on
	state = true;

	PPS_out << "changed: ";
	display_state();

	return true;
}

bool Bindings_Test_Node::negative_update(const Combining_Node * source_node)
{	
	Assert(source_node != 0);
			
	display_state();
	PPS_out << "negative_update from ";
	source_node->display_state();

	bool changed = binding_sets.remove_binding_sets(source_node->get_delta_binding_sets());
	if (!changed)
		return false;
	
	// a binding set was removed, if now empty, state becomes off
	if (binding_sets.empty())
		state = false;

	PPS_out << "changed: ";
	display_state();

	return true;
}

} // end namespace



