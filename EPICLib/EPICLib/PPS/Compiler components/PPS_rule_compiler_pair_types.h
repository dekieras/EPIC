#ifndef RULE_COMPILER_PAIR_TYPES_H
#define RULE_COMPILER_PAIR_TYPES_H

#include "PPS_rule_compiler.h"
#include "Combining_Nodes.h"
#include "And_Nodes.h"
#include "Negation_Nodes.h"
#include "Overlap.h"
#include "PPS_globals.h"

#include <iostream>
//#include <string>
//#include <list>
#include <map>
#include <typeinfo>

namespace Parsimonious_Production_System {
using std::endl;

typedef std::map<Compiler_node_data_pair, Counter, std::less<Compiler_node_data_pair> > Compiler_node_data_pair_counter_map_t;

// a function-object like class that specifies how to recognize a pair and form a new node in its place
struct Simple_And_Node_pair
{
	bool make_valid_pair (const Compiler_node_data& first, const Compiler_node_data& second, Compiler_node_data_pair& new_pair)
	{
		if(!first.has_variables && !second.has_variables) {
			new_pair = Compiler_node_data_pair(first, second);
			return true;
			}
		return false;
	}

	Compiler_node_data make_new_node_data_for_pair(const Compiler_node_data_pair& node_data_pair)
	{		
		Combining_Node * new_node_ptr = new Simple_And_Node(
			Combining_Node::get_next_node_number(), 
			node_data_pair.first.node_ptr, node_data_pair.second.node_ptr);
				
		return Compiler_node_data(new_node_ptr);
	}
};


struct Full_Overlap_And_Node_pair
{

	bool make_valid_pair (const Compiler_node_data& first, const Compiler_node_data& second, Compiler_node_data_pair& new_pair)
	{
		if (first.has_variables && second.has_variables) {
			Full_Overlap overlap;
			Variable_list varlist = overlap(first.varlist,second.varlist);
			if (varlist.size() > 0) {
				new_pair = Compiler_node_data_pair(first, second, varlist);
				return true;
				}
			}
		return false;
	}

	Compiler_node_data make_new_node_data_for_pair(const Compiler_node_data_pair& node_data_pair)
	{		
		Combining_Node * new_node_ptr = new Full_Overlap_And_Node(
			Combining_Node::get_next_node_number(), 
			node_data_pair.first.node_ptr, node_data_pair.second.node_ptr);
				
		return Compiler_node_data(new_node_ptr, node_data_pair.varlist);
	}

};

struct Partial_Overlap_And_Node_pair
{
	bool make_valid_pair (const Compiler_node_data& first, const Compiler_node_data& second, Compiler_node_data_pair& new_pair)
	{
		if (first.has_variables && second.has_variables) {
			Partial_Overlap overlap;
			Variable_list varlist = overlap(first.varlist,second.varlist);
			if (varlist.size() > 0) {
				new_pair = Compiler_node_data_pair(first, second, varlist);
				return true;
				}
			}
		return false;
	}
	
	Compiler_node_data make_new_node_data_for_pair(const Compiler_node_data_pair& node_data_pair)
	{		
		Combining_Node * new_node_ptr = new Partial_Overlap_And_Node(
			Combining_Node::get_next_node_number(), 
			node_data_pair.first.node_ptr, node_data_pair.second.node_ptr);
				
		return Compiler_node_data(new_node_ptr, node_data_pair.varlist);
	}

};


struct No_Overlap_And_Node_pair
{

	bool make_valid_pair (const Compiler_node_data& first, const Compiler_node_data& second, Compiler_node_data_pair& new_pair)
	{
		if (first.has_variables && second.has_variables) {
			No_Overlap overlap;
			Variable_list varlist = overlap(first.varlist,second.varlist);
			if (varlist.size() > 0) {
				new_pair = Compiler_node_data_pair(first, second, varlist);
				return true;
				}
			}
		return false;
	}

