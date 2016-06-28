#ifndef PPS_RULE_COMPILER_CLASSES_H
#define PPS_RULE_COMPILER_CLASSES_H

#include "Variables.h"
#include "Combining_Nodes.h"
#include "Symbol_utilities.h"
//#include "Smart_Pointer.h"
#include "PPS_globals.h"

#include <iostream>
//#include <typeinfo>
//#include <string>
#include <map>


namespace Parsimonious_Production_System {
// These classes are internal to the compiler - should be in a namespace

struct Compiler_node_data {
//	Compiler_node_data() :
//		node_ptr(0), is_pattern(false), has_variables(false), var_count(0)
//		{}
	// a simple combining node
	Compiler_node_data(Combining_Node * in_node_ptr = 0) :
		node_ptr(in_node_ptr), is_pattern(false), has_variables(false), var_count(0)
		{}
	// a variable combining node
	Compiler_node_data(Combining_Node * in_node_ptr, const Variable_list in_varlist) :
		node_ptr(in_node_ptr), is_pattern(false), has_variables(true),
		var_count(int(in_varlist.size())),
		varlist(in_varlist)
		{}
	// a simple or wildcard-only pattern node
	Compiler_node_data(Combining_Node * in_node_ptr, const Symbol_list_t& in_pattern) :
		node_ptr(in_node_ptr), is_pattern(true), has_variables(false), var_count(0),
		pattern(in_pattern)
		{}
	// a variable pattern node
	Compiler_node_data(Combining_Node * in_node_ptr, const Symbol_list_t& in_pattern, 
		const Variable_list in_varlist) :
		node_ptr(in_node_ptr), is_pattern(true), has_variables(true),
		var_count(int(in_varlist.size())),
		varlist(in_varlist), pattern(in_pattern)
		{}
	
	Combining_Node * node_ptr;
	bool is_pattern;
	bool has_variables;
	int var_count;
	
	Variable_list varlist;
	Symbol_list_t pattern;
	
	void display() const;
	
	// if two nodes are both pattern nodes, compare the patterns
	// if they are both non-pattern nodes, compare the ids
	// if only one is a pattern, it is less than the other
	bool operator< (const Compiler_node_data& rhs) const
	{
		if (is_pattern) {
			if (rhs.is_pattern)
				return (pattern < rhs.pattern);
			else
				return true;
			}
		else {
			if (rhs.is_pattern)
				return false;
			else
				return (node_ptr->get_id() < rhs.node_ptr->get_id());
			}
	}
	
	// two node data items are the same if they point to the same node
	bool operator== (const Compiler_node_data& rhs) const
	{
		if (node_ptr == rhs.node_ptr)
			return true;
		return false;
	}
	
	bool operator!= (const Compiler_node_data& rhs) const
	{
		return !(*this == rhs);
	}
};

std::ostream& operator<< (std::ostream& os, const Compiler_node_data& datum);


typedef std::list<Compiler_node_data> Compiler_node_data_list_t;
typedef std::list<Compiler_node_data_list_t> list_Compiler_node_data_list_t;

// This holds the data for a production rule. This is basically a POD class; all members public
struct Compiler_rule_data {
	Compiler_rule_data()
		{}

	Symbol rule_name;
	Compiler_node_data_list_t pos_node_data_list;
	Compiler_node_data_list_t neg_node_data_list;		// used in combining pos & neg conditions
	list_Compiler_node_data_list_t neg_node_data_lists;	// used in combining within neg conditions
	list_Compiler_node_data_list_t neg_conjunction_node_data_lists;
	
	// these members hold the raw information from the parsed production rules	
	list_Symbol_list_t positive_patterns;
	list_list_Symbol_list_t negated_patterns; // a list of lists of Term lists, one for each not clause set
	list_list_Symbol_list_t negated_conjunction_patterns; // a list of lists of Term lists, one for each not clause set
	list_Symbol_list_t predicate_list;
	list_Symbol_list_t add_pattern_list;
	list_Symbol_list_t delete_pattern_list;
	list_Symbol_list_t command_action_list;
	list_Symbol_list_t add_command_action_list;  // for add commands that need access to external data
	list_Symbol_list_t add_command_pattern_list; // patterns that follow add command parameter
};

std::ostream& operator<< (std::ostream& os, const Compiler_rule_data& datum);

typedef std::list<Compiler_rule_data> Compiler_rule_data_list_t;


// A Compiler_node_data_pair holds a pair Compiler_node_data objects, and provides
// Comparisons between two pairs based on comparing their firsts and then their seconds
struct Compiler_node_data_pair {
	Compiler_node_data_pair()
		{}

	Compiler_node_data_pair(const Compiler_node_data& in_first, const Compiler_node_data& in_second) :
		first(in_first), second(in_second)
		{}

	Compiler_node_data_pair(const Compiler_node_data& in_first, const Compiler_node_data& in_second, const Variable_list& in_varlist) :
		first(in_first), second(in_second), varlist(in_varlist)
		{}

	void display() const;
	
	bool operator< (const Compiler_node_data_pair& rhs) const
	{
		if (first < rhs.first || (first == rhs.first && second < rhs.second))
			return true;
		return false;
	}
	
		
	bool operator== (const Compiler_node_data_pair& rhs) const
	{
		if (first == rhs.first && second == rhs.second)
			return true;
		return false;
	}
	
	bool operator!= (const Compiler_node_data_pair& rhs) const
	{
		return !(*this == rhs);
	}

	Compiler_node_data first;
	Compiler_node_data second;
	Variable_list varlist;

};

std::ostream& operator<< (std::ostream& os, const Compiler_node_data_pair& datum);

struct Counter {
	Counter() :
		count(0)
		{}
	void operator++ (int) 
		{count++;}
			
	int count;
	
};

typedef std::map<Compiler_node_data_pair, Counter, std::less<Compiler_node_data_pair> > Compiler_node_data_pair_counter_map_t;

} // end namespace

#endif

