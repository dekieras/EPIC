#ifndef SYMBOL_UTILITIES_H
#define SYMBOL_UTILITIES_H

#include "Symbol.h"
#include <iosfwd>

class Output_tee;

// Return the Symbol that is nth in the list, starting with 0
// if not found, or not legal n, an empty Symbol is returned
Symbol get_nth_Symbol(const Symbol_list_t& in_list, int n);

void print_Symbol_list (const Symbol_list_t& in_list);
void print_Symbol_list (const Symbol_list_t& in_list, Output_tee& ot);

Symbol_list_t cstr_to_Symbol_list(const char *);

Symbol int_to_Symbol(int i);

// return a Symbol consisting of the supplied string(s) or Symbol(s) 
// followed by the digits of the supplied integer

Symbol concatenate_to_Symbol(const char * str, long i);
Symbol concatenate_to_Symbol(const Symbol& sym, long i);
Symbol concatenate_to_Symbol(const char * str1, const char * str2, long i);
Symbol concatenate_to_Symbol(const Symbol& sym1, const Symbol& sym2, long i);
Symbol concatenate_to_Symbol(const char * str, const Symbol& sym, long i);
Symbol concatenate_to_Symbol(const Symbol& sym, const char * str, long i);
Symbol concatenate_to_Symbol(const char * str1, const char * str2, const char * str3, long i);
Symbol concatenate_to_Symbol(const char * str1, const Symbol& sym, const char * str3, long i);

// defined in Symbol.h
//typedef std::list<Symbol> Symbol_list_t;

typedef std::list<Symbol_list_t> list_Symbol_list_t;

typedef std::list<list_Symbol_list_t> list_list_Symbol_list_t;

std::string concatenate_to_string(const Symbol_list_t&);

std::ostream& operator<< (std::ostream& os, const Symbol_list_t& symbol_list);
std::ostream& operator<< (std::ostream& os, const list_Symbol_list_t& list_symbol_list);
std::ostream& operator<< (std::ostream& os, const list_list_Symbol_list_t& list_list_symbol_list);

#endif
