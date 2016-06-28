// *** Symbol.h ***

/*
Symbol objects hold a character string value and/or a numeric value.
These values can be replaced, but not modified inside the Symbol.
If a string value is present, it is used as the basis for comparison.
The numeric value is compared only if no string value is present.

The numeric value is either a single double value, a Point, or a vector of Points.
The single value is stored in both the x and y values of a Point.
Numeric values are compared using the vector comparisons regardless of whether they
are a single value, a single Point, or a vector of Points.

If both string and numeric values are present, the Symbol serves as a "named value" - especially
useful if the value is a vector of many Points.

The strings and numeric values are stored as unique reference counted objects. New values
are compared to the stored values, and re-used if they match. 

Creating, copying, assigning, destroying increment/decrement the reference count for
a value; if the reference count goes to zero, the string or number object id deleted.

The whole goal of this class is to enable copy & assignment to be done with shallow copy and 
equality comparison to be done with only pointer comparison. Ordering comparison
is slowed down by the indirection. Since the strings and numeric value cannot be modified, 
no time is wasted on copying values, but the lookup for previous matching values slows down
creation of new values.

When the symbol is initialized or assigned from a non-symbol string, a check can made about
whether the string can be parsed completely as a number (using std::strtod). If it can, then 
the symbol is set up as containing a numeric value only. If not, it is set up as containing
a string. 

Any attempt to extract a value from a Symbol that does not correspond to the stored value
results in a Symbol_exception being thrown. 

Extracting a std::string always succeeds - the resulting string contains either the string
value of the symbol or the numeric value as written with the output operator.
*/

#ifndef SYMBOL_H
#define SYMBOL_H

#include "Point.h"
#include "Exception.h"
#include "Symbol_memory.h"

#include <iosfwd>
#include <string>	// for interface with strings
#include <vector>	// for interface with vectors
#include <list>		// for typedefs and operators for lists of Symbols

class Symbol_exception : public Exception {
public:
	Symbol_exception(const char * in_msg) : Exception(in_msg) {}
};

namespace GU = Geometry_Utilities;

//class Symbol_memory_Str_rep;
//class Symbol_memory_Vec_rep;

class Symbol {
public:
// change to default to check for number always 6/8/03
// it would be better if these constructors were all explicit to encourage 
// defining const Symbols as much as possible for speed, but there are many
// cases were the string is likely to be unique. Explicit remomved 7/30/06
	Symbol(const char * c = "Nil", bool check_for_number = true)
		: str_rep_ptr(0), vec_rep_ptr(0)
		{setup(c, check_for_number);}

	Symbol(const std::string& s, bool check_for_number = true)
		: str_rep_ptr(0), vec_rep_ptr(0)
		{setup(s.c_str(), check_for_number);}

	Symbol(double in_x)
		: str_rep_ptr(0), vec_rep_ptr(0)
		{setup(in_x);}

	Symbol(int in_x)
		: str_rep_ptr(0), vec_rep_ptr(0)
		{setup(double(in_x));}

	Symbol(long in_x)
		: str_rep_ptr(0), vec_rep_ptr(0)
		{setup(double(in_x));}

	Symbol(double in_x, double in_y)
		: str_rep_ptr(0), vec_rep_ptr(0)
		{setup(GU::Point(in_x, in_y));}

	Symbol(GU::Point p)
		: str_rep_ptr(0), vec_rep_ptr(0)
		{setup(p);}

	Symbol(const std::vector<GU::Point>& v)
		: str_rep_ptr(0), vec_rep_ptr(0)
		{setup(v);}

	Symbol(const char * c, const std::vector<GU::Point>& v)
		: str_rep_ptr(0), vec_rep_ptr(0)
		{setup(c, false); setup(v);}

	~Symbol();

	Symbol(const Symbol& src);
	// uses copy-swap
	const Symbol& operator= (const Symbol& rhs);
	// assign from a C-string
	const Symbol& operator= (const char * rhs);
	const Symbol& operator= (double rhs);
	// swap the members
	void swap(Symbol& other);
	
