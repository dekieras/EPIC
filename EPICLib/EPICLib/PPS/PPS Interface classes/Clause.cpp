#include "Clause.h"
#include "Symbol_utilities.h"
#include "Assert_throw.h"
#include "Output_tee.h"

//#include <list>
#include <string>
#include <iostream>

using std::string;
using std::cout;	using std::ostream;
using std::size_t;
	
// construct a clause from a Tag, object name, property name, property value
Clause::Clause(const Symbol& tag_name, const Symbol& object_name, 
	const Symbol& property_name, const Symbol& property_value)
{
	clause.push_back(tag_name);
	clause.push_back(object_name);
	clause.push_back(property_name);
	clause.push_back(property_value);
	current_term = clause.begin();
}

bool Clause::at_end() const
{
	if(current_term == clause.end())
		return true;
	else
		return false;
}
		
	
void Clause::advance() const
{
	if (current_term != clause.end())
		current_term++;
}
		
// result is undefined if at end!
const Symbol& Clause::get_current_term() const
{
	Assert(!at_end());
	return (*current_term);
}

// lhs Clause is < rhs clause if lhs terms are less than rhs terms
// in the same way that one string is < another
bool Clause::operator< (const Clause& rhs) const
{
	Symbol_list_t::const_iterator lhs_it = clause.begin();
	Symbol_list_t::const_iterator rhs_it = rhs.clause.begin();
	
	while(lhs_it != clause.end() && rhs_it != rhs.clause.end()) {
		if(*lhs_it < *rhs_it)
			return true;
		else if (*lhs_it > *rhs_it)
			return false;
		lhs_it++;
		rhs_it++;
		}
	// here if compared equal until at least one terminated
	if(rhs_it != rhs.clause.end())	// lhs was shorter
		return true;
	return false;	// were either identical, or rhs was shorter
}

void Clause::display(ostream& os) const
{
	os << clause;
//	::operator<<(os, clause);
	os << "\n   current term: ";
	if (at_end())
		os << "at end";
	else
		os << get_current_term();
}

void Clause::display(Output_tee& ot) const
{
	ot << clause;
	ot << "\n   current term: ";
	if (at_end())
		ot << "at end";
	else
		ot << get_current_term();
}

ostream& operator<< (ostream& os, const Clause& in_clause)
{
//	::operator<<(os, in_clause.clause);
	os << in_clause.clause;
	return os;
}

/*
Output_tee& operator<< (Output_tee& ot, const Clause& in_clause)
{
	ot << in_clause.clause;
	return ot;
}
*/

// in_str is a string

Symbol_list_t Clause::parse_clause_string (const string& in_str)
{
	Symbol_list_t result;
	size_t start_pos = 0;

	start_pos = in_str.find_first_of("(", start_pos);
	if (start_pos == string::npos)
		return Symbol_list_t();
	start_pos = in_str.find_first_not_of(" ", start_pos+1);
	if (start_pos == string::npos)
		return Symbol_list_t();
	
	string sterm = get_term(in_str, start_pos);
		
	while (sterm.size() > 0) {
		Symbol term(sterm);
		result.push_back(term);
		sterm = get_term(in_str, start_pos);
		}
		
	return result;
}



// return a string containing the contents of the next word that is terminated with a space
// set the start_pos argument to the next starting position for the search
string Clause::get_term (const string& in_str, size_t& start_pos)
{
	// if start_pos is at the end of the string, return an empty string
	if (start_pos == string::npos)
		return string();	
	// skip any leading spaces
	start_pos = in_str.find_first_not_of(" ", start_pos);
	if (start_pos == string::npos)
		return string();	
	// get the character number where the word ends
	size_t end_pos = in_str.find_first_of(" )", start_pos);
	// if at the end of the string:
	size_t length, next_start;
	if (end_pos == string::npos) {
		length = end_pos;
		next_start = string::npos;
		}
	else {
		length = (end_pos - start_pos);	// note unsigned arithmetic!
		next_start = end_pos + 1;
		}
	// get the substring that starts there and has that many characters
	string result = in_str.substr(start_pos, length);
	start_pos = next_start;
	return result;
}

