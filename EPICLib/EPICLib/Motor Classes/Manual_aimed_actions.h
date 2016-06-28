#ifndef MANUAL_AIMED_ACTIONS_H
#define MANUAL_AIMED_ACTIONS_H


#include "Manual_actions.h"
#include "Geometry.h"
#include "Symbol.h"
#include <memory>
namespace GU = Geometry_Utilities;

// forward declarations
class Eye_processor;
class Visual_perceptual_store;
class Auditory_perceptual_store;
class Manual_processor;

/*
Aimed actions all involve some kind of cursor being moved to a target object
using a hand and possibly a finger. A different equation might govern each type
of movement, by the extent/direction feature set and its rules are basically the same.
The same event, Device_ply_event, is sent to the device by all Manual_aimed_actions

Motor Manual Perform Ply <cursor> <target> <hand>
(does a Fitts movement - specify order regime later)

Motor Manual Perform Point <target> - assumes right hand, and the default cursor name
	if target size is unspecified [0, 0] uses KLM time estimates
*/

// This is a base class for aimed actions
class Manual_aimed_action : public Manual_action {
public:
	Manual_aimed_action(Human_processor * human_ptr_, Symbol_list_t arguments, bool execute_when_prepared, 
		const char * movement_description_cstr);
	Manual_aimed_action(Human_processor * human_ptr_, const Symbol&  cursor_, const Symbol& target_,
		const Symbol&  hand_, bool execute_when_prepared, const char * movement_description_cstr);

	// "virtual constructor" static method to construct appropriate motor action
	// given list of action arguments; throws exception if arguments invalid
	static std::shared_ptr<Motor_action> create(Human_processor * human_ptr,
		Symbol_list_t arguments, bool execute_when_prepared);

	virtual long prepare(long base_time);
	virtual long execute(long base_time);
	
protected:
	Eye_processor * eye_ptr;	// pointer to the connected eye
	Visual_perceptual_store * visual_perceptual_store_ptr;	// pointer to the perceptual store
	Auditory_perceptual_store * auditory_perceptual_store_ptr;	// pointer to the OTHER perceptual store
	Symbol hand;
	Symbol cursor;	// name of cursor object
	GU::Point cursor_location;
	Symbol cursor_device_name;	// device name for the cursor object
	Symbol target;	// name of target object
	GU::Point target_location;
	GU::Size target_size;
	Symbol target_device_name;	// device name for the target object
	GU::Polar_vector movement_vector;
	long movement_time;
	Symbol_list_t movement_description;
	
	// services for aimed movements
	virtual double determine_movement_time() const = 0; // override to choose specific values for movement_time
	virtual long generate_movement_events(long base_time);	// override to modify the sequence of events generated
	double pointing_time(long coefficient, long minimum_time) const;
	bool get_movement_d_and_s(double& d, double& s) const;
	double modified_Welfords_Fitts_law(double d, double s, double minimum_time, double coefficient) const;
};

/*
Motor Manual Perform Ply <cursor> <target> <hand>
(does a Fitts movement - specify order regime later)
*/

class Manual_Ply_action : public Manual_aimed_action {
public:
	Manual_Ply_action(Human_processor * human_ptr_, Symbol_list_t arguments, bool execute_when_prepared);

	
protected:
	virtual double determine_movement_time() const;

};

/* GOMS Aimed movement: Motor Manual Perform Point <target>
Cursor_name_c and right hand are assumed; an action is generated that defaults to 1.1 if target size or distance is unknown or zero.
*/

class Manual_Point_action : public Manual_aimed_action {
public:
	Manual_Point_action(Human_processor * human_ptr_, Symbol_list_t arguments, bool execute_when_prepared);
	Manual_Point_action(Human_processor * human_ptr_, Symbol_list_t arguments, bool execute_when_prepared, const char * movement_description_cstr);
	virtual long prepare(long base_time);
	
protected:
	virtual double determine_movement_time() const;

};

/* Compound Aimed movement: Motor Manual Perform Click_on <target>
Cursor_name_c and right hand index, Mouse_Left_Button_c are assumed; 
a movement is generated that defaults to 1.1 for pointing time if target size or distance is unknown or zero,
followed by a punch-like keystroke of Mouse_Left_Button
*/

class Manual_Click_on_action : public Manual_Point_action {
public:
	Manual_Click_on_action(Human_processor * human_ptr_, Symbol_list_t arguments, bool execute_when_prepared);

	virtual long prepare(long base_time);
	virtual long generate_movement_events(long base_time);
	
protected:
	Symbol keyname;
	
};


#endif
