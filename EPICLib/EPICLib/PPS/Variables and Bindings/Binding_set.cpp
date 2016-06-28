#include "Binding_pair.h"
#include "Binding_set.h"

#include <iostream>
#include <algorithm>
//#include <string>
//#include <list>
//#include "Assert_throw.h"

using std::ostream; using std::cerr; using std::endl;
using std::unique; using std::find_if;
using std::less; using std::bind1st;

namespace Parsimonious_Production_System {

// two binding sets are equal if their binding pairs are equal - same number, contents, order
bool Binding_set::operator== (const Binding_set& rhs) const
{
	if (binding_pair_list.size() != rhs.binding_pair_list.size())
		return false;
	Binding_pair_list_t::const_iterator lhs_it, rhs_it;
	for(lhs_it = binding_pair_list.begin(), rhs_it = rhs.binding_pair_list.begin();
		lhs_it != binding_pair_list.end() && rhs_it != rhs.binding_pair_list.end();
		lhs_it++, rhs_it++) 
		if (*lhs_it != *rhs_it)
			return false;
	// lists had same length, so if here, result is true
	return true;
}

bool Binding_set::operator!= (const Binding_set& rhs) const
{
	return !(operator== (rhs));
}
	
// add a binding pair to a binding set - must keep sorted
void Binding_set::add_binding_pair(const Binding_pair& bp)
{
/*	binding_pair_list.push_back(bp);
	binding_pair_list.sort();	// gotta be a better way!
*/
// linear search and insert seems to benchmark very slightly slower - are the lists relatively small?
	Binding_pair_list_t::iterator it = 
		find_if(binding_pair_list.begin(), binding_pair_list.end(), bind1st(less<Binding_pair>(), bp));
		binding_pair_list.insert(it, bp);
	
}


// this function checks to see if a binding set is legal
// if non empty, each varname must be in order and appear only once

bool Binding_set::is_legal() const
{
	Symbol_list_t name_list;
	Symbol_list_t::const_iterator n_it;
	
	// collect each var_name
	Binding_pair_list_t::const_iterator it;
	for(it = binding_pair_list.begin(); it != binding_pair_list.end(); it++)
		name_list.push_back(it->get_var_name());
	
	// sort the list
	name_list.sort();

	// eliminate any duplicate names
	Symbol_list_t::iterator n_end = unique(name_list.begin(), name_list.end());

	// if lists are different length, something was wrong, return false
	// also, go through the namelist and the binding_pair_list checking:
	// varnames should match
	for(it = binding_pair_list.begin(), n_it = name_list.begin();
		it != binding_pair_list.end() && n_it != n_end;
		it++, n_it++) {
			if (it->get_var_name() != *n_it)
				break;
			}
	// if we didn't go all the way through the binding pair list, it is bad
	if (it != binding_pair_list.end()) {
		cerr << "illegal Binding_set contents: ";
		cerr << *this << endl;
		return false;
		}
	else
		return true;	
}

// return the value for the variable name; return Symbol() if not found
// assumption: since comparison of Symbols for equality is fast, no attempt
// to take advantage of list order is required.
Symbol Binding_set::get_value(Symbol var_name) const
{
	// a simple linear scan of the list
	Binding_pair_list_t::const_iterator it;
	for(it = binding_pair_list.begin(); it != binding_pair_list.end(); it++)
		if (var_name == it->get_var_name())
			return it->get_var_value();
	return Symbol();
}

// return a binding set that has wildcards removed
Binding_set Binding_set::remove_wildcards() const
{
	Binding_set result;
	Binding_pair_list_t::const_iterator it;
	for (it = binding_pair_list.begin(); it != binding_pair_list.end(); it++) {
		if(!(*it).is_wildcard_pair())
			result.add_binding_pair(*it);
		}
	return result;
}


// output each pair separated by a space, whole list enclosed in parentheses
ostream& operator<< (ostream& os, const Binding_set& bs)
{
	Binding_pair_list_t::const_iterator it;
	os << '(';
	for (it = bs.binding_pair_list.begin(); it != bs.binding_pair_list.end(); it++) {
		if (it != bs.binding_pair_list.begin())
			os << ' ';
		os << (*it);
		}
	os << ')';
	return os;
}
} // end namespace


