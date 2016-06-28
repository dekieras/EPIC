#include "Visual_sensory_store.h"
#include "Output_tee_globals.h"
#include "Visual_perceptual_processor.h"
#include "Symbol_utilities.h"
#include "View_base.h"

#include <iostream>
#include <string>
#include <list>
#include <typeinfo>
#include <memory>

using std::cout;	using std::endl;
using std::string;
using std::list;
using std::map;
using std::shared_ptr;

// 02/23/09 - dk
// cancellation logic for erase events - if an appear post-dates the erase, erasure is cancellation


/*** Visual_sensory_object ***/

void Visual_sensory_object::display_contents(Output_tee& ot) const
{
	ot << "Visual_sensory_object ";
	Visual_store_object::display_contents(ot);
}

// access the list of property event times
void Visual_sensory_object::set_property_event_time(const Symbol& property_name, long event_time)
{
	property_event_times[property_name] = event_time;
}

void Visual_sensory_object::remove_property_event_time(const Symbol& property_name)
{
	property_event_times.erase(property_name);
}

// returns zero if not present - 
long Visual_sensory_object::get_property_event_time(const Symbol& property_name) const
{
	map<Symbol, long>::const_iterator it;
	it = property_event_times.find(property_name);
	if(it == property_event_times.end())
		return 0;	
	return it->second;
}



/*** Visual_sensory_store ***/

void Visual_sensory_store::accept_event(const Visual_event * event_ptr)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << typeid(*event_ptr).name() << " received" << endl;
	broadcast_to_recorders(event_ptr);
	// tell the event to handle itself with itself!
	event_ptr->handle_self(this);
}

// accept each event, update the local store generically, and then call the perceptual processor if a change was made (if necessary to test)
void Visual_sensory_store::handle_event(const Visual_Appear_event * event_ptr)
{
	// this might be a reinstated object, so create and insert a new object only if not already present
	if(!is_present(event_ptr->name)) {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() <<  "New object " << event_ptr->name << " appearing at " << event_ptr->location  
				<< " size " << event_ptr->size << endl;
		insert_new(Visual_sensory_object::create(event_ptr->name, event_ptr->location, event_ptr->size));
		// notify only if the object wasn't already there
		notify_views(&View_base::notify_object_appear, event_ptr->name, event_ptr->location, event_ptr->size);
		}
	else {
		// being reinstated, so reset any disappearance time
		if(get_trace() && Trace_out)
			Trace_out << processor_info() <<  "Object " << event_ptr->name << " reappearing" << endl;
		shared_ptr<Visual_store_object> obj_ptr = get_object_ptr(event_ptr->name);
		// cancel any scheduled disappearances
		obj_ptr->remove_disappearing_time();
		notify_views(&View_base::notify_object_reappear, event_ptr->name);
		}


// below removed above substituted - dk 1/29/09
//	Visual_store::make_object_appear(event_ptr->name, event_ptr->location, event_ptr->size);
	get_human_ptr()->get_Visual_perceptual_processor_ptr()->
		make_object_appear(event_ptr->name, event_ptr->location, event_ptr->size);
}

void Visual_sensory_store::handle_event(const Visual_Disappear_event * event_ptr)
{
	Visual_store::make_object_disappear(event_ptr->object_name);	// posts the information, but object is still there
//	get_human_ptr()->get_Visual_perceptual_processor_ptr()->make_object_disappear(event_ptr->object_name);

	// we waiting until now in case the downstream processor wants to access it
//	erase(event_ptr->object_name);

	// schedule an erasure event for later
	long erase_time = get_time() + disappearance_decay_delay.get_long_value();
	shared_ptr<Visual_store_object> obj_ptr = get_object_ptr(event_ptr->object_name);
	obj_ptr->set_disappearing_time(erase_time);
	
	schedule_event(new Visual_Erase_event(erase_time, this, event_ptr->object_name));
}

void Visual_sensory_store::handle_event(const Visual_Erase_event * event_ptr)
{
	// check to see if the object is still present - it might have already disappeared
	if(!is_present(event_ptr->object_name)) {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << event_ptr->object_name 
				<< " no longer present for erasure" << endl;
		return;
		}
	shared_ptr<Visual_store_object> obj_ptr = get_object_ptr(event_ptr->object_name);
	Assert(obj_ptr);	// only if gross programming error
	// check the disappearing time
	long disappearing_time = obj_ptr->get_disappearing_time();
	// if canceled, or comes later than current time, do nothing with this event
	if(disappearing_time == 0 || disappearing_time > get_time()) {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << event_ptr->object_name 
				<< " erase event discarded or superseded" << endl;
		return;
		}
	// at this point, the object is about to be removed
	// tell the down-stream processor it is going
	get_human_ptr()->get_Visual_perceptual_processor_ptr()->make_object_disappear(event_ptr->object_name);
	// now delete the object
	Visual_store::erase_object(event_ptr->object_name);
}

void Visual_sensory_store::handle_event(const Visual_Change_Location_event * event_ptr)
{
	Visual_store::set_object_location(event_ptr->object_name, event_ptr->location);
	if(changed)
		get_human_ptr()->get_Visual_perceptual_processor_ptr()->
			set_object_location(event_ptr->object_name, event_ptr->location);
}

void Visual_sensory_store::handle_event(const Visual_Change_Size_event * event_ptr)
{
	Visual_store::set_object_size(event_ptr->object_name, event_ptr->size);
	if(changed)
		get_human_ptr()->get_Visual_perceptual_processor_ptr()->
			set_object_size(event_ptr->object_name, event_ptr->size);
}

void Visual_sensory_store::handle_event(const Visual_Change_Property_event * event_ptr)
{
	Visual_store::set_object_property(event_ptr->object_name, event_ptr->property_name, event_ptr->property_value);
	if(changed)
		get_human_ptr()->get_Visual_perceptual_processor_ptr()->
			set_object_property(event_ptr->object_name, event_ptr->property_name, event_ptr->property_value);
}

