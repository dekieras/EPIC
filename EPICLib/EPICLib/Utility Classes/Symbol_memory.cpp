#include "Symbol_memory.h"
#include "Utility_templates.h"
#include "Assert_throw.h"
#include <cstddef>
#include <algorithm>

using std::vector;
using std::set;
using std::strcpy;
using std::size_t;

Symbol_memory * Symbol_memory::Symbol_memory_ptr = 0;

// A Meyers singleton would get automatically destructed, which is not
// a good idea because the individual Symbols might get destructed afterwards
// everything should get freed except for the final empty container
Symbol_memory& Symbol_memory::get_instance()
{
	if(!Symbol_memory_ptr)
		Symbol_memory_ptr = new Symbol_memory;
	return *Symbol_memory_ptr;
}


// deallocate the memory for the c-strings and the vectors
void Symbol_memory::clear()
{
	for_each(vec_rep_ptr_set.begin(), vec_rep_ptr_set.end(), Delete());
	vec_rep_ptr_set.clear();
	
	for(Str_rep_ptr_set_t::iterator it = str_rep_ptr_set.begin(); it != str_rep_ptr_set.end(); it++) {
		Symbol_memory_Str_rep * p = *it;
		delete[] p->cstr;
		delete p;
		}
	// deallocate the set contents
	str_rep_ptr_set.clear();
}

// return the pointer to the Symbol_memory_Str_rep if the cstring is already present, 
// add the Symbol_memory_Str_rep if it isn't, and return the pointer
// len is supplied because it is already computed - to save time
Symbol_memory_Str_rep * Symbol_memory::find_or_insert(const char * p, int len)
{
	Symbol_memory_Str_rep sr(0, p, 0);
	Str_rep_ptr_set_t::iterator it = str_rep_ptr_set.find(&sr);
	if (it == str_rep_ptr_set.end()) {
		char * cp = new char[len + 1];
		strcpy(cp, p);
		Symbol_memory_Str_rep * str_rep_ptr = new Symbol_memory_Str_rep(1, cp, len);
		str_rep_ptr_set.insert(str_rep_ptr);
		return str_rep_ptr;
		}
	else {
		Symbol_memory_Str_rep * str_rep_ptr = *it;
		// increment the count
		(str_rep_ptr->count)++;
		return str_rep_ptr;
		}
}

Symbol_memory_Vec_rep * Symbol_memory::find_or_insert(bool single_, const std::vector<GU::Point>& v_)
{
	Symbol_memory_Vec_rep vr(0, single_, v_);
	Vec_rep_ptr_set_t::iterator it = vec_rep_ptr_set.find(&vr);
	if (it == vec_rep_ptr_set.end()) {
		Symbol_memory_Vec_rep * vec_rep_ptr = new Symbol_memory_Vec_rep(1, single_, v_);
		vec_rep_ptr_set.insert(vec_rep_ptr);
		return vec_rep_ptr;
		}
	else {
		Symbol_memory_Vec_rep * vec_rep_ptr = *it;
		// increment the count
		(vec_rep_ptr->count)++;
		return vec_rep_ptr;
		}
}

// decrement the reference count; remove it and free the memory if it was the last use
void Symbol_memory::remove_if_last(Symbol_memory_Str_rep * str_rep_ptr)
{
	(str_rep_ptr->count)--;
	if(str_rep_ptr->count == 0) {
		// remove it from the set
		Str_rep_ptr_set_t::iterator it = str_rep_ptr_set.find(str_rep_ptr);
		Assert(it != str_rep_ptr_set.end());
		str_rep_ptr_set.erase(it);
		delete str_rep_ptr;
		}
}

// decrement the reference count; remove it and free the memory if it was the last use
void Symbol_memory::remove_if_last(Symbol_memory_Vec_rep * vec_rep_ptr)
{
	(vec_rep_ptr->count)--;
	if(vec_rep_ptr->count == 0) {
		// remove it from the set
		Vec_rep_ptr_set_t::iterator it = vec_rep_ptr_set.find(vec_rep_ptr);
		Assert(it != vec_rep_ptr_set.end());
		vec_rep_ptr_set.erase(it);
		delete vec_rep_ptr;
		}
}

