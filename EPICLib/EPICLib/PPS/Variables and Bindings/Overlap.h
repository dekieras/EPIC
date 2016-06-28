#ifndef OVERLAP_H
#define OVERLAP_H

#include "Variables.h"
#include "Binding_set.h"

// These function object classes define the ways in which sets of variables and bindings
// can overlap
namespace Parsimonious_Production_System {

// function object classes
class Full_Overlap {
public:
	Variable_list operator() (const Variable_list& lhs, const Variable_list& rhs);
	Binding_set operator() (const Binding_set& lhs, const Binding_set& rhs);
	bool is_legal(const Binding_set& lhs, const Binding_set& rhs);
};

class Partial_Overlap {
public:
	Variable_list operator() (const Variable_list& lhs, const Variable_list& rhs);
	Binding_set operator() (const Binding_set& lhs, const Binding_set& rhs);
	bool is_legal(const Binding_set& lhs, const Binding_set& rhs);
};

class No_Overlap {
public:
	Variable_list operator() (const Variable_list& lhs, const Variable_list& rhs);
	Binding_set operator() (const Binding_set& lhs, const Binding_set& rhs);
	bool is_legal(const Binding_set& lhs, const Binding_set& rhs);
};

} // end namespace

#endif
