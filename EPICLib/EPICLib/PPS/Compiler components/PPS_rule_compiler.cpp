#include "PPS_rule_compiler_interface.h"
#include "PPS_rule_compiler.h"
#include "PPS_rule_compiler_classes.h"
#include "PPS_rule_compiler_pair_types.h"
#include "Production_System.h"
//#include "Command_Action.h"
#include "Variables.h"
#include "Clause.h"
#include "Discrimination_Nodes.h"
#include "Pattern_Nodes.h"
#include "Combining_Nodes.h"
#include "And_Nodes.h"
#include "Rule_Nodes.h"
#include "Predicate.h"
#include "PPS_globals.h"
#include "Assert_throw.h"

#include <iostream>
#include <fstream>
//#include <string>
//#include <sstream>
//#include <map>
//#include <typeinfo>
#include <algorithm>

using std::endl;
using std::ifstream;
using std::find;

namespace Parsimonious_Production_System {

// local function prototypes
Predicate_ptr_list_t build_predicate_ptr_list(const list_Symbol_list_t& predicate_patterns);

void compile_rules(Production_System * production_system_ptr, Compiler_rule_data_list_t& rule_data_list)
{
	bool output = production_system_ptr->get_output_compiler_details();
		
	// output the list
	if(output)
		print_rule_data_list(rule_data_list);

	build_discrimination_net(production_system_ptr, rule_data_list);	

	//print_rule_data_list(rule_data_list);
	
	// simple and nodes should come before variable and nodes	
	combine(Simple_And_Node_pair(), rule_data_list, output);
	combine(Full_Overlap_And_Node_pair(), rule_data_list, output);
	combine(Partial_Overlap_And_Node_pair(), rule_data_list, output);
	combine(No_Overlap_And_Node_pair(), rule_data_list, output);
	combine(Simple_Bindings_And_Node_pair(), rule_data_list, output);
	combine(Bindings_Simple_And_Node_pair(), rule_data_list, output);

	// try the policy of combining all positive nodes before doing any negation nodes
	
	// make ready to combine positive and negated nodes
	make_neg_node_data_list(rule_data_list);

	// output the list
	// print_rule_data_list(rule_data_list);

	// compute negations with bindings before those with simple
	combine_pos_neg(Full_Overlap_Negation_Node_pair(), rule_data_list, output);
	combine_pos_neg(Partial_Overlap_Negation_Node_pair(), rule_data_list, output);
	combine_pos_neg(No_Overlap_Negation_Node_pair(), rule_data_list, output);
	combine_pos_neg(Simple_Negation_Node_pair(), rule_data_list, output);
	combine_pos_neg(Simple_Bindings_Negation_Node_pair(), rule_data_list, output);
	combine_pos_neg(Bindings_Simple_Negation_Node_pair(), rule_data_list, output);

//	PPS_out << "Hit any key" << endl;
//	cin.get();
	// output the list
//	print_rule_data_list(rule_data_list);

	combine_rules(production_system_ptr, rule_data_list);

	// output the list
//	PPS_out << "\n\nFINAL Rule_Node_data_list" << endl;
//	print_rule_data_list(rule_data_list);
//	PPS_out << "Hit any key" << endl;
//	cin.get();
	if(production_system_ptr->get_output_compiler_details() && PPS_out) {
		PPS_out << "\n\nFinal network nodes" << endl;	
		display_network_nodes(rule_data_list);
		}
	
}


// The parser puts the raw information directly in the Compiler_rule_data items, one for each rule

// build the discrimination net and fill the list of Compiler_node_data items, one list for each production rule
void build_discrimination_net(Production_System * production_system_ptr, Compiler_rule_data_list_t& rule_data_list)
{	
	for(Compiler_rule_data_list_t::iterator rule_it = rule_data_list.begin(); rule_it != rule_data_list.end(); rule_it++) {

		// build for positive clauses
		build_discrimination_net_from_patterns(production_system_ptr, (*rule_it).positive_patterns, 
			(*rule_it).pos_node_data_list);
			
		// build patterns for each negated clause as well
		list_list_Symbol_list_t& neg_pattern_list = (*rule_it).negated_patterns;
//		PPS_out << "in build_discrimination_net doing negated clauses" << endl;
//		PPS_out << neg_pattern_list << endl;
		for(list_list_Symbol_list_t::iterator neg_it = neg_pattern_list.begin(); neg_it != neg_pattern_list.end(); neg_it++) {
			//Compiler_node_data_list_t neg_node_data_list;
			build_discrimination_net_from_patterns(production_system_ptr, (*neg_it), (*rule_it).neg_node_data_list);
			}
			
		// build patterns for each negated conjunction clause as well
		list_list_Symbol_list_t& neg_conj_pattern_list = (*rule_it).negated_conjunction_patterns;
//		PPS_out << "in build_discrimination_net doing negated conjunction clauses" << endl;
//		PPS_out << neg_pattern_list << endl;
		for(list_list_Symbol_list_t::iterator neg_it = neg_conj_pattern_list.begin(); neg_it != neg_conj_pattern_list.end(); neg_it++) {
			Compiler_node_data_list_t neg_node_data_list;
			build_discrimination_net_from_patterns(production_system_ptr, (*neg_it), neg_node_data_list);
			(*rule_it).neg_conjunction_node_data_lists.push_back(neg_node_data_list);
			}
			
		}
}

void build_discrimination_net_from_patterns(Production_System * production_system_ptr,
	list_Symbol_list_t& patterns, Compiler_node_data_list_t& node_data_list)
{		
		for (list_Symbol_list_t::const_iterator patt_it = patterns.begin(); patt_it != patterns.end(); patt_it++) {
//			PPS_out << "in build_discrimination_net_from_patterns - doing pattern:" << endl;
//			PPS_out << (*patt_it) << endl;

			// build the discrimination net and find or create the pattern node
			Clause clause(*patt_it);
			Combining_Node * pattern_ptr = production_system_ptr->get_root_node_ptr()->start_build_pattern(clause);
//			pattern_ptr->display_contents();
			production_system_ptr->add_pattern_node(pattern_ptr);
//			root_node_ptr->display_contents();

//			create the Compiler_node_data item with the right values
			Variable_list varlist = pattern_ptr->get_variables();
			if (varlist.size() > 0) {
				Compiler_node_data new_Node_data(pattern_ptr, *patt_it, varlist);
//				new_Node_data.display(); PPS_out << endl;
				node_data_list.push_back(new_Node_data);
				}
			else {
				node_data_list.push_back(Compiler_node_data(pattern_ptr, *patt_it));
				}
			}
		node_data_list.sort();
		
		return;
}



// combine_rules checks to be sure that each rule has exactly one node data item in its positive pattern list
// it constructs the rule node appropriate for the type, and replaces the node data item with one indicating
// the rule node.
void combine_rules(Production_System * production_system_ptr, Compiler_rule_data_list_t& rule_data_list)
{

	Predicate_ptr_list_t dummy_predicate_list; // dummy list
	
	for(Compiler_rule_data_list_t::iterator rule_it = rule_data_list.begin(); 
		rule_it != rule_data_list.end(); rule_it++) {
		// check that lists are done
		Assert((*rule_it).pos_node_data_list.size() == 1);
		Assert((*rule_it).neg_node_data_list.size() == 0);
		Compiler_node_data& node_datum = (*rule_it).pos_node_data_list.front();
		Predicate_ptr_list_t predicate_list = build_predicate_ptr_list((*rule_it).predicate_list);
		Combining_Node * new_node_ptr = Rule_Node::create(production_system_ptr, (*rule_it).rule_name, node_datum.node_ptr,
			node_datum.varlist, predicate_list, (*rule_it).add_pattern_list, (*rule_it).delete_pattern_list,
			(*rule_it).command_action_list,(*rule_it).add_command_action_list,(*rule_it).add_command_pattern_list);
		// make the downward connections
		node_datum.node_ptr->add_successor(new_node_ptr);
//		new_node_ptr->display_contents();
		node_datum.node_ptr = new_node_ptr;
		// add to the rule node list - cross cast first
		Rule_Node * new_rule_node_ptr = dynamic_cast<Rule_Node *>(new_node_ptr);
		// only a programming error could happen here
		Assert(new_rule_node_ptr);
		production_system_ptr->add_rule_node(new_rule_node_ptr);
		}
}

// Build and return a list of pointers to Predicate objects by building the type of object designated by the
// predicate name.
Predicate_ptr_list_t build_predicate_ptr_list(const list_Symbol_list_t& predicate_patterns)
{
	Predicate_ptr_list_t result;
	
	for(list_Symbol_list_t::const_iterator pred_it = predicate_patterns.begin(); pred_it != predicate_patterns.end(); pred_it++) {
		const Symbol_list_t& pattern = *pred_it;
		Predicate * ptr;
		ptr = Predicate::create(pattern);			
		result.push_back(ptr);
		}
		
	return result;
}


void replace_pair(Compiler_rule_data_list_t& rule_data_list, const Compiler_node_data_pair& max_pair, const Compiler_node_data& replacement)
{
	// remove the max_pair terms from the patterns and replace with a dummy pattern 
	for(Compiler_rule_data_list_t::iterator rule_it = rule_data_list.begin(); rule_it != rule_data_list.end(); rule_it++) {
		// replace in positive patterns
		replace_pair_in_list((*rule_it).pos_node_data_list, max_pair, replacement);
		
		// replace in negated patterns
		for(list_Compiler_node_data_list_t::iterator neg_it = (*rule_it).neg_node_data_lists.begin(); neg_it != (*rule_it).neg_node_data_lists.end(); neg_it++) {
			replace_pair_in_list((*neg_it), max_pair, replacement);
			}
		for(list_Compiler_node_data_list_t::iterator neg_it = (*rule_it).neg_conjunction_node_data_lists.begin(); neg_it != (*rule_it).neg_conjunction_node_data_lists.end(); neg_it++) {
			replace_pair_in_list((*neg_it), max_pair, replacement);
			}
		}
}

void replace_pair_in_list(Compiler_node_data_list_t& node_data_list, const Compiler_node_data_pair& org_pair, const Compiler_node_data& replacement)
{
		Compiler_node_data_list_t::const_iterator found_it1 = find(node_data_list.begin(), node_data_list.end(), org_pair.first);
		// might be a faster way to do this
		if (found_it1 == node_data_list.end()) 
			return;
		Compiler_node_data_list_t::const_iterator found_it2 = find(node_data_list.begin(), node_data_list.end(), org_pair.second);
		if (found_it2 == node_data_list.end()) 
			return;
		// remove them
		node_data_list.remove(org_pair.first);
		node_data_list.remove(org_pair.second);
		// add a replacement pattern at the end
		node_data_list.push_back(replacement);
}

// for each rule, collect the contents of the negated node data lists, and add them to the single list
void make_neg_node_data_list(Compiler_rule_data_list_t& rule_data_list)
{
	for(Compiler_rule_data_list_t::iterator rule_it = rule_data_list.begin(); 
		rule_it != rule_data_list.end(); rule_it++) {
		
		for(list_Compiler_node_data_list_t::const_iterator neg_it = (*rule_it).neg_node_data_lists.begin();
				neg_it != (*rule_it).neg_node_data_lists.end(); neg_it++)
			for (Compiler_node_data_list_t::const_iterator it2 = (*neg_it).begin(); it2 != (*neg_it).end(); it2++) 
				(*rule_it).neg_node_data_list.push_back(*it2);
				
		for(list_Compiler_node_data_list_t::const_iterator neg_it = (*rule_it).neg_conjunction_node_data_lists.begin();
				neg_it != (*rule_it).neg_conjunction_node_data_lists.end(); neg_it++)
			for (Compiler_node_data_list_t::const_iterator it2 = (*neg_it).begin(); it2 != (*neg_it).end(); it2++) 
				(*rule_it).neg_node_data_list.push_back(*it2);
				
		// empty the list of negated node data items
		(*rule_it).neg_node_data_lists.clear();
		(*rule_it).neg_conjunction_node_data_lists.clear();
		(*rule_it).neg_node_data_list.sort();	// put the negated nodes in order by id number	
		}	
}

// replacing a pos_neg_pair means that the new negation node replaces the positive item, while the negated node is removed.

void replace_pos_neg_pair(Compiler_rule_data_list_t& rule_data_list, const Compiler_node_data_pair& org_pair, const Compiler_node_data& replacement)
{
	// remove the max_pair terms from the patterns and replace with a dummy pattern 
	for(Compiler_rule_data_list_t::iterator rule_it = rule_data_list.begin(); rule_it != rule_data_list.end(); rule_it++) {
		// replace in positive patterns, remove from negated patterns
		Compiler_node_data_list_t::const_iterator found_it1 = find((*rule_it).pos_node_data_list.begin(), (*rule_it).pos_node_data_list.end(), org_pair.first);
		// might be a faster way to do this
		if (found_it1 == (*rule_it).pos_node_data_list.end()) 
			continue;
		Compiler_node_data_list_t::const_iterator found_it2 = find((*rule_it).neg_node_data_list.begin(), (*rule_it).neg_node_data_list.end(), org_pair.second);
		if (found_it2 == (*rule_it).neg_node_data_list.end()) 
			continue;
		// remove them
		(*rule_it).pos_node_data_list.remove(org_pair.first);
		(*rule_it).neg_node_data_list.remove(org_pair.second);
		// add a replacement pattern at the end of the positive list
		(*rule_it).pos_node_data_list.push_back(replacement);
		
		}
}

void print_rule_data_list(const Compiler_rule_data_list_t& rule_data_list)
{
	if(!PPS_out)
		return;

	for(Compiler_rule_data_list_t::const_iterator rule_it = rule_data_list.begin(); rule_it != rule_data_list.end(); rule_it++) {
		PPS_out << (*rule_it) << endl;
		}
}


void display_network_nodes(const Compiler_rule_data_list_t& rule_data_list)
{
	if(!PPS_out)
		return;

	for(Compiler_rule_data_list_t::const_iterator rule_it = rule_data_list.begin(); rule_it != rule_data_list.end(); rule_it++) {
		PPS_out << endl;
		PPS_out << (*rule_it).rule_name << endl;
		for (Compiler_node_data_list_t::const_iterator it2 = (*rule_it).pos_node_data_list.begin(); it2 != (*rule_it).pos_node_data_list.end(); it2++) {
			(*it2).node_ptr->display_contents();
			PPS_out << endl;
			}
		for(list_Compiler_node_data_list_t::const_iterator neg_it = (*rule_it).neg_node_data_lists.begin(); neg_it != (*rule_it).neg_node_data_lists.end(); neg_it++) {
			for (Compiler_node_data_list_t::const_iterator it3 = (*neg_it).begin(); it3 != (*neg_it).end(); it3++) {
				(*it3).node_ptr->display_contents();
				PPS_out << endl;
				}
			PPS_out << endl;
			}
		}

}

} // end namespace
