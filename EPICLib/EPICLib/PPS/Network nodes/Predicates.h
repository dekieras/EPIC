#ifndef PREDICATES_H
#define PREDICATES_H

#include "Predicate.h"

namespace Parsimonious_Production_System {

/*	No-argument Predicates */
// If_only_one_Predicate returns the first binding set, if it contains only one set;
// Otherwise, it returns an empty list
class If_only_one_predicate : public Predicate {
public:
	virtual Binding_set_list_t apply(const Binding_set_list_t& input_bsl) const;
};

// Use_only_one_Predicate returns only the first binding set, if any
class Use_only_one_predicate : public Predicate {
public:
	virtual Binding_set_list_t apply(const Binding_set_list_t& input_bsl) const;

};

// Randomly_choose_one_Predicate returns one binding set chosen at random
class Randomly_choose_one_predicate : public Predicate {
public:
	virtual Binding_set_list_t apply(const Binding_set_list_t& input_bsl) const;
};

/* Unary Predicates */
// Unary predicates take a single argument which can be either a variable or a constant.
// Some operation is apply which results in the input binding set being transformed
// for the output.

// At construction, we determine and store whether the argument is 
// a constant or a variable name whose value needs to looked up in the binding  set. 
// A Template Method for apply is supplied here, made virtual so that it can be overridden.
class Unary_predicate : public Predicate {
public:
	Unary_predicate(const Symbol& in_argument);
	virtual ~Unary_predicate()
		{}
	// applying the predicate to a supplied list of binding sets produces a filtered
	// list of binding sets. Template Method in this class calls compare()
	virtual Binding_set_list_t apply(const Binding_set_list_t& input_bsl) const;
	
	// display the static contents of the predicate
	virtual void display_contents(int indent_level = 0) const;

protected:
	// returns true if the binding set is to be included in the result
	virtual bool compare(const Symbol& v1) const = 0;
	
	Symbol get_value(Binding_set_list_t::const_iterator it) const
		{return (arg_is_variable) ? (*it).get_value(argument) : argument;}

	Symbol get_value() const
		{return argument;}
	
private:
	Symbol argument;
	bool arg_is_variable;
};

// Returns each binding with a probability specified by the argument
class Randomly_use_each_predicate : public Unary_predicate {
public:
	Randomly_use_each_predicate(const Symbol& in_argument) :
		Unary_predicate(in_argument) {}
protected:
	virtual bool compare(const Symbol& v1) const;
};

// Returns all of the bindings with a probability specified by the argument
class Randomly_true_predicate : public Unary_predicate {
public:
	Randomly_true_predicate(const Symbol& in_argument) :
		Unary_predicate(in_argument) {}

	virtual Binding_set_list_t apply(const Binding_set_list_t& input_bsl) const;
protected:
	virtual bool compare(const Symbol&) const {return false;}
};

//	TEH 7/17/07
//	Returns one binding for which the argument variable is the least value.
class Least_predicate : public Unary_predicate {
public:
	Least_predicate(const Symbol& in_argument) :
		Unary_predicate(in_argument)
		{}
	
	virtual Binding_set_list_t apply(const Binding_set_list_t& input_bsl) const;
protected:
	virtual bool compare(const Symbol&) const {return false;}
};

// DEK 6/24/2011
//	Returns one binding for which the argument variable is the greatest value.
class Greatest_predicate : public Unary_predicate {
public:
	Greatest_predicate(const Symbol& in_argument) :
		Unary_predicate(in_argument)
		{}
	
	virtual Binding_set_list_t apply(const Binding_set_list_t& input_bsl) const;
protected:
	virtual bool compare(const Symbol&) const {return false;}
};


/* Binary Predicates */
// Binary predicates involve comparing the values of two arguments which can be either
// variables or constants.

// This class provide binary predicate functionality - at construction, we determine
// and store whether each argument is a constant or a variable name whose value needs to
// looked up in the binding  set. A Template Method for apply is supplied here,
// made virtual so that it can be overridden.
class Binary_predicate : public Predicate {
public:
	Binary_predicate(const Symbol& in_argument1, const Symbol& in_argument2);
	virtual ~Binary_predicate()
		{}
	// applying the predicate to a supplied list of binding sets produces a filtered
	// list of binding sets. Template Method in this class calls compare()
	virtual Binding_set_list_t apply(const Binding_set_list_t& input_bsl) const;
	
