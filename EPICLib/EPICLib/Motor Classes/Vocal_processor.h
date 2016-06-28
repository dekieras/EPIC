#ifndef VOCAL_PROCESSOR_H
#define VOCAL_PROCESSOR_H

#include "Motor_processor.h"

class Vocal_action;

class Vocal_processor : public Motor_processor {
public:
	Vocal_processor(Human_processor * human_ptr) :
		Motor_processor("Vocal", human_ptr),
		syllable_time("Syllable_time", 150)
		{
			add_parameter(syllable_time);
			initialize();
		}
	
	virtual void initialize();

	// bring the base class names into the same scope for overload resolution
	using Motor_processor::accept_event;
	using Motor_processor::handle_event;
	
	// inputs
//	virtual void accept_event(const Start_event *); // base class does setup
//	virtual void accept_event(const Stop_event *);

//	services for Vocal_actions
	friend class Vocal_action;

private:

	// state

	// parameters
	Parameter syllable_time;
	
	// helper functions
	
	// no default copy, assignment
	Vocal_processor(const Vocal_processor&);
	Vocal_processor& operator= (const Vocal_processor&);
};



#endif
