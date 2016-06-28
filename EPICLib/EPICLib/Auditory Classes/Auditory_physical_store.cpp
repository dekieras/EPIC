#include "Auditory_physical_store.h"
#include "Speech_word.h"
#include "Output_tee_globals.h"
#include "Epic_standard_symbols.h"
#include "Epic_exception.h"
//#include "Human_processor.h"
//#include "Human_subprocessor.h"
//#include "Auditory_store_processor.h"
#include "Coordinator.h"
#include "Ear_processor.h"
#include "Name_map.h"
#include "Symbol_utilities.h"
#include "View_base.h"
//#include "Assert_throw.h"

#include <iostream>
//#include <string>
#include <sstream>
//#include <list>
#include <typeinfo>


using std::cout;	using std::endl;
using std::string;
using std::ostringstream;
//using std::list;


/*** Auditory_physical_store ***/

void Auditory_physical_store::initialize()
{
	Auditory_store::initialize();
}

void Auditory_physical_store::accept_event(const Auditory_event * event_ptr)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << typeid(*event_ptr).name() << " received" << endl;
	broadcast_to_recorders(event_ptr);
	// tell the event to handle itself with itself!
	event_ptr->handle_self(this);
}

void Auditory_physical_store::handle_event(const Auditory_Sound_Stop_event * event_ptr)
{
	Auditory_physical_store::make_sound_stop(event_ptr->sound_name);
}



/* Sound input functions */


// this is the override for physical store - new object from device has only physical name
// generate the psychological object name, and then pass it on with a next-link based on the stream name
void Auditory_physical_store::make_sound_start(const Symbol& physical_name, const Symbol& physical_stream_name,
    GU::Point location, const Symbol& timbre, double loudness, long intrinsic_duration)
{
	// create the new sound object - a continuing sound has duration 0
	long time_stamp = Coordinator::get_time();
	insert_new(Auditory_sound::create(physical_name, physical_stream_name, time_stamp, location, timbre, loudness, 0, intrinsic_duration));

	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "start sound " << physical_name << ' '
		<< physical_stream_name << ' ' << timbre << ' '  << intrinsic_duration << endl;

	notify_views(&View_base::notify_auditory_sound_start, physical_name, physical_stream_name, time_stamp, location, timbre, loudness);
	
	changed = true;
	// inform the next processor in terms of psychological name only
	get_human_ptr()->get_Ear_processor_ptr()->make_new_sound_start(physical_name, physical_stream_name, time_stamp, location, timbre, loudness, intrinsic_duration);
}


//void Auditory_physical_store::make_speech_start(const Symbol& physical_name, const Symbol& stream_name,  
//	const Symbol& content, const Symbol& speaker_gender, const Symbol& speaker_id, double loudness, long duration)
void Auditory_physical_store::make_speech_start(const Speech_word& physical_word)
{
	// create the new sound object - a continuing sound has duration 0; use duration of speech unit as the intrinsic_duration odf the sound
	long time_stamp = Coordinator::get_time();
    // we have to copy it, unfortunately
    Speech_word new_word = physical_word;
	new_word.time_stamp = time_stamp;
	insert_new(Auditory_sound::create(new_word));

	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "start speech sound: " << new_word.name << ' ' << new_word.stream_name << ' '
		<< new_word.content << ' ' << new_word.speaker_gender << ' ' << new_word.speaker_id << ' '
		<< new_word.pitch << ' ' << new_word.loudness << ' ' << new_word.duration << endl;
// ???TOFIX: this posts the physical word contents, but it does not have a valid time stamp
/*	notify_views(&View_base::notify_auditory_speech_start, physical_word.name, stream_physical_name, time_stamp, physical_word.content, 
		physical_word.speaker_gender, physical_word.speaker_id, physical_word.loudness, physical_word.pitch); */
	notify_views(&View_base::notify_auditory_speech_start, new_word);
	
	changed = true;
	get_human_ptr()->get_Ear_processor_ptr()->make_new_speech_start(new_word);
}

