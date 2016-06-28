/* Temporal_processor.cpp
 * Implements a timing mechanism in EPIC.
 * by Hornof and Halverson, March 2008
 *
 * Usage within EPIC includes:
 * LHS
 *   To verify that there is no current temporal WM item.  (In other words, no ticks are currently being counted.)
 *     (Not (Temporal Ticks ?n))
 *   To retrieve the current number of ticks.
 *     (Temporal Ticks ?n)
 *   To test to make sure the current number of ticks is at least 8.
 *     (Temporal Ticks ?n)
 *     (Greater_than ?n 8)
 * RHS
 *   To start the tick counting.  If there are ticks currently being counted, reset to 0.
 *     (Send_to_temporal Start)
 *   To stop the counting and remove the WM item.
 *     (Send_to_temporal End)
 *
 * Based on Motor_processor.cpp by Kieras.  But Temporal_processor does not need to *cycle*
 * through the phases of a Motor_c movement. It just has to start, end, and increment.
 * Like Motor_processor, Temporal_processor has neither knowledge nor responsibility for choosing
 * the Motor_activity, or the effects that the activity has on anything else in the Universe.
 */

#include "Temporal_processor.h"
#include "Coordinator.h"
#include "Output_tee_globals.h"
#include "Epic_standard_symbols.h"
#include "Epic_exception.h"
#include "Cognitive_event_types.h"
#include "Assert_throw.h"
#include "Random_utilities.h"

#include <iostream>
using std::endl;
// using std::string;  // tmp

class Temporal_processor_error : public Epic_exception {
public:
Temporal_processor_error(Processor * proc_ptr, const std::string& msg_) :
		Epic_exception(proc_ptr, msg_) {}
};

// -------------------------------------------------------------------------------------

void Temporal_processor::initialize()
// This is where EPIC is telling all of its components to reset their state variables to initial values.
{	
	Human_subprocessor::initialize();
	
	// Initialize all member variables.
	numTicks = -1;	// initialized to an impossible time.
	nextTickInterval = 0.0;
	honorIncrementAtTime = -1;
	last_command_time = -1;
}

// -------------------------------------------------------------------------------------

void Temporal_processor::setup()
// Tell the human subprocessor about these parameters.
{
	add_parameter(initial_time_increment);
	add_parameter(increment_multiplier);
	add_parameter(noise_multiplier);
}

// -------------------------------------------------------------------------------------

void Temporal_processor::accept_event(const Start_event *)
// This relates to how, when EPIC starts a simulation, initial sets of events can be sent to processors.
// This can be seen in Coordinator::send_start_event()
{
}

// -------------------------------------------------------------------------------------

void Temporal_processor::start_command()
// Start the "timer" time estimation process based on a (Send_to_temporal Start) PPS command.
// Called directly by the Command_interface.cpp
//	If a start action is received while a count is ongoing, the count will restart from the beginning.
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << " received start_command" << endl;

	check_for_two_events(); // In case multiple commands in a single cycle.
	reset_processor();
	honorIncrementAtTime = get_time();	// Honor the increment request of the current start time.
	increment_ticks();
}

// -------------------------------------------------------------------------------------

void Temporal_processor::end_command()
// End the timing episode based on a (Send_to_temporal End) PPS command.
// Called directly by the Command_interface.cpp
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << " received end_command" << endl;

	check_for_two_events(); // In case multiple commands in a single cycle.
	reset_processor();
}

// -------------------------------------------------------------------------------------

void Temporal_processor::accept_event(const Temporal_event * event_ptr)
//  Increment_command
//	Accept an event for an 'increment' action.  Increment a time estimation in progress.
// 'Start' and 'End' actions were intercepted in Command_interface.cpp, though processed in methods below.
// (Replaced typeid(*event_ptr).name() with "Temporal increment event")
{	
	if(get_trace() && Trace_out)
	{
		Trace_out << processor_info() << " Temporal increment received" << endl;
	}

	// Action is to increment a time estimation in progress
	increment_ticks();
}

// -------------------------------------------------------------------------------------

void Temporal_processor::check_for_two_events()
// Check if an event was already called for this cycle and, if so, throw an error.
// Currently only used for start and end, not for increment.
{
	if(last_command_time == get_time())
		throw Temporal_processor_error(this, "jammed when multiple Send_to_temporal commands received in same cycle.");
	last_command_time = get_time();		// The "last" time a start or end was called.
}

// -------------------------------------------------------------------------------------

void Temporal_processor::reset_processor()
// Reset - Put everything back to its initial state and remove any Temporal WM items.
{

	// Remove the ticks from WM
	// Construct a representation of the time in WM with the form: (Temporal Ticks <numTicks>)
	// So that we can delete it.
	if (numTicks >= 0)	// If numTicks < 0 then there is no WM item to remove.
	{
		Symbol_list_t removal_description;	// Delete the WM item
		removal_description.push_back(Temporal_c);
		removal_description.push_back(Ticks_c);
		removal_description.push_back(Symbol(numTicks));
		Coordinator::get_instance().schedule_event(new Cognitive_Delete_Clause_event(get_time(),
			get_human_ptr()->get_cognitive_ptr(), removal_description));
	}

	//	Indicate that there are no more "good" increment events currently in the queue.
	honorIncrementAtTime = -1; 

	//	initial_time_increment is the parameter for the initial tick interval, "T sub zero".
	nextTickInterval = initial_time_increment.get_long_value();

	// This gets incremented with the increment_ticks() that immediately follows.
	// Zero ticks is reserved for when the the counting starts. 
	numTicks = -1;
}

// -------------------------------------------------------------------------------------

void Temporal_processor::increment_ticks()
// Increment the tick counter.  This is where you add the noise.
{
	if(get_time() == honorIncrementAtTime)
	{	
		// *** Remove the old ticks from WM. ***
		
		// Construct a representation of the time in WM with the form: (Temporal Ticks <numTicks>)
		// So that we can delete it.
		if (numTicks >= 0)	// If numTicks < 0 then there is no WM item to remove.
		{
			Symbol_list_t removal_description;
			removal_description.push_back(Temporal_c);
			removal_description.push_back(Ticks_c);
			removal_description.push_back(Symbol(numTicks));
			Coordinator::get_instance().schedule_event(new Cognitive_Delete_Clause_event(get_time(),
				get_human_ptr()->get_cognitive_ptr(), removal_description));
		}
	
		// *** Add the new ticks to WM. ***

		//	Increment the tick count
		numTicks++;

		//	Add the number of ticks to WM
		Symbol_list_t add_description;
		add_description.push_back(Temporal_c);
		add_description.push_back(Ticks_c);
		add_description.push_back(Symbol(numTicks));
		Coordinator::get_instance().schedule_event(new Cognitive_Add_Clause_event(get_time(),
			get_human_ptr()->get_cognitive_ptr(), add_description));

		// *** Schedule the next tick to add to WM. ***

		// Calculate the *next* tick interval
		// tn+1 = (alpha tn) + noise ; (0 needs to be replaced with noise)
		//		See equation on p. 581 of Taatgen et al. (2007).
		double a_tn = increment_multiplier.get_double_value() * nextTickInterval;
		double noise = normal_random_variable(0, noise_multiplier.get_double_value() * a_tn);
		nextTickInterval = a_tn + noise;
				
		//	Calculate the time of the next tick
		honorIncrementAtTime = get_time() + round(nextTickInterval);

		//	Schedule the next tick
		get_human_ptr()->schedule_event(new Temporal_event(honorIncrementAtTime, this));
	}
	else
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << " ignored a tick update for a previously-ended temporal command." << endl;
}

