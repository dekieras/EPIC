#ifndef DISCRIMINATION_NODES_H
#define DISCRIMINATION_NODES_H

/*
A clause is simply a list of terms, and the current term is pointed out by an iterator

A root node marks the top node in the discrimination net

A constant term node tests for the current term being a specified value

A variable term node assigns a binding pair for specified variable and value
the variable may be either a normal variable or a wildcard

function add/remove_clause is given the current term of a clause and calls itself to
discriminate each remaining term. If the clause is exhausted, we should be at a clause
node pointing to a clause node, if not, nothing is done. 

If variables or wildcards were accumulated, add/remove_binding_set is called 
for the clause node. If not, then add/remove_constant_clause is called instead.

The discrimination function for the current term and discrimination node operates on 
the current term. If it matches (or is a variable/wildcard node), then the term pointer
is incremented and a recursive call is made to each successor.

*/

#include "Variables.h"
#include "Binding_pair.h"
#include "Clause.h"
#include "Pattern_Nodes.h"
#include "Combining_Nodes.h"

#include <list>
#include <set>
#include <string>
//#include <iostream>

namespace Parsimonious_Production_System {

class Discrimination_Node;
typedef std::list<Discrimination_Node *> Discrimination_Node_ptr_list_t;
typedef std::set<Discrimination_Node *> Discrimination_Node_ptr_set_t;


//forward declaration
class Combining_Node;

class Discrimination_Node {
public:
	Discrimination_Node() :
		term(Symbol()), pattern_node_ptr(0)
		{}
	Discrimination_Node(const Symbol& in_term) : 
		term(in_term), pattern_node_ptr(0)
		{}

	virtual ~Discrimination_Node() {}
		
	void add_successor(Discrimination_Node * p)
		{successors.push_back(p);}

	void set_pattern_node_ptr(Combining_Node * p)
		{pattern_node_ptr = p;}

	const Symbol& get_term() const
		{return term;}
	const Discrimination_Node_ptr_list_t& get_successors() const
		{return successors;}

	Combining_Node * build_pattern(const Clause& pattern, Variable_list& var_names, 
		bool wildcard_present, int wildcard_number);
		
//	Combining_Node * find_pattern(const Clause& pattern, Variable_list& var_names, 
//		bool wildcard_present, int wildcard_number)
//		{return build_or_find_pattern (pattern, var_names, wildcard_present, wildcard_number, false);}
		
//	Discrimination_Node *  test_term(const Symbol& in_term) const;

	virtual bool add_clause(const Clause& clause, const Binding_set& binding_set) const;
	virtual bool remove_clause(const Clause& clause, const Binding_set& binding_set) const;

	// add this node and its descendants to the set of nodes
	void accumulate_nodes(Discrimination_Node_ptr_set_t& nodes);

	static Discrimination_Node * create(const Symbol& current_term);
	void display_contents(int indent_level) const;

	static bool debug_flag;

protected:
	const Symbol term;
	
	Discrimination_Node_ptr_list_t successors;
	Combining_Node * pattern_node_ptr;	// pointer to clause node

//	Combining_Node * build_or_find_pattern(const Clause& pattern, Variable_list& var_names, 
//		bool wildcard_present, int wildcard_number, bool build_flag);

};

class Root_Node : public Discrimination_Node {
public:
	Root_Node(const std::string& in_net_name) : 
		net_name(in_net_name)
		{}
	
	const std::string& get_net_name() const
		{return net_name;}
		
//	void add_successor(Discrimination_Node * p)
//		{successors.push_back(p);}

	Combining_Node * start_build_pattern(const Clause& pattern);
//	Combining_Node * start_find_pattern(const Clause& pattern);

	bool start_add_clause(const Clause& clause) const;
	bool start_remove_clause(const Clause& clause) const;
	
	void display_contents() const;
	
private:
	const std::string net_name;
//	Discrimination_Node_ptr_list_t successors;
	Root_Node();
};




class Constant_Discrimination_Node : public Discrimination_Node {
public:
	Constant_Discrimination_Node(const Symbol& in_term) : 
		Discrimination_Node(in_term)
		{}
		
	virtual bool add_clause(const Clause& clause, const Binding_set& binding_set) const;
	virtual bool remove_clause(const Clause& clause, const Binding_set& binding_set) const;

private:	
	Constant_Discrimination_Node();
};

class Variable_Discrimination_Node : public Discrimination_Node {
public:
	Variable_Discrimination_Node(const Symbol& in_var_name) : 
		Discrimination_Node(in_var_name)
		{}
//	~Variable_Discrimination_Node()
//		{cout << "Variable_Discrimination_Node " << term << " dtor " << endl;}
	virtual bool add_clause(const Clause& clause, const Binding_set& binding_set) const;
	virtual bool remove_clause(const Clause& clause, const Binding_set& binding_set) const;

private:	
	Variable_Discrimination_Node();
};

} // end namespace


#endif

