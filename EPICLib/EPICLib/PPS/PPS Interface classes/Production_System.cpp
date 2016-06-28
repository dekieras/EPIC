#include "Production_System.h"
#include "PPS_exception.h"
#include "PPS_file_parser.h"
#include "PPS_rule_compiler_classes.h"
#include "PPS_rule_compiler_interface.h"
#include "Clause.h"
#include "Discrimination_Nodes.h"
#include "Combining_Nodes.h"
#include "Pattern_Nodes.h"
#include "Rule_Nodes.h"
#include "PPS_globals.h"
#include "Assert_throw.h"

#include <iostream>
#include <fstream>
#include <list>
#include <string>
//#include <sstream>
#include <map>
#include <set>
#include <ctime>
#include <cstdlib>
#include <algorithm>

using std::cout; using std::endl; using std::cerr;
using std::ifstream;
using std::list;
using std::string;
using std::set;
using std::map;
using std::clock_t; using std::clock;
using std::exit;

namespace Parsimonious_Production_System {


Production_System::~Production_System()
{
	// clean out the network
	clear();
}


void Production_System::clear()
{
	/* DEBUGGING */
//	output_run_details = true;

	if(output_run_details && PPS_out) 
		PPS_out << "*** Clearing PPS network" << endl;
		
	// reset other variables and containers
	initial_memory_state.clear();
	named_locations.clear();
	parameters.clear();
	// empty the memory contents container
	memory_contents.clear();
	// get rid of the rule names - note: might want to remember those previous set
	rule_names.clear();
	break_rule_names.clear();
	fault_rule_names.clear();
	
	// collect pointers to all of the nodes and then delete them. Use a set for speed
	// It makes sense to collect these now because throwing away the whole net is
	// a relatively uncommon operation - so there is little value in 
	// constructing a list of the nodes while they are created just for this purpose.
	
	// don't clear if no network
	if(!root_node_ptr) {
		if(output_run_details && PPS_out) 
			PPS_out << "No network present to clear" << endl;
		return;
		}

	if(output_run_details && PPS_out) {
		PPS_out << pattern_nodes.size() << " Pattern Nodes present" << endl;
		PPS_out << rule_nodes.size() << " Rule Nodes present" << endl;
		PPS_out << Combining_Node::get_number_of_nodes() << " Combining Nodes created" << endl;
		}

	// collect all of the combining nodes by starting with the pattern nodes
	// using a set here insures uniqueness by node id
	Combining_Node_ptr_set_t combining_nodes;

	// this will start with pattern nodes and include them in the accumulated set, all combining nodes, and all rule nodes
	for(Combining_Node_ptr_list_t::const_iterator it = pattern_nodes.begin(); it != pattern_nodes.end(); it++) {
		(*it)->accumulate_nodes(combining_nodes);
		}
	if(output_run_details && PPS_out) {
		PPS_out << combining_nodes.size() << " Combining Nodes deallocated" << endl;
		}
	for(Combining_Node_ptr_set_t::iterator it = combining_nodes.begin(); it != combining_nodes.end(); it++) {
//		cout << "deleting node " << (*it)->get_id() << endl;
		// here we will delete all pattern nodes and other combining nodes
		delete (*it);
		}
	combining_nodes.clear();

	// reset the list of pattern nodes kept in the production system
	pattern_nodes.clear();
	// reset the list of rules nodes kept in the production system
	rule_nodes.clear();
	// reset the number of nodes counter to zero - used for id's
	Combining_Node::reset_next_node_number();

	// using a set here insures uniqueness by node id
	Discrimination_Node_ptr_set_t discrimination_nodes;

	// collect all of the discrimination nodes by starting with root node, which is a discrimination node
	// the pattern nodes at the base of the descrimination net are combining nodes and are not included
	root_node_ptr->accumulate_nodes(discrimination_nodes);
	if(output_run_details && PPS_out) {
		PPS_out << discrimination_nodes.size() << " Discrimination Nodes deallocated" << endl;
		}
	for(Discrimination_Node_ptr_set_t::iterator it = discrimination_nodes.begin(); it != discrimination_nodes.end(); it++) {
//		cout << "deleting node " << (*it)->get_term() << endl;
		// here we delete root and all other discrimination nodes
		delete (*it);
		}
	discrimination_nodes.clear();

	// network nodes should now be all gone
	root_node_ptr = 0;
	
	/* DEBUGGING */
	//output_run_details = false;
	
// if using Symbol, may need to clear its set of strings
}

void Production_System::initialize()
{
	Assert(root_node_ptr);
	cycle_counter = 0;	// number of cycles completed starts at zero
	// put in initial memory contents - copy them into the add list!
	add_clause_list = initial_memory_state;
	PPS_out << add_clause_list.size() << " items added from initial " << initial_memory_state.size() << endl;
	
	// initialize the display tags
	display_tags.clear();
	display_tags.push_back(Symbol("Goal"));
	display_tags.push_back(Symbol("Strategy"));
	display_tags.push_back(Symbol("Step"));
	display_tags.push_back(Symbol("Status"));
	display_tags.push_back(Symbol("Tag"));
	display_tags.push_back(Symbol("Visual"));
	display_tags.push_back(Symbol("Auditory"));
	display_tags.push_back(Symbol("WM"));
	display_tags.push_back(Symbol("Motor"));
	display_tags.push_back(Symbol("Temporal"));
	PPS_out << display_tags.size() << " display tags to be listed " << endl;

	maintain_memory_contents = true;

/*
for(Clause_list_t::const_iterator init_it = initial_memory_state.begin(); init_it != initial_memory_state.end(); init_it++) {
		if (!root_node_ptr->start_add_clause(*init_it))
			cout << "No match: " << (*init_it) << endl;
		}
*/

}

// remove data, leave network ready to run	
void Production_System::reset()
{
	Assert(root_node_ptr);
	if(output_run_details && PPS_out) {
		PPS_out << "Removing data from network" << endl;
		}
	for(Combining_Node_ptr_list_t::const_iterator it = pattern_nodes.begin(); it != pattern_nodes.end(); it++) {
		(*it)->reset();
		}
	// empty the fired rules list
	fired_rules.clear();
	// empty the input lists
	add_clause_list.clear();
	delete_clause_list.clear();
	updates_done = 0;	// no updates waiting
	// empty the memory contents container used for displaying the current memory state
	memory_contents.clear();
}



// build a production system from the contents of a file
// return true if was successful, false otherwise
bool Production_System::compile_file(string filename)
{
	// remove any old network and reset other variables built from the file
	clear();
	ifstream inp(filename.c_str());
/*
	if(!inp) {
		cerr << "Couldn't open file \"" << filename << '\"' << endl;
		return false;
		}
*/		
	if(!inp)
		throw PPS_exception(string("Couldn't open file \"") + filename + string("\" for input"));
		
	//Compiler_rule_data production;
	
	Compiler_rule_data_list_t production_list;
//initial_memory_state
//	named_locations;
//	named_movements;
	
	PPS_file_parser parser(inp);	// create a new parser object each time!
	
	
	clock_t start, finish;  // declare two variable to hold clock time data
	start = clock();  // save the starting time
	
	// parse the supplied file
	bool rule_set_is_good = parser.parse_file(production_list, initial_memory_state, named_locations, parameters, output_compiler_messages);
	
	// build the list of productions supplied by the parser
	// if we found a parsing error, do not try to build the network
	if(!rule_set_is_good) {
		if(PPS_out)
			PPS_out << "An error was found in parsing production rule file; rules not ready to execute" << endl;
		return false;
		}
	// create the network
	root_node_ptr = new Root_Node(filename.c_str());	// same as production system file name
		
	compile_rules(this, production_list);
	
	// build the list of rule names
	for(Rule_Node_ptr_list_t::const_iterator it = rule_nodes.begin(); it != rule_nodes.end(); ++it) {
		rule_names.push_back((*it)->get_name());
		}

	finish = clock();
	if(output_compiler_details && PPS_out) {
		PPS_out << "Compile time is " << ((double) (finish - start)) / CLOCKS_PER_SEC << endl;
		}
	
	return true;
}
	

// output the static contents of the production system network
void Production_System::display_static_contents() const
{
	if(!(output_compiler_details && PPS_out) )
		return;
	Assert(root_node_ptr);
	if(PPS_out) {
		PPS_out << "\n\nFinal Network:" << endl;	
		PPS_out << "There are " << rule_nodes.size() << " rule nodes" << endl;
		}
	root_node_ptr->display_contents();
	
	for(Rule_Node_ptr_list_t::const_iterator it = rule_nodes.begin(); it != rule_nodes.end(); it++) {
		if(Combining_Node * p = dynamic_cast<Combining_Node *>(*it))
			p->display_contents("");
		}
}

// output the dynamic contents of the production system network
void Production_System::display_dynamic_contents() const
{
	if(!(output_run_details && PPS_out) )
		return;
	for(Rule_Node_ptr_list_t::const_iterator it = rule_nodes.begin(); it != rule_nodes.end(); it++) {
		if(Combining_Node * p = dynamic_cast<Combining_Node *>(*it))
			p->display_contents("", true);
		}
}

// run until no rules fire
void Production_System::run(Command_interface& command_interface)
{
	clock_t start, finish;  // declare two variable to hold clock time data
	Assert(root_node_ptr);
	if(output_run_messages && PPS_out) {
		PPS_out << "\n\nExecution Started" << endl;
		}
	start = clock();  // save the starting time
	reset();
	initialize();
/*
	// initialize the display tags
	display_tags.clear();
	display_tags.push_back(Symbol("GOAL"));
	display_tags.push_back(Symbol("STEP"));
	display_tags.push_back(Symbol("TAG"));
	display_tags.push_back(Symbol("VISUAL"));
	display_tags.push_back(Symbol("WM"));
	maintain_memory_contents = true;
*/
	while (run_cycle(0, command_interface)) {}
	finish = clock();
	if(output_run_messages && PPS_out) {
		PPS_out << "No more rules fired" << endl;
		PPS_out << "Compute time is " << ((double) (finish - start)) / CLOCKS_PER_SEC << endl;
		}
}

// update_add_clause and update_delete_clause do not cause rules to be executed, 
// only to be added or removed from the fired rules list

// update the production system memory by adding a clause
// we add it to the memory collection only if there is a match - avoids confusions
// above is wrong - we should see it in the output even if it doesn't match - 
// helps debugging of e.g. perceptual system - shows state even if irrelevant for rule matching
void Production_System::update_add_clause(const Clause& in_clause)
{
	if(output_run_details && PPS_out) {
		PPS_out << "+++ Adding: " << in_clause << endl;
		}
	if (!root_node_ptr->start_add_clause(in_clause)) {
		if(output_run_details && PPS_out) {
			PPS_out << "No add match: " << in_clause << endl;
			}
		}
/*
	else {
		if(maintain_memory_contents)
			memory_contents[in_clause.get_tag()].insert(in_clause);
		}	
*/
	if(maintain_memory_contents)
		memory_contents[in_clause.get_tag()].insert(in_clause);
		
	updates_done++;
}

// update the production system memory by deleting a clause
void Production_System::update_delete_clause(const Clause& in_clause)
{
	if(output_run_details && PPS_out) {
		PPS_out << "--- Deleting: " << in_clause << endl;
		}
	if (!root_node_ptr->start_remove_clause(in_clause)) {
		if(output_run_details && PPS_out) {
			PPS_out << "No delete match: " << in_clause << endl;
			}
		}
/*
	else {
		if(maintain_memory_contents)
			memory_contents[in_clause.get_tag()].erase(in_clause);
	}
	updates_done++;
*/
	if(maintain_memory_contents)
		memory_contents[in_clause.get_tag()].erase(in_clause);
}

void Production_System::display_memory_contents() const
{
	if(!PPS_out)
		return;
	for (Symbol_list_t::const_iterator tag_it = display_tags.begin(); tag_it != display_tags.end(); tag_it++) {
		// print tag on a line if there will be contents for it
		if(memory_contents[*tag_it].begin() != memory_contents[*tag_it].end())
			PPS_out << *tag_it << endl;
//		const set<Clause> & tag_set = memory_contents[*tag_it];
//		for (set<Clause>::const_iterator clause_it = tag_set.begin(); clause_it != tag_set.end(); clause_it++) {
		for (set<Clause>::const_iterator clause_it = memory_contents[*tag_it].begin(); 
			 clause_it != memory_contents[*tag_it].end(); clause_it++) {
			PPS_out << "  " << *clause_it << endl;
			}
        if (memory_contents.empty()) {
            PPS_out << endl;
        }
	}
}

// run a single cycle, return true if a rule fired, false if not
// The counter shows cycles completed; 0 during first cycle, 1 afterwards. If accessed before, shows one less than if accessed after
bool Production_System::run_cycle(long time, Command_interface& command_interface)
{
	Assert(root_node_ptr);
	break_on_this_cycle = false;
	updates_done += delete_clause_list.size();
	updates_done += add_clause_list.size();

	if(output_run_messages && PPS_out) {
		PPS_out << time << ":Cycle " << cycle_counter <<": ";
		}
	// even if the database did not change, a rule could still fire, but we will output if database changed	
//	Discrimination_Node::debug_flag = true;
	if (updates_done && output_run_messages && output_run_details && PPS_out)
		PPS_out << updates_done << " update clauses: ";
	// update using each delete clause, then empty the list
	for (Clause_list_t::iterator del_it = delete_clause_list.begin(); del_it != delete_clause_list.end(); del_it++) {
		update_delete_clause(*del_it);
		}
	delete_clause_list.clear();
	// update using each add clause, then empty the list
	for (Clause_list_t::iterator add_it = add_clause_list.begin(); add_it != add_clause_list.end(); add_it++) {
		update_add_clause(*add_it);
		}
	add_clause_list.clear();

	// show updated contents of memory only if updates done or rules fired
	if(output_run_memory_contents && maintain_memory_contents && (updates_done || fired_rules.size())) {
		if(PPS_out) {
			PPS_out << endl;
			}
		display_memory_contents();
		}
		
	updates_done = 0;
	// examine each rule node
//	cout << "Rule states:" << endl;
//	for (Combining_Node_ptr_list_t::iterator rule_it = rule_nodes.begin(); rule_it != rule_nodes.end(); rule_it++) {
//		(*rule_it)->display_state();
//		}

	// return false if no rules are fired
	if (fired_rules.size() == 0) {
		if(output_run_messages && PPS_out) {
			PPS_out << "No rules fired" << endl;
			}
		cycle_counter++;	// counter shows cycles completed; 0 during first cycle, 1 afterwards
		return false;
		}
	
// display the state of the network!
	if(output_run_messages && output_run_details && PPS_out) {
		PPS_out << "*** Network state follows ***" << endl;
		display_dynamic_contents();	
		}
	// execute the fired rules
	if(output_run_messages && PPS_out) {
		PPS_out << "*** Rules fired:" << endl;
		}
	for (Rule_Node_ptr_list_t::iterator fired_it = fired_rules.begin(); fired_it != fired_rules.end(); fired_it++) {
		Rule_Node * fired_rule_ptr = *fired_it;
		fired_rule_ptr->execute(command_interface);
		if(break_enabled && fired_rule_ptr->get_break_state()) {
			PPS_out << "Break on this cycle from rule: " << fired_rule_ptr->get_name() << endl;
			break_on_this_cycle = true;
			}
		if(fault_enabled && fired_rule_ptr->get_fault_state()) {
			PPS_out << "Fault on this cycle from rule: " << fired_rule_ptr->get_name() << endl;
			PPS_out << "Nothing happens at this time" << endl;
			}
		}

	cycle_counter++;	// counter shows cycles completed; 0 during first cycle, 1 afterwards
	return true;
}


Geometry_Utilities::Point Production_System::get_named_location(const Symbol& name, bool& found) const
{
	map<Symbol, Geometry_Utilities::Point>::const_iterator it = named_locations.find(name);
	if(it == named_locations.end())
		found = false;
//		throw PPS_exception(string("Unknown named location:") + name.str());
	else
		found = true;
	return it->second;
}

// A function object predicate class to compare rule names
struct Rule_name_match {
	Rule_name_match(const Symbol & probe_) : probe(probe_) {}
	bool operator() (const Rule_Node * p) {
		return (p->get_name() == probe);
		}
	const Symbol probe;
};

// return a pointer to a rule node given its name; result is zero if name unknown
Rule_Node * Production_System::get_rule_node(const Symbol& name, bool throw_not_there) const
{
	Rule_name_match pred(name);
	Rule_Node_ptr_list_t::const_iterator it = 
		find_if(rule_nodes.begin(), rule_nodes.end(), pred);
	// check to see if found
	if(it == rule_nodes.end()) {
		if(throw_not_there)
			throw PPS_exception(string("Attempt to access unknown rule: ") + name.str());
		else
			return 0;
		}
	return *it;
}

bool Production_System::is_rule_name_valid(const Symbol& name) const
{
	return get_rule_node(name, false);
}

bool Production_System::get_rule_break_state(const Symbol& name) const
{
	return get_rule_node(name)->get_break_state();
}

// Functions to access faulted and break fields in rule nodes; throws if not there
void Production_System::set_rule_break_state(const Symbol& name, bool new_state)
{
	Rule_Node * rule_ptr = get_rule_node(name);
	bool old_state = rule_ptr->get_break_state();
	rule_ptr->set_break_state(new_state);
	// if the old state is true and the new state is false, take it out of the list
	if(old_state && !new_state) 
		break_rule_names.remove(name);		
	// if the old state is false, and the new state is true, add it to the list
	else if(!old_state && new_state) 
		break_rule_names.push_back(name);
	// else do nothing
}

bool Production_System::get_rule_fault_state(const Symbol& name) const
{
	return get_rule_node(name)->get_fault_state();
}

void Production_System::set_rule_fault_state(const Symbol& name, bool new_state)
{
	Rule_Node * rule_ptr = get_rule_node(name);
	bool old_state = rule_ptr->get_fault_state();
	rule_ptr->set_fault_state(new_state);
	// if the old state is true and the new state is false, take it out of the list
	if(old_state && !new_state) 
		fault_rule_names.remove(name);		
	// if the old state is false, and the new state is true, add it to the list
	else if(!old_state && new_state) 
		fault_rule_names.push_back(name);
	// else do nothing
}



} // end namespace
