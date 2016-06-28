#include "Predicate.h"
#include "Predicates.h"
#include "Binding_set_list.h"
#include "Numeric_utilities.h"
#include "PPS_globals.h"
#include "Assert_throw.h"

#include <iostream>
#include <typeinfo>

using std::cout; using std::endl;

namespace Parsimonious_Production_System {

// predicate pattern  contains a list of Symbols; the first is the name of the predicate;
// the remainder are the arguments. The parser has checked for the number and type of the arguments.
Predicate * Predicate::create(const Symbol_list_t& predicate_pattern)
{	
	int pattern_size = int(predicate_pattern.size());
	Assert(pattern_size >= 1);
	const Symbol& pred_name = predicate_pattern.front();
	
	switch(pattern_size) {
	
	case 1: {	// no-argument predicates first
	if (pred_name == "If_only_one") {
		return new If_only_one_predicate;
		}
	else if (pred_name == "Use_only_one") {
		return new Use_only_one_predicate;
		}
	else if (pred_name == "Randomly_choose_one") {
		return new Randomly_choose_one_predicate;
		}
	else {
		Assert(!"unknown predicate name shouldn't happen here");
		}
	break;
	}

	case 2: {	// Unary predicates
	Symbol_list_t::const_iterator term_it = predicate_pattern.begin();
	term_it++;	// skip predicate name;
	const Symbol& arg = *term_it;
	
	if (pred_name == "Randomly_use_each") {
		return new Randomly_use_each_predicate(arg);
		}
	else if (pred_name == "Randomly_true") {
		return new Randomly_true_predicate(arg);
		}
	// TEH 7/17/07
	else if (pred_name == "Least") {
		return new Least_predicate(arg);
		}
	// DEK 6/24/2011
	else if (pred_name == "Greatest") {
		return new Greatest_predicate(arg);
		}
	else {
		Assert(!"unknown predicate name shouldn't happen here");
		}
	break;
	}
	
	case 3: {	// Binary predicates
	Symbol_list_t::const_iterator term_it = predicate_pattern.begin();
	term_it++;	// skip predicate name;
	const Symbol& arg1 = *term_it;
	term_it++;
	const Symbol& arg2 = *term_it;
	
	if (pred_name == "Unique") {
		return new Unique_predicate(arg1, arg2);
		}
	else if (pred_name == "Equal") {
		return new Equal_predicate(arg1, arg2);
		}
	else if (pred_name == "Different") {
		return new Different_predicate(arg1, arg2);
		}
	else if (pred_name == "Greater_than") {
		return new Greater_than_predicate(arg1, arg2);
		}
	else if (pred_name == "Greater_than_or_equal_to") {
		return new Greater_than_or_equal_to_predicate(arg1, arg2);
		}
	else if (pred_name == "Less_than") {
		return new Less_than_predicate(arg1, arg2);
		}
	else if (pred_name == "Less_than_or_equal_to") {
		return new Less_than_or_equal_to_predicate(arg1, arg2);
		}
	else {
		Assert(!"unknown predicate shouldn't happen here");
		}
	break;
	}

	default: {
		Assert(!"unknown predicate pattern shouldn't happen here");
		break;
		}
	}
	
	return 0;
}

// display the static contents of the predicate
void Predicate::display_contents(int indent_level) const
{
	// output the required number of leading spaces - simple, brute force here
	for(int n = 0; n < indent_level*2; n++)
		PPS_out << ' ';
	PPS_out << typeid(*this).name() << endl;
}

} // end namespace
