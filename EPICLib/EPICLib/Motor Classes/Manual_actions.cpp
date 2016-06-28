#include "Manual_actions.h"
#include "Manual_aimed_actions.h"
#include "Manual_processor.h"
#include "Coordinator.h"
#include "Cognitive_event_types.h"
#include "Device_event_types.h"
#include "Device_processor.h"
#include "Output_tee_globals.h"
#include "Epic_standard_symbols.h"

using std::shared_ptr;


// this "virtual constructor" method returns a pointer to the created object
// based on the contents of the list of motor command parameters
// or zero if no object was created because the parameters are invalid
shared_ptr<Motor_action> Manual_action::create(Human_processor * human_ptr,
	Symbol_list_t arguments, bool execute_when_prepared)
{	
	if(arguments.size() < 2)
		return 0;
	
	const Symbol style = arguments.front();
	if(style == Keystroke_c) {
		arguments.pop_front();
		return shared_ptr<Motor_action> (new Manual_Keystroke_action(human_ptr, arguments, execute_when_prepared));
		}
	if(style == Hold_c) {
		arguments.pop_front();
		return  shared_ptr<Motor_action> (new Manual_Hold_action(human_ptr, arguments, execute_when_prepared));
		}
	if(style == Release_c) {
		arguments.pop_front();
		return  shared_ptr<Motor_action> (new Manual_Release_action(human_ptr, arguments, execute_when_prepared));
		}
	else if(style == Punch_c) {
		arguments.pop_front();
		return  shared_ptr<Motor_action> (new Manual_Punch_action(human_ptr, arguments, execute_when_prepared));
		}
	else {
		return Manual_aimed_action::create(human_ptr, arguments, execute_when_prepared);
		}
}

Manual_action::Manual_action(Human_processor * human_ptr_, bool execute_when_prepared) :
		Motor_action(human_ptr_, execute_when_prepared)
{
	proc_ptr = human_ptr->get_Manual_processor_ptr();
	device_ptr = human_ptr->get_Device_processor_ptr();
}

// helper so that Manual_actions can access trace status of their motor processor
bool Manual_action::get_trace()
{
	return proc_ptr->get_trace();
}

// this object will send itself to the appropriate processor
void Manual_action::dispatch(long time)
{
	human_ptr->schedule_event(new Motor_Command_Action_event(
		time, 
		human_ptr->get_manual_ptr(),
		shared_from_this()
		));
}


void Manual_action::check_hand(const Symbol& hand)
{
	if(hand != Right_c && hand != Left_c)
		throw Motor_action_exception(human_ptr, "invalid hand", hand);
}

void Manual_action::check_finger(const Symbol& finger)
{
	if(finger != Thumb_c && finger != Index_c && finger != Middle_c && finger != Ring_c && finger != Little_c)
		throw Motor_action_exception(human_ptr, "invalid finger", finger);
}
		



// All randomized fluctuations are done in the Motor_processor parameters, which includes 
// a single fluctuation parameter. Individual movement execution characteristics 
// are fixed parameters defined here.

// local constants
//const long keystroke_execution_time_c = 230;
//const long key_closure_time_c = 25;

/*
Manual_action::Manual_action(Human_processor * hum_ptr, bool execute_when_prepared) :
		Motor_action(hum_ptr, execute_when_prepared)
{
	device_ptr = static_cast<Device_processor *>(proc_ptr->get_human_ptr()->get_device_ptr());
}
*/

Manual_Keystroke_action::Manual_Keystroke_action(Human_processor * hum_ptr, Symbol_list_t arguments, bool execute_when_prepared) :
	Manual_action(hum_ptr, execute_when_prepared)
{
	// argument list should contain only the target symbol
	if(arguments.size() != 1)
		throw Motor_action_exception(hum_ptr, "Target symbol", arguments);
	
	keyname = arguments.front();
}



// note that dynamic_cast of a pointer whose value is zero results in zero

