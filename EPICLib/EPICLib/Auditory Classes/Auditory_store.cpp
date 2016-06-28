#include "Auditory_store.h"
#include "Speech_word.h"
#include "Coordinator.h"
#include "Epic_exception.h"
#include "Output_tee_globals.h"
#include "Symbol_utilities.h"
#include "View_base.h"
#include "Standard_Symbols.h"
//#include "Assert_throw.h"

#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <typeinfo>
#include <memory>


using std::shared_ptr;
using std::cout;	using std::endl;
using std::string;
using std::ostringstream;
using std::list;


/*** Exception classes ***/
// these are user-interpretable errors
class Create_duplicate_auditory_stream : public Epic_exception {
public:
Create_duplicate_auditory_stream(const Processor * proc_ptr, const Symbol& obj_name)
	{
		ostringstream oss;
		oss << proc_ptr->processor_info() 
			<< "Attempt to create duplicate auditory stream with name: "
			<< obj_name;
		msg = oss.str();
	}
};

class Unknown_auditory_stream : public Epic_exception {
public:
Unknown_auditory_stream(const Processor * proc_ptr, const Symbol& obj_name)
	{
		ostringstream oss;
		oss << proc_ptr->processor_info() 
			<< "Attempt to access unknown auditory stream with id: "
			<< obj_name;
		msg = oss.str();
	}
};

class Create_duplicate_auditory_sound : public Epic_exception {
public:
Create_duplicate_auditory_sound(const Processor * proc_ptr, const Symbol& obj_name)
	{
		ostringstream oss;
		oss << proc_ptr->processor_info() 
			<< "Attempt to create duplicate auditory sound with name: "
			<< obj_name;
		msg = oss.str();
	}
};

class Unknown_auditory_sound : public Epic_exception {
public:
Unknown_auditory_sound(const Processor * proc_ptr, const Symbol& obj_name)
	{
		ostringstream oss;
		oss << proc_ptr->processor_info() 
			<< "Attempt to access unknown auditory sound with id: "
			<< obj_name;
		msg = oss.str();
	}
};





/*** Auditory_stream ***/

/*
Auditory_stream::Auditory_stream(const Symbol& name_, GU::Point location_, GU::Size size_) :
		name(name_), pitch(0.), loudness(0.), location(location_), size(size_), time_stamp(Coordinator::get_time())
		{}
*/
Auditory_stream::Auditory_stream(const Symbol& name_, double pitch_, double loudness_, GU::Point location_) :
		name(name_), pitch(pitch_), loudness(loudness_), location(location_), time_stamp(Coordinator::get_time())
		{}

void Auditory_stream::set_disappearing_time(const Symbol&, long)
{
	Assert(!"set_disappearing_time called on wrong kind of object");
}

void Auditory_stream::remove_disappearing_time(const Symbol&)
{
	Assert(!"remove_disappearing_time called on wrong kind of object");
}

// returns zero if not present - 
long Auditory_stream::get_disappearing_time(const Symbol&) const
{
	Assert(!"get_disappearing_time called on wrong kind of object");
	return 0;
}

void Auditory_stream::display_contents(Output_tee& ot) const
{
	ot << name << " at " << location << endl;
	properties.display_contents(ot);
}

Symbol Auditory_stream::create_next_sound_name()
{
	Assert(!"create_next_sound_name called on wrong kind of object");
	return Symbol();
}

Symbol Auditory_stream::get_next_sound_name() const
{
	Assert(!"get_next_sound_name called on wrong kind of object");
	return Symbol();
}



/*** Auditory_sound ***/
// 7/2/15 TODO rationalize this class - it is trying to do too much

Auditory_sound::Auditory_sound(const Symbol& name_, const Symbol& stream_, long time_stamp_, GU::Point location_, const Symbol& timbre_, 
			double loudness_, long duration_, long intrinsic_duration_) :
		name(name_), stream(stream_), time_stamp(time_stamp_), location(location_), timbre(timbre_), 
		content(Symbol()), gender(Symbol()), speaker_id(Symbol()),
		loudness(loudness_), pitch(0.), duration(duration_), intrinsic_duration(intrinsic_duration_),
        fading(false)
		{}

