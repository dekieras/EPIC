#include "Manual_aimed_actions.h"
#include "Coordinator.h"
#include "Output_tee_globals.h"
#include "Numeric_utilities.h"
#include "Manual_processor.h"
#include "Eye_processor.h"
#include "Ear_processor.h"
#include "Visual_perceptual_store.h"
#include "Auditory_perceptual_store.h"
#include "Cognitive_processor.h"
#include "Device_event_types.h"
#include "Device_processor.h"	// to allow conversion of Device_processor * to Processor * in schedule_event
#include "Symbol_utilities.h"
#include "Geometry.h"
#include "Epic_standard_symbols.h"

#include <cmath>
#include <iostream>
#include <memory>

namespace GU = Geometry_Utilities;
using std::endl;
using std::fabs;
using std::shared_ptr;


// All randomized fluctuations are done in the Motor_processor parameters, which includes 
// a single fluctuation parameter. Individual movement execution characteristics 
// are fixed parameters.



// this "virtual constructor" method returns a pointer to the created object
// based on the contents of the list of motor command parameters
// or zero if no object was created because the parameters are invalid
std::shared_ptr<Motor_action> Manual_aimed_action::create(Human_processor * human_ptr,
	Symbol_list_t arguments, bool execute_when_prepared)
{	
	if(arguments.size() < 2)
		return 0;
	
	const Symbol style = arguments.front();
	arguments.pop_front();
	if(style == Ply_c) {
		return shared_ptr<Motor_action>(new Manual_Ply_action(human_ptr, arguments, execute_when_prepared));
		}
	else if(style == Point_c) {
		// construct an argument list
		Symbol_list_t new_arguments;
		new_arguments.push_back(Cursor_name_c);
		new_arguments.push_back(arguments.front()); 
		new_arguments.push_back(Right_c);
		// accept an optional last parameter that is the size of the target object,
		// to be used in case the effective size is different from the visual size.
		arguments.pop_front();
		if(!arguments.empty())
			new_arguments.push_back(arguments.front());
		return shared_ptr<Motor_action>(new Manual_Point_action(human_ptr, new_arguments, execute_when_prepared));
		}
	else if(style == Click_on_c) {
		// construct an argument list
		Symbol_list_t new_arguments;
		new_arguments.push_back(Cursor_name_c);
		new_arguments.push_back(arguments.front()); 
		new_arguments.push_back(Right_c);
		// accept an optional last parameter that is the size of the target object,
		// to be used in case the effective size is different from the visual size.
		arguments.pop_front();
		if(!arguments.empty())
			new_arguments.push_back(arguments.front());
		return shared_ptr<Motor_action>(new Manual_Click_on_action(human_ptr, new_arguments, execute_when_prepared));
		}
	else
		return 0;
}



Manual_aimed_action::Manual_aimed_action(Human_processor * human_ptr_, Symbol_list_t arguments, bool execute_when_prepared, 
		const char * movement_description_cstr) :
	Manual_action(human_ptr_, execute_when_prepared)
{
	// argument list should contain cursor, target, hand, or an optional argument
	if(arguments.size() < 3 || arguments.size() > 4)
		throw Motor_action_exception(human_ptr_, "cursor, target, hand required; target size optional", arguments);
	cursor = arguments.front();
	arguments.pop_front();
	target = arguments.front();
	arguments.pop_front();
	hand = arguments.front();
	check_hand(hand);
	movement_description = cstr_to_Symbol_list(movement_description_cstr);
	arguments.pop_front();	// check optional argument - square size of object
	if(!arguments.empty()) {
		double x = arguments.front().get_numeric_value();
		target_size = GU::Size(x, x);
		}
	eye_ptr = human_ptr->get_Eye_processor_ptr();
	visual_perceptual_store_ptr = human_ptr->get_Visual_perceptual_store_ptr();
	auditory_perceptual_store_ptr = human_ptr->get_Auditory_perceptual_store_ptr();
}

