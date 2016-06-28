#ifndef PPS_RULE_COMPILER_H
#define PPS_RULE_COMPILER_H

#include "PPS_rule_compiler_interface.h"
#include "PPS_rule_compiler_classes.h"

#include <list>

namespace Parsimonious_Production_System {

// forward declaration 
class Production_System;

void build_discrimination_net(Production_System * production_system_ptr, Compiler_rule_data_list_t& rule_data_list);
void build_discrimination_net_from_patterns(Production_System * production_system_ptr,
	list_Symbol_list_t& patterns, Compiler_node_data_list_t& node_data_list);

void display_network_nodes(const Compiler_rule_data_list_t& rule_data_list);
void print_rule_data_list(const Compiler_rule_data_list_t& rule_data_list);
void replace_pair_in_list(Compiler_node_data_list_t& node_data_list, const Compiler_node_data_pair& org_pair, 
	const Compiler_node_data& replacement);

void replace_pair(Compiler_rule_data_list_t& rule_patterns, const Compiler_node_data_pair& max_pair, 
	const Compiler_node_data& replacement);
void make_neg_node_data_list(Compiler_rule_data_list_t& rule_data_list);
void replace_pos_neg_pair(Compiler_rule_data_list_t& rule_data_list, const Compiler_node_data_pair& max_pair, 
	const Compiler_node_data& replacement);
void combine_rules(Production_System * production_system_ptr, Compiler_rule_data_list_t& rule_data_list);
int get_two_predicate_args(const Symbol_list_t& predicate_pattern, Symbol& arg1, Symbol& arg2);

} // end namespace

#endif
