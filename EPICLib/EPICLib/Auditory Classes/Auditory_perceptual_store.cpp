#include "Auditory_sensory_store.h"
#include "Auditory_perceptual_store.h"
#include "Ear_processor.h"
//#include "Auditory_physical_store.h"
#include "Output_tee_globals.h"
#include "Epic_standard_symbols.h"
#include "Symbol_utilities.h"
#include "View_base.h"
#include "Name_map.h"
#include "Cognitive_event_types.h"
#include "Cognitive_processor.h"
#include "Epic_exception.h"
#include "Assert_throw.h"

#include <iostream>
#include <string>
//#include <sstream>
#include <list>
#include <map>
#include <typeinfo>


using std::shared_ptr;
using std::cout;	using std::endl;
using std::string;	
//using std::ostringstream;
using std::list;
using std::map;
using std::dynamic_pointer_cast;


/*** Auditory_perceptual_sound ***/

/*
std::shared_ptr<Auditory_sound> Auditory_perceptual_sound::create(const Speech_word& word)
{
	{return std::shared_ptr<Auditory_sound>(new Auditory_perceptual_sound(word.name, word.stream_name, word.time_stamp, word.location, Speech_c, word.content, word.speaker_gender, word.speaker_id, word.utterance_id, word.loudness, word.pitch, 0, word.duration));}
}
*/

// access the list of disappearing properties
void Auditory_perceptual_sound::set_disappearing_time(const Symbol& property_name, long disappearing_time)
{
	unsupported_properties[property_name] = disappearing_time;
}

void Auditory_perceptual_sound::remove_disappearing_time(const Symbol& property_name)
{
	unsupported_properties.erase(property_name);
}

// returns zero if not present - 
long Auditory_perceptual_sound::get_disappearing_time(const Symbol& property_name) const
{
	map<Symbol, long>::const_iterator it;
	it = unsupported_properties.find(property_name);
	if(it == unsupported_properties.end())
		return 0;	
	return it->second;
}


void Auditory_perceptual_sound::display_contents(Output_tee& ot) const
{
	ot << "Auditory_perceptual_sound ";
	Auditory_sound::display_contents(ot);
}

shared_ptr<Auditory_perceptual_sound> Auditory_perceptual_store::get_perceptual_object_ptr(const Symbol& sound_name)
{
    shared_ptr<Auditory_perceptual_sound> obj_ptr = dynamic_pointer_cast<Auditory_perceptual_sound>(get_sound_ptr(sound_name));

//	Auditory_sound * p = get_sound_ptr(sound_name);
//	shared_ptr<Auditory_perceptual_sound> obj_ptr = dynamic_cast<Auditory_perceptual_sound *>(p);
	if(!obj_ptr)
		throw Epic_internal_error(this, "object was not a Auditory_perceptual_sound");
	return obj_ptr;
}


/*** Auditory_perceptual_store ***/

// these functions use the base class then construct an update for the cognitive processor, that needs
// old and new values for updating.
void Auditory_perceptual_store::accept_event(const Auditory_event * event_ptr)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << typeid(*event_ptr).name() << " received" << endl;
	broadcast_to_recorders(event_ptr);
	// tell the event to handle itself with itself!
	event_ptr->handle_self(this);
}


/* Streams are stored and reported to the Cognitive processor, but not otherwise changed
there are no onset etc events associated with a stream
*/

void Auditory_perceptual_store::handle_event(const Auditory_Stream_Create_event * event_ptr)
{
	Auditory_store::handle_event(event_ptr);
	
	if(changed) {
		// immediately post the change - the stream is present
		get_human_ptr()->get_Cognitive_processor_ptr()->
			update_object_property(Auditory_c, event_ptr->stream_name, Status_c, Nil_c, Present_c);
		}
}


void Auditory_perceptual_store::handle_event(const Auditory_Stream_Destroy_event * event_ptr)
{
	Auditory_store::handle_event(event_ptr);
	// schedule an erasure event for later
	schedule_event(new Auditory_Stream_Erase_event( 
		get_time() + unsupported_decay_delay.get_long_value(), this,
		event_ptr->stream_name));
}

