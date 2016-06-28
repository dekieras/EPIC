#include "Visual_store.h"
#include "Coordinator.h"
#include "Epic_exception.h"
#include "Output_tee_globals.h"
//#include "Visual_store_processor.h"
//#include "Eye_processor.h"
#include "Symbol_utilities.h"
#include "View_base.h"
#include "Assert_throw.h"

#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <memory>

using std::cout;	using std::endl;
using std::string;
using std::ostringstream;
using std::list;
using std::map;
using std::make_pair;
using std::shared_ptr;

/*** Exception classes ***/
class Create_duplicate_store_object : public Epic_internal_error {
public:
Create_duplicate_store_object(const Processor * proc_ptr, const Symbol& obj_name)
	{
		ostringstream oss;
		oss << proc_ptr->processor_info() 
			<< "Attempt to create duplicate store object with name: "
			<< obj_name;
		msg = oss.str();
	}
};

class Unknown_store_object : public Epic_internal_error {
public:
Unknown_store_object(const Processor * proc_ptr, const Symbol& obj_name)
	{
		ostringstream oss;
		oss << proc_ptr->processor_info() 
			<< "Attempt to access unknown store object with id: "
			<< obj_name;
		msg = oss.str();
	}
};


/*** Visual_store_object ***/

Visual_store_object::Visual_store_object(const Symbol& name_, GU::Point location_, GU::Size size_) :
		name(name_), location(location_), size(size_),
		creation_time(Coordinator::get_time())
		{}


// these cause an error if called on the wrong kind of object

void Visual_store_object::update_eccentricity_and_visibility(GU::Point, double)
{
	Assert(!"update_eccentricity_and_visibility called on wrong kind of object");
}

void Visual_store_object::update_eccentricity(GU::Point eye_location)
{
	Assert(!"update_eccentricity called on wrong kind of object");
}

double Visual_store_object::get_eccentricity() const
{
	Assert(!"get_eccentricity called on wrong kind of object");
	return 0.0;
}

bool Visual_store_object::is_visible() const
{
	Assert(!"is_visible called on wrong kind of object");
	return false;
}

void Visual_store_object::set_visible(bool visible_) 
{
	Assert(!"set_visible called on wrong kind of object");
}

bool Visual_store_object::was_visible() const
{
	Assert(!"was_visible called on wrong kind of object");
	return false;
}

bool Visual_store_object::is_available(const Symbol&) const
{
	Assert(!"is_available called on wrong kind of object");
	return false;
}

void Visual_store_object::set_available(const Symbol&)
{
	Assert(!"set_available called on wrong kind of object");
}

void Visual_store_object::set_unavailable(const Symbol&)
{
	Assert(!"set_unavailable called on wrong kind of object");
}

void Visual_store_object::set_disappearing_time(long)
{
	Assert(!"set_disappearing_time called on wrong kind of object");
}

void Visual_store_object::remove_disappearing_time()
{
	Assert(!"remove_disappearing_time called on wrong kind of object");
}

long Visual_store_object::get_disappearing_time() const
{
	Assert(!"get_disappearing_time called on wrong kind of object");
}

void Visual_store_object::set_property_event_time(const Symbol&, long)
{
	Assert(!"set_property_event_time called on wrong kind of object");
}

void Visual_store_object::remove_property_event_time(const Symbol&)
{
	Assert(!"remove_property_event_time called on wrong kind of object");
}

// returns zero if not present - 
long Visual_store_object::get_property_event_time(const Symbol&) const
{
	Assert(!"get_property_event_time called on wrong kind of object");
	return 0;
}

void Visual_store_object::set_disappearing_time(const Symbol&, long)
{
	Assert(!"set_disappearing_time called on wrong kind of object");
}

void Visual_store_object::remove_disappearing_time(const Symbol&)
{
	Assert(!"remove_disappearing_time called on wrong kind of object");
}

// returns zero if not present - 
long Visual_store_object::get_disappearing_time(const Symbol&) const
{
	Assert(!"get_disappearing_time called on wrong kind of object");
	return 0;
}

