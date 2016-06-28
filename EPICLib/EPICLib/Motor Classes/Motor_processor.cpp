/* This class cycles a Motor_action through the phases of a Motor_c movement. It has
neither knowledge nor responsibility for choosing the Motor_action, or the effects
that the movement has on anything else in the Universe.
*/


/*
State indicators are signalled to the cognitive processor, and have the following meanings:

Modality is busy if there is a preparing action, ready & execute-when-prepared action, or executing action.
Processor is busy if there is a preparing action or a ready & execute-when-prepared-action.
Preparation is busy if there is a preparing action.
Execution is busy if there is an executing action.

Member functions here should always test action pointers.
The busy booleans are there only to govern whether a signal should be sent because of a state change.
*/


#include "Motor_processor.h"
#include "Output_tee_globals.h"
#include "Epic_standard_symbols.h"
#include "Motor_event_types.h"
#include "Cognitive_event_types.h"
#include "Motor_action.h"
#include "Assert_throw.h"

#include <iostream>
#include <typeinfo>

using std::endl;

class Motor_processor_error : public Epic_exception {
public:
Motor_processor_error(Processor * proc_ptr, const std::string& msg_) :
		Epic_exception(proc_ptr, msg_) {}
};


void Motor_processor::initialize()
{	
	Human_subprocessor::initialize();
	signalled_modality_busy = false;
	signalled_processor_busy = false;
	signalled_preparation_busy = false;
	signalled_execution_busy = false;
	cleanup();
}

// internal constants

// clean up any left-over actions
void Motor_processor::cleanup()
{
	preparing_action = 0;
	previous_prepared_action = 0;
	ready_action = 0;
	executing_action = 0;
}
// clean up any left-over actions
Motor_processor::~Motor_processor()
{
	cleanup();
}


bool Motor_processor::get_modality_busy() const
	{return (preparing_action || (ready_action && ready_action->get_execute_when_prepared()) || executing_action);}

bool Motor_processor::get_processor_busy() const
	{return (preparing_action || (ready_action && ready_action->get_execute_when_prepared()));}

bool Motor_processor::get_execution_waiting() const
	{return ready_action && ready_action->get_execute_when_prepared();}
		
// on start-up, tell cognitive processor initial processor state
void Motor_processor::accept_event(const Start_event *)
{
	if(!generate_signals)
		return;
	// set the state flag to the opposite to make these functions send their signals
	signalled_modality_busy = true;
	signal_modality_free();
	signalled_processor_busy = true;
	signal_processor_free();
	signalled_preparation_busy = true;
	signal_preparation_free();
	signalled_execution_busy = true;
	signal_execution_free();
}


void Motor_processor::accept_event(const Motor_event * event_ptr)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << typeid(*event_ptr).name() << " received" << endl;
	broadcast_to_recorders(event_ptr);
	// tell the event to handle itself with itself!
	event_ptr->handle_self(this);
}

void Motor_processor::handle_event(const Motor_Command_Action_event * event_ptr)
{
	// if preparation is busy, something has gone wrong - a Jam.
	if(preparing_action) 
		throw Motor_processor_error(this, "jammed when new command started");
	preparing_action = event_ptr->action;
	if(get_trace() && Trace_out) {
		Trace_out << processor_info() << "Preparation started for " << typeid(*preparing_action).name();
		if (!preparing_action->get_execute_when_prepared())
			Trace_out << ", execution deferred";
		Trace_out << endl;
		}
	// get preparation time
	long base_time = get_time();
	long preparation_time =  preparing_action->prepare(base_time);
	// if no time required for preparation, go directly to setup of prepared action
	if(preparation_time == base_time) {
		setup_prepared_action();
		}
	else {
		update_processor_state_signals();
		// signal self when preparation is done
		schedule_event(new Motor_Preparation_Complete_event(preparation_time, this));
		}
	
}



// preparation is complete
void Motor_processor::handle_event(const Motor_Preparation_Complete_event *)
{
	setup_prepared_action();
}

