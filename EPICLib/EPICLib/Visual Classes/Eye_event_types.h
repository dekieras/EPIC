#ifndef EYE_EVENT_TYPES_H
#define EYE_EVENT_TYPES_H

#include "Event_types.h"
#include "Symbol.h"
#include "Geometry.h"
namespace GU = Geometry_Utilities;

class Eye_Voluntary_Saccade_Start_event : public Eye_event {
public:
	Eye_Voluntary_Saccade_Start_event(long t, Processor * r, GU::Point nel, long dt) : 
		Eye_event(t, r), new_eye_location(nel), done_time(dt)
		{}
	virtual void handle_self(Eye_processor *) const;
	GU::Point new_eye_location;
	long done_time;
};


class Eye_Voluntary_Saccade_End_event : public Eye_event {
public:
	Eye_Voluntary_Saccade_End_event(long t, Processor * r, bool nc, bool nr) : 
		Eye_event(t, r), new_centering_mode(nc), new_reflex_mode(nr)
		{}
	virtual void handle_self(Eye_processor *) const;
	bool new_centering_mode;
	bool new_reflex_mode;
};

class Eye_Involuntary_Saccade_Start_event : public Eye_event {
public:
	Eye_Involuntary_Saccade_Start_event(long t, Processor * r, GU::Point nel, long dt) : 
		Eye_event(t, r), new_eye_location(nel), done_time(dt)
		{}
	virtual void handle_self(Eye_processor *) const;
	GU::Point new_eye_location;
	long done_time;
};

class Eye_Involuntary_Saccade_End_event : public Eye_event {
public:
	Eye_Involuntary_Saccade_End_event(long t, Processor * r) : 
		Eye_event(t, r)
		{}
	virtual void handle_self(Eye_processor *) const;
};

class Eye_Involuntary_Smooth_Move_Start_event : public Eye_event {
public:
	Eye_Involuntary_Smooth_Move_Start_event(long t, Processor * r, GU::Point nel, long dt) : 
		Eye_event(t, r), new_eye_location(nel), done_time(dt)
		{}
	virtual void handle_self(Eye_processor *) const;
	GU::Point new_eye_location;
	long done_time;
};

class Eye_Involuntary_Smooth_Move_End_event : public Eye_event {
public:
	Eye_Involuntary_Smooth_Move_End_event(long t, Processor * r) : 
		Eye_event(t, r)
		{}
	virtual void handle_self(Eye_processor *) const;
};


#endif
