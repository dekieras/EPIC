#ifndef PRODUCTION_SYSTEM_H
#define PRODUCTION_SYSTEM_H

#include "Clause.h"
//#include "Discrimination_Nodes.h"
//#include "Combining_Nodes.h"
//#include "Pattern_Nodes.h"
//#include "Rule_Nodes.h"
#include "Geometry.h"
#include "Parameter.h"

#include <list>
#include <map>
#include <set>
#include <string>

class Command_interface;

namespace Parsimonious_Production_System {
// forward declarations
class Root_Node;
class Discrimination_Node;
class Combining_Node;
class Pattern_Node;
class Rule_Node;

class Production_System {
public:
	Production_System() :
		output_compiler_messages(false), output_compiler_details(false),
		output_run_messages(false), output_run_details(false),
		output_run_memory_contents(false), break_enabled(false), fault_enabled(false),
		cycle_counter(0), break_on_this_cycle(false), updates_done(0), root_node_ptr(0)
		{}
	
	~Production_System();
	
	long get_cycle_counter() const
		{return cycle_counter;}
	
	// get/set output flags
	bool get_output_compiler_messages() const
		{return output_compiler_messages;}
	void set_output_compiler_messages(bool flag)
		{output_compiler_messages = flag;}

	bool get_output_compiler_details() const
		{return output_compiler_details;}
	void set_output_compiler_details(bool flag)
		{output_compiler_details = flag;}

	bool get_output_run_messages() const
		{return output_run_messages;}
	void set_output_run_messages(bool flag)
		{output_run_messages = flag;}

	bool get_output_run_details() const
		{return output_run_details;}
	void set_output_run_details(bool flag)
		{output_run_details = flag;}

	bool get_output_run_memory_contents() const
		{return output_run_memory_contents;}
	void set_output_run_memory_contents(bool flag)
		{output_run_memory_contents = flag;}

	
	
	
	void clear();			// remove all network and data
	bool compile_file(std::string filename);	// build a production system from the contents of a file
	void display_static_contents() const;	// output the static contents of the production system
	void display_dynamic_contents() const;	// output the dynamic contents of the production system
	
	void reset();			// remove data, leave network ready to run
	void initialize();		// load initial state
	void run(Command_interface& command_interface);		// run cycles until no more rules fire
	bool run_cycle(long time, Command_interface&);		// run a single cycle, return true if a rule fired, false if not
	
	// update_add_clause and update_delete_clause do not cause rules to be executed, 
	// only to be added or removed from the fired rules list

	// update the production system memory by adding a clause
	void update_add_clause(const Clause& in_clause);
	
	// update the production system memory by deleting a clause
	void update_delete_clause(const Clause& in_clause);
	
		// add a clause to the add clause update list
	void add_to_add_clause_list(const Clause& in_clause)
		{add_clause_list.push_back(in_clause);}
		
	// add a clause to the delete clause update list
	void add_to_delete_clause_list(const Clause& in_clause)
		{delete_clause_list.push_back(in_clause);}
	
	// add a clause to the initial state clause list
	void add_initial_clause(const Clause& in_clause)
		{initial_memory_state.push_back(in_clause);}

	// incremental_compile_file();
	// add_rule();
	// execute_until_quiesence();
	
	Root_Node * get_root_node_ptr()
		{return root_node_ptr;}
		
	Geometry_Utilities::Point get_named_location(const Symbol& name, bool& found) const;
	// get the parameter list extracted by the compiler from the input file
	const Parameter_specification_list_t& get_parameter_specifications() const
		{return parameters;}

	void add_pattern_node(Combining_Node * p)
		{pattern_nodes.push_back(p);}

	void add_rule_node(Rule_Node * p)
		{rule_nodes.push_back(p);}
	
	// add the node to the fired rules list - rule node does not add itself more than once
	void add_to_fired_rules(Rule_Node * p)
		{fired_rules.push_back(p);}
		
	// remove the node from the fired rules list - shouldn't be more than one
	void remove_from_fired_rules(Rule_Node * p)
		{fired_rules.remove(p);}

	// call this function to see if inputs or updates present that might cause a firing
	bool memory_updated() const
		{return (updates_done || !add_clause_list.empty() || !delete_clause_list.empty());}
	void display_memory_contents() const;

	// Functions to control and access faulted and break fields in rule nodes
	const Symbol_list_t& get_rule_names() const
		{return rule_names;}
	bool is_rule_name_valid(const Symbol& name) const;

	bool get_break_enabled() const
		{return break_enabled;}
	void set_break_enabled(bool state)
		{break_enabled = state;}
	void set_rule_break_state(const Symbol& name, bool state);
	bool get_rule_break_state(const Symbol& name) const;
	const Symbol_list_t& get_break_rule_names() const
		{return break_rule_names;}
	bool get_fault_enabled() const
		{return fault_enabled;}
	void set_fault_enabled(bool state)
		{fault_enabled = state;}
	void set_rule_fault_state(const Symbol& name, bool state);
	bool get_rule_fault_state(const Symbol& name) const;
	bool get_break_on_this_cycle() const {return break_on_this_cycle;}
	const Symbol_list_t& get_fault_rule_names() const
		{return fault_rule_names;}

private:
	bool output_compiler_messages;
	bool output_compiler_details;
	bool output_run_messages;
	bool output_run_details;
	bool output_run_memory_contents;
	bool break_enabled;
	bool fault_enabled;

	long cycle_counter;
	bool break_on_this_cycle;					// true if a break-rule is executed
	std::size_t updates_done;					// number of changes to data
	Root_Node * root_node_ptr;					// root node for the discrimination net
	std::list<Combining_Node *> pattern_nodes;	// list of pattern nodes
	std::list<Rule_Node *> rule_nodes;			// list of rule nodes

	std::list<Rule_Node *> fired_rules;			// list of fired rule nodes
	Clause_list_t initial_memory_state;			// clauses in memory on first cycle
	Clause_list_t add_clause_list;				// clauses to be added on next cycle
	Clause_list_t delete_clause_list;			// clauses to be deleted on next cycle
	std::map<Symbol, Geometry_Utilities::Point> named_locations;
	Parameter_specification_list_t parameters;
	
	Symbol_list_t display_tags;					// list of tags to be displayed
	bool maintain_memory_contents;				// true if memory_contents maintained
	mutable std::map<Symbol, std::set<Clause> > memory_contents;	// clauses grouped by tags
	
	Symbol_list_t rule_names;					// list of all rule names
	Symbol_list_t break_rule_names;				// list of rule names set with break set on
	Symbol_list_t fault_rule_names;				// list of rule names set with fault set on
	
	// return a pointer to the rule node with the supplied name
	Rule_Node * get_rule_node(const Symbol& name, bool throw_not_there = true) const;
};



} // end namespace

#endif
