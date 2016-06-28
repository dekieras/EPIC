#include "Variables.h"
//#include "Output_tee.h"
//#include "PPS_globals.h"

#include <iostream>
	using std::ostream; using std::cout; using std::endl;
//#include <string>
#include <sstream>
	using std::ostringstream;

/*
ostream& operator<< (ostream& os, const Symbol_list_t& term_list)
{
	if (term_list.empty())
		return os;
		
	os << '(';
	for(Symbol_list_t::const_iterator it = term_list.begin(); it != term_list.end(); it++) {
		if (it != term_list.begin())
			os << ' ';
		os << (*it);
		}
	os << ')';

	return os;
}

Output_tee& operator<< (Output_tee& os, const Symbol_list_t& term_list)
{
	if (term_list.empty())
		return os;
		
	os << '(';
	for(Symbol_list_t::const_iterator it = term_list.begin(); it != term_list.end(); it++) {
		if (it != term_list.begin())
			os << ' ';
		os << (*it);
		}
	os << ')';

	return os;
}

ostream& operator<< (ostream& os, const list_Symbol_list_t& list_term_list)
{
	if (list_term_list.empty())
		return os;
		
	for(list_Symbol_list_t::const_iterator it = list_term_list.begin(); it != list_term_list.end(); it++) {
		if(it != list_term_list.begin())
			os << endl;
		os << (*it);
		}
	return os;
}


ostream& operator<< (ostream& os, const list_list_Symbol_list_t& list_list_term_list)
{
	if (list_list_term_list.empty())
		return os;
		
	for(list_list_Symbol_list_t::const_iterator it = list_list_term_list.begin(); it != list_list_term_list.end(); it++) {
		if(it != list_list_term_list.begin())
			os << endl;
		os << (*it);
		}
	return os;
}
*/


namespace Parsimonious_Production_System {
using ::operator<<;	// bring global declarations into scope

ostream& operator<< (ostream& os, const Variable_list& varlist)
{
//	varlist.sort();
	if (varlist.empty())
		return os;
	os << '<';
	for(Symbol_list_t::iterator it = varlist.var_names.begin(); it != varlist.var_names.end(); it++) {
		if (it != varlist.var_names.begin())
			os << ' ';
		os << (*it);
		}
	os << '>';

	return os;
}



// append the number to a string consisting of "???"
Symbol full_wildcard_name(int wildcard_number)
{
	ostringstream ss;
	ss << "???" << wildcard_number;
	return Symbol(ss.str());
}

} // end namespace
