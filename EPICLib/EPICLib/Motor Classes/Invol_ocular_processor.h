/*
The Ocular_processor and Invol_ocular_processor interact with each other at execution.

The Ocular_processor handles voluntary eye movement execution as follows:
If an involuntary movement is underway, then execution of a voluntary movement
is deferred until the involuntary one is complete, whereupon the voluntary movement
is started. 

The Invol_ocular_processor handles involuntary eye movement execution as follows:
If a voluntary movement is underway, then the prepared involuntary movement is simply
discarded. 
When an involuntary movement completes, if a prepared voluntary movement is waiting,
then the Ocular_processor starts executing it.

This interaction is handled by overriding the "Template method" functions in Motor_processor
for execute_action() and complete_action().

*/

#ifndef INVOL_OCULAR_PROCESSOR_H
#define INVOL_OCULAR_PROCESSOR_H

#include "Motor_processor.h"
#include "Parameter.h"

class Invol_ocular_action;

class Invol_ocular_processor : public Motor_processor {
public:
	Invol_ocular_processor(Human_processor * human_ptr_) :
		Motor_processor("Invol_ocular", human_ptr_, false),	// no signals
		saccade_intercept("Involuntary_saccade_intercept", 21), 	// values from Carpenter(1988)
		saccade_slope("Involuntary_saccade_slope", 2.2),
		saccade_refractory_period("Involuntary_saccade_refactory_period", 10),
	// check these values
		smooth_move_intercept("Involuntary_smooth_move_intercept", 0),
		smooth_move_slope("Involuntary_smooth_move_slope", 2),
		smooth_move_refractory_period("Involuntary_smooth_move_refractory_period", 10)
		{setup();}
	
	virtual void initialize();

//	services for Ivol_ocular_actions
	friend class Invol_ocular_action;

protected:
	// override to allow discarding of action
	virtual void execute_action();
	// override to tell Ocular_processor that we are done
	virtual void complete_action();

private:

	// state

	// parameters
	Parameter saccade_intercept;
	Parameter saccade_slope;
	Parameter saccade_refractory_period;
	Parameter smooth_move_intercept;
	Parameter smooth_move_slope;
	Parameter smooth_move_refractory_period;
	
	// helper functions
	void setup();
	
	// no default copy, assignment
	Invol_ocular_processor(const Invol_ocular_processor&);
	Invol_ocular_processor& operator= (const Invol_ocular_processor&);
};



#endif
