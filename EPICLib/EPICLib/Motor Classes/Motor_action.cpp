#include "Motor_action.h"
#include "Human_processor.h"
#include "Manual_actions.h"
#include "Ocular_actions.h"
#include "Vocal_actions.h"
#include "Symbol_utilities.h"
#include "Epic_standard_symbols.h"

#include <string>
using std::string;
using std::shared_ptr;

// these are user-interpretable errors
Motor_action_exception::Motor_action_exception(Human_processor * hum_ptr, 
	string msg, const Symbol_list_t& arguments) :
	Epic_exception(hum_ptr, string("Invalid Motor action: ") + msg + "(" + concatenate_to_string(arguments) + ")")
{
}

Motor_action_exception::Motor_action_exception(Human_processor * hum_ptr, 
	string msg, const Symbol& argument) :
	Epic_exception(hum_ptr, string("Invalid Motor action: ") + msg + argument.str())
{
}



// this "virtual constructor" method returns a pointer to the created object
// based on the contents of the list of motor command arguments
// or zero if no object was created because the arguments are invalid
// complete check of validity must wait until run time
// note that arguments list is copied, so that it can be chopped as we go down
shared_ptr<Motor_action> Motor_action::create(Human_processor * human_ptr, Symbol_list_t arguments)
{	
	if(arguments.size() < 3)
		return 0;

	const Symbol modality = arguments.front();
	arguments.pop_front();
	const Symbol command = arguments.front();
	arguments.pop_front();

	bool execute_when_prepared;
	if (command == Perform_c)
		execute_when_prepared = true;
	else if(command == Prepare_c)
		execute_when_prepared = false;
	else
		return 0;

	
	if (modality == Manual_c) 
		return Manual_action::create(human_ptr, arguments, execute_when_prepared);
	else if (modality == Ocular_c) 
		return Ocular_action::create(human_ptr, arguments, execute_when_prepared);
	else if (modality == Vocal_c) 
		return Vocal_action::create(human_ptr, arguments, execute_when_prepared);
	else 
		return 0;
}


/* useless at this level */
// helper so that Motor_actions can access trace status of their motor processor
/*
bool Motor_action::get_trace()
{
	return human_ptr->get_trace();
}
*/
