#include "Visual_perceptual_store.h"
#include "Eye_processor.h"	// for name maps
#include "Output_tee_globals.h"
#include "Epic_standard_symbols.h"
#include "Symbol_utilities.h"
#include "View_base.h"
#include "Cognitive_event_types.h"
#include "Cognitive_processor.h"
#include "Epic_exception.h"
#include "Assert_throw.h"

#include <iostream>
#include <string>
#include <list>
#include <map>
#include <typeinfo>
#include <memory>
	
using std::cout;	using std::endl;
using std::string;
using std::list;
using std::map;
using std::shared_ptr; using std::make_shared;
/*
2/19/09
Perceptual_processor is now required to send appear and disappear events to Perceptual_store
with the same time delays, so that Perceptual_store always gets the events in the same order
that Perceptual_processor did.
*/

/*
When an object appears:
If the same name is not already in the store, 
	an object with that name is created and placed in the store with a state of PRESENT,
	and the Views notified. 
If the same name is already in the store,
	the object is considered as Reappearing, its Status is set to that, 
	and its state is set to PRESENT, its disappearance time removed,
	and the Views notified of a Reappearance.

If an object disappears, it goes through first an UNSUPPORTED state where its Status is still Visible,
then into a RETAINED state where its Status is Disappearing, and then it is removed.  
The notion of persistence in the store has the following ideas:
1. For a short time, there is no indication that the object is gone.
2. For a long time, the object info is available, but we know it is disappearing.
3. Then the object is gone completely.

Specifics:

If an object disappears: 
	set state to UNSUPPORTED
	schedule a first Erase event to arrive after unsupported_object_decay_time, and set the disappearance time to this time also
If an object erase event arrives:
	If the object is not present - ignore with a trace message
	If the disappearance time is 0 (cancelled) or > current_time, ignore this erase event - do nothing (no message)
	Do one of the following depending on the state of the object:
	The object is UNSUPPORTED:
		set the Status to Disappearing and inform cogp and the views
		set the state to RETAINED
		schedule a second Erase event to arrive after retained_object_decay_time, and set the disappearance time to this time
		do nothing more
	The object is REATAINED:
		remove it from the store and inform cogp and the views and remove it from the name map!
	The object is PRESENT:
		ignore with a trace message

Properties of an object are retained, removed, and reinstated as follows:
When a Change Property event arrives:
	If the property is to be removed, we save the disappearing time, and send an erase event for that time.
	If a property is being set (as opposed to removed), any disappearing time is removed.

When an Erase Property event arrives, if the object is still present, we aacess the disappearing time and then decide:
	if 0, was canceled, ignore (no message)
	if > current time, was reinstated, and then lost again, so ignore (no message), let later erase event come.
	if == current time, remove it now with message. (along with disappearing time)
*/


// KLUDGE KLUDGE KLUDGE - COMMENT THIS OUT TO DISABLE UNKNOWN COLOR & SHAPE KLUDGE
//#define COLOR_SHAPE_UNKNOWN_KLUDGE 1


/*** Visual_perceptual_object ***/

// access the list of disappearing properties
void Visual_perceptual_object::set_disappearing_time(const Symbol& property_name, long disappearing_time)
{
	unsupported_properties[property_name] = disappearing_time;
}

void Visual_perceptual_object::remove_disappearing_time(const Symbol& property_name)
{
	unsupported_properties.erase(property_name);
}

// returns zero if not present - 
long Visual_perceptual_object::get_disappearing_time(const Symbol& property_name) const
{
	map<Symbol, long>::const_iterator it;
	it = unsupported_properties.find(property_name);
	if(it == unsupported_properties.end())
		return 0;	
	return it->second;
}

void Visual_perceptual_object::display_contents(Output_tee& ot) const
{
	ot << "Visual_perceptual_object ";
	Visual_store_object::display_contents(ot);
}

// This function is required because Visual_perceptual_objects require the get/set_state function which at present
// isn't declared in the base class because the state enum isn't there ...
shared_ptr<Visual_perceptual_object> Visual_perceptual_store::get_perceptual_object_ptr(const Symbol& object_name)
{
	shared_ptr<Visual_store_object> p = get_object_ptr(object_name);
	shared_ptr<Visual_perceptual_object> obj_ptr = std::dynamic_pointer_cast<Visual_perceptual_object>(p);
	if(!obj_ptr)
		throw Epic_internal_error(this, "object was not a Visual_perceptual_object");
	return obj_ptr;
}


