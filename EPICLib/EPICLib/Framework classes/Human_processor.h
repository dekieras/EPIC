#ifndef HUMAN_PROCESSOR_H
#define HUMAN_PROCESSOR_H

#include "Human_base.h"
#include "Processor.h"
#include "Event_types.h"
#include "Symbol.h"
#include "Parameter.h"

#include <list>
#include <string>
#include <map>

#include "Geometry.h"
namespace GU = Geometry_Utilities;

// These classes are all forward declared here to reduce compile-time coupling
// forward declarations
class Human_subprocessor;
class Visual_physical_store;
class Eye_processor;
class Visual_sensory_store;
class Visual_perceptual_processor;
class Visual_perceptual_store;
class Auditory_physical_store;
class Ear_processor;
class Auditory_sensory_store;
class Auditory_perceptual_processor;
class Auditory_perceptual_store;
class Cognitive_processor;
class Manual_processor;
class Ocular_processor;
class Invol_ocular_processor;
class Vocal_processor;
class Temporal_processor;
class LTM_processor;
class Task_processor;
class WM_processor;
class Device_processor;
class Visual_encoder_base;
class Auditory_encoder_base;

class Output_tee;
class View_base;

// This class defines a Human as a collection of other processors with certain connections,
// and provides the interface between the Human and the outside world.

class Human_processor : public Human_base, public Processor {
public:
	// no public default constructor - cannot create without proper initialization
	Human_processor(const std::string& id);

	virtual ~Human_processor();
	virtual void initialize(); 
	virtual void set_parameters();
	virtual void set_parameter(const std::string& proc_name, const std::string& param_name, const std::string& spec);
	void set_parameters(const Parameter_specification_list_t& parameter_specs);
	virtual void describe_parameters(Output_tee& ot) const;

	// handlers for event base classes
	virtual void accept_event(const Start_event *);
	virtual void accept_event(const Stop_event *);

	// inputs
	void make_visual_object_appear(const Symbol& obj_name, GU::Point location, GU::Size size);
	void make_visual_object_disappear(const Symbol& obj_name);
	void set_visual_object_location(const Symbol& obj_name, GU::Point location);
	void set_visual_object_size(const Symbol& obj_name, GU::Size size);
	void set_visual_object_property(const Symbol& obj_name, const Symbol& propname, const Symbol& propvalue);

//	void create_auditory_stream(const Symbol& name, GU::Point location, GU::Size size = GU::Size());
//	void destroy_auditory_stream(const Symbol& name);
	void set_auditory_stream_location(const Symbol& name, GU::Point location);
	void set_auditory_stream_size(const Symbol& name, GU::Size size);
	void set_auditory_stream_property(const Symbol& name, const Symbol& propname, const Symbol& propvalue);
	
	void make_auditory_sound_start(const Symbol& name, const Symbol& stream_name, GU::Point location,
		const Symbol& timbre, double loudness, long intrinsic_duration);
	void make_auditory_sound_stop(const Symbol& name);
	void make_auditory_sound_event(const Symbol& name, const Symbol& stream_name, GU::Point location,
		const Symbol& timbre, double loudness, long duration, long intrinsic_duration = 0);

//	sound_name, stream_name - to allow finesse of sound/stream object identity
//	content - finessed content of the speech object - e.g. recognized word
//	speaker_gender, speaker_id - surrogates/finesse of e.g. formant structure that distinguishes one speaker from another
//	loudness - in db - can be a cue to stream identification
//	duration - of this speech object
//	void make_auditory_speech_event(const Symbol& sound_name, const Symbol& stream_name, 
//		const Symbol& content, const Symbol& speaker_gender, const Symbol& speaker_id, double loudness, long duration);
	virtual void make_auditory_speech_event(const Speech_word& word);

	void set_auditory_sound_property(const Symbol& name, const Symbol& propname, const Symbol& propvalue);

	// services 
	bool compile(const std::string& filename);
	std::string get_prs_filename() const
		{return prs_filename;}
	void initialize_recorders();
	void add_to_recorder_list (Processor *);
	void remove_from_recorder_list (Processor *);
	void broadcast_to_recorders(const Event *);

	void broadcast_to_list (const Event *); // ???

    void display_memory_contents(); // output contents of current PS WM