/* bug-hunt 3/24/04 The problem is whether we are distinguishing between executing a previously prepared action
and whether that previously prepared action is taking place with the correct timing.
*/
// setup the prepared action as the ready action and start execution if appropriate
void Motor_processor::setup_prepared_action()
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "Preparation complete for " << typeid(*preparing_action).name() << endl;
		
	// the previous action might have been marked as execution deferred, and if this is the same action, then
	// we are simply executing a previously prepared action.
	previous_prepared_action = preparing_action;
	// if there is a ready_action waiting, and it is not execution-deferred, 
	// it will get overwritten and discarded; if it is deferred, then we are probably executing a previously
	// prepared action - but not necessarily
	if(ready_action) {
		if(ready_action->get_execute_when_prepared()) {
			if(get_trace() && Trace_out)
				Trace_out << processor_info() << "Previous ready action " << typeid(*ready_action).name() << " overwritten" << endl;
				}
		else {
			if(get_trace() && Trace_out)
				Trace_out << processor_info() << "Previous ready action " << typeid(*ready_action).name() << " had execution deferred" << endl;
			}
		}
	ready_action = preparing_action;
	preparing_action = 0;
	// if nothing is being executing, and this action can go ahead, go ahead
	if(!executing_action && ready_action && ready_action->get_execute_when_prepared()) {
		execute_action();
		}
	// if this was a preparation only situation, we stop at this point with an update
	else {
		if(!(ready_action->get_execute_when_prepared())) {
			if(get_trace() && Trace_out)
				Trace_out << processor_info() << "Execution deferred for " << typeid(*ready_action).name() << endl;
			}
		update_processor_state_signals();
		}
}

/* following code is obsolete

// there is supposed to be a already prepared movement whose execution needs to be started 
void Motor_processor::handle_event(const Motor_Perform_Prepared_event *)
{
	if(!ready_action)
		throw Motor_processor_error(this, "attempt to perform non-existent previously prepared action.");

	if(!executing_action)
		execute_action();
	else 
		update_processor_state_signals();
}

// there is supposed to be a previously prepared movement whose execution needs to be repeated.
// there should be no preparation in progress
void Motor_processor::handle_event(const Motor_Perform_Previous_event *)
{
	if(!previous_prepared_action)
		throw Motor_processor_error(this, "attempt to repeat non-existent previous action.");
	if(preparing_action)
		throw Motor_processor_error(this, "attempt to repeat previous action while new one being prepared.");

	// make the ready action the same as the previously prepared one
	ready_action = previous_prepared_action;
	if(get_trace() && Trace_out) 
		Trace_out << processor_info() << "Perform repeat movement: " << typeid(*ready_action).name() << endl;

	// start it, or wait
	if(!executing_action)
		execute_action();
	else 
		update_processor_state_signals();
}
*/

// call this function to start an action that was prepared
//  but was deferred for some reason e.g. Invol_ocular will call this to give Vocular the go ahead.
void Motor_processor::start_next_executable_action()
{	
	// there is a ready action, go ahead and start it if execution is free
	// else do nothing (if no ready action, nothing must be happening.
	if(ready_action && !executing_action)
		execute_action();
}

// The ready_action is not going to be used, and so needs to be discared.
// But it should have been saved as the previous_prepared action.
// Sanity check, and then reset the ready action. The actual action object will be
// cleaned up later.
void Motor_processor::discard_prepared_movement()
{
	Assert(previous_prepared_action == ready_action);
	if(get_trace() && Trace_out) 
		Trace_out << processor_info() << "Prepared movement discarded: " << typeid(*ready_action).name() << endl;
	ready_action = 0;
	update_processor_state_signals();
}

void Motor_processor::execute_action()
{
	Assert(ready_action);	
	executing_action = ready_action;
	ready_action = 0;
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "Execution initiated for " << typeid(*executing_action).name() << endl;

	// send a message for any recorders
	// Message * msg_ptr = new Manual_Movement_Started_Message(facade_ptr->get_current_time(), 0, facade_ptr->get_broadcaster());
	// facade_ptr->schedule_message(msg_ptr);
	
	long base_time = get_time() + initiation_time.get_long_value();
	// execution update shold arrive at base time
	// save the movement completion time in case anybody needs it
	long movement_completion_time = executing_action->execute(base_time);
	// if base_time and movement_completion_time are the same, the movement was not done
// below commented out - DK 02/21/06 so that a move to the current fixation point
// takes 50 ms (initiation time) instead of no time to complete
/*	if(movement_completion_time == base_time) {
		if(get_trace() && Trace_out) 
			Trace_out << processor_info() << "No movement for " << typeid(*executing_action).name() << endl;
		complete_action();	// cleanup the action
		}
	// update the state and signal self for movement completion
	else {
		update_processor_state_signals();
		schedule_event(new Motor_Movement_Complete_event(movement_completion_time, this));
		}
*/
	if(movement_completion_time == base_time) {
		if(get_trace() && Trace_out) 
			Trace_out << processor_info() << "No movement for " << typeid(*executing_action).name() << endl;
		}
	// update the state and signal self for movement completion
	update_processor_state_signals();
	schedule_event(new Motor_Movement_Complete_event(movement_completion_time, this));

}

