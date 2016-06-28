#include "Ocular_actions.h"
#include "Ocular_processor.h"
#include "Coordinator.h"
#include "Cognitive_processor.h"
#include "Visual_perceptual_store.h"
#include "Ear_processor.h"
//#include "Auditory_physical_store.h"
#include "Auditory_perceptual_store.h"
#include "Output_tee_globals.h"
#include "Epic_standard_symbols.h"
#include "Eye_processor.h"
#include "Eye_event_types.h"
#include "Device_event_types.h"	// to allow device to eye-track
#include "Device_processor.h"	// to allow conversion of Device_processor * to Processor * in schedule_event
#include "Random_utilities.h"
#include "Symbol_utilities.h"
#include "Geometry.h"
namespace GU = Geometry_Utilities;

#include <cmath>
#include <iostream>
#include <memory>
#include <sstream>

using std::string;
using std::atan2; using std::fabs;
using std::endl;
using std::shared_ptr; using std::make_shared;
using std::ostringstream;

class Unknown_ocular_target : public Epic_exception {
public:
Unknown_ocular_target(const Symbol& obj_name)
	{
		msg = "*** Error: Attempt to move eyes to unknown store object: " + obj_name.str();
	}
};

class Distant_ocular_target : public Epic_exception {
public:
Distant_ocular_target(const Symbol& obj_name, GU::Point location, const GU::Size bounding_box)
	{
        ostringstream oss;
        oss << "*** Error: Attempt to move eyes to object "
            << obj_name << " at location: " << location << " outside of " << bounding_box;
		msg = oss.str();
	}
};
const Symbol Random_Move_c("Random_Move");

const Symbol Random_location_c("Random_location");

// this "virtual constructor" method returns a pointer to the created object
// based on the contents of the list of motor command parameters
// or zero if no object was created because the parameters are invalid
// awkward, but for overall symplicity, we do the arguments list parsing in this function
// for the EPIC-style actions so that the constructors are simpler
shared_ptr<Motor_action> Ocular_action::create(Human_processor * human_ptr,
	Symbol_list_t arguments, bool execute_when_prepared)
{	
	if(arguments.size() < 2)
		return nullptr;
	
	const Symbol style = arguments.front();
	arguments.pop_front();
	if(style == Move_c) {
 		return make_shared<Ocular_Move_action>(human_ptr, arguments, execute_when_prepared);
		}
	else if(style == Random_Move_c) {
		return make_shared<Ocular_Random_Move_action>(human_ptr, arguments, execute_when_prepared);
		}
	else if(style == Set_mode_c) {
		return make_shared<Ocular_Mode_action>(human_ptr, arguments, execute_when_prepared);
        }
	else if(style == Look_for_c) {
		return make_shared<Ocular_Look_for_action>(human_ptr, arguments, execute_when_prepared);
		}
	else
		return nullptr;
}

Ocular_action::Ocular_action(Human_processor * human_ptr_, bool execute_when_prepared) :
		Motor_action(human_ptr_, execute_when_prepared)
{
	proc_ptr = human_ptr->get_Ocular_processor_ptr();
	eye_ptr = human_ptr->get_Eye_processor_ptr();
	visual_perceptual_store_ptr = human_ptr->get_Visual_perceptual_store_ptr();
	auditory_perceptual_store_ptr = human_ptr->get_Auditory_perceptual_store_ptr();
}

double Ocular_action::get_saccade_distance_intercept()
{return proc_ptr->saccade_distance_intercept.get_double_value();}
double Ocular_action::get_saccade_distance_slope()
{return proc_ptr->saccade_distance_slope.get_double_value();}
double Ocular_action::get_saccade_SD_intercept()
{return proc_ptr->saccade_SD_intercept.get_double_value();}
double Ocular_action::get_saccade_SD_slope()
{return proc_ptr->saccade_SD_slope.get_double_value();}

double Ocular_action::get_saccade_duration_intercept()
{return proc_ptr->saccade_duration_intercept.get_double_value();}

double Ocular_action::get_saccade_duration_slope()
{return proc_ptr->saccade_duration_slope.get_double_value();}

double Ocular_action::get_execution_fluctuation()
{return proc_ptr->get_execution_fluctuation();}


// this object will send itself to the appropriate processor
void Ocular_action::dispatch(long time)
{
	human_ptr->schedule_event(new Motor_Command_Action_event(
		time, 
		human_ptr->get_ocular_ptr(),
		shared_from_this()
		));
}

Ocular_Saccade_action::Ocular_Saccade_action(Human_processor * human_ptr_, bool execute_when_prepared) :
		Ocular_action(human_ptr_, execute_when_prepared)
    {}

