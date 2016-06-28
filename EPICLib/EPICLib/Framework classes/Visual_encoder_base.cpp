#include "Visual_encoder_base.h"
#include "Visual_perceptual_processor.h"
//#include "Human_base.h"
//#include "Human_processor.h"
#include "Coordinator.h"
#include "Geometry.h"
//#include "Symbol_Geometry_utilities.h"
#include "Output_tee_globals.h"
//#include "Epic_standard_symbols.h"
//#include "Numeric_utilities.h"

#include <iostream>
#include <cassert>
#include <string>
#include <sstream>

using namespace std;
namespace GU =  Geometry_Utilities;


Visual_encoder_base::Visual_encoder_base(const string& id) :
	encoder_name(id), perceptual_proc_ptr(0)
{
}

Visual_encoder_base::~Visual_encoder_base()
{
}

void Visual_encoder_base::initialize()
{
	// make sure we are connected to the rest of the system
	Assert(perceptual_proc_ptr);
}


/* Event handling interface called by Visual_perceptual_processor
A returned value of true means that this function handled the encoding
and the default action of the Visual_perceptual processor should not be done.
These functions do nothing and return false in this class; override only those necessary.
*/

/*

bool Visual_encoder_base::handle_Start_event()
{
	return false;
}

bool Visual_encoder_base::handle_Stop_event()
{
	return false;
}


bool Visual_encoder_base::make_object_appear(const Symbol&, GU::Point, GU::Size)
{
	return false;
}


bool Visual_encoder_base::make_object_disappear(const Symbol&)
{
	return false;
}


bool Visual_encoder_base::set_object_location(const Symbol&, GU::Point)
{
	return false;
}


bool Visual_encoder_base::set_object_size(const Symbol&, GU::Size)
{
	return false;
}

*/

bool Visual_encoder_base::set_object_property(const Symbol&, const Symbol&, const Symbol&, long)
{
	return false;
}

bool Visual_encoder_base::handle_Delay_event(const Symbol&, const Symbol&, const Symbol&)
{
	return false;
}

/* Services for derived and related classes */

string Visual_encoder_base::processor_info() const
{
	ostringstream oss;
	oss << perceptual_proc_ptr->processor_info() << encoder_name << ':';
	return oss.str();
}

long Visual_encoder_base::get_time() const
{
	return Coordinator::get_instance().get_time();
}
	
bool Visual_encoder_base::get_trace() const
{
	return perceptual_proc_ptr->get_trace();
}
	
// access the value of the named parameter
long Visual_encoder_base::get_parameter_value(const std::string& parameter_name) const
{
	return perceptual_proc_ptr->get_parameter_ptr(parameter_name)->get_long_value();
}

double Visual_encoder_base::get_parameter_double_value(const std::string& parameter_name) const
{
	Parameter * parameter_ptr = perceptual_proc_ptr->get_parameter_ptr(parameter_name);
	Assert(parameter_ptr);
	return parameter_ptr->get_double_value();
}

Geometry_Utilities::Point Visual_encoder_base::get_object_location(const Symbol& object_name) const
{
    std::shared_ptr<Visual_sensory_object> obj_ptr = perceptual_proc_ptr->get_sensory_object_ptr(object_name);
    return obj_ptr->get_location();

}

Geometry_Utilities::Size Visual_encoder_base::get_object_size(const Symbol& object_name) const
{
    std::shared_ptr<Visual_sensory_object> obj_ptr = perceptual_proc_ptr->get_sensory_object_ptr(object_name);
    return obj_ptr->get_size();

}



// schedule an event to be sent to the Visual_perceptual_store at current time + recoding time
void Visual_encoder_base::schedule_change_property_event(long recoding_time, const Symbol& obj_name, const Symbol& propname, const Symbol& encoded_property_value)
{
	perceptual_proc_ptr->schedule_event(new Visual_Change_Property_event(
		get_time() + recoding_time, perceptual_proc_ptr->get_human_ptr()->get_visual_perceptual_store_ptr(),
		obj_name, propname, encoded_property_value));
}

// schedule a delayed and cancelable delivery of a property for an object to happen at current time + delay time, cancelable before then
void Visual_encoder_base::schedule_delayed_delivery_event(long delay_time, const Symbol& object_name, const Symbol& property_name, const Symbol& property_value)
{
	std::shared_ptr<Visual_sensory_object> obj_ptr = perceptual_proc_ptr->get_sensory_object_ptr(object_name);
	long event_time = delay_time + get_time();
	obj_ptr->set_property_event_time(property_name, event_time);
	// send an Visual_Delay_event to the perceptual processor
	perceptual_proc_ptr->schedule_event(new Visual_Delay_event(event_time, perceptual_proc_ptr, object_name, property_name, property_value));
}

// cancel a delayed delivery of a property for an object, if it has been scheduled
void Visual_encoder_base::cancel_delayed_delivery_event(const Symbol& object_name, const Symbol& property_name)
{
	std::shared_ptr<Visual_sensory_object> obj_ptr = perceptual_proc_ptr->get_sensory_object_ptr(object_name);
	if(obj_ptr->get_property_event_time(property_name) > 0)
		obj_ptr->set_property_event_time(property_name, 0); // zero means delivery is cancelled
}

void Visual_encoder_base::schedule_delay_event(long delay, const Symbol& object_name, const Symbol& property_name, const Symbol& property_value)
{	
	if(Trace_out && perceptual_proc_ptr->get_trace()) 
		Trace_out << processor_info() << " Schedule delay of " << delay << ' ' 
			<< object_name << ' ' << property_name << ' ' << property_value << endl;	
	Event * output_event_ptr = new Visual_Delay_event (
		get_time() + delay, perceptual_proc_ptr, object_name, property_name, property_value);
		
	perceptual_proc_ptr->schedule_event(output_event_ptr);		
}






