#include "And_Nodes.h"
#include "Binding_set.h"
#include "Binding_set_list.h"
#include "PPS_globals.h"
#include "Assert_throw.h"

#include <iostream>

using std::cout; using std::endl;

namespace Parsimonious_Production_System {


bool And_Node::debug_flag = false;

// display for debugging purposes

void And_Node::display_positive_update(const Combining_Node * source_node, const Combining_Node * other_node) const
{
	PPS_out << "positive update from: ";
	source_node->display_state();
	PPS_out << "other input: ";
	other_node->display_state();
}

void And_Node::display_negative_update(const Combining_Node * source_node, const Combining_Node * other_node) const
{
	PPS_out << "negative update from: ";
	source_node->display_state();
	PPS_out << "other input: ";
	other_node->display_state();
}


// source has turned on
bool Simple_And_Node::positive_update(const Combining_Node * source_node)
{	
	const Combining_Node * other_node = get_other_node(source_node);
		
	if (And_Node::debug_flag)
		display_positive_update(source_node, other_node);

	// positive update means that source node has turned on;
	// if this node turns on, propagate change; otherwise do nothing.
	if (state)
		return false;	// already on
	if (!other_node->get_state())
		return false;	// other node is off
	state = true;		// turn on
	propagate_positive();
	return true;
}

// source has turned off
bool Simple_And_Node::negative_update(const Combining_Node * source_node)
{	
	const Combining_Node * other_node = get_other_node(source_node);
		
	if (And_Node::debug_flag)
		display_negative_update(source_node, other_node);
	
	// negative update means that source node has turned off;
	// if this node then turns off, propagate change; otherwise do nothing
	if (!state)
		return false;	// already off
	state = false;		// turn off
	propagate_negative();
	return true;
}

// display the contents of the node
void Simple_And_Node::display_contents(const char * label, bool dynamic, int indent_level) const
{
	Combining_Node::display_contents(label, dynamic, indent_level);
	indent_output(indent_level+2);
	PPS_out << "Inputs: Simple A:" << input_A->get_id() << ", Simple B:" << input_B->get_id() << endl;
	indent_level++;
	input_A->display_contents("Simple A:", dynamic, indent_level);
	input_B->display_contents("Simple B:", dynamic, indent_level);
}


// the input has either added bindings, or just turned on
// input A is the simple input, input B is the bindings input
bool Simple_Bindings_And_Node::positive_update(const Combining_Node * source_node)
{	
	Assert(input_A != 0 && input_B != 0 );
	Assert(source_node == input_A || source_node == input_B);

	if (source_node == input_A)
		return positive_update_from_simple();
	else
		return positive_update_from_bindings();
}	

// the input_A has turned on
bool Simple_Bindings_And_Node::positive_update_from_simple()
{	
	if (And_Node::debug_flag)
		display_positive_update(input_A, input_B);
	
	// The simple input has turned on. This node must have been off before.
	// But if the other node is off, then this node stays off, and nothing changes
	if(!input_B->get_state())
		return false;
	// The other input is on, turn on, make our bindings same as other input's,
	// and propagate them. 
	state = true;
	binding_sets = input_B->get_binding_sets();	// copy - yuch
	delta_binding_sets = binding_sets;	// copy - yuch
	propagate_positive();
	// for debugging
	delta_binding_sets.clear();
	return true;
}

// input_B has added bindings
bool Simple_Bindings_And_Node::positive_update_from_bindings()
{	
	if (And_Node::debug_flag)
		display_positive_update(input_B, input_A);
	
	// added 8/4/00
	// if the other(simple) input node is off, 
	// then this node must be off, and stays off,
	// and we do nothing with the bindings at all
	if(!input_A->get_state())
		return false;

	// The bindings input has added bindings or turned on 
	// If this node is on, or (it is off and the other node is on)
	// then node changes, is on, add the bindings and propagate the change.
	// add the input bindings to this node's
	bool changed = binding_sets.add_binding_sets(input_B->get_delta_binding_sets()); 
	// if binding sets didn't change, nothing further to do.
	if (!changed)
		return false;
	// conditional removed here 8/4/00
	// binding was added, and the other input is on.  Turn on (in case was off)
	// and propage the supplied changed binding.
	state = true;
	//  propagate the bindings - delta is same as input
	delta_binding_sets = input_B->get_delta_binding_sets();	// copy - yuch
	propagate_positive();
	// for debugging
	delta_binding_sets.clear();
	return true;
}


// the input has either removed bindings, or just turned off
bool Simple_Bindings_And_Node::negative_update(const Combining_Node * source_node)
{	
	Assert(input_A != 0 && input_B != 0 );
	Assert(source_node == input_A || source_node == input_B);

	if (source_node == input_A)
		return negative_update_from_simple();
	else
		return negative_update_from_bindings();
}	

// The input_B has removed some bindings
bool Simple_Bindings_And_Node::negative_update_from_bindings()
{	
	if (And_Node::debug_flag)
		display_negative_update(input_B, input_A);

	bool changed = binding_sets.remove_binding_sets(input_B->get_delta_binding_sets());
	// if binding sets didn't change, nothing further to do.
	if (!changed)
		return false;
	// if this node was already off, nothing further to do.
	if (!state)
		return false;
	// here if node is on and bindings have changed.
	// a binding set was removed, if now empty, state becomes off
	if (binding_sets.empty())
		state = false;
	// delta bindings are those that were removed
	delta_binding_sets = input_B->get_delta_binding_sets();	// copy - yuch
	propagate_negative();
	// for debugging
	delta_binding_sets.clear();
	return true;
}

// The simple input_A has turned off
bool Simple_Bindings_And_Node::negative_update_from_simple()
{	
	if (And_Node::debug_flag)
		display_negative_update(input_A, input_B);

	// if this node was already off, nothing further to do.
	if (!state)
		return false;
	// Otherwise this node is now turned off
	state = false;
	// delta bindings are those that were present
	delta_binding_sets = binding_sets;	// copy - yuch
	binding_sets.clear();	// this node no longer has bindings
	propagate_negative();
	return true;
}

// display the static contents of the node
void Simple_Bindings_And_Node::display_contents(const char * label, bool dynamic, int indent_level) const
{
	Combining_Node::display_contents(label, dynamic, indent_level);
	indent_output(indent_level+2);
	PPS_out << "Inputs: Simple A:" << input_A->get_id() << ", Bindng B:" << input_B->get_id() << endl;
	indent_level++;
	input_A->display_contents("Simple A:", dynamic, indent_level);
	input_B->display_contents("Bindng B:", dynamic, indent_level);
}

} // end namespace