void Ocular_Saccade_action::set_target_object_source_and_location(GU::Point opt_loc)
{
	// determine the source of the object and save it
	// if the objname_ is Nil, then do not try to locate it ...
	if(obj_name == Nil_c) {
		object_source = Object_source_e::NONE;
        obj_location = opt_loc; // default (0, 0)
		return;
		}
	if(get_visual_perceptual_store_ptr()->is_present(obj_name)) {
		object_source = Object_source_e::VISUAL_OBJECT;
        obj_location = get_visual_perceptual_store_ptr()->get_object_ptr(obj_name)->get_location();
        return;
        }
	if(get_auditory_perceptual_store_ptr()->is_sound_present(obj_name)) {
//	else if(auditory_perceptual_store_ptr->is_stream_present(obj_name))
		object_source = Object_source_e::AUDITORY_SOURCE;
        obj_location = get_auditory_perceptual_store_ptr()->get_sound_ptr(obj_name)->get_location();
        return;
        }
    if(obj_name == Random_location_c) {
        object_source = Object_source_e::RANDOM;
        obj_location = opt_loc;
        Assert(obj_location != GU::Point()); // should be already set
        return;
        }
	// is the target a named location?
	bool found;
    obj_location = human_ptr->get_Cognitive_processor_ptr()->get_named_location(obj_name, found); // awkward - no is_present?
	if(found) {
		object_source = Object_source_e::NAMED_LOCATION;
        return;
        }
	else
		throw Unknown_ocular_target(obj_name);






}

// this preparation can be shared by all voluntary ocular saccade actions
// derived classes need to supply an object name and a location for the eye movement destination
long Ocular_Saccade_action::prepare(long base_time)
{
    Assert(obj_name != Nil_c);
    // Assert(obj_location != GU::Point()); // It is possible for the location to validly be (0, 0)

	// get the current eye position - will be future location if it is in motion
	GU::Point current_eye_location = get_eye_ptr()->get_location();
	// compute the direction and extent for the eye movement as a polar vector
	move_vector = GU::Polar_vector(current_eye_location, obj_location);
	
	// no - must allow a dummy move to an object already in view, but no change here - 2/22/08 DK
	// if the eye is already in position, this move needs to be aborted
	if (move_vector.r < 0.1) {
		move_vector.r = 0.0;	// distance to be moved is really zero
		return base_time;
		}
    // here, move_vector.r is the actual distance to the object
	// check to see if movement is greater than a minimum before adding any noise
	// if not, movement will be accurate
//	if(move_vector.r > 2.5) { // try without 12/4/14
    {
		// modify r using the saccade distance parameters - gain and sd
		double distance_intercept = get_saccade_distance_intercept();
		double distance_slope = get_saccade_distance_slope();
		// calculate actual distance by applying gain parameters
		double base_distance = distance_intercept + move_vector.r * distance_slope;
		// apply noise that depends on the actual distance
		double SD_intercept = get_saccade_SD_intercept();
		double SD_slope = get_saccade_SD_slope();
		double SD = SD_intercept + base_distance * SD_slope;
        double actual_distance = base_distance;
        // if distance is 50, .1 times that gives sd of 5,3 sds out is 15 degrees, saccade of 75 degrees
		if(SD > 0.0) {
            do {actual_distance = normal_random_variable(base_distance, SD);
                } while (actual_distance < 0. || ((actual_distance - base_distance) / SD) > 3.0);
			}
		move_vector.r = actual_distance;
        eye_destination = current_eye_location + move_vector;
        const GU::Size bounding_box(100.,100.);  // 50 degrees to each side, 50 degrees above and below
        if(!is_point_inside_rectangle(eye_destination, GU::Point(0.,0.), bounding_box))
            throw Distant_ocular_target(obj_name, eye_destination, bounding_box);
		}
    
    // dk 090807 - no feature programming time charge for r, theta
	long feature_preparation_time = 0;
	return base_time + feature_preparation_time;
}


// look up and return the physical name for the target object from its source
Symbol Ocular_Saccade_action::get_target_object_device_name() const
{
	Symbol result;
	switch(object_source) {
		case Object_source_e::NAMED_LOCATION:
		case Object_source_e::RANDOM:
			result = obj_name;	// same as orginal name
			break;
		case Object_source_e::VISUAL_OBJECT:
			result = human_ptr->get_Eye_processor_ptr()->get_name_map().get_physical_name(obj_name);
			break;
		case Object_source_e::AUDITORY_SOURCE:	// notice the difference between location of name_map
			result = human_ptr->get_Ear_processor_ptr()->get_name_map().get_physical_name(obj_name);
			break;
		default:
			throw Unknown_ocular_target(obj_name);
			break;
		}
	return result;
}