// this is the override for physical store - let the next processor know the sound is gone,
// then remove it
void Auditory_physical_store::make_sound_stop(const Symbol& physical_name)
{
	// call the base function to note the sound is stopping
	Auditory_store::make_sound_stop(physical_name);

	// inform the next processor

	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "Auditory_physical_store stop sound " << physical_name << endl;

	get_human_ptr()->get_Ear_processor_ptr()->make_sound_stop(physical_name);

	// now that next processor has had a chance to see it, remove the object
	erase_sound(physical_name);

}

// have the base class remove the objects, then remove the names
void Auditory_physical_store::erase_sound(const Symbol& physical_name)
{
	Auditory_store::erase_sound(physical_name);
//	name_map.remove_names_with_physical_name(physical_name); // let perceptual store delete the names
}


// This enables the device to "fire and forget" a sound that lasts a specific amount of time
// Create a sound object, but then schedule its removal at duration time from now.
void Auditory_physical_store::make_sound_event(const Symbol& physical_name, const Symbol& stream_name, GU::Point location,
			const Symbol& timbre, double loudness, long duration, long intrinsic_duration)
{
	// start the sound
	make_sound_start(physical_name, stream_name, location, timbre, loudness, intrinsic_duration);

	// make the sound stop at the end ofthe speech input
	schedule_event(new Auditory_Sound_Stop_event(get_time() + duration, this, physical_name));

}


// handle a speech event originated by Vocal_processor
void Auditory_physical_store::handle_event(const Auditory_Speech_event * event_ptr)
{
	// do the direct call to the function that does the work for externally-generated speech events
	// construct a Speech_word object from the event data
/*	Speech_word word;
	word.word_name = event_ptr->name;
	word.stream_name = event_ptr->stream_name;
	word.content = event_ptr->content;
	word.speaker_gender = event_ptr->speaker_gender;
	word.speaker_id = event_ptr->speaker_id;
	word.loudness = event_ptr->loudness;
	word.pitch = event_ptr->pitch;
	word.duration = event_ptr->duration;
*/	
//	make_speech_event(event_ptr->sound_name, event_ptr->stream_name, event_ptr->content,event_ptr->speaker_gender, event_ptr->speaker_id, event_ptr->loudness, event_ptr->duration);
	make_speech_event(event_ptr->word);
}


// This function allows other components to "fire & forget" long-duration speech input
// create an auditory object to represent the speech content,and schedule an event to remove it
// from the physical space at the end of the duration. The speech is reformulated as a sound
// with Timbre: Speech and Content:<utterance>.
//void Auditory_physical_store::make_speech_event(const Symbol& physical_name, const Symbol& stream_name, 
//		const Symbol& content, const Symbol& speaker_gender, const Symbol& speaker_id, double loudness, long duration)
void Auditory_physical_store::make_speech_event(const Speech_word& word)
{
	// the sound name here is the physical name
	make_speech_start(word);
	// timbre is Speech, the utterance becomes a Content property, intrinsic_duration is duration
//	make_speech_start(physical_name, stream_name, content, speaker_gender, speaker_id, loudness, duration);
//	set_sound_property(physical_name, Content_c, content);
//	set_sound_property(physical_name, Gender_c, speaker_gender);
//	set_sound_property(physical_name, Speaker_c, speaker_id);
	const Symbol& physical_name = word.name;
	
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << " speech event stop scheduled " << physical_name << endl;		
	
	// make the sound stop at the end of the speech input
	schedule_event(new Auditory_Sound_Stop_event(get_time() + word.duration, this, physical_name));

}


Symbol Auditory_physical_store::set_sound_property(const Symbol& physical_name, const Symbol& propname, const Symbol& propvalue)
{
	Symbol old_value = Auditory_store::set_sound_property(physical_name, propname, propvalue);
	if(changed) {
		// inform the next processor using the physical_name
		get_human_ptr()->get_Ear_processor_ptr()->set_sound_property(physical_name, propname, propvalue);
		}
	return old_value;
}
