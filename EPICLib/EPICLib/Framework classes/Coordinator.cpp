#include "Coordinator.h"
#include "Processor.h"
#include "Event_types.h"
#include "Output_tee.h"
#include "Assert_throw.h"
#include "Epic_exception.h"
#include "Output_tee_globals.h"

#include <string>
#include <list>
#include <queue>
#include <algorithm>
#include <iostream>
#include <climits>
//#include <typeinfo>


using std::string;
using std::list;
using std::priority_queue;	using std::find;
using std::cout;	using std::endl;	using std::cerr;
using std::time_t;	using std::clock_t;	using std::time;	using std::clock;	using std::difftime;

long Coordinator::current_time = 0;

Coordinator::Coordinator() : state(UNREADY)
{}

// A Meyers singleton 
Coordinator & Coordinator::get_instance()
{
	static Coordinator the_coordinator;
	return the_coordinator;
}


/*** Simulation control interface ***/
// setup the simulation and initialize the processors
void Coordinator::initialize()
{
	current_time = 0;

	int counter = 0;
	while(!event_queue.empty()) {
		const Event * event_ptr = event_queue.top();
		// we need to delete the event
		delete event_ptr;
		event_queue.pop();
		counter++;
		}

	if(Normal_out && counter > 0)
		Normal_out << "Event queue had " << counter << " left-over events" << endl;

	
	initialize_processors();	// every processor should reset itself
	state = INITIALIZED;
	if(Normal_out)
		Normal_out << "Simulation initialized with " << processor_list.size() << " processors" << endl;
}

void Coordinator::run_until_done()
{
	run_for(LONG_MAX);
}

// will not work unless running set by setup routine
// outsider can turn off the run flag to halt the execution in a resumable state
// if run_duration is zero, run for maximum amount of time
/* 
State transitions are governed by the principle that if this function is called, the
caller wants the model to be run, doing whatever is necessary to start it up.
When this function returns, the state is either TIMED_OUT, PAUSED, or FINISHED.
When this function is entered, the state is either UNREADY, INITIALIZED, TIMED_OUT, PAUSED, or FINISHED.
When the main body of this function is being executed, the state might be STARTING, or RUNNING.
*/
void Coordinator::run_for(long run_duration)
{
	switch(state) {
		case UNREADY: case FINISHED: {
			initialize();
			// drop through
			}
		case INITIALIZED: {
			start_process_time = clock();
			start_wallclock_time = time(0);
			send_start_event();
			// drop through
			}
		case STARTED: case TIMED_OUT: case PAUSED: {
			state = RUNNING;
			break;
			}
		case RUNNING: {
			break;
			}
		default: {
			Assert(!"Improper state in Coordinator run_for!");
			}
		}
	
	long time_out_time = run_duration + current_time;
	Assert(time_out_time > current_time);	// should never be called with run_duration of 0

	const Event * event_ptr; Processor * proc_ptr; 

	// main event delivery loop - note later condition takes us out when the time is up
	// this ensures that all events at the time_out_time get dispatched.
	// this supersedes modification of DR 9/2/2005 - DK 5/9/06
	while (state == RUNNING && !event_queue.empty()) {
		event_ptr = event_queue.top();
		// peek at the next event time - have we processed all these events?
		if(event_ptr->get_time() > time_out_time) {
			// stop with time at the time_out value 
         	current_time = time_out_time;
			break;
			}

		event_queue.pop();
		// the current time is now that of the event to be sent
		current_time = event_ptr->get_time();
		proc_ptr = event_ptr->get_destination();
		event_ptr->send_self(proc_ptr);
		
		//done with this event - must destroy it here!		
		delete event_ptr;
		
		// if the current time is less than 1 sec from the maximum time, throw an exception
		if(LONG_MAX - current_time < 1000)
			throw Epic_exception(string("Epic Coordinator current time is within 1000 ms of maximum possible time"));
		
		}
		

	// if stop() was called or stopped because of no more events, 
	// set the finished flag
	if(state == FINISHED || event_queue.empty()) {

		state = FINISHED;
		// tell the processors everything is done
		shutdown();
		}
	// if paused by call to PAUSE function, do nothing further
	else if(state == PAUSED) {
		}
	// else, deliveries stopped due to running out of time
	else {
		state = TIMED_OUT;
		}
}


