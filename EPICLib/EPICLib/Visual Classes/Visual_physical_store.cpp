#include "Visual_physical_store.h"
#include "Output_tee_globals.h"
#include "Epic_exception.h"
#include "Eye_processor.h"
#include "Eye_retina_functions.h"	// to allow access to visual availability - actually done in the wrong place (see
#include "Symbol_utilities.h"
#include "View_base.h"
#include "Assert_throw.h"

#include <iostream>
#include <string>
#include <sstream>
#include <list>

using std::cout;	using std::endl;
using std::string;
using std::ostringstream;
using std::list;


/* Exception classes */
class Create_duplicate_physical_store_object : public Epic_internal_error {
public:
Create_duplicate_physical_store_object(const Processor * proc_ptr, const Symbol& obj_name)
	{
		ostringstream oss;
		oss << proc_ptr->processor_info() 
			<< "Attempt to create duplicate store object with name: "
			<< obj_name;
		msg = oss.str();
	}
};




/*** Visual_physical_object ***/

// this function updates the eccentricity and visibility of an object - it is visible
// if any part of it is within the peripheral vision radius. If it is visible, was_visible is set to true
// and stays that way
// about to be superseded - see comments in Eye_processor.cpp 082909 -dk

void Visual_physical_object::update_eccentricity_and_visibility(GU::Point eye_location, double periphery_radius)
{
	eccentricity = cartesian_distance(location, eye_location);
	// it is visible if the center is close enough, or the closest point on the rectangle is close enough
	if(eccentricity < periphery_radius ||
		closest_distance(eye_location, location, size) < periphery_radius)
			visible = true;
	else
		visible = false;
}

// see comments in Eye_processor.cpp 082909 -dk
void Visual_physical_object::update_eccentricity(GU::Point eye_location)
{
	eccentricity = cartesian_distance(location, eye_location);
	// make visibility false by default
	visible = false;
}


double Visual_physical_object::get_eccentricity() const
{
	return eccentricity;
}

bool Visual_physical_object::is_visible() const
{
	return visible;
}


void Visual_physical_object::set_visible(bool visible_)
{
	visible = visible_;
}


bool Visual_physical_object::is_available(const Symbol& prop_name) const
{
	return (available_properties.find(prop_name) != available_properties.end());
}

void Visual_physical_object::set_available(const Symbol& prop_name)
{
	available_properties.insert(prop_name);
}

void Visual_physical_object::set_unavailable(const Symbol& prop_name)
{
	available_properties.erase(prop_name);
}

void Visual_physical_object::display_contents(Output_tee& ot) const
{
	ot << "Visual_physical_object ";
	Visual_store_object::display_contents(ot);
	ot << "eccentricity " << eccentricity << endl;
}


/*** Visual_physical_store ***/

void Visual_physical_store::initialize()
{
	Visual_store::initialize();
	//Assert(vsensory_ptr);
//	psychobj_counter = 0;
//	set_randomize_when_used(true);
//	set_randomize(true);
//	randomize();
//	describe_parameters(Normal_out);
}

/*
// these functions select whether the destination processor gets physical or psychological names
// if it gets only one
const Symbol& Visual_store::select_destination_name(Smart_Pointer<Visual_store_object> obj_ptr)
{
	return obj_ptr->get_physical_name();
}

const Symbol& Visual_store::select_destination_name(const Symbol& obj_name)
{
	return get_object_ptr(obj_name)->get_physical_name();
}
*/

// this is the override for physical store - new object from device has only physical name
// generate the psychological object name, and then pass it on
void Visual_physical_store::make_object_appear(const Symbol& physical_name, GU::Point location, GU::Size size)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "make object " << physical_name 
			<< " appear at " << location  << " size " << size << endl;
	// we disallow making an object appear that is still present in the physical store
	if(is_present(physical_name))
		throw Create_duplicate_physical_store_object(this, physical_name);
		
	insert_new(Visual_physical_object::create(physical_name, location, size));
	
	if(get_trace() && Trace_out) {
		Trace_out << processor_info() << "new object: " << physical_name << endl;
//		obj.display_contents(Trace_out);
		}

	notify_views(&View_base::notify_object_appear, physical_name, location, size);

	// inform the next processor
	get_human_ptr()->get_Eye_processor_ptr()->make_object_appear(physical_name, location, size);

}

// these functions use the base class then pass the call on if a change was made and needs testing
void Visual_physical_store::make_object_disappear(const Symbol& physical_name)
{
	// do the common processing on objects that are disappearing
	Visual_store::make_object_disappear(physical_name);
	// call downstream to let the processor know 
	get_human_ptr()->get_Eye_processor_ptr()->make_object_disappear(physical_name);
	// now that eye processor has had a chance to see it, remove the object
	Visual_store::erase_object(physical_name);
}

GU::Point Visual_physical_store::set_object_location(const Symbol& physical_name, GU::Point location)
{
	GU::Point old_value = Visual_store::set_object_location(physical_name, location);
	if(changed)
		get_human_ptr()->get_Eye_processor_ptr()->set_object_location(physical_name, location);
	return old_value;
}

GU::Size Visual_physical_store::set_object_size(const Symbol& physical_name, GU::Size size)
{
	GU::Size old_value = Visual_store::set_object_size(physical_name, size);
	if(changed)
		get_human_ptr()->get_Eye_processor_ptr()->set_object_size(physical_name, size);
	return old_value;
}

Symbol Visual_physical_store::set_object_property(const Symbol& physical_name, const Symbol& propname, const Symbol& propvalue)
{
	Symbol old_value = Visual_store::set_object_property(physical_name, propname, propvalue);
	if(changed)
		get_human_ptr()->get_Eye_processor_ptr()->set_object_property(physical_name, propname, propvalue);
	return old_value;
}


/*
void Visual_physical_store::display_contents(Output_tee& ot) const
{
	ot << processor_info() << ':' << endl;
	Visual_store::display_contents(ot);
}
*/


