#ifndef VOCAL_ACTIONS_H
#define VOCAL_ACTIONS_H

/*
The Vocal_action classes encapsulate how a particular type of Vocal movement is 
prepared and executed. Each subclass defines what the preparation time is in relation
to the previously prepared movement, and the execution time for the movement.
The execute function is responsible for sending messages to the device and
any other special messages.

The controlling Vocal motor processor is responsible for sequencing the movements
through waiting, preparation, ready, and executing stages. For each commanded movement,
it will create a new Vocal_action object to store the movement information, and then
delete it when it is no longer needed.
*/

#include "Motor_action.h"
#include "Symbol.h"
#include <memory>

// forward declarations
class Vocal_processor;
class Device_processor;

class Vocal_action : public Motor_action {
public:
	// "virtual constructor" static method to construct appropriate motor action
	// given list of action arguments; throws exception if arguments invalid
	static std::shared_ptr<Motor_action> create(Human_processor * human_ptr,
		Symbol_list_t arguments, bool execute_when_prepared);

	// regular constructor initializes pointer to containing human processor and execution flag
	Vocal_action(Human_processor * human_ptr_, bool execute_when_prepared);

	// this object will send itself to the appropriate processor
	virtual void dispatch(long time);

	Vocal_processor * get_processor_ptr() const
		{return proc_ptr;}
	Device_processor * get_device_ptr() const
		{return device_ptr;}
protected:
private:
	Vocal_processor * proc_ptr;	// pointer to the controlling motor processor
	Device_processor * device_ptr;
};


class Vocal_Speak_action : public Vocal_action {
public:
	Vocal_Speak_action(Human_processor * human_ptr_, Symbol_list_t arguments, bool overt_, bool execute_when_prepared);
	virtual long prepare(long base_time);
	virtual long execute(long base_time);

private:
	Symbol utterance;	// the string of words to be said
	bool overt;			// true if utterrance is overt
	long articulation_time(); 	// return the number of ms to articulate the utterance
	static int utterance_counter;
};



#endif