/* Auditory_sound::Auditory_sound(const Symbol& name_, const Symbol& stream_, long time_stamp_, GU::Point location_, const Symbol& timbre_,
			const Symbol& content_, const Symbol& gender_, const Symbol& speaker_id_, int utterance_id_, double loudness_, double level_right_, double level_left_, double pitch_, long duration_, long intrinsic_duration_) :
		name(name_), stream(stream_), time_stamp(time_stamp_), location(location_), timbre(timbre_), 
		content(content_), gender(gender_), speaker_id(speaker_id_), utterance_id(utterance_id_),
		loudness(loudness_), level_right(level_right_), level_left(level_left_), pitch(pitch_), duration(duration_), intrinsic_duration(intrinsic_duration_),
        fading(false)
//		time_stamp(Coordinator::get_time())
		{}
*/

Auditory_sound::Auditory_sound(const Speech_word& word) :
		name(word.name), stream(word.stream_name), time_stamp(word.time_stamp), location(word.location), timbre(Speech_c),
		content(word.content), gender(word.speaker_gender), speaker_id(word.speaker_id), utterance_id(word.utterance_id),
		loudness(word.loudness), level_right(word.level_right), level_left(word.level_left), pitch(word.pitch), duration(word.duration), intrinsic_duration(word.duration),
        fading(false)
//		time_stamp(Coordinator::get_time())
		{}
		

std::shared_ptr<Auditory_sound> Auditory_sound::create(const Symbol& sound_name, const Symbol& stream_name, long time_stamp, GU::Point location, const Symbol& timbre,
			double loudness, long duration, long intrinsic_duration)
		{return std::shared_ptr<Auditory_sound>(new Auditory_sound(sound_name, stream_name, time_stamp, location, timbre, loudness, duration, intrinsic_duration));}
// fatter interface for speech sounds
/* Smart_Pointer<Auditory_sound> Auditory_sound::create(const Symbol& sound_name, const Symbol& stream_name, long time_stamp, GU::Point location, const Symbol& timbre,
			const Symbol& content, const Symbol& gender, const Symbol& speaker_id, int utterance_id, double loudness, double pitch, long duration, long intrinsic_duration)
		{return new Auditory_sound(sound_name, stream_name, time_stamp, location, timbre, content, gender, speaker_id, utterance_id, loudness, pitch, duration, intrinsic_duration);}
*/
// smaller interface for speech sounds
std::shared_ptr<Auditory_sound> Auditory_sound::create(const Speech_word& word)
        {return std::shared_ptr<Auditory_sound>(new Auditory_sound(word));}
//		{return std::shared_ptr<Auditory_sound>(new Auditory_sound(word.name, word.stream_name, word.time_stamp, word.location, Speech_c, word.content, word.speaker_gender, word.speaker_id, word.utterance_id, word.loudness, word.level_right, word.level_left, word.pitch, 0, word.duration));}


void Auditory_sound::set_disappearing_time(const Symbol&, long)
{
	Assert(!"set_disappearing_time called on wrong kind of object");
}

void Auditory_sound::remove_disappearing_time(const Symbol&)
{
	Assert(!"remove_disappearing_time called on wrong kind of object");
}

// returns zero if not present - 
long Auditory_sound::get_disappearing_time(const Symbol&) const
{
	Assert(!"get_disappearing_time called on wrong kind of object");
	return 0;
}

void Auditory_sound::display_contents(Output_tee& ot) const
{
//	ot << name << ' ' << stream << ' ' << time_stamp << ' ' << timbre << ' '
	ot << name << ' ' << stream << ' ' << time_stamp << ' ' << location << ' ' << timbre << ' '
		<< content << ' '  << gender << ' '  << speaker_id << ' '  << loudness << ' '
		<< duration << ' ' << intrinsic_duration << ' ' << fading << endl;
	properties.display_contents(ot);
}




/*** Auditory_store ***/

Auditory_store::Auditory_store(const std::string& name_, Human_processor * human_ptr_) :
		Auditory_processor (name_, human_ptr_)
{
//	Assert(destination_ptr);
}

void Auditory_store::initialize()
{
	Human_subprocessor::initialize();
	streams.clear();
	sounds.clear();
//	set_randomize_when_used(true);
//	set_randomize(true);
//	randomize();
//	describe_parameters(Normal_out);
}


// this version of the function is called to create a new object when its name is available,
// which applies after the physical store
// exception thrown if input is invalid
/* disabled 5/9/12
void Auditory_store::create_stream(const Symbol& name, GU::Point location, GU::Size size)
{
	// create the new object
	insert_new(Auditory_stream::create(name, location, size));
	
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "create stream " << name << " at " << location << endl;
			
	notify_views(&View_base::notify_auditory_stream_appear, name, location, size);
	
	changed = true;
}
*/

