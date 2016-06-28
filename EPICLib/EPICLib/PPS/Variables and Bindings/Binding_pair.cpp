#include "Binding_pair.h"

#include <iostream>
	using std::ostream;

namespace Parsimonious_Production_System {


// output a pair in parentheses with a colon after the varname
ostream& operator<< (ostream& os, const Binding_pair& bp)
{
	os << '(' << bp.get_var_name() << ':' << bp.get_var_value() << ')';
	return os;
}

} // end namespace
