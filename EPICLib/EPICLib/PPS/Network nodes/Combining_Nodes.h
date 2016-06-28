#ifndef COMBINING_NODES_H
#define COMBINING_NODES_H

#include "Binding_set.h"
#include "Binding_set_list.h"
#include "PPS_globals.h"
//#include "Assert_throw.h"

#include <iostream>
#include <string>
#include <list>
#include <set>


class Command_interface;

namespace Parsimonious_Production_System {

// forward declaration
class Combining_Node;
struct less_Combining_Node_ptr;
typedef std::set<Combining_Node *, less_Combining_Node_ptr> Combining_Node_ptr_set_t;
typedef std::list<Combining_Node *> Combining_Node_ptr_list_t;

// A Combining_Node has a name, state, and sucessors
// This is an abstract class; positive/negative update functions must be overridden

class Combining_Node {
public:

	Combining_Node(int in_id) : 
		id(in_id), state(false)
		{}
	virtual ~Combining_Node()
		{}
	void add_successor(Combining_Node * p)
		{successors.push_back(p);}
	// display the run-time state
	virtual void display_state(int indent_level = 0) const;
	// display the contents of the node
	virtual void display_contents(const char * label = "", bool dynamic = false, int indent_level = 0) const;
	int get_id() const
		{return id;}
	bool get_state() const
		{return state;}
	const Combining_Node_ptr_list_t& get_successors() const
		{return successors;}
	
	// Fat Interface for all possible subclasses of Combining_Node
	// There are dummy definitions of these to catch erroneous calls

	// reset this node to the initial state
	virtual void reset();
	// add this node and its descendants to the set of nodes
	void accumulate_nodes(Combining_Node_ptr_set_t& nodes);
	
	// add and remove database items (implemented by Pattern_Nodes)
	// Add a clause by turning on or adding a binding set
	virtual bool add_clause(const Binding_set& bs);
	// Remove a clause by turning off or removing a binding set
	virtual bool remove_clause(const Binding_set& bs);

	// access pattern and variable list data (implemented by Pattern_Nodes)
	virtual const Symbol_list_t& get_pattern() const;
	virtual const Variable_list& get_variables() const;

	// access binding sets
	virtual const Binding_set_list& get_binding_sets() const;
	// access delta_binding_sets
	virtual const Binding_set_list& get_delta_binding_sets() const;

	// source has turned on or added bindings
	virtual bool positive_update(const Combining_Node * source_node);
	// source has turned off or removed bindings
	virtual bool negative_update(const Combining_Node * source_node);

	// execute the rule actions - implemented by Rule_Nodes
//	virtual void execute(Command_interface&) const;

	static bool debug_flag;
	
	static int get_next_node_number()
		{return next_node_number++;}
		
	static void reset_next_node_number()
		{next_node_number = 0;}

	static int get_number_of_nodes()
		{return next_node_number;}
	

protected:
	const int id;						// id for this node
	bool state; 						// on is true, off is false

	void propagate_positive() const
	{
		if (debug_flag) {
			PPS_out << "propagating positive change:\n";
			display_state();
			}

		// update successors
		for (Combining_Node_ptr_list_t::const_iterator it = successors.begin(); it != successors.end(); it++)
			(*it)->positive_update(this);
	}
	
	void propagate_negative() const
	{
		if (debug_flag) {
			PPS_out << "propagating negative change:\n";
			display_state();
			}

		// update successors
		for (Combining_Node_ptr_list_t::const_iterator it = successors.begin(); it != successors.end(); it++)
			(*it)->negative_update(this);
	}
		
		
	void indent_output(int indent_level) const
	{
		// output the required number of leading spaces - simple, brute force here
		for(int n = 0; n < indent_level*2; n++)
			PPS_out << ' ';
	}
	
	// return a string containing this nodes type name, without any qualifiers
	std::string clean_typeid_name() const;

	
private:
	static int next_node_number;

	Combining_Node_ptr_list_t successors;				// nodes for changes to be propagated to
	Combining_Node(Combining_Node&);
	Combining_Node& operator= (Combining_Node&);
};

// a comparison function object class
struct less_Combining_Node_ptr {
	bool operator() (const Combining_Node * p1, const Combining_Node * p2) const
	{
		return (p1->get_id() < p2->get_id());
	}
};



// Bindings_Node is a Combining_Node that has binding list functionality
class Bindings_Node : public Combining_Node {
public:

	Bindings_Node(int in_id) : 
		Combining_Node(in_id)
		{}
		
	virtual void reset() 
		{
			Combining_Node::reset();
			binding_sets.clear();
			delta_binding_sets.clear();
		}
	virtual void display_state(int indent_level = 0) const;	// display state and bindings

	const Binding_set_list& get_binding_sets() const
		{return binding_sets;}
	const Binding_set_list& get_delta_binding_sets() const
		{return delta_binding_sets;}		
			
protected:
	Binding_set_list binding_sets;			// current bindings for this node
	Binding_set_list delta_binding_sets;	// current delta bindings to be propagated
	
private:
	Bindings_Node& operator= (Bindings_Node&);
};

// ** Test Nodes ***
// Used for debugging purposes
// A Test_Node is a Combining_Node that simply reports its changed state

class Simple_Test_Node : public Combining_Node {
public:
	Simple_Test_Node(int in_id) :
		Combining_Node(in_id)
		{}
	
	// source has turned on or added bindings
	virtual bool positive_update(const Combining_Node * source_node);
	// source has turned off or removed bindings
	virtual bool negative_update(const Combining_Node * source_node);

private:
	Simple_Test_Node();	
};

class Bindings_Test_Node : public Bindings_Node {
public:
	Bindings_Test_Node(int in_id) :
		Bindings_Node(in_id)
		{}
	
	// source has turned on or added bindings
	virtual bool positive_update(const Combining_Node * source_node);
	// source has turned off or removed bindings
	virtual bool negative_update(const Combining_Node * source_node);

private:
};

} // end namespace

#endif
