#ifndef AUDITORY_EVENT_TYPES_H
#define AUDITORY_EVENT_TYPES_H

#include "Event_types.h"
#include "Speech_word.h"
#include "Symbol.h"
#include "Geometry.h"
namespace GU = Geometry_Utilities;

// These events carry a single name, which is either physical or psychological name
// depending on the stream and target processor
/* disable 5/9/12
class Auditory_Stream_Create_event : public Auditory_event {
public:
	Auditory_Stream_Create_event(long t, Processor * r, const Symbol& nm, GU::Point loc, GU::Size sz) : 
		Auditory_event(t, r), stream_name(nm), location(loc), size(sz)
		{}
	virtual void handle_self(Auditory_store *) const;
	virtual void handle_self(Auditory_sensory_store *) const;
	virtual void handle_self(Auditory_perceptual_store *) const;
	Symbol stream_name;
	GU::Point location;
	GU::Size size;
};
*/

// Only the Auditory_perceptual processor and store does Auditory Stream objects - 6/16/15
class Auditory_Stream_Create_event : public Auditory_event {
public:
	Auditory_Stream_Create_event(long t, Processor * r, const Symbol& nm, double pitch_, double loudness_, GU::Point loc) : 
		Auditory_event(t, r), stream_name(nm), pitch(pitch_), loudness(loudness_), location(loc)
		{}
	virtual void handle_self(Auditory_store *) const;
//	virtual void handle_self(Auditory_sensory_store *) const;
	virtual void handle_self(Auditory_perceptual_store *) const;
	Symbol stream_name;
	double pitch;
	double loudness;
	GU::Point location;
	GU::Size size;
};

class Auditory_Stream_Destroy_event : public Auditory_event {
public:
	Auditory_Stream_Destroy_event(long t, Processor * r, const Symbol& nm) : 
		Auditory_event(t, r), stream_name(nm)
		{}
	virtual void handle_self(Auditory_store *) const;
//	virtual void handle_self(Ear_processor *) const;
//	virtual void handle_self(Auditory_sensory_store *) const;
    virtual void handle_self(Auditory_perceptual_processor *) const;
	virtual void handle_self(Auditory_perceptual_store *) const;
	Symbol stream_name;
};

class Auditory_Stream_Erase_event : public Auditory_event {
public:
	Auditory_Stream_Erase_event(long t, Processor * r, const Symbol& nm) : 
		Auditory_event(t, r), stream_name(nm)
		{}
	virtual void handle_self(Auditory_store *) const;
//	virtual void handle_self(Auditory_sensory_store *) const;
	virtual void handle_self(Auditory_perceptual_store *) const;
	Symbol stream_name;
};

class Auditory_Stream_Set_Location_event : public Auditory_event {
public:
	Auditory_Stream_Set_Location_event(long t, Processor * r, const Symbol& nm, GU::Point loc) : 
		Auditory_event(t, r), stream_name(nm), location(loc)
		{}
	virtual void handle_self(Auditory_store *) const;
//	virtual void handle_self(Auditory_sensory_store *) const;
	virtual void handle_self(Auditory_perceptual_store *) const;
	Symbol stream_name;
	GU::Point location;
};

class Auditory_Stream_Set_Pitch_event : public Auditory_event {
public:
	Auditory_Stream_Set_Pitch_event(long t, Processor * r, const Symbol& nm, double pitch_) : 
		Auditory_event(t, r), stream_name(nm), pitch(pitch_)
		{}
	virtual void handle_self(Auditory_store *) const;
//	virtual void handle_self(Auditory_sensory_store *) const;
	virtual void handle_self(Auditory_perceptual_store *) const;
	Symbol stream_name;
	double pitch;
};

class Auditory_Stream_Set_Loudness_event : public Auditory_event {
public:
	Auditory_Stream_Set_Loudness_event(long t, Processor * r, const Symbol& nm, double loudness_) : 
		Auditory_event(t, r), stream_name(nm), loudness(loudness_)
		{}
	virtual void handle_self(Auditory_store *) const;
//	virtual void handle_self(Auditory_sensory_store *) const;
	virtual void handle_self(Auditory_perceptual_store *) const;
	Symbol stream_name;
	double loudness;
};

class Auditory_Stream_Set_Size_event : public Auditory_event {
public:
	Auditory_Stream_Set_Size_event(long t, Processor * r, const Symbol& nm, GU::Size sz) : 
		Auditory_event(t, r), stream_name(nm), size(sz)
		{}
	virtual void handle_self(Auditory_store *) const;
//	virtual void handle_self(Auditory_sensory_store *) const;
	virtual void handle_self(Auditory_perceptual_store *) const;
	Symbol stream_name;
	GU::Size size;
private:
};

class Auditory_Stream_Set_Property_event : public Auditory_event {
public:
	Auditory_Stream_Set_Property_event(
		long t, Processor * r, const Symbol& nm, const Symbol& pn, const Symbol& pv) : 
		Auditory_event(t, r), stream_name(nm), property_name(pn), property_value(pv)
		{}
	virtual void handle_self(Auditory_store *) const;
//	virtual void handle_self(Auditory_sensory_store *) const;
	virtual void handle_self(Auditory_perceptual_store *) const;
	Symbol stream_name;
	Symbol property_name;
	Symbol property_value;
};

class Auditory_Stream_Erase_Property_event : public Auditory_event {
public:
	Auditory_Stream_Erase_Property_event(
		long t, Processor * r, const Symbol& nm, const Symbol& pn, const Symbol& pv) : 
		Auditory_event(t, r), stream_name(nm), property_name(pn), property_value(pv)
		{}
	virtual void handle_self(Auditory_store *) const;
//	virtual void handle_self(Auditory_sensory_store *) const;
	virtual void handle_self(Auditory_perceptual_store *) const;
	Symbol stream_name;
	Symbol property_name;
	Symbol property_value;
};

