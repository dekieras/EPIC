/*
This is the .cpp file for the Device_processor adapter. It simply forwards
the calls to the Device_base object.
*/

#include "Device_processor.h"
#include "Device_base.h"
#include "Human_base.h"
#include "Speech_word.h"
#include "Assert_throw.h"

#include "Output_tee_globals.h"
#include "Epic_standard_symbols.h"

#include <iostream>

using namespace std;

Device_processor::Device_processor(const std::string& id) :
	Processor(id), device_interface_ptr(0), human_interface_ptr(0)
{
}

void Device_processor::initialize()
{
	Assert(device_interface_ptr);
	if(Trace_out && get_trace())
		Trace_out << processor_info() << " Initialize" << endl;
	device_interface_ptr->initialize();
}

void Device_processor::display() const
{
	Assert(device_interface_ptr);
	device_interface_ptr->display();
}

string Device_processor::processor_info() const
{
	Assert(device_interface_ptr);
	return(device_interface_ptr->processor_info());
}

void Device_processor::set_parameter_string(const std::string& str)
{
	Assert(device_interface_ptr);
	device_interface_ptr->set_parameter_string(str);
}

std::string Device_processor::get_parameter_string() const
{
	Assert(device_interface_ptr);
	return device_interface_ptr->get_parameter_string();
}

void Device_processor::connect(Device_base * device_interface_ptr_)
{
	Assert(device_interface_ptr_);
	device_interface_ptr = device_interface_ptr_;
}

void Device_processor::connect(Human_base * human_interface_ptr_)
{
	Assert(human_interface_ptr_);
	human_interface_ptr = human_interface_ptr_;
}


// These forward the Event system calls to functions defined in the Device_base base class
void Device_processor::accept_event(const Start_event *)
{
	Assert(device_interface_ptr);
	if(Trace_out && get_trace())
		Trace_out << processor_info() << " Start event" << endl;
	device_interface_ptr->handle_Start_event();
}

void Device_processor::accept_event(const Stop_event *)
{
	Assert(device_interface_ptr);
	if(Trace_out && get_trace())
		Trace_out << processor_info() << " Stop event" << endl;
	device_interface_ptr->handle_Stop_event();
}	

void Device_processor::accept_event(const Device_event * event_ptr)
{
	// tell the event to handle itself with yourself!
	event_ptr->handle_self(this);
}

void Device_processor::handle_event(const Device_Delay_event * event_ptr)
{
	Assert(device_interface_ptr);
	if(Trace_out && get_trace())
		Trace_out << processor_info() << " Delay " << event_ptr->type << ' ' << event_ptr->datum << ' ' 
		<< event_ptr->object_name << ' ' << event_ptr->property_name << ' ' << event_ptr->property_value << endl;
	device_interface_ptr->handle_Delay_event(event_ptr->type, event_ptr->datum, event_ptr->object_name, event_ptr->property_name, event_ptr->property_value);
}

void Device_processor::handle_event(const Device_Keystroke_event * event_ptr)
{
	Assert(device_interface_ptr);
	if(Trace_out && get_trace())
		Trace_out << processor_info() << " Keystroke: " << event_ptr->key_name << endl;
	device_interface_ptr->handle_Keystroke_event(event_ptr->key_name);
}

void Device_processor::handle_event(const Device_Type_In_event * event_ptr)
{
	Assert(device_interface_ptr);
	if(Trace_out && get_trace())
		Trace_out << processor_info() << " Type_In: " << event_ptr->type_in_string << endl;
	device_interface_ptr->handle_Type_In_event(event_ptr->type_in_string);
}

void Device_processor::handle_event(const Device_Hold_event * event_ptr)
{
	Assert(device_interface_ptr);
	if(Trace_out && get_trace())
		Trace_out << processor_info() << " Hold: " << event_ptr->button_name << endl;
	device_interface_ptr->handle_Hold_event(event_ptr->button_name);
}

