#include "Device_base.h"
#include "Device_processor.h"
#include "Coordinator.h"
#include "Human_base.h"
#include "Speech_word.h"
//#include "Human_processor.h"
#include "Geometry.h"
#include "Symbol_Geometry_utilities.h"
#include "Output_tee_globals.h"
#include "Epic_standard_symbols.h"

#include <iostream>
#include <cassert>
#include <string>
#include <sstream>

using namespace std;
namespace GU =  Geometry_Utilities;


Device_base::Device_base(const string& id, Output_tee& ot) :
	device_out(ot), device_name(id), device_proc_ptr(0)
{
	// create a Device_processor and connect this device to it. 
	// Processor() automatically adds itself to the simulation
	device_proc_ptr = new Device_processor(device_name + "_interface");
	device_proc_ptr->connect(this);
}

Device_base::~Device_base()
{
	// ~Processor automatically removes itself from the simulation
	delete device_proc_ptr;
	device_proc_ptr = 0;
}

void Device_base::initialize()
{
	// make sure we are connected to the rest of the system
	Assert(device_proc_ptr);
}


// These dummy functions should be overridden to obtain more complex behavior.

void Device_base::handle_Start_event()
{
}

void Device_base::handle_Stop_event()
{
}

void Device_base::handle_Report_event(long)
{
}

void Device_base::handle_Delay_event(const Symbol& type, const Symbol& datum, 
		const Symbol& object_name, const Symbol& property_name, const Symbol& property_value)
{
}

void Device_base::handle_Keystroke_event(const Symbol& key_name)
{
}

void Device_base::handle_Type_In_event(const Symbol& type_in_string)
{
}

void Device_base::handle_Hold_event(const Symbol& button_name)
{
}

void Device_base::handle_Release_event(const Symbol& button_name)
{
}

void Device_base::handle_Click_event(const Symbol& button_name)
{
}

void Device_base::handle_Double_Click_event(const Symbol& button_name)
{
}

void Device_base::handle_Point_event(const Symbol& target_name)
{
}

void Device_base::handle_Ply_event(const Symbol& cursor_name, const Symbol& target_name,
		GU::Point new_location, GU::Polar_vector movement_vector)
{
}

void Device_base::handle_Vocal_event(const Symbol& vocal_input)
{
}

void Device_base::handle_Vocal_event(const Symbol& vocal_input, long duration)
{
}

void Device_base::handle_VisualFocusChange_event(const Symbol& object_name)
{
}

void Device_base::handle_Eyemovement_Start_event(const Symbol& target_name, GU::Point new_location)
{
}

void Device_base::handle_Eyemovement_End_event(const Symbol& target_name, GU::Point new_location)
{
}

void Device_base::handle_HLGet_event(const Symbol_list_t& props, const Symbol_list_t& values, const Symbol& tag)
{
	// default response is to simply echo the information back with a dummy name
	make_high_level_input_appear(Symbol("HLDummyObject"), props, values, tag);
}

void Device_base::handle_HLPut_event(const Symbol_list_t& props, const Symbol_list_t& values)
{
}

/* Services for derived and related classes */

string Device_base::processor_info() const
{
	ostringstream oss;
	oss << get_time() << ' ' << device_name << ':';
	return oss.str();
}

long Device_base::get_time() const
{
	return Coordinator::get_instance().get_time();
}
	
bool Device_base::get_trace() const
{
	return device_proc_ptr->get_trace();
}

void Device_base::set_trace(bool trace_)
{
	return device_proc_ptr->set_trace(trace_);
}

	
// Tell the simulated human we have a new visual object with unspecified location and size
void Device_base::make_visual_object_appear(const Symbol& object_name)
{
	device_proc_ptr->make_visual_object_appear(object_name);
}

// Tell the simulated human we have a new visual object with specified location and size
void Device_base::make_visual_object_appear(const Symbol& object_name, 
	Geometry_Utilities::Point location, Geometry_Utilities::Size size)
{
	device_proc_ptr->make_visual_object_appear(object_name, location, size);
}

void Device_base::set_visual_object_location(const Symbol& object_name, GU::Point location)
{
	device_proc_ptr->set_visual_object_location(object_name, location);
}

void Device_base::set_visual_object_size(const Symbol& object_name, GU::Size size)
{
	device_proc_ptr->set_visual_object_size(object_name, size);
}


// Tell the simulated human we have a value for a property of a visual object
void Device_base::set_visual_object_property(const Symbol& object_name, 
	const Symbol& property_name, const Symbol& property_value)
{
	Assert(object_name != Nil_c);
	device_proc_ptr->set_visual_object_property(object_name, property_name, property_value);
}

// Tell the simulated human that a visual object is gone
void Device_base::make_visual_object_disappear(const Symbol& object_name)
{
	device_proc_ptr->make_visual_object_disappear(object_name);
}


