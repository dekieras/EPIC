#include "Predicates.h"
#include "Binding_set_list.h"
#include "Random_utilities.h"
#include "PPS_globals.h"
#include "Assert_throw.h"

#include <iostream>
#include <typeinfo>

using std::cout; using std::endl;

namespace Parsimonious_Production_System {


/*	No-argument Predicates */

// If_only_one_Predicate returns the first binding set, if it contains only one set;
// Otherwise, it returns an empty list
Binding_set_list_t If_only_one_predicate::apply(const Binding_set_list_t& input_bsl) const
{
	if (input_bsl.size() == 1) 
		return input_bsl;
	else
		return Binding_set_list_t();
}

// Use_only_one_Predicate returns only the first binding set, if any
Binding_set_list_t Use_only_one_predicate::apply(const Binding_set_list_t& input_bsl) const
{
	Binding_set_list_t result;
	if (input_bsl.size() > 0) 
		result.push_back(input_bsl.front());
	return result;
}

// Randomly_choose_one_Predicate returns one binding set chosen at random
Binding_set_list_t Randomly_choose_one_predicate::apply(const Binding_set_list_t& input_bsl) const
{
	Binding_set_list_t result;
	if (input_bsl.size() == 0) 
		return result;
	// choose which one
	// e.g. for three bs's, range will be 3
	// this creates a random integer in the range 0 ... size - 1, e.g. 0, 1, or 2
	// random_int is guaranteed not to return a value equal to the supplied range maximum
	int chosen = random_int(int(input_bsl.size()));
//	Assert(chosen < input_bsl.size());
	
	// because we are using lists, we have to walk down it to find the chosen one
	Binding_set_list_t::const_iterator it = input_bsl.begin();
	for (int i = 0; i < chosen; i++) {	// i goes 0, 1, 2
		it++;	// increment 0,1, or 2 times
		// we shouldn't be at or past the end
//		Assert(it != input_bsl.end());
		}

	// it will point to first(item 0), second(item 1), or third(item 2)
//	Assert (it != input_bsl.end());
	result.push_back(*it);
	return result;
}

/* Unary Predicates */
// Unary predicates take a single argument which can be either a variable or a constant.
// Some operation is applied which results in the input binding set being transformed
// for the output.

// At construction, we determine and store whether the argument is 
// a constant or a variable name whose value needs to looked up in the binding set. 
// A Template Method for apply is supplied here, made virtual so that it can be overridden.
// display the static contents of the predicate
void Unary_predicate::display_contents(int indent_level) const
{
	// output the required number of leading spaces - simple, brute force here
	for(int n = 0; n < indent_level*2; n++)
		PPS_out << ' ';
	PPS_out << typeid(*this).name() << ' ' << argument << endl;
}

Unary_predicate::Unary_predicate(const Symbol& in_argument) :
	argument(in_argument), arg_is_variable(is_variable(in_argument))
{
}

// this is the Template Method for apply - it calls derived class function for the actual comparison
// It assumes an operation comparing the argument to each binding in the binding_set
Binding_set_list_t Unary_predicate::apply(const Binding_set_list_t& input_bsl) const
{
	Binding_set_list_t result;
	for (Binding_set_list_t::const_iterator it = input_bsl.begin(); it != input_bsl.end(); it++) {
		if (compare(get_value(it)))
			result.push_back(*it);
		}
	return result;
}

// This returns each binding with a probability specified by the argument
bool Randomly_use_each_predicate::compare(const Symbol& v1) const
{
	// v1 is assumed to contain a number to be used as a probability
	return (unit_uniform_random_variable() <= v1.get_numeric_value());
}

// This retuns all of the bindings with a probability specified by the argument
Binding_set_list_t Randomly_true_predicate::apply(const Binding_set_list_t& input_bsl) const
{
	// the argument must be a constant number
	if(unit_uniform_random_variable() <= get_value().get_numeric_value())
		return input_bsl;
	else
		return Binding_set_list_t();	// return an empty binding set
}

//	TEH 7/17/07 DK 8/17/07
Binding_set_list_t Least_predicate::apply(const Binding_set_list_t& input_bsl) const
{
	Binding_set_list_t result;
	
	std::size_t n = input_bsl.size();		// can be O(n) in some implementations
	// If the binding set size is zero, return the empty list
//	if (input_bsl.size() == 0) 
	if (n == 0) 
		return result;
	// if the input list has size 1, return a copy
	if(n == 1)
		return input_bsl;

	// scan the input list looking for the binding set with the least value
	// avoid doing the look-up on the least item every time
	// get the name for this variable
	Symbol var_name = get_value();
	// Start with the first element as the least.
	Binding_set_list_t::const_iterator least = input_bsl.begin();
	// get the value stored in the least argument
	double min = (*least).get_value(var_name).get_numeric_value();
	Binding_set_list_t::const_iterator it = input_bsl.begin();
	++it;
	for(; it != input_bsl.end(); ++it)
		if ((*it).get_value(var_name) < min) {
			least = it;
			min = (*least).get_value(var_name).get_numeric_value();
			}
	
	// If there is more than one element, compare elements to find the least
/*	if(input_bsl.size() > 1)
	{
		Binding_set_list_t::const_iterator it = input_bsl.begin();
		it++;
		
		while(it != input_bsl.end())
		{
			//	A little explanation for this comparison:
			//	The innermost calls to get_value(), i.e. the get_value calls without arguments,
			//	return the name of the variable that this least comparison is using.
			//	The name of the variable is passed as an arg to the get_value call for the binding set.
			//	The binding set then returns the value of the variable being used in the least comparison.
			//	Finally, only if the new binding has a variable less than the lowest value so far, we have a new
			//	binding set that is the least. Note: This means that if there is a tie for the lowest
			//	variable, then the first in the list of bindings is chosen. Refer to the binding set
			//	code to determine how the binding sets are ordered.
			if ((*it).get_value(get_value()) < (*least).get_value(get_value()))
				least = it;
			
			it++;
		}
	}
*/	
	// Create a list with just the least value
	result.push_back(*least);
	
	return result;
}

// DEK 6/24/2011
Binding_set_list_t Greatest_predicate::apply(const Binding_set_list_t& input_bsl) const
{
	Binding_set_list_t result;
	
	std::size_t n = input_bsl.size();		// can be O(n) in some implementations
	// If the binding set size is zero, return the empty list
//	if (input_bsl.size() == 0) 
	if (n == 0) 
		return result;
	// if the input list has size 1, return a copy
	if(n == 1)
		return input_bsl;

	// scan the input list looking for the binding set with the greatest value
	// avoid doing the look-up on the greatest item every time
	// get the name for this variable
	Symbol var_name = get_value();
	// Start with the first element as the least.
	Binding_set_list_t::const_iterator greatest = input_bsl.begin();
	// get the value stored in the greatest argument
	double max = (*greatest).get_value(var_name).get_numeric_value();
	Binding_set_list_t::const_iterator it = input_bsl.begin();
	++it;
	for(; it != input_bsl.end(); ++it) {
		double it_value = (*it).get_value(var_name).get_numeric_value();
		if (it_value > max) {
			greatest = it;
			max = it_value;
			}
		}
	
	// If there is more than one element, compare elements to find the least
/*	if(input_bsl.size() > 1)
	{
		Binding_set_list_t::const_iterator it = input_bsl.begin();
		it++;
		
		while(it != input_bsl.end())
		{
			//	A little explanation for this comparison:
			//	The innermost calls to get_value(), i.e. the get_value calls without arguments,
			//	return the name of the variable that this least comparison is using.
			//	The name of the variable is passed as an arg to the get_value call for the binding set.
			//	The binding set then returns the value of the variable being used in the least comparison.
			//	Finally, only if the new binding has a variable less than the lowest value so far, we have a new
			//	binding set that is the least. Note: This means that if there is a tie for the lowest
			//	variable, then the first in the list of bindings is chosen. Refer to the binding set
			//	code to determine how the binding sets are ordered.
			if ((*it).get_value(get_value()) < (*least).get_value(get_value()))
				least = it;
			
			it++;
		}
	}
*/	
	// Create a list with just the greatest value
	result.push_back(*greatest);
	
	return result;
}

/* Binary Predicates */
// Binary predicates involve comparing the values of two arguments which can be either
// variables or constants.

// This class provide binary predicate functionality - at construction, we determine
// and store whether each argument is a constant or a variable name whose value needs to
// looked up in the binding  set. A Template Method for apply is supplied here,
// made virtual so that it can be overridden.

// display the static contents of the predicate
void Binary_predicate::display_contents(int indent_level) const
{
	// output the required number of leading spaces - simple, brute force here
	for(int n = 0; n < indent_level*2; n++)
		PPS_out << ' ';
	PPS_out << typeid(*this).name() << ' ' << argument1 << ' ' << argument2 << endl;
}

Binary_predicate::Binary_predicate(const Symbol& in_argument1, const Symbol& in_argument2) :
	argument1(in_argument1), is_variable1(is_variable(argument1)),
	argument2(in_argument2), is_variable2(is_variable(argument2))
{
}

// this is the Template Method for apply - it calls derived class function for the actual comparison
Binding_set_list_t Binary_predicate::apply(const Binding_set_list_t& input_bsl) const
{
	Binding_set_list_t result;
	for (Binding_set_list_t::const_iterator it = input_bsl.begin(); it != input_bsl.end(); it++) {
		if (compare(get_value1(it), get_value2(it)))
			result.push_back(*it);
		}
	return result;
}

/* Specific Predicate implementations */

bool Equal_predicate::compare(const Symbol& v1, const Symbol& v2) const
{
	return v1 == v2;
}

bool Different_predicate::compare(const Symbol& v1, const Symbol& v2) const
{
	return v1 != v2;
}

bool Greater_than_predicate::compare(const Symbol& v1, const Symbol& v2) const
{
	return v1 > v2;
}

bool Greater_than_or_equal_to_predicate::compare(const Symbol& v1, const Symbol& v2) const
{
	return v1 >= v2;
}

bool Less_than_predicate::compare(const Symbol& v1, const Symbol& v2) const
{
	return v1 < v2;
}

bool Less_than_or_equal_to_predicate::compare(const Symbol& v1, const Symbol& v2) const
{
	return v1 <= v2;
}


// Unique_predicate has a pair of variable names, and returns the binding sets such that
// the variables have unique values. This predicate can be used to avoid certain trivial
// multiple instantiations.
// For example, if the variables are ?x and ?y,
// and the binding sets are (?x a ?y b), (?x b ?y a), (?x c ?y d), (?x a ?y e)
// the results will be (?x a ?y b), (?x c ?y d), (?x a ?y e)
// The second set is not included because it duplicates the values of ?x and ?y.
// The fourth set is included because ?y has a different value than ?x.
// Note that the first set is always included.
// The two variables should have different names, and should both be in all binding sets.

Binding_set_list_t Unique_predicate::apply(const Binding_set_list_t& input_bsl) const
{
	Binding_set_list_t output_bsl;
	for (Binding_set_list_t::const_iterator iit = input_bsl.begin(); iit != input_bsl.end(); iit++) {
		bool unique = true;
		for (Binding_set_list_t::const_iterator oit = output_bsl.begin(); oit != output_bsl.end(); oit++) {
			//PPS_out << *iit << ' ' << variable1 << " compare to " << *oit << ' ' << variable2 << endl;
//			if ((*iit).get_value(variable1) == (*oit).get_value(variable2)) {
			if (compare(get_value1(iit), get_value2(oit))) {
				unique = false;
				break;
				}
			}
		if (unique)
			output_bsl.push_back(*iit);
		}
	return output_bsl;
}

bool Unique_predicate::compare(const Symbol& v1, const Symbol& v2) const
{
	return v1 == v2;
}




} // end namespace

