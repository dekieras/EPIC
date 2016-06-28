#ifndef AUDITORY_STORE_H
#define AUDITORY_STORE_H

/*
An Auditory_store manages Auditory_store_objects, and has both direct and event interfaces to
create, destroy, and modify Auditory_store_objects.

All Auditory_store_objects have:
both psychological and physical names
location - but defaults to (0, 0)
properties

All Auditory_stores have a container of Auditory_store_objects.

Both Auditory_store and Auditory_stream are base classes.
*/

#include <map>
#include <set>
#include <memory>

#include "Auditory_processor.h"
#include "Auditory_event_types.h"
#include "Speech_word.h"
#include "Symbol.h"
#include "OPV_Containers.h"
#include "Geometry.h"
//#include "Output_tee.h"

namespace GU = Geometry_Utilities;

// struct Speech_word;

//class Auditory_store_processor;

/* Auditory_stream_object (Auditory_stream) is a base class for objects that can be in stores, that
have a location, size, and other_name properties.
Streams carry a physical name as well as a psychological name, but the physical name is present
only to support interaction with the device - e.g. if pointing to a stream, the physical name
can be sent to the device to simplify the device knowing what is being pointed to.
*/

class Auditory_stream {
public:
	static std::shared_ptr<Auditory_stream> create(const Symbol& name, double pitch, double loudness, GU::Point location)
				{return std::shared_ptr<Auditory_stream>(new Auditory_stream(name, pitch, loudness, location));}

	virtual ~Auditory_stream()
		{}
	const Symbol& get_name() const
		{return name;}
	void set_pitch(double pitch_)
		{pitch = pitch_;}
	double get_pitch() const
		{return pitch;}
	void set_loudness(double loudness_)
		{loudness = loudness_;}
	double get_loudness() const
		{return loudness;}
	void set_location(GU::Point location_)
		{location = location_;}
	GU::Point get_location() const
		{return location;}
//	void set_size(GU::Size size_)
//		{size = size_;}
//	GU::Size get_size() const
//		{return size;}
	long get_time_stamp() const
		{return time_stamp;}
	void set_time_stamp(long time_stamp_)
		{time_stamp = time_stamp_;}
		
	// if value is Symbol(), property is removed
	void set_property(const Symbol& property_name, const Symbol& property_value)
		{properties.store(property_name, property_value);}
	// if not present, Symbol() is returned
	Symbol get_property_value(const Symbol& property_name) const
		{return properties.retrieve(property_name);}
	// only properties with non-default values should be present
	void get_pv_lists(Symbol_list_t & prop_names, Symbol_list_t & prop_values) const
		{properties.get_pv_lists(prop_names, prop_values);}
		
	// Fat Interface for subtypes of Auditory_stream - these cause an error if called on the wrong kind of object
	virtual void set_disappearing_time(const Symbol& property_name, long disappearing_time);
	virtual void remove_disappearing_time(const Symbol& property_name);
	virtual long get_disappearing_time(const Symbol& property_name) const;
	virtual Symbol create_next_sound_name();
	virtual Symbol get_next_sound_name() const;
	
	void display_contents(Output_tee& ot) const;
	
protected:
	Symbol name;	// this will be the key for sorting these objects
	double pitch;
	double loudness;
	GU::Point location;
	GU::Size size;
	long time_stamp;
	PV_Container properties;
/* disabled 5/9/12
	Auditory_stream(const Symbol& name, GU::Point location, GU::Size size);
*/
	Auditory_stream(const Symbol& name, double pitch_, double loudness_, GU::Point location_);

private:
	// no default copy, assignment
	Auditory_stream(const Auditory_stream&);
	Auditory_stream& operator= (const Auditory_stream&);
	
};

// ordering relation between Smart_Pointer<Auditory_stream>
//inline bool operator< (Smart_Pointer<Auditory_stream> lhs, Smart_Pointer<Auditory_stream> rhs)
inline bool operator< (std::shared_ptr<Auditory_stream> lhs, std::shared_ptr<Auditory_stream> rhs)
		{return lhs->get_name() < rhs->get_name();}

/* Auditory_sound is a base class for sounds that can be in stores;
both speech sounds and non-speech sounds can be represented in terms
of fixed properties as well as arbitrary properties. 

Sounds do not carry both a physical and psychological name, because
they do not support interaction with a device.
*/

//class Auditory_sound : public Reference_Counted_Object {
class Auditory_sound {
public:
	// for simple sounds
	static std::shared_ptr<Auditory_sound> create(const Symbol& sound_name, const Symbol& stream_name, long time_stamp, GU::Point location, const Symbol& timbre,
			double loudness, long duration, long intrinsic_duration = 0);
	// smaller interface for speech sounds
	static std::shared_ptr<Auditory_sound> create(const Speech_word& word);

	virtual ~Auditory_sound()
		{}

