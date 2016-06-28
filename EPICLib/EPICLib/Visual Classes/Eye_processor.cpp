/*
8/29/09 - dk
Starting work on seeing if visibility can be made a function of retinal availability in a transparent way
- idea is that Visible will be specified as a retinal function, defaulted to peripheral zone, but can be set to 
any other function, in particular a probilitistic one based on object eccentricity and size.
Need to be sure that *no* messages about an object get sent forward unless it is visible - cf your come-into/go-outof-view modifications.

2/19/09 - dk
New constraint: appear/disappear output event time delays are constrained to be the same so that
subsequent processors and stores get these events in the same order as this processor does.
Notion that disappearances propagate more slowly than appearances is handled strictly by persistence in the stores.
*/

/*
1/30/09 - dk
New regime to handle objects that appear, disappear, go in and out of peripheral vision, in the context 
of eye movements.

Definitions:
"In view" means within peripheral vision.
"Eye moving/not moving" means while a saccade is/is not in progress (between saccade start and end events).
"Appear/disappear" means appearing/disappearing in the physical environment (the physical store).
If not otherwise qualified, if an object is said to appear/disappear, it is assumed to be in view.

Physical and Psychological names:
The device assigns a physical name to an object, and the Eye_processor assigns a psychological name when the object appears for the first time,
regardless of whether the object is in view or an eye movement is underway. 
The names are used to establish object identity. Changes happening to an object 
with the same physical name are assumed to be happening to the same corresponding psychological object.

If an object appears or comes into view with a physical name that has no corresponding psychological name, it
is taken to be a new object and has status Visible.

When an object disappears or goes out of view, the psychological status of the object is Disappearing. 
The physical name is retained until the psychological name (and corresponding object) is discarded from visual perceptual store, 
(which normally happens later), whereupon the physical name is discarded.

A reappearance or return into view of an object with the same physical name as an existing psychological object
is taken to be a reinstatement of the same object - it now has Status Visible again.

If and only if an object is in view and the eye is not moving does appearance/disappearance give rise to detection
onset/offset events.

At the end of an eye movement, all objects currently present in physical store have their visibility and available
properties updated to reflect the new eye position.

Changes in the physical store that happen during an eye movement have no effect during the eye movement; as appropriate, the object
information will be updated when the eye movement is complete.


7/20/01
Inputs received from Visual_physical_store; events send to Visual_sensory_store.

3/12/01 Need to reconcile inconsistency on when object changes are deferred versus updated and 
then deferred 

6/26/01 - install changes to prevent sending changes to Vsensory processor if the value is not 
changed - since Physical space does not remember this, must check Sensory_space to see what
current value is - there might be a problem with lag if changes happen close together in time - 
second might be lost if value hasn't changed yet ...

Looking downstream is fraught with peril - there might be other updates enroute that confuse the
issue. Need to keep track of what properties of an object are available and what are not.

*/

//#include <typeinfo>

#include "Eye_processor.h"
#include "Output_tee_globals.h"
#include "Epic_standard_symbols.h"
#include "Eye_event_types.h"
#include "Visual_constants.h"
#include "Eye_retina_functions.h"
#include "Visual_physical_store.h"
#include "Visual_sensory_store.h"
#include "Visual_event_types.h"
#include "Invol_ocular_actions.h"
#include "Invol_ocular_processor.h"
#include "Ocular_processor.h"
#include "Symbol_utilities.h"
#include "View_base.h"

#include <string>
#include <list>
#include <iostream>
#include <algorithm>
#include <functional>
#include <memory>

using std::string;
using std::list;
using std::endl;
using std::shared_ptr;
using std::make_shared;

/*** Exception classes ***/
// these are user-interpretable errors
class Unknown_physical_property : public Epic_exception {
public:
Unknown_physical_property(Processor * proc_ptr, const Symbol& propname)
	{
		msg = proc_ptr->processor_info() + 
		"Unknown physical property received: " + propname.str();
	}
};

/*** Eye_processor ***/