void Auditory_perceptual_store::handle_event(const Auditory_Stream_Erase_event * event_ptr)
{
	const Symbol& stream_name = event_ptr->stream_name;
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << stream_name  << " lost from memory" << endl;
	
	shared_ptr<Auditory_stream> obj_ptr = get_stream_ptr(event_ptr->stream_name);
	

	get_human_ptr()->get_Cognitive_processor_ptr()->
		update_object_property(Auditory_c, stream_name, Status_c, Present_c, Nil_c);
	get_human_ptr()->get_Cognitive_processor_ptr()->
		update_object_property(Auditory_c, stream_name, Status_c, Present_c, Nil_c);
	get_human_ptr()->get_Cognitive_processor_ptr()->
		update_object_property(Auditory_c, stream_name, Status_c, Present_c, Nil_c);
		
	// collect the other properties of the object
	Symbol_list_t prop_names;
	Symbol_list_t prop_values;
	// this contains only properties with non-default values
	obj_ptr->get_pv_lists(prop_names, prop_values);
	
	// immediately post removals of the properties of the object
	Symbol_list_t ::iterator it_name = prop_names.begin();
	Symbol_list_t ::iterator it_value = prop_values.begin();
	for(;it_name != prop_names.end() && it_value != prop_values.end(); ++it_name, ++it_value) {
		get_human_ptr()->get_Cognitive_processor_ptr()->
			update_object_property(Auditory_c, stream_name, *it_name, *it_value, Nil_c);
		}

	// now actually erase the object from this store
	Auditory_store::erase_stream(stream_name);
}

void Auditory_perceptual_store::handle_event(const Auditory_Stream_Set_Location_event * event_ptr)
{
	Auditory_store::handle_event(event_ptr);
	
	if(changed) {
		// immediately post the change
		get_human_ptr()->get_Cognitive_processor_ptr()->
			update_object_property(Auditory_c, event_ptr->stream_name, Location_c, Nil_c, Change_c);
		// schedule for later delivery
		schedule_event(new Cognitive_Update_event(
			get_time() + change_decay_time.get_long_value(), get_human_ptr()->get_Cognitive_processor_ptr(),
			Auditory_c, event_ptr->stream_name, Location_c, Change_c, Nil_c));
		}
}

void Auditory_perceptual_store::handle_event(const Auditory_Stream_Set_Pitch_event * event_ptr)
{
	Auditory_store::handle_event(event_ptr);
	
	if(changed) {
		// immediately post the change
		get_human_ptr()->get_Cognitive_processor_ptr()->
			update_object_property(Auditory_c, event_ptr->stream_name, Pitch_c, Nil_c, Change_c);
		// schedule for later delivery
		schedule_event(new Cognitive_Update_event(
			get_time() + change_decay_time.get_long_value(), get_human_ptr()->get_Cognitive_processor_ptr(),
			Auditory_c, event_ptr->stream_name, Pitch_c, Change_c, Nil_c));
		}
}

void Auditory_perceptual_store::handle_event(const Auditory_Stream_Set_Loudness_event * event_ptr)
{
	Auditory_store::handle_event(event_ptr);
	
	if(changed) {
		// immediately post the change
		get_human_ptr()->get_Cognitive_processor_ptr()->
			update_object_property(Auditory_c, event_ptr->stream_name, Loudness_c, Nil_c, Change_c);
		// schedule for later delivery
		schedule_event(new Cognitive_Update_event(
			get_time() + change_decay_time.get_long_value(), get_human_ptr()->get_Cognitive_processor_ptr(),
			Auditory_c, event_ptr->stream_name, Loudness_c, Change_c, Nil_c));
		}
}

void Auditory_perceptual_store::handle_event(const Auditory_Stream_Set_Size_event * event_ptr)
{
	Auditory_store::handle_event(event_ptr);
	
	if(changed) {
		// immediately post the change
		get_human_ptr()->get_Cognitive_processor_ptr()->
			update_object_property(Auditory_c, event_ptr->stream_name, Size_c, Nil_c, Change_c);

		// schedule for later delivery
		schedule_event(new Cognitive_Update_event(
			get_time() + change_decay_time.get_long_value(), get_human_ptr()->get_Cognitive_processor_ptr(),
			Auditory_c, event_ptr->stream_name, Size_c, Change_c, Nil_c));
		}
}

