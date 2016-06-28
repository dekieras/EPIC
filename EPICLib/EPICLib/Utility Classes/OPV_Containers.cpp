#include "OPV_Containers.h"
#include "Standard_utility_symbols.h"

#include "Output_tee.h"

#include <iostream>


using std::ostream;	using std::cout;	using std::cerr;	using std::endl;
using std::map;	using std::less;	using std::pair;	using std::make_pair;
using std::list;


/* PV_Container members */

void PV_Container::store (const Symbol& property_name, const Symbol& property_value)
{
	// if property_value is Symbol(), the default value, erase the property name
	if(property_value == Symbol())
		p_v_con.erase(property_name);
	else
		p_v_con[property_name] = property_value;
}

void PV_Container::erase (const Symbol& property_name)
{
	p_v_con.erase(property_name);
}

Symbol PV_Container::retrieve (const Symbol& property_name) const
{
	p_v_con_t::const_iterator it = p_v_con.find(property_name);
	if (it == p_v_con.end() )
		return Symbol();
	else
		return (*it).second;
}

void PV_Container::get_pv_lists (Symbol_list_t & prop_names, Symbol_list_t& prop_values) const
{
	p_v_con_t::const_iterator it;
	for (it = p_v_con.begin(); it != p_v_con.end(); ++it) {
		prop_names.push_back((*it).first);
		prop_values.push_back((*it).second);
		}
}

void PV_Container::display_contents(Output_tee& ot) const
{
	p_v_con_t::const_iterator it;
	for (it = p_v_con.begin(); it != p_v_con.end(); ++it) {
		 ot << (*it).first << ' ' << (*it).second << endl;
		}
}

ostream& operator<< (ostream& os, const PV_Container& pvc)
{
	PV_Container::p_v_con_t::const_iterator it;
	for (it = pvc.p_v_con.begin(); it != pvc.p_v_con.end(); ++it) {
		 os << (*it).first << ' ' << (*it).second << endl;
		}
	return os;
}


/* OPV_Container members */

// create an object of name object_name in the container, with empty property list.
// return true if result is normal, false if object was already present, and do nothing otherwise - 
bool OPV_Container::create_object (const Symbol& object_name) 
{
	// see if there is an object in the container with that name
	o_p_v_con_t::iterator iter = o_p_v_con.find(object_name);
	// if none, put a new empty object in
	if (iter == o_p_v_con.end()) {
		o_p_v_con.insert(make_pair(object_name, PV_Container()));
		return true;
		}
	else {
		return false;
		}
}

// erase the object of name object_name from the container.
// if not present, no error
void OPV_Container::erase(const Symbol& object_name)
{
	o_p_v_con.erase(object_name);
}

// store value prop_value under prop_name for object object_name.
// If object does not already exist, it will be created.
// If the property is already there, the PV_Container will replace it with new value
void OPV_Container::store (const Symbol& object_name, const Symbol& prop_name , const Symbol& prop_value)
{
	// see if there is an object in the container with that name
	o_p_v_con_t::iterator obj_iter = o_p_v_con.find(object_name);
	// if none, put a new empty object in and set the iterator to it
	if (obj_iter == o_p_v_con.end()) {
		o_p_v_con.insert(make_pair(object_name, PV_Container(prop_name, prop_value)));
		}
	else
	// store the prop_name, prop_value at the iterator.
		((*obj_iter).second).store(prop_name, prop_value);
}

// store values prop_values under prop_names for object object_name.
// If object does not already exist, it should be created.
// the properties will be added to those already there
// If a property is already there, its value will be replaced with the new value
void OPV_Container::store (const Symbol& object_name, const Symbol_list_t& prop_names, const Symbol_list_t& prop_values)
{
	// see if there is an object in the container with that name
	o_p_v_con_t::iterator obj_iter = o_p_v_con.find(object_name);
	// if none, put a new empty object in and set the iterator to it
	if (obj_iter == o_p_v_con.end()) {
		o_p_v_con.insert(make_pair(object_name, PV_Container()));
		obj_iter = o_p_v_con.find(object_name);
		}

	// iterate through the lists and store each property value pair;
	Symbol_list_t::const_iterator prop_it;
	Symbol_list_t::const_iterator value_it;
	for (prop_it = prop_names.begin(), value_it = prop_values.begin();
		 prop_it != prop_names.end() && value_it != prop_values.end();
		 prop_it++, value_it++)
			// store the prop_name-prop_value pair
			((*obj_iter).second).store((*prop_it), (*value_it));
}


// erase prop name and its value for object object_name.
// If object or prop name does not exist, there is no error
void OPV_Container::erase (const Symbol& object_name, const Symbol& prop_name)
{
	// see if there is an object in the container with that name
	o_p_v_con_t::iterator obj_iter = o_p_v_con.find(object_name);
	// if none, just return
	if (obj_iter == o_p_v_con.end())
		return;
	// erase the prop_name at the iterator.
	((*obj_iter).second).erase(prop_name);
}


// return true if the object object_name exists in the container, false otherwise.
bool OPV_Container::is_present (const Symbol& object_name) const
{
	// see if there is an object in the container with that name
	o_p_v_con_t::const_iterator obj_iter = o_p_v_con.find(object_name);
	return((obj_iter != o_p_v_con.end()));
}

