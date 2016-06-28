#ifndef MOTOR_EVENT_TYPES_H
#define MOTOR_EVENT_TYPES_H

#include "Event_types.h"
#include "Motor_action.h"
#include <memory>

// forward declarations
class Motor_processor;


// a command event to start executing the previously prepared movement
class Motor_Command_Action_event : public Motor_event, public std::enable_shared_from_this<Motor_Command_Action_event>{
public:
	Motor_Command_Action_event(long t, Processor * d, std::shared_ptr<Motor_action> a) :
		Motor_event(t, d) , action(a) {}
	virtual void handle_self(Motor_processor *) const;
	
	std::shared_ptr<Motor_action> action;
};

/*
// a command event to start executing the previously prepared movement
class Motor_Perform_Prepared_event : public Motor_event {
public:
	static const Event * create(long t, Processor * r)
		{return new Motor_Perform_Prepared_event(t, r);}
	virtual const Event * clone() const
		
		{return new Motor_Perform_Prepared_event(*this);}
	virtual void handle_self(Motor_processor *) const;
private:
	Motor_Perform_Prepared_event(long t, Processor * d) :
		Motor_event(t, d) {}
};

// a command event to start executing the previous movement (repeat it)
class Motor_Perform_Previous_event : public Motor_event {
public:
	static const Event * create(long t, Processor * r)
		{return new Motor_Perform_Previous_event(t, r);}
	virtual const Event * clone() const
		
		{return new Motor_Perform_Previous_event(*this);}
	virtual void handle_self(Motor_processor *) const;
private:
	Motor_Perform_Previous_event(long t, Processor * d) :
		Motor_event(t, d) {}
};
*/

class Motor_Preparation_Complete_event : public Motor_event {
public:
	Motor_Preparation_Complete_event(long t, Processor * d) :
		Motor_event(t, d) {}
	virtual void handle_self(Motor_processor *) const;
};

class Motor_Movement_Started_event : public Motor_event {
public:
	Motor_Movement_Started_event(long t, Processor * d) :
		Motor_event(t, d) {}
	virtual void handle_self(Motor_processor *) const;
};

class Motor_Movement_Complete_event : public Motor_event {
public:
	Motor_Movement_Complete_event(long t, Processor * d) :
		Motor_event(t, d) {}
	virtual void handle_self(Motor_processor *) const;
private:
};


#endif
