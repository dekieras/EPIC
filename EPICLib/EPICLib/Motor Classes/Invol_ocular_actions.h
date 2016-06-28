#ifndef INVOL_OCULAR_ACTIONS_H
#define INVOL_OCULAR_ACTIONS_H

/*
The Invol_ocular_action classes encapsulate how a particular type of involuntary ocular movement is 
prepared and executed. Each subclass defines what the preparation time is in relation
to the previously prepared movement, and the execution time for the movement.
The execute function is responsible for sending messages to the device and
any other special messages.

The controlling involuntary ocular motor processor is responsible for sequencing the movements
through waiting, preparation, ready, and executing stages. For each commanded movement,
it will create a new Invol_ocular_action object to store the movement information, and then
delete it when it is no longer needed.
*/

#include "Motor_action.h"
#include "Geometry.h"
namespace GU = Geometry_Utilities;
#include "Symbol.h"

// forward declaration
class Invol_ocular_processor;
class Eye_processor;
class Visual_sensory_store;
class Visual_perceptual_store;
class Auditory_perceptual_store;
class Parameter;
/*
Involuntary ocular command events always originate from another processor, 
e.g. the Eye, and never from a to-be-parsed input expression.
Therefore, this action class does not have a virtual constructor method 
because the type of the object to be constructed is defined at compile time.
It also does not have a dispatch method because it is not sent from the cognitive processor.
Invol_ocular actions always execute when prepared, so this parameter is not available in the ctor.
*/

// Abstract base class
class Invol_ocular_action : public Motor_action {
public:
	// initialize with pointer to related processors and whether overlappable
	Invol_ocular_action(Human_processor * human_ptr_, const Symbol& objname_);

	// this object will send itself to the appropriate processor at the specified time
	virtual void dispatch(long time);

	Invol_ocular_processor * get_processor_ptr() const
		{return proc_ptr;}
	Eye_processor * get_eye_ptr() const
		{return eye_ptr;}
	Visual_sensory_store * get_visual_sensory_store_ptr() const
		{return visual_sensory_store_ptr;}
	GU::Point get_obj_location() const
		{return obj_location;}
	GU::Polar_vector get_move_vector() const
		{return move_vector;}
	Symbol get_object_name() const
		{return objname;}
	Parameter& get_saccade_intercept() const;
	Parameter& get_saccade_slope() const;
	Parameter& get_saccade_refractory_period() const;
	Parameter& get_smooth_move_intercept() const;
	Parameter& get_smooth_move_slope() const;
	Parameter& get_smooth_move_refractory_period() const;

protected:
	// look up and return the physical name for the target object from its source
	Symbol get_target_object_device_name() const;
	void compute_movement_features();
	long compute_execution_time(long base_time);
	
private:
	Invol_ocular_processor * proc_ptr;	// pointer to the controlling motor processor
	Eye_processor * eye_ptr;	// pointer to the connected eye
	Visual_sensory_store * visual_sensory_store_ptr;	// pointer to the sensory store
	Visual_perceptual_store * visual_perceptual_store_ptr;	// pointer to the perceptual store
	Auditory_perceptual_store * auditory_perceptual_store_ptr;	// pointer to the perceptual store
	enum Object_source_e {NONE, NAMED_LOCATION, VISUAL_OBJECT, AUDITORY_SOURCE};
	Object_source_e object_source;
	Symbol objname;
	GU::Point obj_location;
	GU::Polar_vector move_vector;
	
};

// Specify a saccade to the object
class Invol_ocular_Saccade_action : public Invol_ocular_action {
public:
	Invol_ocular_Saccade_action(Human_processor * human_ptr_, const Symbol& objname_) :
		Invol_ocular_action(human_ptr_, objname_)
		{}
	virtual long prepare(long base_time);
	virtual long execute(long base_time);
};

// Specify a smooth movement to the object
class Invol_ocular_Smooth_move_action : public Invol_ocular_action {
public:
	Invol_ocular_Smooth_move_action(Human_processor * human_ptr_, const Symbol& objname_) :
		Invol_ocular_action(human_ptr_, objname_)
		{}
	virtual long prepare(long base_time);
	virtual long execute(long base_time);
};


#endif