	bool has_string_value() const
		{return str_rep_ptr;}
	bool has_numeric_value() const
		{return vec_rep_ptr;}
	bool has_single_numeric_value() const;
	bool has_Point_numeric_value() const;
	bool has_pair_numeric_value() const;
	bool has_multiple_numeric_value() const;

	// the accessors will throw an exception if expected data is not present
	// string value must be present for these
	char const * c_str() const;
	int size() const;
	int length() const;
	std::string str() const; 
	// some form of numeric value must be present for these
	double get_numeric_value() const;	// must be a single double value
	GU::Point get_Point() const;	// must be a single Point
	// get x or y part of a pair of values
	double get_x() const;	// must be a single Point present
	double get_y() const;	// must be a single Point present
	// get the vector of Points
	const std::vector<GU::Point>& get_Point_vector() const;	// must be any numeric value


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

	bool operator== (const Symbol& rhs) const;
	bool operator!= (const Symbol& rhs) const;
	bool operator< (const Symbol& rhs) const;
	bool operator<= (const Symbol& rhs) const; 
	bool operator> (const Symbol& rhs) const ;
	bool operator>= (const Symbol& rhs) const;

	bool operator== (const char * rhs) const;	
	bool operator!= (const char * rhs) const;		
	bool operator<  (const char * rhs) const;	
	bool operator<= (const char * rhs) const;	
	bool operator>  (const char * rhs) const;	
	bool operator>= (const char * rhs) const;	
	bool operator== (double rhs) const;
	bool operator!= (double rhs) const;
	bool operator<  (double rhs) const;
	bool operator<= (double rhs) const;
	bool operator>  (double rhs) const;
	bool operator>= (double rhs) const;
	
	// non-member comparisons
	friend bool operator== (const char * lhs, const Symbol& rhs);
	friend bool operator!= (const char * lhs, const Symbol& rhs);
	friend bool operator<  (const char * lhs, const Symbol& rhs);
	friend bool operator<= (const char * lhs, const Symbol& rhs);
	friend bool operator>  (const char * lhs, const Symbol& rhs);
	friend bool operator>= (const char * lhs, const Symbol& rhs);
	friend bool operator== (double lhs, const Symbol& rhs);
	friend bool operator!= (double lhs, const Symbol& rhs);
	friend bool operator<  (double lhs, const Symbol& rhs);
	friend bool operator<= (double lhs, const Symbol& rhs);
	friend bool operator>  (double lhs, const Symbol& rhs);
	friend bool operator>= (double lhs, const Symbol& rhs);

	friend std::ostream& operator<< (std::ostream&, const Symbol&);
			
private:
	Symbol_memory_Str_rep * str_rep_ptr;	// zero if no string value is present
	Symbol_memory_Vec_rep * vec_rep_ptr;	// zero if no numeric value is present
	//	str_rep_ptr	vec_rep_ptr		contents of this Symbol
	//	!= 0		0				string only - comparisons done on strings
	//	0			!= 0			numeric value(s) only - comparisons done on values
	//	!= 0		!= 0			"named values" - comparisons done on strings
	//	0			0				not possible

	// setup a Symbol from a supplied c-string
	void setup(const char * c, bool check_for_number);
	// check for the string containing a number, set up this symbol if so.
	void setup(double x);
	void setup(GU::Point p);
	void setup(const std::vector<GU::Point>& v);
};

// following inlined here for speed given possible uses in containers and algorithms
inline bool Symbol::operator== (const Symbol& rhs) const
{	
	// to be equal, both pointers must be equal
	// equal if pointing to the same value or are zero	
	return (str_rep_ptr == rhs.str_rep_ptr) && (vec_rep_ptr == rhs.vec_rep_ptr);
}

inline bool Symbol::operator!= (const Symbol& rhs) const
{
	return !(this->operator== (rhs));
}

std::ostream& operator<< (std::ostream&, const Symbol&);

typedef std::list<Symbol> Symbol_list_t;


#endif
