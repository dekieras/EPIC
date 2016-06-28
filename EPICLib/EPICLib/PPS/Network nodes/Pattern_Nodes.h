#ifndef CLAUSE_NODES_H
#define CLAUSE_NODES_H

//#include <iostream>
//#include <string>
//#include <list>
//#include "Assert_throw.h"

#include "Variables.h"
#include "Binding_set.h"
#include "Binding_set_list.h"
#include "Combining_Nodes.h"
#include "Clause.h"

namespace Parsimonious_Production_System {

// ** Pattern Nodes ***
// Pattern nodes represent a condition pattern in a production rule

// In general, a pattern node is asked to add either a variable binding set, or
// or a wildcard binding set, or both

// This is a mixin class only

class Pattern_Node {
public:
	
	Pattern_Node(const Symbol_list_t& in_pattern) :
		pattern(in_pattern), varlist(Variable_list())
		{}

	Pattern_Node(const Symbol_list_t& in_pattern, const Variable_list& in_varlist) :
		pattern(in_pattern), varlist(in_varlist)
		{}
	
	virtual ~Pattern_Node() {}
	
	static bool debug_flag;
	 // var_names needs to be put into sorted order!
	 // if var_names empty, no variables;
	 // if wildcard_number is zero, no wildcards
	static Combining_Node * create(const Clause& pattern, const Variable_list& var_names, bool wildcard_present);
	
protected:
	const Symbol_list_t pattern;	// the original pattern expressed as a list of terms
	const Variable_list varlist;
			
};

// A Simple_Pattern_Node stores state for a single clause 
//in a rule condition containing only constants
class Simple_Pattern_Node : public Combining_Node, public Pattern_Node {
public:
	Simple_Pattern_Node(int in_id, const Symbol_list_t& in_pattern) :
		Combining_Node(in_id), Pattern_Node(in_pattern)
		{}

//	~Simple_Pattern_Node()
//		{cout << "Simple_Pattern_Node " << get_id() << " dtor" << endl;}

	// Add a clause by turning on
	virtual bool add_clause(const Binding_set& bs);
	// Remove a clause by turning off
	virtual bool remove_clause(const Binding_set& bs);

	// display the contents of the node
	virtual void display_contents(const char * label = "", bool dynamic = false, int indent_level = 0) const;

	// access pattern and variable list data (implemented by Pattern_Nodes)
	const Symbol_list_t& get_pattern() const
		{return pattern;}
	const Variable_list& get_variables() const	// return empty variable list
		{return varlist;}
	
private:
	Simple_Pattern_Node();
};

// A Bindings_Pattern_Node stores state for a single pattern
// in a rule condition containing variables
class Bindings_Pattern_Node : public Bindings_Node, public Pattern_Node {
public:
	Bindings_Pattern_Node(int in_id, const Symbol_list_t& in_pattern, const Variable_list& in_varlist) :
		Bindings_Node(in_id), Pattern_Node(in_pattern, in_varlist)
		{}
	Bindings_Pattern_Node(int in_id, const Symbol_list_t& in_pattern) :
		Bindings_Node(in_id), Pattern_Node(in_pattern)
		{}
	
//	~Bindings_Pattern_Node()
//		{cout << "Bindings_Pattern_Node " << get_id() << " dtor" << endl;}

	// Add a clause by adding a binding set
	virtual bool add_clause(const Binding_set& bs);
	// Remove a clause by removing a binding set
	virtual bool remove_clause(const Binding_set& bs);
	// display the contents of the node
	virtual void display_contents(const char * label = "", bool dynamic = false, int indent_level = 0) const;

	// access pattern and variable list data (implemented by Pattern_Nodes)
	const Symbol_list_t& get_pattern() const
		{return pattern;}
	const Variable_list& get_variables() const	// return empty variable list
		{return varlist;}

private:
	Bindings_Pattern_Node();
};


// A Wildcards_Pattern_Node stores state for a single pattern
// in a rule condition containing only wildcards
// it behaves like a Bindings_Pattern_Node but propagates no bindings
class Wildcard_Pattern_Node : public Bindings_Pattern_Node {
public:
	Wildcard_Pattern_Node(int in_id, const Symbol_list_t& in_pattern) :
		Bindings_Pattern_Node(in_id, in_pattern)
		{}
	Wildcard_Pattern_Node(int in_id, const Symbol_list_t& in_pattern, const Variable_list& in_varlist) :
		Bindings_Pattern_Node(in_id, in_pattern, in_varlist)
		{}
	
	virtual void reset(); 
	virtual void display_state(int indent_level = 0) const;	// display state and bindings
	// Add a clause by adding a binding set
	virtual bool add_clause(const Binding_set& bs);
	// Remove a clause by removing a binding set
	virtual bool remove_clause(const Binding_set& bs);
	// display the contents of the node
	virtual void display_contents(const char * label = "", bool dynamic = false, int indent_level = 0) const;
	
protected:
	Binding_set_list wildcard_binding_sets;	// current wildcard bindings for this node

private:
	Wildcard_Pattern_Node();
};

// A Bindings_Wildcard_Pattern_Node stores state for a single pattern
// in a rule condition containing both variables and wildcards
// it behaves like a Bindings_Pattern_Node but propagates only variable bindings
class Bindings_Wildcard_Pattern_Node : public Wildcard_Pattern_Node {
public:
	Bindings_Wildcard_Pattern_Node(int in_id, const Symbol_list_t& in_pattern, const Variable_list& in_varlist) :
		Wildcard_Pattern_Node(in_id, in_pattern, in_varlist)
		{}
	
	// Add a clause by adding a binding set
	virtual bool add_clause(const Binding_set& bs);
	// Remove a clause by removing a binding set
	virtual bool remove_clause(const Binding_set& bs);
	// display the static contents of the node
	// display the contents of the node
	virtual void display_contents(const char * label = "", bool dynamic = false, int indent_level = 0) const;
	
private:
	Bindings_Wildcard_Pattern_Node();
};

} // end namespace

#endif
