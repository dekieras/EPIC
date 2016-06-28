#ifndef HUMAN_BASE_H
#define HUMAN_BASE_H

#include "Symbol.h"
#include "Geometry.h"
namespace GU = Geometry_Utilities;

struct Speech_word;

// This class defines an interface to a Human in terms of functions to be called to supply input to the
// human. Cf. Device_base which provides a similar functional interface for input to the device, along
// with functions to call to create inputs to the human, which call these functions.

class Human_base  {
public:
	// inputs - Tells the simulated human about a sensory event
	// A new visual object with unspecified location and size
	virtual void make_visual_object_appear(const Symbol& obj_name) {}
	// A new visual object with specified location and size
	virtual void make_visual_object_appear(const Symbol& obj_name, GU::Point location, GU::Size size) {}
	// A visual object is gone
	virtual void make_visual_object_disappear(const Symbol& obj_name) {}
	// The location of a visual object has changed
	virtual void set_visual_object_location(const Symbol& obj_name, GU::Point location) {}
	// The size of a visual object has changed
	virtual void set_visual_object_size(const Symbol& obj_name, GU::Size size) {}
	// A property of a visual object has changed
	virtual void set_visual_object_property(const Symbol& obj_name, const Symbol& propname, const Symbol& propvalue) {}

	// A new auditory stream with location and size
	virtual void create_auditory_stream(const Symbol& name, GU::Point location, GU::Size size = GU::Size()) {}
	// An auditory stream is gone
	virtual void destroy_auditory_stream(const Symbol& name) {}
	// The location of an auditory stream has changed
	virtual void set_auditory_stream_location(const Symbol& name, GU::Point location) {}
	// The size of an auditory stream has changed
	virtual void set_auditory_stream_size(const Symbol& name, GU::Size size) {}
	// A property of an auditory stream has changed
	virtual void set_auditory_stream_property(const Symbol& name, const Symbol& propname, const Symbol& propvalue) {}
	
	// An auditory event with a "message" as a simple signal
	virtual void make_auditory_event(const Symbol& message) {}
	// An auditory event with location, timbre, stream, duration, and optional intrinsic duration
	virtual void make_auditory_sound_event(const Symbol& name, const Symbol& stream, GU::Point location, const Symbol& timbre, double loudness, long duration, long intrinsic_duration = 0) {}
	// A new auditory sound with location, timbre, stream, and instrinsic duration has started
	virtual void make_auditory_sound_start(const Symbol& name, const Symbol& stream, GU::Point location, const Symbol& timbre, double loudness, long intrinsic_duration) {}
	// The auditory sound has stopped
	virtual void make_auditory_sound_stop(const Symbol& name) {}
	// A property of the auditory sound has changed
	virtual void set_auditory_sound_property(const Symbol& name, const Symbol& propname, const Symbol& propvalue) {}
	// A speech event with stream, content (utterance), and duration
//	virtual void make_auditory_speech_event(const Symbol& sound_name, const Symbol& stream_name, 
//		const Symbol& content, const Symbol& speaker_gender, const Symbol& speaker_id, double loudness, long duration) {}
	virtual void make_auditory_speech_event(const Speech_word& word) {}
	
	// A high-level input event
	virtual void make_high_level_input_appear(const Symbol& object_name, const Symbol_list_t& props, const Symbol_list_t& values, const Symbol& tag) {}
	// A high-level input disappears
	virtual void make_high_level_input_disappear(const Symbol& object_name) {}
	
	// Make a report
	virtual void make_report(long duration) {};

	// Allow a client to set a human parameter - e.g. for generating parametric runs
	virtual void set_parameter(const std::string& proc_name, const std::string& param_name, const std::string& spec) {}

	// Allow the device to access the human's production rule set filename for documentation purposes
	virtual std::string get_prs_filename() const {return std::string();}

	
protected:
	Human_base() {}		// this class serves only as a base for derived classes
	virtual ~Human_base() {}
};

#endif
