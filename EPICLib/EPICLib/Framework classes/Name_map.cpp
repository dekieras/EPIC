#include "Name_map.h"
#include "Epic_exception.h"

#include <sstream>


using std::ostringstream;
using std::make_pair;


/* Exception classes */
// this is a user-interpretable error
class Duplicate_physical_name : public Epic_exception {
public:
Duplicate_physical_name(const Symbol& physical_name)
	{
		ostringstream oss;
		oss << "Attempt to store physical name"
			<< physical_name 
			<< " in name map, but it is already present";
		msg = oss.str();
	}
};

// this is an internal error
class Duplicate_psychological_name : public Epic_internal_error {
public:
Duplicate_psychological_name(const Symbol& psychological_name)
	{
		ostringstream oss;
		oss << "Attempt to store psychological name"
			<< psychological_name 
			<< " in name map, but it is already present";
		msg = oss.str();
	}
};

class Duplicate_names : public Epic_internal_error {
public:
Duplicate_names(const Symbol& physical_name, const Symbol& psychological_name)
	{
		ostringstream oss;
		oss << "Attempt to store "
			<< physical_name << '/' << psychological_name
			<< " in name map, but both names are already present";
		msg = oss.str();
	}
};

class Unknown_name : public Epic_internal_error {
public:
Unknown_name(const Symbol& obj_name)
	{
		ostringstream oss;
		oss << "Attempt to access name map using unknown name: "
			<< obj_name;
		msg = oss.str();
	}
};

class Unknown_name_in_use : public Epic_internal_error {
public:
Unknown_name_in_use(const Symbol& obj_name)
	{
		ostringstream oss;
		oss << "Attempt to access name map name in use using unknown name: "
			<< obj_name;
		msg = oss.str();
	}
};



// *** name map functions ***
// we always add both names at the same time, never separately
// dk 070807 - rethinking this - when object disappears physically, we replace the
// physical name with a special value - so below is superseded.
// any time a new object appears, it can have the same physical name as a now-disappeared object
// had, but it must be the only one of its name at this time.
// dk 6/6/03
// it appears that duplicating a physical name can be a great convenience, because
// the device does not have to create new names for objects all the time
// just because the psychological name might linger for awhile ...
// however, if the physical name is duplicated, we must remove the old psychological
// name - but then later, the object will disappear - what happens then?
// Solution: if duplicated physical name, remove the old psychological name,
// and enter the new name pair;
// but if removing a psychological name and it is not present, this is not an error.
void Name_map::add_names(const Symbol& physical_name, const Symbol& psychological_name)
{
	Symbol_map_t::iterator psyc_it = psychological_names.find(physical_name);
	Symbol_map_t::iterator phys_it = physical_names.find(psychological_name);
	if(psyc_it != psychological_names.end() && phys_it != physical_names.end()) 
		throw Duplicate_names(physical_name, psychological_name);
	if(phys_it != physical_names.end()) 
		throw Duplicate_psychological_name(psychological_name);
	if(psyc_it != psychological_names.end()) {
		throw Duplicate_physical_name(physical_name);
//		remove_names_with_physical_name(physical_name);
		}

/*	// if either name is duplicated, we have a problem - efficiency is a problem here!
	if(psychological_names.find(physical_name) != psychological_names.end() &&
			physical_names.find(psychological_name) != physical_names.end()) 
		throw Duplicate_names(physical_name, psychological_name);
	if(physical_names.find(psychological_name) != physical_names.end()) 
		throw Duplicate_psychological_name(psychological_name);
	if(psychological_names.find(physical_name) != psychological_names.end()) {
//		throw Duplicate_physical_name(physical_name);
		remove_names_with_physical_name(physical_name);
		}
*/
	psychological_names.insert(make_pair(physical_name, psychological_name));
	physical_names.insert(make_pair(psychological_name, physical_name));

/*	psychological_names[physical_name] = psychological_name;
	physical_names[psychological_name] = physical_name;
*/
}

