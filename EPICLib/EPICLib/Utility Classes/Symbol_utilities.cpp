#include "Symbol_utilities.h"
#include "Output_tee.h"

#include <iostream>
#include <sstream>
#include <string>


using std::ostream;	using std::cout;	using std::endl;
using std::ostringstream;	using std::istringstream;
using std::string;


// Return the Symbol that is nth in the list, starting with 0
// if not found, or not legal n, an empty Symbol is returned
Symbol get_nth_Symbol(const Symbol_list_t & in_list, int n)
{
	if (n > int((in_list.size() - 1)))
		return Symbol();
	
	Symbol_list_t ::const_iterator it = in_list.begin();
	
	for (int i = 0; i < n; i++)
		it++;
		
	if (it != in_list.end())
		return *it;
	else
		return Symbol();
}

void print_Symbol_list (const Symbol_list_t & in_list)
{
	Symbol_list_t ::const_iterator it;
	
	for (it = in_list.begin(); it != in_list.end(); it++)
		cout << *it << endl;
}

void print_Symbol_list(const Symbol_list_t & in_list, Output_tee& ot)
{
	Symbol_list_t ::const_iterator it;
	
	for (it = in_list.begin(); it != in_list.end(); it++)
		ot << *it << endl;
}

// return a list of Symbols, where each sysbol is the whitespace delimited sequence
// in the input C-string.  e.g. "A B CD E" => (A B CD E)
Symbol_list_t cstr_to_Symbol_list(const char * str)
{
	istringstream is(str);
	string s;
	Symbol_list_t result;
	while(is >> s)
		result.push_back(Symbol(s));
	return result;
}


Symbol int_to_Symbol(int i)
{
	ostringstream ss;
	ss << i;
	return Symbol(ss.str());
}

Symbol concatenate_to_Symbol(const char * str, long i)
{
	ostringstream ss;
	ss << str;
	ss << i;
	return Symbol(ss.str());
}

Symbol concatenate_to_Symbol(const Symbol& sym, long i)
{
	return concatenate_to_Symbol(sym.c_str(), i);
}

Symbol concatenate_to_Symbol(const char * str1, const char * str2, long i)
{
	ostringstream ss;
	ss << str1;
	ss << str2;
	ss << i;
	return Symbol(ss.str());
}

Symbol concatenate_to_Symbol(const Symbol& sym1, const Symbol& sym2, long i)
{
	return concatenate_to_Symbol(sym1.c_str(), sym2.c_str(), i);
}

Symbol concatenate_to_Symbol(const char * str, const Symbol& sym, long i)
{
	return concatenate_to_Symbol(str, sym.c_str(), i);
}

Symbol concatenate_to_Symbol(const Symbol& sym, const char * str, long i)
{
	return concatenate_to_Symbol(sym.c_str(), str, i);
}

Symbol concatenate_to_Symbol(const char * str1, const char * str2, const char * str3, long i)
{
	ostringstream ss;
	ss << str1;
	ss << str2;
	ss << str3;
	ss << i;
	return Symbol(ss.str());
}

Symbol concatenate_to_Symbol(const char * str1, const Symbol& sym, const char * str3, long i)
{
	return concatenate_to_Symbol(str1, sym.c_str(), str3, i);
}

// copy the list of Symbols into a space-delimited string
string concatenate_to_string(const Symbol_list_t& symbol_list)
{
	if (symbol_list.empty())
		return string();
		
	ostringstream ss;
	for(Symbol_list_t::const_iterator it = symbol_list.begin(); it != symbol_list.end(); it++) {
		if (it != symbol_list.begin())
			ss << ' ';
		ss << (*it);
		}
	return ss.str();
}

ostream& operator<< (ostream& os, const Symbol_list_t& symbol_list)
{
	if (symbol_list.empty())
		return os;
		
	os << '(';
	for(Symbol_list_t::const_iterator it = symbol_list.begin(); it != symbol_list.end(); it++) {
		if (it != symbol_list.begin())
			os << ' ';
		os << (*it);
		}
	os << ')';

	return os;
}

ostream& operator<< (ostream& os, const list_Symbol_list_t& list_symbol_list)
{
	if (list_symbol_list.empty())
		return os;
		
	for(list_Symbol_list_t::const_iterator it = list_symbol_list.begin(); it != list_symbol_list.end(); it++) {
		if(it != list_symbol_list.begin())
			os << endl;
		os << (*it);
		}
	return os;
}


ostream& operator<< (ostream& os, const list_list_Symbol_list_t& list_list_symbol_list)
{
	if (list_list_symbol_list.empty())
		return os;
		
	for(list_list_Symbol_list_t::const_iterator it = list_list_symbol_list.begin(); it != list_list_symbol_list.end(); it++) {
		if(it != list_list_symbol_list.begin())
			os << endl;
		os << (*it);
		}
	return os;
}
