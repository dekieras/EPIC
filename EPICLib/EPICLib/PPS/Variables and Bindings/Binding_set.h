#ifndef BINDING_SET_H
#define BINDING_SET_H

#include "Binding_pair.h"

#include <iosfwd>
#include <list>


namespace Parsimonious_Production_System {
using ::operator<<;	// bring global declarations into this scope
// output each pair separated by a space
class Binding_set;
std::ostream& operator<< (std::ostream& os, const Binding_set& bs);

// forward declarations
class Full_Overlap;
class Partial_Overlap;
class No_Overlap;

typedef std::list<Binding_pair> Binding_pair_list_t;

class Binding_set {
public:
	void add_binding_pair(const Binding_pair& bp);

	friend class Full_Overlap;
	friend class Partial_Overlap;
	friend class No_Overlap;

	bool operator== (const Binding_set& rhs) const;
	bool operator!= (const Binding_set& rhs) const;
	
	// Convert a Binding_set to true if it is non-empty, false if it is empty
	operator bool() {return !binding_pair_list.empty();}
	
	// return a binding set that has wildcard binding pairs removed
	Binding_set remove_wildcards() const;

	std::size_t size() const
		{return binding_pair_list.size();}
	bool empty() const
		{return binding_pair_list.empty();}
	void clear()
		{binding_pair_list.clear();}
		
	// true if this binding set has unique varnames in order
	bool is_legal() const;
	// return the value for the variable name; return Symbol() if not found
	Symbol get_value(Symbol var_name) const;
	
private:
	Binding_pair_list_t binding_pair_list;

friend std::ostream& operator<< (std::ostream& os, const Binding_set& bs);
};


} // end namespace

#endif

