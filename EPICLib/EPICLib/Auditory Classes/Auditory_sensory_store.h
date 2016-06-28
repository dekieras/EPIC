/* Auditory_sensory_store is supposed to correspond to the precategorical acoustic store
at least when fully developed. It accepts events sent from the Ear_processor, creates
and stores the corresponding streams and sounds, and "remembers" them for some time
after they disappear. Each input event is translated to a direct call to the Auditory perceptual
processor. At this point, the memory has no down-stream consequences, since
the downstream processor is called when a disappearance event first appears, not when
the local memory for it disappears.

*/

#ifndef AUDITORY_SENSORY_STORE_H
#define AUDITORY_SENSORY_STORE_H

#include "Auditory_store.h"
#include "Human_subprocessor.h"
#include "Symbol.h"
#include "Geometry.h"
namespace GU = Geometry_Utilities;


/* Auditory_sensory_store contains Auditory Streams and Sounds
*/

class Auditory_sensory_store : public Auditory_store {
public:
	Auditory_sensory_store(Human_processor * human_ptr_) :
		Auditory_store("Auditory_sensory_store", human_ptr_),
		// delay for when the object actually disappears from this store
		disappearance_decay_delay("Disappearance_decay_delay_time", 200)
		{add_parameter(disappearance_decay_delay);}

	/* event interface
	*/
//	virtual void accept_event(const Start_event *);
//	virtual void accept_event(const Stop_event *);
	virtual void accept_event(const Auditory_event *);
	virtual void handle_event(const Auditory_Stream_Create_event *);
	virtual void handle_event(const Auditory_Stream_Destroy_event *);
	virtual void handle_event(const Auditory_Stream_Erase_event *);
	virtual void handle_event(const Auditory_Stream_Set_Location_event *);
	virtual void handle_event(const Auditory_Stream_Set_Pitch_event *);
	virtual void handle_event(const Auditory_Stream_Set_Loudness_event *);
	virtual void handle_event(const Auditory_Stream_Set_Size_event *);
	virtual void handle_event(const Auditory_Stream_Set_Property_event *);
//	virtual void handle_event(const Auditory_Stream_Erase_Property_event *);
	virtual void handle_event(const Auditory_Sound_Start_event *);
	virtual void handle_event(const Auditory_Speech_Start_event *);
	virtual void handle_event(const Auditory_Sound_Stop_event *);
	virtual void handle_event(const Auditory_Sound_Erase_event *);
	virtual void handle_event(const Auditory_Sound_Set_Property_event *);
//	virtual void handle_event(const Auditory_Sound_Erase_Property_event *);

private:
	// parameters
	Parameter disappearance_decay_delay;
	
	// no default copy, assignment
	Auditory_sensory_store(const Auditory_sensory_store&);
	Auditory_sensory_store& operator= (const Auditory_sensory_store&);
};



#endif
