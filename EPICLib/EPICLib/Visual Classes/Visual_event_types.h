#ifndef VISUAL_EVENT_TYPES_H
#define VISUAL_EVENT_TYPES_H

#include "Event_types.h"
#include "Symbol.h"
#include "Geometry.h"
namespace GU = Geometry_Utilities;

// this event class carries both the physical name and the psychological name of the object
class Visual_Appear_event : public Visual_event {
public:
	Visual_Appear_event(long t, Processor * r, const Symbol& name_, GU::Point loc, GU::Size sz) : 
		Visual_event(t, r), name(name_),location(loc), size(sz)
		{}
//	virtual void handle_self(Eye_processor *) const;
	virtual void handle_self(Visual_sensory_store *) const;
//	virtual void handle_self(Visual_perceptual_processor *) const;
	virtual void handle_self(Visual_perceptual_store *) const;
	Symbol name;
	GU::Point location;
	GU::Size size;
};

class Visual_Disappear_event : public Visual_event {
public:
	Visual_Disappear_event(long t, Processor * r, const Symbol& nm) : 
		Visual_event(t, r), object_name(nm)
		{}
	virtual void handle_self(Eye_processor *) const;
	virtual void handle_self(Visual_sensory_store *) const;
//	virtual void handle_self(Visual_perceptual_processor *) const;
	virtual void handle_self(Visual_perceptual_store *) const;
	Symbol object_name;
};

class Visual_Erase_event : public Visual_event {
public:
	Visual_Erase_event(long t, Processor * r, const Symbol& nm) : 
		Visual_event(t, r), object_name(nm)
		{}
//	virtual void handle_self(Eye_processor *) const;
	virtual void handle_self(Visual_sensory_store *) const;
//	virtual void handle_self(Visual_perceptual_processor *) const;
	virtual void handle_self(Visual_perceptual_store *) const;
	Symbol object_name;
};

class Visual_Change_Location_event : public Visual_event {
public:
	Visual_Change_Location_event(long t, Processor * r, const Symbol& nm, GU::Point loc) : 
		Visual_event(t, r), object_name(nm), location(loc)
		{}
	virtual void handle_self(Eye_processor *) const;
	virtual void handle_self(Visual_sensory_store *) const;
//	virtual void handle_self(Visual_perceptual_processor *) const;
	virtual void handle_self(Visual_perceptual_store *) const;
	Symbol object_name;
	GU::Point location;
};

class Visual_Change_Size_event : public Visual_event {
public:
	Visual_Change_Size_event(long t, Processor * r, const Symbol& nm, GU::Size sz) : 
		Visual_event(t, r), object_name(nm), size(sz)
		{}
	virtual void handle_self(Eye_processor *) const;
	virtual void handle_self(Visual_sensory_store *) const;
//	virtual void handle_self(Visual_perceptual_processor *) const;
	virtual void handle_self(Visual_perceptual_store *) const;
	Symbol object_name;
	GU::Size size;
};

class Visual_Change_Property_event : public Visual_event {
public:
	Visual_Change_Property_event(
		long t, Processor * r, const Symbol& nm, const Symbol& pn, const Symbol& pv) : 
		Visual_event(t, r), object_name(nm), property_name(pn), property_value(pv)
		{}
	virtual void handle_self(Eye_processor *) const;
	virtual void handle_self(Visual_sensory_store *) const;
//	virtual void handle_self(Visual_perceptual_processor *) const;
	virtual void handle_self(Visual_perceptual_store *) const;
	Symbol object_name;
	Symbol property_name;
	Symbol property_value;
};

class Visual_Erase_Property_event : public Visual_event {
public:
	Visual_Erase_Property_event(
		long t, Processor * r, const Symbol& nm, const Symbol& pn, const Symbol& pv) : 
		Visual_event(t, r), object_name(nm), property_name(pn), property_value(pv)
		{}
//	virtual void handle_self(Eye_processor *) const;
	virtual void handle_self(Visual_sensory_store *) const;
//	virtual void handle_self(Visual_perceptual_processor *) const;
	virtual void handle_self(Visual_perceptual_store *) const;
	Symbol object_name;
	Symbol property_name;
	Symbol property_value;
};

class Visual_Delay_event : public Visual_event {
public:
	Visual_Delay_event(
		long t, Processor * r, const Symbol& nm, const Symbol& pn, const Symbol& pv) : 
		Visual_event(t, r), object_name(nm), property_name(pn), property_value(pv)
		{}
//	virtual void handle_self(Eye_processor *) const;
//	virtual void handle_self(Visual_sensory_store *) const;
	virtual void handle_self(Visual_perceptual_processor *) const;
//	virtual void handle_self(Visual_perceptual_store *) const;
	Symbol object_name;
	Symbol property_name;
	Symbol property_value;
};

#endif
