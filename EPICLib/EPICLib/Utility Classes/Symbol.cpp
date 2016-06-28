// *** Symbol.cpp ***

// This defines a class of read-only strings suitable for use as symbols.
// Once created, they can be only output, compared, or destroyed. 
// They can be initialized with or compared to C strings.
// A const pointer to the internal C string can be obtained.

// When the Symbol object is constructed from a string that can be parsed as a number,
// the numeric value is saved and a flag is set the the object contains a numeric value.
// If the object is constructed from a number, a default string is created for it using
// default iostream output rules.
// The numeric value can be accessed, and used in comparisons, but may not be changed.

#include "Symbol.h"
#include "Symbol_memory.h"
#include "Assert_throw.h"
#include "Utility_templates.h"

#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdio>
#include <utility>	// for swap

using std::ostream; using std::cerr; using std::endl;
using std::set;
using std::ostringstream;
using std::string;
using std::strlen; using std::strcpy; using std::strtod; using std::strcmp;
using std::size_t;
using std::vector;

//#include "Output_tee.h"

// setup an Symbol from a supplied c-string
// check for it being a number, save it as a number if so
// if not a number, enter the c-string;

// needs fixing to allow for named value!!!

void Symbol::setup(const char * c, bool check_string_for_number)
{
	int len = int(strlen(c));	// calculate this only once
	// if len is zero, we must want an empty string
	// and it can't contain anything else
	if(len) {
		if (check_string_for_number) {
			// check for the c-string containing a number
			// the whole string must be the number
			char * ptr;
			double temp = strtod(c, &ptr);
			if (ptr == c + len) {
				str_rep_ptr = 0;	// we have a number
				setup(temp); // set it up instead
				return;		// setup is done
				}
			}
		}
	// if check not asked for, this is not a number, store as string only
	// set up the string representation in memory
	str_rep_ptr = Symbol_memory::get_instance().find_or_insert(c, len);
}


void Symbol::setup(double x_)
{
	vector<GU::Point> v(1, GU::Point(x_, x_));
	// set up a single value
	vec_rep_ptr = Symbol_memory::get_instance().find_or_insert(true, v);
}
	
void Symbol::setup(GU::Point p)
{
	vector<GU::Point> v(1, p);
	vec_rep_ptr = Symbol_memory::get_instance().find_or_insert(false, v);
}
	
void Symbol::setup(const vector<GU::Point>& v_)
{
	vec_rep_ptr = Symbol_memory::get_instance().find_or_insert(false, v_);
}

		
Symbol::~Symbol()
{
	// if this symbol has a string, remove it if it is the last
	if(str_rep_ptr) {
		Symbol_memory::get_instance().remove_if_last(str_rep_ptr);
		}
	// ditto if it has a vector
	if(vec_rep_ptr) {
		Symbol_memory::get_instance().remove_if_last(vec_rep_ptr);
		}
}

Symbol::Symbol(const Symbol& src)
{
	str_rep_ptr = src.str_rep_ptr;
	if(str_rep_ptr)
		(str_rep_ptr->count)++;
	vec_rep_ptr = src.vec_rep_ptr;
	if(vec_rep_ptr)
		(vec_rep_ptr->count)++;
}
	
// uses copy-swap
const Symbol& Symbol::operator= (const Symbol& rhs) 
{
	Symbol temp(rhs);
	swap(temp);
	return *this;
}
		
// swap the members; note how reference counts of reps are unchanged
void Symbol::swap(Symbol& other)
{
	std::swap(str_rep_ptr, other.str_rep_ptr);
	std::swap(vec_rep_ptr, other.vec_rep_ptr);
}

// assign from a C-string
const Symbol& Symbol::operator= (const char * rhs)
{
	Symbol temp(rhs);
	swap(temp);
	return *this;
}

const Symbol& Symbol::operator= (double rhs)
{
	Symbol temp(rhs);
	swap(temp);
	return *this;
}

bool Symbol::has_single_numeric_value() const 
{
	return vec_rep_ptr && vec_rep_ptr->single;
}

bool Symbol::has_Point_numeric_value() const 
{
	return vec_rep_ptr && !(vec_rep_ptr->single) && (vec_rep_ptr->vec.size() == 1);
}

bool Symbol::has_pair_numeric_value() const 
{
	return has_Point_numeric_value();
}