void Auditory_store::create_stream(const Symbol& name, double pitch, double loudness, GU::Point location)
{
	// create the new object
	insert_new(Auditory_stream::create(name, pitch, loudness, location));
	
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "create stream " << name << " with " << ' ' << pitch << ' ' << loudness << ' ' << location << endl;
			
	notify_views(&View_base::notify_auditory_stream_appear, name, pitch, loudness, location);
	
	changed = true;
}

// nothing is done here immediately when a stream is destroyed - overrider is responsible
void Auditory_store::destroy_stream(const Symbol& obj_name)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "destroy stream " << obj_name << endl;

	notify_views(&View_base::notify_auditory_stream_disappear, obj_name);

	// exception thrown if input was invalid
	changed = false;
}

void Auditory_store::erase_stream(const Symbol& obj_name)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "erase stream " << obj_name << endl;

	notify_views(&View_base::notify_erase_object, obj_name);

	erase_from_streams(obj_name);
}

// return the old location
GU::Point Auditory_store::set_stream_location(const Symbol& obj_name, GU::Point location)
{
	changed = false;
	
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "set stream " << obj_name 
			<< " location to " << location << endl;
	shared_ptr<Auditory_stream> obj_ptr = get_stream_ptr(obj_name);	// throws if not there

	// do nothing more if location has not actually changed
	GU::Point old_location = obj_ptr->get_location();
	if(location == old_location)
		return old_location;

	// update the location
	obj_ptr->set_location(location);

	notify_views(&View_base::notify_auditory_stream_location_changed, obj_name, location);

	// exception thrown if input was invalid
	changed = true;
	return old_location;
}

double Auditory_store::set_stream_pitch(const Symbol& obj_name, double pitch)
{
	changed = false;
	
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "set stream " << obj_name 
			<< " pitch to " << pitch << endl;
	shared_ptr<Auditory_stream> obj_ptr = get_stream_ptr(obj_name);	// throws if not there

	// do nothing more if pitch has not actually changed
	double old_pitch = obj_ptr->get_pitch();
	if(pitch == old_pitch)
		return old_pitch;

	// update the location
	obj_ptr->set_pitch(pitch);

	notify_views(&View_base::notify_auditory_stream_pitch_changed, obj_name, pitch);

	// exception thrown if input was invalid
	changed = true;
	return old_pitch;
}

double Auditory_store::set_stream_loudness(const Symbol& obj_name, double loudness)
{
	changed = false;
	
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "set stream " << obj_name 
			<< " loudness to " << loudness << endl;
	shared_ptr<Auditory_stream> obj_ptr = get_stream_ptr(obj_name);	// throws if not there

	// do nothing more if loudness has not actually changed
	double old_loudness = obj_ptr->get_loudness();
	if(loudness == old_loudness)
		return old_loudness;

	// update the location
	obj_ptr->set_loudness(loudness);

	notify_views(&View_base::notify_auditory_stream_loudness_changed, obj_name, loudness);

	// exception thrown if input was invalid
	changed = true;
	return old_loudness;
}

// return the old size
GU::Size Auditory_store::set_stream_size(const Symbol& obj_name, GU::Size size)
{
/* disabled 5/8/2012
	changed = false;

	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "set stream " << obj_name 
			<< " size to " << size << endl;
	Smart_Pointer<Auditory_stream> obj_ptr = get_stream_ptr(obj_name);	// throws if not there

	// do nothing more if size has not actually changed
	GU::Size old_size = obj_ptr->get_size();
	if(size == old_size)
		return old_size;

	obj_ptr->set_size(size);

	notify_views(&View_base::notify_auditory_stream_size_changed, obj_name, size);

	// exception thrown if input was invalid
	changed = true;
	return old_size;
*/
	return GU::Size();
}


// return the old property value
Symbol Auditory_store::set_stream_property(const Symbol& obj_name, const Symbol& prop_name, 
	const Symbol& prop_value)
{
	changed = false;

	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "set stream " << obj_name 
			<< " property " << prop_name << " to " << prop_value << endl;

	shared_ptr<Auditory_stream> obj_ptr = get_stream_ptr(obj_name);	// throws if not there
	// Symbol() will be returned if property was not there before
	Symbol old_prop_value = obj_ptr->get_property_value(prop_name);
	// do nothing if no change in property value
	if(prop_value == old_prop_value)
		return old_prop_value;
		
	obj_ptr->set_property(prop_name, prop_value); // will remove the property if value is Symbol();

	notify_views(&View_base::notify_auditory_stream_property_changed, obj_name, prop_name, prop_value);

	// exception thrown if input was invalid
	changed = true;
	return old_prop_value;
}