/*** Visual_perceptual_store ***/

// these functions use the base class then construct an update for the cognitive processor, that needs
// old and new values for updating.
void Visual_perceptual_store::accept_event(const Visual_event * event_ptr)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << typeid(*event_ptr).name() << " received" << endl;
	broadcast_to_recorders(event_ptr);
	// tell the event to handle itself with itself!
	event_ptr->handle_self(this);
}

// accept each event, update the local store using the local type of object
void Visual_perceptual_store::handle_event(const Visual_Appear_event * event_ptr)
{
	// this might be a reinstated object, so create and insert a new object only if not already present
	if(!is_present(event_ptr->name)) {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() <<  "New object " << event_ptr->name << " appearing at " << event_ptr->location  
				<< " size " << event_ptr->size << endl;
		insert_new(Visual_perceptual_object::create(event_ptr->name, event_ptr->location, event_ptr->size));
		// notify only if the object wasn't already there
		notify_views(&View_base::notify_object_appear, event_ptr->name, event_ptr->location, event_ptr->size);
		}
	// if it is already present, it is being reinstated, so we need to change its state to present and Status Property to Visible
	else {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() <<  "Object " << event_ptr->name << " reappearing" << endl;
		shared_ptr<Visual_perceptual_object> obj_ptr = get_perceptual_object_ptr(event_ptr->name);
		// cancel any scheduled disappearances
		obj_ptr->remove_disappearing_time();
		// if the object was disappearing, set its Status to Reappearing and wait for Visible property to arrive later
		// apparently we needed some delay there
		if(obj_ptr->get_state()	== Visual_perceptual_object::RETAINED)
			update_property(event_ptr->name, Status_c, Reappearing_c);
		// unconditionally set the state to PRESENT since the object is now here
		obj_ptr->set_state(Visual_perceptual_object::PRESENT);
		notify_views(&View_base::notify_object_reappear, event_ptr->name);
	}
			
		
// KLUDGE KLUDGE KLUDGE
/*
#ifdef COLOR_SHAPE_UNKNOWN_KLUDGE
	// add that the object has  unknown Color and Shape
	Symbol old_value = Visual_store::set_object_property(event_ptr->name, Color_c, Unknown_c);
	// send through to cognitive processor immediately
	get_human_ptr()->get_Cognitive_processor_ptr()->
		update_object_property(Visual_c, event_ptr->name, Color_c, old_value, Unknown_c);
	old_value = Visual_store::set_object_property(event_ptr->name, Shape_c, Unknown_c);
	// send through to cognitive processor immediately
	get_human_ptr()->get_Cognitive_processor_ptr()->
		update_object_property(Visual_c, event_ptr->name, Shape_c, old_value, Unknown_c);
#endif		
*/		
//	get_human_ptr()->get_Cognitive_processor_ptr()->
//		update_object_property(Visual_c, event_ptr->name, Detection_c, Nil_c, Onset_c);
	
	// schedule Onset removal for later delivery
//	schedule_event(new Cognitive_Update_event(
//		get_time() + change_decay_time.get_value(), get_human_ptr()->get_Cognitive_processor_ptr(),
//		Visual_c, event_ptr->name, Detection_c, Onset_c, Nil_c));
}


/*
If a Visual_Disappear_event comes in, arrange for a Visual_Erase_event to come after a delay
of unsupported_decay_delay. 
Can disappear events arrive before a corresponding appear event?
*/