void Auditory_perceptual_store::handle_event(const Auditory_Stream_Set_Property_event * event_ptr)
{
	// update the store and broadcast the change
	Symbol old_value = Auditory_store::set_stream_property(event_ptr->stream_name, event_ptr->property_name, event_ptr->property_value);
	// send through to cognitive processor immediately
	get_human_ptr()->get_Cognitive_processor_ptr()->
		update_object_property(Auditory_c, event_ptr->stream_name, event_ptr->property_name, 
			old_value, event_ptr->property_value);
}

void Auditory_perceptual_store::handle_event(const Auditory_Stream_Erase_Property_event * )
{
	Assert("Auditory_perceptual_store::handle_event(const Auditory_Stream_Set_Property_event *) called unexpectedly");
}


// accept each event, update the local store using the local type of object
void Auditory_perceptual_store::handle_event(const Auditory_Sound_Start_event * event_ptr)
{
//	Auditory_store::handle_event(event_ptr);

	// create the new sound object - a continuing sound has duration 0
	insert_new(Auditory_perceptual_sound::create(event_ptr->sound_name, event_ptr->stream_name, event_ptr->time_stamp, event_ptr->location, event_ptr->timbre, 0, event_ptr->intrinsic_duration));

	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "start sound " << event_ptr->sound_name << event_ptr->stream_name  << ' ' << event_ptr->time_stamp 
			<< ' ' << event_ptr->timbre<< ' '<< event_ptr->intrinsic_duration << endl;		

	notify_views(&View_base::notify_auditory_sound_start, event_ptr->sound_name, event_ptr->stream_name, event_ptr->time_stamp, event_ptr->location, event_ptr->timbre, event_ptr->loudness);
	
	changed = true;


	// immediately post that the object is audible and its timbre and time_stamp
	get_human_ptr()->get_Cognitive_processor_ptr()->
		update_object_property(Auditory_c, event_ptr->sound_name, Status_c, Nil_c, Audible_c);
	// create a stream clause only if one was specified:
	if(event_ptr->stream_name != Symbol())
		get_human_ptr()->get_Cognitive_processor_ptr()->
			update_object_property(Auditory_c, event_ptr->sound_name, Stream_c, Nil_c, event_ptr->stream_name);
	get_human_ptr()->get_Cognitive_processor_ptr()->
		update_object_property(Auditory_c, event_ptr->sound_name, Timbre_c, Nil_c, event_ptr->timbre);
	get_human_ptr()->get_Cognitive_processor_ptr()->
		update_object_property(Auditory_c, event_ptr->sound_name, Time_stamp_c, Nil_c, event_ptr->time_stamp);
}

