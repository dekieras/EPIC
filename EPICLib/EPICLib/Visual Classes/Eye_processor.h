/*
The eye processor requires that an object first appear with location and size, then additional
properties can be specified. The location and size can be changed with special calls. If a property
disappears, it is indicated by supplying the default value for the property value.  The location
and size properties are special because they are used to determine the retinal availability
of the sensory property.

This is where the physical_name and psychological name are first associated. 
All inputs come from the physical environment and are in terms of the physical name.
All outputs go to psychological entities and are in terms of the (psychological) name.
The physical_name accompanies the object just so it can be sent to device in a motor
action for convenience in enabling easy programming of the device.
*/

#ifndef EYE_PROCESSOR_H
#define EYE_PROCESSOR_H

//#include "Output_tee.h"
#include "Human_subprocessor.h"
#include "Visual_store_processor.h"
#include "Geometry.h"
namespace GU = Geometry_Utilities;
#include "Parameter.h"
#include "Visual_physical_store.h"
#include "Eye_event_types.h"
#include "Visual_event_types.h"
#include "Eye_retina_functions.h"
#include "Name_map.h"
#include "Statistics.h"

#include <map>
//#include <set>

class Human_processor;
class Symbol;

// centering_enabled initialized off due to some apparent bugs in choice  of object 6/7/03
class Eye_processor : public Visual_store_processor {
public:
	Eye_processor(Human_processor * human_ptr_) :
		Visual_store_processor ("Eye", human_ptr_), 
		voluntary_saccade_underway(false), involuntary_saccade_underway(false), involuntary_smooth_move_underway(false), 
		eye_movement_complete_time(0), closest_eccentricity(0.),
		centering_enabled(false), centering_on(false), centering_active(false),
		reflex_enabled(false), reflex_on(false),
		auditory_reflex_enabled(false), auditory_reflex_on(false),
        involuntary_movement_in_progress(false), last_invol_ocular_command_time(0),
		appearance_disappearance_delay("Appear_disappear_transduction_time", 20),
//		disappearance_delay("Disappearance_transduction_time", 50),
		eccentricity_delay("Eccentricity_transduction_time", 50),	// longer than appearance_delay
		location_delay("Location_change_transduction_time", 20),
		size_delay("Size_change_transduction_time", 20),
		eccentricity_fluctuation("Eccentricity_fluctuation_factor", Parameter::Normal, Parameter::Never, 1.0, .1),
		property_time_fluctuation("Property_delay_fluctuation_factor", Parameter::Normal, Parameter::Never, 1.0, .5),
		bouquet_radius("Foveal_bouquet_radius", .25),
		centering_smooth_move_radius("Centering_smooth_move_radius", 1.0),
		centering_saccade_radius("Centering_saccade_radius", 10.0),
		inform_invol_ocular_delay("Inform_invol_ocular_delay", 20), psychobj_counter(0) //,
		//availability_distribution(10., 1.0)
		{setup();}

	virtual void initialize();
	virtual void set_parameter(const Parameter_specification& param_spec);
	virtual void describe_parameters(Output_tee& ot) const;
	// monitoring interface
	//Proportion_accumulator& get_availability_accumulator() {return availability_accumulator;}
	
	// inputs - physical name is supplied
	virtual void make_object_appear(const Symbol& name, GU::Point location, GU::Size size);
	virtual void make_object_disappear(const Symbol& obj_name);
	virtual void set_object_location(const Symbol& obj_name, GU::Point location);
	virtual void set_object_size(const Symbol& obj_name, GU::Size size);
	virtual void set_object_property(const Symbol& obj_name, const Symbol& propname, const Symbol& propvalue);


	// event interface
//	virtual void accept_event(const Start_event *);
	virtual void accept_event(const Stop_event *);

	virtual void accept_event(const Eye_event *);
	virtual void accept_event(const Visual_event *);
	
	// these visual events are self-generated and responded to if an eye movement 
	// is in progress when an input is supplied
//	void handle_event(const Visual_Appear_event *);
	void handle_event(const Visual_Disappear_event *);
	void handle_event(const Visual_Erase_event *) {}
	void handle_event(const Visual_Change_Location_event *);
	void handle_event(const Visual_Change_Size_event *);
	void handle_event(const Visual_Change_Property_event *);
	// these events are supplied by the ocular motor processors
	void handle_event(const Eye_Voluntary_Saccade_Start_event *);
	void handle_event(const Eye_Voluntary_Saccade_End_event *);
	void handle_event(const Eye_Involuntary_Saccade_Start_event *);
	void handle_event(const Eye_Involuntary_Saccade_End_event *);
	void handle_event(const Eye_Involuntary_Smooth_Move_Start_event *);
	void handle_event(const Eye_Involuntary_Smooth_Move_End_event *);
	
