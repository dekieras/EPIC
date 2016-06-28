#include "Device_event_types.h"
#include "Device_processor.h"

// These definitions must be in a separate file from the event types declaration
// to avoid circular definitions.

void Device_Delay_event::handle_self(Device_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Device_Keystroke_event::handle_self(Device_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Device_Type_In_event::handle_self(Device_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Device_Hold_event::handle_self(Device_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Device_Release_event::handle_self(Device_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Device_Click_event::handle_self(Device_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Device_Double_Click_event::handle_self(Device_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Device_Point_event::handle_self(Device_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Device_Ply_event::handle_self(Device_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Device_Vocal_event::handle_self(Device_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Device_VisualFocusChange_event::handle_self(Device_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Device_Eyemovement_Start_event::handle_self(Device_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Device_Eyemovement_End_event::handle_self(Device_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Device_HLGet_event::handle_self(Device_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}

void Device_HLPut_event::handle_self(Device_processor * proc_ptr) const
{		
	proc_ptr->handle_event(this);
}



