#ifndef SYMBOL_MEMORY_H
#define SYMBOL_MEMORY_H

#include "Point.h"
#include <vector>
#include <cstring>
#include <set>

namespace GU = Geometry_Utilities;

// A class for keeping reference-counted strings
class Symbol_memory_Str_rep {
public:
	Symbol_memory_Str_rep(long count_, const char * cstr_, int cstr_len_) :
		count(count_), cstr(cstr_), cstr_len(cstr_len_)
		{}
	friend class Symbol;
	friend class Symbol_memory;
	friend struct Symbol_memory_less_Str_rep;
private:
	long count;
	const char * cstr;
	size_t cstr_len;
};

// function object class for ordering Str_reps
struct Symbol_memory_less_Str_rep {
	bool operator() (const Symbol_memory_Str_rep * lhs, const Symbol_memory_Str_rep * rhs) const
		{
			return (std::strcmp(lhs->cstr, rhs->cstr) < 0);
		}
};

// A class for keeping reference-counted vectors of Points
class Symbol_memory_Vec_rep {
public:
	Symbol_memory_Vec_rep(long count_, bool single_, const std::vector<GU::Point>& v_) :
		count(count_), single(single_), vec(v_)
		{}
	friend class Symbol;
	friend class Symbol_memory;
	friend struct Symbol_memory_less_Vec_rep;
private:
	long count;
	bool single;	// true if only a single value (not a pair) is stored
	const std::vector<GU::Point> vec;
};

// function object class for ordering Vec_reps
// single value is less than non-single
struct Symbol_memory_less_Vec_rep {
	bool operator() (const Symbol_memory_Vec_rep * lhs, const Symbol_memory_Vec_rep * rhs) const
		{
			if(lhs->single == rhs->single)
				return (lhs->vec < rhs->vec);
			else if (lhs->single && !(rhs->single))
				return true;
			else
				return false;
		}
};


// This singleton class encapsulates a container for Str_reps and Vec_reps
// It is used by the Symbol class, hence most of the members are private
class Symbol_memory {
public:
	static Symbol_memory& get_instance();

	~Symbol_memory()
		{clear();}

	friend class Symbol;

private:
	static Symbol_memory * Symbol_memory_ptr;
	// privatize the members for this singleton class
	Symbol_memory()
		{}
	Symbol_memory(const Symbol_memory&);
	Symbol_memory& operator= (const Symbol_memory&);
	
	// deallocate all of the memory.
	void clear();
		
	// return the pointer to the Symbol_memory_Str_rep if the cstring is already present, 
	// add the Symbol_memory_Str_rep if it isn't, and return the pointer
	// len is supplied because it is already computed - to save time
	Symbol_memory_Str_rep * find_or_insert(const char * p, int len);
	
	// decrement reference count for pointed-to string, and deallocate it if no
	// longer in use
	void remove_if_last(Symbol_memory_Str_rep *);

	typedef std::set<Symbol_memory_Str_rep *, Symbol_memory_less_Str_rep> Str_rep_ptr_set_t;
	Str_rep_ptr_set_t str_rep_ptr_set;
	std::size_t total_Str_rep_size;
	
	// Return the pointer to the Symbol_memory_Vec_rep if it is already present, 
	// add the Symbol_memory_Vec_rep if it isn't, and return the pointer
	Symbol_memory_Vec_rep * find_or_insert(bool single_, const std::vector<GU::Point>& v_);
			
	// decrement reference count for pointed-to string, and deallocate it if no
	// longer in use
	void remove_if_last(Symbol_memory_Vec_rep *);

	// Not clear that there  is any advantage to keeping a uniquified container of Vec_reps
	// that are shared between Symbols ...
	typedef std::set<Symbol_memory_Vec_rep *, Symbol_memory_less_Vec_rep> Vec_rep_ptr_set_t;
	Vec_rep_ptr_set_t vec_rep_ptr_set;
};

#endif

