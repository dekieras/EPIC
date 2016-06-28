#ifndef MOTOR_ACTION_H
#define MOTOR_ACTION_H

/*
The Motor_action classes encapsulate how a particular type of movement is 
prepared and executed. Each subclass defines what the preparation time is in relation
to the previously prepared movement, and the execution time for the movement.
The execute function is responsible for sending messages to the device and
any other special messages.

The controlling motor processor is responsible for sequencing the movements
through waiting, preparation, ready, and executing stages. For each commanded movement,
it will create a new Motor_action object to store the movement information, and then
delete it when it is no longer needed.
*/

#include <list>
#include <memory>
#include "Symbol.h"
//#include "Smart_Pointer.h"
#include "Processor.h"
#include "Epic_exception.h"
 
// forward declaration
//class Motor_processor;
class Human_processor;

// Abstract base class
// refer to with Smart_Pointers only
class Motor_action : public std::enable_shared_from_this<Motor_action>{
public:
	// initialize with whether overlappable
	Motor_action(Human_processor * human_ptr_, bool execute_when_prepared_) :
		human_ptr(human_ptr_), execute_when_prepared(execute_when_prepared_)
		{}
	virtual ~Motor_action() {};
	
	/* Methods for creation and dispatch of Motor_action events */
	// "virtual constructor" static method to construct appropriate motor action
	// given list of action parameters - returns 0 if invalid parameters
	static std::shared_ptr<Motor_action> create(Human_processor *, Symbol_list_t);

	// this object will send itself to the appropriate processor at the specified time
	virtual void dispatch(long time) = 0;
	
	/* Movement preparation and Execution methods */
	// return the time at feature preparation complete, comparing to previous action
	// base time is the current time, feature time is current time per feature
	virtual long prepare(long base_time) = 0;
	// return the time at execution complete, sending appropriate messages
	// base time is the current time + initiation time, fluctuation is a random multiplier
	// for the execution mechanical time
	virtual long execute(long base_time) = 0;
	// send any final messages needed
	virtual void finish()
		{}
	bool get_execute_when_prepared() const
		{return execute_when_prepared;}
	void set_execute_when_prepared(bool execute_when_prepared_)
		{execute_when_prepared = execute_when_prepared_;}
		
/*
	Symbol_list_t get_arguments() const
		{return arguments;}
		
	Symbol_list_t set_arguments(const Symbol_list_t& arguments_)
		{arguments = arguments_;}
*/
	
protected:
	Human_processor * const human_ptr;	// const so subclasses can't modify it
	
/* useless at this level */
	// helper so that Motor_actions can access trace status of their motor processor
//	bool get_trace();
	
	// helper for scanning parameter list
	const Symbol& get_next_parameter(const Symbol_list_t& parameters, Symbol_list_t::const_iterator& it);
		
private:

	// true if this action should be executed as soon as possible after preparation,
	// false if it must wait for a perform-previous command
	bool execute_when_prepared;

	Motor_action(const Motor_action&); 	// no copy or assignment
	Motor_action& operator= (const Motor_action&);
	

};


class Motor_action_exception : public Epic_exception {
public:
	Motor_action_exception(Human_processor *, std::string, const Symbol&);
	Motor_action_exception(Human_processor *, std::string, const Symbol_list_t&);
};


#endif