void Device_processor::handle_event(const Device_Release_event * event_ptr)
{
	Assert(device_interface_ptr);
	if(Trace_out && get_trace())
		Trace_out << processor_info() << " Release: " << event_ptr->button_name << endl;
	device_interface_ptr->handle_Release_event(event_ptr->button_name);
}

void Device_processor::handle_event(const Device_Click_event * event_ptr)
{
	Assert(device_interface_ptr);
	if(Trace_out && get_trace())
		Trace_out << processor_info() << " Click: " << event_ptr->button_name << endl;
	device_interface_ptr->handle_Click_event(event_ptr->button_name);
}

void Device_processor::handle_event(const Device_Double_Click_event * event_ptr)
{
	Assert(device_interface_ptr);
	if(Trace_out && get_trace())
		Trace_out << processor_info() << " Double_Click: " << event_ptr->button_name << endl;
	device_interface_ptr->handle_Double_Click_event(event_ptr->button_name);
}

void Device_processor::handle_event(const Device_Point_event * event_ptr)
{
	Assert(device_interface_ptr);
	if(Trace_out && get_trace())
		Trace_out << processor_info() << " Point_to: " << event_ptr->target_name << endl;
	device_interface_ptr->handle_Point_event(event_ptr->target_name);
}

void Device_processor::handle_event(const Device_Ply_event * event_ptr)
{
	Assert(device_interface_ptr);
	if(Trace_out && get_trace())
		Trace_out << processor_info() << " Ply: " << event_ptr->cursor_name << " to " << event_ptr->target_name 
		<< " at " << event_ptr->new_location << " along " << event_ptr->movement_vector << endl;
	device_interface_ptr->handle_Ply_event(event_ptr->cursor_name, event_ptr->target_name, event_ptr->new_location, event_ptr->movement_vector);
}

void Device_processor::handle_event(const Device_Vocal_event * event_ptr)
{
	Assert(device_interface_ptr);
	if(Trace_out && get_trace())
		Trace_out << processor_info() << " Vocal: " << event_ptr->vocal_input << endl;
	device_interface_ptr->handle_Vocal_event(event_ptr->vocal_input);
}

void Device_processor::handle_event(const Device_VisualFocusChange_event * event_ptr)
{
	Assert(device_interface_ptr);
	if(Trace_out && get_trace())
		Trace_out << processor_info() << " VisualFocusChanged to: " << event_ptr->object_name << endl;
	device_interface_ptr->handle_VisualFocusChange_event(event_ptr->object_name);
}

void Device_processor::handle_event(const Device_Eyemovement_Start_event * event_ptr)
{
	Assert(device_interface_ptr);
	if(Trace_out && get_trace())
		Trace_out << processor_info() << " Eyemovement started to: " << event_ptr->target_name << ' ' << event_ptr->new_location << endl;
	device_interface_ptr->handle_Eyemovement_Start_event(event_ptr->target_name, event_ptr->new_location);
}


void Device_processor::handle_event(const Device_Eyemovement_End_event * event_ptr)
{
	Assert(device_interface_ptr);
	if(Trace_out && get_trace())
		Trace_out << processor_info() << " Eyemovement ended at: " << event_ptr->target_name << ' ' << event_ptr->new_location << endl;
	device_interface_ptr->handle_Eyemovement_End_event(event_ptr->target_name, event_ptr->new_location);
}


void Device_processor::handle_event(const Device_HLGet_event * event_ptr)
{
	Assert(device_interface_ptr);
	if(Trace_out && get_trace()) {
		Trace_out << processor_info() << " High-level Get_from_device: ";
		Symbol_list_t::const_iterator pit = event_ptr->props.begin();
		Symbol_list_t::const_iterator vit = event_ptr->values.begin();
		for(;pit != event_ptr->props.end() && vit != event_ptr->values.end(); ++pit, ++vit)
			Trace_out << *pit << " is " << *vit << ", ";
		Trace_out << "and_store_under " << event_ptr->tag << endl;
		}
	device_interface_ptr->handle_HLGet_event(event_ptr->props, event_ptr->values, event_ptr->tag);	
}

