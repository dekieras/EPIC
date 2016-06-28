#ifndef OPV_CONTAINERS_H
#define OPV_CONTAINERS_H

/* These classes provide a facility for representing objects that have properties and values.
The objects have names stored in a Symbol, and associated with each object is a container
of properties, each with a value. The property names are represented by Symbols as are the Values. 

A PV_Container is a property list - store a value under a property name, 
supply the property name, and get a value back. 

An OPV_Container is a set of objects, each with an associated PV_container.
Store or retrieve a value by supplying an object name and a property name.

Note individual accessor functions return different values if the object
or property is not found. The returned values are defined in 
Standard_utility_symbols.h

*/

#include "Symbol.h"

#include <map>
#include <list>
#include <iosfwd>

class Output_tee;

class PV_Container {
public:
	// creators
	PV_Container() {}	// default is empty
	// create with a single name-value pair
    PV_Container(const Symbol& prop_name, const Symbol& prop_value) 
		{store(prop_name, prop_value);}

	// modifiers
	// store the value under the name; if the value is Nil_c, erase the property-value pair.
	void store (const Symbol& prop_name, const Symbol& prop_value);
	// remove the property-value pair.
	void erase (const Symbol& prop_name);
	void clear() 
		{p_v_con.clear();}

	// accessors
	bool empty() const
		{return p_v_con.empty();}
	// retrieve the value for the name; returns Nil_c if not found
	Symbol retrieve (const Symbol& prop_name) const;
	// fill the supplied lists with the property names and values
	void get_pv_lists (Symbol_list_t&prop_names, Symbol_list_t& prop_values) const;

    bool operator== (const PV_Container & rhs) const
		{return p_v_con == rhs.p_v_con;}
    bool operator!= (const PV_Container & rhs) const
		{return p_v_con != rhs.p_v_con;}

	// output each property and value (space-separated) on a separate line
	void display_contents(Output_tee&) const;
	friend std::ostream& operator<< (std::ostream& os, const PV_Container& pvc);
			
private:
	typedef std::map<Symbol, Symbol> p_v_con_t;
	p_v_con_t p_v_con;
};

std::ostream& operator<< (std::ostream& os, const PV_Container& pvc);

class OPV_Container {
public:
	// default constructor results in empty container.
	
	// modifiers
	// create an object object_name, return true if created, false if already present
	bool create_object (const Symbol& object_name);
	// erase the object object_name
	void erase (const Symbol& object_name);
	// store the value prop_value under property prop_name of object object_name
	// create a new object if not already there; prop_value replaces any existing value
	// following rules for PV_Container::store()
	void store (const Symbol& object_name, const Symbol& prop_name, const Symbol& prop_value);
	// store the values under the properties of object object_name
	// create a new object if not already there; each prop_value replaces any existing value
	// following rules for PV_Container::store()
	void store (const Symbol& object_name, const Symbol_list_t& prop_names, const Symbol_list_t& prop_values);
	// erase the value and property name under property prop_name of object object_name
	void erase (const Symbol& object_name, const Symbol& prop_name);
	// empty the container of all objects and their properties and values
	void clear()
		{o_p_v_con.clear();}
	
	// accessors
	bool empty() const 
		{return o_p_v_con.empty();}
	// return true if the object object_name exists in the container, false otherwise.
	bool is_present (const Symbol& object_name) const;
	// return the value under property prop_name of object object_name
	// Nil_c is returned if either the object_name or the prop_name is unknown
	Symbol retrieve (const Symbol& object_name, const Symbol& prop_name) const;
	// return the name of the first object that has value prop_value of property prop_name
	// returns Absent_c if not present
	Symbol find (const Symbol& prop_name, const Symbol& prop_value) const;
	// return the name of the first object that matches all pv pairs; return Absent_c if none
	Symbol find (const Symbol_list_t& prop_names, const Symbol_list_t& prop_values) const;	
	// return a list of all object names that match all pv pairs
	Symbol_list_t find_all (const Symbol_list_t& prop_names, const Symbol_list_t& prop_values) const;
	// return a list of all the object names
	Symbol_list_t get_all_object_names() const;
	// fill the supplied lists with the property names and values for object object_name
	bool get_pv_lists (const Symbol& object_name, Symbol_list_t& prop_names, Symbol_list_t& prop_values) const;

	
	// define equality in terms of the underlying containers
	bool operator== (const OPV_Container & rhs) const 
		{return o_p_v_con == rhs.o_p_v_con;}
	bool operator!= (const OPV_Container & rhs) const 
		{return o_p_v_con != rhs.o_p_v_con;}

	// display the contents of the container
	void display_contents(Output_tee&) const;
	
	
	// the following set of accessors are unnecessarily extreme and should be removed; the programming is idiosyncratic
	// and the names and operations are badly named and were originally defined inline, cluttering the header.
    bool equals(const OPV_Container & other) const
    	{return o_p_v_con == other.o_p_v_con;}
    // below erases all previous property-value pairs for an object - should be called "set" instead of "add"
    void add( const Symbol& object_name, const PV_Container & prop_list )
   		{o_p_v_con[object_name] = prop_list;}
    std::map<Symbol, PV_Container>::const_iterator begin() const 
    	{return o_p_v_con.begin();}
    std::map<Symbol, PV_Container>::const_iterator end() const 
    	{return o_p_v_con.end();}

private:
	typedef std::map<Symbol, PV_Container> o_p_v_con_t;
	o_p_v_con_t o_p_v_con;
};


#endif
