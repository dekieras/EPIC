#include "Auditory_processor.h"
#include "Assert_throw.h"

// Define error-signaling definitions for the base class direct-input virtual functions - they
// should not be called on the wrong objects.

// stream inputs
void Auditory_processor::create_stream(const Symbol&, GU::Point, GU::Size) 
{
	Assert(!"create_stream called on wrong kind of object");
}

void Auditory_processor::destroy_stream(const Symbol&) 
{
	Assert(!"destroy_stream called on wrong kind of object");
}

void Auditory_processor::erase_stream(const Symbol&) 
{
	Assert(!"erase_stream called on wrong kind of object");
}

GU::Point Auditory_processor::set_stream_location(const Symbol&, GU::Point) 
{
	Assert(!"set_stream_location called on wrong kind of object"); 
	return GU::Point();
}

GU::Size Auditory_processor::set_stream_size(const Symbol&, GU::Size) 
{
	Assert(!"set_stream_size called on wrong kind of object"); 
	return GU::Size();
}

Symbol Auditory_processor::set_stream_property(const Symbol&, const Symbol&, const Symbol&) 
{
	Assert(!"set_stream_property called on wrong kind of object"); 
	return Symbol();
}

// sound inputs
	
// physical interface
void Auditory_processor::make_sound_start(const Symbol&, const Symbol&, const Symbol&, 
	double, long) 
{
	Assert(!"make_sound_start called on wrong kind of object");
}
void Auditory_processor::make_sound_event(const Symbol&, const Symbol&, const Symbol&, 
	double, long, long)
{
	Assert(!"make_sound_event called on wrong kind of object");
}

void Auditory_processor::make_speech_sound_event(const Speech_word&) 
{
	Assert(!"make_speech_sound_event called on wrong kind of object");
}

// psychological interface
void Auditory_processor::make_new_sound_start(const Symbol&, const Symbol&, long,
	const Symbol&, double, long) 
{
	Assert(!"make_new_sound_start called on wrong kind of object");
}

// psychological interface
void Auditory_processor::make_new_speech_start(const Speech_word&) 
{
	Assert(!"make_new_speech_start called on wrong kind of object");
}

void Auditory_processor::erase_sound(const Symbol&) 
{
	Assert(!"erase_sound called on wrong kind of object");
}

// both physical and psychological
void Auditory_processor::make_sound_stop(const Symbol&) 
{
	Assert(!"make_sound_stop called on wrong kind of object");
}	

Symbol Auditory_processor::set_sound_property(const Symbol&, const Symbol&, const Symbol&) 
{
	Assert(!"set_sound_property called on wrong kind of object"); return Symbol();
}

