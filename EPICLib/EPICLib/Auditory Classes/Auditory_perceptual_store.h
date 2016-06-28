/* Auditory_perceptual_store contains streams and sounds with additional recoded properties.
These are available to the cognitive processor.  
By definition, perceptual objects are audible. They have location and properties, but 
only recoded properties are possed on to the cognitive processors. 

The main function provided here is the  time course of auditory memory once an object
disappears, it is retained for some time  in the  auditory_preceptual store. 

*/

#ifndef AUDITORY_PERCEPTUAL_STORE_H
#define AUDITORY_PERCEPTUAL_STORE_H

#include "Auditory_store.h"
#include "Human_subprocessor.h"
#include "Symbol.h"
#include "Geometry.h"
namespace GU = Geometry_Utilities;

#include <map>



class Auditory_perceptual_sound : public Auditory_sound {
public:
//	static Smart_Pointer<Auditory_sound> create(const Symbol& sound_name, const Symbol& stream_name, long time_stamp, const Symbol& timbre, 
	static std::shared_ptr<Auditory_sound> create(const Symbol& sound_name, const Symbol& stream_name, long time_stamp, GU::Point location, const Symbol& timbre,
			double loudness, long duration, long intrinsic_duration = 0)
		{return std::shared_ptr<Auditory_sound>(new Auditory_perceptual_sound(sound_name, stream_name, time_stamp, location, timbre, loudness, duration, intrinsic_duration));}
	// fatter interface for speech sounds
//	static std::shared_ptr<Auditory_sound> create(const Symbol& sound_name, const Symbol& stream_name, long time_stamp, GU::Point location,
//		const Symbol& timbre,
//		const Symbol& content, const Symbol& gender, const Symbol& speaker_id, int utterance_id,
//		double loudness, double pitch, long duration, long intrinsic_duration = 0)
//			{return new Auditory_perceptual_sound(sound_name, stream_name, time_stamp, timbre, content, gender, speaker_id, loudness, duration, intrinsic_duration);}
//			{return std::shared_ptr<Auditory_sound>(new Auditory_perceptual_sound(sound_name, stream_name, time_stamp, location, timbre, content, gender, speaker_id, utterance_id, loudness, pitch, duration, intrinsic_duration));}
	// smaller interface for speech sounds
	static std::shared_ptr<Auditory_sound> create(const Speech_word& word)
        {return std::shared_ptr<Auditory_sound>(new Auditory_perceptual_sound(word));}

	// access the list of disappearing properties
	virtual void set_disappearing_time(const Symbol& property_name, long disappearing_time);
	virtual void remove_disappearing_time(const Symbol& property_name);
	virtual long get_disappearing_time(const Symbol& property_name) const;
	
	enum Auditory_perceptual_sound_e {PRESENT, UNSUPPORTED, RETAINED};

	Auditory_perceptual_sound_e get_state() const
		{return state;}
	void set_state(Auditory_perceptual_sound_e state_)
		{state = state_;}

	void display_contents(Output_tee& ot) const;
	
private:
    // for simple sounds
	Auditory_perceptual_sound(const Symbol& sound_name, const Symbol& stream_name,  long time_stamp, GU::Point location,
		const Symbol& timbre,
		double loudness, long duration, long intrinsic_duration) :
		Auditory_sound(sound_name, stream_name, time_stamp, location, timbre, loudness, duration, intrinsic_duration),
		state(Auditory_perceptual_sound::PRESENT)
		{}
    
    Auditory_perceptual_sound(const Speech_word& word) :
        Auditory_sound(word),state(Auditory_perceptual_sound::PRESENT)
        {}

	Auditory_perceptual_sound_e state;
	// regular list of properties has those that are supported by perceptual input
	// the long variable here stores the time at which the state transition should occur;
	std::map<Symbol, long> unsupported_properties;	// list of properties no longer supported
	std::map<Symbol, long> retained_properties;		// list of properties being retained
	
	// no copy, assignment
	Auditory_perceptual_sound(const Auditory_perceptual_sound&) = delete;
	Auditory_perceptual_sound& operator= (const Auditory_perceptual_sound&) = delete;

};

class Auditory_perceptual_store : public Auditory_store {
public:
	Auditory_perceptual_store(Human_processor * human_ptr_) :
		Auditory_store("Auditory_perceptual_store", human_ptr_),
		change_decay_time("Change_decay_time", 75),
		unsupported_decay_delay("Unsupported_decay_delay_time", 200),
		retained_decay_delay("Retained_decay_delay_time", 1800),
		property_decay_time_delay("Property_decay_time", 500)
		{
			add_parameter(change_decay_time);
			add_parameter(unsupported_decay_delay);
			add_parameter(retained_decay_delay);
			add_parameter(property_decay_time_delay);
		}

//	virtual void initialize()

	// event interface
//	virtual void accept_event(const Start_event *);
//	virtual void accept_event(const Stop_event *);

	// these events arrive from the Perceptual_processor
	virtual void accept_event(const Auditory_event *);
	virtual void handle_event(const Auditory_Stream_Create_event *);
	virtual void handle_event(const Auditory_Stream_Destroy_event *);
	virtual void handle_event(const Auditory_Stream_Erase_event *);
	virtual void handle_event(const Auditory_Stream_Set_Location_event *);
	virtual void handle_event(const Auditory_Stream_Set_Pitch_event *);
	virtual void handle_event(const Auditory_Stream_Set_Loudness_event *);
	virtual void handle_event(const Auditory_Stream_Set_Size_event *);
	virtual void handle_event(const Auditory_Stream_Set_Property_event *);
	virtual void handle_event(const Auditory_Stream_Erase_Property_event *);
	virtual void handle_event(const Auditory_Sound_Start_event *);
	virtual void handle_event(const Auditory_Speech_Start_event *);
	virtual void handle_event(const Auditory_Sound_Stop_event *);
	virtual void handle_event(const Auditory_Sound_Erase_event *);
	virtual void handle_event(const Auditory_Sound_Set_Property_event *);
	virtual void handle_event(const Auditory_Sound_Erase_Property_event *);

	std::shared_ptr<Auditory_perceptual_sound> get_perceptual_object_ptr(const Symbol& name);
	void lose_stream_from_memory(std::shared_ptr<Auditory_stream> obj_ptr);
	void lose_sound_from_memory(std::shared_ptr<Auditory_perceptual_sound> obj_ptr);

//	void display_contents(Output_tee& ot) const;

private:
	// parameters
//	Parameter appearance_delay;
//	Parameter disappearance_delay;
//	Parameter location_delay;
//	Parameter size_delay;
	Parameter change_decay_time;
	Parameter unsupported_decay_delay;
	Parameter retained_decay_delay;
	Parameter property_decay_time_delay;

	// no default copy, assignment
	Auditory_perceptual_store(const Auditory_perceptual_store&);
	Auditory_perceptual_store& operator= (const Auditory_perceptual_store&);
};



#endif
