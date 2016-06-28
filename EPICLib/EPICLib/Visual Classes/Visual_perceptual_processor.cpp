/* 2/19/09 -dk
Appearance/disappearance output events are constrained to have the same delay,
so that the perceptual store will get appear/disappear events in the same order
that they arrive at the perceptual processor.  Slower disappearance that appearance
is implemented strictly in terms of the persistence in the perceptual store.
*/

/* 1/30/09 - dk
NOT YET IMPLEMENTED:As part of new regime for appearance/come into view, perceptual
processor should no longer generate the Onset/Offset detection
events - thses should only happen for objects that appear while
the eye is stationary, which Eye_processor is best equipped to do -
these events seem like an "early vision" function as well.
*/


#include "Visual_perceptual_processor.h"
#include "Visual_sensory_store.h"
#include "Visual_perceptual_store.h"
#include "Visual_event_types.h"
#include "Epic_exception.h"
#include "Output_tee_globals.h"
#include "Epic_standard_symbols.h"
#include "Numeric_utilities.h"
#include "Visual_encoder_base.h"

#include <string>
#include <iostream>
#include <sstream>
//#include <algorithm>
//#include <functional>
#include <typeinfo>
#include <memory>

using std::string;
using std::endl;
using std::string;
using std::ostringstream;
using std::istringstream;
using std::shared_ptr;

//const long standard_short_delay = 25;
const long standard_delay = 50;
const long standard_long_delay = 100;


/*** Exception classes ***/ 
// a user-interpretable error
class Unknown_perceptual_property : public Epic_exception {
public:
Unknown_perceptual_property(Processor * proc_ptr, const Symbol& propname)
	{
		msg = proc_ptr->processor_info() + 
		"Unknown perceptual property received: " + propname.str();
	}
};

void Visual_perceptual_processor::setup()
{
	recoding_times[Eccentricity_c] = standard_delay;
	recoding_times[Eccentricity_zone_c] = standard_delay;
	recoding_times[Color_c] = standard_delay;
	recoding_times[Targetable_c] = standard_delay;
//	recoding_times[Size_c] = standard_delay;
	recoding_times[Encoded_size_c] = standard_delay;
	recoding_times[Shape_c] = standard_delay;
	recoding_times[Text_c] = standard_long_delay;
	recoding_times[Position_c] = standard_delay;
	recoding_times[Vposition_c] = standard_delay;
	recoding_times[Hposition_c] = standard_delay;
	recoding_times[In_row_c] = standard_delay;
	recoding_times[In_col_c] = standard_delay;
	recoding_times[Orientation_c] = standard_long_delay;
	recoding_times[Leader_c] = standard_delay;
	recoding_times[Left_of_c] = standard_delay;
	recoding_times[Right_of_c] = standard_delay;
	recoding_times[Above_c] = standard_delay;
	recoding_times[Below_c] = standard_delay;
	recoding_times[Inside_c] = standard_delay;
	recoding_times[In_center_of_c] = standard_delay;
	recoding_times[Outside_c] = standard_delay;
	recoding_times[Depth_c] = standard_long_delay;
	recoding_times[Distance_c] = standard_delay;
	
	add_parameter(appear_disappear_delay);
	add_parameter(onset_offset_decay_time);
	add_parameter(visible_delay);
//	add_parameter(onset_delay);
//	add_parameter(disappearance_delay);
//	add_parameter(offset_decay_time);
	add_parameter(location_delay);
	add_parameter(size_delay);
//	add_parameter(property_time_fluctuation);
	add_parameter(recoding_time_fluctuation);
//	add_parameter(text_recoding_failure_rate);
//	add_parameter(text_partial_recoding_rate);
	add_parameter(recoding_failure_rate1);
	add_parameter(recoding_failure_rate2);
	add_parameter(recoding_failure_rate3);
	add_parameter(recoding_failure_rate4);
}

// set overall state
void Visual_perceptual_processor::initialize()
{
	Human_subprocessor::initialize();
	
	if(visual_encoder_ptr)
		visual_encoder_ptr->initialize();
}

// set externally with an address supplied by a dynamic library - connects this processor back to it.
void Visual_perceptual_processor::set_visual_encoder_ptr(Visual_encoder_base * visual_encoder_ptr_)
{
	visual_encoder_ptr = visual_encoder_ptr_;
	visual_encoder_ptr->connect(this);
}