	Compiler_node_data make_new_node_data_for_pair(const Compiler_node_data_pair& node_data_pair)
	{		
		Combining_Node * new_node_ptr = new No_Overlap_And_Node(
			Combining_Node::get_next_node_number(), 
			node_data_pair.first.node_ptr, node_data_pair.second.node_ptr);
				
		return Compiler_node_data(new_node_ptr, node_data_pair.varlist);
	}

};

struct Simple_Bindings_And_Node_pair
{
	bool make_valid_pair (const Compiler_node_data& first, const Compiler_node_data& second, Compiler_node_data_pair& new_pair)
	{
		if (!first.has_variables && second.has_variables) {
			Variable_list varlist = second.varlist;
			if (varlist.size() > 0) {
				new_pair = Compiler_node_data_pair(first, second, varlist);
				return true;
				}
			}
		return false;
	}

	Compiler_node_data make_new_node_data_for_pair(const Compiler_node_data_pair& node_data_pair)
	{		
		// first node is simple, second is bindings
		Combining_Node * new_node_ptr = new Simple_Bindings_And_Node(
			Combining_Node::get_next_node_number(), 
			node_data_pair.first.node_ptr, node_data_pair.second.node_ptr);
				
		return Compiler_node_data(new_node_ptr, node_data_pair.varlist);
	}

};

struct Bindings_Simple_And_Node_pair
{
	bool make_valid_pair (const Compiler_node_data& first, const Compiler_node_data& second, Compiler_node_data_pair& new_pair)
	{
		if (first.has_variables && !second.has_variables) {
			Variable_list varlist = first.varlist;
			if (varlist.size() > 0) {
				new_pair = Compiler_node_data_pair(first, second, varlist);
				return true;
				}
			}
		return false;
	}

	Compiler_node_data make_new_node_data_for_pair(const Compiler_node_data_pair& node_data_pair)
	{		
		// first node is bindings, second is simple
		Combining_Node * new_node_ptr = new Simple_Bindings_And_Node(
			Combining_Node::get_next_node_number(), 
			node_data_pair.second.node_ptr, node_data_pair.first.node_ptr);
				
		return Compiler_node_data(new_node_ptr, node_data_pair.varlist);
	}

};

struct Simple_Negation_Node_pair
{
	bool make_valid_pair (const Compiler_node_data& first, const Compiler_node_data& second, Compiler_node_data_pair& new_pair)
	{
		if(!first.has_variables && !second.has_variables) {
			new_pair = Compiler_node_data_pair(first, second);
			return true;
			}
		return false;
	}

	// first node is pos input, second is negation input
	Compiler_node_data make_new_node_data_for_pair(const Compiler_node_data_pair& node_data_pair)
	{		
		Combining_Node * new_node_ptr = new Simple_Negation_Node(
			Combining_Node::get_next_node_number(), 
			node_data_pair.first.node_ptr, node_data_pair.second.node_ptr);
				
		return Compiler_node_data(new_node_ptr);
	}
};

struct Simple_Bindings_Negation_Node_pair
{
	bool make_valid_pair (const Compiler_node_data& first, const Compiler_node_data& second, Compiler_node_data_pair& new_pair)
	{
		if(!first.has_variables && second.has_variables) {
			new_pair = Compiler_node_data_pair(first, second);
			return true;
			}
		return false;
	}

	// first node is pos input, second is negation input
	Compiler_node_data make_new_node_data_for_pair(const Compiler_node_data_pair& node_data_pair)
	{		
		Combining_Node * new_node_ptr = new Simple_Bindings_Negation_Node(
			Combining_Node::get_next_node_number(), 
			node_data_pair.first.node_ptr, node_data_pair.second.node_ptr);
		// no bindings propagated from the negated input		
		return Compiler_node_data(new_node_ptr);
	}
};

struct Bindings_Simple_Negation_Node_pair
{
	bool make_valid_pair (const Compiler_node_data& first, const Compiler_node_data& second, Compiler_node_data_pair& new_pair)
	{
		if (first.has_variables && !second.has_variables) {
			Variable_list varlist = first.varlist;
			if (varlist.size() > 0) {
				new_pair = Compiler_node_data_pair(first, second, varlist);
				return true;
				}
			}
		return false;
	}