bool Symbol::has_multiple_numeric_value() const 
{
	return vec_rep_ptr && (vec_rep_ptr->vec.size() > 2);
}


// return a C string or throw an exception if there is none
char const * Symbol::c_str() const
{
	if (str_rep_ptr)
		return str_rep_ptr->cstr;
	throw Symbol_exception("No C-string for Symbol");
}	

int Symbol::size() const
{
	if (str_rep_ptr)
//		return int(strlen(str_rep_ptr->cstr));
		return int(str_rep_ptr->cstr_len);
	throw Symbol_exception("No C-string size for Symbol");
}

int Symbol::length() const
{
	if (str_rep_ptr)
		return int(str_rep_ptr->cstr_len);
	throw Symbol_exception("No C-string length for Symbol");
}

// return a string containing either the C string or a string
// representation of the numeric value using the output operator
string Symbol::str() const
{
	ostringstream oss;
		oss << *this;
	return oss.str();	
}

// get the numeric values or throw an exception if not present
double Symbol::get_numeric_value() const
{
	if(has_single_numeric_value())
		return vec_rep_ptr->vec[0].x;
	throw Symbol_exception("Symbol is not single numeric value");
}

GU::Point Symbol::get_Point() const
{
	if(has_Point_numeric_value())
		return vec_rep_ptr->vec[0];
	throw Symbol_exception("Symbol is not a Point numeric value");
}

double Symbol::get_x() const
{
	if(has_pair_numeric_value())
		return vec_rep_ptr->vec[0].x;
	throw Symbol_exception("Symbol is not a pair numeric value");
}

double Symbol::get_y() const
{
	if(has_pair_numeric_value())
		return vec_rep_ptr->vec[0].y;
	throw Symbol_exception("Symbol is not a pair numeric value");
}

const vector<GU::Point>& Symbol::get_Point_vector() const
{
	if(has_numeric_value())
		return vec_rep_ptr->vec;
	throw Symbol_exception("Symbol is not a vector value");
}
	
/* Comparisons
Symbol1			Symbol2				Comparison
string only		string only			strings are compared
string only		number only			number is always less-than string
string & number	string				strings are compared, number ignored
string & number	string & number		strings are compared, numbers ignored
string & number	number only			number is always less-than string & number
number only		number only			numbers are compared

Number comparisons are done with vector and Point comparison rules

Comparisons with non-Symbol types follow the corresponding rules 
*/

// not inlined because Str_rep and Vec_rep would have to be visible
bool Symbol::operator< (const Symbol& rhs) const 
{
	// if the two are different string configurations, can quit immediately
	if(str_rep_ptr && !rhs.str_rep_ptr) {
		//Assert(rhs.vec_rep_ptr);
		return false;	// str is not less than number only
		}
	else if(!str_rep_ptr && rhs.str_rep_ptr) {
		//Assert(vec_rep_ptr);
		return true;	// number only is less than str
		}
	else if(str_rep_ptr && rhs.str_rep_ptr)	{ // if both strings, compare them
		if(str_rep_ptr == rhs.str_rep_ptr) {	// same strings 
			return false;	// same str is not less than
			}
		// compare the different strings for less than
		else 
			return (strcmp(str_rep_ptr->cstr, rhs.str_rep_ptr->cstr) < 0);
		}
	// no strings, just numbers
	//Assert(vec_rep_ptr && rhs.vec_rep_ptr);
	return vec_rep_ptr->vec < rhs.vec_rep_ptr->vec;
}


// define these in terms of operator<
bool Symbol::operator<= (const Symbol& rhs) const 
{
	return  !(rhs < *this);
}

bool Symbol::operator> (const Symbol& rhs) const 
{
	return rhs < *this;
}
		
bool Symbol::operator>= (const Symbol& rhs) const 
{
	return  !(*this < rhs);
}

/* Member comparisons */
bool Symbol::operator== (const char * rhs) const
{
	return (str_rep_ptr) ? (std::strcmp(str_rep_ptr->cstr, rhs) == 0) : false;
}

bool Symbol::operator!= (const char * rhs) const
{
	return (str_rep_ptr) ? (std::strcmp(str_rep_ptr->cstr, rhs) != 0) : true;
}

bool Symbol::operator<  (const char * rhs) const
{
	return (str_rep_ptr) ? (std::strcmp(str_rep_ptr->cstr, rhs) < 0) : true;
}

