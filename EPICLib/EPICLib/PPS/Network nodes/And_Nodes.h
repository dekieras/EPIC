#ifndef AND_NODES_H
#define AND_NODES_H

#include "Assert_throw.h"
#include "Binding_set.h"
#include "Overlap.h"
#include "Binding_set_list.h"
#include "Combining_Nodes.h"
#include "Output_tee_globals.h"

namespace Parsimonious_Production_System {

// And_Node mixin class contains shared variables and functionality
class And_Node {
public:
	And_Node (Combining_Node * in_a, Combining_Node * in_b) :
		input_A(in_a), input_B(in_b)
		{}

	static bool debug_flag;

protected:
	Combining_Node * input_A;
	Combining_Node * input_B;

	// return the node that is not the source
	const Combining_Node * get_other_node (const Combining_Node * source_node) const
	{
		Assert(input_A != 0 && input_B != 0 );
		Assert(source_node == input_A || source_node == input_B);
		if (source_node == input_A)
			return input_B;
		else 
			return input_A;
	}
	
	// display for debugging purposes
	void display_positive_update(const Combining_Node * source_node, const Combining_Node * other_node) const;
	void display_negative_update(const Combining_Node * source_node, const Combining_Node * other_node) const;

private:
};


// A Simple_And_Node has two constant-containing inputs
class Simple_And_Node : public Combining_Node, public And_Node {
public:
	Simple_And_Node(int in_id, Combining_Node * in_a, Combining_Node * in_b) : 
		Combining_Node(in_id), And_Node (in_a, in_b)
		{}

	// source has turned on
	virtual bool positive_update(const Combining_Node * source_node);
	// source has turned off
	virtual bool negative_update(const Combining_Node * source_node);

	// display the contents of the node
	virtual void display_contents(const char * label = "", bool dynamic = false, int indent_level = 0) const;

private:
};


// A Simple_Bindings_And_Node has one constant-containing inputs
// and one input with bindings
// the A input is Simple, the B input is Bindings
class Simple_Bindings_And_Node : public Bindings_Node, public And_Node {
public:
	Simple_Bindings_And_Node(int in_id, Combining_Node * in_a, Combining_Node * in_b) : 
		Bindings_Node(in_id), And_Node (in_a,  in_b)
		{}

	virtual bool positive_update(const Combining_Node * source_node);
	virtual bool negative_update(const Combining_Node * source_node);

	// display the contents of the node
	virtual void display_contents(const char * label = "", bool dynamic = false, int indent_level = 0) const;

private:
	bool positive_update_from_simple();
	bool positive_update_from_bindings();
	bool negative_update_from_simple();
	bool negative_update_from_bindings();
};


// A Bindings_And_Node base class template for And Nodes whose inputs are both bindings
// with the match-types parameterized
// MT - match type for:
// P positive update
// N negative update
template <class PMT, class NMT>
class Bindings_And_Node: public Bindings_Node, public And_Node  {
public:
	Bindings_And_Node(int in_id, Combining_Node * in_a, Combining_Node * in_b) : 
		Bindings_Node(in_id), And_Node (in_a,  in_b)
		{}

	// display the static contents of the node
	virtual void display_contents(const char * label = "", bool dynamic = false, int indent_level = 0) const
	{
		Combining_Node::display_contents(label, dynamic, indent_level);
		indent_output(indent_level+2);
		PPS_out << "Inputs: Bindng A:" << input_A->get_id() << ", Bindng B:" << input_B->get_id() << std::endl;
		indent_level++;
		input_A->display_contents("Bindng A:", dynamic, indent_level);
		input_B->display_contents("Bindng B:", dynamic, indent_level);
	}

	// source has added bindings
	virtual bool positive_update(const Combining_Node * source_node)
	{		
		const Combining_Node * other_node = get_other_node(source_node);
		
		if (And_Node::debug_flag) {
			display_state();
			display_positive_update(source_node, other_node);
			}

		bool changed = binding_sets.add_combination_if_match_source(pos_match_type,
			source_node->get_delta_binding_sets(), other_node->get_binding_sets(), delta_binding_sets);
		// if binding sets didn't change, nothing further to do.
		if (!changed)
			return false;
		// since a binding set was added, state must be on now.
		state = true;
		propagate_positive();
		// for debugging
		delta_binding_sets.clear();
		return true;
	}

	// source has removed bindings
	virtual bool negative_update(const Combining_Node * source_node)
	{	
		const Combining_Node * other_node = get_other_node(source_node);
			
		if (And_Node::debug_flag) {
			display_state();
			display_negative_update(source_node, other_node);
			}
	
		bool changed = binding_sets.remove_if_match(neg_match_type,
			source_node->get_delta_binding_sets(), delta_binding_sets);
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

private:
	PMT pos_match_type;	// for positive update
	NMT neg_match_type;	// for negative update
	Bindings_And_Node();
};


// Full_Overlap_And_Node class
class Full_Overlap_And_Node : public Bindings_And_Node<Full_Overlap, Full_Overlap>
{
public:
	Full_Overlap_And_Node(int in_id, Combining_Node * in_a, Combining_Node * in_b) : 
		Bindings_And_Node<Full_Overlap, Full_Overlap>::Bindings_And_Node(in_id, in_a, in_b)
		{}

private:
};


// Partial_Overlap_And_Node class
class Partial_Overlap_And_Node : public Bindings_And_Node<Partial_Overlap, Partial_Overlap>
{
public:
	Partial_Overlap_And_Node(int in_id, Combining_Node * in_a, Combining_Node * in_b) : 
		Bindings_And_Node<Partial_Overlap, Partial_Overlap>::Bindings_And_Node(in_id, in_a, in_b)
		{}

private:
};

// No_Overlap_And_Node class
class No_Overlap_And_Node : public Bindings_And_Node<No_Overlap, Partial_Overlap>
{
public:
	No_Overlap_And_Node(int in_id, Combining_Node * in_a, Combining_Node * in_b) : 
		Bindings_And_Node<No_Overlap, Partial_Overlap>::Bindings_And_Node(in_id, in_a, in_b)
		{}

private:
};

} // end namespace


#endif