void Device_processor::handle_event(const Device_HLPut_event * event_ptr)
{
	Assert(device_interface_ptr);
	if(Trace_out && get_trace()) {
		Trace_out << processor_info() << " High-level Put_to_device: ";
		Symbol_list_t::const_iterator pit = event_ptr->props.begin();
		Symbol_list_t::const_iterator vit = event_ptr->values.begin();
		for(;pit != event_ptr->props.end() && vit != event_ptr->values.end(); ++pit, ++vit) {
			if(pit != event_ptr->props.begin())
				Trace_out << ", ";
			Trace_out << *pit << " is " << *vit;
			}
		Trace_out << endl;
		}
	device_interface_ptr->handle_HLPut_event(event_ptr->props, event_ptr->values);	
}

// functions for creating device delay events to arrive at this processor after a delay
void Device_processor::schedule_delay_event(long delay)
{	
	if(Trace_out && get_trace()) 
		Trace_out << processor_info() << " Schedule delay of " << delay << endl;	
	Event * output_event_ptr = new Device_Delay_event (
		get_time() + delay, this);
	schedule_event(output_event_ptr);		
}

// send a Device_Delay_event to self
void Device_processor::schedule_delay_event(long delay, const Symbol& delay_type, const Symbol& delay_datum)
{	
	if(Trace_out && get_trace()) 
		Trace_out << processor_info() << " Schedule delay of " << delay << ' ' 
			<< delay_type << ' ' << delay_datum << endl;	
	Event * output_event_ptr = new Device_Delay_event (
		get_time() + delay, this, delay_type, delay_datum);
		
	schedule_event(output_event_ptr);		
}

void Device_processor::schedule_delay_event(long delay, const Symbol& delay_type, 
		const Symbol& object_name, const Symbol& property_name, const Symbol& property_value)
{	
	if(Trace_out && get_trace()) 
		Trace_out << processor_info() << " Schedule delay of " << delay << ' ' 
			<< delay_type << ' ' << object_name << ' ' << property_name << ' ' << property_value << endl;	
	Event * output_event_ptr = new Device_Delay_event (
		get_time() + delay, this, delay_type, object_name, property_name, property_value);
		
	schedule_event(output_event_ptr);		
}

// Tell the simulated human we have a new visual object with unspecified location and size
void Device_processor::make_visual_object_appear(const Symbol& object_name)
{
	if(Trace_out && get_trace()) 
		Trace_out << processor_info() << " Make appear: "<< object_name << endl;
	human_interface_ptr->make_visual_object_appear(object_name);
}

// Tell the simulated human we have a new visual object with specified location and size
void Device_processor::make_visual_object_appear(const Symbol& object_name, 
	Geometry_Utilities::Point location, Geometry_Utilities::Size size)
{
	if(Trace_out && get_trace()) 
		Trace_out << processor_info() << " Make appear: "
			<< object_name << " at " << location << " size " << size << endl;
	human_interface_ptr->make_visual_object_appear(object_name, location, size);
}

void Device_processor::set_visual_object_location(const Symbol& object_name, GU::Point location)
{
	if(Trace_out && get_trace()) 
		Trace_out << processor_info() << " Set location: "
			<< object_name << " to " << location << endl;
	human_interface_ptr->set_visual_object_location(object_name, location);
}

void Device_processor::set_visual_object_size(const Symbol& object_name, GU::Size size)
{
	if(Trace_out && get_trace()) 
		Trace_out << processor_info() << " Set size: "
			<< object_name << " to " << size << endl;
	human_interface_ptr->set_visual_object_size(object_name, size);
}

// Tell the simulated human we have a value for a property of a visual object
void Device_processor::set_visual_object_property(const Symbol& object_name, 
	const Symbol& property_name, const Symbol& property_value)
{
	if(Trace_out && get_trace())
		Trace_out << processor_info() << " Set: "
			<< object_name << ' ' << property_name << ' ' << property_value << endl;
	human_interface_ptr->set_visual_object_property(object_name, property_name, property_value);
}