void Eye_processor::setup()
{
	Visual_physical_store& physical_store = *(get_human_ptr()->get_Visual_physical_store_ptr());
	// Visibility - a special availability - dk 082909
	availabilities[Visible_c] =
		make_shared<Periphery_std_zone_availability>(physical_store, Visible_c);
	availabilities[Position_c] = 
		make_shared<Relation_std_zone_availability>(physical_store, Position_c);
	availabilities[Vposition_c] = 
		make_shared<Relation_std_zone_availability>(physical_store, Vposition_c);
	availabilities[Hposition_c] = 
		make_shared<Relation_std_zone_availability>(physical_store, Hposition_c);
	availabilities[In_row_c] = 
		make_shared<Relation_std_zone_availability>(physical_store, In_row_c);
	availabilities[In_col_c] = 
		make_shared<Relation_std_zone_availability>(physical_store, In_col_c);
	availabilities[Orientation_c] = 
		make_shared<Relation_std_zone_availability>(physical_store, Orientation_c);
	availabilities[Leader_c] = 
		make_shared<Relation_std_zone_availability>(physical_store, Orientation_c);
	availabilities[Above_c] = 
		make_shared<Relation_std_zone_availability>(physical_store, Above_c);
	availabilities[Below_c] = 
		make_shared<Relation_std_zone_availability>(physical_store, Below_c);
	availabilities[Left_of_c] = 
		make_shared<Relation_std_zone_availability>(physical_store, Left_of_c);
	availabilities[Right_of_c] = 
		make_shared<Relation_std_zone_availability>(physical_store, Right_of_c);
	availabilities[Inside_c] = 
		make_shared<Relation_std_zone_availability>(physical_store, Inside_c);
	availabilities[In_center_of_c] = 
		make_shared<Relation_std_zone_availability>(physical_store, In_center_of_c);
	availabilities[Outside_c] = 
		make_shared<Relation_std_zone_availability>(physical_store, Outside_c);
	availabilities[Placed_on_c] = 
		make_shared<Relation_std_zone_availability>(physical_store, Placed_on_c);
	availabilities[Depth_c] = 
		make_shared<Relation_std_zone_availability>(physical_store, Depth_c);
	availabilities[Distance_c] = 
		make_shared<Relation_std_zone_availability>(physical_store, Distance_c);
	availabilities[Color_c] = 
		make_shared<Color_std_zone_availability>(physical_store);
	availabilities[Targetable_c] =
		make_shared<Color_std_zone_availability>(physical_store);
//	availabilities[Color_Vague_c] =
//		make_shared<Color_std_zone_availability>(physical_store);
	availabilities[Shape_c] =
		make_shared<Shape_std_zone_availability>(physical_store);
	availabilities[Text_c] = 
		make_shared<Text_std_zone_availability>(physical_store);
//	availabilities[Size_c] = 
//		make_shared<Fovea_std_zone_availability>(physical_store, Size_c);
	availabilities[Encoded_size_c] = 
		make_shared<Fovea_std_zone_availability>(physical_store, Encoded_size_c);
	default_availability = 
		make_shared<Fovea_std_zone_availability>(physical_store, Default_c);

	add_parameter(appearance_disappearance_delay);
	add_parameter(eccentricity_delay);
//	add_parameter(disappearance_delay);
	add_parameter(location_delay);
	add_parameter(size_delay);
	add_parameter(eccentricity_fluctuation);
	add_parameter(property_time_fluctuation);
	add_parameter(inform_invol_ocular_delay);
	
}

// set overall state
void Eye_processor::initialize()
{
	Human_subprocessor::initialize();
	psychobj_counter = 0;
	name_map.clear();
	eye_location = GU::Point(0., 0.);
	future_eye_location = GU::Point(0., 0.);
	voluntary_saccade_underway = false;
	involuntary_saccade_underway = false;
	involuntary_smooth_move_underway = false;
	eye_movement_complete_time = 0;
	closest_object_ptr = 0;	// current object closest to point of fixation
	closest_eccentricity = 0.;
	centering_enabled = false;
	centering_on= false;
	centering_active= false;
	reflex_enabled= false;
	reflex_on= false;
	auditory_reflex_enabled= false;
	auditory_reflex_on= false;
	involuntary_movement_in_progress= false;
	last_invol_ocular_command_time = 0;
	
	//availability_accumulator.reset();
	
//	eccentricity_fluctuation.set_randomization_time(Parameter::When_used);
//	eccentricity_fluctuation.randomize();
//	set_trace(true);
//	set_randomize_when_used(true);
//	set_randomize(true);
//	describe_parameters(Normal_out);
}

//  parameter modifiers
// 	appearance_delayparameter_ = appearance_delayconstant_;

// string contains <property> <availability_type> ... parameters depending on type ...

void Eye_processor::set_parameter(const Parameter_specification& param_spec)
{
	if(param_spec.parameter_name == "Availability") {
		Visual_physical_store& physical_store = *(get_human_ptr()->get_Visual_physical_store_ptr());
		shared_ptr<Availability> p = Availability::create(physical_store, param_spec);
		availabilities[p->get_property_name()] = p;
		}
	else {
		Human_subprocessor::set_parameter(param_spec);
		}
}

void Eye_processor::describe_parameters(Output_tee& ot) const
{
	Human_subprocessor::describe_parameters(ot);

	ot << "Availability:" << endl;
	for(Availability_map_t::const_iterator it = availabilities.begin(); it != availabilities.end(); ++it) {
//		shared_ptr<Availability> a_ptr = it->second;
		ot << "    " << it->second->get_description() << endl;
		}
}


// return current or future location
GU::Point Eye_processor::get_location() const
{
	if(voluntary_saccade_underway || involuntary_saccade_underway || involuntary_smooth_move_underway)
		return future_eye_location;
	return eye_location;
}

GU::Point Eye_processor::location_noise(GU::Point location)
{
	// dummy
	return location;
}

void Eye_processor::accept_event(const Stop_event *)
{
//	Normal_out << "Eye location is " << eye_location << endl;
//	physical_space.display_contents(Normal_out);
//	Normal_out << "Eye_processor availability statistics: " << availability_accumulator.get_proportion() << ' ' << availability_accumulator.get_n() << endl;
//	for(int i = 0; i < availability_distribution.get_n_bins(); i++) {
//		Normal_out << i * availability_distribution.get_bin_size() << ' ' << availability_distribution.get_bin_proportion(i) << endl;
//		}
}

void Eye_processor::accept_event(const Eye_event * event_ptr)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << typeid(*event_ptr).name() << " received" << endl;
	broadcast_to_recorders(event_ptr);
	// tell the event to handle itself with itself!
	event_ptr->handle_self(this);
}


void Eye_processor::accept_event(const Visual_event * event_ptr)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << typeid(*event_ptr).name() << " received" << endl;
	broadcast_to_recorders(event_ptr);
	// tell the event to handle itself with itself!
	event_ptr->handle_self(this);
}

/* 
	const Symbol Onset_stationary_c ("Onset_stationary");
*/

