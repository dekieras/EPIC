#include "Auditory_event_types.h"
#include "Auditory_physical_store.h"
#include "Ear_processor.h"
#include "Auditory_sensory_store.h"
#include "Auditory_perceptual_store.h"
#include "Auditory_perceptual_processor.h"

// These definitions must be in a separate file from the message types declaration
// to avoid circular definitions.

void Auditory_Stream_Create_event::handle_self(Auditory_store * proc_ptr) const
	{proc_ptr->handle_event(this);}
//void Auditory_Stream_Create_event::handle_self(Auditory_sensory_store * proc_ptr) const
//	{proc_ptr->handle_event(this);}
void Auditory_Stream_Create_event::handle_self(Auditory_perceptual_store * proc_ptr) const
	{proc_ptr->handle_event(this);}

void Auditory_Stream_Destroy_event::handle_self(Auditory_store * proc_ptr) const
	{proc_ptr->handle_event(this);}
//void Auditory_Stream_Destroy_event::handle_self(Ear_processor * proc_ptr) const
//	{proc_ptr->handle_event(this);}
//void Auditory_Stream_Destroy_event::handle_self(Auditory_sensory_store * proc_ptr) const
//	{proc_ptr->handle_event(this);}
void Auditory_Stream_Destroy_event::handle_self(Auditory_perceptual_processor * proc_ptr) const
	{proc_ptr->handle_event(this);}
void Auditory_Stream_Destroy_event::handle_self(Auditory_perceptual_store * proc_ptr) const
	{proc_ptr->handle_event(this);}

void Auditory_Stream_Erase_event::handle_self(Auditory_store * proc_ptr) const
	{proc_ptr->handle_event(this);}
//void Auditory_Stream_Erase_event::handle_self(Auditory_sensory_store * proc_ptr) const
//	{proc_ptr->handle_event(this);}
void Auditory_Stream_Erase_event::handle_self(Auditory_perceptual_store * proc_ptr) const
	{proc_ptr->handle_event(this);}

void Auditory_Stream_Set_Location_event::handle_self(Auditory_store * proc_ptr) const
	{proc_ptr->handle_event(this);}
//void Auditory_Stream_Set_Location_event::handle_self(Auditory_sensory_store * proc_ptr) const
//	{proc_ptr->handle_event(this);}
void Auditory_Stream_Set_Location_event::handle_self(Auditory_perceptual_store * proc_ptr) const
	{proc_ptr->handle_event(this);}

void Auditory_Stream_Set_Pitch_event::handle_self(Auditory_store * proc_ptr) const
	{proc_ptr->handle_event(this);}
//void Auditory_Stream_Set_Pitch_event::handle_self(Auditory_sensory_store * proc_ptr) const
//	{proc_ptr->handle_event(this);}
void Auditory_Stream_Set_Pitch_event::handle_self(Auditory_perceptual_store * proc_ptr) const
	{proc_ptr->handle_event(this);}

void Auditory_Stream_Set_Loudness_event::handle_self(Auditory_store * proc_ptr) const
	{proc_ptr->handle_event(this);}
//void Auditory_Stream_Set_Loudness_event::handle_self(Auditory_sensory_store * proc_ptr) const
//	{proc_ptr->handle_event(this);}
void Auditory_Stream_Set_Loudness_event::handle_self(Auditory_perceptual_store * proc_ptr) const
	{proc_ptr->handle_event(this);}

void Auditory_Stream_Set_Size_event::handle_self(Auditory_store * proc_ptr) const
	{proc_ptr->handle_event(this);}
//void Auditory_Stream_Set_Size_event::handle_self(Auditory_sensory_store * proc_ptr) const
//	{proc_ptr->handle_event(this);}
void Auditory_Stream_Set_Size_event::handle_self(Auditory_perceptual_store * proc_ptr) const
	{proc_ptr->handle_event(this);}

void Auditory_Stream_Set_Property_event::handle_self(Auditory_store * proc_ptr) const
	{proc_ptr->handle_event(this);}
//void Auditory_Stream_Set_Property_event::handle_self(Auditory_sensory_store * proc_ptr) const
//	{proc_ptr->handle_event(this);}
void Auditory_Stream_Set_Property_event::handle_self(Auditory_perceptual_store * proc_ptr) const
	{proc_ptr->handle_event(this);}

void Auditory_Stream_Erase_Property_event::handle_self(Auditory_store * proc_ptr) const
	{proc_ptr->handle_event(this);}
//void Auditory_Stream_Erase_Property_event::handle_self(Auditory_sensory_store * proc_ptr) const
//	{proc_ptr->handle_event(this);}
void Auditory_Stream_Erase_Property_event::handle_self(Auditory_perceptual_store * proc_ptr) const
	{proc_ptr->handle_event(this);}

