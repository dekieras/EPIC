#ifndef NEGATION_NODES_H
#define NEGATION_NODES_H

//#include <iostream>
#include "Assert_throw.h"
#include "Binding_set.h"
#include "Overlap.h"
#include "Binding_set_list.h"
#include "Combining_Nodes.h"
#include "Output_tee_globals.h"

namespace Parsimonious_Production_System {

// Negation_Node mixin class contains shared variables and functionality
class Negation_Node {
public:
	Negation_Node(Combining_Node * in_pos, Combining_Node * in_neg) : 
		input_positive(in_pos), input_negative(in_neg)
		{}

	static bool debug_flag;

protected:
	Combining_Node * input_positive;
	Combining_Node * input_negative;

	void display_positive_update_from_positive() const;
	void display_negative_update_from_positive() const;
	void display_positive_update_from_negative() const;
	void display_negative_update_from_negative() const;

private:
};


// An Simple_Negation_Node class for nodes that have inputs
// that contain only constants - inherits from Negation Combining_Node directly
class Simple_Negation_Node : public Combining_Node, public Negation_Node {
public:
	Simple_Negation_Node(int in_id, Combining_Node * in_pos, Combining_Node * in_neg) : 
		Combining_Node(in_id), Negation_Node(in_pos, in_neg)
		{}

	virtual bool positive_update(const Combining_Node * source_node);
	virtual bool negative_update(const Combining_Node * source_node);

	// display the contents of the node
	virtual void display_contents(const char * label = "", bool dynamic = false, int indent_level = 0) const;

protected:
	virtual bool positive_update_from_positive();
	virtual bool positive_update_from_negative();
	virtual bool negative_update_from_positive();
	virtual bool negative_update_from_negative();

private:
};


// A Simple_Bindings_Negation_Node class for nodes that have a simple positive input
// and a bindings negative input - bindings are irrelevant; only state
// So simply inherits from Simple_Negation_Node

class Simple_Bindings_Negation_Node : public Simple_Negation_Node {
public:
	Simple_Bindings_Negation_Node(int in_id, Combining_Node * in_pos, Combining_Node * in_neg) : 
		Simple_Negation_Node(in_id, in_pos, in_neg)
		{}
	// display the contents of the node
	virtual void display_contents(const char * label = "", bool dynamic = false, int indent_level = 0) const;

private:
};


// An Bindings_Simple_Negation_Node class for nodes that have a bindings positive input
// and a simple negative input
class Bindings_Simple_Negation_Node : public Bindings_Node, public Negation_Node {
public:
	Bindings_Simple_Negation_Node(int in_id, Combining_Node * in_pos, Combining_Node * in_neg) : 
		Bindings_Node(in_id), Negation_Node(in_pos, in_neg)
		{}

	virtual bool positive_update(const Combining_Node * source_node);
	virtual bool negative_update(const Combining_Node * source_node);

	// display the contents of the node
	virtual void display_contents(const char * label = "", bool dynamic = false, int indent_level = 0) const;

protected:
	virtual bool positive_update_from_positive();
	virtual bool positive_update_from_negative();
	virtual bool negative_update_from_positive();
	virtual bool negative_update_from_negative();

private:
};

// No_Overlap_Negation_Node class has two bindings inputs that do not
// overlap in bindings. In this case, the negative input bindings are irrelevant;
// The functionality is identical to Bindings_Simple_Negation_Node

class No_Overlap_Negation_Node : public Bindings_Simple_Negation_Node {
public:
	No_Overlap_Negation_Node(int in_id, Combining_Node * in_pos, Combining_Node * in_neg) : 
		Bindings_Simple_Negation_Node(in_id, in_pos, in_neg)
		{}
private:
};


// A template base class to be specialized and inherited from

// An Bindings_Negation_Node base class for nodes that have inputs
// that both contain binding_sets, with the match-types parameterized
// MT - match type for:
// PP positive update from positive
// PN positive update from negative
// NP negative update from positive
// NN negative update from negative
template <class PPMT, class PNMT, class NPMT, class NNMT>
class Bindings_Negation_Node : public Bindings_Node, public Negation_Node {
public:
	Bindings_Negation_Node(int in_id, Combining_Node * in_pos, Combining_Node * in_neg) : 
		Bindings_Node(in_id), Negation_Node(in_pos, in_neg)
		{}

	virtual bool positive_update(const Combining_Node * source_node)
	{	
		Assert(input_positive != 0 && input_negative != 0 );
		Assert(source_node == input_positive || source_node == input_negative);

		if (source_node == input_positive)
			return this->positive_update_from_positive();
		else
			return this->positive_update_from_negative();
	}
		
