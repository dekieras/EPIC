/*
The Processor class handles events and has a common interface of a set of virtual functions 
for the generic event types for all the subclasses of processors. Since these might need to be copied,
copy and assignment of the base class of Processor is allowed.
*/

#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>

// incomplete declarations for event classes
class Event;
class Start_event;
class Stop_event;
class Report_event;
class Device_event;
class Visual_event;
class Eye_event;
class Auditory_event;
class Cognitive_event;
class Motor_event;
class Manual_event;
class Vocal_event;
class Temporal_event;
class WM_event;
class LTM_event;
class Task_event;
class High_Level_event;

class Processor {
public:
	// the add_processor parameter allows creation of "passive" processors 
	// that simply observe events explicitly handed to them - like Recorders
	Processor (const std::string& processor_name_, bool add_processor = true);
	virtual ~Processor();
	
	virtual void initialize() = 0;	// an abstract base class

	// handlers for event base classes - not necessary to override
	virtual void accept_event(const Event *) {}
	virtual void accept_event(const Start_event *) {}
	virtual void accept_event(const Stop_event *) {}
	virtual void accept_event(const Report_event *) {}
	virtual void accept_event(const Device_event *) {}
	virtual void accept_event(const Visual_event *) {}
	virtual void accept_event(const Eye_event *) {}
	virtual void accept_event(const Auditory_event *) {}
	virtual void accept_event(const Cognitive_event *) {}
	virtual void accept_event(const Motor_event *) {}
	virtual void accept_event(const Manual_event *) {}
	virtual void accept_event(const Vocal_event *) {}
	virtual void accept_event(const Temporal_event *) {}
	virtual void accept_event(const WM_event *) {}
	virtual void accept_event(const LTM_event *) {}
	virtual void accept_event(const Task_event *) {}
	virtual void accept_event(const High_Level_event *) {}
	
	// services for external and derived classes
	const std::string& get_name() const {return processor_name;}
	virtual std::string processor_info() const;	// produce string containing output info
	void set_trace(bool trace_)
		{trace = trace_;}
	bool get_trace() const
		{return trace;}
	static long get_time();
	static void schedule_event(const Event * p);
	static void stop_simulation();

private:
	std::string processor_name;
	bool trace;	// on for trace output generated for this processor
	bool added;	// true if originally added to simulation in ctor
};


#endif