	const Symbol& get_name() const
		{return name;}
	const Symbol& get_stream() const
		{return stream;}
	long get_time_stamp() const
		{return time_stamp;}
	GU::Point get_location() const
		{return location;}
	const Symbol& get_timbre() const
		{return timbre;}
	double get_loudness() const
		{return loudness;}		
	double get_level_right() const
		{return level_right;}
	double get_level_left() const
		{return level_left;}
	double get_pitch() const
		{return pitch;}		
	long get_duration() const
		{return duration;}		
	long get_intrinsic_duration() const
		{return intrinsic_duration;}		
	bool get_fading() const
		{return fading;}
	const Symbol& get_content() const
		{return content;}
	const Symbol& get_gender() const
		{return gender;}
	const Symbol& get_speaker_id() const
		{return speaker_id;}
	const int get_utterance_id() const
		{return utterance_id;}
		
	// if value is Symbol(), property is removed
	void set_property(const Symbol& property_name, const Symbol& property_value)
		{properties.store(property_name, property_value);}
	// if not present, Symbol() is returned
	Symbol get_property_value(const Symbol& property_name) const
		{return properties.retrieve(property_name);}
	// only properties with non-default values should be present
	void get_pv_lists(Symbol_list_t & prop_names, Symbol_list_t & prop_values) const
		{properties.get_pv_lists(prop_names, prop_values);}
		
	// Fat Interface for subtypes of Auditory_sound - these cause an error if called on the wrong kind of object
	virtual void set_disappearing_time(const Symbol& property_name, long disappearing_time);
	virtual void remove_disappearing_time(const Symbol& property_name);
	virtual long get_disappearing_time(const Symbol& property_name) const;

	void set_fading(bool fading_)
        {fading = fading_;}
	
	void display_contents(Output_tee& ot) const;
		
protected:
	Symbol name;	// this is key used for sorting
	Symbol stream;
	long time_stamp;
	GU::Point location;
	Symbol timbre;
	Symbol content;	// for speech
	Symbol gender;	// for speech
	Symbol speaker_id;	// for speech
    int utterance_id;   // for speech
	double loudness;
    double level_right;
    double level_left;
	double pitch;		// for fundamental frequency - assume 0 if no fundamental (e.g. noise)
	long duration;
	long intrinsic_duration;
    bool fading;   // true if object is fading, false if it is not - set by make_sound_stop
	PV_Container properties;

	// constructors
	// used for both simple sounds and speech, differing in whether pitch is provided
	Auditory_sound(const Symbol& name, const Symbol& stream, long time_stamp, GU::Point location, const Symbol& timbre,
			double loudness, long duration, long intrinsic_duration);
//	Auditory_sound(const Symbol& name, const Symbol& stream, long time_stamp, GU::Point location, const Symbol& timbre,
//			const Symbol& content, const Symbol& gender, const Symbol& speaker_id, int utterance_id_, double pitch, double loudness, double level_right_, double level_left, long duration, long intrinsic_duration);
    // simpler constructor for speech
    Auditory_sound(const Speech_word& word);
    
private:
	// no default copy, move
	Auditory_sound(const Auditory_sound&) = delete;
	Auditory_sound& operator= (const Auditory_sound&) = delete;
	
		
	
};

// ordering relation between Smart_Pointer<Auditory_sound>
//inline bool operator< (Smart_Pointer<Auditory_sound> lhs, Smart_Pointer<Auditory_sound> rhs)
inline bool operator< (std::shared_ptr<Auditory_sound> lhs, std::shared_ptr<Auditory_sound> rhs)
		{return lhs->get_name() < rhs->get_name();}


/* Auditory_store - stores Auditory_stream and Auditory_sound objects
This base class contains virtual functions that output trace information, update the storage,
inform the views, and then rely on an overriding wrapper to transmit the information as needed
to the next processor.
*/

class Auditory_store : public Auditory_processor {
public:
	Auditory_store(const std::string& name, Human_processor * human_ptr);
	virtual ~Auditory_store()
		{}

	virtual void initialize();
	
	/* direct inputs - 
	   object name (stream or sound) is supplied along with all relevant information
	   returns true if a change was made to data base (and so input should be processed further) and false otherwise.
	   Some functions return the previous value, and do nothing unless it is different from the new value.
	*/
	// stream inputs
	virtual void create_stream(const Symbol& stream_name, double pitch, double loudness, GU::Point location);
	virtual void destroy_stream(const Symbol& stream_name);
	virtual void erase_stream(const Symbol& stream_name);
	virtual GU::Point set_stream_location(const Symbol& stream_name, GU::Point location);
	virtual double set_stream_pitch(const Symbol& stream_name, double pitch_);
	virtual double set_stream_loudness(const Symbol& stream_name, double loudness_);
	virtual GU::Size set_stream_size(const Symbol& stream_name, GU::Size size);
	virtual Symbol set_stream_property(const Symbol& stream_name, const Symbol& propname, const Symbol& propvalue);