	virtual bool negative_update(const Combining_Node * source_node)
	{	
		Assert(input_positive != 0 && input_negative != 0 );
		Assert(source_node == input_positive || source_node == input_negative);

		if (source_node == input_positive)
			return this->negative_update_from_positive();
		else
			return this->negative_update_from_negative();
	}

	// display the static contents of the node
	virtual void display_contents(const char * label, bool dynamic, int indent_level) const
	{
		Combining_Node::display_contents(label, dynamic, indent_level);
		indent_output(indent_level+2);
		PPS_out << "Inputs: Bindng +:" << input_positive->get_id() << ", Bindng -:" << input_negative->get_id() << std::endl;
		indent_level++;
		input_positive->display_contents("Bindng +:", dynamic, indent_level);
		input_negative->display_contents("Bindng -:", dynamic, indent_level);
	}


private:

protected:

	PPMT pos_pos_match_type;	// for positive_update_from_positive
	PNMT pos_neg_match_type;	// for positive_update_from_negative
	NPMT neg_pos_match_type;	// for negative_update_from_positive
	NNMT neg_neg_match_type;	// for negative_update_from_negative

	virtual bool positive_update_from_positive()
	{
		if (Negation_Node::debug_flag) {
			display_state();
			display_positive_update_from_positive();
			}

		// positive node is source of increment; add binding sets that are not in the negative input
		bool changed = binding_sets.add_mod_if_not_match_source(pos_pos_match_type,
			input_positive->get_delta_binding_sets(), input_negative->get_binding_sets(), delta_binding_sets);
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

	virtual bool positive_update_from_negative()
	{
		if (Negation_Node::debug_flag) {
			display_state();
			display_positive_update_from_negative();
			}

		// negative node is source of increment; remove binding sets that consistent with the input delta
		// output delta will be the removed sets, propagate as a removal
		bool changed = binding_sets.remove_if_match(pos_neg_match_type,
			input_negative->get_delta_binding_sets(), delta_binding_sets);
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
	
	virtual bool negative_update_from_positive()
	{
		if (Negation_Node::debug_flag) {
			display_state();
			display_negative_update_from_positive();
			}

		// positive node is source of decrement; remove binding sets that consistent with the input delta
		// output delta will be the removed sets, propagate as a removal
		bool changed = binding_sets.remove_if_match(neg_pos_match_type,
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
	
	virtual bool negative_update_from_negative()
	{
		if (Negation_Node::debug_flag) {
			display_state();
			display_negative_update_from_negative();
			}

		// negative node is source of decrement; add binding sets from positive that are consistent with the negative delta
		// output delta will be binding sets added
		bool changed = binding_sets.add_mod_if_match_source(neg_neg_match_type,
			input_positive->get_binding_sets(), input_negative->get_delta_binding_sets(), delta_binding_sets);
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

};

// negative update from positive always means to remove the fully matching bindings present in the node

// Full_Overlap_Negation_Node class
class Full_Overlap_Negation_Node :
public Bindings_Negation_Node<Full_Overlap, Full_Overlap, Full_Overlap, Full_Overlap>
{
public:
	Full_Overlap_Negation_Node(int in_id, Combining_Node * in_pos, Combining_Node * in_neg) : 
		Bindings_Negation_Node<Full_Overlap, Full_Overlap, Full_Overlap, Full_Overlap>::Bindings_Negation_Node(in_id, in_pos, in_neg)
		{}

private:
	Full_Overlap_Negation_Node();
};


// Partial_Negation_Node class
class Partial_Overlap_Negation_Node : 
public Bindings_Negation_Node<Partial_Overlap, Partial_Overlap, Full_Overlap, Partial_Overlap>
{
public:
	Partial_Overlap_Negation_Node(int in_id, Combining_Node * in_pos, Combining_Node * in_neg) : 
		Bindings_Negation_Node<Partial_Overlap, Partial_Overlap, Full_Overlap, Partial_Overlap>::Bindings_Negation_Node(in_id, in_pos, in_neg)
		{}

private:
	Partial_Overlap_Negation_Node();
};

/*
// Template version not needed
// No_Overlap_Negation_Node class
class No_Overlap_Negation_Node : 
public Bindings_Negation_Node<No_Overlap, No_Overlap, Full_Overlap, No_Overlap>
{
public:
	No_Overlap_Negation_Node(int in_id, Combining_Node * in_pos, Combining_Node * in_neg) : 
		Bindings_Negation_Node(in_id, in_pos, in_neg)
		{}

private:
	No_Overlap_Negation_Node();
};

*/

} // end namespace


#endif