// update the eccentricity and visibility of the supplied object using the visibility availability - dk 082909
bool Eye_processor::update_eccentricity_and_visibility(shared_ptr<Visual_store_object> obj_ptr)
{
	obj_ptr->update_eccentricity(eye_location);
	// apply the availability function for visibility
	// we should cache this function for efficiency
	Availability_map_t::iterator it_availability = availabilities.find(Visible_c);
	if(it_availability == availabilities.end()) {
		throw Unknown_physical_property(this, Visible_c);
		}
	shared_ptr<Availability> a_ptr = it_availability->second;
//	double x = eccentricity_fluctuation.get_double_value();
	bool visible = a_ptr->available(obj_ptr);
	obj_ptr->set_visible(visible);
	return visible;
}

void Eye_processor::make_object_appear(const Symbol& physical_name, GU::Point location, GU::Size size)
{
	// if the object's name is not present, create the object's psychological name, and note as new objects
	bool new_object = false;
	Symbol psychological_name = get_psychological_name_and_new_status(physical_name, new_object);
	// the physical name is now in use for an object that is present
	name_map.set_physical_name_in_use(physical_name, true);

	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "make object " << physical_name << '/' << psychological_name 
			<< ((new_object) ? " appear at " : " reappear at ") << location  << " size " << size << endl;
			
	// if a saccade is underway, we do not reschedule the event but simply do nothing further.
	// - we let the post-saccade update handle apparent appearance of a new object - dk 12/12/08
	if(voluntary_saccade_underway || involuntary_saccade_underway) {
		if(get_trace() && Trace_out) {
			Trace_out << processor_info() << " saccadic suppression: defer appear/reappear of " << physical_name 
				<< " until saccade end" << endl;
			}
		return;
		}
		
	// at this point an object has appeared or reappeared when the eye is NOT in motion
	// but at this point we don't know if it is now in view ...
	shared_ptr<Visual_store_object> obj_ptr = get_human_ptr()->get_Visual_physical_store_ptr()->get_object_ptr(physical_name);
	// it wasn't visible before, because it is appearing while eye is still
//  - dk 082909
//	obj_ptr->update_eccentricity_and_visibility(eye_location, standard_peripheral_radius);
//	bool new_visibility = obj_ptr->is_visible();
	bool new_visibility = update_eccentricity_and_visibility(obj_ptr);
	if(new_visibility) {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << " object " << physical_name << '/' << psychological_name << " has appeared while eye stationary" << endl;
		long appearance_time = get_time() + appearance_disappearance_delay.get_long_value();
		schedule_event(new Visual_Appear_event(
			appearance_time, get_human_ptr()->get_visual_sensory_store_ptr(),
			psychological_name, location_noise(obj_ptr->get_location()), obj_ptr->get_size()));
/*
		// send onset event
		schedule_event(new Visual_Change_Property_event(
			appearance_time, get_human_ptr()->get_visual_sensory_store_ptr(), 
			psychological_name, Detection_c, Onset_stationary_c));
*/

		// send update on whether it is targetable
		set_object_property(physical_name, Targetable_c, Targetable_c);
		
		
		// send update where it is relative to the eye position
		send_eccentricity_update(psychological_name, obj_ptr->get_eccentricity());
		}
	// if it is not currently visible, it is out of view - do nothing more
	else {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << " object " << physical_name << '/' << psychological_name << " has appeared out of view" << endl;
		// nothing more to do here
		return;
		}

	// if reflex enabled and on, and Invol_ocular processor is free,
	// make a reflex saccade to the new object, allowing for time for appearance information to get processed
	if(reflex_enabled && reflex_on && free_to_move()) {
//		involuntary_movement_in_progress = true;
		shared_ptr<Motor_action> action_ptr(new Invol_ocular_Saccade_action(get_human_ptr(), psychological_name));
		last_invol_ocular_command_time = get_time() + appearance_disappearance_delay.get_long_value() + inform_invol_ocular_delay.get_long_value();
		action_ptr->dispatch(last_invol_ocular_command_time);
		}


}

void Eye_processor::handle_event(const Visual_Disappear_event * event_ptr)
{
//	make_object_disappear(event_ptr->object_name);
	// here if the original disappear event was rescheduled because an eye movement was in progress.
	// The object has already been removed, and was originally in view, and was de-centered if
	// it was originally being centered. So the only thing necessary is to pass its disappearance
	// to the next store - psychological name is passed along

	schedule_event(new Visual_Disappear_event( 
		get_time() + appearance_disappearance_delay.get_long_value(), get_human_ptr()->get_visual_sensory_store_ptr(),
		event_ptr->object_name));
}

/*
const Symbol Offset_stationary_c ("Offset_stationary");
*/