// specification contains <property> <time>
void Visual_perceptual_processor::set_parameter(const Parameter_specification& param_spec)
{
	if(param_spec.parameter_name == "Recoding_time") {
		istringstream iss(param_spec.specification);
		// first term is property name
		string property_name;
		iss >> property_name;
		if(!iss)
			Parameter::throw_parameter_error("Unable to read property name in parameter specification", param_spec);
		Symbol prop_name(property_name);
	// second term is delay value
		long delay;
		iss >> delay;
		if(!iss)
			Parameter::throw_parameter_error("Unable to read recoding delay in parameter specification", param_spec);
		recoding_times[prop_name] = delay;
		}
	else {
		Human_subprocessor::set_parameter(param_spec);
		}
}

void Visual_perceptual_processor::describe_parameters(Output_tee& ot) const
{
	Human_subprocessor::describe_parameters(ot);

	ot<< "Recoding_time:" << endl;
	for(Recoding_map_t::const_iterator it = recoding_times.begin(); it != recoding_times.end(); ++it) {
		ot << "    " << it->first << ": recoded after " << it->second << " ms." << endl;
		}
}


void Visual_perceptual_processor::accept_event(const Visual_event * event_ptr)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << typeid(*event_ptr).name() << " received" << endl;
	broadcast_to_recorders(event_ptr);
	// tell the event to handle itself with itself!
	event_ptr->handle_self(this);
}

void Visual_perceptual_processor::accept_event(const Stop_event *)
{
//	perceptual_space.display_contents(Normal_out);
}

void Visual_perceptual_processor::make_object_appear(const Symbol& name, GU::Point location, GU::Size size)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "make object " << name << " appear" << endl;
			
	// transmit the event forward to the next store
	// use the same appearance time value to avoid inconsistencies if randomized.
	long appear_time = get_time() + appear_disappear_delay.get_long_value();
	schedule_event(new Visual_Appear_event(
		appear_time, get_human_ptr()->get_visual_perceptual_store_ptr(),
		name, location, size));
	// create and send a transient property forward
	schedule_event(new Visual_Change_Property_event(
		appear_time, get_human_ptr()->get_visual_perceptual_store_ptr(),
		name, Detection_c, Onset_c));
	// arrange to remove the transient after its delay
	schedule_event(new Visual_Change_Property_event(
		appear_time + onset_offset_decay_time.get_long_value(), get_human_ptr()->get_visual_perceptual_store_ptr(),
		name, Detection_c, Nil_c));
		
	// send the "Visible" property forward after its own delay
	long visible_time = get_time() + visible_delay.get_long_value();
	schedule_event(new Visual_Change_Property_event(
		visible_time, get_human_ptr()->get_visual_perceptual_store_ptr(),
		name, Status_c, Visible_c));
	
}

void Visual_perceptual_processor::make_object_disappear(const Symbol& name)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "make object " << name << " disappear" << endl;

	// transmit the event forward to the next store
	// use the same disappearance time value to avoid inconsistencies if randomized.
	long disappear_time = get_time() + appear_disappear_delay.get_long_value();
	schedule_event(new Visual_Disappear_event(
		disappear_time, get_human_ptr()->get_visual_perceptual_store_ptr(),
		name));
	// create and send a transient property forward
	schedule_event(new Visual_Change_Property_event(
		disappear_time, get_human_ptr()->get_visual_perceptual_store_ptr(),
		name, Detection_c, Offset_c));
	// arrange to remove the transient after its delay
	schedule_event(new Visual_Change_Property_event(
		disappear_time + onset_offset_decay_time.get_long_value(), get_human_ptr()->get_visual_perceptual_store_ptr(),
		name, Detection_c, Nil_c));

}

// simply update the location stored with the object - 
void Visual_perceptual_processor::set_object_location(const Symbol& name, GU::Point location)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "set object " << name
			<< " location to " << location << endl;
			
	// transmit the event forward to the next store
	schedule_event(new Visual_Change_Location_event(
		get_time() + location_delay.get_long_value(), get_human_ptr()->get_visual_perceptual_store_ptr(),
		name,location));
}

// simply update the size stored with the object - 
void Visual_perceptual_processor::set_object_size(const Symbol& name, GU::Size size)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "set object " << name 
			<< " size to " << size << endl;

	// transmit the event forward to the next store
	schedule_event(new Visual_Change_Size_event(
		get_time() + size_delay.get_long_value(), get_human_ptr()->get_visual_perceptual_store_ptr(),
		name, size));
}
/*
const Symbol Onset_stationary_c ("Onset_stationary");
const Symbol Offset_stationary_c ("Offset_stationary");
*/

void Visual_perceptual_processor::set_object_property(const Symbol& object_name, const Symbol& property_name, const Symbol& property_value)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "set object " << object_name 
			<< " property " << property_name << " to " << property_value << endl;
