#include "Manual_processor.h"
#include "Output_tee_globals.h"

#include <string>
#include <iostream>

using std::string;
using std::endl;

void Manual_processor::initialize()
{
	Motor_processor::initialize();
	right_hand_position = Keyboard_name_c;
	button_held = false;
//	set_randomize_when_used(true);
//	set_randomize(true);
//	randomize();
//	describe_parameters(Normal_out);
}


/* Aimed Actions
Aimed actions all imply that some visually-locable object (the mouse cursor, the finger) is going to be moved to the position
of some other located object. 
*/
/*
void Manual_processor::handle_event(const Manual_Point_Mouse_event * event_ptr)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "Point_Mouse" << event_ptr->target << endl;
	start_command(new Manual_Point_Mouse_action(this, event_ptr->cursor, event_ptr->target, event_ptr->execute_when_prepared));	
}
*/


/*
void Manual_processor::handle_event(Manual_Type_in_event const * event_ptr)
{
	Trace_out << facade_ptr->processor_info() << " Type_in " << event_ptr->specification << endl;
	start_command(event_ptr, new Manual_Type_in_Action(this, event_ptr->specification));	
}

void Manual_processor::handle_event(Manual_Home_event const * event_ptr)
{	
	Trace_out << facade_ptr->processor_info() <<  " Home_to " << event_ptr->specification << endl;
	// specification is a destination for the movement
	start_command(event_ptr, new Manual_Home_Action(this, event_ptr->specification));		
}

void Manual_processor::handle_event(Manual_Hold_event const * event_ptr)
{
	Trace_out << facade_ptr->processor_info() << " Hold "	<< event_ptr->specification << endl;
	// specification is which button
	start_command(event_ptr, new Manual_Hold_Action(this, event_ptr->specification));		
}

void Manual_processor::handle_event(Manual_Release_event const * event_ptr)
{
	Trace_out << facade_ptr->processor_info() << " Release " << event_ptr->specification << endl;
 	// specification is which button
	start_command(event_ptr, new Manual_Release_Action(this, event_ptr->specification));	
	
}

void Manual_processor::handle_event(Manual_Click_event const * event_ptr)
{
	Trace_out << facade_ptr->processor_info() << " Click "	<< event_ptr->specification << endl;
	// specification is which button
	start_command(event_ptr, new Manual_Click_Action(this, event_ptr->specification));	
}

void Manual_processor::handle_event(Manual_Double_Click_event const * event_ptr)
{
	Trace_out << facade_ptr->processor_info() << " Double Click "	<< event_ptr->specification << endl;
	// specification is which button
	start_command(event_ptr, new Manual_Double_Click_Action(this, event_ptr->specification));	
	
}


void Manual_processor::send_operation_complete(long signal_time)
{	
// if time is a multiple of 50 ms, subtract one to align with next cognitive cycle.
	if ((signal_time % cognitive_cycle_time) == 0)
		signal_time--;
	
	Message * event_ptr = new Cognitive_Manual_Operation_Complete_Message (signal_time, cognitive_ptr, facade_ptr->get_broadcaster());
		
	facade_ptr->schedule_message(event_ptr); 
	
}

// plub in the destination and broadcaster fields and schedule the message
void Manual_processor::send_to_device(Message * event_ptr)
{
	// motor commands have had one ms subtracted; add one ms to round to natural time units
	event_ptr->time++;
	// fill in other fields
	event_ptr->recepient = device_ptr;
	event_ptr->broadcaster = facade_ptr->get_broadcaster();
	facade_ptr->schedule_message(event_ptr);
}


*/

long Manual_processor::auto_home_to_keyboard()
{
	if (right_hand_position != Keyboard_name_c) {
		right_hand_position = Keyboard_name_c;
	if(get_trace() && Trace_out)
			Trace_out << processor_info() << " auto-homing right hand to keyboard" << endl;
		return home_time.get_long_value();
		}
	else return 0;
}

long Manual_processor::auto_home_to_mouse()
{
	if (right_hand_position != Mouse_name_c) {
		right_hand_position = Mouse_name_c;
	if(get_trace() && Trace_out)
			Trace_out << processor_info() << " auto-homing right hand to mouse" << endl;
		return home_time.get_long_value();
		}
	else return 0;
}

