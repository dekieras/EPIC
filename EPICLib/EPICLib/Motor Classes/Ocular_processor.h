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


#ifndef OCULAR_PROCESSOR_H
#define OCULAR_PROCESSOR_H

#include "Motor_processor.h"
#include "Parameter.h"

class Ocular_action;

class Ocular_processor : public Motor_processor {
public:
	Ocular_processor(Human_processor * human_ptr) :
		Motor_processor("Ocular", human_ptr),
		// values from Carpenter(1988)
		saccade_duration_intercept("Voluntary_saccade_duration_intercept", 21.0),
		saccade_duration_slope("Voluntary_saccade_duration_slope", 2.2),

/*
		// values that disable gain and noise
		saccade_distance_intercept("Voluntary_saccade_distance_intercept", 0.0),
		saccade_distance_slope("Voluntary_saccade_distance_slope", 1.0),
		saccade_SD_intercept("Voluntary_saccade_SD_intercept", 0.0),
		saccade_SD_slope("Voluntary_saccade_SD_slope", 0.0)
*/
/*
		// values from Abrams, Meyer, Kornblum (1989)
		saccade_distance_intercept("Voluntary_saccade_distance_intercept", 0.32),
		saccade_distance_slope("Voluntary_saccade_distance_slope", 0.91),
		saccade_SD_intercept("Voluntary_saccade_SD_intercept", 0.55),
		saccade_SD_slope("Voluntary_saccade_SD_slope", 0.04)
*/

		// experimental values - 95% gain, sd is 10% of distance
		saccade_distance_intercept("Voluntary_saccade_distance_intercept", 0.0),
		saccade_distance_slope("Voluntary_saccade_distance_slope", 0.95),
		saccade_SD_intercept("Voluntary_saccade_SD_intercept", 0.0),
		saccade_SD_slope("Voluntary_saccade_SD_slope", 0.10)

		{setup();}
	
	virtual void initialize();

//	services for Ocular_actions
	friend class Ocular_action;
	
protected:
	// override to allow deferral of initiation
	virtual void execute_action();

private:
	// parameters
	Parameter saccade_duration_intercept;
	Parameter saccade_duration_slope;
	Parameter saccade_distance_intercept;
	Parameter saccade_distance_slope;
	Parameter saccade_SD_intercept;
	Parameter saccade_SD_slope;
	
	// helper functions
	void setup();
	
	// no default copy, assignment
	Ocular_processor(const Ocular_processor&);
	Ocular_processor& operator= (const Ocular_processor&);
};



#endif
