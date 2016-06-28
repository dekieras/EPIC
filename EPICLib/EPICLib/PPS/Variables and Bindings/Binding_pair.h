#ifndef BINDING_PAIR_H
#define BINDING_PAIR_H

#include "Variables.h"
#include "Symbol_utilities.h"
#include <iosfwd>
//#include <string>
//#include <list>


/* A binding_pair is a variable name and a variable value.  
They are ordered first by variable name and second by variable value.
They are equal if both the variable name and the variable value are equal.
*/

namespace Parsimonious_Production_System {
using ::operator<<;	// bring other declarations into the global scope
// output a pair in parentheses with a colon after the varname
class Binding_pair;
std::ostream& operator<< (std::ostream& os, const Binding_pair& bp);


class Binding_pair {
public:
	Binding_pair() {}
	Binding_pair(const char * v_name, const char * v_value) :
		var_name(v_name), var_value(v_value) {}

	Binding_pair(const Symbol& v_name, const Symbol& v_value) :
		var_name(v_name), var_value(v_value) {}
		
	const Symbol& get_var_name() const
		{return var_name;}
	
	const Symbol& get_var_value() const
		{return var_value;}
		
	bool is_wildcard_pair() const	// return true if var_name is a wildcard
{
	return is_wildcard(var_name);
}

//	ordering relation applies only to the var_names
	bool operator< (const Binding_pair& rhs) const {
		return (var_name < rhs.var_name);
		}
	bool operator== (const Binding_pair& rhs) const {
		return ((var_name == rhs.var_name) && (var_value == rhs.var_value));
		}
		
	bool operator!= (const Binding_pair& rhs) const {
		return ((var_name != rhs.var_name) || (var_value != rhs.var_value));
		}

private:
	Symbol var_name;
	Symbol var_value;
	
friend std::ostream& operator<< (std::ostream& os, const Binding_pair& bp);

};

} // end namespace

#endif