void Visual_perceptual_store::handle_event(const Visual_Disappear_event * event_ptr)
{
/*	// save the properties of the object
	shared_ptr<Visual_store_object> obj_ptr = get_object_ptr(event_ptr->object_name);
	Symbol_list_t prop_names;
	Symbol_list_t prop_values;
	// this contains only properties with non-default values
	obj_ptr->get_pv_lists(prop_names, prop_values);
*/

	if(get_trace() && Trace_out)
		Trace_out << processor_info() << event_ptr->object_name  << " losing sensory support" << endl;
	shared_ptr<Visual_perceptual_object> obj_ptr = get_perceptual_object_ptr(event_ptr->object_name);
	
	// The object must first go into the unsupported state - it is still fully present as far as anyone else is concerned
	// Put the object into the "Unsupported" state to show that perceptual input no longer supports it
	obj_ptr->set_state(Visual_perceptual_object::UNSUPPORTED);
	// schedule an erasure event for later,  note the time
	long first_erase_time = get_time() + unsupported_object_decay_time.get_long_value();
	obj_ptr->set_disappearing_time(first_erase_time);
	schedule_event(new Visual_Erase_event(first_erase_time, this, event_ptr->object_name));
	
	// that's all for now

/*
	// post the disappearing, but object is still there
	Visual_store::make_object_disappear(event_ptr->object_name);

	// immediately tell cogp that the object is disappearing
	get_human_ptr()->get_Cognitive_processor_ptr()->
		update_object_property(Visual_c, event_ptr->object_name, Status_c, Visible_c, Symbol("Disappearing"));

*/
/*
	// immediately post that the object has offset
	get_human_ptr()->get_Cognitive_processor_ptr()->
		update_object_property(Visual_c, event_ptr->object_name, Detection_c, Nil_c, Offset_c);
	
*/	
/*
	// immediately post removals of the properties of the object
	Symbol_list_t ::iterator it_name = prop_names.begin();
	Symbol_list_t ::iterator it_value = prop_values.begin();
	for(;it_name != prop_names.end() && it_value != prop_values.end(); ++it_name, ++it_value) {
		get_human_ptr()->get_Cognitive_processor_ptr()->
			update_object_property(Visual_c, event_ptr->object_name, *it_name, *it_value, Nil_c);
		}
*/	
/*
	// schedule removal of Offset for later delivery
	schedule_event(new Cognitive_Update_event(
		get_time() + offset_decay_time.get_value(), get_human_ptr()->get_Cognitive_processor_ptr(),
		Visual_c, event_ptr->object_name, Detection_c, Offset_c, Nil_c));
*/	

//	// in case the downstream processor wants to access it
//	erase(event_ptr->object_name);
}

void Visual_perceptual_store::handle_event(const Visual_Erase_event * event_ptr)
{
	// check to see if the object is still present - it might have already disappeared
	if(!is_present(event_ptr->object_name)) {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << event_ptr->object_name 
				<< " no longer present for erasure" << endl;
		return;
		}
	
	shared_ptr<Visual_perceptual_object> obj_ptr = get_perceptual_object_ptr(event_ptr->object_name);
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
	// at this point, this erase event applies to this object
	Assert(disappearing_time == get_time());
	// if the object was in the unsupported state, it is now "disappearing" in the RETAINED state.
	switch(obj_ptr->get_state()) {
		case Visual_perceptual_object::UNSUPPORTED: {
			if(get_trace() && Trace_out)
				Trace_out << processor_info() << event_ptr->object_name  << " disappearing but being retained in memory" << endl;

			Visual_store::make_object_disappear(event_ptr->object_name);

			// immediately tell cogp that the object is disappearing
			update_property(event_ptr->object_name, Status_c, Disappearing_c);
			// Put the object into the "Retained" state to show that it is now in the special visual memory state
			obj_ptr->set_state(Visual_perceptual_object::RETAINED);

			// here is where we would decide whether this object is retained, or everything is going to be lost

			/* removed 1/21/03 - reinstate for capacity limited model
			if(!update_retained_objects()) {
				if(get_trace() && Trace_out)
					Trace_out << processor_info() << event_ptr->object_name  << " and other objects lost from memory " << endl;
				}
			else {
				// schedule an erasure event for later
				schedule_event(new Visual_Erase_event( 
					get_time() + retained_decay_delay.get_value(), this, event_ptr->object_name));
				}
			*/
			
			// 1/20/03 this gives independent object loss
			// schedule an erasure event for later
			long second_erase_time = get_time() + retained_object_decay_time.get_long_value();
			obj_ptr->set_disappearing_time(second_erase_time);
			schedule_event(new Visual_Erase_event(second_erase_time, this, event_ptr->object_name));
			break;
			}
		
		case Visual_perceptual_object::RETAINED: {
			// the object's time is up - get rid of it
			lose_object_from_memory(obj_ptr);
			break;
			}
		
		case Visual_perceptual_object::PRESENT: {
			if(get_trace() && Trace_out)
				Trace_out << processor_info() << event_ptr->object_name  << " was disappearing but got reinstated" << endl;
			break;
			}
		
		default: {
			throw Epic_internal_error(this, "Unknown object state in handling Visual_Erase_event");
			break;
			}
		}
}