// return the time required for preparation
// Keystroke is style>(direction = extent, average 1.5 features)
long Manual_Keystroke_action::prepare(long base_time)
{
	long feature_preparation_time;
	shared_ptr<Manual_Keystroke_action> ptr = std::dynamic_pointer_cast<Manual_Keystroke_action>(get_processor_ptr()->get_previous_ptr());
	long feature_time = get_processor_ptr()->get_feature_time();

	if (ptr) {								// previous is same type of action
		if (ptr->keyname == keyname)
			feature_preparation_time = 0;	// identical movement
		else
			feature_preparation_time = long(feature_time * 1.5); // 1.5 features
		}
	else 
			feature_preparation_time = long(feature_time  * 3);	// style change - three features
	
	
	
	return base_time + feature_preparation_time;
}

// return the time at which the final movement is complete
// and send any additional messages in the meantime
long Manual_Keystroke_action::execute(long base_time)
{
	double fluctuation = get_processor_ptr()->get_execution_fluctuation();
	long keystroke_execution_time = get_processor_ptr()->get_parameter_ptr("KLM_keystroke_execution_time")->get_long_value();
	keystroke_execution_time = keystroke_execution_time - get_processor_ptr()->get_initiation_time();
	long execution_time = long((keystroke_execution_time + get_processor_ptr()->auto_home_to_keyboard()) * fluctuation);
	long movement_completion_time = base_time + execution_time;

	// send the keystroke at the movement_completion_time time to the device
	Coordinator::get_instance().schedule_event(new 
		Device_Keystroke_event(movement_completion_time, get_device_ptr(), keyname));
	
	return movement_completion_time;
}


Manual_Hold_action::Manual_Hold_action(Human_processor * hum_ptr, Symbol_list_t arguments, bool execute_when_prepared) :
	Manual_action(hum_ptr, execute_when_prepared)
{
	// argument list should contain only the target symbol
	if(arguments.size() != 1)
		throw Motor_action_exception(hum_ptr, "Target symbol", arguments);
	
	keyname = arguments.front();
}


// return the time required for preparation
long Manual_Hold_action::prepare(long base_time)
{
	long feature_preparation_time;
	shared_ptr<Manual_Hold_action> ptr = std::dynamic_pointer_cast<Manual_Hold_action>(get_processor_ptr()->get_previous_ptr());
	long feature_time = get_processor_ptr()->get_feature_time();

	if (ptr) {								// previous is same type of action
		if (ptr->keyname == keyname)
			feature_preparation_time = 0;	// identical movement
		else
			feature_preparation_time = long(feature_time * 0.); // 0 features - two holds in a row?
		}
	else 
			feature_preparation_time = long(feature_time  * 3);	// style change - three features
	
	return base_time + feature_preparation_time;
}

// return the time at which the final movement is complete
// and send any additional messages in the meantime
long Manual_Hold_action::execute(long base_time)
{
	double fluctuation = get_processor_ptr()->get_execution_fluctuation();
	long hold_execution_time = get_processor_ptr()->get_parameter_ptr("KLM_hold_release_execution_time")->get_long_value();
	hold_execution_time = hold_execution_time - get_processor_ptr()->get_initiation_time();
	long execution_time = long((hold_execution_time + get_processor_ptr()->auto_home_to_mouse()) * fluctuation);
	long movement_completion_time = base_time + execution_time;

	// send the button name at the movement_completion_time time to the device
	Coordinator::get_instance().schedule_event(new 
		Device_Hold_event(movement_completion_time, get_device_ptr(), keyname));
	
	return movement_completion_time;
}

Manual_Release_action::Manual_Release_action(Human_processor * hum_ptr, Symbol_list_t arguments, bool execute_when_prepared) :
	Manual_action(hum_ptr, execute_when_prepared)
{
	// argument list should contain only the target symbol
	if(arguments.size() != 1)
		throw Motor_action_exception(hum_ptr, "Target symbol", arguments);
	
	keyname = arguments.front();
}