// this causes deliveries to be halted without a shutdown
void Coordinator::pause()
{
	state = PAUSED;
}

// tell the simulation to stop running, and set the finished flag to cause a shutdown
void Coordinator::stop()
{
	state = FINISHED;
}

/*** Event delivery interface ***/
// add a processor to coordinator's list of processors.
void Coordinator::add_processor(Processor * p)
{
	// should be unique
//	cout << p->get_name() << " adding" << endl;
	Assert(find(processor_list.begin(), processor_list.end(), p) == processor_list.end());
	processor_list.push_back(p);
}

void Coordinator::remove_processor(Processor * p)
{
	// should be unique
//	cout << p->get_name() << " removing" << endl;
	Assert(find(processor_list.begin(), processor_list.end(), p) != processor_list.end());
	processor_list.remove(p);
}

// puts a copy of event_ptr into the schedule queue, immediately returns to originator.
void Coordinator::schedule_event(const Event * event_ptr)
{
	event_queue.push(event_ptr); 
}

// Immediately call the processor with the event.
// Assumption is that the  processor will modify the event.
// Event is an object with caller's scope, since control returns immediately to caller.
void Coordinator::call_with_event(Event& msg)
{
	Event * event_ptr = &msg;
	Processor * proc_ptr = event_ptr->get_destination();
	Assert(proc_ptr);
	event_ptr->send_self(proc_ptr);
}

/* private helper functions */

// should be here only in FINISHED state - leave in FINISHED state
void Coordinator::shutdown()
{
//	if(Normal_out)
//		Normal_out << "Final Coordinator time is " << current_time << endl;
	send_stop_event();	// tell every processor events have ended
	
	std::clock_t stop_time = clock();
	std::time_t  stop_wallclock_time = time(0);
	double simulated_run_time = double(current_time) / 1000.;
	double process_run_time = double(stop_time - start_process_time) / CLOCKS_PER_SEC;
	double wallclock_run_time = difftime(stop_wallclock_time, start_wallclock_time);
	if(Normal_out) {
		Normal_out << "Simulation Done!" << endl;
		Normal_out << "Run time was " 
			<< simulated_run_time << " sec (simulated), " 
			<< process_run_time << " sec (process), " 
			<< wallclock_run_time << " sec (wallclock)" << endl;
		}
}

// Tell each processor to initialize itself by resetting all of its state variable to initial values. 
// If any processor throws any sort of exception, the initialization has failed.
// rethrow the exception so that somebody else can output it.
void Coordinator::initialize_processors()
{
	try{
		for (list<Processor *>::iterator it = processor_list.begin(); it != processor_list.end(); ++it) {
			Processor * proc_ptr = *it;
			proc_ptr->initialize();
			}
		state = INITIALIZED;
	}
	catch (...) {
		state = UNREADY;
		// rethrow the exception
		throw;
		}
}

// Tell each processor that the simulation events are starting. A processor can use
// this signal to generate any initial set of events that need to go to another processor.
void Coordinator::send_start_event()
{
	for (list<Processor *>::iterator it = processor_list.begin(); it != processor_list.end(); ++it) {
		Processor * proc_ptr = *it;
		Assert(proc_ptr);
		const Event * event_ptr = new Start_event(current_time, proc_ptr);
		event_ptr->send_self(proc_ptr);
		delete event_ptr;
		}
	state = STARTED;
}

void Coordinator::send_stop_event()
{
	// send a stop event to every processor, using the last current time.
	for (list<Processor *>::iterator it = processor_list.begin(); it != processor_list.end(); ++it) {
		Processor * proc_ptr = *it;
		Assert(proc_ptr);
		const Event * event_ptr = new Stop_event(current_time, proc_ptr);
		event_ptr->send_self(proc_ptr);
		delete event_ptr;
		}
}

