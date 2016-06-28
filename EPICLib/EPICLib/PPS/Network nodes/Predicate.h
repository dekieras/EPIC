#ifndef PREDICATE_H
#define PREDICATE_H

#include "Binding_set_list.h"

namespace Parsimonious_Production_System {

// This abstract base class defines the interface for Predicates
class Predicate {
public:
	virtual ~Predicate()
		{}
	// applying the predicate to a supplied list of binding sets produces a filtered
	// list of binding sets.
	virtual Binding_set_list_t apply(const Binding_set_list_t& input_bsl) const = 0;
	// display the static contents of the predicate
	virtual void display_contents(int indent_level = 0) const;
	// a factory function for creating predicates
	static Predicate * create(const Symbol_list_t& predicate_pattern);
};

typedef std::list<Predicate *> Predicate_ptr_list_t;

} // end namespace

#endif