void Visual_store_object::display_contents(Output_tee& ot) const
{
	ot << name << ' ' << location << ' ' << size << endl;
	properties.display_contents(ot);
}

/*** Visual_store ***/

Visual_store::Visual_store(const std::string& name_, Human_processor * human_ptr_) :
		Human_subprocessor (name_, human_ptr_)
{
//	Assert(destination_ptr);
}

void Visual_store::initialize()
{
	Human_subprocessor::initialize();
	objects.clear();
//	set_randomize_when_used(true);
//	set_randomize(true);
//	randomize();
//	describe_parameters(Normal_out);
}

// this is apparently superfluous now
//void Visual_store::make_object_appear(const Symbol&, GU::Point, GU::Size)
//{
//	changed = false;
	// do nothing
//}


// this version of the function is called to create a new object when its psychological_name is available,
// which applies after the physical store
void Visual_store::make_object_appear(const Symbol& name, GU::Point location, GU::Size size)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "make object " << ' ' << name
			<< " appear at " << location  << " size " << size << endl;
			
	// create the new object
	insert_new(Visual_store_object::create(name, location, size));
	
	if(get_trace() && Trace_out) {
		Trace_out << processor_info() << "new object: " << endl;
//		obj.display_contents(Trace_out);
		}

	notify_views(&View_base::notify_object_appear, name, location, size);

	// exception thrown if input was invalid
	changed = true;
}

void Visual_store::make_object_disappear(const Symbol& obj_name)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "make object " << obj_name 
			<< " disappear" << endl;

	notify_views(&View_base::notify_object_disappear, obj_name);
	// remove the object - overrider must call ...
//	erase(obj_name);

	// exception thrown if input was invalid
	changed = false;
}

void Visual_store::make_object_reappear(const Symbol& obj_name)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "make object " << obj_name 
			<< " reappear" << endl;

	notify_views(&View_base::notify_object_reappear, obj_name);
	// exception thrown if input was invalid
	changed = false;
}

void Visual_store::erase_object(const Symbol& obj_name)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "erase object " << obj_name << endl;

	notify_views(&View_base::notify_erase_object, obj_name);

	erase(obj_name);
}

// return the old location
GU::Point Visual_store::set_object_location(const Symbol& obj_name, GU::Point location)
{
	changed = false;
	
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "set object " << obj_name 
			<< " location to " << location << endl;
	shared_ptr<Visual_store_object> obj_ptr = get_object_ptr(obj_name);	// throws if not there

	// do nothing more if location has not actually changed
	GU::Point old_location = obj_ptr->get_location();
	if(location == old_location)
		return old_location;

	// update the location
	obj_ptr->set_location(location);

	notify_views(&View_base::notify_visual_location_changed, obj_name, location);

	// exception thrown if input was invalid
	changed = true;
	return old_location;
}

// return the old size
GU::Size Visual_store::set_object_size(const Symbol& obj_name, GU::Size size)
{
	changed = false;

	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "set object " << obj_name 
			<< " size to " << size << endl;
	shared_ptr<Visual_store_object> obj_ptr = get_object_ptr(obj_name);	// throws if not there

	// do nothing more if size has not actually changed
	GU::Size old_size = obj_ptr->get_size();
	if(size == old_size)
		return old_size;

	obj_ptr->set_size(size);

	notify_views(&View_base::notify_visual_size_changed, obj_name, size);

	// exception thrown if input was invalid
	changed = true;
	return old_size;
}

// return the old property value
Symbol Visual_store::set_object_property(const Symbol& obj_name, const Symbol& prop_name, 
	const Symbol& prop_value)
{
	changed = false;

	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "set object " << obj_name 
			<< " property " << prop_name << " to " << prop_value << endl;

	shared_ptr<Visual_store_object> obj_ptr = get_object_ptr(obj_name);	// throws if not there
	// Symbol() will be returned if property was not there before
	Symbol old_prop_value = obj_ptr->get_property_value(prop_name);
	// do nothing if no change in property value
	if(prop_value == old_prop_value)
		return old_prop_value;
		
	obj_ptr->set_property(prop_name, prop_value); // will remove the property if value is Symbol();

	notify_views(&View_base::notify_visual_property_changed, obj_name, prop_name, prop_value);

	// exception thrown if input was invalid
	changed = true;
	return old_prop_value;
}


