/*
This serves as the base class for a custom encoder object that the Auditory_perceptual_processor forwards 
events to in the the form of simple function calls. This class structure is intended to insulate
the coding of a specific encoder from the internal structure of simulation system as much as possible. 
Thus this class does not inherit from any class defined within the simulation system, and uses only a 
few utility classes from the simulation system.

The default behaviors defined in this implementation simply do nothing. Normally, a do-nothing
implementation should not need to be created or loaded into the simulation system.

Copy and assignment are forbidden.
*/

#ifndef AUDITORY_ENCODER_BASE_H
#define AUDITORY_ENCODER_BASE_H

#include "Symbol.h"
#include "Geometry.h"
#include "Output_tee.h"

#include <string>

class Auditory_perceptual_processor;

class Auditory_encoder_base {
public:
	// constructor with empty initial states
	Auditory_encoder_base(const std::string& id);
	virtual ~Auditory_encoder_base() = 0;	// abstract class
	
	const std::string& get_name() const {return encoder_name;}
	// called at simulation start-up time
	virtual void initialize();
	
	// must be called after construction to connect to the Auditory_perceptual_processor
	void connect(Auditory_perceptual_processor * perceptual_proc_ptr_) 
		{perceptual_proc_ptr = perceptual_proc_ptr_;}

	Auditory_perceptual_processor * get_perceptual_proc_ptr() const
		{return perceptual_proc_ptr;}
		
	/* Event handling interface called by Auditory_perceptual_processor
	A returned value of true means that this function handled the encoding
	and the default action of the Auditory_perceptual processor should not be done.
	These functions do nothing and return false in this class; override only those necessary.
	*/
/* - not supported at this time
	virtual bool handle_Start_event();
	virtual bool handle_Stop_event();
	virtual bool make_object_appear(const Symbol& name, GU::Point location, GU::Size size);
	virtual bool make_object_disappear(const Symbol& obj_name);
	virtual bool set_object_location(const Symbol& obj_name, GU::Point location);
	virtual bool set_object_size(const Symbol& obj_name, GU::Size size);
*/
	// the supplied encoding time is the available value for the property name
	// these functions by default return false unless overridden
	virtual bool set_object_property(const Symbol& object_name, const Symbol& property_name, const Symbol& property_value, long encoding_time);
	virtual bool handle_Delay_event(const Symbol& object_name, const Symbol& property_name, const Symbol& property_value);
	
	virtual GU::Point recode_location(GU::Point original_location);
	
protected:
	/* services for the derived classes */
	// override default to produce string containing output info
	virtual std::string processor_info() const;
	// get the current time
	long get_time() const;
	// get the trace state of the associated Auditory_perceptual_processor
	bool get_trace() const;
	// access the value of the named parameter
	long get_parameter_value(const std::string& parameter_name) const;
	double get_parameter_double_value(const std::string& parameter_name) const;
	
	// schedule an event to be sent to the Auditory_perceptual_store at current time + recoding time
	void schedule_change_property_event(long recoding_time, const Symbol& object_name, const Symbol& property_name, const Symbol& property_value);
	
	// These scheduled events use the default Perceptual_processor delay event handler
	// schedule a delayed and cancelable delivery of a property for an object to happen at current time + delay time, cancelable before then
	void schedule_delayed_delivery_event(long delay_time, const Symbol& object_name, const Symbol& property_name, const Symbol& property_value);
	// cancel a delayed delivery of a property for an object, if it has been scheduled
	void cancel_delayed_delivery_event(const Symbol& object_name, const Symbol& property_name);

	
	// Create a device delay event with the specified contents, to arrive at the current time + the delay.
	// The result is a call to the handle_Delay_event function at that time.
	void schedule_delay_event(long delay, const Symbol& object_name, const Symbol& property_name, const Symbol& property_value);
	
private:
	std::string encoder_name;
	Auditory_perceptual_processor * perceptual_proc_ptr; // the connected device processor interface to system
	
	Auditory_encoder_base(const Auditory_encoder_base&);
	Auditory_encoder_base& operator= (const Auditory_encoder_base&);
};


#endif