// Tell the simulated human that a visual object is gone
void Device_processor::make_visual_object_disappear(const Symbol& object_name)
{
	if(Trace_out && get_trace()) 
		Trace_out << processor_info() << " Disappear: " << object_name << endl;	
	human_interface_ptr->make_visual_object_disappear(object_name);
}


/*
// A new auditory stream with location and size
void Device_processor::create_auditory_stream(const Symbol& name, GU::Point location, GU::Size size)
{
	if(Trace_out && get_trace()) 
		Trace_out << processor_info() << " Create stream: "
		<< name << ' ' << location << ' ' << size << endl;	
	human_interface_ptr->create_auditory_stream(name, location, size);
}

// An auditory stream is gone
void Device_processor::destroy_auditory_stream(const Symbol& name)
{
	if(Trace_out && get_trace()) 
		Trace_out << processor_info() << " Destroy stream: "
		<< name << endl;	
	human_interface_ptr->destroy_auditory_stream(name);
}
*/

// The location of an auditory stream has changed
void Device_processor::set_auditory_stream_location(const Symbol& name, GU::Point location)
{
	if(Trace_out && get_trace()) 
		Trace_out << processor_info() << " Set location: "
			<< name << " to " << location << endl;
	human_interface_ptr->set_auditory_stream_location(name, location);
}

// The size of an auditory stream has changed
void Device_processor::set_auditory_stream_size(const Symbol& name, GU::Size size)
{
	if(Trace_out && get_trace()) 
		Trace_out << processor_info() << " Set size: "
			<< name << " to " << size << endl;
	human_interface_ptr->set_auditory_stream_size(name, size);
}

// A property of an auditory stream has changed
void Device_processor::set_auditory_stream_property(const Symbol& name, const Symbol& propname, const Symbol& propvalue)
{
	if(Trace_out && get_trace()) 
		Trace_out << processor_info() << " Set: " 
			<< name << ' ' << propname << ' ' << propvalue << endl;	
	human_interface_ptr->set_auditory_stream_property(name, propname, propname);
}

// An auditory event with a "message" as a simple signal
void Device_processor::make_auditory_event(const Symbol& message)
{
	if(Trace_out && get_trace()) 
		Trace_out << processor_info() << " Sound event: " << message << endl;	
	human_interface_ptr->make_auditory_event(message);
}

// An auditory event with timbre, stream, duration, and optional intrinsic duration
void Device_processor::make_auditory_sound_event(const Symbol& name, const Symbol& stream, GU::Point location, const Symbol& timbre, double loudness, long duration, long intrinsic_duration)
{
	if(Trace_out && get_trace()) 
		Trace_out << processor_info() << " Sound event: "
			<< name << ' ' << stream << ' ' << location << ' ' << timbre << ' ' << loudness << ' ' << duration << ' ' << intrinsic_duration << endl;
	human_interface_ptr->make_auditory_sound_event(name, stream, location, timbre, loudness, duration, intrinsic_duration);
}

// A new auditory sound with timbre, stream, and instrinsic duration has started
void Device_processor::make_auditory_sound_start(const Symbol& name, const Symbol& stream, GU::Point location, const Symbol& timbre, double loudness, long intrinsic_duration)
{
	if(Trace_out && get_trace()) 
		Trace_out << processor_info() << " Sound start: "
			<< name << ' ' << stream << ' ' << location << ' ' << timbre << ' '  << ' ' << loudness << intrinsic_duration << endl;
	human_interface_ptr->make_auditory_sound_start(name, stream, location, timbre, loudness, intrinsic_duration);
}

// The auditory sound has stopped
void Device_processor::make_auditory_sound_stop(const Symbol& name)
{
	if(Trace_out && get_trace()) 
		Trace_out << processor_info() << " Sound stop: "
			<< name << endl;
	human_interface_ptr->make_auditory_sound_stop(name);
}