	// sound and speech inputs
	// physical interface for "fire and forget" events - no implementation in this class
	virtual void make_sound_event(const Symbol& name, const Symbol& stream, const Symbol& timbre, double loudness, long duration, long intrinsic_duration = 0);
//	virtual void make_speech_event(const Symbol& sound_name, const Symbol& stream_name, 
//		const Symbol& content, const Symbol& speaker_gender, const Symbol& speaker_id, double loudness, long duration);
	virtual void make_speech_event(const Speech_word&);

	// psychological interface -- note that sound_name, stream_name, and time_stamp come first for these functions, followed by sound-specific parameters
//	virtual void make_new_sound_start(const Symbol& sound_name, const Symbol& stream_name, long time_stamp, 
	virtual void make_new_sound_start(const Symbol& sound_name, const Symbol& stream_name, long time_stamp, GU::Point location,
		const Symbol& timbre, double loudness, long intrinsic_duration);
	// bigger interface for speech sounds, where intrinsic_duration is same as duration
//	virtual void make_new_speech_start(const Symbol& sound_name, const Symbol& stream_name, long time_stamp, 
//	virtual void make_new_speech_start(const Symbol& sound_name, const Symbol& stream_name, long time_stamp,
//			const Symbol& content, const Symbol& speaker_gender, const Symbol& speaker_id, double loudness, long duration);
	virtual void make_new_speech_start(const Speech_word&);
	virtual void erase_sound(const Symbol& sound_name);

	// both physical and psychological
	virtual void make_sound_stop(const Symbol& sound_name);	
	virtual Symbol set_sound_property(const Symbol& sound_name, const Symbol& propname, const Symbol& propvalue);

	/* event interface
		default action is to call the corresponding direct input function - override as needed
	*/
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
	virtual void handle_event(const Auditory_Speech_event *);	// for Vocal output to auditory system
	
	// services

	// streams are accessed via the obj_name; throw exception if not already present
	std::shared_ptr<Auditory_stream> get_stream_ptr(const Symbol& obj_name) const;
	// see if the object is present in the store; return true if so, false if not - no exception thrown
	bool is_stream_present(const Symbol& obj_name) const;
	// return a list of the obj_names of all the current objects
	Symbol_list_t get_stream_name_list() const;
	// return a list of pointers to all the current objects
	typedef std::list<std::shared_ptr<Auditory_stream> > Stream_ptr_list_t;
	Stream_ptr_list_t get_stream_ptr_list() const;

	// sounds are accessed via the obj_name; throw exception if not already present
	std::shared_ptr<Auditory_sound> get_sound_ptr(const Symbol& obj_name) const;
	// see if the object is present in the store; return true if so, false if not - no exception thrown
	bool is_sound_present(const Symbol& obj_name) const;
	// return a list of the obj_names of all the current objects
	Symbol_list_t get_sound_name_list() const;
	// return a list of pointers to all the current objects
	typedef std::list<std::shared_ptr<Auditory_sound> > Sound_ptr_list_t;
	Sound_ptr_list_t get_sound_ptr_list() const;

	// see if the object is present in the store and is not fading; return true if so, false if not - no exception thrown
	bool is_not_fading_sound_present(const Symbol& obj_name) const;
	// return a list of the obj_names of all the current objects
	Symbol_list_t get_not_fading_sound_name_list() const;
	// return a list of pointers to all the current objects
	Sound_ptr_list_t get_not_fading_sound_ptr_list() const;

	// output a description of all of the objects currently present
	virtual void display_contents(Output_tee& ot) const;

protected:	
	// state
	bool changed;	// true if data was updated due to input. // not clear what this is for - 6/17/15
	typedef std::map<Symbol, std::shared_ptr<Auditory_stream> > Stream_container_t;
	Stream_container_t streams;	// the stream storage
	typedef std::map<Symbol, std::shared_ptr<Auditory_sound> > Sound_container_t;
	Sound_container_t sounds;	// the sounds storage
	
	// helper functions
	// insert a new pointer to a new object, but throw an exception if already present
	void insert_new(std::shared_ptr<Auditory_stream> obj_ptr);
	void insert_new(std::shared_ptr<Auditory_sound> obj_ptr);
	// erase the specified object,throw an exception if not present
	void erase_from_streams(const Symbol& obj_name);
	void erase_from_sounds(const Symbol& obj_name);
	// erase the specified object,throw an exception if not present
	void erase(std::shared_ptr<Auditory_stream> obj_ptr);
	void erase(std::shared_ptr<Auditory_sound> obj_ptr);
	// empty the contents of both containers
	void clear();

private:

	// no default copy, assignment
	Auditory_store(const Auditory_store&) = delete;
	Auditory_store& operator= (const Auditory_store&) = delete;
};

#endif
