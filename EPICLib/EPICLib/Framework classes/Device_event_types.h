#ifndef DEVICE_EVENT_TYPES_H
#define DEVICE_EVENT_TYPES_H

#include "Event_types.h"
#include "Symbol.h"
#include "Geometry.h"
namespace GU = Geometry_Utilities;

//forward declarations
class Device_processor;


class Device_Input_event : public Device_event {
protected:
	Device_Input_event(long t, Processor * d) : Device_event(t, d) {}
};

class Device_Output_event : public Device_event {
protected:
	Device_Output_event(long t, Processor * d) : Device_event(t, d) {}
};

// device delay message has multiple fields to allow the device to use this for a variety of
// purposes without having to redefine this class (hopefully)
class Device_Delay_event : public Device_event {
public:
	Device_Delay_event(long t, Processor * d) : 
		Device_event(t, d) {}
	Device_Delay_event(long t, Processor * d, const Symbol& tp, const Symbol& dn) : 
		Device_event(t, d), type (tp), datum(dn) {}
	Device_Delay_event(long t, Processor * d, 
		const Symbol& tp, const Symbol& on, const Symbol& pn, const Symbol& pv) : 
		Device_event(t, d), type (tp), object_name(on), property_name(pn), property_value(pv) {}
	virtual void handle_self(Device_processor *) const;
	Symbol type;
	Symbol datum;
	Symbol object_name;
	Symbol property_name;
	Symbol property_value;
};

class Device_Keystroke_event : public Device_Input_event {
public:
	Device_Keystroke_event(long t, Processor * d, const Symbol& k_n) : 
		Device_Input_event(t, d), key_name(k_n) {}
	virtual void handle_self(Device_processor *) const;
	Symbol key_name;
};

class Device_Type_In_event : public Device_Input_event {
public:
	Device_Type_In_event(long t, Processor * d, const Symbol& t_s) : 
		Device_Input_event(t, d), type_in_string(t_s) {}
	virtual void handle_self(Device_processor *) const;
	Symbol type_in_string;
};


class Device_Hold_event : public Device_Input_event {
public:
	Device_Hold_event(long t, Processor * d, const Symbol& b_n) : 
		Device_Input_event(t, d), button_name(b_n) {}
	virtual void handle_self(Device_processor *) const;
	Symbol button_name;
};

class Device_Release_event : public Device_Input_event {
public:
	Device_Release_event(long t, Processor * d, const Symbol& b_n) : 
		Device_Input_event(t, d), button_name(b_n) {}
	virtual void handle_self(Device_processor *) const;
	Symbol button_name;
};

class Device_Click_event : public Device_Input_event {
public:
	Device_Click_event(long t, Processor * d, const Symbol& b_n) : 
		Device_Input_event(t, d), button_name(b_n) {}
	virtual void handle_self(Device_processor *) const;
	Symbol button_name;
};

class Device_Double_Click_event : public Device_Input_event {
public:
	Device_Double_Click_event(long t, Processor * d, const Symbol& b_n) : 
		Device_Input_event(t, d), button_name(b_n) {}
	virtual void handle_self(Device_processor *) const;
	Symbol button_name;
};

// this class can be used for different pointing devices depending
// on the supplied names. The target may or may not be named, depending on the
// coupling between the device names and visual objects, The cursor name might not
// be used if only one pointing device is involved.
// The new location might be used if the target is named. But either the target name
// or the new location needs to be specified.
// In all cases, the arrival time of the event is the effective time of the movement.
class Device_Ply_event : public Device_Input_event {
public:
	Device_Ply_event(long t, Processor * d, const Symbol& cursor_name_, 
		const Symbol& target_name_, GU::Point new_location_, GU::Polar_vector movement_vector_) : 
		Device_Input_event(t, d), cursor_name(cursor_name_),
		target_name(target_name_), new_location(new_location_), movement_vector(movement_vector_) {}
	virtual void handle_self(Device_processor *) const;
	Symbol cursor_name;
	Symbol target_name;
	GU::Point new_location;
	GU::Polar_vector movement_vector;
};

class Device_Point_event : public Device_Input_event {
public:
	
	Device_Point_event(long t, Processor * r, const Symbol& t_s) : 
		Device_Input_event(t, r), target_name(t_s) {}

	virtual void handle_self(Device_processor *) const;

	Symbol target_name;
};

// The vocal input and the duration can be provided, so that when 
// the event is delivered at the time the input starts, the device
// can note the time when the input is finished.
class Device_Vocal_event : public Device_Input_event {
public:
	
	Device_Vocal_event(long t, Processor * r, const Symbol& v_s) : 
		Device_Input_event(t, r), vocal_input(v_s), loudness(0.), duration(0) {}
	Device_Vocal_event(long t, Processor * d, const Symbol& v_i, long dur) : 
		Device_Input_event(t, d), vocal_input(v_i), loudness(30.), duration(dur) {}
	Device_Vocal_event(long t, Processor * d, const Symbol& v_i, double loud, long dur) : 
		Device_Input_event(t, d), vocal_input(v_i), loudness(loud), duration(dur) {}
	virtual void handle_self(Device_processor *) const;
	Symbol vocal_input;
	double loudness;
	long duration;
};


// This event is transmitted to the device when the visual processor
// changes the visual object that is "in focus"
class Device_VisualFocusChange_event : public Device_Input_event {
public:
	Device_VisualFocusChange_event(long t, Processor * r, const Symbol& o_n) : 
		Device_Input_event(t, r), object_name(o_n) {}
	virtual void handle_self(Device_processor *) const;
	Symbol object_name;
};

// These classes are present to allow the device to track eye movements
class Device_Eyemovement_Start_event : public Device_Input_event {
public:
	Device_Eyemovement_Start_event(long t, Processor * d,  
		const Symbol& target_name_, GU::Point new_location_) : 
		Device_Input_event(t, d), 
		target_name(target_name_), new_location(new_location_) {}
	virtual void handle_self(Device_processor *) const;
	Symbol target_name;
	GU::Point new_location;
};

class Device_Eyemovement_End_event : public Device_Input_event {
public:
	Device_Eyemovement_End_event(long t, Processor * d,  
		const Symbol& target_name_, GU::Point new_location_) : 
		Device_Input_event(t, d), 
		target_name(target_name_), new_location(new_location_) {}
	virtual void handle_self(Device_processor *) const;
	Symbol target_name;
	GU::Point new_location;
};

// Get is an input to the device to send information to the human's task memory
class Device_HLGet_event : public Device_Input_event {
public:
	Device_HLGet_event(long t, Processor * r, 
			const Symbol_list_t & ps, const Symbol_list_t& vs, const Symbol& tg) : 
		Device_Input_event(t, r), props(ps), values(vs), tag(tg) {}
	virtual void handle_self(Device_processor *) const;
	Symbol_list_t props;
	Symbol_list_t values;
	Symbol tag;
};

class Device_HLPut_event : public Device_Input_event {
public:
	Device_HLPut_event(long t, Processor * r, 
			const Symbol_list_t & ps, const Symbol_list_t& vs) : 
		Device_Input_event(t, r), props(ps), values(vs) {}
	virtual void handle_self(Device_processor *) const;
	Symbol_list_t props;
	Symbol_list_t values;
};



#endif