Manual_aimed_action::Manual_aimed_action(Human_processor * human_ptr_, const Symbol&  cursor_, const Symbol& target_, 
	const Symbol& hand_, bool execute_when_prepared, const char * movement_description_cstr) : 
	Manual_action(human_ptr_, execute_when_prepared)
{
	cursor = cursor_;
	target = target_;
	hand = hand_;
	check_finger(hand);
	movement_description = cstr_to_Symbol_list(movement_description_cstr);
	eye_ptr = human_ptr->get_Eye_processor_ptr();
	visual_perceptual_store_ptr = human_ptr->get_Visual_perceptual_store_ptr();
	auditory_perceptual_store_ptr = human_ptr->get_Auditory_perceptual_store_ptr();
}

	
// return the time required for preparation - this accepts any previous aimed action as the same style
long Manual_aimed_action::prepare(long base_time)
{
	// get cursor, target locations, compute direction and extent of movement

	// if the currently executing movement is also a aimed action and the cursor object is the same,
	// then use that movement's target location as the cursor location for this movement
	shared_ptr<Manual_aimed_action> exptr = std::dynamic_pointer_cast<Manual_aimed_action>(get_processor_ptr()->get_executing_ptr());
	if(exptr && cursor == exptr->cursor)
		cursor_location = visual_perceptual_store_ptr->get_object_ptr(exptr->target)->get_location();
	else
		cursor_location = visual_perceptual_store_ptr->get_object_ptr(cursor)->get_location();
	// The target might be an object in either auditory or visual perceptual store. 
	// Check auditory first, because if in neither, the error is more likely to be useful for visual
	if(auditory_perceptual_store_ptr->is_sound_present(target)) {
		target_location = auditory_perceptual_store_ptr->get_sound_ptr(target)->get_location();
		// assume a fixed size for the target
		target_size = GU::Size(1., 1.);
		// get the physical name for the target to send to the device
		target_device_name = human_ptr->get_Ear_processor_ptr()->get_name_map().get_physical_name(target);
		}
	else {		
		target_location = visual_perceptual_store_ptr->get_object_ptr(target)->get_location();
		// if target size does not have the default-constructed value, use it instead of the target visual size
		if(target_size == GU::Size())
			target_size = visual_perceptual_store_ptr->get_object_ptr(target)->get_size();
		// get the physical name for the target to send to the device
		target_device_name = human_ptr->get_Eye_processor_ptr()->get_name_map().get_physical_name(target);
		}
	// get the physical name for the cursor to send to the device
	cursor_device_name = human_ptr->get_Eye_processor_ptr()->get_name_map().get_physical_name(cursor);
	
	// compute the direction and extent for the movement as a polar vector
	movement_vector = GU::Polar_vector(cursor_location, target_location);
	
	// compare the movement to the previous one
	int features = 0;
	shared_ptr<Manual_aimed_action> ptr = std::dynamic_pointer_cast<Manual_aimed_action>(get_processor_ptr()->get_previous_ptr());
	if(ptr) {
		// previous is also an aimed movement
		if(hand != ptr->hand)
// dk 090807 - no r, theta feature programming time cost, but charge for a change in style or hand
//			features = 3;		// change hands, need hand, r, theta features
			features = 1;		// change hands, need hand, r, theta features
/*
		else {	// same hand, r and theta independent
			if (fabs(movement_vector.r - ptr->movement_vector.r) > 2.0)
				features++;		// generate new r
			if (fabs(movement_vector.theta - ptr->movement_vector.theta) > GU_pi/4.0)
				features++;		// generate new theta
				}
*/
		}
	else {
//		features = 4;	// style, hand, r, theta
		features = 2;	// style, hand
		}
		
	long feature_preparation_time = long (features * get_processor_ptr()->get_feature_time());
	return base_time + feature_preparation_time;
}

