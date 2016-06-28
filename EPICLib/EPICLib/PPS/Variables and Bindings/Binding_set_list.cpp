
#include "Binding_pair.h"
#include "Binding_set.h"
#include "Binding_set_list.h"

#include <iostream>
	using std::ostream; using std::cerr; using std::endl;
#include <algorithm>
using std::size_t;

namespace Parsimonious_Production_System {
using ::operator<<;	// bring global declarations into scope



// check to see if a Binding_set_list legal - 
// each binding set should be legal
// they should all have the same length
// there should be no duplicated binding_sets
bool Binding_set_list::is_legal() const
{
	bool bad_flag = false;
	if(binding_set_list.size() == 0)
		return true;
	// get the number of binding pairs in the first binding_set
	size_t first_size = binding_set_list.begin()->size();
	
	Binding_set_list_t::const_iterator probe_it, scan_it;
	for (probe_it = binding_set_list.begin(); probe_it != binding_set_list.end(); probe_it++) {
		// check each binding pair
		if (!probe_it->is_legal()) {
			bad_flag = true;
			}
		// check to see if the size is different from the first one
		if (first_size != probe_it->size()) {
			cerr << "illegal different Binding_set lengths" << endl;
			bad_flag = true;
			}
		// check for a duplicate
		scan_it = probe_it;
		scan_it++;
		while(scan_it != binding_set_list.end()) {
			if (*probe_it == *scan_it) {
				cerr << "illegal duplicate Binding_set" << endl;
				bad_flag = true;
				}
			else
				scan_it++;
			}
		}
	if(bad_flag) {
		cerr << "in Binding_set_list:\n";
		cerr << *this << endl;
		return false;
		}
	else
		return true;
}



// add a binding set by simply putting it at the end of the list,
// but only if not already present - checks for present first!
// return true if binding set list changed, false if not

bool Binding_set_list::add_binding_set(const Binding_set& bs)
{
	if (binding_set_list.size() > 0 && 
		(find(binding_set_list.begin(), binding_set_list.end(), bs) != binding_set_list.end()) )
		return false;	// already present, no change
	
	binding_set_list.push_back(bs);
	return true;	// binding_set_list changed
}

// remove a binding set by finding it and removing it
// add_binding_set ensured each binding set is unique, so
// removal here does not have to check for duplication.
bool Binding_set_list::remove_binding_set(const Binding_set& bs)
{
	// if list is empty, no removal, no change
	if (binding_set_list.size() == 0)
		return false;
	// search nonempty list for binding set
	Binding_set_list_t::iterator pos = find(binding_set_list.begin(), binding_set_list.end(), bs);
	if (pos == binding_set_list.end())
		return false;	// not present, no change
	
	binding_set_list.erase(pos);
	return true;	// changed
}

// add all of the binding sets, return true if a change was made
bool Binding_set_list::add_binding_sets(const Binding_set_list& bsl)
{
	bool changed = false;
	Binding_set_list_t::const_iterator it;
	for(it = bsl.binding_set_list.begin(); it != bsl.binding_set_list.end(); it++)
		changed = add_binding_set(*it) || changed; 	// beware short circuit evaluation!
	return changed;
}

// remove all of the binding sets, return true if a change was made
bool Binding_set_list::remove_binding_sets(const Binding_set_list& bsl)
{
	bool changed = false;
	Binding_set_list_t::const_iterator it;
	for(it = bsl.binding_set_list.begin(); it != bsl.binding_set_list.end(); it++)
		changed = remove_binding_set(*it) || changed; 	// beware short circuit evaluation!
	return changed;
}






/*
// figure out explicit instantiation syntax for these

template <class MT> 
bool add_if_match_source (MT match_type,
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
			
template <class MT> 
bool add_if_not_match_source (MT match_type,
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
			if (!result) {
				binding_set_list.push_back(result);
				changes.add_binding_set(result);
				changed = true;
				}
			}
	return changed;
}
			
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

// explicit instantiations

template bool Binding_set_list::add_if_match_source<Combine_full_overlap_binding_sets> (Combine_full_overlap_binding_sets);
template bool Binding_set_list::add_if_match_source<Combine_partial_overlap_binding_sets> (Combine_partial_overlap_binding_sets);
template bool Binding_set_list::add_if_match_source<Combine_no_overlap_binding_sets> (Combine_no_overlap_binding_sets);

template bool Binding_set_list::add_if_not_match_source<Combine_full_overlap_binding_sets> (Combine_full_overlap_binding_sets);
template bool Binding_set_list::add_if_not_match_source<Combine_partial_overlap_binding_sets> (Combine_partial_overlap_binding_sets);
template bool Binding_set_list::add_if_not_match_source<Combine_no_overlap_binding_sets> (Combine_no_overlap_binding_sets);

template bool Binding_set_list::remove_if_match<Combine_full_overlap_binding_sets> (Combine_full_overlap_binding_sets);
template bool Binding_set_list::remove_if_match<Combine_partial_overlap_binding_sets> (Combine_partial_overlap_binding_sets);
template bool Binding_set_list::remove_if_match<Combine_no_overlap_binding_sets> (Combine_no_overlap_binding_sets);
*/


// return a list of binding sets that have wildcards removed
Binding_set_list Binding_set_list::remove_wildcards() const
{
	Binding_set_list result;
		Binding_set_list_t::const_iterator it;
		for (it = binding_set_list.begin(); it != binding_set_list.end(); it++) 
			result.add_binding_set((*it).remove_wildcards());
	return result;
}



// output each set in the list
ostream& operator<< (ostream& os, const Binding_set_list_t& bsl)
{
	if (bsl.empty())
		os << "-empty-";
	else {
		Binding_set_list_t::const_iterator it;
		for (it = bsl.begin(); it != bsl.end(); it++) {
//			if (it != bsl.begin())	// if more than one, start a new line
//				os << endl;
//			os << (*it);
			if (it != bsl.begin())	// if more than one, put a space before
				os << ' ';
			os << (*it);

			}
		}
	return os;
}

// output each set in the list in the object
ostream& operator<< (ostream& os, const Binding_set_list& bsl)
{
	os << bsl.binding_set_list;
	return os;
/*	if (bsl.binding_set_list.empty())
		os << "-empty-";
	else {
		list<Binding_set>::const_iterator it;
		for (it = bsl.binding_set_list.begin(); it != bsl.binding_set_list.end(); it++) {
			if (it != bsl.binding_set_list.begin())	// if more than one, start a new line
				os << endl;
			os << (*it);
			}
		}
	return os;
*/
}

} // end namespace