	// display the static contents of the predicate
	virtual void display_contents(int indent_level = 0) const;

protected:
	// returns true if the binding set is to be included in the result
	virtual bool compare(const Symbol& v1, const Symbol& v2)  const = 0;
	
	Symbol get_value1(Binding_set_list_t::const_iterator it) const
		{return (is_variable1) ? (*it).get_value(argument1) : argument1;}
	
	Symbol get_value2(Binding_set_list_t::const_iterator it) const
		{return (is_variable2) ? (*it).get_value(argument2) : argument2;}
private:
	Symbol argument1;
	bool is_variable1;
	Symbol argument2;
	bool is_variable2;
};

// returns binding sets in which the two arguments are equal
class Equal_predicate : public Binary_predicate {
public:
	Equal_predicate(const Symbol& in_argument1, const Symbol& in_argument2) :
		Binary_predicate(in_argument1, in_argument2) {}
protected:
	virtual bool compare(const Symbol& v1, const Symbol& v2) const;
};

// returns binding sets in which the two arguments are not equal
class Different_predicate : public Binary_predicate {
public:
	Different_predicate(const Symbol& in_argument1, const Symbol& in_argument2) :
		Binary_predicate(in_argument1, in_argument2) {}
protected:
	virtual bool compare(const Symbol& v1, const Symbol& v2) const;
};

// The following return binding sets in which the first argument has the
// named relation to the second.

class Greater_than_predicate : public Binary_predicate {
public:
	Greater_than_predicate(const Symbol& in_argument1, const Symbol& in_argument2) :
		Binary_predicate(in_argument1, in_argument2) {}
protected:
	virtual bool compare(const Symbol& v1, const Symbol& v2) const;
};

class Greater_than_or_equal_to_predicate : public Binary_predicate {
public:
	Greater_than_or_equal_to_predicate(const Symbol& in_argument1, const Symbol& in_argument2) :
		Binary_predicate(in_argument1, in_argument2) {}
protected:
	virtual bool compare(const Symbol& v1, const Symbol& v2) const;
};

class Less_than_predicate : public Binary_predicate {
public:
	Less_than_predicate(const Symbol& in_argument1, const Symbol& in_argument2) :
		Binary_predicate(in_argument1, in_argument2) {}
protected:
	virtual bool compare(const Symbol& v1, const Symbol& v2) const;
};

class Less_than_or_equal_to_predicate : public Binary_predicate {
public:
	Less_than_or_equal_to_predicate(const Symbol& in_argument1, const Symbol& in_argument2) :
		Binary_predicate(in_argument1, in_argument2) {}
protected:
	virtual bool compare(const Symbol& v1, const Symbol& v2) const;
};


// Unique_predicate has a pair of variable names, and returns the binding sets such that
// the variables have unique values. This predicate can be used to avoid certain trivial
// multiple instantiations.
// For example, if the variables are ?x and ?y,
// and the binding sets are (?x a ?y b), (?x b ?y a), (?x c ?y d), (?x a ?y e)
// the results will be (?x a ?y b), (?x c ?y d), (?x a ?y e)
// The second set is not included because it duplicates the values of ?x and ?y.
// The fourth set is included because ?y has a different value than ?x.
// Note that the first set is always included.
// The two variables should have different names, and should both be in all binding sets.

class Unique_predicate : public Binary_predicate {
public:
	Unique_predicate(const Symbol& in_variable1, const Symbol& in_variable2) :
		Binary_predicate(in_variable1,in_variable2)
		{}
	virtual Binding_set_list_t apply(const Binding_set_list_t& input_bsl) const;
protected:
	virtual bool compare(const Symbol& v1, const Symbol& v2) const;
};




} // end namespace

#endif