	// connection functions
	void connect(Device_processor * dpp)
		{device_ptr = dpp;}
		
	Visual_encoder_base * get_visual_encoder_ptr() const;
	void set_visual_encoder_ptr(Visual_encoder_base * visual_encoder_ptr); 

	Auditory_encoder_base * get_auditory_encoder_ptr() const;
	void set_aditory_encoder_ptr(Auditory_encoder_base * auditory_encoder_ptr); 
	
	// These functions (with class name capitalized) return a constituent processor pointer 
	// of the actual processor derived-class type.
	Device_processor * get_Device_processor_ptr() const
		{return device_ptr;}
	Visual_physical_store * get_Visual_physical_store_ptr() const
		{return visual_physical_store_ptr;}
	Eye_processor * get_Eye_processor_ptr() const
		{return eye_ptr;}
	Visual_sensory_store * get_Visual_sensory_store_ptr() const
		{return visual_sensory_store_ptr;}
	Visual_perceptual_processor * get_Visual_perceptual_processor_ptr() const
		{return visual_perceptual_processor_ptr;}
	Visual_perceptual_store * get_Visual_perceptual_store_ptr() const
		{return visual_perceptual_store_ptr;}
	Auditory_physical_store * get_Auditory_physical_store_ptr() const
		{return auditory_physical_store_ptr;}
	Ear_processor * get_Ear_processor_ptr() const
		{return ear_ptr;}
	Auditory_sensory_store * get_Auditory_sensory_store_ptr() const
		{return auditory_sensory_store_ptr;}
	Auditory_perceptual_processor * get_Auditory_perceptual_processor_ptr() const
		{return auditory_perceptual_processor_ptr;}
	Auditory_perceptual_store * get_Auditory_perceptual_store_ptr() const
		{return auditory_perceptual_store_ptr;}
	Cognitive_processor * get_Cognitive_processor_ptr() const
		{return cognitive_ptr;}
	Manual_processor * get_Manual_processor_ptr() const
		{return manual_ptr;}
	Ocular_processor * get_Ocular_processor_ptr() const
		{return ocular_ptr;}
	Invol_ocular_processor * get_Invol_ocular_processor_ptr() const
		{return invol_ocular_ptr;}
	Vocal_processor * get_Vocal_processor_ptr() const
		{return vocal_ptr;}
	Temporal_processor * get_Temporal_processor_ptr() const
		{return temporal_ptr;}
//	LTM_processor * get_LTM_processor_ptr() const
//		{return ltm_ptr;}
//	Task_processor * get_Task_processor_ptr() const
//		{return task_ptr;}
	
	// These functions return a pointer of the base class type Processor *, 
	// for convenience in event dispatching. 
	// They are not defined inline, in order to help decouple the header files 
	// throughout the system because all processor headers then do not need to be included here.
	// (e.g. to convert Eye_processor * to Processor *, inheritance must be known)
	Processor * get_visual_physical_store_ptr() const;
	Processor * get_eye_ptr() const;
	Processor * get_visual_sensory_store_ptr() const;
	Processor * get_visual_perceptual_processor_ptr() const;
	Processor * get_visual_perceptual_store_ptr() const;
	Processor * get_auditory_physical_store_ptr() const;
	Processor * get_ear_ptr() const;
	Processor * get_auditory_sensory_store_ptr() const;
	Processor * get_auditory_perceptual_processor_ptr() const;
	Processor * get_auditory_perceptual_store_ptr() const;
	Processor * get_cognitive_ptr() const;
	Processor * get_manual_ptr() const;
	Processor * get_ocular_ptr() const;
	Processor * get_invol_ocular_ptr() const;
	Processor * get_vocal_ptr() const;
	Processor * get_temporal_ptr() const;
//	Processor * get_ltm_ptr() const;
//	Processor * get_task_ptr() const;
	Processor * get_device_ptr() const;
	
	// view functions
	void add_visual_physical_view(View_base * view_ptr);
	void add_visual_sensory_view(View_base * view_ptr);
	void add_visual_perceptual_view(View_base * view_ptr);
	
	void remove_visual_physical_view(View_base * view_ptr);
	void remove_visual_sensory_view(View_base * view_ptr);
	void remove_visual_perceptual_view(View_base * view_ptr);
	