	// services
	bool get_voluntary_underway() const
		{return voluntary_saccade_underway;}
	bool get_involuntary_underway() const
		{return involuntary_saccade_underway || involuntary_smooth_move_underway;}
	
	GU::Point get_location() const;	// return current or future location
	void set_centering_enabled(bool c)
		{centering_enabled = c; centering_on = c;}
	void set_reflex_enabled(bool r)
		{reflex_enabled = r; reflex_on = r;}
	void set_auditory_reflex_enabled(bool r)
		{auditory_reflex_enabled = r; auditory_reflex_on = r;}

	// allow outsiders to access and modify the name map - e.g. to remove names
	Name_map& get_name_map()
		{return name_map;}
		
//	typedef bool (*Availability_function_t)(const Visual_physical_object&, double eccentricity_fluctuation);
//	typedef long (*Delay_function_t)(const Visual_physical_object&, double time_fluctuation);

private:

	// state
	GU::Point eye_location;
	GU::Point future_eye_location;	// if eye movement underway
	bool voluntary_saccade_underway;
	bool involuntary_saccade_underway;
	bool involuntary_smooth_move_underway;
	long eye_movement_complete_time;
	std::shared_ptr<Visual_store_object> closest_object_ptr;	// current object closest to point of fixation
	double closest_eccentricity;
	bool centering_enabled;
	bool centering_on;
	bool centering_active;
	bool reflex_enabled;
	bool reflex_on;
	bool auditory_reflex_enabled;
	bool auditory_reflex_on;
	bool involuntary_movement_in_progress;
	long last_invol_ocular_command_time;	// time the last invol.ocular command would arrive

	// parameters
	Parameter appearance_disappearance_delay;
	Parameter eccentricity_delay;	// to report eccentricity
//	Parameter disappearance_delay;
	Parameter location_delay;
	Parameter size_delay;
	Parameter eccentricity_fluctuation;
	Parameter property_time_fluctuation;
	Parameter bouquet_radius;
	Parameter centering_smooth_move_radius;
	Parameter centering_saccade_radius;
	Parameter inform_invol_ocular_delay;
	
	// availability and delay function maps
	typedef std::map<Symbol, std::shared_ptr<Availability> > Availability_map_t;
	Availability_map_t availabilities;
	std::shared_ptr<Availability> default_availability;
	
	// state
	long psychobj_counter;	// counter for new object psychologial names
	Name_map name_map;	// name map for visual objects
	// monitoring
	//Proportion_accumulator availability_accumulator;
	//Distribution_accumulator availability_distribution;
	
	// helper functions
	void setup();
	Symbol get_psychological_name_and_new_status(Symbol physical_name, bool& new_object);
//	void finish_make_object_appear(const Symbol& physical_name, GU::Point location, GU::Size size);
	GU::Point location_noise(GU::Point location);
	bool update_eccentricity_and_visibility(std::shared_ptr<Visual_store_object> obj_ptr);
	bool update_location_information(std::shared_ptr<Visual_store_object> obj_ptr);
	bool apply_availability(std::shared_ptr<Visual_store_object> obj_ptr, const Symbol prop_name, Symbol& prop_value, long& delay);
	void send_eccentricity_update(const Symbol& psychological_name, double ecc);
	void send_property_availability_update(std::shared_ptr<Visual_store_object> obj_ptr,
		const Symbol& prop_name, const Symbol& prop_value);
	Symbol change_related_object_name(const Symbol& prop_name, const Symbol& prop_value);
//	void send_property_change(const Visual_physical_object& obj, 
//		const Symbol& prop_name, const Symbol& prop_value);
	void update_all_object_location_information();
	void update_all_properties(std::shared_ptr<Visual_store_object> obj_ptr);
	void update_all_object_properties();
	void update_objects_after_eye_movement();	
	void update_closest_object();
	void perform_centering();
	bool free_to_move() const;

	// no default copy, assignment
	Eye_processor(const Eye_processor&);
	Eye_processor& operator= (const Eye_processor&);
};

#endif