// return a list of the names of all the current objects
Symbol_list_t Visual_store::get_name_list() const
{
	Symbol_list_t results;
	for(Store_container_t::const_iterator it = objects.begin(); it != objects.end(); ++it)
		results.push_back((it->second)->get_name());
	return results;
}

Visual_store::Store_object_ptr_list_t Visual_store::get_object_ptr_list() const
{
	Store_object_ptr_list_t results;
	for(Store_container_t::const_iterator it = objects.begin(); it != objects.end(); ++it)
		results.push_back((it->second));
	return results;
}

// this find returns a list of the names of all objects that match all of prop-name-value pairs
// iterate through the container and check the values.
Symbol_list_t Visual_store::find_all (const Symbol_list_t& pv_list) const
{
	// pv_list must be of even length >= 2
	Assert((pv_list.size() >= 2) && (pv_list.size() % 2 == 0));

	Symbol_list_t found_objects;

	for (Store_container_t::const_iterator obj_iter = objects.begin(); 
			obj_iter != objects.end(); obj_iter++) {
		shared_ptr<Visual_store_object> obj_ptr = obj_iter->second;
		
		Symbol_list_t::const_iterator it = pv_list.begin();
		while(it != pv_list.end()) {
			const Symbol& pname = *it;
			++it;
			const Symbol& pvalue = *it;
			if (pvalue != obj_ptr->get_property_value(pname))
				break;
			++it;	// don't increment unless has matched
			}
		// if got to end of pv_list, save the object name we found
		
		if(it == pv_list.end())
			found_objects.push_back(obj_iter->first);
		}
	return found_objects;
}

// insert the object pointer if it is not there, do nothing if it is
void Visual_store::insert_new(shared_ptr<Visual_store_object> obj_ptr)
{
	const Symbol& obj_name = obj_ptr->get_name();
	Store_container_t::iterator it = objects.find(obj_name);
	// if present do nothing - dk 2/11/09
	if(it != objects.end()) {
/*		shared_ptr<Visual_store_object> orig_ptr = it->second;
		Normal_out << processor_info() << " Already present: " 
			<< orig_ptr->get_psychological_name() << '/' 
			<< orig_ptr->get_physical_name() << endl;
		orig_ptr->display_contents(Normal_out);
		obj_ptr->display_contents(Normal_out);
*/
//		throw Create_duplicate_store_object(this, obj_name);
		}
	else
//		objects[obj_name] = obj_ptr;	// significantly slower
		objects.insert(make_pair(obj_name, obj_ptr));
}

// erase the specified object,throw an exception if not present
void Visual_store::erase(const Symbol& obj_name)
{
	int n = int(objects.erase(obj_name));
	if(n == 0)
		throw Unknown_store_object(this, obj_name);
}


shared_ptr<Visual_store_object> Visual_store::get_object_ptr(const Symbol& obj_name) const
{
	Store_container_t::const_iterator it;
	it = objects.find(obj_name);
	if(it == objects.end())
		throw Unknown_store_object(this, obj_name);
	
	return it->second;
}

bool Visual_store::is_present(const Symbol& obj_name) const
{
	Store_container_t::const_iterator it;
	it = objects.find(obj_name);
	return(it != objects.end());
}

void Visual_store::clear()
{
	objects.clear();
}

void Visual_store::display_contents(Output_tee& ot) const
{
	ot << processor_info() << ':' << endl;
	Store_container_t::const_iterator it;
	for(it = objects.begin(); it != objects.end(); ++it)
		(it->second)->display_contents(ot);
}
