#ifndef AUDITORY_STORE_PROCESSOR_H
#define AUDITORY_STORE_PROCESSOR_H

// this is a class of Human_subprocessor that simply have direct calls 
// for store updates to call added.


#include "Human_subprocessor.h"
#include "Geometry.h"
namespace GU = Geometry_Utilities;

class Symbol;

class Auditory_store_processor : public Human_subprocessor {
public:
	Auditory_store_processor(const std::string& name_, Human_processor * human_ptr_) :
		Human_subprocessor (name_, human_ptr_)
		{}
	virtual ~Auditory_store_processor()
		{}
	/* direct inputs - 
	   object name is supplied along with all relevant information - no backwards lookup necessary
	   returns true if a change was made to data base (and so input should be processed further) and false otherwise.
	   Some functions return the previous value, and do nothing unless it is different from the new value.
	*/
	// stream inputs
	virtual void create_stream(const Symbol& stream_name, GU::Point location, GU::Size size = GU::Size());
	virtual void destroy_stream(const Symbol& stream_name);
	virtual void erase_stream(const Symbol& stream_name);
	virtual GU::Point set_stream_location(const Symbol& stream_name, GU::Point location);
	virtual GU::Size set_stream_size(const Symbol& stream_name, GU::Size size);
	virtual Symbol set_stream_property(const Symbol& stream_name, const Symbol& propname, const Symbol& propvalue);

	// sound inputs
	virtual void make_sound_start(const Symbol& sound_name, const Symbol& stream_name, const Symbol& timbre, double loudness, long intrinsic_duration);
	virtual void make_speech_start(const Symbol& sound_name, const Symbol& stream_name, const Symbol& timbre, const Symbol& content, const Symbol& speaker_gender, const Symbol& speaker_id, 
		double loudness, long intrinsic_duration);
	virtual void make_sound_stop(const Symbol& sound_name);	
	virtual void make_sound_event(const Symbol& sound_name, const Symbol& stream_name, const Symbol& timbre, double loudness, long duration, long intrinsic_duration = 0);
	virtual void make_speech_sound_event(const Symbol& sound_name, const Symbol& stream_name, const Symbol& utterance, double loudness, long duration);
	virtual void erase_sound(const Symbol& sound_name);
	virtual Symbol set_sound_property(const Symbol& sound_name, const Symbol& propname, const Symbol& propvalue);

private:
	// no default copy, assignment
	Auditory_store_processor(const Auditory_store_processor&);
	Auditory_store_processor& operator= (const Auditory_store_processor&);


};


#endif