void Auditory_perceptual_store::handle_event(const Auditory_Speech_Start_event * event_ptr)
{
//	Auditory_store::handle_event(event_ptr);

	// create the new sound object - a continuing sound has duration 0
//	insert_new(Auditory_perceptual_sound::create(event_ptr->sound_name, event_ptr->stream_name, event_ptr->time_stamp, 
//		Speech_c, event_ptr->content,  event_ptr->speaker_gender,  event_ptr->speaker_id,  event_ptr->loudness, 0, event_ptr->duration));

	insert_new(Auditory_perceptual_sound::create(event_ptr->word));

	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "start speech sound " 
			<< event_ptr->word.name << event_ptr->word.stream_name  << ' ' << event_ptr->word.time_stamp
            << ' ' << event_ptr->word.location
			<< ' ' << event_ptr->word.content << ' ' << event_ptr->word.speaker_gender << ' ' << event_ptr->word.speaker_id 
			<< ' ' << event_ptr->word.loudness << ' '<< event_ptr->word.pitch << ' '<< event_ptr->word.duration << endl;		

//	notify_views(&View_base::notify_auditory_speech_start, event_ptr->sound_name, event_ptr->stream_name, event_ptr->time_stamp, event_ptr->content, event_ptr->speaker_gender, event_ptr->speaker_id, event_ptr->loudness);
	notify_views(&View_base::notify_auditory_speech_start, event_ptr->word);
	
	changed = true;

	// immediately post that the object is audible and its timbre and time_stamp
	get_human_ptr()->get_Cognitive_processor_ptr()->
		update_object_property(Auditory_c, event_ptr->word.name, Status_c, Nil_c, Audible_c);
	// create a stream clause only if one was specified:
	if(event_ptr->word.stream_name != Symbol())
		get_human_ptr()->get_Cognitive_processor_ptr()->
			update_object_property(Auditory_c, event_ptr->word.name, Stream_c, Nil_c, event_ptr->word.stream_name);
	get_human_ptr()->get_Cognitive_processor_ptr()->
		update_object_property(Auditory_c, event_ptr->word.name, Time_stamp_c, Nil_c, event_ptr->word.time_stamp);
    // provide only the azimuth (location x-value)
	get_human_ptr()->get_Cognitive_processor_ptr()->
		update_object_property(Auditory_c, event_ptr->word.name, Azimuth_c, Nil_c, event_ptr->word.location.x);
	get_human_ptr()->get_Cognitive_processor_ptr()->
		update_object_property(Auditory_c, event_ptr->word.name, Content_c, Nil_c, event_ptr->word.content);
	get_human_ptr()->get_Cognitive_processor_ptr()->
		update_object_property(Auditory_c, event_ptr->word.name, Gender_c, Nil_c, event_ptr->word.speaker_gender);
	get_human_ptr()->get_Cognitive_processor_ptr()->
		update_object_property(Auditory_c, event_ptr->word.name, Speaker_c, Nil_c, event_ptr->word.speaker_id);
	get_human_ptr()->get_Cognitive_processor_ptr()->
		update_object_property(Auditory_c, event_ptr->word.name, Loudness_c, Nil_c, event_ptr->word.loudness);
	get_human_ptr()->get_Cognitive_processor_ptr()->
		update_object_property(Auditory_c, event_ptr->word.name, Pitch_c, Nil_c, event_ptr->word.pitch);
}



void Auditory_perceptual_store::handle_event(const Auditory_Sound_Stop_event * event_ptr)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << event_ptr->sound_name  << " losing perceptual support" << endl;
	shared_ptr<Auditory_perceptual_sound> obj_ptr = get_perceptual_object_ptr(event_ptr->sound_name);
	
	// The object must first go into the unsupported state - it is still fully present as far as anyone else is concerned
	// Put the object into the "Unsupported" state to show that perceptual input no longer supports it
	obj_ptr->set_state(Auditory_perceptual_sound::UNSUPPORTED);

	// schedule an erasure event for later
	schedule_event(new Auditory_Sound_Erase_event( 
		get_time() + unsupported_decay_delay.get_long_value(), this,
		event_ptr->sound_name));
}



void Auditory_perceptual_store::handle_event(const Auditory_Sound_Erase_event * event_ptr)
{
	// check to see if the object is still present - it might have already disappeared
	if(!is_sound_present(event_ptr->sound_name)) {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << event_ptr->sound_name 
				<< " no longer present for erasure" << endl;
		return;
		}
	
	shared_ptr<Auditory_perceptual_sound> obj_ptr = get_perceptual_object_ptr(event_ptr->sound_name);
	Assert(obj_ptr);	// only if gross programming error

	// if the object was in the unsupported state, it is now "disappearing" in the RETAINED state.
	switch(obj_ptr->get_state()) {
		case Auditory_perceptual_sound::UNSUPPORTED: {
			if(get_trace() && Trace_out)
				Trace_out << processor_info() << event_ptr->sound_name  << " being retained in auditory memory " << endl;

			// This does not remove the object, just updates the views
			Auditory_store::make_sound_stop(event_ptr->sound_name);

			// immediately tell cogp that the object is disappearing
			get_human_ptr()->get_Cognitive_processor_ptr()->
				update_object_property(Auditory_c, event_ptr->sound_name, Status_c, Audible_c, Fading_c);
		
			// we would decide here whether to  actually retain this  object, or really get rid of it

			// Put the object into the "Retained" state to show that it is now in the auditory memory state
			obj_ptr->set_state(Auditory_perceptual_sound::RETAINED);

			
			// schedule an erasure event for later
			schedule_event(new Auditory_Sound_Erase_event( 
				get_time() + retained_decay_delay.get_long_value(), this, event_ptr->sound_name));
				
			break;
		}
		
		// if it was already in the retained state, it is now time to get rid of it
		case Auditory_perceptual_sound::RETAINED: {
			// the object's time is up - get rid of it
			lose_sound_from_memory(obj_ptr);
			break;
		}
		
		default: {
			throw Epic_internal_error(this, "Unknown object state in handling Auditory_Sound_Erase_event");
			break;
			}
		}
}

