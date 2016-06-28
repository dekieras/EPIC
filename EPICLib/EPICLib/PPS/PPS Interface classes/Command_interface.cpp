#include "Command_interface.h"
#include "Cognitive_processor.h"
#include "Motor_action.h"
#include "Temporal_processor.h"
#include "Parameter.h"
#include "Random_utilities.h"
#include "Epic_exception.h"
//#include "Smart_Pointer.h"
#include "PPS_globals.h"
#include "Epic_standard_symbols.h"	// for temporal

#include <memory>
#include <iostream>
#include <string>

using std::shared_ptr;
using std::cout;	using std::endl;
using std::string;


class Command_exception : public Epic_exception {
public:
	Command_exception(Processor * proc_ptr, const std::string& msg_) :
		Epic_exception(proc_ptr, msg_) {}
};


// Check first to see if this is a reserved command - denoted by the first term of the arguments
// dispatching consists of calling an action virtual-constructor to create a Motor_action object
// with the appropriate parameters and then sending it to the appropriate processor for its type

void Command_interface::dispatch(const Symbol_list_t& arguments)
{	
	if(arguments.front() == "Log") {
		PPS_out << human_ptr->processor_info();
		for(Symbol_list_t::const_iterator it = arguments.begin(); it != arguments.end(); it++) {
			PPS_out << (*it) << ' ';
			}
		PPS_out << endl;
		return;
		}
		

	// Start and end the temporal counter directly, rather than adding 'start' and 'end' events to the queue.
	// added by ajh & teh 2 - 3/17/08
	else if(arguments.front() == "Send_to_temporal")
	{	
		Symbol_list_t args = arguments;	// so we can modify the argument list
		args.pop_front();
		
		if(args.front() == Start_c)
			human_ptr->get_Temporal_processor_ptr()->start_command();
			
		else if(args.front() == End_c)
			human_ptr->get_Temporal_processor_ptr()->end_command();
			
		else
			throw Command_exception(human_ptr,
				string("Invalid rule Temporal action specification: (") + concatenate_to_string(arguments) + ")");
		return;
	}

	// Previously DK just assumed at this point it would be Send_to_motor.  ajh & teh made it explicit.
	else if(arguments.front() == "Send_to_motor")		// "else if" added by ajh & teh 2 - 3/17/08
	{
		// Remove the "Send_to_motor" - ajh & teh
		Symbol_list_t args = arguments;	// so we can modify the argument list
		args.pop_front();

		// the arguments must contain a motor command - create and dispatch the action
		shared_ptr<Motor_action> action_ptr = Motor_action::create(human_ptr, args);
		if(!action_ptr)
			throw Command_exception(human_ptr, 
				string("Invalid rule action specification: (") + concatenate_to_string(args) + ")");
		// tell the action to send itself to the appropriate motor processor at the step end time.
		action_ptr->dispatch(human_ptr->get_Cognitive_processor_ptr()->get_step_end_time());
	}
	
	else
	
	{
		throw Command_exception(human_ptr, 
			string("Invalid rule action specification: (") + concatenate_to_string(arguments) + ")");
	}
}