// return the time required for preparation
long Manual_Release_action::prepare(long base_time)
{
	long feature_preparation_time;
	shared_ptr<Manual_Release_action> ptr = std::dynamic_pointer_cast<Manual_Release_action>(get_processor_ptr()->get_previous_ptr());
	long feature_time = get_processor_ptr()->get_feature_time();

	if (ptr) {								// previous is same type of action
		if (ptr->keyname == keyname)
			feature_preparation_time = 0;	// identical movement
		else
			feature_preparation_time = long(feature_time * 0.); // 0 features - two holds in a row?
		}
	else 
			feature_preparation_time = long(feature_time  * 3);	// style change - three features
	
	return base_time + feature_preparation_time;
}

// return the time at which the final movement is complete
// and send any additional messages in the meantime
long Manual_Release_action::execute(long base_time)
{
	double fluctuation = get_processor_ptr()->get_execution_fluctuation();
	long release_execution_time = get_processor_ptr()->get_parameter_ptr("KLM_hold_release_execution_time")->get_long_value();
	release_execution_time = release_execution_time - get_processor_ptr()->get_initiation_time();
	long execution_time = long((release_execution_time + get_processor_ptr()->auto_home_to_mouse()) * fluctuation);
	long movement_completion_time = base_time + execution_time;

	// send the button name at the movement_completion_time time to the device
	Coordinator::get_instance().schedule_event(new 
		Device_Release_event(movement_completion_time, get_device_ptr(), keyname));
	
	return movement_completion_time;
}


Manual_Punch_action::Manual_Punch_action(Human_processor * hum_ptr, Symbol_list_t arguments, bool execute_when_prepared) :
		Manual_action(hum_ptr, execute_when_prepared)
{
	// argument list should contain only the target symbol + hand + finger
	if(arguments.size() != 3)
		throw Motor_action_exception(hum_ptr, "key name, hand, finger required", arguments);
	keyname = arguments.front();
	arguments.pop_front();
	hand = arguments.front();
	arguments.pop_front();
	check_hand(hand);
	finger = arguments.front();
	check_finger(finger);
}


// return the time required for preparation
// Punch is  style>(hand > finger)
long Manual_Punch_action::prepare(long base_time)
{
	shared_ptr<Manual_Punch_action> ptr = std::dynamic_pointer_cast<Manual_Punch_action>(get_processor_ptr()->get_previous_ptr());
	int features;

	if (ptr) {	// previous has same style
		if (ptr->hand != hand) // previous is different hand
			features = 2;
		else if (ptr->finger != finger)	// previous is different finger
			features = 1;
		else
			features = 0;
		}
	else 
		features = 3;

	long feature_time = get_processor_ptr()->get_feature_time();
	long feature_preparation_time = long(feature_time  * features);	// style change - three features
		
	return base_time + feature_preparation_time;
}

