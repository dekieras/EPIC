#ifndef VISUAL_PERCEPTUAL_PROCESSOR_H
#define VISUAL_PERCEPTUAL_PROCESSOR_H

#include <map>
#include "Output_tee.h"
#include "Human_subprocessor.h"
#include "Visual_store_processor.h"
#include "Visual_sensory_store.h"
#include "Parameter.h"

/* 
Visual_sensory_store does direct calls to this processor, which recodes the information
and sends scheduled event to Visual_perceptual_store, with various delays as appropriate,
and also transient properties ???

At this time, all perceptual recoding is done here - the Visual_perceptual_store
simply passes-throughrelevant information to the cognitive processor.
*/

#include "Geometry.h"
namespace GU = Geometry_Utilities;
//#include "Visual_perceptual_processor_event_types.h"
//#include "Visual_event_types.h"

class Human_processor;
class Symbol;
class Visual_Delay_event;
class Visual_encoder_base;

class Visual_perceptual_processor : public Visual_store_processor {
public:
	Visual_perceptual_processor(Human_processor * human_ptr_) :
		Visual_store_processor ("Visual_perceptual_processor", human_ptr_),
		appear_disappear_delay("Appear_disappear_recoding_time", 10),
		onset_offset_decay_time("Onset_offset_decay_time", 75),
		visible_delay("Visible_recoding_time", 55),
//		onset_delay("Onset_recoding_time", 10),
//		disappearance_delay("Disappearance_recoding_time", 50),
//		offset_decay_time("Offset_decay_time", 75),
		location_delay("Location_change_recoding_time", 25),
		size_delay("Size_change_recoding_time", 25),
//		property_time_fluctuation("Property_delay_fluctuation_factor", Parameter::Normal, Parameter::Never, 1.0, .5),
		recoding_time_fluctuation("Recoding_time_fluctuation_factor", 1.0),
//		text_recoding_failure_rate("Text_recoding_failure_rate", 0.0),	// text is encoded as Unknown
//		text_partial_recoding_rate("Text_partial_recoding_rate", 0.0),	// text is encoded with prefix of Partial_
		recoding_failure_rate1("Recoding_failure_rate1", 0.0),	// Custom failure rate #1
		recoding_failure_rate2("Recoding_failure_rate2", 0.0),	// Custom failure rate #2
		recoding_failure_rate3("Recoding_failure_rate3", 0.0),	// Custom failure rate #3
		recoding_failure_rate4("Recoding_failure_rate4", 0.0),	// Custom failure rate #4
		visual_encoder_ptr(0)
		{
			setup();
		}

	virtual void initialize();
	
	// set externally with an address supplied by a dynamic library - connects this processor back to it.
	void set_visual_encoder_ptr(Visual_encoder_base * visual_encoder_ptr_);
	Visual_encoder_base * get_visual_encoder_ptr() const
		{return visual_encoder_ptr;}

	void set_parameter(const Parameter_specification& param_spec);
	void describe_parameters(Output_tee& ot) const;
	
	// inputs
	virtual void make_object_appear(const Symbol& name, GU::Point location, GU::Size size);
	virtual void make_object_disappear(const Symbol& obj_name);
	virtual void set_object_location(const Symbol& obj_name, GU::Point location);
	virtual void set_object_size(const Symbol& obj_name, GU::Size size);
	virtual void set_object_property(const Symbol& obj_name, const Symbol& propname, const Symbol& propvalue);

	// event interface
//	virtual void accept_event(const Start_event *);
	virtual void accept_event(const Visual_event *);
	virtual void handle_event(const Visual_Delay_event *);
	virtual void accept_event(const Stop_event *);
	
	// allow Visual_encoder_base to have access
	friend class Visual_encoder_base;

private:
	// state

	// parameters
	Parameter appear_disappear_delay;
	Parameter onset_offset_decay_time;
	Parameter visible_delay;
//	Parameter disappearance_delay;
//	Parameter onset_decay_time;
//	Parameter offset_decay_time;
	Parameter location_delay;
	Parameter size_delay;
//	Parameter property_time_fluctuation;
	Parameter recoding_time_fluctuation;
//	Parameter text_recoding_failure_rate;
//	Parameter text_partial_recoding_rate;
	Parameter recoding_failure_rate1;
	Parameter recoding_failure_rate2;
	Parameter recoding_failure_rate3;
	Parameter recoding_failure_rate4;
	// delay function maps
	typedef std::map<Symbol, long> Recoding_map_t;
	Recoding_map_t recoding_times;
	
	// helper functions
	void setup();
	std::shared_ptr<Visual_sensory_object> get_sensory_object_ptr(const Symbol& object_name);
	
	// pointer to custom encoder object
	Visual_encoder_base * visual_encoder_ptr;	// set externally with address supplied by a dynamic library
	
	// no default copy, assignment
	Visual_perceptual_processor(const Visual_perceptual_processor&);
	Visual_perceptual_processor& operator= (const Visual_perceptual_processor&);
};

#endif