// return a list of the names of all the current streams
Symbol_list_t Auditory_store::get_stream_name_list() const
{
	Symbol_list_t results;
	for(Stream_container_t::const_iterator it = streams.begin(); it != streams.end(); ++it)
		results.push_back((it->second)->get_name());
	return results;
}

Auditory_store::Stream_ptr_list_t Auditory_store::get_stream_ptr_list() const
{
	Stream_ptr_list_t results;
	for(Stream_container_t::const_iterator it = streams.begin(); it != streams.end(); ++it)
		results.push_back((it->second));
	return results;
}

void Auditory_store::insert_new(shared_ptr<Auditory_stream> obj_ptr)
{
	const Symbol& obj_name = obj_ptr->get_name();
	Stream_container_t::iterator it = streams.find(obj_name);
	if(it != streams.end()) {
		throw Create_duplicate_auditory_stream(this, obj_name);
		}
	else
		streams[obj_name] = obj_ptr;
}

// erase the specified object,throw an exception if not present
void Auditory_store::erase_from_streams(const Symbol& obj_name)
{
	int n = int(streams.erase(obj_name));
	if(n == 0)
		throw Unknown_auditory_stream(this, obj_name);
}


shared_ptr<Auditory_stream> Auditory_store::get_stream_ptr(const Symbol& obj_name) const
{
	Stream_container_t::const_iterator it;
	it = streams.find(obj_name);
	if(it == streams.end())
		throw Unknown_auditory_stream(this, obj_name);
	
	return it->second;
}

bool Auditory_store::is_stream_present(const Symbol& obj_name) const
{
	Stream_container_t::const_iterator it;
	it = streams.find(obj_name);
	if(it == streams.end())
		return false;
	return true;
}

//void Auditory_store::make_new_sound_start(const Symbol& sound_name, const Symbol& stream_name, long time_stamp, const Symbol& timbre, double loudness, 
void Auditory_store::make_new_sound_start(const Symbol& sound_name, const Symbol& stream_name, long time_stamp, GU::Point location, const Symbol& timbre, double loudness, 
	long intrinsic_duration)
{
	// create the new sound object - a continuing sound has duration 0
//	insert_new(Auditory_sound::create(sound_name, stream_name, time_stamp, timbre, loudness, 0, intrinsic_duration));
	insert_new(Auditory_sound::create(sound_name, stream_name, time_stamp, location, timbre, loudness, 0, intrinsic_duration));

	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "start sound " << sound_name << ' ' << stream_name << ' ' << timbre << ' ' 
			<< loudness << ' ' << intrinsic_duration << endl;		

	notify_views(&View_base::notify_auditory_sound_start, sound_name, stream_name, time_stamp, location, timbre, loudness);
	
	changed = true;
}

// bigger interface for speech sounds, where intrinsic_duration is same as duration
//void Auditory_store::make_new_speech_start(const Symbol& name, const Symbol& stream, long time_stamp, 
//		const Symbol& content, const Symbol& speaker_gender, const Symbol& speaker_id, double loudness, long duration)
void Auditory_store::make_new_speech_start(const Speech_word& word)
{
	// create the new sound object - coninuing sound, so duration is 0, and intrinsic_duration is the duration of the speech sound
//	insert_new(Auditory_sound::create(name, stream, time_stamp, Speech_c, content, speaker_gender, speaker_id, loudness, 0, duration));
	insert_new(Auditory_sound::create(word));

	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "start sound " << word.name << ' ' << word.stream_name << ' ' << Speech_c << ' ' 
			<< word.loudness << ' ' << word.duration << ' ' << word.content << ' ' << word.speaker_gender << ' ' << word.speaker_id << ' ' << endl;		

//	notify_views(&View_base::notify_auditory_speech_start, word.name, word.stream_name, word.time_stamp, word.content, word.speaker_gender, word.speaker_id, word.loudness);
	notify_views(&View_base::notify_auditory_speech_start, word);
	
	changed = true;
}

// when the sound stops we immediately set its state to fading
void Auditory_store::make_sound_stop(const Symbol& name)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "stop sound " << name << endl;

	notify_views(&View_base::notify_auditory_sound_stop, name);
    
    get_sound_ptr(name)->set_fading(true);

	changed = true;
}