bool Name_map::is_psychological_name_present(const Symbol& psychological_name) const
{
	Symbol_map_t::const_iterator it = physical_names.find(psychological_name);
	return (it != physical_names.end());
}

bool Name_map::is_physical_name_present(const Symbol& physical_name) const
{
	Symbol_map_t::const_iterator it = psychological_names.find(physical_name);
	return (it != psychological_names.end());
}

Symbol Name_map::get_psychological_name(const Symbol& physical_name) const
{
	Symbol_map_t::const_iterator it = psychological_names.find(physical_name);
	if(it == psychological_names.end())
		throw Unknown_name(physical_name);
	return it->second;
}

Symbol Name_map::get_physical_name(const Symbol& psychological_name) const
{
	Symbol_map_t::const_iterator it = physical_names.find(psychological_name);
	if(it == physical_names.end())
		throw Unknown_name(psychological_name);
	return it->second;
}

// replace the physical name for the psychologically-named object, do not check for duplicate physical name values,
// and overwrite any previous physical_name value.
void Name_map::replace_physical_name_for_psychological_name(const Symbol& physical_name, const Symbol& psychological_name)
{
	// given a psychological_name, find the physical_name for it
	Symbol_map_t::iterator it = physical_names.find(psychological_name);
	if(it == physical_names.end()) {
		throw Unknown_name(psychological_name);
		}
	Symbol old_physical_name = it->second;
	it->second = physical_name;
//	physical_names.erase(it);
//	physical_names.insert(make_pair(psychological_name, physical_name));

	// now take the psychological name out of the physical name map
	it = psychological_names.find(old_physical_name);
	if(it == psychological_names.end())
		throw Unknown_name(old_physical_name);
	psychological_names.erase(it);	// the old physical name is no longer a key
	psychological_names[physical_name] = psychological_name;  // overwrite any previous value of the key
}

// we always remove both names, given one to be removed
// if the physical names do not contain the psychological name, that is OK - not an error ...
void Name_map::remove_names_with_psychological_name(const Symbol& psychological_name)
{
	// given a psychological_name, take it out of the physical_name
	Symbol_map_t::iterator it = physical_names.find(psychological_name);
	if(it == physical_names.end()) {
		throw Unknown_name(psychological_name);
		}
	// save the to-be eliminated physical name
	Symbol physical_name = it->second;
	physical_names.erase(it);
	// now take the psychological name out of the physical name map
	// no error if not present ...
	it = psychological_names.find(physical_name);
	if(it != psychological_names.end())
		psychological_names.erase(it);
//	if(it == psychological_names.end())
//		throw Unknown_name(physical_name);
//	psychological_names.erase(it);
}

// we always remove both names, given one to be removed
void Name_map::remove_names_with_physical_name(const Symbol& physical_name)
{
	// given a physical name, take it out of the psychological_name map
	Symbol_map_t::iterator it = psychological_names.find(physical_name);
	if(it == psychological_names.end())
		throw Unknown_name(physical_name);
	// save the to-be eliminated psychological name
	Symbol psychological_name = it->second;
	psychological_names.erase(it);
	// now take the psychological name out of the physical name map
	// no error if not present ...
	it = physical_names.find(psychological_name);
//	if(it == physical_names.end())
//		throw Unknown_name(psychological_name);
	if(it != physical_names.end())
		physical_names.erase(it);
}

// set or test state of whether a name is in use. The name must be present in the maps
void Name_map::set_physical_name_in_use(const Symbol& physical_name, bool state)
{
	Symbol_map_t::iterator it = psychological_names.find(physical_name);
	if(it == psychological_names.end())
		throw Unknown_name_in_use(physical_name);
	if(state)
		physical_names_in_use.insert(physical_name);
	else
		physical_names_in_use.erase(physical_name);
}

bool Name_map::is_physical_name_in_use(const Symbol& physical_name)
{
	return physical_names_in_use.find(physical_name) != physical_names_in_use.end();
}


void Name_map::clear()
{
	psychological_names.clear();
	physical_names.clear();
	physical_names_in_use.clear();
}