// return the time at which the final movement is complete
// and send any additional messages in the meantime
// base time is the beginning of actual physical movement; initiation has already happened.
long Manual_aimed_action::execute(long base_time)
{
	Human_processor * human_ptr = get_processor_ptr()->get_human_ptr();
	Processor * cognitive_ptr = human_ptr->get_cognitive_ptr();

	// movement description form: (Motor Manual <style> <hand> <finger> Started|Finished)	
	movement_description.push_back(hand);
	movement_description.push_back(Started_c);


 	//	tell cog that movement has started at base_time, signal to be deleted after a delay
 	Coordinator::get_instance().schedule_event(new 
		Cognitive_Add_Clause_event(base_time, cognitive_ptr, movement_description));
 	Coordinator::get_instance().schedule_event(new 
		Cognitive_Delete_Clause_event(base_time + get_processor_ptr()->get_efferent_deletion_delay(),
		cognitive_ptr, movement_description));
 
	//	tell tactile that movement has started at base_time
	/* not implemented yet */
	
	// generate the desired sequence of events
	long completion_time = generate_movement_events(base_time);

//	tell tactile that the movement is done
	/* not implemented yet */
// 	tell cog that movement is finished when the movement is done
	movement_description.pop_back();	// discard "Started" term
	movement_description.push_back(Symbol("Finished"));
	Coordinator::get_instance().schedule_event(new 
		Cognitive_Add_Clause_event(completion_time, cognitive_ptr, movement_description));
 	Coordinator::get_instance().schedule_event(new 
		Cognitive_Delete_Clause_event(completion_time + get_processor_ptr()->get_efferent_deletion_delay(),
		cognitive_ptr, movement_description));

//	tell tactile the movement is finished
	/* not implemented yet */
//	the movement is complete at this final time	

	return completion_time;
}

// Services for aimed movements

// generate a sequence of Device_Ply_events at the Subevent_time intervals, terminating on the target
// at the final movement time
// install auto home ot mouse (or joystick) if hand is on keyboard
// - note that hand would now be on mouse, so keyboard keystroke now requires a home back
long Manual_aimed_action::generate_movement_events(long base_time)
{
	movement_time = long(determine_movement_time());
	double fluctuation = get_processor_ptr()->get_execution_fluctuation();
	movement_time += long((get_processor_ptr()->auto_home_to_mouse()) * fluctuation); // right hand used on mouse or joystick
//	movement_time += long((get_processor_ptr()->auto_home_to_mouse())); // right hand used on mouse or joystick

	long device_input_time = base_time + movement_time;

	// if the Manual_processor is being traced, output this info
	if(Trace_out & get_trace()) {
		Trace_out << get_processor_ptr()->processor_info() 
			<< " Aimed movement of " << cursor << '/' << cursor_device_name << " to " << target << '/' << target_device_name << endl;
		}
	
	// generate a string of intermediate position events with no specified target name
	// assuming linear here
	long subevent_time = get_processor_ptr()->get_parameter_ptr("Subevent_time")->get_long_value();
	int number_of_subevents = int(movement_time / subevent_time);	// integer division on purpose
	Assert(number_of_subevents > 0);
	double subevent_r = movement_vector.r / number_of_subevents;
	GU::Polar_vector subevent_pv(subevent_r, movement_vector.theta);
	GU::Cartesian_vector subevent_displacement(subevent_pv);
	const Symbol no_target;	// default ctor'd
	GU::Point current_location = cursor_location;
	// send n-1 subevents; the last one is separately generated to have the exact target position
	for(int i = 0; i < number_of_subevents - 1; i++) {
		current_location = current_location + subevent_displacement;
		Coordinator::get_instance().schedule_event(new 
			Device_Ply_event(base_time + i * subevent_time, get_device_ptr(), 
				cursor_device_name, no_target, current_location, subevent_pv));
		}
	// send the last event with the exact target location
	Coordinator::get_instance().schedule_event(new 
		Device_Ply_event(device_input_time, get_device_ptr(), cursor_device_name, 
			target_device_name, target_location, movement_vector));
			
	return base_time + movement_time;
}



