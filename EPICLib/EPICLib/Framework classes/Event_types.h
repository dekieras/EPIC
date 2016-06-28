// Event pointers are passed around. The events are const objects;
// once created, their contents cannot be altered.
// All members of Events are public for convenience in writing the many functions
// that need to access their content.
// The originator of an event calls the create function to create an event, 
// which returns a pointer to a const Event.
// Event receivers must declare parameter as const Event_type *.

#ifndef EVENT_TYPES_H
#define EVENT_TYPES_H

#include "Event_base.h"

#include "Symbol.h"

// forward declarations
class Processor;
//class WM_processor;	
//class LTM_processor;
//class Task_processor;
class Eye_processor;	
class Visual_sensory_store;	
class Visual_perceptual_processor;	
class Visual_perceptual_store;	

class Auditory_store;
class Auditory_physical_store;
class Ear_processor;	
class Auditory_sensory_store;	
class Auditory_perceptual_processor;	
class Auditory_perceptual_store;	

class Motor_processor;	
//class Manual_processor;	
//class Ocular_processor;	
//class Invol_ocular_processor;	
//class Vocal_processor;	

class Cognitive_processor;	

class Device_processor;	

// These event classes are used in all processors and are not
// base classes.

class Start_event : public Event {
public:
	Start_event(long t, Processor * d) : Event(t, d) {}		
	virtual void send_self(Processor *) const;
};

class Stop_event : public Event {
public:
	Stop_event(long t, Processor * d) : Event(t, d) {}
	virtual void send_self(Processor *) const;
};

// processor-specific Event classes - these are base classes with protected constructors
class Eye_event : public Event {
public:
	virtual void send_self(Processor *) const;
	virtual void handle_self(Eye_processor *) const = 0;
protected:
	Eye_event(long t, Processor * d) : Event(t, d) {}
};

class Visual_event : public Event {
public:	
	virtual void send_self(Processor *) const;
	virtual void handle_self(Eye_processor *) const;
	virtual void handle_self(Visual_sensory_store *) const;
	virtual void handle_self(Visual_perceptual_processor *) const;
	virtual void handle_self(Visual_perceptual_store *) const ;
protected:
	Visual_event(long t, Processor * d) : Event(t, d) {}
};

// There are no Ear event types because the Ear_processor takes only direct inputs.
// The Auditory_physical_store accepts events to simplify inputs of long duration
class Auditory_event : public Event {
public:
	virtual void send_self(Processor *) const;
	virtual void handle_self(Auditory_store *) const;
	virtual void handle_self(Auditory_physical_store *) const;
	virtual void handle_self(Ear_processor *) const;
	virtual void handle_self(Auditory_sensory_store *) const;
	virtual void handle_self(Auditory_perceptual_processor *) const;
	virtual void handle_self(Auditory_perceptual_store *) const ;
protected:
	Auditory_event(long t, Processor * d) : Event(t, d) {}
};

class Cognitive_event : public Event {
public:		
	virtual void send_self(Processor *) const;
	virtual void handle_self(Cognitive_processor *) const = 0;
protected:
	Cognitive_event(long t, Processor * d) : Event(t, d) {}
};


class Motor_event : public Event {
public:	
	virtual void send_self(Processor *) const;
	virtual void handle_self(Motor_processor *) const = 0;
protected:
	Motor_event(long t, Processor * d) : Event(t, d) {}
};


// A temporal 'increment' event gets put in the queue.  The coordinator goes through the queue and tells
//   the event to send an instruction to its processor to handle that event.
class Temporal_event : public Event {
public:	
	Temporal_event(long t, Processor * d) : Event(t, d)
		{}
	virtual void send_self(Processor *) const;
};


/*
class Manual_event : public Event {
public:
	virtual void send_self(Processor *) const;
	virtual void handle_self(Manual_processor *) const = 0;
	bool execute_when_prepared;
protected:
	Manual_event(long t, Processor * d, bool e = true) : 
		Event(t, d), execute_when_prepared(e) {}
private:
	Manual_event();
};

class Ocular_event : public Event {
public:
	virtual void send_self(Processor *) const;
	virtual void handle_self(Ocular_processor *) const = 0;
	Symbol specification;
	bool execute_when_prepared;
protected:
	Ocular_event(long t, Processor * d, const Symbol& spc, bool e = true) : 
		Event(t, d), specification (spc), execute_when_prepared(e) {}
private:
	Ocular_event();
		
};

class Invol_ocular_event : public Event {
public:
	virtual void send_self(Processor *) const;
	virtual void handle_self(Invol_ocular_processor *) const = 0;
	Symbol specification;
protected:
	Invol_ocular_event(long t, Processor * d, const Symbol& spc) : 
		Event(t, d), specification (spc) {}
private:
	Invol_ocular_event();
		
};

class Vocal_event : public Event {
public:
	virtual void send_self(Processor *) const;
	virtual void handle_self(Vocal_processor *) const = 0;
	Symbol specification;
	bool execute_when_prepared;
protected:
	Vocal_event(long t, Processor * d, const Symbol& spc, bool e = true) : 
		Event(t, d), specification (spc), execute_when_prepared(e) {}
private:
	Vocal_event();
};

class WM_event : public Event {
public:
	virtual void send_self(Processor *) const;
	virtual void handle_self(WM_processor *) const = 0;
protected:
	WM_event(long t, Processor * d) : 
		Event(t, d) {}
private:
	WM_event();
};

class LTM_event : public Event {
public:
	virtual void send_self(Processor *) const;
	virtual void handle_self(LTM_processor *) const = 0;
protected:
	LTM_event(long t, Processor * d) :
		Event(t, d) {}
private:
	LTM_event();
};

class Task_event : public Event {
public:
	virtual void send_self(Processor *) const;
	virtual void handle_self(Task_processor *) const = 0;
protected:
	Task_event(long t, Processor * d) :
		Event(t, d) {}
private:
	Task_event();
};
*/

class Device_event : public Event {
public:
	virtual void send_self(Processor *) const;
	virtual void handle_self(Device_processor *) const = 0;
protected:
	Device_event(long t, Processor * d) : Event(t, d) {}
};

#endif