// return the time at which the final movement is complete
// and send any additional messages in the meantime
// base time is the beginning of actual physical movement; initiation has already happened.
long Manual_Punch_action::execute(long base_time)
{
	Human_processor * human_ptr = get_processor_ptr()->get_human_ptr();
	Processor * cognitive_ptr = human_ptr->get_cognitive_ptr();
	double fluctuation = get_processor_ptr()->get_execution_fluctuation();

	// movement description form: (Motor Manual Punch <hand> <finger> Started|Finished)	
	Symbol_list_t movement_description;
	movement_description.push_back(Motor_c);
	movement_description.push_back(Manual_c);
	movement_description.push_back(Punch_c);
	movement_description.push_back(hand);
	movement_description.push_back(finger);
	movement_description.push_back(Started_c);

	//	tell cog that movement has started at base_time, signal to be deleted after a delay
 	Coordinator::get_instance().schedule_event(new 
		Cognitive_Add_Clause_event(base_time, cognitive_ptr, movement_description));
 	Coordinator::get_instance().schedule_event(new 
		Cognitive_Delete_Clause_event(base_time + get_processor_ptr()->get_efferent_deletion_delay(),
		cognitive_ptr, movement_description));
 
	//	tell tactile that movement has started at base_time
	/* not implemented yet */
	//	tell device that key has closed at base_time + key closure time
	long key_closure_time = get_processor_ptr()->get_parameter_ptr("Key_closure_time")->get_long_value();
	long device_input_time = base_time + long(key_closure_time * fluctuation);
	Coordinator::get_instance().schedule_event(new 
		Device_Keystroke_event(device_input_time, get_device_ptr(), keyname));
	//	the downstroke is done at start time + burst time
	long downstroke_done_time = base_time + get_processor_ptr()->get_burst_time();
	//	tell tactile that the downstroke is done
	/* not implemented yet */
	// 	tell cog that movement is finished when the downstroke is done
	movement_description.pop_back();	// discard "Started" term
	movement_description.push_back(Finished_c);
	Coordinator::get_instance().schedule_event(new 
		Cognitive_Add_Clause_event(downstroke_done_time, cognitive_ptr, movement_description));
 	Coordinator::get_instance().schedule_event(new 
		Cognitive_Delete_Clause_event(downstroke_done_time + get_processor_ptr()->get_efferent_deletion_delay(),
		cognitive_ptr, movement_description));

	//	to complete the movement, the finger must be upstroked, which takes an additional burst time
	long upstroke_done_time = downstroke_done_time + get_processor_ptr()->get_burst_time();

	//	tell tactile the movement is finished
	/* not implemented yet */
	//	the movement is complete at this final time	

	return upstroke_done_time;
}


