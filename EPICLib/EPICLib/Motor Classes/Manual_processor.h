#ifndef MANUAL_PROCESSOR_H
#define MANUAL_PROCESSOR_H

#include "Motor_processor.h"
#include "Parameter.h"
#include "Epic_standard_symbols.h"

class Manual_action;

class Manual_processor : public Motor_processor {
public:
	Manual_processor(Human_processor * human_ptr) :
		Motor_processor("Manual", human_ptr),
		right_hand_position(Keyboard_name_c), button_held(false),
		// manual action parameters
		home_time("Homing_time", 400),
		keystroke_execution_time("KLM_keystroke_execution_time", 280),
		hold_release_execution_time("KLM_hold_release_execution_time", 100),
		key_closure_time("Key_closure_time", 25),
		// manual aimed action parameters
		point_coefficient("Point_coefficient", 100),
		point_coefficient_button_held("Point_coefficient_button_held", 135),
		point_minimum_time("Point_minimum_time", 100),
		KLM_point_time("KLM_point_time", 1100),
		ply_coefficient("Ply_coefficient", 75),
		ply_minimum_time("Ply_minimum_time", 100),
		subevent_time("Subevent_time", 25)	// corresponds to display update time in ms; 15 would be a good working value
		{
			add_parameter(home_time);
			add_parameter(keystroke_execution_time);
			add_parameter(hold_release_execution_time);
			add_parameter(key_closure_time);
			add_parameter(point_coefficient);
			add_parameter(point_coefficient_button_held);
			add_parameter(point_minimum_time);
			add_parameter(KLM_point_time);
			add_parameter(ply_coefficient);
			add_parameter(ply_minimum_time);
			add_parameter(subevent_time);
			
			initialize();
		}
	
	virtual void initialize();

	// bring the base class names into the same scope for overload resolution
	using Motor_processor::accept_event;
	using Motor_processor::handle_event;
	
	// inputs
//	virtual void accept_event(const Start_event *); // base class does setup
//	virtual void accept_event(const Stop_event *);

//	services for Manual_actions
	friend class Manual_action;
	bool get_button_held() const
		{return button_held;}
	void set_button_held(bool button_held_)
		{button_held = button_held_;}
	long auto_home_to_keyboard();
	long auto_home_to_mouse();

private:

	// state
	Symbol right_hand_position;
	bool button_held;

	// parameters (accessed via Human_subprocessor::get_parameter_ptr)
	Parameter home_time;
	Parameter keystroke_execution_time;
	Parameter hold_release_execution_time;
	Parameter key_closure_time;
	Parameter point_coefficient;
	Parameter point_coefficient_button_held;
	Parameter point_minimum_time;
	Parameter KLM_point_time;
	Parameter ply_coefficient;
	Parameter ply_minimum_time;
	Parameter subevent_time;
	
	// helper functions
	
	// no default copy, assignment
	Manual_processor(const Manual_processor&);
	Manual_processor& operator= (const Manual_processor&);
};



#endif