void Auditory_store::make_sound_event(const Symbol&, const Symbol&, 
	const Symbol&, double, long, long)
	{}
	
/*
void Auditory_store::make_speech_event(const Symbol& sound_name, const Symbol& stream_name, 
		const Symbol& content, const Symbol& speaker_gender, const Symbol& speaker_id, double loudness, long duration)
	{}
*/
void Auditory_store::make_speech_event(const Speech_word& word)
	{}

// remove the sound itself from the store
void Auditory_store::erase_sound(const Symbol& name)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "erase sound " << name << endl;

	notify_views(&View_base::notify_erase_sound, name);

	erase_from_sounds(name);
}


// return the old property value
Symbol Auditory_store::set_sound_property(const Symbol& obj_name, const Symbol& prop_name, 
	const Symbol& prop_value)
{
	changed = false;

	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "set sound " << obj_name 
			<< " property " << prop_name << " to " << prop_value << endl;

	shared_ptr<Auditory_sound> obj_ptr = get_sound_ptr(obj_name);	// throws if not there
	// Symbol() will be returned if property was not there before
	Symbol old_prop_value = obj_ptr->get_property_value(prop_name);
	// do nothing if no change in property value
	if(prop_value == old_prop_value)
		return old_prop_value;
		
	obj_ptr->set_property(prop_name, prop_value); // will remove the property if value is Symbol();

	notify_views(&View_base::notify_auditory_sound_property_changed, obj_name, prop_name, prop_value);

	// exception thrown if input was invalid
	changed = true;
	return old_prop_value;
}

/* Event interface - call the corresponding direct functions, unless overridden */


void Auditory_store::accept_event(const Auditory_event * event_ptr)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << typeid(*event_ptr).name() << " received" << endl;
	broadcast_to_recorders(event_ptr);
	// tell the event to handle itself with itself!
	event_ptr->handle_self(this);
}

void Auditory_store::handle_event(const Auditory_Stream_Create_event * event_ptr)
{
	Auditory_store::create_stream(event_ptr->stream_name, event_ptr->pitch, event_ptr->loudness, event_ptr->location);
}

void Auditory_store::handle_event(const Auditory_Stream_Destroy_event * event_ptr)
{
	Auditory_store::destroy_stream(event_ptr->stream_name);
}

void Auditory_store::handle_event(const Auditory_Stream_Erase_event * event_ptr)
{
	// now actually delete the object
	Auditory_store::erase_stream(event_ptr->stream_name);
}

void Auditory_store::handle_event(const Auditory_Stream_Set_Location_event * event_ptr)
{
	Auditory_store::set_stream_location(event_ptr->stream_name, event_ptr->location);
}
void Auditory_store::handle_event(const Auditory_Stream_Set_Pitch_event * event_ptr)
{
	Auditory_store::set_stream_pitch(event_ptr->stream_name, event_ptr->pitch);
}
void Auditory_store::handle_event(const Auditory_Stream_Set_Loudness_event * event_ptr)
{
	Auditory_store::set_stream_loudness(event_ptr->stream_name, event_ptr->loudness);
}
void Auditory_store::handle_event(const Auditory_Stream_Set_Size_event * event_ptr)
{
	Auditory_store::set_stream_size(event_ptr->stream_name, event_ptr->size);
}

void Auditory_store::handle_event(const Auditory_Stream_Set_Property_event * event_ptr)
{
	Auditory_store::set_stream_property(event_ptr->stream_name, event_ptr->property_name, event_ptr->property_value);
}

void Auditory_store::handle_event(const Auditory_Stream_Erase_Property_event * event_ptr)
{
	Auditory_store::set_stream_property(event_ptr->stream_name, event_ptr->property_name, Symbol());
}

void Auditory_store::handle_event(const Auditory_Sound_Start_event * event_ptr)
{
	Auditory_store::make_new_sound_start(event_ptr->sound_name, event_ptr->stream_name, event_ptr->time_stamp, event_ptr->location, 
		event_ptr->timbre, event_ptr->loudness, event_ptr->intrinsic_duration);
}

void Auditory_store::handle_event(const Auditory_Speech_Start_event * event_ptr)
{
/*	Auditory_store::make_new_speech_start(event_ptr->sound_name, event_ptr->stream_name, event_ptr->time_stamp, 
		event_ptr->content, event_ptr->speaker_gender, event_ptr->speaker_id, 
		event_ptr->loudness, event_ptr->duration); */
	Auditory_store::make_new_speech_start(event_ptr->word);
}