	// first node is pos input, second is negation input
	Compiler_node_data make_new_node_data_for_pair(const Compiler_node_data_pair& node_data_pair)
	{		
		Combining_Node * new_node_ptr = new Bindings_Simple_Negation_Node(
			Combining_Node::get_next_node_number(), 
			node_data_pair.first.node_ptr, node_data_pair.second.node_ptr);
				
		return Compiler_node_data(new_node_ptr, node_data_pair.varlist);
	}
};

// negation node pair varlists are the first (positive) varlists only!
struct Full_Overlap_Negation_Node_pair
{
	bool make_valid_pair (const Compiler_node_data& first, const Compiler_node_data& second, Compiler_node_data_pair& new_pair)
	{
		if (first.has_variables && second.has_variables) {
			Full_Overlap overlap;
			Variable_list varlist = overlap(first.varlist, second.varlist);
			if (varlist.size() > 0) {
				new_pair = Compiler_node_data_pair(first, second, first.varlist);
				return true;
				}
			}
		return false;
	}

	// first node is pos input, second is negation input
	Compiler_node_data make_new_node_data_for_pair(const Compiler_node_data_pair& node_data_pair)
	{		
		Combining_Node * new_node_ptr = new Full_Overlap_Negation_Node(
			Combining_Node::get_next_node_number(), 
			node_data_pair.first.node_ptr, node_data_pair.second.node_ptr);
				
		return Compiler_node_data(new_node_ptr, node_data_pair.varlist);
	}
};

struct Partial_Overlap_Negation_Node_pair
{
	bool make_valid_pair (const Compiler_node_data& first, const Compiler_node_data& second, Compiler_node_data_pair& new_pair)
	{
		if (first.has_variables && second.has_variables) {
			Partial_Overlap overlap;
			Variable_list varlist = overlap(first.varlist, second.varlist);
			if (varlist.size() > 0) {
				new_pair = Compiler_node_data_pair(first, second, first.varlist);
				return true;
				}
			}
		return false;
	}

	// first node is pos input, second is negation input
	Compiler_node_data make_new_node_data_for_pair(const Compiler_node_data_pair& node_data_pair)
	{		
		Combining_Node * new_node_ptr = new Partial_Overlap_Negation_Node(
			Combining_Node::get_next_node_number(), 
			node_data_pair.first.node_ptr, node_data_pair.second.node_ptr);
				
		return Compiler_node_data(new_node_ptr, node_data_pair.varlist);
	}
};

struct No_Overlap_Negation_Node_pair
{
	bool make_valid_pair (const Compiler_node_data& first, const Compiler_node_data& second, Compiler_node_data_pair& new_pair)
	{
		if (first.has_variables && second.has_variables) {
			No_Overlap overlap;
			Variable_list varlist = overlap(first.varlist, second.varlist);
			if (varlist.size() > 0) {
				new_pair = Compiler_node_data_pair(first, second, first.varlist);
				return true;
				}
			}
		return false;
	}