long Ocular_Saccade_action::compute_saccade_execution_time(long base_time)
{
	if (move_vector.r == 0.0)
		return base_time;
		
	double duration_intercept = get_saccade_duration_intercept();
	double duration_slope = get_saccade_duration_slope();
	double fluctuation = get_execution_fluctuation();
	long execution_time = long(duration_intercept + move_vector.r * duration_slope * fluctuation);
	return base_time + execution_time;
}

// return the time at which the final movement is complete
// and send any additional messages in the meantime
long Ocular_Saccade_action::execute(long base_time)
{
	long movement_completion_time = compute_saccade_execution_time(base_time);

	// if no move needs to be made, return no additional movement time, and send no events
	// to the eye processor
	// change made so that a move to the same object as current fixated one signals the eye 
	// as if a zero-length eye movement has been made.  DK 2/22/08
//	if (movement_completion_time == base_time)	// see preparation code
//		return base_time;

	// send the eye movement start event at the current time to the eye
/*	Coordinator::get_instance().schedule_event(new
		Eye_Voluntary_Saccade_Start_event(
			base_time, get_eye_ptr(), get_target_object_location(), movement_completion_time)); */
	Coordinator::get_instance().schedule_event(new 
		Eye_Voluntary_Saccade_Start_event(
			base_time, get_eye_ptr(), eye_destination, movement_completion_time));
			
	// send the eye movement end event at the movement_completion_time time to the eye
	// set centering on, reflexes off
	// add 1 to the movement_completion time so that the motor processor will change to complete state before
	// the eye processor will ask for a centering to take place - DK 4/23/16
	Coordinator::get_instance().schedule_event(new 
		Eye_Voluntary_Saccade_End_event(
			movement_completion_time + 1, get_eye_ptr(), true, false));
			
	// do not signal the device if the e.m. is zero length  DK 2/22/08
	if (movement_completion_time == base_time)	// see preparation code
		return base_time;
	
	// signal the device (to allow eye tracking)
	// use either the actual object name or the named location name
	Symbol obj_device_name = get_target_object_device_name();
/*	Symbol obj_device_name = (get_actual_object()) ?
		human_ptr->get_Eye_processor_ptr()->get_name_map().get_physical_name(get_objname())
		:
		get_objname();
*/
/*	Coordinator::get_instance().schedule_event(new
		Device_Eyemovement_Start_event(
			base_time, human_ptr->get_device_ptr(), obj_device_name, get_target_object_location())); */
	Coordinator::get_instance().schedule_event(new 
		Device_Eyemovement_Start_event(
			base_time, human_ptr->get_device_ptr(), obj_device_name, eye_destination));

/*	Coordinator::get_instance().schedule_event(new
		Device_Eyemovement_End_event(
			movement_completion_time, human_ptr->get_device_ptr(), obj_device_name, get_target_object_location()));*/
	// see above - DK 4/23/16
	Coordinator::get_instance().schedule_event(new 
		Device_Eyemovement_End_event(
			movement_completion_time + 1, human_ptr->get_device_ptr(), obj_device_name, eye_destination));
			
	return movement_completion_time;
}


// These constructors set up the object name, source, and location for a saccade
Ocular_Move_action::Ocular_Move_action(Human_processor * human_ptr, const Symbol_list_t arguments,
        bool execute_when_prepared) :
		Ocular_Saccade_action(human_ptr, execute_when_prepared)
{
    // action has only one argument, the object name
    if(arguments.size() != 1)
		throw Motor_action_exception(human_ptr, " Ocular_Move: ", arguments);
    set_obj_name(arguments.front());
    set_target_object_source_and_location();
}

Ocular_Random_Move_action::Ocular_Random_Move_action(Human_processor * human_ptr_, const Symbol_list_t& arguments, bool execute_when_prepared) :
		Ocular_Saccade_action(human_ptr_, execute_when_prepared)
{
    // action has only two arguments, the x_bounds and y_bounds around (0, 0)
    if(arguments.size() != 2)
		throw Motor_action_exception(human_ptr, " Ocular_Random_Move: ", arguments);
    double x_bounds = arguments.front().get_numeric_value();
    double y_bounds = arguments.back().get_numeric_value();
    
    set_obj_name(Random_location_c);
    // generate a random coordinate  in range -xbounds/2, +x_bounds/2
    double x = uniform_random_variable(0., x_bounds/2.);
    double y = uniform_random_variable(0., y_bounds/2.);
    set_target_object_source_and_location(GU::Point(x, y));
}



Ocular_Mode_action::Ocular_Mode_action(Human_processor * human_ptr_, 
	const Symbol_list_t& arguments, bool execute_when_prepared) :
		Ocular_action(human_ptr_, execute_when_prepared)
{
	// arguments in the form Enable/Disable Centering/Reflex
	command = arguments.front();
	system = arguments.back();	// assuming only two
	
	
	if(command != Enable_c && command != Disable_c)
		throw Motor_action_exception(human_ptr, "Invalid command in Set_mode action", command);
		
	if(system != Centering_c && system != Reflex_c)
		throw Motor_action_exception(human_ptr, "Invalid system in Set_mode action", system);
}

