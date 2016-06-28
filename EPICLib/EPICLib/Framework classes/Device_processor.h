/* 
This is a Object Adapter class that adapts the pointed-to Device_base object into the
Processor event interface. The events are accepted by this class, which then forwards them
as function calls to the Device_base object. Thus this class provides an interface to the 
Device_base from the GLEAN event system.

It accepts events which are forwarded to the Device_base object, and has functions that 
the Device_base class calls to create either Device_delay events or events for the simulated human. 
This class is supposed to be a leaf class only. 

After creation, this processor must be connected to a Device_base object, checked for by
initialize(), and optionally to a Human processor, checked at function-call time. 
Because devices should be unique in the domain, copy and assignment are forbidden.
*/

#ifndef DEVICE_PROCESSOR_H
#define DEVICE_PROCESSOR_H

#include "Processor.h"
#include "Device_event_types.h"
#include "Geometry.h"
#include <string>

class Human_processor;
class Human_base;
class Device_base;
struct Speech_word;

class Device_processor : public Processor {
public:
	// This constructor requires that Device_base device and Human_processor must be created first
	Device_processor(const std::string& id);
	
	virtual void initialize();
	virtual void display() const;
	virtual std::string processor_info() const;
	virtual void set_parameter_string(const std::string&);
	virtual std::string get_parameter_string() const;
	
	void connect(Device_base * device_interface_ptr_);
	Device_base * get_device_ptr() const
		{return device_interface_ptr;}
	// the device interacts with at most one human
	void connect(Human_base * human_interface_ptr_);
	Human_base * get_human_ptr() const
		{return human_interface_ptr;}
	
	// event acceptance -- forwarded to device implementation
	virtual void accept_event(const Start_event *); 	// override Processor definition
	virtual void accept_event(const Stop_event *);
	virtual void accept_event(const Device_event *);

	// event handling
	virtual void handle_event(const Device_Delay_event *);
	virtual void handle_event(const Device_Keystroke_event *);
	virtual void handle_event(const Device_Type_In_event *);
	virtual void handle_event(const Device_Hold_event *);
	virtual void handle_event(const Device_Release_event *);
	virtual void handle_event(const Device_Click_event *);
	virtual void handle_event(const Device_Double_Click_event *);
	virtual void handle_event(const Device_Ply_event *);
	virtual void handle_event(const Device_Point_event *);
	virtual void handle_event(const Device_Vocal_event *);
	virtual void handle_event(const Device_VisualFocusChange_event *);
	virtual void handle_event(const Device_Eyemovement_Start_event *);
	virtual void handle_event(const Device_Eyemovement_End_event *);
	virtual void handle_event(const Device_HLGet_event *);
	virtual void handle_event(const Device_HLPut_event *);

	// functions for creating device delay events to arrive at this processor after a delay
	void schedule_delay_event(long delay);
	void schedule_delay_event(long delay, const Symbol& delay_type, const Symbol& delay_datum);
	void schedule_delay_event(long delay, const Symbol& delay_type, 
		const Symbol& object_name, const Symbol& property_name, const Symbol& property_value);
		
	// functions for creating inputs to the simulated human
	// inputs - Tells the simulated human about a sensory event
	// A new visual object with unspecified location and size
	void make_visual_object_appear(const Symbol& obj_name);
	// A new visual object with specified location and size
	void make_visual_object_appear(const Symbol& obj_name, GU::Point location, GU::Size size);
	// A visual object is gone
	void make_visual_object_disappear(const Symbol& obj_name);
	// The location of a visual object has changed
	void set_visual_object_location(const Symbol& obj_name, GU::Point location);
	// The size of a visual object has changed
	void set_visual_object_size(const Symbol& obj_name, GU::Size size);
	// A property of a visual object has changed
	void set_visual_object_property(const Symbol& obj_name, const Symbol& propname, const Symbol& propvalue);

/*	// A new auditory stream with location and size
	void create_auditory_stream(const Symbol& name, GU::Point location, GU::Size size = GU::Size());
	// An auditory stream is gone
	void destroy_auditory_stream(const Symbol& name);
*/
	// The location of an auditory stream has changed
	void set_auditory_stream_location(const Symbol& name, GU::Point location);
	// The size of an auditory stream has changed
	void set_auditory_stream_size(const Symbol& name, GU::Size size);
	// A property of an auditory stream has changed
	void set_auditory_stream_property(const Symbol& name, const Symbol& propname, const Symbol& propvalue);
	
	// An auditory event with a "message" as a simple signal
	void make_auditory_event(const Symbol& message);
	// An auditory event with location, timbre, stream, duration, and optional intrinsic duration
	void make_auditory_sound_event(const Symbol& name, const Symbol& stream, GU::Point location, const Symbol& timbre, double loudness, long duration, long intrinsic_duration = 0);
	// A new auditory sound with location, timbre, stream, and instrinsic duration has started
	void make_auditory_sound_start(const Symbol& name, const Symbol& stream, GU::Point location, const Symbol& timbre, double loudness, long intrinsic_duration);
	// The auditory sound has stopped
	void make_auditory_sound_stop(const Symbol& name);
	// A property of the auditory sound has changed
	void set_auditory_sound_property(const Symbol& name, const Symbol& propname, const Symbol& propvalue);
//	sound_name, stream_name - to allow finesse of sound/stream object identity
//	content - finessed content of the speech object - e.g. recognized word
//	speaker_gender, speaker_id - surrogates/finesse of e.g. formant structure that distinguishes one speaker from another
//	loudness - in db - can be a cue to stream identification
//	duration - of this speech object
//	void make_auditory_speech_event(const Symbol& sound_name, const Symbol& stream_name, 
//		const Symbol& content, const Symbol& speaker_gender, const Symbol& speaker_id, double loudness, long duration);
	void make_auditory_speech_event(const Speech_word& word);
	
	// A high-level input event
	void make_high_level_input_appear(const Symbol& object_name, const Symbol_list_t& props, const Symbol_list_t& values, const Symbol& tag);
	// A high-level input disappears
	void make_high_level_input_disappear(const Symbol& object_name);
	
	// Make a report
	void make_report(long time, long duration);
	
	// Allow the device to set a human parameter - for generating parametric runs
	void set_human_parameter(const std::string& proc_name, const std::string& param_name, const std::string& spec);

	// Allow the device to access the human's production rule set filename for documentation purposes
	std::string get_human_prs_filename() const;
	
	// stop the simulation - separate function to allow trace message
	void do_stop_simulation();

private:
	Device_base * device_interface_ptr;				// the Device object
	Human_base * human_interface_ptr;				// destination for visual and auditory output
	
	Device_processor(const Device_processor&);
	Device_processor& operator= (const Device_processor&);
};

// provide a definition for this function to create a specific subclass of device
Device_base * create_specific_device();

#endif
