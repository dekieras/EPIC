#include "Overlap.h"
#include "Variables.h"
#include "Binding_set.h"

#include <iostream>
	using std::ostream; using std::cerr; using std::endl; 

namespace Parsimonious_Production_System {

// These function object classes operate on binding sets and variable lists
// to implement the definitions of the levels of overlap


// combine Variable_lists under full overlap criterion, return a combined Variable_list
// Required that input lists have exact same variable names; empty list otherwise
// Assumes variable lists are in the sorted state
Variable_list Full_Overlap::operator() (const Variable_list& lhs, const Variable_list& rhs)
{
	Variable_list result;

	// if not the same length, result is empty
	if(lhs.var_names.size() != rhs.var_names.size())
    	return Variable_list();	// return empty binding set

	Symbol_list_t::const_iterator lhs_it = lhs.var_names.begin();
	Symbol_list_t::const_iterator rhs_it = rhs.var_names.begin();

    while (lhs_it != lhs.var_names.end() && rhs_it != rhs.var_names.end()) {
    	// zip through lists comparing values for matching var_names.
		// variable names should be the same here     	
    	if((*lhs_it) != (*rhs_it))
    		return Variable_list();	// return empty variable list
        ++lhs_it;
        ++rhs_it;
		}
	// return a copy of one of the inputs
	return lhs;
}


// combine Binding_sets under full overlap criterion, return a combined Binding_set
Binding_set Full_Overlap::operator() (const Binding_set& lhs, const Binding_set& rhs)
{
	Binding_set result;

	// if not the same length, result is nothing
	if(lhs.binding_pair_list.size() != rhs.binding_pair_list.size())
    	return Binding_set();	// return empty binding set

	Binding_pair_list_t::const_iterator lhs_it = lhs.binding_pair_list.begin();
	Binding_pair_list_t::const_iterator rhs_it = rhs.binding_pair_list.begin();

    while (lhs_it != lhs.binding_pair_list.end() && rhs_it != rhs.binding_pair_list.end()) {
    	// zip through lists comparing values for matching var_names.
		// variable names should be the same here, so compare values      	
    	// Assert((*lhs_it).get_var_name() == (*rhs_it).get_var_name());
    	if((*lhs_it).get_var_name() != (*rhs_it).get_var_name())
    		return Binding_set();	// return empty binding set

    	if ((*lhs_it).get_var_value() == (*rhs_it).get_var_value()) {
        	// add to result, go to next
            result.binding_pair_list.push_back(*lhs_it);
            ++lhs_it;
            ++rhs_it;
            }
        else {
			// if values do not match, result is an empty list
    		return Binding_set();	// return empty binding set
			}
		}
	return result;
}

// return true if 
// lists are both non_empty or
// lists are same length and
// variable names are the same

bool Full_Overlap::is_legal(const Binding_set& lhs, const Binding_set& rhs)
{
	Binding_pair_list_t::const_iterator lhs_it = lhs.binding_pair_list.begin();
	Binding_pair_list_t::const_iterator rhs_it = rhs.binding_pair_list.begin();

    while (lhs_it != lhs.binding_pair_list.end() && rhs_it != rhs.binding_pair_list.end()) {
    	if((*lhs_it).get_var_name() != (*rhs_it).get_var_name())
    		break;
    	else {
            ++lhs_it;
            ++rhs_it;
    		}
		}
	// if went all the way through the list, and the lists were at least one in length, it was good
	if (lhs_it == lhs.binding_pair_list.end() && rhs_it == rhs.binding_pair_list.end() &&
		lhs.size() > 1 && rhs.size() > 1)
		return true;
		
	cerr << "Illegal Binding_sets for full overlap comparison:" << endl;
	cerr << lhs << endl;
	cerr << rhs << endl;
	return false;
}

// combine Variable_lists under partial overlap criterion, return a combined Variable_list
// Required that input lists be different in at least one variable name,
// and the same in at least one variable name; empty list otherwise
// Assumes variable lists are in the sorted state
Variable_list Partial_Overlap::operator() (const Variable_list& lhs, const Variable_list& rhs)
{

	bool same_name = false;
	bool diff_name = false;
	
	Variable_list result;
	Symbol_list_t::const_iterator lhs_it = lhs.var_names.begin();
	Symbol_list_t::const_iterator rhs_it = rhs.var_names.begin();

//	must go all the way through both lists, even if one is shorter than the other.
    while (lhs_it != lhs.var_names.end() && rhs_it != rhs.var_names.end()) {
    	// zip through lists until found matching var_names.
        if ((*lhs_it) < (*rhs_it)) {
        	// a variable in the first list doesn't have a match in the second list
        	// add to output, go to next
             result.add(*lhs_it);
            ++lhs_it;
            diff_name = true;
            }
        else if ((*rhs_it) < (*lhs_it)) {
        	// a variable in the second list doesn't have a match in the first list
        	// add to output, go to next
            result.add(*rhs_it);
            ++rhs_it;
            diff_name = true;
            }
        else {
			// variable names are the same here	
        	// add to output, go to next
            result.add(*lhs_it);
            ++lhs_it;
            ++rhs_it;
			same_name = true;
			}
		}
    // here if at least one of the lists has been scanned all the way
    // copy remaining names in
    while (lhs_it != lhs.var_names.end()) {
		result.add(*lhs_it);
		++lhs_it;
        diff_name = true;
		}
    while (rhs_it != rhs.var_names.end()) {
		result.add(*rhs_it);
		++rhs_it;
        diff_name = true;
		}

	if (same_name && diff_name)
	    return result;

	return Variable_list();
}



// function object class operator definition
// combine under partial overlap criterion, return a combined binding set
Binding_set Partial_Overlap::operator() (const Binding_set& lhs, const Binding_set& rhs)
{
	Binding_set result;
	Binding_pair_list_t::const_iterator lhs_it = lhs.binding_pair_list.begin();
	Binding_pair_list_t::const_iterator rhs_it = rhs.binding_pair_list.begin();
	
//	must go all the way through both lists, even if one is shorter than the other.
    while (lhs_it != lhs.binding_pair_list.end() && rhs_it != rhs.binding_pair_list.end())
    	// zip through lists until found matching var_names.
        if ((*lhs_it).get_var_name() < (*rhs_it).get_var_name()) {
        	// a variable in the first list doesn't have a match in the second list
        	//cout << (*first) << endl;
        	// add to output, go to next
             result.binding_pair_list.push_back(*lhs_it);
            ++lhs_it;
            }
        else if ((*rhs_it).get_var_name() < (*lhs_it).get_var_name()) {
        	// a variable in the second list doesn't have a match in the first list
         	//cout << (*second) << endl;
        	// add to output, go to next
            result.binding_pair_list.push_back(*rhs_it);
            ++rhs_it;
            }
        else
        {
        	//cout << (*first) << " - " << (*second) << endl;
			// variable names are the same here, so compare values      	
            if ((*lhs_it).get_var_value() == (*rhs_it).get_var_value()) {
        		// add to output, go to next
            	result.binding_pair_list.push_back(*lhs_it);
            	++lhs_it;
            	++rhs_it;
            	}
            else {
            	// result is an empty list
            	result.binding_pair_list.clear();
            	return result;
            	}
           }
    // here if at least one of the lists has been scanned all the way
    // copy remaining binding_pairs in
    while (lhs_it != lhs.binding_pair_list.end()) {
    	//cout << (*first) << endl;
		result.binding_pair_list.push_back(*lhs_it);
		++lhs_it;
		}
    while (rhs_it != rhs.binding_pair_list.end()) {
    	//cout << (*second) << endl;
		result.binding_pair_list.push_back(*rhs_it);
		++rhs_it;
		}
		    
    return result;
}

// return true if 
// lists are both non-empty and
// there is at least one variable name in common
// and at least one variable name that is different

bool Partial_Overlap::is_legal(const Binding_set& lhs, const Binding_set& rhs)
{
	bool same_name = false;
	bool diff_name = false;
	
	Binding_pair_list_t::const_iterator lhs_it;
	Binding_pair_list_t::const_iterator rhs_it;

	for(lhs_it = lhs.binding_pair_list.begin(); lhs_it != lhs.binding_pair_list.end(); lhs_it++)
		for(rhs_it = rhs.binding_pair_list.begin(); rhs_it != rhs.binding_pair_list.end(); rhs_it++) {
			if ((*lhs_it).get_var_name() == (*rhs_it).get_var_name()) 
				same_name = true;
			else
				diff_name = true;
			}

	if (same_name && diff_name)	// won't get set unless at least one in one list, two in the other
		return true;
		
	cerr << "Illegal Binding_sets for partial overlap comparison:" << endl;
	cerr << lhs << endl;
	cerr << rhs << endl;
	return false;
}



// combine Variable_lists under no overlap criterion, return a combined Variable_list.
// Required that input lists have no variable names in common; empty list otherwise
// Assumes variable lists are in the sorted state
Variable_list No_Overlap::operator() (const Variable_list& lhs, const Variable_list& rhs)
{	
	Variable_list result;
	Symbol_list_t::const_iterator lhs_it = lhs.var_names.begin();
	Symbol_list_t::const_iterator rhs_it = rhs.var_names.begin();

//	must go all the way through both lists, even if one is shorter than the other.
    while (lhs_it != lhs.var_names.end() && rhs_it != rhs.var_names.end()) {
    	// zip through lists until found matching var_names.
        if ((*lhs_it) < (*rhs_it)) {
        	// add to output, go to next
             result.add(*lhs_it);
            ++lhs_it;
            }
        else if ((*rhs_it) < (*lhs_it)) {
        	// add to output, go to next
            result.add(*rhs_it);
            ++rhs_it;
            }
        else {
			// variable names are the same here	- not supposed to be
			return Variable_list();
			}
		}
    // here if at least one of the lists has been scanned all the way
    // copy remaining names in
    while (lhs_it != lhs.var_names.end()) {
		result.add(*lhs_it);
		++lhs_it;
		}
    while (rhs_it != rhs.var_names.end()) {
		result.add(*rhs_it);
		++rhs_it;
		}

	return result;
}



Binding_set No_Overlap::operator() (const Binding_set& lhs, const Binding_set& rhs)
{
	// no check for overlapping pairs - takes too long
	Binding_set result;
	result.binding_pair_list = lhs.binding_pair_list;	// copy first list in
	Binding_pair_list_t rhs_list = rhs.binding_pair_list; // copy the other list
	result.binding_pair_list.merge(rhs_list);	

    return result;
}

// return true if 
// both lists are at least one in length and
// there are no variable names in common

bool No_Overlap::is_legal(const Binding_set& lhs, const Binding_set& rhs)
{
	bool same_name = false;
	
	Binding_pair_list_t::const_iterator lhs_it;
	Binding_pair_list_t::const_iterator rhs_it = rhs.binding_pair_list.begin();

	for(lhs_it = lhs.binding_pair_list.begin(); lhs_it != lhs.binding_pair_list.end(); lhs_it++)
		for(rhs_it = rhs.binding_pair_list.begin(); rhs_it != rhs.binding_pair_list.end(); rhs_it++) {
			if ((*lhs_it).get_var_name() == (*rhs_it).get_var_name())
				same_name = true;
			}

	if (!same_name && lhs.size() > 0 && rhs.size() > 0)
		return true;
		
	cerr << "Illegal Binding_sets for no overlap comparison:" << endl;
	cerr << lhs << endl;
	cerr << rhs << endl;
	return false;
}
} // end namespace