// when we call in here, the object has not been erased yet from Visual_physical_store, but will be as soon
// as this function returns.  If an eye movement is underway, set up to pass this event on when it is complete.
void Eye_processor::make_object_disappear(const Symbol& physical_name)
{
	// if the object went out of view a long time ago, it will be unknown to the system that it is disappearing, so do nothing
	// if the object is not known.
	if(!name_map.is_physical_name_present(physical_name)) {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << " unknown object " << physical_name << " disappearing - ignored" << endl;
		return;
		}
	
	// access the object
	shared_ptr<Visual_store_object> obj_ptr = get_human_ptr()->get_Visual_physical_store_ptr()->get_object_ptr(physical_name);
	const Symbol& psychological_name = name_map.get_psychological_name(physical_name);

	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "make object " << physical_name << '/' << psychological_name << " disappear" << endl;
	// the physical name is now now longer in use because the object isn't visually present any more
	name_map.set_physical_name_in_use(physical_name, false);
	// leave physical name in the map - it may be needed if e.g. an eye movement is made to an object that is disappearing - dk 2/2/09
	/* // here we set the physical_name to Disappeared_object_c to show that the physical object is actually not there any more
	name_map.replace_physical_name_for_psychological_name(Disappeared_object_c, psychological_name);
	*/
	// do nothing further if the object is not visible (within peripheral vision!) ???
	// attempt to check this only if the object is present in Visual_physical_store
	if(!obj_ptr->is_visible())
		return;
	// if object was being centered, it is now no longer
	if(obj_ptr == closest_object_ptr && centering_active) {
		centering_active = false;
		closest_object_ptr = 0;
		// if reflex enabled, turn them on in this case
		if(reflex_enabled)
			reflex_on = true;
			}

	if(voluntary_saccade_underway || involuntary_saccade_underway) {
		// queue a new event back to this Eye - the physical object will be gone, 
		// so only the psychological name is meaningful and will be passed along
		long rescheduled_time = eye_movement_complete_time + 1;
		if(get_trace() && Trace_out) {
			Trace_out << processor_info() << "saccadic suppression: defer disappearing object " << psychological_name 
				<< " until " << rescheduled_time << endl;
			}
		schedule_event(new Visual_Disappear_event(rescheduled_time, this, psychological_name));
		}

	else {
		// eye is stationary transmit disappearance forward 
		long disappearance_time = get_time() + appearance_disappearance_delay.get_long_value();
		schedule_event(new Visual_Disappear_event( 
			disappearance_time, get_human_ptr()->get_visual_sensory_store_ptr(), psychological_name));
/*
		// send offset event
		schedule_event(new Visual_Change_Property_event(
			disappearance_time, get_human_ptr()->get_visual_sensory_store_ptr(), 
			psychological_name, Detection_c, Offset_stationary_c));
*/
		}
}

void Eye_processor::handle_event(const Visual_Change_Location_event * event_ptr)
{
	set_object_location(event_ptr->object_name, event_ptr->location);
}

void Eye_processor::set_object_location(const Symbol& physical_name, GU::Point location)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "set object " << physical_name 
			<< " location to " << location << endl;
	
	if(!(get_human_ptr()->get_Visual_physical_store_ptr()->is_present(physical_name))) {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << "set object location for " << physical_name 
				<< " discarded because object no longer present" << endl;
		return;
		}

	if(voluntary_saccade_underway || involuntary_saccade_underway) {
		// queue a new event back to this Eye
		long rescheduled_time = eye_movement_complete_time + 1;
		if(get_trace() && Trace_out) {
			Trace_out << processor_info() << "saccadic suppression: defer set object " << physical_name 
				<< " location to " << location 
				<< " until " << rescheduled_time << endl;
			}
		schedule_event(new Visual_Change_Location_event(
			rescheduled_time, this, physical_name, location));
		return;
		}
	// access the object
	shared_ptr<Visual_store_object> obj_ptr = get_human_ptr()->get_Visual_physical_store_ptr()->get_object_ptr(physical_name);

//	bool old_visibility = obj_ptr->is_visible();
	// update the location; if object not visible, do nothing more
	// case where object becomes invisible is not currently handled
//	obj_ptr->set_location(location);	// was a redundant call - DK 6/11/04
	if(!update_location_information(obj_ptr))
		return;
//	obj_ptr->update_eccentricity_and_visibility(eye_location, standard_peripheral_radius);


	// if the object was not visible before, and is not visible now, do nothing more
//	if(!old_visibility && !obj_ptr->is_visible())
//		return;
		
	// transmit it forward and update the properties
	const Symbol& psychological_name = name_map.get_psychological_name(physical_name);
	schedule_event(new Visual_Change_Location_event(
		get_time() + location_delay.get_long_value(), get_human_ptr()->get_visual_sensory_store_ptr(),
		psychological_name, location));

	update_all_properties(obj_ptr);

	// if nothing is being centered, but reflexes are enabled and on, reflex saccade to this object
	// after time for location information to be processed.
	if(reflex_enabled && reflex_on && !centering_active && free_to_move()) {
//		involuntary_movement_in_progress = true;
		shared_ptr<Motor_action> action_ptr(new Invol_ocular_Saccade_action(get_human_ptr(), psychological_name));
		last_invol_ocular_command_time = get_time() + location_delay.get_long_value() + inform_invol_ocular_delay.get_long_value();
		action_ptr->dispatch(last_invol_ocular_command_time);
//		schedule_event(new Invol_ocular_Saccade_event(
//			get_time() + location_delay.get_value() + inform_invol_ocular_delay.get_value(), 
//			get_human_ptr()->get_invol_ocular_ptr(), psychological_name));
		}
	
	// This one object has moved. It may or may not be the one that needs to be centered.
	// Some other object might now be closer.
	update_closest_object();
	perform_centering();
}

void Eye_processor::handle_event(const Visual_Change_Size_event * event_ptr)
{
	set_object_size(event_ptr->object_name, event_ptr->size);
}

void Eye_processor::set_object_size(const Symbol& physical_name, GU::Size size)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "set object " << physical_name 
			<< " size to " << size << endl;
	if(!(get_human_ptr()->get_Visual_physical_store_ptr()->is_present(physical_name))) {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << "set object size for " << physical_name 
				<< " discarded because object no longer present" << endl;
		return;
		}
		
	if(voluntary_saccade_underway || involuntary_saccade_underway) {
		// queue a new event back to this Eye
		long rescheduled_time = eye_movement_complete_time + 1;
		if(get_trace() && Trace_out) {
			Trace_out << processor_info() << "saccadic suppression: defer set object " << physical_name 
				<< " size to " << size
				<< " until " << rescheduled_time << endl;
			}
		schedule_event(new Visual_Change_Size_event(rescheduled_time, this, physical_name, size));
		return;
		}
	// access the object
	shared_ptr<Visual_store_object> obj_ptr = get_human_ptr()->get_Visual_physical_store_ptr()->get_object_ptr(physical_name);

	// this might affect the visibility (which depends on the size) but not the eccentricity
	// so do not update the other location-dependent properties
	// - dk 082909
	obj_ptr->update_eccentricity_and_visibility(eye_location, standard_peripheral_radius);
	bool visible = update_eccentricity_and_visibility(obj_ptr);


	// transmit it forward if it is visible! (within peripheral vision!) and update its properties
	// ******* VISIBILITY MAY HAVE CHANGED! NOT IMPLEMENTED ***************
	// still not implemented! - dk 082909
	if(visible) {
		Symbol psychological_name = name_map.get_psychological_name(physical_name);
		schedule_event(new Visual_Change_Size_event(
			get_time() + size_delay.get_long_value(), get_human_ptr()->get_visual_sensory_store_ptr(),
			psychological_name, size));
		update_all_properties(obj_ptr);
		}
}

