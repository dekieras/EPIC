/* Temporal_processor.h 
 * Implements a timing mechanism in EPIC.
 * by Hornof and Halverson, March 2008
 *
 * Based on Motor_processor.h by Kieras, but...
 *
 * The Motor_processor class cycles a Motor_action through the phases of a motor movement.
 * Temporal_processor only needs to accept start or end events, and increment a temporal
 * WM item if there is one in WM.  It does not need to cycle through phases of events.
 *
 * See the Temporal_processor.cpp header comment for usage within EPIC.
 */

#ifndef TEMPORAL_PROCESSOR_H
#define TEMPORAL_PROCESSOR_H

#include "Human_subprocessor.h"
#include "Parameter.h"

#include <string>
#include <list>

class Temporal_processor : public Human_subprocessor {
public:
	Temporal_processor(Human_processor * human_ptr) :
		Human_subprocessor("Temporal", human_ptr),
		// Removed "0.0, Parameter::Fixed" from the end of the three arguement lists below after DK's Parameter.cpp fix in 2008.
		initial_time_increment("Initial_time_increment", 100),							// the "T sub zero" parameter
		increment_multiplier("Increment_multiplier", 1.02),								// the "a" parameter
		noise_multiplier("Noise_multiplier", 0.015)										// the "b" parameter
	{
		setup();
	}

	virtual void initialize();
	
	void start_command();
	void end_command();

	// inputs
	virtual void accept_event(const Start_event *);
	virtual void accept_event(const Temporal_event *); 


private:

	// Member variables

	// Parameters that can be set by the analyst.
	Parameter initial_time_increment;	// This is the "T sub zero" used in the time estimation equation,
										//		from p. 581 of Taatgen et al. (2007), in milliseconds.
	Parameter increment_multiplier;		// This is the "a" parameter
	Parameter noise_multiplier;			// This is the "b" parameter

	int numTicks;				// How long since temporal timing started, in ticks.
	double nextTickInterval;	// tn+1 and tn (see Taatgen et al.) depending on which side of the assignment it is on.
									// In ms.  Note that this gets rounded to an integer when actually used.
	long honorIncrementAtTime;	// In ms.  This identifies the next increment event that should be accepted.
								//         as opposed to events for a temporal sequence that has ended.
								//         This is also the same as tn+1 plus current time (see Taatgen et al.) 
	long last_command_time;		// The last time a start or end was called -- to make sure only one per cycle.


	// member functions used internally
	void check_for_two_events();
	void reset_processor();
	void increment_ticks();

	// helper functions
	void setup();	// Tells the human subprocessor about these parameters.

	// no default copy, assignment
	Temporal_processor(const Temporal_processor&);
	Temporal_processor& operator= (const Temporal_processor&);

};

#endif
