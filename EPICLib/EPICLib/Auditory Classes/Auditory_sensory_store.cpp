#include "Auditory_sensory_store.h"
#include "Output_tee_globals.h"
#include "Auditory_perceptual_processor.h"
#include "Symbol_utilities.h"
#include "View_base.h"

#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <typeinfo>


using std::cout;	using std::endl;
//using std::string;
//using std::ostringstream;
using std::list;


/*** Auditory_sensory_store ***/

void Auditory_sensory_store::accept_event(const Auditory_event * event_ptr)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << typeid(*event_ptr).name() << " received" << endl;
	broadcast_to_recorders(event_ptr);
	// tell the event to handle itself with itself!
	event_ptr->handle_self(this);
}

// accept each event, update the local store generically, and then call the perceptual processor if a change was made (if necessary to test)
void Auditory_sensory_store::handle_event(const Auditory_Stream_Create_event * event_ptr)
{
	Auditory_store::handle_event(event_ptr);
	get_human_ptr()->get_Auditory_perceptual_processor_ptr()->
		create_stream(event_ptr->stream_name, event_ptr->pitch, event_ptr->loudness, event_ptr->location);
}

void Auditory_sensory_store::handle_event(const Auditory_Stream_Destroy_event * event_ptr)
{
	// do the default activities - nothing happens immediately
	Auditory_store::handle_event(event_ptr);
	// schedule an erase event for when this stream should disappear from the store
	schedule_event(new Auditory_Stream_Erase_event(get_time() + disappearance_decay_delay.get_long_value(),
		this, event_ptr->stream_name));

}

void Auditory_sensory_store::handle_event(const Auditory_Stream_Erase_event * event_ptr)
{
	// tell the down-stream processor it is going at this time
	get_human_ptr()->get_Auditory_perceptual_processor_ptr()->destroy_stream(event_ptr->stream_name);
	// now actually remove it from the store
	Auditory_store::handle_event(event_ptr);
}

void Auditory_sensory_store::handle_event(const Auditory_Stream_Set_Location_event * event_ptr)
{
	Auditory_store::handle_event(event_ptr);
	if(changed)
		get_human_ptr()->get_Auditory_perceptual_processor_ptr()->
			set_stream_location(event_ptr->stream_name, event_ptr->location);
}

void Auditory_sensory_store::handle_event(const Auditory_Stream_Set_Pitch_event * event_ptr)
{
	Auditory_store::handle_event(event_ptr);
	if(changed)
		get_human_ptr()->get_Auditory_perceptual_processor_ptr()->
			set_stream_pitch(event_ptr->stream_name, event_ptr->pitch);
}

void Auditory_sensory_store::handle_event(const Auditory_Stream_Set_Loudness_event * event_ptr)
{
	Auditory_store::handle_event(event_ptr);
	if(changed)
		get_human_ptr()->get_Auditory_perceptual_processor_ptr()->
			set_stream_loudness(event_ptr->stream_name, event_ptr->loudness);
}


void Auditory_sensory_store::handle_event(const Auditory_Stream_Set_Size_event * event_ptr)
{
	Auditory_store::handle_event(event_ptr);
	if(changed)
		get_human_ptr()->get_Auditory_perceptual_processor_ptr()->
			set_stream_size(event_ptr->stream_name, event_ptr->size);
}

void Auditory_sensory_store::handle_event(const Auditory_Stream_Set_Property_event * event_ptr)
{
	Auditory_store::handle_event(event_ptr);
	if(changed)
		get_human_ptr()->get_Auditory_perceptual_processor_ptr()->
			set_stream_property(event_ptr->stream_name, event_ptr->property_name, event_ptr->property_value);
}

// accept each event, update the local store generically, and then call the perceptual processor if a change was made (if necessary to test)
void Auditory_sensory_store::handle_event(const Auditory_Sound_Start_event * event_ptr)
{
	Auditory_store::handle_event(event_ptr);
	get_human_ptr()->get_Auditory_perceptual_processor_ptr()->
		make_new_sound_start(event_ptr->sound_name, event_ptr->stream_name, event_ptr->time_stamp, event_ptr->location, 
			event_ptr->timbre, event_ptr->loudness, event_ptr->intrinsic_duration);
}

// accept each event, update the local store generically, and then call the perceptual processor if a change was made (if necessary to test)
/*
void Auditory_sensory_store::handle_event(const Auditory_Speech_Start_event * event_ptr)
{
	Auditory_store::handle_event(event_ptr);
	get_human_ptr()->get_Auditory_perceptual_processor_ptr()->
		make_new_speech_start(event_ptr->sound_name, event_ptr->stream_name, event_ptr->time_stamp, 
			event_ptr->content, event_ptr->speaker_gender, event_ptr->speaker_id, 
			event_ptr->loudness, event_ptr->duration);
}
*/

void Auditory_sensory_store::handle_event(const Auditory_Speech_Start_event * event_ptr)
{
	Auditory_store::handle_event(event_ptr);
	get_human_ptr()->get_Auditory_perceptual_processor_ptr()->
		make_new_speech_start(event_ptr->word);
}

void Auditory_sensory_store::handle_event(const Auditory_Sound_Stop_event * event_ptr)
{
	// do the default activities
	Auditory_store::handle_event(event_ptr);
	// schedule an erase event for when this sound should disappear
	schedule_event(new Auditory_Sound_Erase_event(get_time() + disappearance_decay_delay.get_long_value(),
		get_human_ptr()->get_auditory_sensory_store_ptr(), event_ptr->sound_name));

}

void Auditory_sensory_store::handle_event(const Auditory_Sound_Erase_event * event_ptr)
{
	// tell the down-stream processor it is going first
	get_human_ptr()->get_Auditory_perceptual_processor_ptr()->make_sound_stop(event_ptr->sound_name);
	Auditory_store::handle_event(event_ptr);
}

void Auditory_sensory_store::handle_event(const Auditory_Sound_Set_Property_event * event_ptr)
{
	Auditory_store::handle_event(event_ptr);
	if(changed)
		get_human_ptr()->get_Auditory_perceptual_processor_ptr()->
			set_sound_property(event_ptr->sound_name, event_ptr->property_name, event_ptr->property_value);
}

