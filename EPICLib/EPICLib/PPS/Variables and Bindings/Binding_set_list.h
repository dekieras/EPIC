#ifndef BINDING_SET_LIST_H
#define BINDING_SET_LIST_H

#include "Binding_pair.h"
#include "Binding_set.h"

#include <iosfwd>
#include <list>


namespace Parsimonious_Production_System {
using ::operator<<;	// bring global declarations into scope
using std::operator<<;

// output each set in the list on a line by itself
typedef std::list<Binding_set> Binding_set_list_t;
std::ostream& operator<< (std::ostream& os, const Binding_set_list_t& bsl);

class Binding_set_list;
// output each set in the list in the object on a line by itself
std::ostream& operator<< (std::ostream& os, const Binding_set_list& bsl);

class Binding_set_list {
public:
    Binding_set_list() {}   // default ctor to allow declaration of const empty object - dk 07/18/2012
	bool add_binding_set(const Binding_set& bs);
	bool remove_binding_set(const Binding_set& bs);
	bool add_binding_sets(const Binding_set_list& bsl);
	bool remove_binding_sets(const Binding_set_list& bsl);
	
	// return a list of binding sets that have wildcard binding pairs removed
	Binding_set_list remove_wildcards() const;

	void clear()
		{return binding_set_list.clear();}

	bool empty() const
		{return binding_set_list.empty();}
		
	bool is_legal() const;
	
	const Binding_set_list_t& get_binding_set_list() const
		{return binding_set_list;}

	friend std::ostream& operator<< (std::ostream& os, const Binding_set_list& bsl);
		
private:
	Binding_set_list_t binding_set_list;
	

public:

// The following function templates are member functions of the Binding_set_list class.

/*
template <class MT> 
bool add_if_match_source (MT match_type,
	const Binding_set_list& modification, const Binding_set_list& source, Binding_set_list& changes);

template <class MT> 
bool add_if_not_match_source (MT match_type,
	const Binding_set_list& modification, const Binding_set_list& source, Binding_set_list& changes);

template <class MT> 
bool remove_if_match (MT match_type,
	const Binding_set_list& modification, Binding_set_list& changes);
*/

// check whether this object's bindings are legal to compare with the other object's bindings
template <class MT> 
bool is_legal (MT match_type, const Binding_set_list& second) const
{
	Binding_set_list_t::const_iterator first_it;
	Binding_set_list_t::const_iterator second_it;
	bool result = true;
		
	for(first_it = binding_set_list.begin(); first_it != binding_set_list.end(); first_it++)
		for(second_it = second.binding_set_list.begin(); second_it != second.binding_set_list.end(); second_it++) {
			if(!match_type.is_legal(*first_it, *second_it))
				result = false;
			}
	return result;
}
			
// for each binding_set in modification, if it matches with an item in the source, combine them, 
// and add it to this binding set list and to the list of changes
template <class MT> 
bool add_combination_if_match_source (MT match_type,
	const Binding_set_list& modification, const Binding_set_list& source, Binding_set_list& changes)
{
	Binding_set_list_t::const_iterator mod_it;
	Binding_set_list_t::const_iterator source_it;
	Binding_set result;	
	bool changed = false;	// flag to return on whether there has been a change in the bindings
	changes.clear();		// to ensure output is well defined
		
	for(mod_it = modification.binding_set_list.begin(); mod_it != modification.binding_set_list.end(); mod_it++)
		for(source_it = source.binding_set_list.begin(); source_it != source.binding_set_list.end(); source_it++) {
			result = match_type(*mod_it, *source_it);
			if (result) {
				binding_set_list.push_back(result);
				changes.add_binding_set(result);
				changed = true;
				}
			}
	return changed;
}
			
// for each binding_set in modification, if it matches with an item in the source, 
// add it without combining with the source to this binding set list and to the list of changes
template <class MT> 
bool add_mod_if_match_source (MT match_type,
	const Binding_set_list& modification, const Binding_set_list& source, Binding_set_list& changes)
{
	Binding_set_list_t::const_iterator mod_it;
	Binding_set_list_t::const_iterator source_it;
	Binding_set result;	
	bool changed = false;	// flag to return on whether there has been a change in the bindings
	changes.clear();		// to ensure output is well defined
		
	for(mod_it = modification.binding_set_list.begin(); mod_it != modification.binding_set_list.end(); mod_it++)
		for(source_it = source.binding_set_list.begin(); source_it != source.binding_set_list.end(); source_it++) {
			result = match_type(*mod_it, *source_it);
			if (result) {
				binding_set_list.push_back(*mod_it);
				changes.add_binding_set(*mod_it);
				changed = true;
				}
			}
	return changed;
}
			
// for each binding_set in modification, if it does not match with some item in the source, add it to this 
// binding set list and to the list of changes. Note that only the modification binding_set
// is added to this list and to the changes.
template <class MT> 
bool add_mod_if_not_match_source (MT match_type,
	const Binding_set_list& modification, const Binding_set_list& source, Binding_set_list& changes)
{
	Binding_set_list_t::const_iterator mod_it;
	Binding_set_list_t::const_iterator source_it;
	Binding_set result;	
	bool changed = false;	// flag to return on whether there has been a change in the bindings
	changes.clear();		// to ensure output is well defined
		
	for(mod_it = modification.binding_set_list.begin(); mod_it != modification.binding_set_list.end(); mod_it++) {
		bool found = false;
		for(source_it = source.binding_set_list.begin(); source_it != source.binding_set_list.end(); source_it++) {
			result = match_type(*mod_it, *source_it);
			if (result) {
				found = true;
				break;
				}
			}
		if (!found) {
			binding_set_list.push_back(*mod_it);
			changes.add_binding_set(*mod_it);
			changed = true;
			}
		}
	return changed;
}

// for each modification binding set, remove it from this list if it matches
// the removed items are added to changes.
template <class MT> 
bool remove_if_match (MT match_type,
	const Binding_set_list& modification, Binding_set_list& changes)
{
	Binding_set_list_t::const_iterator mod_it;
	Binding_set_list_t::iterator this_it;
	Binding_set result;
	bool changed = false;
	changes.clear();		// to ensure output is well defined
	
	for(mod_it = modification.binding_set_list.begin(); mod_it != modification.binding_set_list.end(); mod_it++) {
		this_it = binding_set_list.begin();
		while (this_it != binding_set_list.end()) {
			result = match_type(*mod_it, *this_it);
			if(result) {
				changes.add_binding_set(*this_it);
				binding_set_list.erase(this_it++);	// erase using pre-increment value, then continue with increment
				changed = true;
				}
			else
				this_it++;
			}
		}
	return changed;
}


}; // end class


} // end namespace

#endif