	void add_auditory_physical_view(View_base * view_ptr);
	void add_auditory_sensory_view(View_base * view_ptr);
	void add_auditory_perceptual_view(View_base * view_ptr);
	
	void remove_auditory_physical_view(View_base * view_ptr);
	void remove_auditory_sensory_view(View_base * view_ptr);
	void remove_auditory_perceptual_view(View_base * view_ptr);
	
	// functions to get/set output flags in Production System
	bool get_output_compiler_messages() const;
	void set_output_compiler_messages(bool flag);

	bool get_output_compiler_details() const;
	void set_output_compiler_details(bool flag);

	bool get_output_run_messages() const;
	void set_output_run_messages(bool flag);

	bool get_output_run_details() const;
	void set_output_run_details(bool flag);

	bool get_output_run_memory_contents() const;
	void set_output_run_memory_contents(bool flag);

	// tracing accessors - these can modify the trace state of multiple subprocessors
	bool get_trace_all() const
		{return trace_all;}
	void set_trace_all(bool);
	bool get_trace_visual() const
		{return trace_visual;}
	void set_trace_visual(bool);
	bool get_trace_auditory() const
		{return trace_auditory;}
	void set_trace_auditory(bool);
	bool get_trace_cognitive() const
		{return trace_cognitive;}
	void set_trace_cognitive(bool);
	bool get_trace_ocular() const
		{return trace_ocular;}
	void set_trace_ocular(bool);
	bool get_trace_manual() const
		{return trace_manual;}
	void set_trace_manual(bool);
	bool get_trace_vocal() const
		{return trace_vocal;}
	void set_trace_vocal(bool);	
	bool get_trace_temporal() const
		{return trace_temporal;}
	void set_trace_temporal(bool);	
	
	// break and fault accessors
	const Symbol_list_t& get_rule_names() const;
	bool is_rule_name_valid(const Symbol& rule_name);
	bool get_break_enabled() const;
	void set_break_enabled(bool);
	void set_rule_break_state(const Symbol& rule_name, bool state);
	bool get_rule_break_state(const Symbol& rule_name) const;
	const Symbol_list_t& get_break_rule_names() const;
	bool get_fault_enabled() const;
	void set_fault_enabled(bool);
	void set_rule_fault_state(const Symbol& rule_name, bool state);
	bool get_rule_fault_state(const Symbol& rule_name) const;
	const Symbol_list_t& get_fault_rule_names() const;
	
private:
	typedef std::list<Processor *> Recorders_t;
	Recorders_t recorder_list;	// list of recorder processors
	typedef std::map<std::string, Human_subprocessor *> Processors_t;
	Processors_t processors;	// to look up processors by name
	std::string prs_filename;	// keep the last compiled filename here

	// connection to device
	Device_processor * device_ptr;
	
	// pointers to sub-processors
	Visual_physical_store * visual_physical_store_ptr;
	Eye_processor * eye_ptr;
	Visual_sensory_store * visual_sensory_store_ptr;
	Visual_perceptual_processor * visual_perceptual_processor_ptr;
	Visual_perceptual_store * visual_perceptual_store_ptr;
	Auditory_physical_store * auditory_physical_store_ptr;
	Ear_processor * ear_ptr;
	Auditory_sensory_store * auditory_sensory_store_ptr;
	Auditory_perceptual_processor * auditory_perceptual_processor_ptr;
	Auditory_perceptual_store * auditory_perceptual_store_ptr;
	Cognitive_processor * cognitive_ptr;
	Manual_processor * manual_ptr;
	Ocular_processor * ocular_ptr;
	Invol_ocular_processor * invol_ocular_ptr;
	Vocal_processor * vocal_ptr;
	Temporal_processor * temporal_ptr;
	LTM_processor * ltm_ptr;
	Task_processor * task_ptr;
	
	bool trace_all;
	bool trace_visual;
	bool trace_auditory;
	bool trace_cognitive;
	bool trace_manual;
	bool trace_ocular;
	bool trace_vocal;
	bool trace_temporal;

	// disallow copy, assignment
	Human_processor(const Human_processor&);
	Human_processor& operator= (const Human_processor&);

};

#endif
