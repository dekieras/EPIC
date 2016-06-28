#include "Eye_event_types.h"
#include "Eye_processor.h"

// These definitions must be in a separate file from the message types declaration
// to avoid circular definitions.


void Eye_Voluntary_Saccade_Start_event::handle_self(Eye_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Eye_Voluntary_Saccade_End_event::handle_self(Eye_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}


void Eye_Involuntary_Saccade_Start_event::handle_self(Eye_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Eye_Involuntary_Saccade_End_event::handle_self(Eye_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Eye_Involuntary_Smooth_Move_Start_event::handle_self(Eye_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Eye_Involuntary_Smooth_Move_End_event::handle_self(Eye_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}