// the target size and position is given by a location point and a size point
// the location (x, y) is the upper left corner
// the size (w, h) is the width, height
// the movement is made from the cursor location to the target center,
// the center is given by (x + w/2, y - h/2)
// apply the movement time fluctuation here,to allow independent override
double Manual_aimed_action::pointing_time(long coefficient, long minimum_time) const
{
	double target_d, target_s;
	double movement_time;
		
	// if the starting point is outside the target, calculate s & d, and corresponding Fitts time
	// if not, then pointing time is the minimum - why are we pointing to it if already there?
	if (get_movement_d_and_s(target_d, target_s))
		movement_time = modified_Welfords_Fitts_law(target_d, target_s, double(minimum_time), double(coefficient));
	else
		movement_time = double(minimum_time);
	
	double fluctuation = get_processor_ptr()->get_execution_fluctuation();
	return movement_time * fluctuation;
}


// If the cursor is outside the target, return d, the distance from the cursor location 
// to the center of the target, and s, the size of the target along the line of motion.
// If the cursor is inside the target, return false;
bool Manual_aimed_action::get_movement_d_and_s(double& d, double& s) const
{
	GU::Line_segment movement_line(cursor_location, target_location);
	GU::Line_segment clipped_line;
	if(compute_center_intersecting_line(movement_line, target_size, clipped_line)){
		// here if cursor location is outside the target
		d = movement_line.get_length();
		s = 2.* clipped_line.get_length();
		return true;
		}
	return false;
}


// Modified Welford's law with parameters
// index of difficulty = (distance/s) + .5
// check if target closer than S, 
//	if index_of_difficulty < 1., t = 0.
//	else, t = if slope * logb2(index_of_difficulty) // logb2 is my function for base two log
// pointing time = max(minimum, t)

double Manual_aimed_action::modified_Welfords_Fitts_law(double d, double s, double minimum_time, double coefficient) const
{
	double id = (d/s) + .5;
	double t = (id < 1.) ? 0. :
		coefficient * logb2(id);				// log2 is C99 standard function
	t = (t > minimum_time) ? t : minimum_time;	// correct for minimum movement duration
	return t;
}



/* Manual_Ply_action functions */

Manual_Ply_action::Manual_Ply_action(Human_processor * human_ptr_, Symbol_list_t arguments, bool execute_when_prepared) :
	Manual_aimed_action(human_ptr_, arguments, execute_when_prepared, "Motor Manual Ply") 
{}

	
double Manual_Ply_action::determine_movement_time() const
{
	long coefficient = get_processor_ptr()->get_parameter_ptr("Ply_coefficient")->get_long_value();
	long minimum_time = get_processor_ptr()->get_parameter_ptr("Ply_minimum_time")->get_long_value();
	return pointing_time(coefficient, minimum_time);
}


/* Manual_Point_action functions */

Manual_Point_action::Manual_Point_action(Human_processor * human_ptr_, Symbol_list_t arguments, bool execute_when_prepared) :
	Manual_aimed_action(human_ptr_, arguments, execute_when_prepared, "Motor Manual Point")
{
}
// to construct derived classes
Manual_Point_action::Manual_Point_action(Human_processor * human_ptr_, Symbol_list_t arguments, 
	bool execute_when_prepared, const char * movement_description_cstr) :
	Manual_aimed_action(human_ptr_, arguments, execute_when_prepared, movement_description_cstr)
{
}

// return the time required for preparation
// if the target size is unknown (actually, [0, 0]), default to two features (100 preparation time)
// along with an assumption that the mouse is always in the same hand.
long Manual_Point_action::prepare(long base_time)
{
	// let the base class do all the calculations for the movement, but we might not use its preparation time
	// it accepts any aimed movement as the same style
	long prep_done_time = Manual_aimed_action::prepare(base_time);
	
	// for any savings, the previous action must also be a Point action
	long feature_time = get_processor_ptr()->get_feature_time();
	shared_ptr<Manual_Point_action> ptr = std::dynamic_pointer_cast<Manual_Point_action>(get_processor_ptr()->get_previous_ptr());
	if(ptr) {
	// dk 090807 - no r, theta feature programming time cost, but charge for a change in style or hand
		// the target_size should already be stored
		// target_size = visual_perceptual_store_ptr->get_object_ptr(target)->get_size();
		if(target_size == GU::Size()) {
//			prep_done_time = base_time + 2 * feature_time;
			prep_done_time = base_time;
			}
		}
	else {
//			prep_done_time = base_time + 4 * feature_time;	// a new movement, style, hand, r, theta
			prep_done_time = base_time + 2 * feature_time;	// a new movement, style, hand
		}
	
	return prep_done_time;
}