void Eye_processor::handle_event(const Visual_Change_Property_event * event_ptr)
{
	set_object_property(event_ptr->object_name, event_ptr->property_name, event_ptr->property_value);
}

void Eye_processor::set_object_property(const Symbol& physical_name, const Symbol& prop_name, const Symbol& prop_value)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "set object " << physical_name 
			<< " property " << prop_name << " to " << prop_value << endl;

	if(!(get_human_ptr()->get_Visual_physical_store_ptr()->is_present(physical_name))) {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << "set object property for " << physical_name 
				<< " discarded because object no longer present" << endl;
		return;
		}

	if(voluntary_saccade_underway || involuntary_saccade_underway) {
		// queue a new event back to this Eye
		long rescheduled_time = eye_movement_complete_time + 1;
		if(get_trace() && Trace_out) {
			Trace_out << processor_info() << "saccadic suppression: defer set object " << physical_name 
				<< " property " << prop_name << " to " << prop_value 
				<< " until " << rescheduled_time << endl;
			}
		schedule_event(new Visual_Change_Property_event(rescheduled_time, 
			this, physical_name, prop_name, prop_value));
		return;
		}
		
	// access the object
	shared_ptr<Visual_store_object> obj_ptr = get_human_ptr()->get_Visual_physical_store_ptr()->get_object_ptr(physical_name);
	// now determine if the property is to be sent to sensory store
	// - dk 082909
	// seems like this should have been done before - ignore a property change if the object is not visible
	if(!obj_ptr->is_visible()) {
		// set this property to unavailable because we can't see it
		obj_ptr->set_unavailable(prop_name);
		// do nothing further because the object is not visible
		return;
		}
	long delay = 0;
	Symbol new_value = prop_value;
	// see if this property is available, and modify value if necessary
	bool available = apply_availability(obj_ptr, prop_name, new_value, delay);
//	if(prop_name == Color_c)
//		Normal_out << "from apply_availability: " << obj_ptr->get_name() << ' ' << available << ' ' << new_value << endl;
	
	// remember, but do nothing if the property is not available
	if(!available) {
		obj_ptr->set_unavailable(prop_name);
		return;
		}
	obj_ptr->set_available(prop_name);
	
	// if the property is an inter-object relation, change the value to the psychological name
	new_value = change_related_object_name(prop_name, new_value);
	
//	if(prop_name == Color_c)
//		Normal_out << "Set Color: " << obj_ptr->get_name() << ' ' << new_value << endl;
	
	// go ahead and send the property forward
	// queue a message to the sensory store
	schedule_event(new Visual_Change_Property_event(get_time() + delay, 
		get_human_ptr()->get_visual_sensory_store_ptr(), name_map.get_psychological_name(physical_name), prop_name, new_value));
}

// returns true if property prop_name is available at this eccentricity for this object,
// sets prop_value to new value if applicable
bool Eye_processor::apply_availability(shared_ptr<Visual_store_object> obj_ptr, Symbol prop_name, Symbol& prop_value, long& delay)
{
	// look up availability and delay functions in maps
	Availability_map_t::iterator it_availability = availabilities.find(prop_name);
	if(it_availability == availabilities.end()) {
		throw Unknown_physical_property(this, prop_name);
		}
	shared_ptr<Availability> a_ptr = it_availability->second;
	delay = 0; // initialize caller's delay
//	double ecc = eccentricity_fluctuation.get_double_value(); // should delete this parameter 8/5/15
	bool available = a_ptr->available(obj_ptr);
	if(available)
		delay = a_ptr->delay(obj_ptr);
    // count the available for Text if this is the closest object
//	if(prop_name == Text_c && obj_ptr == closest_object_ptr) {
/*	if(prop_name == Text_c) {
		availability_accumulator.update(available);  //obj_ptr->get_eccentricity()
		if(available)
			availability_distribution.update(obj_ptr->get_eccentricity());
		}
*/
/*	// if property is Color and not available, check to see if should make it Vague Color available
	if(prop_name == Color_c && !available) {
	Normal_out << "Color not available for " << obj_ptr->get_name() << endl;
		auto it_vague = availabilities.find(Color_Vague_c);
		if(it_vague == availabilities.end()) {
			throw Unknown_physical_property(this, Color_Vague_c);
			}
		shared_ptr<Availability> v_ptr = it_vague->second;
		bool vague_available = v_ptr->available(obj_ptr);
		if(vague_available) {
	Normal_out << "Color should be vague for " << obj_ptr->get_name() << endl;
//			prop_value = Vague_c;	// new color is "Vague"
			prop_value = Black_c;
			available = true;	// Color property is now available
			delay = v_ptr->delay(obj_ptr);
			}
		}
	*/
	if(delay)
		delay = delay * property_time_fluctuation.get_double_value();
//	if(prop_name == Color_c)
//		Normal_out << "Color apply_availability result for " << obj_ptr->get_name() << ' ' << available << " as " << prop_value << endl;
	return available;
}