	// first node is pos input, second is negation input
	Compiler_node_data make_new_node_data_for_pair(const Compiler_node_data_pair& node_data_pair)
	{		
		Combining_Node * new_node_ptr = new No_Overlap_Negation_Node(
			Combining_Node::get_next_node_number(), 
			node_data_pair.first.node_ptr, node_data_pair.second.node_ptr);
				
		return Compiler_node_data(new_node_ptr, node_data_pair.varlist);
	}
};




template <class PT>
void combine(PT pair_type, Compiler_rule_data_list_t& rule_data_list, bool output)
{
	int pass_counter = 0;
	int max_count = 0;
	if(output)
		PPS_out << "\n" << typeid(pair_type).name() << std::endl;
	while (true) {
		Compiler_node_data_pair max_pair;
		max_count = 0;
		
		// this version of find max pair finds simple pairs (no variables)
		find_most_frequent_pair(pair_type, rule_data_list, max_pair, max_count);
		
		// when max_count is 1, a shortcut would be possible because
		// all pairs left are unique. 
		
		// no more pairs exist, so compilation is finished
		if (max_count == 0)
			break;
		
		pass_counter++;
			
//		Combining_Node * new_node_ptr = pair_type.new_node_for_pair(
//			Combining_Node::get_next_node_number(), max_pair);

		Compiler_node_data new_node_data = pair_type.make_new_node_data_for_pair(max_pair);

		// make the downward connections
		max_pair.first.node_ptr->add_successor(new_node_data.node_ptr);
		max_pair.second.node_ptr->add_successor(new_node_data.node_ptr);

		if(output) {
			PPS_out << "pass: " << pass_counter << " replacing " << max_count << " pairs" << std::endl;
			max_pair.display();
			PPS_out << " with ";
			new_node_data.display();
			PPS_out << std::endl;
//		new_node_data.node_ptr->display_contents();
			}
			
		replace_pair(rule_data_list, max_pair, new_node_data);

		// output the list
//		print_data_list(rule_data_list);

		}
	// output the list
//	print_rule_data_list(rule_data_list);
}

// find the most frequent pair
template <class PT>
void find_most_frequent_pair(PT pair_type, const Compiler_rule_data_list_t& rule_data_list, Compiler_node_data_pair& max_pair, int& max_count)
{
	
	Compiler_node_data_pair_counter_map_t pair_counts;

	// generate the pairs in each rule condition and put into a sorted container
	for(Compiler_rule_data_list_t::const_iterator rule_it = rule_data_list.begin(); 
		rule_it != rule_data_list.end(); rule_it++) {
//		PPS_out << endl;
		// generate pairs for positive patterns
		generate_pairs(pair_type, (*rule_it).pos_node_data_list, pair_counts);
		// generate all the pairs for the negative patterns
		for(list_Compiler_node_data_list_t::const_iterator neg_it = (*rule_it).neg_node_data_lists.begin(); neg_it != (*rule_it).neg_node_data_lists.end(); neg_it++) {
				generate_pairs(pair_type, (*neg_it), pair_counts);
			}
		for(list_Compiler_node_data_list_t::const_iterator neg_it = (*rule_it).neg_conjunction_node_data_lists.begin(); neg_it != (*rule_it).neg_conjunction_node_data_lists.end(); neg_it++) {
				generate_pairs(pair_type, (*neg_it), pair_counts);
			}
		
		}

	// find the largest count pattern
	// output the counts
	
	for(Compiler_node_data_pair_counter_map_t::iterator it = pair_counts.begin(); it != pair_counts.end(); it++) {
//		 PPS_out << (*it).second.count << " ";
//		 (*it).first.display();
//		 PPS_out << endl;
		
		if ((*it).second.count > max_count) {
			max_count = (*it).second.count;
			max_pair = (*it).first;
			}
		}

//	if (max_count > 0) {
//		PPS_out << "Most frequent pair count: " << max_count << " pair is: " << endl;
//		max_pair.display();
//		PPS_out << endl;
//		}
	return;
}


template <class PT>
void generate_pairs(PT pair_type, const Compiler_node_data_list_t& node_data_list, Compiler_node_data_pair_counter_map_t& pair_counts)
{
	for (Compiler_node_data_list_t::const_iterator it2 = node_data_list.begin(); it2 != node_data_list.end(); it2++) {
		Compiler_node_data_list_t::const_iterator it3 = it2;
		it3++;
		Compiler_node_data_pair new_pair;
		
		for (; it3 != node_data_list.end(); it3++) {
			if (pair_type.make_valid_pair((*it2), (*it3), new_pair)) {
//				 new_pair.display();
//				 PPS_out << endl;
				
				pair_counts[new_pair]++;
				}

			}
		}
}

// for each combination of pos & neg nodes
template <class PT>
void combine_pos_neg(PT pair_type, Compiler_rule_data_list_t& rule_data_list, bool output)
{
	int pass_counter = 0;
	int max_count = 0;
	if(output)
		PPS_out << typeid(pair_type).name() << endl;
	while (true) {
		Compiler_node_data_pair max_pair;
		max_count = 0;
		
		// this version of find max pair finds simple pairs (no variables)
		find_most_frequent_pos_neg_pair(pair_type, rule_data_list, max_pair, max_count);
		
		// when max_count is 1, a shortcut would be possible because
		// all pairs left are unique. 
		
		// no more pairs exist, so compilation is finished
		if (max_count == 0)
			break;
		
		pass_counter++;
			
//		Combining_Node * new_node_ptr = pair_type.new_node_for_pair(
//			Combining_Node::get_next_node_number(), max_pair);

		Compiler_node_data new_node_data = pair_type.make_new_node_data_for_pair(max_pair);

		// make the downward connections
		max_pair.first.node_ptr->add_successor(new_node_data.node_ptr);
		max_pair.second.node_ptr->add_successor(new_node_data.node_ptr);

		if(output) {
			PPS_out << "pass: " << pass_counter << " replacing " << max_count << " pairs" << std::endl;
			max_pair.display();
			PPS_out << " with ";
			new_node_data.display();
			PPS_out << std::endl;
	//		new_node_data.node_ptr->display_contents();
			}
			
		replace_pos_neg_pair(rule_data_list, max_pair, new_node_data);

		// output the list
//		print_rule_data_list(rule_data_list);

		}
	// output the list
//	print_rule_data_list(rule_data_list);
}




// find the most frequent pair
template <class PT>
void find_most_frequent_pos_neg_pair(PT pair_type, const Compiler_rule_data_list_t& rule_data_list, Compiler_node_data_pair& max_pair, int& max_count)
{
	
	Compiler_node_data_pair_counter_map_t pair_counts;

	// generate the pairs in each rule condition and put into a sorted container
	for(Compiler_rule_data_list_t::const_iterator rule_it = rule_data_list.begin(); 
		rule_it != rule_data_list.end(); rule_it++) {
		generate_pos_neg_pairs(pair_type, (*rule_it).pos_node_data_list, (*rule_it).neg_node_data_list, pair_counts);		
		}

	// find the largest count pattern
	// output the counts
	
	for(Compiler_node_data_pair_counter_map_t::iterator it = pair_counts.begin(); it != pair_counts.end(); it++) {
//		 PPS_out << (*it).second.count << " ";
//		 (*it).first.display();
//		 PPS_out << endl;
		
		if ((*it).second.count > max_count) {
			max_count = (*it).second.count;
			max_pair = (*it).first;
			}
		}

//	if (max_count > 0) {
//		PPS_out << "Most frequent pos_neg_pair count: " << max_count << " pair is: " << endl;
//		max_pair.display();
//		PPS_out << endl;
//		}
	return;
}

// form pairs by taking the one item out of the positive list and combining it with each item in the negated list
template <class PT>
void generate_pos_neg_pairs(PT pair_type, const Compiler_node_data_list_t& pos_node_data_list, const Compiler_node_data_list_t& neg_node_data_list, Compiler_node_data_pair_counter_map_t& pair_counts)
{
	for (Compiler_node_data_list_t::const_iterator it2 = pos_node_data_list.begin(); it2 != pos_node_data_list.end(); it2++) {

		Compiler_node_data_pair new_pair;
		for (Compiler_node_data_list_t::const_iterator it3 = neg_node_data_list.begin(); it3 != neg_node_data_list.end(); it3++) {
			if (pair_type.make_valid_pair((*it2), (*it3), new_pair)) {
				 // new_pair.display();
				 // PPS_out << endl;
				
				pair_counts[new_pair]++;
				}

			}
		}
}

} // end namespace

#endif