// A property of the auditory sound has changed
void Device_processor::set_auditory_sound_property(const Symbol& name, const Symbol& propname, const Symbol& propvalue)
{
	if(Trace_out && get_trace()) 
		Trace_out << processor_info() << " Set: " 
			<< name << ' ' << propname << ' ' << propvalue << endl;	
	human_interface_ptr->set_auditory_sound_property(name, propname, propvalue);
}

// A speech event with stream, content (utterance), and duration
/* void Device_processor::make_auditory_speech_event(const Symbol& sound_name, const Symbol& stream_name, 
		const Symbol& content, const Symbol& speaker_gender, const Symbol& speaker_id, double loudness, long duration)
{
	if(Trace_out && get_trace()) 
		Trace_out << processor_info() << " Speech sound event: " 
			<< sound_name << ' ' << stream_name << ' ' << content << ' ' << speaker_gender << ' ' << speaker_id << ' ' << loudness << ' ' << duration << endl;	
	human_interface_ptr->make_auditory_speech_event(sound_name, stream_name, content, speaker_gender, speaker_id, loudness, duration);
}
*/
void Device_processor::make_auditory_speech_event(const Speech_word& word)
{
	if(Trace_out && get_trace()) 
		Trace_out << processor_info() << " Speech sound event: " 
			<< word.name << ' ' << word.stream_name << ' ' << word.content << ' ' << word.speaker_gender << ' ' << word.speaker_id 
			<< ' ' << word.pitch << ' ' << word.loudness << ' ' << word.duration << endl;	
	human_interface_ptr->make_auditory_speech_event(word);
}



// Tell the simulated human that a High-Level input object with properties and values should be
// stored under a WM tag after a specified time delay from now

void Device_processor::make_high_level_input_appear(const Symbol& object_name, const Symbol_list_t& props, const Symbol_list_t& values, const Symbol& tag)
{
	if(Trace_out && get_trace()) {
		Trace_out << processor_info() << " Make HL input appear as " << object_name << ": ";
		Symbol_list_t::const_iterator pit = props.begin();
		Symbol_list_t::const_iterator vit = values.begin();
		for(;pit != props.end() && vit != values.end(); ++pit, ++vit)
			Trace_out << *pit << " is " << *vit;
		Trace_out << " and_store_under " << tag << endl;
		}
	human_interface_ptr->make_high_level_input_appear(object_name, props, values, tag);
}

// Tell the simulated human that a High-Level input object is gone
// A high-level input disappears after a specified delay
void Device_processor::make_high_level_input_disappear(const Symbol& object_name)
{
	if(Trace_out && get_trace()) {
		Trace_out << processor_info() << " Erase HL input of " << object_name << endl;
		}	
	human_interface_ptr->make_high_level_input_disappear(object_name);
}

// Tell the simulated human that a report with the specified duration must be made
// this is done visually
void Device_processor::make_report(long time, long duration)
{
	Assert(human_interface_ptr);
	if(Trace_out && get_trace()) 
		Trace_out << processor_info() << " Schedule report event at time: " << time << " with duration: " << duration << endl;	
	human_interface_ptr->make_report(duration);
}

// Allow the device to set a human parameter - for generating parametric runs
void Device_processor::set_human_parameter(const std::string& proc_name, const std::string& param_name, const std::string& spec)
{
	if(Trace_out && get_trace()) 
		Trace_out << processor_info() << " Set human parameter: "
			<< proc_name << ' ' << param_name << ' ' << spec << endl;
	human_interface_ptr->set_parameter(proc_name, param_name, spec);
}

// Allow the device to access the human's production rule set filename for documentation purposes
string Device_processor::get_human_prs_filename() const
{
	return human_interface_ptr->get_prs_filename();
}


void Device_processor::do_stop_simulation()
{
	if(Trace_out && get_trace()) 
		Trace_out << processor_info() << " Stop simulation" << endl;	
	stop_simulation();
}