// retrieve the value under prop name for object object_name.
// If object or prop name does not exist, there is no error
// and the default value is returned.
Symbol OPV_Container::retrieve (const Symbol& object_name, const Symbol& prop_name) const
{
	// see if there is an object in the container with that name
	o_p_v_con_t::const_iterator obj_iter = o_p_v_con.find(object_name);
	// if none, return default value
	if (obj_iter == o_p_v_con.end())
		return Nil_c;
	// retrieve the value for prop_name at the iterator.
	return ((*obj_iter).second).retrieve(prop_name);
}


// find returns the name of the first object that has value prop_value of property prop_name
// iterate through the container and check the values.
Symbol OPV_Container::find (const Symbol& prop_name, const Symbol& prop_value) const
{
	o_p_v_con_t::const_iterator obj_iter;
	for (obj_iter = o_p_v_con.begin(); 
		 obj_iter != o_p_v_con.end();
		 ++obj_iter) {
		// check if the value matches, return object name if matches
		if (prop_value == ((*obj_iter).second).retrieve(prop_name))
			return (*obj_iter).first;
		}
	// not found, return absent;
	return Absent_c;
}


// this find returns the name of the first object that matches all of prop-name-value pairs
// iterate through the container and check the values.
Symbol OPV_Container::find (const Symbol_list_t& prop_names, const Symbol_list_t& prop_values) const
{

	o_p_v_con_t::const_iterator obj_iter;
	Symbol_list_t::const_iterator prop_name_it;
	Symbol_list_t::const_iterator prop_value_it;

	for (obj_iter = o_p_v_con.begin(); obj_iter != o_p_v_con.end(); ++obj_iter) {
		// iterate through the list of prop names and prop values
		for (prop_name_it = prop_names.begin(), prop_value_it = prop_values.begin();
			 prop_name_it != prop_names.end() && prop_value_it != prop_values.end();
			 ++prop_name_it, ++prop_value_it) {
				// if value does not match, break out of the loop;
				if (*prop_value_it != ((*obj_iter).second).retrieve(*prop_name_it))
					break;
				}
			// if got to end of lists, return the object
			if (prop_name_it == prop_names.end() && prop_value_it == prop_values.end())
				return (*obj_iter).first;
			// otherwise, try next object
		}
	// not found, return absent;
	return Absent_c;
}

// this find returns a list of the names of all objects that match all of prop-name-value pairs
// iterate through the container and check the values.
Symbol_list_t OPV_Container::find_all (const Symbol_list_t& prop_names, const Symbol_list_t& prop_values) const
{
	o_p_v_con_t::const_iterator obj_iter;
	Symbol_list_t::const_iterator prop_name_it;
	Symbol_list_t::const_iterator prop_value_it;
	Symbol_list_t found_objects;

	for (obj_iter = o_p_v_con.begin(); obj_iter != o_p_v_con.end(); ++obj_iter) {
		// iterate through the list of prop names and prop values
		for (prop_name_it = prop_names.begin(), prop_value_it = prop_values.begin();
			 prop_name_it != prop_names.end() && prop_value_it != prop_values.end();
			 ++prop_name_it, ++prop_value_it) {
				// if value does not match, break out of the loop;
				if (*prop_value_it != ((*obj_iter).second).retrieve(*prop_name_it))
					break;
				}
			// if got to end of lists, save the object
			if (prop_name_it == prop_names.end() && prop_value_it == prop_values.end())
				found_objects.push_back((*obj_iter).first);
			// otherwise, try next object
		}
	// return the list of found objects
	return found_objects;
}

// return a list with all of the object_names in it
Symbol_list_t OPV_Container::get_all_object_names() const
{
	Symbol_list_t found_objects;
	o_p_v_con_t::const_iterator obj_iter;
	for (obj_iter = o_p_v_con.begin(); obj_iter != o_p_v_con.end(); ++obj_iter) {
		found_objects.push_back(obj_iter->first);
		}
	return found_objects;
}
	

// fill the supplied lists with the property names and values for object object_name
// return false if object not found
bool OPV_Container::get_pv_lists (const Symbol& object_name, Symbol_list_t& prop_names, Symbol_list_t& prop_values) const
{
	// see if there is an object in the container with that name
	o_p_v_con_t::const_iterator obj_iter = o_p_v_con.find(object_name);
	// if none, return false
	if (obj_iter == o_p_v_con.end())
		return false;
	
	// have the object container fill the lists with the PV-pairs for the object
	((*obj_iter).second).get_pv_lists(prop_names, prop_values);
	return true;
}

void OPV_Container::display_contents(Output_tee& ot) const
{
	ot << "Number of objects: " << o_p_v_con.size() << endl;
	o_p_v_con_t::const_iterator obj_iter;
	for (obj_iter = o_p_v_con.begin(); obj_iter != o_p_v_con.end(); ++obj_iter) {
		// Output name:
		ot << "Object: " << (*obj_iter).first << ":" << endl;
		// Output the pv container
		((*obj_iter).second).display_contents(ot);
		}
}