// if the target size is unknown (actually, [0, 0]), default to 1.0 sec total execution time 
// (gives 1.1 total, with preparation added). 
double Manual_Point_action::determine_movement_time() const
{
	double movement_time;
	if(target_size == GU::Size()) {
		long KLM_point_time = get_processor_ptr()->get_parameter_ptr("KLM_point_time")->get_long_value();
		movement_time = double(KLM_point_time - 2 * get_processor_ptr()->get_feature_time() - get_processor_ptr()->get_initiation_time());
		}
	else {
		long coefficient = (get_processor_ptr()->get_button_held()) ?
			get_processor_ptr()->get_parameter_ptr("Point_coefficient_button_held")->get_long_value()
			:
			get_processor_ptr()->get_parameter_ptr("Point_coefficient")->get_long_value();
		long minimum_time = get_processor_ptr()->get_parameter_ptr("Point_minimum_time")->get_long_value();
		movement_time = pointing_time(coefficient, minimum_time);
		}
	return movement_time;
}

/* Manual_Click_on_action functions */

Manual_Click_on_action::Manual_Click_on_action(Human_processor * human_ptr_, Symbol_list_t arguments, bool execute_when_prepared) :
	Manual_Point_action(human_ptr_, arguments, execute_when_prepared, "Motor Manual Click_on"), keyname(Mouse_Left_Button_c)
{
}

// return the time required for preparation, using the Point action calculations
long Manual_Click_on_action::prepare(long base_time)
{
	// let the base class do all the calculations for the movement, but we might not use its preparation time
	// it accepts any Manual_Point_action movement as the same style
	long prep_done_time = Manual_Point_action::prepare(base_time);
	
	// for any savings, the previous action must also be a Click_on action
	long feature_time = get_processor_ptr()->get_feature_time();
	shared_ptr<Manual_Click_on_action> ptr = std::dynamic_pointer_cast<Manual_Click_on_action>(get_processor_ptr()->get_previous_ptr());
	// dk 090807 - no r, theta feature programming time cost, but charge for a change in style or hand
	if(!ptr) {
//		prep_done_time = base_time + 5 * feature_time;	// a new movement style, hand, finger, r, theta
		prep_done_time = base_time + 3 * feature_time;	// a new movement style, hand, finger
		}
	
	return prep_done_time;
}



long Manual_Click_on_action::generate_movement_events(long base_time)
{
	// do the ply part of the compound movement
	base_time = Manual_aimed_action::generate_movement_events(base_time); // does auto-home to mouse
	
	// this does not auto-home to keyboard because the keyboard is not used - just for convenience
	// a Keystroke event is sent to the device - it shouldn't have to care what the movement style was
	// not obviously the best choice here - somewhat confusing
	// now generate the keystroke at the end (cf Punch action)
	//	tell device that key has closed at base_time + key closure time
	long key_closure_time = get_processor_ptr()->get_parameter_ptr("Key_closure_time")->get_long_value();
	double fluctuation = get_processor_ptr()->get_execution_fluctuation();
	long device_input_time = base_time + long(key_closure_time * fluctuation);
	Coordinator::get_instance().schedule_event(new 
		Device_Keystroke_event(device_input_time, get_device_ptr(), keyname));
	//	the downstroke is done at start time + burst time
	long downstroke_done_time = base_time + get_processor_ptr()->get_burst_time();

	//	to complete the movement, the finger must be upstroked, which takes an additional burst time
	long upstroke_done_time = downstroke_done_time + get_processor_ptr()->get_burst_time();
	//	the movement is complete at this final time	

	return upstroke_done_time;

}