void Auditory_store::handle_event(const Auditory_Sound_Stop_event * event_ptr)
{
	Auditory_store::make_sound_stop(event_ptr->sound_name);
}

void Auditory_store::handle_event(const Auditory_Sound_Erase_event * event_ptr)
{
	Auditory_store::erase_sound(event_ptr->sound_name);
}

void Auditory_store::handle_event(const Auditory_Sound_Set_Property_event * event_ptr)
{
	Auditory_store::set_sound_property(event_ptr->sound_name, event_ptr->property_name, event_ptr->property_value);
}

void Auditory_store::handle_event(const Auditory_Sound_Erase_Property_event * event_ptr)
{
	Auditory_store::set_sound_property(event_ptr->sound_name, event_ptr->property_name, Symbol());
}

void Auditory_store::handle_event(const Auditory_Speech_event * event_ptr)
{
//	Auditory_store::make_speech_event(event_ptr->sound_name, event_ptr->stream_name, event_ptr->content, event_ptr->speaker_gender, event_ptr->speaker_id, event_ptr->loudness, event_ptr->duration);
	Auditory_store::make_speech_event(event_ptr->word);
}



// services

// return a list of the names of all the current sounds
Symbol_list_t Auditory_store::get_sound_name_list() const
{
	Symbol_list_t results;
	for(Sound_container_t::const_iterator it = sounds.begin(); it != sounds.end(); ++it)
		results.push_back((it->second)->get_name());
	return results;
}

// return a list of the names of all the current sounds that are not fading
Symbol_list_t Auditory_store::get_not_fading_sound_name_list() const
{
	Symbol_list_t results;
	for(auto it = sounds.begin(); it != sounds.end(); ++it) {
		if(!(it->second->get_fading()))
            results.push_back((it->second)->get_name());
        }
	return results;
}

// return a list of pointers to all the current sounds
Auditory_store::Sound_ptr_list_t Auditory_store::get_sound_ptr_list() const
{
	Sound_ptr_list_t results;
	for(Sound_container_t::const_iterator it = sounds.begin(); it != sounds.end(); ++it)
		results.push_back((it->second));
	return results;
}

// return a list of pointers to all the current sounds that are not fading
Auditory_store::Sound_ptr_list_t Auditory_store::get_not_fading_sound_ptr_list() const
{
	Sound_ptr_list_t results;
	for(auto it = sounds.begin(); it != sounds.end(); ++it) {
        if(!(it->second->get_fading()))
            results.push_back((it->second));
        }
	return results;
}

void Auditory_store::insert_new(shared_ptr<Auditory_sound> obj_ptr)
{
	const Symbol& obj_name = obj_ptr->get_name();
	Sound_container_t::iterator it = sounds.find(obj_name);
	if(it != sounds.end()) {
		throw Create_duplicate_auditory_sound(this, obj_name);
		}
	else
		sounds[obj_name] = obj_ptr;
}

// erase the specified object,throw an exception if not present
void Auditory_store::erase_from_sounds(const Symbol& obj_name)
{
	int n = int(sounds.erase(obj_name));
	if(n == 0)
		throw Unknown_auditory_sound(this, obj_name);
}


shared_ptr<Auditory_sound> Auditory_store::get_sound_ptr(const Symbol& obj_name) const
{
	Sound_container_t::const_iterator it;
	it = sounds.find(obj_name);
	if(it == sounds.end())
		throw Unknown_auditory_sound(this, obj_name);
	
	return it->second;
}

bool Auditory_store::is_sound_present(const Symbol& obj_name) const
{
	return sounds.find(obj_name) != sounds.end();
}

bool Auditory_store::is_not_fading_sound_present(const Symbol& obj_name) const
{
    auto it = sounds.find(obj_name);
    if(it == sounds.end())
        return false;
    return !(it->second->get_fading());
}



void Auditory_store::clear()
{
	streams.clear();
	sounds.clear();
}


void Auditory_store::display_contents(Output_tee& ot) const
{
	ot << processor_info() << ':' << endl;
	for(Stream_container_t::const_iterator it = streams.begin(); it != streams.end(); ++it)
		(it->second)->display_contents(ot);
	for(Sound_container_t::const_iterator it = sounds.begin(); it != sounds.end(); ++it)
		(it->second)->display_contents(ot);
}