// retrieve or create the psychological name and set to new_object if not already known
Symbol Eye_processor::get_psychological_name_and_new_status(Symbol physical_name, bool& new_object)
{
	// get or create the psychological name
	Symbol psychological_name;
	if(!name_map.is_physical_name_present(physical_name)) {
			new_object = true;
			if(physical_name == Cursor_name_c || physical_name == Tracking_cursor_name_c) {
				// this becomes the psychological name as well
				psychological_name = physical_name;
				}
			else {
				psychological_name = concatenate_to_Symbol(physical_name, "_V", ++psychobj_counter);
				}
		// add it to the name maps
		name_map.add_names(physical_name, psychological_name);
		}
	else {
		new_object = false;
		psychological_name = name_map.get_psychological_name(physical_name);
		}
	return psychological_name;
}

// Update the object's location-based information, return true if object is currently visible or has gone out of view
// meaning that its properties need checked for availability
bool Eye_processor::update_location_information(shared_ptr<Visual_store_object> obj_ptr)
{
	bool old_visibility = obj_ptr->is_visible();
	// - dk 082909
//	obj_ptr->update_eccentricity_and_visibility(eye_location, standard_peripheral_radius);
//	bool new_visibility = obj_ptr->is_visible();
	bool new_visibility = update_eccentricity_and_visibility(obj_ptr);
	Symbol physical_name = obj_ptr->get_name();
	// we are going to be sending info about the object forward	
	// get or create the psychological name and the status
	bool new_object = false;
	Symbol psychological_name = get_psychological_name_and_new_status(physical_name, new_object);
	
	// if the object is visible now, and wasn't before, either it has appeared/come into view
	if(new_visibility) {
		if(!old_visibility) {	// came into view or appeared
			if(get_trace() && Trace_out)
				Trace_out << processor_info() << ((new_object) ? " new" : " reinstated") << " object " << physical_name << '/' << psychological_name << " has come into view" << endl;
			// the physical name is now in use for an object that is present
			name_map.set_physical_name_in_use(physical_name, true);
			long appearance_delay_time = appearance_disappearance_delay.get_long_value();
			schedule_event(new Visual_Appear_event(
				get_time() + appearance_delay_time, get_human_ptr()->get_visual_sensory_store_ptr(),
				psychological_name, location_noise(obj_ptr->get_location()), obj_ptr->get_size()));
			}
		// either way, now update where it is relative to the eye position
		send_eccentricity_update(psychological_name, obj_ptr->get_eccentricity());
		return true;
		}
	// if it is not currently visible, but was visible before or sometime in the past, it has gone out of view
	// if it is not currently visible, but was visible before, it has gone out of view
//	else if(old_visibility || old_was_visible) {
	else if(old_visibility) {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << " object " << physical_name << '/' << psychological_name << " has gone out of view" << endl;
		// the physical name is now now longer in use because the object isn't visually present any more
		name_map.set_physical_name_in_use(physical_name, false);
		// first update where it is relative to the eye position - even though it is disappearing
		// below might fail if eccentricity update arrives after object is gone
		send_eccentricity_update(psychological_name, obj_ptr->get_eccentricity());
		schedule_event(new Visual_Disappear_event( 
			get_time() + appearance_disappearance_delay.get_long_value(), get_human_ptr()->get_visual_sensory_store_ptr(), psychological_name));
		return true;
		}
	// otherwise the object is not visible and was never visible, so there is nothing to do
	return false;
}

void Eye_processor::send_eccentricity_update(const Symbol& psychological_name, double ecc)
{
	Symbol eccentricity_value(ecc);
	const Symbol& eccentricity_zone_value = (ecc <= standard_fovea_radius) ?
		Fovea_c : Periphery_c;
	schedule_event(new Visual_Change_Property_event(get_time() + eccentricity_delay.get_long_value(),
		get_human_ptr()->get_visual_sensory_store_ptr(), psychological_name, Eccentricity_c, eccentricity_value));
	schedule_event(new Visual_Change_Property_event(get_time() + eccentricity_delay.get_long_value(),
		get_human_ptr()->get_visual_sensory_store_ptr(), psychological_name, Eccentricity_zone_c, eccentricity_zone_value));
}

// Send the visual processor an update to all property values according to their visibility,
// which has changed due to a change in location, size, or an eye movement.
void Eye_processor::update_all_properties(shared_ptr<Visual_store_object> obj_ptr)
{
	// access the object
	Symbol_list_t prop_names;
	Symbol_list_t prop_values;
	// this contains only properties with non-default values
	obj_ptr->get_pv_lists(prop_names, prop_values);
	Symbol_list_t ::iterator it_name = prop_names.begin();
	Symbol_list_t ::iterator it_value = prop_values.begin();
	for(;it_name != prop_names.end() && it_value != prop_values.end(); ++it_name, ++it_value)
		send_property_availability_update(obj_ptr, *it_name, *it_value);
}


