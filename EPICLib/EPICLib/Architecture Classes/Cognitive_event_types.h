#ifndef COGNITIVE_EVENT_TYPES_H
#define COGNITIVE_EVENT_TYPES_H

//#include <iostream>
#include "Event_types.h"
#include "Symbol.h"

// forward declarations
class Cognitive_processor;
 

class Cognitive_Cycle_event : public Cognitive_event {
public:
	Cognitive_Cycle_event(long t, Processor * r) : Cognitive_event(t, r) {}
	virtual void handle_self(Cognitive_processor *) const;
};

// this class is used to update an item in a working memory specified with a tag, an "object" name, a property name,
// an old value to be removed, and a new value to be added. If an old or new value is "Nil", then the value is ignored.
// E.g. a new value of Nil is not added, but the old value will be delete.
class Cognitive_Update_event : public Cognitive_event {
public:
	Cognitive_Update_event(long t, Processor * d, const Symbol& tag_name_, const Symbol& object_name_, const Symbol& property_name_,
			const Symbol& old_value_, const Symbol& new_value_) : Cognitive_event(t, d), 
			tag_name(tag_name_), object_name(object_name_), property_name(property_name_), old_value(old_value_), new_value(new_value_)
		{}
	virtual void handle_self(Cognitive_processor *) const;
	Symbol tag_name;
	Symbol object_name;
	Symbol property_name;
	Symbol old_value;
	Symbol new_value;
};

/*
class Cognitive_Visual_Update_event : public Cognitive_event {
public:
	static const Event * create(long t, Processor * r, 
		const Symbol& obj_name, const Symbol& property_name_, const Symbol& old_value_, const Symbol& new_value_)
		{return new Cognitive_Visual_Update_event(t, r, obj_name, property_name_, old_value_, new_value_);}
	virtual const Event * clone() const
		
		{return new Cognitive_Visual_Update_event(*this);}
	virtual void handle_self(Cognitive_processor *) const;
	Symbol object_name;
	Symbol property_name;
	Symbol old_value;
	Symbol new_value;
private:
	Cognitive_Visual_Update_event(long t, Processor * r, 
		const Symbol& object_name_, const Symbol& property_name_, const Symbol& old_value_, const Symbol& new_value_) :
		Cognitive_event(t, r), object_name(object_name_), property_name(property_name_), old_value(old_value_), new_value(new_value_)
		{}
};

class Cognitive_Motor_State_Update_event : public Cognitive_event {
public:
	static const Event * create(long t, Processor * r, const Symbol& processor_name_, const Symbol& phase_name_,
			const Symbol& old_state_, const Symbol& new_state_)
		{return new Cognitive_Motor_State_Update_event(t, r, processor_name_, phase_name_, old_state_, new_state_);}
	virtual const Event * clone() const
		
		{return new Cognitive_Motor_State_Update_event(*this);}
	virtual void handle_self(Cognitive_processor *) const;
	Symbol processor_name;
	Symbol phase_name;
	Symbol old_state;
	Symbol new_state;
private:
	Cognitive_Motor_State_Update_event(long t, Processor * r, const Symbol& processor_name_, const Symbol& phase_name_,
			const Symbol& old_state_, const Symbol& new_state_) : Cognitive_event(t, r), 
			processor_name(processor_name_), phase_name(phase_name_), old_state(old_state_), new_state(new_state_)
		{}
};
*/
class Cognitive_Add_Clause_event : public Cognitive_event {
public:
	Cognitive_Add_Clause_event(long t, Processor * r, const Symbol_list_t & clause_) :
		Cognitive_event(t, r), clause(clause_)
		{}
	virtual void handle_self(Cognitive_processor *) const;
	Symbol_list_t clause;
private:
};

class Cognitive_Delete_Clause_event : public Cognitive_event {
public:
	Cognitive_Delete_Clause_event(long t, Processor * r, const Symbol_list_t & clause_) :
		Cognitive_event(t, r), clause(clause_)
		{}
	virtual void handle_self(Cognitive_processor *) const;
	Symbol_list_t clause;
};


/*
class Cognitive_Operation_Complete_event : public Cognitive_event {
public:
	Cognitive_Operation_Complete_event(long t, Processor * r) : Cognitive_event(t, r) {}
		
	virtual void handle_self(Cognitive_processor *) const;
};

class Cognitive_Visual_Operation_Complete_event : public Cognitive_Operation_Complete_event {
public:
	Cognitive_Visual_Operation_Complete_event(long t, Processor * r) : Cognitive_Operation_Complete_event(t, r) {}
		
	virtual void handle_self(Cognitive_processor *) const;
};

class Cognitive_Auditory_Operation_Complete_event : public Cognitive_Operation_Complete_event {
public:
	Cognitive_Auditory_Operation_Complete_event(long t, Processor * r) : Cognitive_Operation_Complete_event(t, r) {}
		
	virtual void handle_self(Cognitive_processor *) const;
};

class Cognitive_Manual_Operation_Complete_event : public Cognitive_Operation_Complete_event {
public:
	Cognitive_Manual_Operation_Complete_event(long t, Processor * r) : Cognitive_Operation_Complete_event(t, r) {}
		
	virtual void handle_self(Cognitive_processor *) const;
};

class Cognitive_Vocal_Operation_Complete_event : public Cognitive_Operation_Complete_event {
public:
	Cognitive_Vocal_Operation_Complete_event(long t, Processor * r) : Cognitive_Operation_Complete_event(t, r) {}
		
	virtual void handle_self(Cognitive_processor *) const;
};

class Cognitive_Task_Operation_Complete_event : public Cognitive_Operation_Complete_event {
public:
	Cognitive_Task_Operation_Complete_event(long t, Processor * r) : Cognitive_Operation_Complete_event(t, r) {}
		
	virtual void handle_self(Cognitive_processor *) const;
};

class Cognitive_LTM_Operation_Complete_event : public Cognitive_Operation_Complete_event {
public:
	Cognitive_LTM_Operation_Complete_event(long t, Processor * r) : Cognitive_Operation_Complete_event(t, r) {}
		
	virtual void handle_self(Cognitive_processor *) const;
};


class Cognitive_Delay_event : public Cognitive_event {
public:
	long delay;
	
	Cognitive_Delay_event(long t, Processor * r, long dly) : Cognitive_event(t, r), delay(dly) {}
		
	virtual void handle_self(Cognitive_processor *) const;
};


class Cognitive_Accomplish_Goal_event : public Cognitive_event {
public:
	GOMS_Goal goal;
	
	Cognitive_Accomplish_Goal_event(long t, Processor * r, const GOMS_Goal& g) : Cognitive_event(t, r), goal(g) {}
		
	virtual void handle_self(Cognitive_processor *) const;
};

class Cognitive_Return_Goal_Accomplished_event : public Cognitive_event {
public:
	GOMS_Goal goal;
	
	Cognitive_Return_Goal_Accomplished_event(long t, Processor * r, const GOMS_Goal& g) : Cognitive_event(t, r), goal(g) {}
		
	virtual void handle_self(Cognitive_processor *) const;
};

class Cognitive_Abort_and_Restart_event : public Cognitive_event {
public:
	GOMS_Goal goal;
	
	Cognitive_Abort_and_Restart_event(long t, Processor * r, const GOMS_Goal& g) : Cognitive_event(t, r), goal(g) {}
		
	virtual void handle_self(Cognitive_processor *) const;
};

*/

#endif
