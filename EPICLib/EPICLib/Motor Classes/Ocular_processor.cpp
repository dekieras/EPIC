#include "Ocular_processor.h"
#include "Output_tee_globals.h"
#include "Invol_ocular_processor.h"
#include "Eye_processor.h"

#include <string>
#include <iostream>

using std::string;
using std::endl;

void Ocular_processor::initialize()
{
	Motor_processor::initialize();
//	set_randomize_when_used(true);
//	set_randomize(true);
//	randomize();
//	describe_parameters(Normal_out);
}

void Ocular_processor::setup()
{
	add_parameter(saccade_duration_intercept);
	add_parameter(saccade_duration_slope);
	add_parameter(saccade_distance_intercept);
	add_parameter(saccade_distance_slope);
	add_parameter(saccade_SD_intercept);
	add_parameter(saccade_SD_slope);
}

// this will cross check state of Invol_ocular processor
void Ocular_processor::execute_action()
{
	if(!(get_human_ptr()->get_Invol_ocular_processor_ptr()->get_execution_busy())) {
		// no involuntary movement underway, so go ahead and start this movement
		Motor_processor::execute_action();
		}
	else {
		// an involuntary movement is being executed, do not start this movement
		// invol_ocular processor will signal this processor when to go ahead
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << "action execution deferred" << endl;
		}
}


