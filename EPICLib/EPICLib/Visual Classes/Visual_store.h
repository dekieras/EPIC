#ifndef VISUAL_STORE_H
#define VISUAL_STORE_H

/*
A Visual_store manages Visual_store_objects, and has both direct and event interfaces to
create, destroy, and modify Visual_store_objects.

All Visual_store_objects have:
both psychological and physical names
location & size
properties

All Visual_stores have a container of Visual_store_objects.

Both Visual_store and Visual_store_object are base classes.
*/


#include "Human_subprocessor.h"
#include "Symbol.h"
#include "OPV_Containers.h"
//#include "Smart_Pointer.h"
#include "Visual_event_types.h"
#include "Geometry.h"
namespace GU = Geometry_Utilities;

#include <map>
#include <set>
#include <memory>

/* Visual_store_object is a base class for objects that can be in stores. 
*/

class Visual_store_object {
public:
	virtual ~Visual_store_object()
		{}
	static std::shared_ptr<Visual_store_object> create(const Symbol& name,  GU::Point location, GU::Size size)
				{return std::shared_ptr<Visual_store_object>(new Visual_store_object(name, location, size));}
	const Symbol& get_name() const
		{return name;}
	void set_location(GU::Point location_)
		{location = location_;}
	GU::Point get_location() const
		{return location;}
	void set_size(GU::Size size_)
		{size = size_;}
	GU::Size get_size() const
		{return size;}
	long get_creation_time() const
		{return creation_time;}
		
	// if value is Symbol(), property is removed
	void set_property(const Symbol& property_name, const Symbol& property_value)
		{properties.store(property_name, property_value);}
	// if not present, Symbol() is returned
	Symbol get_property_value(const Symbol& property_name) const
		{return properties.retrieve(property_name);}
	// only properties with non-default values should be present
	void get_pv_lists(Symbol_list_t & prop_names, Symbol_list_t & prop_values) const
		{properties.get_pv_lists(prop_names, prop_values);}
		
	// Fat Interface for subtypes of Visual_store_object - these cause an error if called on the wrong kind of object
// about to be superseded - see comments in Eye_processor.cpp 082909 -dk
	virtual void update_eccentricity_and_visibility(GU::Point eye_location, double peripheral_radius);
	virtual void update_eccentricity(GU::Point eye_location);
	virtual double get_eccentricity() const;
	virtual void set_visible(bool visible_);
	virtual bool is_visible() const;
	virtual bool was_visible() const;
	virtual bool is_available(const Symbol& prop_name) const;
	virtual void set_available(const Symbol& prop_name);
	virtual void set_unavailable(const Symbol& prop_name);
	virtual void set_disappearing_time(long disappearing_time_);
	virtual void remove_disappearing_time();
	virtual long get_disappearing_time() const;
	virtual void set_property_event_time(const Symbol& property_name, long event_time);
	virtual void remove_property_event_time(const Symbol& property_name);
	virtual long get_property_event_time(const Symbol& property_name) const;
	virtual void set_disappearing_time(const Symbol& property_name, long disappearing_time);
	virtual void remove_disappearing_time(const Symbol& property_name);
	virtual long get_disappearing_time(const Symbol& property_name) const;
	
	void display_contents(Output_tee& ot) const;
	
protected:
	Symbol name;	// this is key used for sorting
	GU::Point location;
	GU::Size size;
	long creation_time;
	PV_Container properties;
	Visual_store_object(const Symbol& name, GU::Point location, GU::Size size);

private:
	// no copy, assignment
	Visual_store_object(const Visual_store_object&);
	Visual_store_object& operator= (const Visual_store_object&);
	
};

// ordering relation between Smart_Pointer<Visual_store_object>
inline bool operator< (std::shared_ptr<Visual_store_object> lhs, std::shared_ptr<Visual_store_object> rhs)
		{return lhs->get_name() < rhs->get_name();}


/* Visual_store
This base class contains virtual functions that output trace information, update the storage,
inform the views, and then rely on an overriding wrapper to transmit the information as needed
to the next processor.
*/

class Visual_store : public Human_subprocessor {
public:
	Visual_store(const std::string& name, Human_processor * human_ptr);
	virtual ~Visual_store()
		{}

	virtual void initialize();
	
	// direct inputs - object name is supplied along with all relevant information - no backwards lookup necessary
	// one-name form refers to whatever name is available or relevant
	// returns true if a change was made to data base (and so input should be processed further) and false otherwise
	// this creates a Visual_store_object, which is probably not what is needed ...
	virtual void make_object_appear(const Symbol& name, GU::Point location, GU::Size size);
	virtual void make_object_disappear(const Symbol& name);
	virtual void make_object_reappear(const Symbol& name);
	virtual void erase_object(const Symbol& name);
	// the following return the previous value, and do nothing unless it is different from the new value
	virtual GU::Point set_object_location(const Symbol& name, GU::Point location);
	virtual GU::Size set_object_size(const Symbol& name, GU::Size size);
	virtual Symbol set_object_property(const Symbol& name, const Symbol& propname, const Symbol& propvalue);

	// event interface - defined as empty by default
	virtual void accept_event(const Start_event *) {}
	virtual void accept_event(const Stop_event *) {}
	virtual void accept_event(const Visual_event *) {}
	virtual void handle_event(const Visual_Appear_event *) {}
	virtual void handle_event(const Visual_Disappear_event *) {}
	virtual void handle_event(const Visual_Erase_event *) {}
	virtual void handle_event(const Visual_Change_Location_event *) {}
	virtual void handle_event(const Visual_Change_Size_event *) {}
	virtual void handle_event(const Visual_Change_Property_event *) {}
	virtual void handle_event(const Visual_Erase_Property_event *) {}

	
	// services

	// objects are accessed via the obj_name; throw exception if not present
	std::shared_ptr<Visual_store_object> get_object_ptr(const Symbol& name) const;
	// see if the object is present in the store; return true if so, false if not - no exception thrown
	bool is_present(const Symbol& name) const;
	
	// return a list of the obj_names of all the current objects
	Symbol_list_t get_name_list() const;
	// return a list of pointers to all the current objects
	typedef std::list<std::shared_ptr<Visual_store_object> > Store_object_ptr_list_t;
	Store_object_ptr_list_t get_object_ptr_list() const;

	Symbol_list_t find_all (const Symbol_list_t& pv_list) const;
	// output a description of all of the objects currently present
	virtual void display_contents(Output_tee& ot) const;

protected:	
	// state
	bool changed;	// true if data was updated due to input.
	typedef std::map<Symbol, std::shared_ptr<Visual_store_object> > Store_container_t;
	Store_container_t objects;	// the object storage
	
	// helper functions
	// insert a new pointer to a new object, but throw an exception if already present
	void insert_new(std::shared_ptr<Visual_store_object> obj_ptr);
	// erase the specified object,throw an exception if not present
	void erase(const Symbol& name);
	// erase the specified object,throw an exception if not present
	void erase(std::shared_ptr<Visual_store_object> obj_ptr);
	// empty the contents
	void clear();

private:
	// no copy, assignment
	Visual_store(const Visual_store&);
	Visual_store& operator= (const Visual_store&);
};

#endif
