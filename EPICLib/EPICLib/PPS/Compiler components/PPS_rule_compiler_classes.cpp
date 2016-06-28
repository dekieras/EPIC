#include "PPS_rule_compiler_classes.h"

using namespace std;

namespace Parsimonious_Production_System {

void Compiler_node_data::display() const
{
	if(PPS_out)
		PPS_out << *this;
}


ostream& operator<< (ostream& os, const Compiler_node_data& datum)
{
	os << "[Node " << datum.node_ptr->get_id() << ' ';
	if (datum.is_pattern) {
		os << datum.pattern;
		}
	os << ' ' << datum.var_count;
	if (datum.has_variables) {
		os << ' ' << datum.varlist;
		}
	os << "]";
	
	return os;
}





void Compiler_node_data_pair::display() const
{
	if(!PPS_out)
		PPS_out << *this;
}

ostream& operator<< (ostream& os, const Compiler_node_data_pair& datum)
{
	os << "<";
	os << datum.first;
	os << " - ";
	os << datum.second;
	os << ">";
	
	return os;
}


ostream& operator<< (ostream& os, const Compiler_rule_data& datum)
{
	os << endl;
	os << datum.rule_name << endl;
	if (datum.positive_patterns.size() > 0) {
		os << "Positive patterns:" << endl;
		os << datum.positive_patterns << endl;
		}
	if (datum.pos_node_data_list.size() > 0) {
		os << "Positive pattern nodes:" << endl;
		for (Compiler_node_data_list_t::const_iterator it2 = datum.pos_node_data_list.begin(); it2 != datum.pos_node_data_list.end(); it2++) {
			os << (*it2) << endl;
			}
		}
	if (datum.negated_patterns.size() > 0) {
		os << "Negated patterns:" << endl;
		os << datum.negated_patterns << endl;
		}
	if (datum.neg_node_data_lists.size() > 0) {
		os << "Negated pattern nodes:" << endl;
		for(list_Compiler_node_data_list_t::const_iterator neg_it = datum.neg_node_data_lists.begin(); neg_it != datum.neg_node_data_lists.end(); neg_it++) {
			os << "(";
			for (Compiler_node_data_list_t::const_iterator it3 = (*neg_it).begin(); it3 != (*neg_it).end(); it3++) {
				os << (*it3) << ' ';
				}
			os << ")" << endl;
			}
		}
	if (datum.negated_conjunction_patterns.size() > 0) {
		os << "Negated conjunction patterns:" << endl;
		os << datum.negated_conjunction_patterns << endl;
		}
	if (datum.neg_conjunction_node_data_lists.size() > 0) {
		os << "Negated conjunction pattern nodes:" << endl;
		for(list_Compiler_node_data_list_t::const_iterator neg_it = datum.neg_conjunction_node_data_lists.begin(); neg_it != datum.neg_conjunction_node_data_lists.end(); neg_it++) {
			os << "(";
			for (Compiler_node_data_list_t::const_iterator it3 = (*neg_it).begin(); it3 != (*neg_it).end(); it3++) {
				os << (*it3) << ' ';
				}
			os << ")" << endl;
			}
		}
	if(datum.neg_node_data_list.size() > 0) {
		os << "Negated nodes:" << endl;
		for (Compiler_node_data_list_t::const_iterator it4 = datum.neg_node_data_list.begin(); it4 != datum.neg_node_data_list.end(); it4++) {
			os << (*it4) << endl;
			}
		}
	if (datum.predicate_list.size() > 0) {
		os << "Predicate patterns:" << endl;
		os << datum.predicate_list << endl;
		}
	if(datum.add_pattern_list.size() > 0) {
		os << "Add action patterns:" << endl;
		os << datum.add_pattern_list << endl;
		}
	if(datum.delete_pattern_list.size() > 0) {
		os << "Delete action patterns:" << endl;
		for (list_Symbol_list_t::const_iterator it6 = datum.delete_pattern_list.begin(); it6 != datum.delete_pattern_list.end(); it6++) {
			os << (*it6) << endl;
			}
		}
	if(datum.command_action_list.size() > 0) {
		os << "Motor actions:" << endl;
		for (list_Symbol_list_t::const_iterator it7 = datum.command_action_list.begin(); it7 != datum.command_action_list.end(); it7++) {
			os << (*it7) << endl;
			}
		}
	return os;
}



} // end namespace

