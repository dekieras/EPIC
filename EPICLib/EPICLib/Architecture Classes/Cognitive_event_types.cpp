//#include <iostream>
//using namespace std;
#include "Cognitive_event_types.h"
#include "Cognitive_processor.h"

// These definitions must be in a separate file from the event type declaration
// to avoid circular definitions.

void Cognitive_Cycle_event::handle_self(Cognitive_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Cognitive_Update_event::handle_self(Cognitive_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Cognitive_Add_Clause_event::handle_self(Cognitive_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Cognitive_Delete_Clause_event::handle_self(Cognitive_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

/*
void Cognitive_Operation_Complete_event::handle_self(Cognitive_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Cognitive_Visual_Operation_Complete_event::handle_self(Cognitive_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Cognitive_Auditory_Operation_Complete_event::handle_self(Cognitive_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Cognitive_Manual_Operation_Complete_event::handle_self(Cognitive_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Cognitive_Vocal_Operation_Complete_event::handle_self(Cognitive_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Cognitive_Task_Operation_Complete_event::handle_self(Cognitive_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Cognitive_LTM_Operation_Complete_event::handle_self(Cognitive_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Cognitive_Delay_event::handle_self(Cognitive_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}


void Cognitive_Accomplish_Goal_event::handle_self(Cognitive_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Cognitive_Return_Goal_Accomplished_event::handle_self(Cognitive_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Cognitive_Abort_and_Restart_event::handle_self(Cognitive_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

*/