// this function removes an object from the perceptual store and all of its properties from cognitive processor
void Visual_perceptual_store::lose_object_from_memory(shared_ptr<Visual_perceptual_object> obj_ptr)
{
	const Symbol& object_name = obj_ptr->get_name();
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << object_name  << " being erased from memory" << endl;

	// remove the relevant properties from cognitive processor's memory
	switch(obj_ptr->get_state()) {
		default:
		case Visual_perceptual_object::PRESENT: 
			// just update the cognitive processor, because the object is going to be removed
			get_human_ptr()->get_Cognitive_processor_ptr()->
				update_object_property(Visual_c, object_name, Status_c, Reappearing_c, Nil_c);
		case Visual_perceptual_object::UNSUPPORTED: {
			// just update the cognitive processor, because the object is going to be removed
			get_human_ptr()->get_Cognitive_processor_ptr()->
				update_object_property(Visual_c, object_name, Status_c, Visible_c, Nil_c);
			break;
			}
		case Visual_perceptual_object::RETAINED: {
			// just update the cognitive processor, because the object is going to be removed
			get_human_ptr()->get_Cognitive_processor_ptr()->
				update_object_property(Visual_c, object_name, Status_c, Disappearing_c, Nil_c);
			break;
			}
		}

	// collect the other properties of the object
	Symbol_list_t prop_names;
	Symbol_list_t prop_values;
	// this contains only properties with non-default values
	obj_ptr->get_pv_lists(prop_names, prop_values);
	
	// immediately post removals of the properties of the object
	Symbol_list_t ::iterator it_name = prop_names.begin();
	Symbol_list_t ::iterator it_value = prop_values.begin();
	for(;it_name != prop_names.end() && it_value != prop_values.end(); ++it_name, ++it_value) {
		get_human_ptr()->get_Cognitive_processor_ptr()->
			update_object_property(Visual_c, object_name, *it_name, *it_value, Nil_c);
		}

	// now actually delete the object
	Visual_store::erase_object(object_name);
	// At this point, the object by this name is gone psychologically, and this means
	// that both the psychological and physical names are no longer in use.
	// As part of the disappearance/reappearance process, the object might have just
	// repeared shortly before this final removal, meaning that the physical name is
	// still in use. Cognitively, this means that the object will seem to disappear and
	// then immediately thereafter reappear under the same name.
	// Thus we only remove the names if the physical name is not in use (e.g. for a just appeared object) - dk 3/11/09
	// A problem might still appear if the name is referred to on this same cycle for an aimed movement.
	Name_map& name_map = get_human_ptr()->get_Eye_processor_ptr()->get_name_map();
	const Symbol& physical_name = name_map.get_physical_name(object_name);
	if(!(name_map.is_physical_name_in_use(physical_name))) {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() <<  "Object names " << physical_name << '/' << object_name << " removed from Visual system" << endl;
		name_map.remove_names_with_psychological_name(object_name);
		}
	else {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() <<  "Object names " << physical_name << '/' << object_name << " being retained in Visual system" << endl;
		}
		
	//get_human_ptr()->get_Eye_processor_ptr()->get_name_map().remove_names_with_psychological_name(object_name);
}