/*	
// return the time required for preparation
// Type_in is style>(direction = extent, average 1.5 features)
long Manual_Type_in_action::prepare(long base_time, const Manual_action * previous_ptr)
{
	int feature_preparation_time;
	const Manual_Type_in_action * ptr = dynamic_cast<const Manual_Type_in_action *>(previous_ptr);
	
	if (ptr) {								// previous is same type of action
			feature_preparation_time = Manual_processor::feature_time * 1.5; // 1.5 features
		}
	else 
			feature_preparation_time = Manual_processor::feature_time * 3;	// style change - three features
	
	return base_time + feature_preparation_time;
}

// return the time at which the final movement is complete
// and send any additional messages in the meantime
long Manual_Type_in_action::execute(long base_time)
{
	int execution_time = 280;
	// specification is a string of characters to be typed
	long movement_completion_time = base_time + proc_ptr->auto_home_to_keyboard() + 
		execution_time * strlen(type_in_string.get_cstr());
// time less one to align with next cognitive cycle.
	if ((movement_completion_time % 50) == 0)
		movement_completion_time--;

	// send the keystroke at the movement_completion_time time to the device
	proc_ptr->send_to_device(new Device_Type_In_Message(movement_completion_time, 0, 0, type_in_string));
	
	return movement_completion_time;
}

// Home is style>direction>extent (hand is always right, 100 ms preparation)
// no check for whether hand is already at the destination
long Manual_Home_action::prepare(long base_time, const Manual_action * previous_ptr)
{
	int feature_preparation_time;
	const Manual_Home_action * ptr = dynamic_cast<const Manual_Home_action *>(previous_ptr);
	
	if (ptr) {								// previous is same type of action
		if (ptr->destination == destination)
			feature_preparation_time = 0;	// identical movement
		else
			feature_preparation_time = Manual_processor::feature_time * 1.5; // 1.5 features
		}
	else 
			feature_preparation_time = Manual_processor::feature_time * 2;	// style change - two features

	return base_time + feature_preparation_time;
}

long Manual_Home_action::execute(long base_time)
{
	int execution_time = 350;
	long movement_completion_time = base_time + execution_time;
	// nothing is sent to the device
		
	return movement_completion_time;
}

// Hand is now at the destination
void Manual_Home_action::finish()
{
	proc_ptr->set_right_hand_position(destination);
}

// Hold is style>hand>finger
long Manual_Hold_action::prepare(long base_time, const Manual_action * previous_ptr)
{
	int feature_preparation_time;
	const Manual_Hold_action * ptr = dynamic_cast<const Manual_Hold_action *>(previous_ptr);
	
	if (ptr) {								// previous is same type of action
		if (ptr->button_name == button_name)
			feature_preparation_time = 0;	// identical movement
		else
			feature_preparation_time = Manual_processor::feature_time; // 1 feature
		}
	else 
			feature_preparation_time = Manual_processor::feature_time * 2;	// style change - two features

	return base_time + feature_preparation_time;
}

long Manual_Hold_action::execute(long base_time)
{
	// the button is not really held yet, but since it can affect overlapping pointing, set now
	int execution_time = 50;
	long movement_completion_time = base_time + proc_ptr->auto_home_to_mouse() + execution_time;
	proc_ptr->send_to_device(new Device_Hold_Message(movement_completion_time, 0, 0, button_name));
	
	return movement_completion_time;
}

// we are holding a finger down now
void Manual_Hold_action::finish()
{
	proc_ptr->set_button_held(true);
}

// Release is style>hand>finger
long Manual_Release_action::prepare(long base_time, const Manual_action * previous_ptr)
{
	int feature_preparation_time;
	const Manual_Release_action * ptr = dynamic_cast<const Manual_Release_action *>(previous_ptr);
	
	if (ptr) {								// previous is same type of action
		if (ptr->button_name == button_name)
			feature_preparation_time = 0;	// identical movement
		else
			feature_preparation_time = Manual_processor::feature_time; // 1 feature
		}
	else 
			feature_preparation_time = Manual_processor::feature_time * 2;	// style change - two features

	return base_time + feature_preparation_time;
}

long Manual_Release_action::execute(long base_time)
{
	int execution_time = 50;
	long movement_completion_time = base_time + proc_ptr->auto_home_to_mouse() + execution_time;
	proc_ptr->send_to_device(new Device_Hold_Message(movement_completion_time, 0, 0, button_name));
	
	return movement_completion_time;
}

// we are now no longer holding a finger down
void Manual_Release_action::finish()
{
	proc_ptr->set_button_held(false);
}

// click is style>hand>finger
long Manual_Click_action::prepare(long base_time, const Manual_action * previous_ptr)
{
	int feature_preparation_time;
	const Manual_Click_action * ptr = dynamic_cast<const Manual_Click_action *>(previous_ptr);
	
	if (ptr) {								// previous is same type of action
		if (ptr->button_name == button_name)
			feature_preparation_time = 0;	// identical movement
		else
			feature_preparation_time = Manual_processor::feature_time; // 1 feature
		}
	else 
			feature_preparation_time = Manual_processor::feature_time * 2;	// style change - two features
	
	return base_time + feature_preparation_time;
}

long Manual_Click_action::execute(long base_time)
{
	int execution_time = 150;
	long movement_completion_time = base_time + proc_ptr->auto_home_to_mouse() + execution_time;
	proc_ptr->send_to_device(new Device_Click_Message(movement_completion_time, 0, 0, button_name));
	
	return movement_completion_time;
}

// double click is style>hand>finger
long Manual_Double_Click_action::prepare(long base_time, const Manual_action * previous_ptr)
{
	int feature_preparation_time;
	const Manual_Double_Click_action * ptr = dynamic_cast<const Manual_Double_Click_action *>(previous_ptr);
	
	if (ptr) {								// previous is same type of action
		if (ptr->button_name == button_name)
			feature_preparation_time = 0;	// identical movement
		else
			feature_preparation_time = Manual_processor::feature_time; // 1 feature
		}
	else 
			feature_preparation_time = Manual_processor::feature_time * 2;	// style change - two features
	
	return base_time + feature_preparation_time;
}

long Manual_Double_Click_action::execute(long base_time)
{
	int execution_time = 350;
	long movement_completion_time = base_time + proc_ptr->auto_home_to_mouse() + execution_time;
	proc_ptr->send_to_device(new Device_Double_Click_Message(movement_completion_time, 0, 0, button_name));
	
	return movement_completion_time;
}

*/
