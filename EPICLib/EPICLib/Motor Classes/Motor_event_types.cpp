#include "Motor_event_types.h"
#include "Motor_processor.h"

// These definitions must be in a separate file from the message types declaration
// to avoid circular definitions.

void Motor_Command_Action_event::handle_self(Motor_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

/*
void Motor_Perform_Prepared_event::handle_self(Motor_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Motor_Perform_Previous_event::handle_self(Motor_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}
*/

void Motor_Preparation_Complete_event::handle_self(Motor_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Motor_Movement_Started_event::handle_self(Motor_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Motor_Movement_Complete_event::handle_self(Motor_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

