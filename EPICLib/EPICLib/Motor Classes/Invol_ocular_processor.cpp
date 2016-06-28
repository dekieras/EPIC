#include "Invol_ocular_processor.h"
#include "Output_tee_globals.h"
#include "Invol_ocular_actions.h"
#include "Ocular_processor.h"

#include <string>
#include <iostream>

using std::string;
using std::endl;


void Invol_ocular_processor::initialize()
{
	Motor_processor::initialize();
//	set_randomize_when_used(true);
//	set_randomize(true);
//	randomize();
//	describe_parameters(Normal_out);
}

void Invol_ocular_processor::setup()
{
	add_parameter(saccade_intercept);
	add_parameter(saccade_slope);	
	add_parameter(saccade_refractory_period);	
	add_parameter(smooth_move_intercept);
	add_parameter(smooth_move_slope);	
	add_parameter(smooth_move_refractory_period);	
}


// this will cross check state of Invol_ocular processor
void Invol_ocular_processor::execute_action()
{
	if(!(get_human_ptr()->get_Ocular_processor_ptr()->get_execution_busy())) {
		// no voluntary movement underway, so go ahead and start this movement
		Motor_processor::execute_action();
		}
	else {
		// a voluntary movement is being executed, do not start this movement
		// discard it because it is irrelevant once the movement is complete.
		// The eye processor will start a new movement at the right time.
		discard_prepared_movement();
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << "action execution discarded" << endl;
		}
}

/*// if the movement is complete, and another one is ready, start it.
// otherwise, processor is done
void Invol_ocular_processor::handle_event(const Motor_Movement_Complete_event * event_ptr)
{
	// tell Voluntary processor to start any ready action that it has prepared.
	get_human_ptr()->get_Ocular_processor_ptr()->start_next_executable_action();

	// then go ahead and finish this process, and consider starting the next movement
	// if appropriate
	Motor_processor::handle_event(event_ptr);
}
*/
void Invol_ocular_processor::complete_action()
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "Execution complete for involuntary action" << endl;
	
	// if the Ocular processor has something ready to go, then we need to dump anything we
	// have ready to go, finish up our movement, and tell the Ocular processor to go ahead
	if(get_human_ptr()->get_Ocular_processor_ptr()->action_ready_to_execute()) {
		if(action_ready_to_execute())	// if we have a movement ready, discard it
			discard_prepared_movement();
		Motor_processor::complete_action();	// finish our last movement
			
		// tell Voluntary processor to start the ready action that it has prepared.
		get_human_ptr()->get_Ocular_processor_ptr()->start_next_executable_action();
		}
	else {
		// go ahead and finish this process, and consider starting the next movement
		// if appropriate
		Motor_processor::complete_action();
		}
	
}