bool Symbol::operator<=  (const char * rhs) const
{
	return (str_rep_ptr) ? (std::strcmp(str_rep_ptr->cstr, rhs) <= 0) : true;
}

bool Symbol::operator>  (const char * rhs) const
{
	return (str_rep_ptr) ? (std::strcmp(str_rep_ptr->cstr, rhs) > 0) : false;
}

bool Symbol::operator>=  (const char * rhs) const
{
	return (str_rep_ptr) ? (std::strcmp(str_rep_ptr->cstr, rhs) >= 0) : false;
}

bool Symbol::operator== (double rhs) const
{
	return (str_rep_ptr) ? false : (get_numeric_value() == rhs);
}

bool Symbol::operator!= (double rhs) const
{
	return (str_rep_ptr) ? true : (get_numeric_value() != rhs);
}

bool Symbol::operator<  (double rhs) const
{
	return (str_rep_ptr) ? false : (get_numeric_value() < rhs);
}

bool Symbol::operator<=  (double rhs) const
{
	return (str_rep_ptr) ? false : (get_numeric_value() <= rhs);
}

bool Symbol::operator>  (double rhs) const
{
	return (str_rep_ptr) ? true : (get_numeric_value() > rhs);
}

bool Symbol::operator>=  (double rhs) const
{
	return (str_rep_ptr) ? true : (get_numeric_value() >= rhs);
}

/* Non-member comparisons */

// if rhs has no string, result is false
bool operator== (const char * lhs, const Symbol& rhs)  
//	{return (rhs.str_rep_ptr) ? (strcmp(lhs, rhs.str_rep_ptr->cstr) == 0) : false;}
{
	return (rhs.has_string_value()) ? (strcmp(lhs, rhs.c_str()) == 0) : false;}

// if rhs has no string, result is true
bool operator!= (const char * lhs, const Symbol& rhs)  
	{return (rhs.has_string_value()) ? (strcmp(lhs, rhs.c_str()) != 0) : true;}

// if rhs has no string, result is false - numbers are less
bool operator< (const char * lhs, const Symbol& rhs)  
	{return (rhs.has_string_value()) ? (strcmp(lhs, rhs.c_str()) < 0) : false;}

bool operator<= (const char * lhs, const Symbol& rhs)  
	{return (rhs.has_string_value()) ? (strcmp(lhs, rhs.c_str()) <= 0) : false;}

// if rhs has no string, result is true - numbers are less
bool operator> (const char * lhs, const Symbol& rhs)  
	{return (rhs.has_string_value()) ? (strcmp(lhs, rhs.c_str()) > 0) : true;}

bool operator>= (const char * lhs, const Symbol& rhs)  
	{return (rhs.has_string_value()) ? (strcmp(lhs, rhs.c_str()) >= 0) : true;}

// if rhs is not a number, result is false
bool operator== (double lhs, const Symbol& rhs)
	{return (rhs.has_numeric_value()) ? (rhs.get_numeric_value() == lhs) : false;}

// if rhs is not a number, result is true
bool operator!= (double lhs, const Symbol& rhs)
	{return (rhs.has_numeric_value()) ? (rhs.get_numeric_value() != lhs) : true;}
		
// if rhs is not a number, result is true - numbers are less
bool operator< (double lhs, const Symbol& rhs) 
	{return (rhs.has_numeric_value()) ? (rhs.get_numeric_value() < lhs) : true;}

bool operator<= (double lhs, const Symbol& rhs) 
	{return (rhs.has_numeric_value()) ? (rhs.get_numeric_value() <= lhs) : true;}

// if rhs is not a number, result is false - numbers are less
bool operator> (double lhs, const Symbol& rhs) 
	{return (rhs.has_numeric_value()) ? (rhs.get_numeric_value() > lhs) : false;}

bool operator>= (double lhs, const Symbol& rhs) 
	{return (rhs.has_numeric_value()) ? (rhs.get_numeric_value() >= lhs) : false;}
	
// output operators
ostream& operator<< (ostream& os, const Symbol& s) 
{
	if (s.has_string_value()) {
		os << s.c_str();
		}
	else if (s.has_single_numeric_value()) {
		os << s.get_numeric_value();
		}
	else if (s.has_pair_numeric_value()) {
		os << '(' << s.get_x() << ", " << s.get_y() << ')';
		}
	// nothing for other cases
	return os;
}
