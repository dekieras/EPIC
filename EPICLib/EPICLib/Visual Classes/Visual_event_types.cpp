#include "Visual_event_types.h"
#include "Eye_processor.h"
#include "Visual_sensory_store.h"
#include "Visual_perceptual_store.h"
#include "Visual_perceptual_processor.h"

// These definitions must be in a separate file from the message types declaration
// to avoid circular definitions.


//void Visual_Appear_event::handle_self(Eye_processor * proc_ptr) const
//	{proc_ptr->handle_event(this);}
void Visual_Appear_event::handle_self(Visual_sensory_store * proc_ptr) const
	{proc_ptr->handle_event(this);}
//void Visual_Appear_event::handle_self(Visual_perceptual_processor * proc_ptr) const
//	{proc_ptr->handle_event(this);}
void Visual_Appear_event::handle_self(Visual_perceptual_store * proc_ptr) const
	{proc_ptr->handle_event(this);}

void Visual_Disappear_event::handle_self(Eye_processor * proc_ptr) const
	{proc_ptr->handle_event(this);}
void Visual_Disappear_event::handle_self(Visual_sensory_store * proc_ptr) const
	{proc_ptr->handle_event(this);}
//void Visual_Disappear_event::handle_self(Visual_perceptual_processor * proc_ptr) const
//	{proc_ptr->handle_event(this);}
void Visual_Disappear_event::handle_self(Visual_perceptual_store * proc_ptr) const
	{proc_ptr->handle_event(this);}


//void Visual_Erase_event::handle_self(Eye_processor * proc_ptr) const
//	{proc_ptr->handle_event(this);}
void Visual_Erase_event::handle_self(Visual_sensory_store * proc_ptr) const
	{proc_ptr->handle_event(this);}
//void Visual_Erase_event::handle_self(Visual_perceptual_processor * proc_ptr) const
//	{proc_ptr->handle_event(this);}
void Visual_Erase_event::handle_self(Visual_perceptual_store * proc_ptr) const
	{proc_ptr->handle_event(this);}


void Visual_Change_Location_event::handle_self(Eye_processor * proc_ptr) const
	{proc_ptr->handle_event(this);}
void Visual_Change_Location_event::handle_self(Visual_sensory_store * proc_ptr) const
	{proc_ptr->handle_event(this);}
//void Visual_Change_Location_event::handle_self(Visual_perceptual_processor * proc_ptr) const
//	{proc_ptr->handle_event(this);}
void Visual_Change_Location_event::handle_self(Visual_perceptual_store * proc_ptr) const
	{proc_ptr->handle_event(this);}

void Visual_Change_Size_event::handle_self(Eye_processor * proc_ptr) const
	{proc_ptr->handle_event(this);}
void Visual_Change_Size_event::handle_self(Visual_sensory_store * proc_ptr) const
	{proc_ptr->handle_event(this);}
//void Visual_Change_Size_event::handle_self(Visual_perceptual_processor * proc_ptr) const
//	{proc_ptr->handle_event(this);}
void Visual_Change_Size_event::handle_self(Visual_perceptual_store * proc_ptr) const
	{proc_ptr->handle_event(this);}

void Visual_Change_Property_event::handle_self(Eye_processor * proc_ptr) const
	{proc_ptr->handle_event(this);}
void Visual_Change_Property_event::handle_self(Visual_sensory_store * proc_ptr) const
	{proc_ptr->handle_event(this);}
//void Visual_Change_Property_event::handle_self(Visual_perceptual_processor * proc_ptr) const
//	{proc_ptr->handle_event(this);}
void Visual_Change_Property_event::handle_self(Visual_perceptual_store * proc_ptr) const
	{proc_ptr->handle_event(this);}

//void Visual_Erase_Property_event::handle_self(Eye_processor * proc_ptr) const
//	{proc_ptr->handle_event(this);}
void Visual_Erase_Property_event::handle_self(Visual_sensory_store * proc_ptr) const
	{proc_ptr->handle_event(this);}
//void Visual_Change_Property_event::handle_self(Visual_perceptual_processor * proc_ptr) const
//	{proc_ptr->handle_event(this);}
void Visual_Erase_Property_event::handle_self(Visual_perceptual_store * proc_ptr) const
	{proc_ptr->handle_event(this);}

//void Visual_Delay_event::handle_self(Eye_processor * proc_ptr) const
//	{proc_ptr->handle_event(this);}
// void Visual_Delay_event::handle_self(Visual_sensory_store * proc_ptr) const
//	{proc_ptr->handle_event(this);}
void Visual_Delay_event::handle_self(Visual_perceptual_processor * proc_ptr) const
	{proc_ptr->handle_event(this);}
//void Visual_Delay_event::handle_self(Visual_perceptual_store * proc_ptr) const
//	{proc_ptr->handle_event(this);}


