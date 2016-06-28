/*	
The coordinator schedules and delivers events.
To run the simulation, 
	1. call initialize to initialize the event queue and all the processors.
	2. call run_for repeatedly or run_until_done to run the simulation; 
		the start event is sent if required;
	3. to pause the simulation, call pause(); 
		to halt the simulation before it is done, call stop(); 
			actual halt is delayed by transmission of stop event.
		if it finishes by itself, it sends stop event, and halts.
			
The coordinator object is created in the UNREADY state.
When it and the processors are intialized, it goes into the INITIALIZED state.
While starting up, it is in the STARTING state.
While running, it is in the RUNNING state.
If it stops running before it is finished, it goes into the PAUSED state.
If the model completes, or it is told to halt, it goes into the FINISHED state.

*/

#ifndef COORDINATOR_H
#define COORDINATOR_H

#include "Event_base.h"	// need definition of Event, greater_Event_ptr

#include <list>
#include <vector>
#include <queue>
#include <ctime>

// forward declarations
class Processor;

class Coordinator {
public:
	// create if needed, and return the pointer to the singleton Coordinator
	static Coordinator & get_instance();	
	
	// get the universal time
	static long get_time()
		{return current_time;}
				
	/*** Simulation control interface ***/
	// setup the simulation and initialize the processors
	void initialize();
	// begin simulation execution, and continue until done or stopped
	void run_until_done();
	// run for the duration supplied, then return
	void run_for(long run_duration);
	// stop deliveries, but leave everything as is so that simulation can be restarted by a run_for call
	void pause();
	// stop the simulation unconditionally by setting the running to false; deliveries will stop, and processor shut down
	void stop();
	
	/* from the outside, the simulation is either not_ready, runnable, or finished.
		runnable means the run_for function can be called - 
			if always attempts to run the simulation,
				either initializing or starting it if necessary
				if initialization fails, an exception is thrown and the state is UNREADY
			run_for leaves the simulation in PAUSED or FINISHED state.
		finished means the simulation was shutdown either from the outside,
			or the model completed.
	*/
	bool is_not_ready() const
		{return (state == UNREADY);}
	bool is_runnable() const
		{return (state == INITIALIZED || state == TIMED_OUT || state == PAUSED || state == FINISHED);}
	bool is_timed_out() const
		{return (state == TIMED_OUT);} 
	bool is_paused() const
		{return (state == PAUSED);} 
	bool is_finished() const
		{return (state == FINISHED);} 
		
	/*** Event delivery interface ***/
	// add/remove a processor to/from the list of processors
	void add_processor (Processor *);
	void remove_processor (Processor *);
	// put an event into the schedule queue for later delivery.
	// event must be created with new because it will be deleted after delivery.
	void schedule_event(const Event *);
	// call the processor with a event and immediately return to originator processor.
	// this enables the recorders to notice the event even though function-call logic is actually involved.
	void call_with_event(Event&); 
	
private:
	static long current_time;	// the true universal current simulated time
	enum Simulation_state_e {UNREADY, INITIALIZED, STARTED, RUNNING, TIMED_OUT, PAUSED, FINISHED};
	Simulation_state_e state;	// state of simulation
	std::list<Processor *> processor_list;		// list of processors
	std::time_t start_wallclock_time;	// for process time output
	std::clock_t start_process_time;	// for process time output
	
	typedef std::priority_queue<const Event *, std::vector<const Event *>, greater_Event_ptr > Event_ptr_priority_queue_t;
	Event_ptr_priority_queue_t event_queue;	// schedule queue of events
	
	/* private helper functions */
	void initialize_processors();
	void send_start_event();
	void shutdown();
	void send_stop_event();
	
	Coordinator();
	Coordinator(const Coordinator&);	// no copy
	Coordinator& operator= (const Coordinator&);	// no assignment
};


#endif