/*
// function commented out 1/21/03 - it implements a "snapshot" model which is superseded
// this function is called after all objects have been added to the retained storage - it decides 
// whether to keep or delete them (or all of them) it returns bool if all objects are retained, false if not
bool Visual_perceptual_store::update_retained_objects()
{
	// collect the current set of retained objects
	typedef list<shared_ptr<Visual_perceptual_object> > Perceptual_object_ptr_list_t;
	Perceptual_object_ptr_list_t results;
	Store_object_ptr_list_t object_ptrs = get_object_ptr_list();
	for(Store_object_ptr_list_t::iterator it = object_ptrs.begin(); it != object_ptrs.end(); ++it) {
		// damn downcast needed!
		Visual_store_object * p = (*it);
		shared_ptr<Visual_perceptual_object> obj_ptr = dynamic_cast<Visual_perceptual_object *>(p);
		if(obj_ptr && obj_ptr->get_state() == Visual_perceptual_object::RETAINED)
			results.push_back(obj_ptr);
		}	
	
	// flip a coin to decide whether to keep them or not 
	// testing version - probability is .9 for 4 or fewwr object, 1/(n / 2) otherwise
	double p;
	if (results.size() <= 4)
		p = .9;
	else
		p = 1.0 / (results.size() / 2.0);
	if(unit_uniform_random_variable() < p) {
		// keeping them
		return true;
		}
	
	// get rid of all of them
	for(Perceptual_object_ptr_list_t::iterator it = results.begin(); it != results.end(); ++it) {
		lose_object_from_memory(*it);
		}
	return false;
}
*/


void Visual_perceptual_store::handle_event(const Visual_Change_Location_event * event_ptr)
{
	Visual_store::set_object_location(event_ptr->object_name, event_ptr->location);
	if(changed) {
		// immediately post the change
		get_human_ptr()->get_Cognitive_processor_ptr()->
			update_object_property(Visual_c, event_ptr->object_name, Location_c, Nil_c, Change_c);

		// schedule for later delivery
		schedule_event(new Cognitive_Update_event(
			get_time() + change_decay_time.get_long_value(), get_human_ptr()->get_Cognitive_processor_ptr(),
			Visual_c, event_ptr->object_name, Location_c, Change_c, Nil_c));
		}
}

void Visual_perceptual_store::handle_event(const Visual_Change_Size_event * event_ptr)
{
	Visual_store::set_object_size(event_ptr->object_name, event_ptr->size);
	if(changed) {
		// immediately post the change
		get_human_ptr()->get_Cognitive_processor_ptr()->
			update_object_property(Visual_c, event_ptr->object_name, Size_c, Nil_c, Change_c);

		// schedule for later delivery
		schedule_event(new Cognitive_Update_event(
			get_time() + change_decay_time.get_long_value(), get_human_ptr()->get_Cognitive_processor_ptr(),
			Visual_c, event_ptr->object_name, Size_c, Change_c, Nil_c));
		}
}

/*
If a property is being removed, arrange for it to disappear after property_decay_time_delay -
a Visual_Erase_Property_event will arrive at that time.
*/

void Visual_perceptual_store::handle_event(const Visual_Change_Property_event * event_ptr)
{
	// access the object
	shared_ptr<Visual_store_object> obj_ptr = get_object_ptr(event_ptr->object_name);
	Symbol old_value = obj_ptr->get_property_value(event_ptr->property_name);
	Symbol new_value = event_ptr->property_value;
	
	
// if property and value is Status Visible, add that the color and shape are unknown
// KLUDGE KLUDGE KLUDGE
#ifdef COLOR_SHAPE_UNKNOWN_KLUDGE
	if(event_ptr->property_name == Status_c && new_value == Visible_c) {
		// add that the object has  unknown Color and Shape
		Symbol value = Visual_store::set_object_property(event_ptr->object_name, Color_c, Unknown_c);
		// send through to cognitive processor immediately
		get_human_ptr()->get_Cognitive_processor_ptr()->
			update_object_property(Visual_c, event_ptr->object_name, Color_c, value, Unknown_c);
		value = Visual_store::set_object_property(event_ptr->object_name, Shape_c, Unknown_c);
		// send through to cognitive processor immediately
		get_human_ptr()->get_Cognitive_processor_ptr()->
			update_object_property(Visual_c, event_ptr->object_name, Shape_c, value, Unknown_c);
		}
#endif		
		

	
// KLUDGE KLUDGE KLUDGE
#ifdef COLOR_SHAPE_UNKNOWN_KLUDGE
	// if property is Color or Shape, and is being removed, change it to Unknown, and continue - this
	// Unknown property will stay present indefinitely
	if((event_ptr->property_name == Color_c || event_ptr->property_name == Shape_c) && new_value == Nil_c) {
		new_value = Unknown_c;
		}
	else // to attach to following if! (Messy!)
#endif		
		
		
	// are we removing a property value that is still present? - assuming everything else is OK
	// Detection property is not retained ...
	if(new_value == Nil_c && old_value != Nil_c && event_ptr->property_name != Detection_c) {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << event_ptr->object_name << " losing sensory support for property " 
			<< event_ptr->property_name << endl;
	
		long disappearing_time = get_time() + property_decay_time.get_long_value();
		// save the disappearance time for this property
		obj_ptr->set_disappearing_time(event_ptr->property_name, disappearing_time);
		// schedule a property erasure event for later - send old value along for comparison to current value
		schedule_event(new Visual_Erase_Property_event( disappearing_time, this,
			event_ptr->object_name, event_ptr->property_name, old_value));
		return;	// do nothing further
		}
		
	// otherwise, we are setting to a real value, so remove any disappearance marker - the value has been reestablished
	// so any pending erasure must be canceled!
	obj_ptr->remove_disappearing_time(event_ptr->property_name);
	
	update_property(event_ptr->object_name, event_ptr->property_name, new_value);

/*	// then update the store and broadcast the change
	old_value = Visual_store::set_object_property(event_ptr->object_name, event_ptr->property_name, new_value);
	// send through to cognitive processor immediately
	get_human_ptr()->get_Cognitive_processor_ptr()->
		update_object_property(Visual_c, event_ptr->object_name, event_ptr->property_name, 
			old_value, new_value);
*/
}

