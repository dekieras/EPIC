#include "Event_types.h"
#include "Processor.h"
#include "Assert_throw.h"
//#include "Output_tee_globals.h"

// These functions must be defined separately from the declarations to avoid circularities
// in the header files.

void Start_event::send_self(Processor * proc_ptr) const 
{
	proc_ptr->accept_event(this);
}

void Stop_event::send_self(Processor * proc_ptr) const 
{		
	proc_ptr->accept_event(this);
}

void Eye_event::send_self(Processor * proc_ptr) const
{		
	proc_ptr->accept_event(this);
}

void Visual_event::send_self(Processor * proc_ptr) const
{		
	proc_ptr->accept_event(this);
}

void Visual_event::handle_self(Eye_processor *) const
{
	Assert(!"Non-overridden virtual function called");
}

void Visual_event::handle_self(Visual_sensory_store *) const
{
	Assert(!"Non-overridden virtual function called");
}

void Visual_event::handle_self(Visual_perceptual_processor *) const
{
	Assert(!"Non-overridden virtual function called");
}

void Visual_event::handle_self(Visual_perceptual_store *) const
{
	Assert(!"Non-overridden virtual function called");
}

void Auditory_event::send_self(Processor * proc_ptr) const
{		
	proc_ptr->accept_event(this);
}

void Auditory_event::handle_self(Auditory_store *) const
{
	Assert(!"Non-overridden virtual function Auditory_event::handle_self(Auditory_store *) called");
}

void Auditory_event::handle_self(Auditory_physical_store *) const
{
	Assert(!"Non-overridden virtual function Auditory_event::handle_self(Auditory_physical_store *) called");
}

void Auditory_event::handle_self(Ear_processor *) const
{
	Assert(!"Non-overridden virtual function Auditory_event::handle_self(Ear_processor *) called");
}

void Auditory_event::handle_self(Auditory_sensory_store *) const
{
	Assert(!"Non-overridden virtual function Auditory_event::handle_self(Auditory_sensory_store *) called");
}

void Auditory_event::handle_self(Auditory_perceptual_processor *) const
{
//	Normal_out << "No override for Auditory_event::handle_self(Auditory_perceptual_processor *) for " << typeid(*this).name() << std::endl;
    Assert(!"Non-overridden virtual function Auditory_event::handle_self(Auditory_perceptual_processor *) called");
}

void Auditory_event::handle_self(Auditory_perceptual_store *) const
{
	Assert(!"Non-overridden virtual function Auditory_event::handle_self(Auditory_perceptual_store *) called");
}

void Cognitive_event::send_self(Processor * proc_ptr) const 
{		
	proc_ptr->accept_event(this);
}

void Motor_event::send_self(Processor * proc_ptr) const
{		
	proc_ptr->accept_event(this);
}

void Temporal_event::send_self(Processor * proc_ptr) const	// Added by ajh 3-12-08
{		
	proc_ptr->accept_event(this);
}

/*
void Manual_event::send_self(Processor * proc_ptr) const
{		
	proc_ptr->accept_event(this);
}

void Ocular_event::send_self(Processor * proc_ptr) const
{		
	proc_ptr->accept_event(this);
}

void Invol_ocular_event::send_self(Processor * proc_ptr) const
{		
	proc_ptr->accept_event(this);
}

void Vocal_event::send_self(Processor * proc_ptr) const
{		
	proc_ptr->accept_event(this);
}

void WM_event::send_self(Processor * proc_ptr) const
{		
	proc_ptr->accept_event(this);
}

void LTM_event::send_self(Processor * proc_ptr) const
{		
	proc_ptr->accept_event(this);
}

void Task_event::send_self(Processor * proc_ptr) const
{		
	proc_ptr->accept_event(this);
}

*/

void Device_event::send_self(Processor * proc_ptr) const
{		
	proc_ptr->accept_event(this);
}
