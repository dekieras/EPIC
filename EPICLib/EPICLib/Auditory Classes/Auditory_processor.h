#ifndef AUDITORY_PROCESSOR_H
#define AUDITORY_PROCESSOR_H

/* Auditory_processor
This class defines the interface for the auditory processors and auditory store processors,
which have mixture of direct inputs and event inputs.
This class defines the interface as a set of empty virtual functions that the 
subclasses override as they require.

*/

#include "Human_subprocessor.h"
#include "Geometry.h"
//#include "Smart_Pointer.h"
#include "Symbol.h"
#include "Auditory_event_types.h"
#include "Speech_word.h"

namespace GU = Geometry_Utilities;

class Auditory_processor : public Human_subprocessor {
public:
	Auditory_processor(const std::string& name_, Human_processor * human_ptr_) :
		Human_subprocessor (name_, human_ptr_)
		{}

	/* direct inputs - 
	   object name is supplied along with all relevant information - no backwards lookup necessary
	   returns true if a change was made to data base (and so input should be processed further) and false otherwise.
	   Some functions return the previous value, and do nothing unless it is different from the new value.
	*/
	// stream inputs
	virtual void create_stream(const Symbol&, GU::Point, GU::Size size = GU::Size());
	virtual void destroy_stream(const Symbol&);
	virtual void erase_stream(const Symbol&);
	virtual GU::Point set_stream_location(const Symbol&, GU::Point);
	virtual GU::Size set_stream_size(const Symbol&, GU::Size);
	virtual Symbol set_stream_property(const Symbol&, const Symbol&, const Symbol&);

	// sound inputs
	
	// physical interface
	virtual void make_sound_start(const Symbol& name, const Symbol& stream, const Symbol& timbre, double loudness, long intrinsic_duration);
	virtual void make_sound_event(const Symbol& name, const Symbol& stream, const Symbol& timbre, double loudness, long duration, long intrinsic_duration = 0);
	virtual void make_speech_sound_event(const Speech_word& word);

	// psychological interface -- note that sound_name, stream_name, and time_stamp come first for these functions, followed by sound-specific parameters
	virtual void make_new_sound_start(const Symbol& sound_name, const Symbol& stream_name, long time_stamp, 
		const Symbol& timbre, double loudness, long intrinsic_duration);
//	virtual void make_new_speech_start(const Symbol& sound_name, const Symbol& stream_name, long time_stamp, 
//		const Symbol& content, const Symbol& speaker_gender, const Symbol& speaker_id, double loudness, long duration);
	virtual void make_new_speech_start(const Speech_word& word);

	// both physical and psychological
	virtual void make_sound_stop(const Symbol& sound_name);
	virtual Symbol set_sound_property(const Symbol& sound_name, const Symbol& propname, const Symbol& propvalue);
	virtual void erase_sound(const Symbol& sound_name);

	/* event interface - defined as empty by default
	*/
	virtual void accept_event(const Start_event *) {}
	virtual void accept_event(const Stop_event *) {}
	virtual void accept_event(const Auditory_event *) {}
	virtual void handle_event(const Auditory_Stream_Create_event *) {}
	virtual void handle_event(const Auditory_Stream_Destroy_event *) {}
	virtual void handle_event(const Auditory_Stream_Erase_event *) {}
	virtual void handle_event(const Auditory_Stream_Set_Location_event *) {}
	virtual void handle_event(const Auditory_Stream_Set_Size_event *) {}
	virtual void handle_event(const Auditory_Stream_Set_Property_event *) {}
	virtual void handle_event(const Auditory_Stream_Erase_Property_event *) {}
	virtual void handle_event(const Auditory_Sound_Start_event *) {};
	virtual void handle_event(const Auditory_Speech_Start_event *) {};
	virtual void handle_event(const Auditory_Sound_Stop_event *) {}
	virtual void handle_event(const Auditory_Sound_Erase_event *) {}
	virtual void handle_event(const Auditory_Sound_Set_Property_event *) {}
	virtual void handle_event(const Auditory_Sound_Erase_Property_event *) {}
	virtual void handle_event(const Auditory_Speech_event *) {}
	virtual void handle_event(const Auditory_Delay_event *) {}

private:
	// no default copy, assignment
	Auditory_processor(const Auditory_processor&);
	Auditory_processor& operator= (const Auditory_processor&);
};


#endif
