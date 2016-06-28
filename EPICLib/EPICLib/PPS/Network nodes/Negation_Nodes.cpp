#include "Negation_Nodes.h"
#include "PPS_globals.h"
#include "Binding_set.h"
#include "Binding_set_list.h"
#include "Assert_throw.h"

#include <iostream>

using std::cout; using std::endl;


namespace Parsimonious_Production_System {

bool Negation_Node::debug_flag = false;

void Negation_Node::display_positive_update_from_positive() const
{
	PPS_out << "positive update from positive: ";
	input_positive->display_state();
	PPS_out << "negative input: ";
	input_negative->display_state();
}

void Negation_Node::display_negative_update_from_positive() const
{
	PPS_out << "negative update from positive: ";
	input_positive->display_state();
	PPS_out << "negative input: ";
	input_negative->display_state();
}

void Negation_Node::display_positive_update_from_negative() const
{
	PPS_out << "positive update from negative: ";
	input_negative->display_state();
	PPS_out << "positive input: ";
	input_positive->display_state();
}

void Negation_Node::display_negative_update_from_negative() const
{
	PPS_out << "negative update from negative: ";
	input_negative->display_state();
	PPS_out << "positive input: ";
	input_positive->display_state();
}

// the binding set list comes from the pointed-to predecessor
bool Simple_Negation_Node::positive_update(const Combining_Node * source_node)
{	
	Assert(input_positive != 0 && input_negative != 0 );
	Assert(source_node == input_positive || source_node == input_negative);

	if (source_node == input_positive)
		return this->positive_update_from_positive();
	else
		return this->positive_update_from_negative();
}	

bool Simple_Negation_Node::negative_update(const Combining_Node * source_node)
{	
	Assert(input_positive != 0 && input_negative != 0 );
	Assert(source_node == input_positive || source_node == input_negative);

	if (source_node == input_positive)
		return this->negative_update_from_positive();
	else
		return this->negative_update_from_negative();
}

// The positive input node has turned on
bool Simple_Negation_Node::positive_update_from_positive()
{
	if (Negation_Node::debug_flag) {
		display_state();
		display_positive_update_from_positive();
		}

	// positive input has turned on. If this node is already on, no change.
	// If currently off, and negative input is also off, then this node turns on.
	if (state)
		return false;	// already on
	if (input_negative->get_state())
		return false;	// negative input node is on
	state = true;		// turn on
	propagate_positive();
	return true;
}

// The negative input node has turned on
bool Simple_Negation_Node::positive_update_from_negative()
{
	if (Negation_Node::debug_flag) {
		display_state();
		display_positive_update_from_negative();
		}

	// negative input node has turned on. If this node is already off, no change.
	// If currently on, turn off unconditionally.
	if (!state)
		return false;	// already off
	state = false;		// turn off
	propagate_negative();
	return true;
}

// positive input has turned off
bool Simple_Negation_Node::negative_update_from_positive()
{
	if (Negation_Node::debug_flag) {
		display_state();
		display_negative_update_from_positive();
		}

	// positive input node has turned off. If this node is already off, no change.
	// If currently on, turn off unconditionally.
	if (!state)
		return false;	// already off
	state = false;		// turn off
	propagate_negative();
	return true;
}

// negative input has turned off
bool Simple_Negation_Node::negative_update_from_negative()
{
	if (Negation_Node::debug_flag) {
		display_state();
		display_negative_update_from_negative();
		}
		
	// negative input node has turned off. If this node is already on, no change.
	// If currently off, and positive input is on, turn on.
	if (state)
		return false;	// already on
	if (!input_positive->get_state())
		return false;	// positive input node is off, stay off
	state = true;		// turn on
	propagate_positive();
	return true;
}

// display the static contents of the node
void Simple_Negation_Node::display_contents(const char * label, bool dynamic, int indent_level) const
{
	Combining_Node::display_contents(label, dynamic, indent_level);
	indent_output(indent_level+2);
	PPS_out << "Inputs: Simple +:" << input_positive->get_id() << ", Simple -:" << input_negative->get_id() << endl;
	indent_level++;
	input_positive->display_contents("Simple +:", dynamic, indent_level);
	input_negative->display_contents("Simple -:", dynamic, indent_level);
}

// display the static contents of the node
// rest of functionality is same as Simple_Negation_Node
void Simple_Bindings_Negation_Node::display_contents(const char * label, bool dynamic, int indent_level) const
{
	Combining_Node::display_contents(label, dynamic, indent_level);
	indent_output(indent_level+2);
	PPS_out << "Inputs: Simple +:" << input_positive->get_id() << ", Bindng -:" << input_negative->get_id() << endl;
	indent_level++;
	input_positive->display_contents("Simple +:", dynamic, indent_level);
	input_negative->display_contents("Bindng -:", dynamic, indent_level);
}



bool Bindings_Simple_Negation_Node::positive_update(const Combining_Node * source_node)
{	
	Assert(input_positive != 0 && input_negative != 0 );
	Assert(source_node == input_positive || source_node == input_negative);

	if (source_node == input_positive)
		return this->positive_update_from_positive();
	else
		return this->positive_update_from_negative();
}	

bool Bindings_Simple_Negation_Node::negative_update(const Combining_Node * source_node)
{	
	Assert(input_positive != 0 && input_negative != 0 );
	Assert(source_node == input_positive || source_node == input_negative);

	if (source_node == input_positive)
		return this->negative_update_from_positive();
	else
		return this->negative_update_from_negative();
}

// display the static contents of the node
void Bindings_Simple_Negation_Node::display_contents(const char * label, bool dynamic, int indent_level) const
{
	Combining_Node::display_contents(label, dynamic, indent_level);
	indent_output(indent_level+2);
	PPS_out << "Inputs: Bindng +:" << input_positive->get_id() << ", Simple -:" << input_negative->get_id() << endl;
	indent_level++;
	input_positive->display_contents("Bindng +:", dynamic, indent_level);
	input_negative->display_contents("Simple -:", dynamic, indent_level);
}


// Bindings_Simple_Negation_Node
// The only thing relevant about the negation input is whether it is on or off
// If the negative input is on, then nothing comes out of this node.
// If the negative input is off, then the positive input comes out.
// If the positive input has come on, then all the added bindings come out;
// If the positive input has gone off, then all of the removed bindings come out

// the positive input has added bindings
bool Bindings_Simple_Negation_Node::positive_update_from_positive()
{
	if (Negation_Node::debug_flag) {
		display_state();
		display_positive_update_from_positive();
		}

	// positive node is source of increment; if negative is not on, add binding sets
	// output delta will be same as input delta
	bool changed = false;
	if (!input_negative->get_state())
		changed = binding_sets.add_binding_sets(input_positive->get_delta_binding_sets());
	// if binding sets didn't change, nothing further to do.
	if (!changed)
		return false;
	// since a binding set was added, state must be on now.
	state = true;
	// output delta is same as input delta
	delta_binding_sets = input_positive->get_delta_binding_sets(); // make a copy - yuch
	propagate_positive();
	// for debugging
	delta_binding_sets.clear();
	return true;
}

// the negative input has turned on
bool Bindings_Simple_Negation_Node::positive_update_from_negative()
{
	if (Negation_Node::debug_flag) {
		display_state();
		display_positive_update_from_negative();
		}

	// negative node has turned on, so this node needs to turn off
	// if already off, no change
	if (!state)
		return false;
	// if it was on, it is now off, and current bindings propagate as negative delta
	state = false;
	// output delta is the current bindings
	delta_binding_sets = binding_sets; // make a copy - yuch
	// remove all bindings
	binding_sets.clear();
	propagate_negative();	
	// for debugging
	delta_binding_sets.clear();
	return true;
}

bool Bindings_Simple_Negation_Node::negative_update_from_positive()
{
				
	if (Negation_Node::debug_flag) {
		display_state();
		display_negative_update_from_positive();
		}

	// positive node is source of decrement; remove binding sets regardless of content of negation input
	// The bindings input has removed some bindings, so remove them here. 
	// delta bindings are those that were removed
	bool changed = binding_sets.remove_if_match(Full_Overlap(),
		input_positive->get_delta_binding_sets(), delta_binding_sets);
	// if binding sets didn't change, nothing further to do.
	if (!changed)
		return false;
	// a binding set was removed, if now empty, state becomes off
	if (binding_sets.empty())
		state = false;
	propagate_negative();
	// for debugging
	delta_binding_sets.clear();
	return true;
}

bool Bindings_Simple_Negation_Node::negative_update_from_negative()
{
				
	if (Negation_Node::debug_flag) {
		display_state();
		display_negative_update_from_negative();
		}
		
	// this class is used either:
	//		for an actual Bindings_Simple_Negation node, 
	// 		where the negative input is a simple on/off node, which will only send
	// 		a negative update if it turns off.
	
	// or:
	// 		for a No_Overlap_Negation_Node, where the negative input may be from a
	// 		node with bindings. Such a node can send negative updates and still
	// 		be turned on; the deltas from the negative node are irrelevant.

	// To handle both uses, check the negative input and ignore the negative update if it is still on. -dk 10/15/04
	if(input_negative->get_state())
		return false;
	// negative node has turned off; this node may then turn on if positive input node is on
	// this node is supposed to be off and empty
	Assert(!state && binding_sets.empty());
	// If this node was off, but the positive input is on, 
	// then if the positive input bindings become current bindings,
	// and these bindings are propagated as a positive delta.
	if (!input_positive->get_state())
		return false;
	// turn on
	state = true;
	binding_sets = input_positive->get_binding_sets();
	delta_binding_sets = binding_sets;
	propagate_positive();
	// for debugging
	delta_binding_sets.clear();
	return true;
}

} // end namespace