// all this function does is to decide whether the pattern should be added, returning true if so
bool Command_interface::dispatch(const Symbol_list_t& arguments, const Symbol_list_t& pattern)
{
	if(arguments.front() == "Add_with_probability")
        {
		Symbol_list_t args = arguments;	// so we can modify the argument list
		args.pop_front();
        
        Symbol prob_param = args.front();
        Cognitive_processor* cog_ptr = human_ptr->get_Cognitive_processor_ptr();
        double prob = 0.;
        // should be either a number or a parameter name
        if(prob_param.has_numeric_value()) {
            prob = prob_param.get_numeric_value();
            if(prob < 0 || prob > 1.0)
                throw Command_exception(human_ptr,
				string("Invalid Add_with_probability specification: (") + concatenate_to_string(arguments) + ")");
            }
       
		else { // should be EPIC cognitive processor parameter
            // Parameter* get_parameter_ptr(const std::string& parameter_name) const;
            // following throws if no parameter of that name, halting run
            Parameter* param_ptr = cog_ptr->get_parameter_ptr(prob_param.str());
            prob = param_ptr->get_double_value(); // get_value returns as long!
//            PPS_out << "probability parameter is " << prob << endl;
            }
        
        if(pattern.empty())
            throw Command_exception(human_ptr,
			string("Invalid Add_with_probability specification: no pattern supplied"));

        // flip coin; if "heads" then add the first (only one) clause; if tails, do not
        if(biased_coin_flip(prob)) {
//            PPS_out << " pattern added" << endl;
            return true;
            }
            
		return false;
        }

	else
		throw Command_exception(human_ptr,
			string("Invalid rule action specification: (") + concatenate_to_string(arguments) + ")");
}
/*


	if(command == "Perform" || command == "Prepare") {
		bool execute_when_prepared = true;
		if (command == "Prepare")
			execute_when_prepared = false;
		Symbol_list_t::const_iterator it = parameters.begin();
		Symbol motor_processor_name = get_next_parameter(parameters, it);
		Symbol movement_style = get_next_parameter(parameters, it);
		if(command == "Perform" && movement_style == "Prepared" && motor_processor_name == "Manual") {
			// generate an instruction to execute the previously prepared movement
			cog_proc_ptr->schedule_event(Motor_Perform_Prepared_event::create(
				cog_proc_ptr->step_end_time, 
				cog_proc_ptr->get_human_ptr()->get_manual_ptr()));
			return;
			}
		if(command == "Perform" && movement_style == "Previous" && motor_processor_name == "Manual") {
			// generate an instruction to repeat the previous movement
			cog_proc_ptr->schedule_event(Motor_Perform_Previous_event::create(
				cog_proc_ptr->step_end_time, 
				cog_proc_ptr->get_human_ptr()->get_manual_ptr()));
			return;
			}
			
		if(motor_processor_name == "Vocular" && movement_style == "Move") {
			Symbol target = get_next_parameter(parameters, it);
			cog_proc_ptr->schedule_event(Ocular_Move_event::create(
				cog_proc_ptr->step_end_time, 
				cog_proc_ptr->get_human_ptr()->get_ocular_ptr(),
				target, execute_when_prepared));
			}
		else if(motor_processor_name == "Manual" && movement_style == "Keystroke") {
			Symbol key_name = get_next_parameter(parameters, it);
			cog_proc_ptr->schedule_event(Manual_Keystroke_event::create(
				cog_proc_ptr->step_end_time, 
				cog_proc_ptr->get_human_ptr()->get_manual_ptr(),
				key_name, execute_when_prepared));
			}
		else if(motor_processor_name == "Manual" && movement_style == "Punch") {
			Symbol key_name = get_next_parameter(parameters, it);
			Symbol hand = get_next_parameter(parameters, it);
			Symbol finger = get_next_parameter(parameters, it);
			cog_proc_ptr->schedule_event(Manual_Punch_event::create(
				cog_proc_ptr->step_end_time, 
				cog_proc_ptr->get_human_ptr()->get_manual_ptr(),
				key_name, hand, finger, execute_when_prepared));
			}
		else if(motor_processor_name == "Manual" && movement_style == "Cursor_Point") {
			Symbol cursor = get_next_parameter(parameters, it);
			Symbol target = get_next_parameter(parameters, it);
			cog_proc_ptr->schedule_event(Manual_Point_Mouse_event::create(
				cog_proc_ptr->step_end_time, 
				cog_proc_ptr->get_human_ptr()->get_manual_ptr(),
				cursor, target, execute_when_prepared));
			}
		else 
			throw Command_exception(cog_proc_ptr, "Unrecognized PPS command parameters");
		}
	else
		throw Command_exception(cog_proc_ptr, "Unrecognized PPS command");
}

const Symbol& Command_interface::get_next_parameter(const Symbol_list_t& parameters, Symbol_list_t::const_iterator& it)
{
	if(it == parameters.end()) {
		throw Command_exception(cog_proc_ptr, "Attempt to read too many command parameters");
		}

	return *it++;
}
*/