// The the simulated human about the following events:
/*
// A new auditory stream with location and size
void Device_base::create_auditory_stream(const Symbol& name, GU::Point location, GU::Size size)
{
	device_proc_ptr->create_auditory_stream(name, location, size);
}

// An auditory stream is gone
void Device_base::destroy_auditory_stream(const Symbol& name)
{
	device_proc_ptr->destroy_auditory_stream(name);
}

*/
// The location of an auditory stream has changed
void Device_base::set_auditory_stream_location(const Symbol& name, GU::Point location)
{
	device_proc_ptr->set_auditory_stream_location(name, location);
}

// The size of an auditory stream has changed
void Device_base::set_auditory_stream_size(const Symbol& name, GU::Size size)
{
	device_proc_ptr->set_auditory_stream_size(name, size);
}

// A property of an auditory stream has changed
void Device_base::set_auditory_stream_property(const Symbol& name, const Symbol& propname, const Symbol& propvalue)
{
	device_proc_ptr->set_auditory_stream_property(name, propname, propvalue);
}

// An auditory event with a "message" as a simple signal
void Device_base::make_auditory_event(const Symbol& message)
{
	device_proc_ptr->make_auditory_event(message);
}

// An auditory event with location, timbre, stream, duration, and optional intrinsic duration
void Device_base::make_auditory_sound_event(const Symbol& name, const Symbol& stream, GU::Point location, 
		const Symbol& timbre, double loudness, long duration, long intrinsic_duration)
{
	device_proc_ptr->make_auditory_sound_event(name, stream, location, timbre, loudness, duration, intrinsic_duration);
}

// A new auditory sound with location, timbre, stream, and instrinsic duration has started
void Device_base::make_auditory_sound_start(const Symbol& name, const Symbol& stream, GU::Point location,
		const Symbol& timbre, double loudness, long intrinsic_duration)
{
	device_proc_ptr->make_auditory_sound_start(name, stream, location, timbre, loudness, intrinsic_duration);
}

// The auditory sound has stopped
void Device_base::make_auditory_sound_stop(const Symbol& name)
{
	device_proc_ptr->make_auditory_sound_stop(name);
}

// A property of the auditory sound has changed
void Device_base::set_auditory_sound_property(const Symbol& name, const Symbol& propname, const Symbol& propvalue)
{
	device_proc_ptr->set_auditory_sound_property(name, propname, propvalue);
}

// A speech event with stream, content (utterance), and duration
void Device_base::make_auditory_speech_event(const Speech_word& word)
{
	device_proc_ptr->make_auditory_speech_event(word);
}

/*
void Device_base::make_auditory_speech_event(const Symbol& sound_name, const Symbol& stream_name, 
		const Symbol& content, const Symbol& speaker_gender, const Symbol& speaker_id, double loudness, long duration)
{
	device_proc_ptr->make_auditory_speech_event(sound_name, stream_name, content, speaker_gender, speaker_id, loudness, duration);
}
*/

// Tell the simulated human that a High-Level input object with properties and values should be
// stored under a WM tag after a specified time delay from now
void Device_base::make_high_level_input_appear(const Symbol& object_name, const Symbol_list_t& props, const Symbol_list_t& values, const Symbol& tag)
{
	device_proc_ptr->make_high_level_input_appear(object_name, props, values, tag);
}

// Tell the simulated human that a High-Level input object is gone
// A high-level input disappears after a specified delay
void Device_base::make_high_level_input_disappear(const Symbol& object_name)
{
	device_proc_ptr->make_high_level_input_disappear(object_name);
}


// send a Device_Delay_event to self
void Device_base::schedule_delay_event(long delay)
{	
	device_proc_ptr->schedule_delay_event(delay);
}

void Device_base::schedule_delay_event(long delay, const Symbol& delay_type, const Symbol& delay_datum)
{	
	device_proc_ptr->schedule_delay_event(delay, delay_type, delay_datum);
}

void Device_base::schedule_delay_event(long delay, const Symbol& delay_type, 
		const Symbol& object_name, const Symbol& property_name, const Symbol& property_value)
{			
	device_proc_ptr->schedule_delay_event(delay, delay_type, object_name, property_name, property_value);	
}

// Tell the simulated human that a report with the specified duration must be made
// this is done visually
void Device_base::make_report(long time, long duration)
{
	device_proc_ptr->make_report(time, duration);
}

// Allow the device to set a human parameter - for generating parametric runs
void Device_base::set_human_parameter(const std::string& proc_name, const std::string& param_name, const std::string& spec)
{
	device_proc_ptr->set_human_parameter(proc_name, param_name, spec);
}

// Allow the device to access the human's production rule set filename for documentation purposes
string Device_base::get_human_prs_filename() const
{
	return device_proc_ptr->get_human_prs_filename();
}

// service for descendant classes to use to halt simulation.
void Device_base::stop_simulation()
{
	device_proc_ptr->do_stop_simulation();
}

