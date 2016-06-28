#ifndef VISUAL_SENSORY_STORE_H
#define VISUAL_SENSORY_STORE_H

#include "Visual_store.h"
#include "Symbol.h"
#include "Geometry.h"
namespace GU = Geometry_Utilities;

#include <map>

/* Visual_sensory_store contains objects as they appear after having been filtered
through the retina. They have location, size, and properties, but do not have
defined positions on the retina. By definition, Sensory objects are visible.

Sensory objects are almost just the default Visual_store_object type.
*/

class Visual_sensory_object : public Visual_store_object {
public:
	static std::shared_ptr<Visual_store_object> create(const Symbol& psychological_name_, GU::Point location_, GU::Size size_)
				{return std::shared_ptr<Visual_store_object>(new Visual_sensory_object(psychological_name_, location_, size_));}

	// access the object disappearance time
	virtual void set_disappearing_time(long disappearing_time_)
		{disappearing_time = disappearing_time_;}
	virtual void remove_disappearing_time()
		{disappearing_time = 0;}
	virtual long get_disappearing_time() const
		{return disappearing_time;}

	// access the list of property event times
	virtual void set_property_event_time(const Symbol& property_name, long event_time);
	virtual void remove_property_event_time(const Symbol& property_name);
	virtual long get_property_event_time(const Symbol& property_name) const;

	void display_contents(Output_tee& ot) const;
	
private:

	Visual_sensory_object(const Symbol& psychological_name_, GU::Point location_, GU::Size size_) :
		Visual_store_object(psychological_name_, location_, size_),
		disappearing_time(0)
		{}

	long disappearing_time;
	std::map<Symbol, long> property_event_times;	// list of event time for each property

	// no copy, assignment
	Visual_sensory_object(const Visual_sensory_object&);
	Visual_sensory_object& operator= (const Visual_sensory_object&);
};

class Visual_sensory_store : public Visual_store {
public:
	Visual_sensory_store(Human_processor * human_ptr_) :
		Visual_store("Visual_sensory_store", human_ptr_),
		disappearance_decay_delay("Disappearance_decay_delay_time", 200)
		{add_parameter(disappearance_decay_delay);}

//	virtual void initialize();
		
	// event interface
//	virtual void accept_event(const Start_event *);
//	virtual void accept_event(const Stop_event *);

	// these events arrive from the Eye_processor
	virtual void accept_event(const Visual_event *);
	virtual void handle_event(const Visual_Appear_event *);
	virtual void handle_event(const Visual_Disappear_event *);
	virtual void handle_event(const Visual_Erase_event *);
	virtual void handle_event(const Visual_Change_Location_event *);
	virtual void handle_event(const Visual_Change_Size_event *);
	virtual void handle_event(const Visual_Change_Property_event *);
	virtual void handle_event(const Visual_Erase_Property_event *) {}	// dummy - why needed?

//	void display_contents(Output_tee& ot) const;

private:
	// parameters
	Parameter disappearance_decay_delay;

	
	// no copy, assignment
	Visual_sensory_store(const Visual_sensory_store&);
	Visual_sensory_store& operator= (const Visual_sensory_store&);
};



#endif