void Visual_perceptual_store::update_property(const Symbol& object_name, const Symbol& property_name, const Symbol& new_value)
{
	// update the store and broadcast the change
	Symbol old_value = Visual_store::set_object_property(object_name, property_name, new_value);
	// send through to cognitive processor immediately
	get_human_ptr()->get_Cognitive_processor_ptr()->
		update_object_property(Visual_c, object_name, property_name, old_value, new_value);
}


void Visual_perceptual_store::handle_event(const Visual_Erase_Property_event * event_ptr)
{
	// here because the property value is being erased
	// do nothing if the object is not present - not very efficient, but erasure of whole object may have already happened
	if(!is_present(event_ptr->object_name)) {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << event_ptr->object_name 
				<< " no longer present for erasure of " << event_ptr->property_name << endl;
		return;
		}
	// access the disappearing_property record
	shared_ptr<Visual_store_object> obj_ptr = get_object_ptr(event_ptr->object_name);
	Symbol current_value = obj_ptr->get_property_value(event_ptr->property_name);
	// this returns 0 if the disappearance record for this property does not exist
	long disappearing_time = obj_ptr->get_disappearing_time(event_ptr->property_name);
	// do nothing further if the disappearance record does not exist - it was conceled.
	if(disappearing_time == 0)
		return;
	// if the disapperance time comes later than the current time, apparently the property was
	// reinstated and then lost again before the scheduled disappearance event. If so, do nothing
	// further and let the next scheduled event take place
	else if(disappearing_time > get_time())
		return;
	
	// The disappearance time should match the current time, and the value in the event must match the current state;
	// if so, do the erasure and removal from the cognitive processor store
	else if (get_time() == disappearing_time && current_value == event_ptr->property_value) {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << event_ptr->object_name << " losing property " << event_ptr->property_name << endl;
		// remove the property disappearance record
		obj_ptr->remove_disappearing_time(event_ptr->property_name);
		// update the store and broadcast the change
		update_property(event_ptr->object_name, event_ptr->property_name, Nil_c);
/*		// update the store and broadcast the change
		Symbol old_value = Visual_store::set_object_property(event_ptr->object_name, event_ptr->property_name, Nil_c);
		// send through to cognitive processor immediately
		get_human_ptr()->get_Cognitive_processor_ptr()->
			update_object_property(Visual_c, event_ptr->object_name, event_ptr->property_name, 
				event_ptr->property_value, Nil_c);
*/
		}
	else
		throw Epic_internal_error(this, string("Unexpected Visual_Erase_Property_event for ") +
			event_ptr->object_name.str() + " property " + event_ptr->property_name.str());
}