/* 
Send the visual sensory store a message for every current property that says whether the property
is present (the current value is sent) or absent(the default value is sent). 
The visual sensory store will remove properties that are absent.
*/
void Eye_processor::send_property_availability_update(shared_ptr<Visual_store_object> obj_ptr, 
	const Symbol& prop_name, const Symbol& prop_value)
{
	// get the old availability
	bool old_available = obj_ptr->is_available(prop_name);
	
	long delay = 0;
	Symbol new_value = prop_value;
	
	bool available = apply_availability(obj_ptr, prop_name, new_value, delay);
//	if(prop_name == Color_c)
//		Normal_out << "in send_property_availability_update after apply_availability " <<
//			obj_ptr->get_name() << " color " << available << " as " << new_value << endl;

	// do nothing more if no change in availability and no change in value
//	if(available == old_available && prop_value == new_value)
//		return;
	// do nothing more if no change in availability
	if(available == old_available)
		return;
	
	// update the availability and the value;
	if(available) {
		obj_ptr->set_available(prop_name);
//		new_value = prop_value;
		}
	else {
		obj_ptr->set_unavailable(prop_name);
		new_value = Nil_c;
		}	
	delay = delay * property_time_fluctuation.get_double_value();
	Symbol psychological_name = name_map.get_psychological_name(obj_ptr->get_name());
	// check for an inter-object relation, and make the new value the psychological name
	new_value = change_related_object_name(prop_name, new_value);
	// go ahead and send the property forward
	schedule_event(new Visual_Change_Property_event(get_time() + delay, 
		get_human_ptr()->get_visual_sensory_store_ptr(), psychological_name, prop_name, new_value));
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "Availability change for object " << psychological_name 
			<< " property " << prop_name << " to " << new_value << endl;
}

// This should be fixed so that these are not hard coded!
Symbol Eye_processor::change_related_object_name(const Symbol& prop_name, const Symbol& prop_value)
{
	Symbol new_value = prop_value;
	if(prop_value == Nil_c)
		return new_value;
	if(	prop_name == Left_of_c || 
		prop_name == Right_of_c || 	
		prop_name == Above_c || 	
		prop_name == Below_c || 	
		prop_name == Inside_c || 	
		prop_name == In_center_of_c || 	
		prop_name == Outside_c || 	
		prop_name == In_row_c || 	
		prop_name == In_col_c || 	
		prop_name == Placed_on_c	
		) {
		new_value = name_map.get_psychological_name(prop_value);
		}
	return new_value;
}


void Eye_processor::handle_event(const Eye_Voluntary_Saccade_Start_event * event_ptr)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "Voluntary_Saccade_Start to "
			 << event_ptr->new_eye_location << endl;
	voluntary_saccade_underway = true;
	// if we are doing a voluntary movement, we can't possibly be doing an involuntary one - it might have gotten cancelled!
//	involuntary_movement_in_progress = false;
	eye_movement_complete_time = event_ptr->done_time;
	future_eye_location = event_ptr->new_eye_location;
}

void Eye_processor::handle_event(const Eye_Voluntary_Saccade_End_event * event_ptr)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "Voluntary_Saccade_End to"
			 << future_eye_location << endl;
	voluntary_saccade_underway = false;
	eye_movement_complete_time = 0;
	eye_location = future_eye_location;

	notify_views(&View_base::notify_eye_movement, eye_location);

	// centering is turned on by end of saccade only if it is enabled.
	if(event_ptr->new_centering_mode && centering_enabled) {
		centering_on = true;
		}
	else {
		centering_on = false;
		centering_active = false;
		}
		
	// reflex is turned on by end of saccade only if it is enabled.
	if(event_ptr->new_reflex_mode && reflex_enabled) {
		reflex_on = true;
		}
	else {
		reflex_on = false;
		}
	update_objects_after_eye_movement();
	perform_centering();
}

void Eye_processor::handle_event(const Eye_Involuntary_Saccade_Start_event * event_ptr)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "Involuntary_Saccade_Start to "
			 << event_ptr->new_eye_location << endl;
	involuntary_saccade_underway = true;
	eye_movement_complete_time = event_ptr->done_time;
	future_eye_location = event_ptr->new_eye_location;
}

void Eye_processor::handle_event(const Eye_Involuntary_Saccade_End_event *)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "Involuntary_Saccade_End to"
			 << future_eye_location << endl;
	involuntary_saccade_underway = false;
//	involuntary_movement_in_progress = false;
	eye_movement_complete_time = 0;
	eye_location = future_eye_location;

	notify_views(&View_base::notify_eye_movement, eye_location);

	update_objects_after_eye_movement();
	// always go into centering mode if it is enabled after an involuntary saccade
	if(centering_enabled)
		centering_on = true;
	// always center after an involuntary saccade
	perform_centering();
}

void Eye_processor::handle_event(const Eye_Involuntary_Smooth_Move_Start_event * event_ptr)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "Involuntary_Smooth_Move_Start to "
			 << event_ptr->new_eye_location << endl;
	involuntary_smooth_move_underway = true;
	eye_movement_complete_time = event_ptr->done_time;
	future_eye_location = event_ptr->new_eye_location;
}

void Eye_processor::handle_event(const Eye_Involuntary_Smooth_Move_End_event *)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "Involuntary_Smooth_Move_End to"
			 << future_eye_location << endl;
	involuntary_smooth_move_underway = false;
//	involuntary_movement_in_progress = false;
	eye_movement_complete_time = 0;
	eye_location = future_eye_location;

	notify_views(&View_base::notify_eye_movement, eye_location);

	update_objects_after_eye_movement();
}

void Eye_processor::update_objects_after_eye_movement()
{
	update_all_object_location_information();
	update_closest_object();
	update_all_object_properties();
//	update_closest_object();
}

/* These functions are repetitious in the loop structure, but done that way for now for clarity */

// This function updates the eccentricities of all objects and their other location parameters
void Eye_processor::update_all_object_location_information()
{
	Visual_physical_store * visual_physical_store_ptr = get_human_ptr()->get_Visual_physical_store_ptr();
	// scan all objects from a list of their ptr's
	const Visual_store::Store_object_ptr_list_t& object_ptrs = visual_physical_store_ptr->get_object_ptr_list();
	for(Visual_store::Store_object_ptr_list_t::const_iterator it = object_ptrs.begin(); it != object_ptrs.end(); ++it) {
		update_location_information(*it);
		}
}