class Auditory_Sound_Start_event : public Auditory_event {
public:
	Auditory_Sound_Start_event(long t, Processor * r, const Symbol& nm, 
//		const Symbol& src, const Symbol& nl, const Symbol& tm, double loud, long intdur) : 
		const Symbol& src, long ts, GU::Point loc, const Symbol& tm, double loud, long intdur) : 
		Auditory_event(t, r), sound_name(nm), stream_name(src), time_stamp(ts), location(loc), timbre(tm), 
			loudness(loud), intrinsic_duration(intdur)
		{}
	virtual void handle_self(Auditory_store *) const;
	virtual void handle_self(Ear_processor *) const;
	virtual void handle_self(Auditory_sensory_store *) const;
    virtual void handle_self(Auditory_perceptual_processor *) const;
	virtual void handle_self(Auditory_perceptual_store *) const;
	Symbol sound_name;
	Symbol stream_name;
//	long time_stamp;
	long time_stamp;
	GU::Point location;
	Symbol timbre;
	double loudness;
	long intrinsic_duration;
};

class Auditory_Speech_Start_event : public Auditory_event {
public:
	Auditory_Speech_Start_event(long t, Processor * r, const Speech_word& word_) : 
		Auditory_event(t, r), word(word_)
		{}
	virtual void handle_self(Auditory_store *) const;
	virtual void handle_self(Ear_processor *) const;
	virtual void handle_self(Auditory_sensory_store *) const;
    virtual void handle_self(Auditory_perceptual_processor *) const;
	virtual void handle_self(Auditory_perceptual_store *) const;
	Speech_word word;
};


class Auditory_Sound_Stop_event : public Auditory_event {
public:
	Auditory_Sound_Stop_event(long t, Processor * r, const Symbol& nm) : 
		Auditory_event(t, r), sound_name(nm)
		{}
	virtual void handle_self(Auditory_store *) const;
	virtual void handle_self(Auditory_physical_store *) const;
	virtual void handle_self(Ear_processor *) const;
	virtual void handle_self(Auditory_sensory_store *) const;
    virtual void handle_self(Auditory_perceptual_processor *) const;
	virtual void handle_self(Auditory_perceptual_store *) const;
	Symbol sound_name;
};

class Auditory_Sound_Erase_event : public Auditory_event {
public:
	Auditory_Sound_Erase_event(long t, Processor * r, const Symbol& nm) : 
		Auditory_event(t, r), sound_name(nm)
		{}
	virtual void handle_self(Auditory_store *) const;
	virtual void handle_self(Auditory_sensory_store *) const;
	virtual void handle_self(Auditory_perceptual_store *) const;
	Symbol sound_name;
};

class Auditory_Sound_Set_Property_event : public Auditory_event {
public:
	Auditory_Sound_Set_Property_event(
		long t, Processor * r, const Symbol& nm, const Symbol& pn, const Symbol& pv) : 
		Auditory_event(t, r), sound_name(nm), property_name(pn), property_value(pv)
		{}
	virtual void handle_self(Auditory_store *) const;
	virtual void handle_self(Ear_processor *) const;
	virtual void handle_self(Auditory_sensory_store *) const;
    virtual void handle_self(Auditory_perceptual_processor *) const;
	virtual void handle_self(Auditory_perceptual_store *) const;
	Symbol sound_name;
	Symbol property_name;
	Symbol property_value;
};

class Auditory_Sound_Erase_Property_event : public Auditory_event {
public:
	Auditory_Sound_Erase_Property_event(
		long t, Processor * r, const Symbol& nm, const Symbol& pn, const Symbol& pv) : 
		Auditory_event(t, r), sound_name(nm), property_name(pn), property_value(pv)
		{}
	virtual void handle_self(Auditory_store *) const;
//	virtual void handle_self(Auditory_sensory_store *) const;
	virtual void handle_self(Auditory_perceptual_store *) const;
	Symbol sound_name;
	Symbol property_name;
	Symbol property_value;
private:
};

// for Vocal output to Auditory system
class Auditory_Speech_event : public Auditory_event {
public:
/*	Auditory_Speech_event(long t, Processor * r, const Symbol& sound_name_, const Symbol& stream_, const Symbol& content_, 
		const Symbol& speaker_gender_, const Symbol& speaker_id_,
		double loudness_, long duration_) : 
		Auditory_event(t, r), sound_name(sound_name_), stream_name(stream_), content(content_), 
			speaker_gender(speaker_gender_), speaker_id(speaker_id_), loudness(loudness_), duration(duration_)
		{} */
	Auditory_Speech_event(long t, Processor * r, const Speech_word& word_) : 
		Auditory_event(t, r), word(word_)
		{}
	virtual void handle_self(Auditory_store *) const;
	virtual void handle_self(Auditory_physical_store *) const;
//	virtual void handle_self(Ear_processor *) const;
//	virtual void handle_self(Auditory_perceptual_store *) const;
	Speech_word word;
/*	Symbol sound_name;
	Symbol stream_name;
	Symbol content;
	Symbol speaker_gender;
	Symbol speaker_id;
	double loudness;
	long duration; */
};

class Auditory_Delay_event : public Auditory_event {
public:
	Auditory_Delay_event(
		long t, Processor * r, const Symbol& nm, const Symbol& pn, const Symbol& pv) : 
		Auditory_event(t, r), object_name(nm), property_name(pn), property_value(pv)
		{}
	virtual void handle_self(Auditory_perceptual_processor *) const;
//	virtual void handle_self(Auditory_perceptual_store *) const;
	Symbol object_name;
	Symbol property_name;
	Symbol property_value;
};



#endif