// a mode action has a constant preparation time based on 2 features
// purely speculative - but command is one feature, system is another
long Ocular_Mode_action::prepare(long base_time)
{
	long feature_preparation_time = long(get_processor_ptr()->get_feature_time() * 2);				

	return base_time + feature_preparation_time;
}

// return the time at which the action is complete
// assume - purely speculative, that the mode exection takes a burst time
long Ocular_Mode_action::execute(long base_time)
{
	long movement_completion_time = base_time + get_processor_ptr()->get_burst_time();
	Assert(command == Enable_c || command == Disable_c);
	Assert(system == Centering_c || system == Reflex_c);
	
	if(get_processor_ptr()->get_trace() && Trace_out)
		Trace_out << get_processor_ptr()->processor_info() << "Mode Command: " 
		<< command << ' ' << system << endl;

	bool state = (command == Enable_c) ? true : false;
	if(system == Centering_c) {
		get_eye_ptr()->set_centering_enabled(state);
		}
	else if(system == Reflex_c) {
		get_eye_ptr()->set_reflex_enabled(state);
		get_eye_ptr()->set_auditory_reflex_enabled(state);
		}
	
	return movement_completion_time;
}

/* GOMS Look_for operator implementation */
// Send_to_motor Ocular Look_for method_name tag_name p v p v ...
// results in eye on object and (Tag method_name object tag_name) in PPS database
Ocular_Look_for_action::Ocular_Look_for_action(Human_processor * human_ptr, const Symbol_list_t& arguments, bool execute_when_prepared) :
	Ocular_action(human_ptr, execute_when_prepared), pv_list(arguments)
{
	// check for correct length
	if(!((pv_list.size() >= 4) && ((pv_list.size() % 2) == 0)))
		throw Motor_action_exception(human_ptr, " Ocular_Look_for: ", pv_list);
		
	method_name = pv_list.front();
	pv_list.pop_front();
	tag_name = pv_list.front();
	pv_list.pop_front();
	// keep remainder as list of property name and value pairs
}
const long Look_for_preparation_time_c = 200;

long Ocular_Look_for_action::prepare(long base_time)
{
	// find and store the sought-for object's psychological name using the property list
	Visual_physical_store * visual_store_ptr = human_ptr->get_Visual_physical_store_ptr();
	Symbol_list_t objects = visual_store_ptr->find_all(pv_list);
	if(objects.empty()) {
		/* no object there to be found */
		object_name = Absent_c;
		return base_time + Look_for_preparation_time_c;
		}
	else if(objects.size() == 1) {
		object_name = objects.front();
		}
	else {
		// pick one at random
		object_name = get_nth_Symbol(objects, random_int(int(objects.size())));
		}
		
	// store the object location as known physically
	obj_location = visual_store_ptr->get_object_ptr(object_name)->get_location();
	
	// unconditionally, the feature preparation time is a constant
	
	return base_time + Look_for_preparation_time_c;
}

const long Look_for_execution_time_c = 1000;

// return the time at which the final movement is complete
// and send any additional messages in the meantime
long Ocular_Look_for_action::execute(long base_time)
{
	long movement_completion_time = base_time + Look_for_execution_time_c;
	// if no object there to be found, skip the actual eye movement, but take the full time
	if(object_name == Absent_c)
		return movement_completion_time;

	// send the eye movement start event at the current time to the eye
	Coordinator::get_instance().schedule_event(new 
		Eye_Voluntary_Saccade_Start_event(
			base_time, get_eye_ptr(), obj_location, movement_completion_time));

	// send the eye movement end event at the movement_completion_time time to the eye
	// set centering on, reflexes off
	Coordinator::get_instance().schedule_event(new 
		Eye_Voluntary_Saccade_End_event(
			movement_completion_time, get_eye_ptr(), true, false));
			
	
	return movement_completion_time;
}


// Inform cognitive processor of object identity using the supplied tag
void Ocular_Look_for_action::finish()
{
	// if the object name is not absent, get the psychological name for the object and store that
	if(object_name != Absent_c) {
		const Symbol& psych_name = get_eye_ptr()->get_name_map().get_psychological_name(object_name);
		human_ptr->get_Cognitive_processor_ptr()->add_clause(Clause (Tag_c, method_name, psych_name, tag_name));
		}
	else {
		human_ptr->get_Cognitive_processor_ptr()->add_clause(Clause (Tag_c, method_name, Absent_c, tag_name));
		}
}


