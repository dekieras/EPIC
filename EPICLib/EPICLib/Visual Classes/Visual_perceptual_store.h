#ifndef VISUAL_PERCEPTUAL_STORE_H
#define VISUAL_PERCEPTUAL_STORE_H


#include "Visual_store.h"
#include "Symbol.h"
#include "Geometry.h"
namespace GU = Geometry_Utilities;

#include <map>


/* Visual_perceptual_store contains Perceptual objects with additional recoded properties.
By definition, Sensory objects are visible.
They have location, size, and properties, but only recoded properties are
possed on to the cognitive processors.  

The cognitive processor will recode the updates, accessing the information
in the perceptual store to do so.

See .cpp file for more complete description

*/

class Visual_perceptual_object : public Visual_store_object {
public:
	static std::shared_ptr<Visual_store_object> create(const Symbol& name, GU::Point location, GU::Size size)
				{return std::shared_ptr<Visual_perceptual_object>(new Visual_perceptual_object(name, location, size));}

	// access the object disappearance time
	virtual void set_disappearing_time(long disappearing_time_)
		{disappearing_time = disappearing_time_;}
	virtual void remove_disappearing_time()
		{disappearing_time = 0;}
	virtual long get_disappearing_time() const
		{return disappearing_time;}
	
	// access the list of disappearing properties
	virtual void set_disappearing_time(const Symbol& property_name, long disappearing_time);
	virtual void remove_disappearing_time(const Symbol& property_name);
	virtual long get_disappearing_time(const Symbol& property_name) const;

	enum Visual_perceptual_object_state_e {PRESENT, UNSUPPORTED, RETAINED};

	Visual_perceptual_object_state_e get_state() const
		{return state;}
	void set_state(Visual_perceptual_object_state_e state_)
		{state = state_;}

	void display_contents(Output_tee& ot) const;
	
private:

	Visual_perceptual_object(const Symbol& name, GU::Point location, GU::Size size) :
		Visual_store_object(name, location, size), state(Visual_perceptual_object::PRESENT), 
		disappearing_time(0)
		{}

	Visual_perceptual_object_state_e state;
	long disappearing_time;
	// regular list of properties has those that are supported by perceptual input
	// the long variable here stores the time at which the state transition should occur;
	std::map<Symbol, long> unsupported_properties;	// list of properties no longer supported
	
	// no copy, assignment
	Visual_perceptual_object(const Visual_perceptual_object&);
	Visual_perceptual_object& operator= (const Visual_perceptual_object&);

};

class Visual_perceptual_store : public Visual_store {
public:
	Visual_perceptual_store(Human_processor * human_ptr_) :
		Visual_store("Visual_perceptual_store", human_ptr_),
		change_decay_time("Change_decay_time", 75),
//		offset_decay_time("Offset_decay_time", 75),
		// time for an object to persist in an unsupported state
		unsupported_object_decay_time("Unsupported_object_decay_time", 200),
		// time for an object to remain in a retained state
		retained_object_decay_time("Retained_object_decay_time", 500),
		// time for a property to decay for an object that is present
		// if the object disappears, the property is lost immediately
		property_decay_time("Property_decay_time", 500)
		{
			add_parameter(change_decay_time);
//			add_parameter(offset_decay_time);
			add_parameter(unsupported_object_decay_time);
			add_parameter(retained_object_decay_time);
			add_parameter(property_decay_time);
		}

//	virtual void initialize()

	// event interface
//	virtual void accept_event(const Start_event *);
//	virtual void accept_event(const Stop_event *);

	// these events arrive from the Perceptual_processor
	virtual void accept_event(const Visual_event *);
	virtual void handle_event(const Visual_Appear_event *);
	virtual void handle_event(const Visual_Disappear_event *);
	virtual void handle_event(const Visual_Erase_event *);
	virtual void handle_event(const Visual_Change_Location_event *);
	virtual void handle_event(const Visual_Change_Size_event *);
	virtual void handle_event(const Visual_Change_Property_event *);
	virtual void handle_event(const Visual_Erase_Property_event *);
	
	std::shared_ptr<Visual_perceptual_object> get_perceptual_object_ptr(const Symbol& name);

//	void display_contents(Output_tee& ot) const;

private:
	// parameters
//	Parameter appearance_delay;
//	Parameter disappearance_delay;
//	Parameter location_delay;
//	Parameter size_delay;
	Parameter change_decay_time;
//	Parameter offset_decay_time;
	Parameter unsupported_object_decay_time;
	Parameter retained_object_decay_time;
	Parameter property_decay_time;

	void lose_object_from_memory(std::shared_ptr<Visual_perceptual_object> obj_ptr);
//	bool update_retained_objects();
	void update_property(const Symbol& object_name, const Symbol& property_name, const Symbol& new_value);
	// no copy, assignment
	Visual_perceptual_store(const Visual_perceptual_store&);
	Visual_perceptual_store& operator= (const Visual_perceptual_store&);
};



#endif