// default implementation of this function is to do nothing
void Motor_processor::handle_event(const Motor_Movement_Started_event *)
{
}

// if the movement is complete, and another one is ready, start it.
// otherwise, processor is done
void Motor_processor::handle_event(const Motor_Movement_Complete_event *)
{
	complete_action();
}

void Motor_processor::complete_action()
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "Execution complete for " << typeid(*executing_action).name() << endl;
	
	// call the final finish method for the action - note unconditional ...
	executing_action->finish();
	
// below unneeded with smart pointers
	// are we finished with this action object? if so, get rid of it
//	if (executing_action != previous_prepared_action) {
//		delete executing_action;
//		}
	executing_action.reset();
	
	// is there another movement ready, and should it be started? If so, start it executing
	if (ready_action && ready_action->get_execute_when_prepared()) {
		execute_action();
		}
	else 
		update_processor_state_signals();
}

void Motor_processor::update_processor_state_signals()
{
	if(!generate_signals)
		return;

	if(preparing_action || (ready_action && ready_action->get_execute_when_prepared()) || executing_action)
		signal_modality_busy();
	else
		signal_modality_free();
	
	if(preparing_action || (ready_action && ready_action->get_execute_when_prepared()))
		signal_processor_busy();
	else
		signal_processor_free();
		
	if(preparing_action)
		signal_preparation_busy();
	else
		signal_preparation_free();

	if(executing_action)
		signal_execution_busy();
	else
		signal_execution_free();
}

void Motor_processor::signal_modality_busy()
{
	if(!signalled_modality_busy) {
		signalled_modality_busy = true;
		schedule_event(new Cognitive_Update_event(
			get_time(), get_human_ptr()->get_cognitive_ptr(), Motor_c, modality_name, 
			Modality_c, Free_c, Busy_c));
		}
}

void Motor_processor::signal_modality_free()
{
	if(signalled_modality_busy) {
		signalled_modality_busy = false;
		schedule_event(new Cognitive_Update_event(
			get_time(), get_human_ptr()->get_cognitive_ptr(), Motor_c, modality_name, 
			Modality_c, Busy_c, Free_c));
		}
}

void Motor_processor::signal_processor_busy()
{
	if(!signalled_processor_busy) {
		signalled_processor_busy = true;
		schedule_event(new Cognitive_Update_event(
			get_time(), get_human_ptr()->get_cognitive_ptr(), Motor_c, modality_name, 
			Processor_c, Free_c, Busy_c));
	}
}	

void Motor_processor::signal_processor_free()
{
	if(signalled_processor_busy) {
		signalled_processor_busy = false;
		schedule_event(new Cognitive_Update_event(
			get_time(), get_human_ptr()->get_cognitive_ptr(), Motor_c, modality_name, 
			Processor_c, Busy_c, Free_c));
		}
}

void Motor_processor::signal_preparation_busy()
{
	if(!signalled_preparation_busy) {
		signalled_preparation_busy = true;
		schedule_event(new Cognitive_Update_event(
			get_time(), get_human_ptr()->get_cognitive_ptr(), Motor_c, modality_name, 
			Preparation_c, Free_c, Busy_c));
		}
}

void Motor_processor::signal_preparation_free()
{
	if(signalled_preparation_busy) {
		signalled_preparation_busy = false;
		schedule_event(new Cognitive_Update_event(
			get_time(), get_human_ptr()->get_cognitive_ptr(), Motor_c, modality_name, 
			Preparation_c, Busy_c, Free_c));
		}
}

void Motor_processor::signal_execution_busy()
{
	if(!signalled_execution_busy) {
		signalled_execution_busy = true;
		schedule_event(new Cognitive_Update_event(
			get_time(), get_human_ptr()->get_cognitive_ptr(), Motor_c, modality_name, 
			Execution_c, Free_c, Busy_c));
		}
}

void Motor_processor::signal_execution_free()
{
	if(signalled_execution_busy) {
		signalled_execution_busy = false;
		schedule_event(new Cognitive_Update_event(
			get_time(), get_human_ptr()->get_cognitive_ptr(), Motor_c, modality_name, 
			Execution_c, Busy_c, Free_c));
		}
}