void Auditory_Sound_Start_event::handle_self(Auditory_store * proc_ptr) const
	{proc_ptr->handle_event(this);}
void Auditory_Sound_Start_event::handle_self(Ear_processor * proc_ptr) const
	{proc_ptr->handle_event(this);}
void Auditory_Sound_Start_event::handle_self(Auditory_sensory_store * proc_ptr) const
	{proc_ptr->handle_event(this);}
void Auditory_Sound_Start_event::handle_self(Auditory_perceptual_processor * proc_ptr) const
	{proc_ptr->handle_event(this);}
void Auditory_Sound_Start_event::handle_self(Auditory_perceptual_store * proc_ptr) const
	{proc_ptr->handle_event(this);}

void Auditory_Speech_Start_event::handle_self(Auditory_store * proc_ptr) const
	{proc_ptr->handle_event(this);}
void Auditory_Speech_Start_event::handle_self(Ear_processor * proc_ptr) const
	{proc_ptr->handle_event(this);}
void Auditory_Speech_Start_event::handle_self(Auditory_sensory_store * proc_ptr) const
	{proc_ptr->handle_event(this);}
void Auditory_Speech_Start_event::handle_self(Auditory_perceptual_processor * proc_ptr) const
	{proc_ptr->handle_event(this);}
void Auditory_Speech_Start_event::handle_self(Auditory_perceptual_store * proc_ptr) const
	{proc_ptr->handle_event(this);}

void Auditory_Sound_Stop_event::handle_self(Auditory_store * proc_ptr) const
	{proc_ptr->handle_event(this);}
void Auditory_Sound_Stop_event::handle_self(Auditory_physical_store * proc_ptr) const
	{proc_ptr->handle_event(this);}
void Auditory_Sound_Stop_event::handle_self(Ear_processor * proc_ptr) const
	{proc_ptr->handle_event(this);}
void Auditory_Sound_Stop_event::handle_self(Auditory_sensory_store * proc_ptr) const
	{proc_ptr->handle_event(this);}
void Auditory_Sound_Stop_event::handle_self(Auditory_perceptual_processor * proc_ptr) const
	{proc_ptr->handle_event(this);}
void Auditory_Sound_Stop_event::handle_self(Auditory_perceptual_store * proc_ptr) const
	{proc_ptr->handle_event(this);}

void Auditory_Sound_Erase_event::handle_self(Auditory_store * proc_ptr) const
	{proc_ptr->handle_event(this);}
void Auditory_Sound_Erase_event::handle_self(Auditory_sensory_store * proc_ptr) const
	{proc_ptr->handle_event(this);}
void Auditory_Sound_Erase_event::handle_self(Auditory_perceptual_store * proc_ptr) const
	{proc_ptr->handle_event(this);}


void Auditory_Sound_Set_Property_event::handle_self(Auditory_store * proc_ptr) const
	{proc_ptr->handle_event(this);}
void Auditory_Sound_Set_Property_event::handle_self(Ear_processor * proc_ptr) const
	{proc_ptr->handle_event(this);}
void Auditory_Sound_Set_Property_event::handle_self(Auditory_sensory_store * proc_ptr) const
	{proc_ptr->handle_event(this);}
void Auditory_Sound_Set_Property_event::handle_self(Auditory_perceptual_processor * proc_ptr) const
	{proc_ptr->handle_event(this);}
void Auditory_Sound_Set_Property_event::handle_self(Auditory_perceptual_store * proc_ptr) const
	{proc_ptr->handle_event(this);}

void Auditory_Sound_Erase_Property_event::handle_self(Auditory_store * proc_ptr) const
	{proc_ptr->handle_event(this);}
//void Auditory_Sound_Erase_Property_event::handle_self(Auditory_sensory_store * proc_ptr) const
//	{proc_ptr->handle_event(this);}
void Auditory_Sound_Erase_Property_event::handle_self(Auditory_perceptual_store * proc_ptr) const
	{proc_ptr->handle_event(this);}

void Auditory_Speech_event::handle_self(Auditory_store * proc_ptr) const
	{proc_ptr->handle_event(this);}
void Auditory_Speech_event::handle_self(Auditory_physical_store * proc_ptr) const
	{proc_ptr->handle_event(this);}
//void Auditory_Speech_event::handle_self(Ear_processor * proc_ptr) const
//	{proc_ptr->handle_event(this);}

void Auditory_Delay_event::handle_self(Auditory_perceptual_processor * proc_ptr) const
	{proc_ptr->handle_event(this);}
//void Visual_Delay_event::handle_self(Auditory_perceptual_store * proc_ptr) const
//	{proc_ptr->handle_event(this);}

