/* This serves as the base class for the device implementation that Device_processor forwards
events to in the form of simple function calls. This class structure is intended to insulate
the coding of a specific device implementation from the internal structure of simulation
system as much as possible. Thus this class does not inherit from any class defined
within the simulation system, and uses only a few utility classes from the simulation system.

This class provides some basic facilities for interacting with the simulated human. The default
behaviors defined in this implementation simply echo the supplied input and generate no output.
The output functions are provided as an inheritable facility. 
The object store is a problem here ...

Copy and assignment are forbidden.
*/

#ifndef DEVICE_BASE_H
#define DEVICE_BASE_H

#include "Symbol.h"
#include "Geometry.h"
#include "Output_tee.h"

#include <string>

class Device_processor;
struct Speech_word;

class Device_base {
public:
	// constructor with empty initial states
	Device_base(const std::string& id, Output_tee& ot);
	virtual ~Device_base();
	
	virtual void set_parameter_string(const std::string&) {}
	virtual std::string get_parameter_string() const {return std::string();}

	const std::string& get_name() const {return device_name;}
	virtual void initialize();
	virtual void display() const {}
	virtual std::string processor_info() const;	// override default to produce string containing output info
	
	// implementation device interacts with simulation through a Device_processor
//	void connect(Device_processor * device_proc_ptr_)
//		{device_proc_ptr = device_proc_ptr_;}
		
	Device_processor * get_device_proc_ptr() const
		{return device_proc_ptr;}
		
	// event handling interface called by Device_processor
	// these functions do nothing in this class; override only those necessary
	virtual void handle_Start_event();
	virtual void handle_Stop_event();
	virtual void handle_Report_event(long duration);
	virtual void handle_Delay_event(const Symbol& type, const Symbol& datum, 
		const Symbol& object_name, const Symbol& property_name, const Symbol& property_value);
	virtual void handle_Keystroke_event(const Symbol& key_name);
	virtual void handle_Type_In_event(const Symbol& type_in_string);
	virtual void handle_Hold_event(const Symbol& button_name);
	virtual void handle_Release_event(const Symbol& button_name);
	virtual void handle_Click_event(const Symbol& button_name);
	virtual void handle_Double_Click_event(const Symbol& button_name);
	virtual void handle_Point_event(const Symbol& target_name);
	virtual void handle_Ply_event(const Symbol& cursor_name, const Symbol& target_name,
		GU::Point new_location, GU::Polar_vector movement_vector);
	virtual void handle_Vocal_event(const Symbol& vocal_input);
	virtual void handle_Vocal_event(const Symbol& vocal_input, long duration);
	virtual void handle_VisualFocusChange_event(const Symbol& object_name);
	virtual void handle_Eyemovement_Start_event(const Symbol& target_name, GU::Point new_location);
	virtual void handle_Eyemovement_End_event(const Symbol& target_name, GU::Point new_location);
	virtual void handle_HLGet_event(const Symbol_list_t& props, const Symbol_list_t& values, const Symbol& tag);
	virtual void handle_HLPut_event(const Symbol_list_t& props, const Symbol_list_t& values);
	
	/* services for the derived Device_base class */
	
	// get the current time
	long get_time() const;
	// get the trace state of the associated Device_processor
	bool get_trace() const;
	// set the trace state of the associated Device_processor - to allow e.g. Dummy device to trace by default
	void set_trace(bool);
	
	// functions to manipulate the Device's display and output
	// Tell the simulated human we have a new visual object with unspecified location and size
	void make_visual_object_appear(const Symbol& object_name);
	// Tell the simulated human we have a new visual object with specified location and size
	void make_visual_object_appear(const Symbol& object_name, Geometry_Utilities::Point location,
		Geometry_Utilities::Size size);
	// Tell the simulated human that location of a visual object has changed
	void set_visual_object_location(const Symbol& object_name, GU::Point new_location);
	// Tell the simulated human that size of a visual object has changed
	void set_visual_object_size(const Symbol& object_name, GU::Size new_size);
	// Tell the simulated human we have a value for a property of a visual object
	void set_visual_object_property(const Symbol& object_name, 
		const Symbol& property_name, const Symbol& property_value);
	// the following assume that the property name is appropriate for the value being sent,
	// 		which is encoded identically in the Symbol for the value.	
	// Tell the simulated human we have a Point value for a property of a visual object
//	void set_visual_object_property(const Symbol& object_name, 
//		const Symbol& property_name, Geometry_Utilities::Point pv)
//		{set_visual_object_property(object_name, property_name, Symbol(pv.x, pv.y));}
	// Tell the simulated human we have a Size value for a property of a visual object
//	void set_visual_object_property(const Symbol& object_name, 
//		const Symbol& property_name, Geometry_Utilities::Size pv)
//		{set_visual_object_property(object_name, property_name, Symbol(pv.h, pv.v));}
	// Tell the simulated human we have two numerical values for a property of a visual object
//	void set_visual_object_property(const Symbol& object_name, 
//		const Symbol& property_name, double x, double y)
//		{set_visual_object_property(object_name, property_name, Symbol(x, y));}
	// Tell the simulated human that a visual object is gone
	void make_visual_object_disappear(const Symbol& object_name);

	// The the simulated human about the following events:
/*	// A new auditory stream with location and size
	void create_auditory_stream(const Symbol& name, GU::Point location, GU::Size size = GU::Size());
	// An auditory stream is gone
	void destroy_auditory_stream(const Symbol& name);
	// The location of an auditory stream has changed
*/
	void set_auditory_stream_location(const Symbol& name, GU::Point location);
	// The size of an auditory stream has changed
	void set_auditory_stream_size(const Symbol& name, GU::Size size);
	// A property of an auditory stream has changed
	void set_auditory_stream_property(const Symbol& name, const Symbol& propname, const Symbol& propvalue);

	// An auditory event with a "message" as a simple signal
	void make_auditory_event(const Symbol& message);
	// An auditory event with lcation, timbre, stream, duration, and optional intrinsic duration
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
	
	// Tell the simulated human that a High-Level input object with properties and values should be
	// stored under a WM tag
	void make_high_level_input_appear(const Symbol& object_name, 
		const Symbol_list_t& props, const Symbol_list_t& values, const Symbol& tag);
	// Tell the simulated human that a High-Level input object is gone
	void make_high_level_input_disappear(const Symbol& object_name);

	// Create a device delay event with the specified contents, to arrive at the current time + the delay.
	// The result is a call to the handle_Delay_event function at that time.
	void schedule_delay_event(long delay);
	void schedule_delay_event(long delay, const Symbol& delay_type, const Symbol& delay_datum);
	void schedule_delay_event(long delay, const Symbol& delay_type, 
		const Symbol& object_name, const Symbol& property_name, const Symbol& property_value);
	
	// Tell the simulated human that a report with the specified duration must be made
	void make_report(long time, long duration);

	// Allow the device to set a human parameter - for generating parametric runs
	void set_human_parameter(const std::string& proc_name, const std::string& param_name, const std::string& spec);

	// Allow the device to access the human's production rule set filename for documentation purposes
	std::string get_human_prs_filename() const;

	void stop_simulation();	// service for descendant classes to use to halt simulation.

	// protected for ease of use in derived classes - note that as reference it can't be modified after initialization
	Output_tee& device_out;

private:
	std::string device_name;
	Device_processor * device_proc_ptr; // the connected device processor interface to system
	
	Device_base(const Device_base&);
	Device_base& operator= (const Device_base&);
};

#endif

