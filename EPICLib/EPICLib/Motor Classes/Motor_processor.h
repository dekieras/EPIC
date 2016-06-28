/* This class cycles a Motor_action through the phases of a motor movement. It has
neither knowledge nor responsibility for choosing the Motor_action, or the effects
that the movement has on anything else in the Universe.
*/

#ifndef MOTOR_PROCESSOR_H
#define MOTOR_PROCESSOR_H


#include "Human_subprocessor.h"
#include "Parameter.h"
#include "Motor_event_types.h"

#include <string>
#include <list>
#include <memory>

class Motor_action;

class Motor_processor : public Human_subprocessor {
public:
	Motor_processor(const std::string& name_, Human_processor * human_ptr_, bool generate_signals_ = true) :
		Human_subprocessor(name_, human_ptr_), modality_name(Symbol(name_)),
		generate_signals(generate_signals_),
		signalled_modality_busy(false), signalled_processor_busy(false), 
		signalled_preparation_busy(false), signalled_execution_busy(false),
		movement_completion_time(0),
// dk 090807
//		feature_time("Feature_time", 50),
		feature_time("Feature_time for non-aimed movements", 50),
		initiation_time("Initiation_time", 50),
		burst_time("Burst_time", 100),
		efferent_deletion_delay("Efferent_deletion_delay", 75),
		execution_fluctuation("Execution_fluctuation_factor", Parameter::Normal, Parameter::Never, 1.0, .5)
		{
			add_parameter(initiation_time);
			add_parameter(feature_time);
			add_parameter(burst_time);
			add_parameter(efferent_deletion_delay);
			add_parameter(execution_fluctuation);
		}
	// must delete any leftover Motor_action objects
	~Motor_processor() = 0;  // this is an abstract class
	
	virtual void initialize();

	// inputs
	virtual void accept_event(const Start_event *);
//	virtual void accept_event(const Stop_event *);
	virtual void accept_event(const Motor_event *); 
	void handle_event(const Motor_Command_Action_event *);
	void handle_event(const Motor_Preparation_Complete_event *);
/*
	void handle_event(const Motor_Perform_Prepared_event *);
	void handle_event(const Motor_Perform_Previous_event *);
*/
	void handle_event(const Motor_Movement_Started_event *);
	void handle_event(const Motor_Movement_Complete_event *);
	
	// call this function to start an action that was prepared
	// but was deferred for some reason
	void start_next_executable_action();
	void discard_prepared_movement();
		
	// state accessors
//	bool modality_waiting() const
//		{return waiting_action;}
	bool get_modality_busy() const;
	bool get_modality_free() const
		{return !get_modality_busy();}
	bool get_processor_busy() const;
	bool get_processor_free() const
		{return !get_processor_busy();}
	bool get_preparation_busy() const
		{return static_cast<bool>(preparing_action);}
	bool get_preparation_free() const
		{return !get_preparation_busy();}
	bool get_execution_busy() const
		{return static_cast<bool>(executing_action);}
	bool get_execution_free() const
		{return !get_execution_busy();}
		
	bool get_execution_waiting() const;
	long get_movement_completion_time() const
		{return movement_completion_time;}
	// return true if an action is ready to start executing
	bool action_ready_to_execute() const
		{return static_cast<bool>(ready_action);}
		
	// services for Motor_action classes
	long get_feature_time()
		{return feature_time.get_long_value();}
	long get_initiation_time()
		{return initiation_time.get_long_value();}
	long get_burst_time()
		{return burst_time.get_long_value();}
	long get_efferent_deletion_delay()
		{return efferent_deletion_delay.get_long_value();}
	double get_execution_fluctuation()
		{return execution_fluctuation.get_double_value();}
	std::shared_ptr<Motor_action> get_previous_ptr() const
		{return previous_prepared_action;}
	std::shared_ptr<Motor_action> get_executing_ptr() const
		{return executing_action;}
		
protected:
	// services for derived motor processors
	// call this function with a Motor_action to start the processing for a movement
//	virtual void start_command(Motor_action * action_ptr);
	virtual void setup_prepared_action();
//	virtual void process_command();
//	virtual void prepare_action();
	virtual void execute_action();
	virtual void complete_action();
						
	// void send_operation_complete(long time_required);

private:
	Symbol modality_name;
	// state
	// flags for whether we have signaled this status already
	bool generate_signals;	// true if signals should be generated
	bool signalled_modality_busy;
	bool signalled_processor_busy;
	bool signalled_preparation_busy;
	bool signalled_execution_busy;

	// pointers to Motor_action objects in different phases
	// preparing or ready, processor is busy
	// if preparing, ready, or executing, modality is busy
//	Motor_action * waiting_action;
	std::shared_ptr<Motor_action> preparing_action;
	std::shared_ptr<Motor_action> previous_prepared_action;
	std::shared_ptr<Motor_action> ready_action;
	std::shared_ptr<Motor_action> executing_action;
	long movement_completion_time;
	
	// parameters
	Parameter feature_time;
	Parameter initiation_time;
	Parameter burst_time;
	Parameter efferent_deletion_delay;
	Parameter execution_fluctuation;

	// helper functions
	void cleanup();
	void update_processor_state_signals();
	void signal_modality_busy();
	void signal_modality_free();
	void signal_processor_busy();
	void signal_processor_free();
	void signal_preparation_busy();
	void signal_preparation_free();
	void signal_execution_busy();
	void signal_execution_free();

	// no default copy, assignment
	Motor_processor(const Motor_processor&);
	Motor_processor& operator= (const Motor_processor&);

};

#endif