// this function removes a sound from the perceptual store and all of its properties from cognitive processor
// we might call it from a memory retention function, so it has to accept objects in different states
void Auditory_perceptual_store::lose_sound_from_memory(shared_ptr<Auditory_perceptual_sound> obj_ptr)
{
	const Symbol& sound_name = obj_ptr->get_name();
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << sound_name  << " lost from memory" << endl;

	// remove the relevant properties from cognitive processor's memory
	switch(obj_ptr->get_state()) {
		default:
		case Auditory_perceptual_sound::PRESENT: 
		case Auditory_perceptual_sound::UNSUPPORTED: {
			// remove the Audible property
			get_human_ptr()->get_Cognitive_processor_ptr()->
				update_object_property(Auditory_c, sound_name, Status_c, Audible_c, Nil_c);
		// deliberate drop through to the RETAINED case
			}
		case Auditory_perceptual_sound::RETAINED: {
			// remove stream, timbre and next separately, and also speech characteristics
			get_human_ptr()->get_Cognitive_processor_ptr()->
				update_object_property(Auditory_c, sound_name, Timbre_c, obj_ptr->get_timbre(), Nil_c);
			get_human_ptr()->get_Cognitive_processor_ptr()->
				update_object_property(Auditory_c, sound_name, Stream_c, obj_ptr->get_stream(), Nil_c);
			get_human_ptr()->get_Cognitive_processor_ptr()->
				update_object_property(Auditory_c, sound_name, Time_stamp_c, obj_ptr->get_time_stamp(), Nil_c);
            // remove only the azimuth (location x-value)
			get_human_ptr()->get_Cognitive_processor_ptr()->
				update_object_property(Auditory_c, sound_name, Azimuth_c, obj_ptr->get_location().x, Nil_c);
			get_human_ptr()->get_Cognitive_processor_ptr()->
				update_object_property(Auditory_c, sound_name, Content_c, obj_ptr->get_content(), Nil_c);
			get_human_ptr()->get_Cognitive_processor_ptr()->
				update_object_property(Auditory_c, sound_name, Gender_c, obj_ptr->get_gender(), Nil_c);
			get_human_ptr()->get_Cognitive_processor_ptr()->
				update_object_property(Auditory_c, sound_name, Speaker_c, obj_ptr->get_speaker_id(), Nil_c);
			get_human_ptr()->get_Cognitive_processor_ptr()->
				update_object_property(Auditory_c, sound_name, Loudness_c, obj_ptr->get_loudness(), Nil_c);
			get_human_ptr()->get_Cognitive_processor_ptr()->
				update_object_property(Auditory_c, sound_name, Pitch_c, obj_ptr->get_pitch(), Nil_c);
			// the sound is no longer present, so the fading property is also gone
			get_human_ptr()->get_Cognitive_processor_ptr()->
				update_object_property(Auditory_c, sound_name, Status_c, Fading_c, Nil_c);
			break;
			}
		}

	// collect the other properties of the object
	Symbol_list_t prop_names;
	Symbol_list_t prop_values;
	// this contains only properties with non-default values
	obj_ptr->get_pv_lists(prop_names, prop_values);
	
	// immediately post removals of the properties of the object
	Symbol_list_t ::iterator it_name = prop_names.begin();
	Symbol_list_t ::iterator it_value = prop_values.begin();
	for(;it_name != prop_names.end() && it_value != prop_values.end(); ++it_name, ++it_value) {
		get_human_ptr()->get_Cognitive_processor_ptr()->
			update_object_property(Auditory_c, sound_name, *it_name, *it_value, Nil_c);
		}

	// now actually erase the object from this store
	Auditory_store::erase_sound(sound_name);
	// we are assuming at this point that the physical and psychological names will no longer be valid
	// and so can be removed
	Name_map& name_map = get_human_ptr()->get_Ear_processor_ptr()->get_name_map();
	name_map.remove_names_with_psychological_name(sound_name);

}