/*			
	// special case for Detection Onset and Offset properties
	if(property_name == Detection_c && (property_value == Onset_stationary_c || property_value == Offset_stationary_c)) {
		// create and send a transient property forward
		long appear_time = get_time() + appear_disappear_delay.get_value();
		schedule_event(new Visual_Change_Property_event(
			appear_time, get_human_ptr()->get_visual_perceptual_store_ptr(),
			object_name, Detection_c, property_value));
		// arrange to remove the transient after its delay
		schedule_event(new Visual_Change_Property_event(
			appear_time + onset_offset_decay_time.get_value(), get_human_ptr()->get_visual_perceptual_store_ptr(),
			object_name, Detection_c, Nil_c));
		return;
		}
*/	

	// look up recoding time for this property - no substantive recoding
	Recoding_map_t::iterator it = recoding_times.find(property_name);
	if(it == recoding_times.end())
		throw Unknown_perceptual_property(this, property_name);
	long recoding_time = it->second;
	recoding_time = long(recoding_time * recoding_time_fluctuation.get_double_value());

	// default is that the value is already fully encoded			
	// check for and apply a custom encoding, providing the encoding time for this property
	// the custom encoding function returns true if its encoding should replace the default encoding
	if(visual_encoder_ptr && visual_encoder_ptr->set_object_property(object_name, property_name, property_value, recoding_time))
		return;

	// transmit the event forward to the next store
	schedule_event(new Visual_Change_Property_event(
		get_time() + recoding_time, get_human_ptr()->get_visual_perceptual_store_ptr(),
		object_name, property_name, property_value));
}

/*
	A delay event serves the purpose of allow a property to be transmitted after a time delay, 
	during which it can be cancelled by intervening events. A delay event contains
	the object, property, and values for a property-change event, 
	and the property-event-time at which the property is supposed to be delivered.
	If the delay event refers to a non-existent object, it is discarded.
	If the delay event arrives at a time that does not match the delivery time for the
	property that was stored with the object, it is discarded. Thus if the property event time
	has been set to zero, it means te delivery should be canceled. If the property event time
	mismatches and is larger, it means that a subsequent delivery has been scheduled, and this
	delivery should not be done.
*/
void Visual_perceptual_processor::handle_event(const Visual_Delay_event * event_ptr)
{
	Symbol object_name = event_ptr->object_name;
	Symbol property_name = event_ptr->property_name;
	Symbol property_value = event_ptr->property_value;
	// check to see if the object is still present, do nothing if not
	if(!(get_human_ptr()->get_Visual_sensory_store_ptr()->is_present(object_name))) {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << object_name 
				<< " no longer present for delay event of " << property_value << " set to " << property_value << endl;
		return;
		}

	// if an encoder has been set, apply it and do nothing more if it wants to override the default behavior
	if(visual_encoder_ptr && visual_encoder_ptr->handle_Delay_event(object_name, property_name, property_value))
		return;

	// access the object
	shared_ptr<Visual_sensory_object> obj_ptr = get_sensory_object_ptr(object_name);
	// check to see if the object's event time for the property and the property value matches this event; if not, do nothing
	long object_time = obj_ptr->get_property_event_time(property_name);
	Symbol object_property_value = obj_ptr->get_property_value(property_name);
	if(object_time != event_ptr->get_time() || object_property_value != property_value) {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << "Delay event discarded for " << object_name 
				<< " because event time and value " << event_ptr->get_time() << " " << property_value << " does not match stored " << object_time << " " << object_property_value << endl;
		return;
		}
	// here if event matches - reset the stored event time, and transmit the information immediately to the perceptual store and 
	// transmit the event forward to the next store
	obj_ptr->remove_property_event_time(property_name);
	schedule_event(new Visual_Change_Property_event(
		get_time(), get_human_ptr()->get_visual_perceptual_store_ptr(),
		object_name, property_name, property_value));
}

// This function seems to be required because my Smart_Pointer doesn't work properly
shared_ptr<Visual_sensory_object> Visual_perceptual_processor::get_sensory_object_ptr(const Symbol& object_name)
{
	shared_ptr<Visual_store_object> p = get_human_ptr()->get_Visual_sensory_store_ptr()->get_object_ptr(object_name);
	shared_ptr<Visual_sensory_object> obj_ptr = std::dynamic_pointer_cast<Visual_sensory_object>(p);
	if(!obj_ptr)
		throw Epic_internal_error(this, "object was not a Visual_sensory_object");
	return obj_ptr;
}

