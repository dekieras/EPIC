#ifndef VARIABLES_H
#define VARIABLES_H

#include "Symbol.h"
#include "Symbol_utilities.h"

#include <iosfwd>
#include <list>
#include <string>

//#include "Output_tee.h"

/*
By definition, a variable name is 
	at least 2 characters long 
	starts with '?' followed by some other character
a wildcard name is
	at least 3 characters long
	and starts with "???".

*/

namespace Parsimonious_Production_System {
using ::operator<<;	//bring global declarations into scope
class Variable_list;
std::ostream& operator<< (std::ostream& os, const Variable_list& varlist);

// forward declarations
class Full_Overlap;
class Partial_Overlap;
class No_Overlap;

inline bool is_variable(const Symbol& term)
{
	if(term.has_string_value() && term.size() >= 2 && term.c_str()[0] == '?' && term.c_str()[1] != '?')
		return true;
	else
		return false;
}

inline bool is_wildcard(const Symbol& term)
{
	// brute force, but should be fast
	if(term.has_string_value() && term.size() >= 3 && term.c_str()[0] == '?' && term.c_str()[1] == '?' && term.c_str()[2] == '?')
		return true;
	else
		return false;
}

Symbol full_wildcard_name(int wildcard_number);

// Varlist encapsulates a list of variable names and provides useful operations on them
// The list is sorted on request, and whenever it is provided to other objects,
// or compared to others

class Variable_list
{
public:
	Variable_list() : 
		sorted(false)
		{}

	// sort before copy and assignment
	Variable_list(const Variable_list& varlist)
		{
			varlist.sort();
			var_names = varlist.var_names;
			sorted = varlist.sorted;
		}

	Variable_list& operator= (const Variable_list& rhs)
		{
			rhs.sort();
			var_names = rhs.var_names;
			sorted = rhs.sorted;
			return *this;
		}

	void add(const Symbol& varname)
		{var_names.push_back(varname); sorted = false;}

	void sort() const
		{
			if (!sorted) {
				var_names.sort();
				sorted = true;
				}
		}
		
	bool empty() const
		{return var_names.empty();}
		
	std::size_t size() const
		{return var_names.size();}
		
	// tests true if list is non-empty
	operator bool ()
		{return !var_names.empty();}

	bool operator== (const Variable_list& rhs)
		{return var_names == rhs.var_names;}

	bool operator!= (const Variable_list& rhs)
		{return var_names != rhs.var_names;}

// function object classes for operating on variable lists
	friend class Full_Overlap;
	friend class Partial_Overlap;
	friend class No_Overlap;

friend std::ostream& operator<< (std::ostream& os, const Variable_list& varlist);
//friend Output_tee& operator<< (Output_tee& os, const Variable_list& varlist);

private:
	mutable Symbol_list_t var_names;
	mutable bool sorted;
};



//Output_tee& operator<< (Output_tee& os, const Variable_list& varlist);
//Output_tee& operator<< (Output_tee& os, const Symbol_list_t& term_list);
//Output_tee& operator<< (Output_tee& os, const list_Symbol_list_t& list_term_list);
//Output_tee& operator<< (Output_tee& os, const list_list_Symbol_list_t& list_list_term_list);

} // end namespace

#endif