// This function updates the properties of all objects based on current eccentricity information
void Eye_processor::update_all_object_properties()
{
	Visual_physical_store * visual_physical_store_ptr = get_human_ptr()->get_Visual_physical_store_ptr();
	// scan all objects from a list of their ptr's
	const Visual_store::Store_object_ptr_list_t& object_ptrs = visual_physical_store_ptr->get_object_ptr_list();
	for(Visual_store::Store_object_ptr_list_t::const_iterator it = object_ptrs.begin(); it != object_ptrs.end(); ++it) {
		// only do this if the object is visible - dk 082909
		if((*it)->is_visible())
			update_all_properties(*it);
		}
}

// After an object has moved, check for the new closest object
void Eye_processor::update_closest_object()
{
	Visual_physical_store * visual_physical_store_ptr = get_human_ptr()->get_Visual_physical_store_ptr();
	// scan all objects from a list of their ptr's
	const Visual_store::Store_object_ptr_list_t& object_ptrs = visual_physical_store_ptr->get_object_ptr_list();
	for(Visual_store::Store_object_ptr_list_t::const_iterator it = object_ptrs.begin(); it != object_ptrs.end(); ++it) {
		shared_ptr<Visual_store_object> obj_ptr = *it;
		double eccentricity = obj_ptr->get_eccentricity();
		if(it == object_ptrs.begin()) {
			closest_eccentricity = eccentricity;
			closest_object_ptr = obj_ptr;
			}
		else if(eccentricity < closest_eccentricity) {
			closest_eccentricity = eccentricity;
			closest_object_ptr = obj_ptr;
			}
		}
}




// This function assumes that update_objects_after_eye_movement() already done
// it uses the closet_object and closest_eccentricity to decide what to do.
// Note that ocular processors get location from sensory space, 
// so centering needs to be correspondingly delayed. But note also that
// the to-be-centered object is determined based on the physical locations, not the
// sensory ones!

void Eye_processor::perform_centering()
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "perform_centering mode " << centering_enabled << ' ' << centering_on << ' ' << free_to_move() << endl;
	// if we are not centering, and the eye is busy, don't go any further
	if(!(centering_enabled && centering_on) || !free_to_move())
		return;
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "perform_centering checks " << closest_eccentricity << ' ' << bouquet_radius.get_double_value() << ' ' << centering_smooth_move_radius.get_double_value() << ' ' << centering_saccade_radius.get_double_value() << endl;
	if(closest_eccentricity < bouquet_radius.get_double_value())
		return;	// an object is already centered
	else if(closest_eccentricity < centering_smooth_move_radius.get_double_value()) {
		centering_active = true;
//		involuntary_movement_in_progress = true; // this gets reset for either a voluntary or involuntary movement
		// tell involuntary ocular processor to do an involuntary saccade to the closest object
		// tell involuntary ocular processor to do a smooth move to the closest object
		shared_ptr<Motor_action> action_ptr (new Invol_ocular_Smooth_move_action(
				get_human_ptr(), name_map.get_psychological_name(closest_object_ptr->get_name())));
		last_invol_ocular_command_time = get_time() + location_delay.get_long_value() + inform_invol_ocular_delay.get_long_value();
		action_ptr->dispatch(last_invol_ocular_command_time);
		}
	else if(closest_eccentricity < centering_saccade_radius.get_double_value()) {
		centering_active = true;
//		involuntary_movement_in_progress = true; // this gets reset for either a voluntary or involuntary movement
		// tell involuntary ocular processor to do an involuntary saccade to the closest object
		shared_ptr<Motor_action> action_ptr(new Invol_ocular_Saccade_action(
				get_human_ptr(), name_map.get_psychological_name(closest_object_ptr->get_name())));
		last_invol_ocular_command_time = get_time() + location_delay.get_long_value() + inform_invol_ocular_delay.get_long_value();
		action_ptr->dispatch(last_invol_ocular_command_time);
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << "Invol_ocular_Saccade_action commanded for " << last_invol_ocular_command_time << endl;
		}
	else {
		// if there is nothing within centering range, eye stays put, but reflex turned on if enabled
		centering_active = false;
		if(reflex_enabled)
			reflex_on = true;
		}

}

// this function returns true if it makes sense to command an involuntary eye movement - that is, any type of movement is
// neither underway nor be preparing; the situation will simply be changing too soon

bool Eye_processor::free_to_move() const
{
	// if our last involuntary ocular command has not yet had time to arrive
	if(get_time() < last_invol_ocular_command_time)
		return false;
	// if an eyemovement is underway
	else if (voluntary_saccade_underway || involuntary_saccade_underway || involuntary_smooth_move_underway)
		return false;
	// free if the involuntary ocular processors is free to accept a new command
	// and the voluntary Ocular process is not busy at all - no e.m. in progress in any way
	// try modality free for the involuntary processor - was just processor_free - 5/10/04
	else
		return (get_human_ptr()->get_Invol_ocular_processor_ptr()->get_modality_free() &&
				get_human_ptr()->get_Ocular_processor_ptr()->get_modality_free());
/*	return (
		!(
			voluntary_saccade_underway || involuntary_saccade_underway || 
			involuntary_smooth_move_underway || involuntary_movement_in_progress
		) && 
		get_human_ptr()->get_Invol_ocular_processor_ptr()->get_processor_free() &&
		get_human_ptr()->get_Ocular_processor_ptr()->get_processor_free()
		);
*/
}



