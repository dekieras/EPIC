#ifndef MANUAL_ACTIONS_H
#define MANUAL_ACTIONS_H

/*
The Manual_action classes encapsulate how a particular type of manual movement is 
prepared and executed. Each subclass defines what the preparation time is in relation
to the previously prepared movement, and the execution time for the movement.
The execute function is responsible for sending messages to the device and
any other special messages.

The controlling manual motor processor is responsible for sequencing the movements
through waiting, preparation, ready, and executing stages. For each commanded movement,
it will create a new Manual_action object to store the movement information, and then
delete it when it is no longer needed.
*/

#include "Motor_action.h"
#include "Symbol.h"
#include <memory>

// forward declaration
class Manual_processor;
class Device_processor;
//class Perceptual_space;

class Manual_action : public Motor_action {
public:
	// "virtual constructor" static method to construct appropriate motor action
	// given list of action arguments; throws exception if arguments invalid
	static std::shared_ptr<Motor_action> create(Human_processor * human_ptr,
		Symbol_list_t arguments, bool execute_when_prepared);

	// regular constructor initializes pointer to containing human processor and execution flag
	Manual_action(Human_processor * human_ptr_, bool execute_when_prepared);

	// this object will send itself to the appropriate processor
	virtual void dispatch(long time);

	Manual_processor * get_processor_ptr() const
		{return proc_ptr;}
	Device_processor * get_device_ptr() const
		{return device_ptr;}
		
	// helper so that Manual_actions can access trace status of their motor processor
	bool get_trace();
	

protected:
	void check_hand(const Symbol&);
	void check_finger(const Symbol&);

private:
	Manual_processor * proc_ptr;	// pointer to the controlling motor processor
	Device_processor * device_ptr;
};


class Manual_Keystroke_action : public Manual_action {
public:
	Manual_Keystroke_action(Human_processor * human_ptr_, Symbol_list_t arguments, bool execute_when_prepared);
	virtual long prepare(long base_time);
	virtual long execute(long base_time);

private:
	Symbol keyname;
};

class Manual_Hold_action : public Manual_action {
public:
	Manual_Hold_action(Human_processor * human_ptr_, Symbol_list_t arguments, bool execute_when_prepared);
	virtual long prepare(long base_time);
	virtual long execute(long base_time);

private:
	Symbol keyname;
};

class Manual_Release_action : public Manual_action {
public:
	Manual_Release_action(Human_processor * human_ptr_, Symbol_list_t arguments, bool execute_when_prepared);
	virtual long prepare(long base_time);
	virtual long execute(long base_time);

private:
	Symbol keyname;
};

// spec is of the form hand (Right, Left), finger (Thumb, Index, Middle, Ring, Little), keyname for device
class Manual_Punch_action : public Manual_action {
public:
	Manual_Punch_action(Human_processor * human_ptr_, Symbol_list_t arguments, bool execute_when_prepared);
	virtual long prepare(long base_time);
	virtual long execute(long base_time);
	
private:
	Symbol hand;
	Symbol finger;
	Symbol keyname;
};

/*
class Manual_Type_in_action : public Manual_action {
public:
	Manual_Type_in_action(Manual_processor * in_man_ptr, const Symbol& in_spec) :
		Manual_action(in_man_ptr, false), type_in_string(in_spec)
		{}
	virtual long prepare(long base_time, const Manual_action * previous_ptr);
	virtual long execute(long base_time);

private:
	Symbol type_in_string;
};


class Manual_Home_action : public Manual_action {
public:
	Manual_Home_action(Manual_processor * in_man_ptr, const Symbol& in_spec) :
		Manual_action(in_man_ptr, true), destination(in_spec)
		{}
	virtual long prepare(long base_time, const Manual_action * previous_ptr);
	virtual long execute(long base_time);
	virtual void finish();

private:
	Symbol destination;
};

class Manual_Hold_action : public Manual_action {
public:
	Manual_Hold_action(Manual_processor * in_man_ptr, const Symbol& in_spec) :
		Manual_action(in_man_ptr, true), button_name(in_spec)
		{}
	virtual long prepare(long base_time, const Manual_action * previous_ptr);
	virtual long execute(long base_time);
	virtual void finish();

private:
	Symbol button_name;
};

class Manual_Release_action : public Manual_action {
public:
	Manual_Release_action(Manual_processor * in_man_ptr, const Symbol& in_spec) :
		Manual_action(in_man_ptr, true), button_name(in_spec)
		{}
	virtual long prepare(long base_time, const Manual_action * previous_ptr);
	virtual long execute(long base_time);
	virtual void finish();

private:
	Symbol button_name;
};

class Manual_Click_action : public Manual_action {
public:
	Manual_Click_action(Manual_processor * in_man_ptr, const Symbol& in_spec) :
		Manual_action(in_man_ptr, true), button_name(in_spec)
		{}
	virtual long prepare(long base_time, const Manual_action * previous_ptr);
	virtual long execute(long base_time);

private:
	Symbol button_name;
};

class Manual_Double_Click_action : public Manual_action {
public:
	Manual_Double_Click_action(Manual_processor * in_man_ptr, const Symbol& in_spec) :
		Manual_action(in_man_ptr, true), button_name(in_spec)
		{}
	virtual long prepare(long base_time, const Manual_action * previous_ptr);
	virtual long execute(long base_time);

private:
	Symbol button_name;
};

*/

#endif