void Auditory_perceptual_store::handle_event(const Auditory_Sound_Set_Property_event * event_ptr)
{
	// access the object
	shared_ptr<Auditory_sound> obj_ptr = get_sound_ptr(event_ptr->sound_name);
	Symbol old_value = obj_ptr->get_property_value(event_ptr->property_name);
	// are we removing a property value that is still present? - assuming everything else is OK
	if(event_ptr->property_value == Nil_c && old_value != Nil_c && event_ptr->property_name != Detection_c) {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << event_ptr->sound_name 
				<< " losing perceptual support for " << event_ptr->property_name << endl;
	
		long disappearing_time = get_time() + property_decay_time_delay.get_long_value();
		// save the disappearance time for this property
		obj_ptr->set_disappearing_time(event_ptr->property_name, disappearing_time);
		// schedule a property erasure event for later - send old value along for comparison to current value
		schedule_event(new Auditory_Sound_Erase_Property_event(disappearing_time, this,
			event_ptr->sound_name, event_ptr->property_name, old_value));
		return;	// do nothing further
		}
		
	// otherwise, we are setting to a real value, so remove any disappearance marker - the value has been reestablished
	// so any pending erasure must be canceled!
	obj_ptr->remove_disappearing_time(event_ptr->property_name);

	// then update the store and broadcast the change
	old_value = Auditory_store::set_sound_property(event_ptr->sound_name, event_ptr->property_name, event_ptr->property_value);
	// send through to cognitive processor immediately
	get_human_ptr()->get_Cognitive_processor_ptr()->
		update_object_property(Auditory_c, event_ptr->sound_name, event_ptr->property_name, 
			old_value, event_ptr->property_value);
}

void Auditory_perceptual_store::handle_event(const Auditory_Sound_Erase_Property_event * event_ptr)
{
	// here because the property value is being erased
	// do nothing if the object is not present - not very efficient, but erasure of whole object may have already happened
	if(!is_sound_present(event_ptr->sound_name)) {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << event_ptr->sound_name 
				<< " no longer present for erasure of " << event_ptr->property_name << endl;
		return;
		}
	// access the disappearing_property record
	shared_ptr<Auditory_sound> obj_ptr = get_sound_ptr(event_ptr->sound_name);
	Symbol current_value = obj_ptr->get_property_value(event_ptr->property_name);
	// this returns 0 if the disappearance record for this property does not exist
	long disappearing_time = obj_ptr->get_disappearing_time(event_ptr->property_name);
	// do nothing further if the disappearance record does not exit - it was conceled.
	if(disappearing_time == 0)
		return;
	
	// The disappearance time should match the current time, and the value in the event must match the current state;
	// if so, do the erasure and removal from the cognitive processor store
	// otherwise this erasure has been overridden and should just be ignored
	if (get_time() == disappearing_time && current_value == event_ptr->property_value) {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << event_ptr->sound_name 
				<< " property " << event_ptr->property_name << " lost" << endl;
		// update the store and broadcast the change
		Symbol old_value = Auditory_store::set_sound_property(event_ptr->sound_name, event_ptr->property_name, Nil_c);
		// send through to cognitive processor immediately
		get_human_ptr()->get_Cognitive_processor_ptr()->
			update_object_property(Auditory_c, event_ptr->sound_name, event_ptr->property_name, 
				event_ptr->property_value, Nil_c);
		}
	// always remove the property disappearance record (nothing happens if not present)
	obj_ptr->remove_disappearing_time(event_ptr->property_name);
}

