#include "Vocal_actions.h"
#include "Vocal_processor.h"
#include "Coordinator.h"
#include "Cognitive_event_types.h"
#include "Device_event_types.h"
#include "Device_processor.h"
#include "Auditory_event_types.h"
#include "Output_tee_globals.h"
#include "Epic_standard_symbols.h"
#include "Symbol_utilities.h"
#include "Syllable_counter.h"

#include <string>
#include <memory>

using std::string;
using std::shared_ptr;

const double overt_loudness_c = 60.;

// this "virtual constructor" method returns a pointer to the created object
// based on the contents of the list of motor command parameters
// or zero if no object was created because the parameters are invalid
shared_ptr<Motor_action> Vocal_action::create(Human_processor * human_ptr,
	Symbol_list_t arguments, bool execute_when_prepared)
{	
	if(arguments.size() < 2)
		return 0;
			
	const Symbol style = arguments.front();
	arguments.pop_front();
	if(style == Speak_c) {
		return shared_ptr<Motor_action> (new Vocal_Speak_action(human_ptr, arguments, true, execute_when_prepared));
		}
	if(style == Subvocalize_c) {
		return shared_ptr<Motor_action> (new Vocal_Speak_action(human_ptr, arguments, false, execute_when_prepared));
		}
	else
		return 0;
}

Vocal_action::Vocal_action(Human_processor * human_ptr_, bool execute_when_prepared) :
		Motor_action(human_ptr_, execute_when_prepared)
{
	proc_ptr = human_ptr->get_Vocal_processor_ptr();
	device_ptr = human_ptr->get_Device_processor_ptr();
}

// this object will send itself to the appropriate processor
void Vocal_action::dispatch(long time)
{
	human_ptr->schedule_event(new Motor_Command_Action_event(
		time, 
		human_ptr->get_Vocal_processor_ptr(),
		shared_from_this()
//		std::static_pointer_cast<Motor_action>(shared_from_this())
		));
}



// All randomized fluctuations are done in the Motor_processor parameters, which includes 
// a single fluctuation parameter. Individual movement execution characteristics 
// are fixed parameters defined here.

// local constants

//const long syllable_time_c = 150;	// ms per syllable

int Vocal_Speak_action::utterance_counter  = 0;

Vocal_Speak_action::Vocal_Speak_action(Human_processor * hum_ptr, Symbol_list_t arguments, bool overt_, bool execute_when_prepared) :
		Vocal_action(hum_ptr, execute_when_prepared), overt(overt_)
{
	// argument list should contain at least one symbol
	if(arguments.size() < 1)
		throw Motor_action_exception(hum_ptr, "Utterance string", arguments);
	
	// concatenate the rest of the arguments to make up the utterance - one space between them
	string s = concatenate_to_string(arguments);
	utterance = Symbol(s);
}



// note that dynamic_cast of a pointer whose value is zero results in zero
// return the time required for preparation
// average preparation is 2 features, 3 if the style has changed
long Vocal_Speak_action::prepare(long base_time)
{
	long feature_preparation_time;
	shared_ptr<Vocal_Speak_action> ptr = std::dynamic_pointer_cast<Vocal_Speak_action>(get_processor_ptr()->get_previous_ptr());
	long feature_time = get_processor_ptr()->get_feature_time();

	if (ptr) {								// previous is same type of action
		if (ptr->utterance == utterance)
			feature_preparation_time = 0;	// identical movement
		else
			feature_preparation_time = long(feature_time * 2); // 2 features
		}
	else 
			feature_preparation_time = long(feature_time  * 3);	// style change - three features
	
	return base_time + feature_preparation_time;
}

// return the time at which the final movement is complete
// and send any additional messages in the meantime
long Vocal_Speak_action::execute(long base_time)
{
	Human_processor * human_ptr = get_processor_ptr()->get_human_ptr();
	Processor * cognitive_ptr = human_ptr->get_cognitive_ptr();

	double fluctuation = get_processor_ptr()->get_execution_fluctuation();
	// the execution time is based on an estimated articulation time
	long duration = long(articulation_time() * fluctuation);

	long movement_completion_time = base_time + duration;

	// movement description form: (Motor Vocal Speak <utterance> Started|Finished)	
	Symbol_list_t movement_description;
	movement_description.push_back(Motor_c);
	movement_description.push_back(Vocal_c);
	movement_description.push_back((overt) ? Speak_c : Subvocalize_c);
	movement_description.push_back(utterance);
	movement_description.push_back(Started_c);

 //	tell cog that movement has started at base_time, signal to be deleted after a delay
 	Coordinator::get_instance().schedule_event(new 
		Cognitive_Add_Clause_event(base_time, cognitive_ptr, movement_description));
 	Coordinator::get_instance().schedule_event(new 
		Cognitive_Delete_Clause_event(base_time + get_processor_ptr()->get_efferent_deletion_delay(),
		cognitive_ptr, movement_description));
 
	// if overt, send the utterance at the base time to the device - duration is included
	// the arrival time is when the speech starts, arrival+duration is when it ends
	if(overt) {
		Coordinator::get_instance().schedule_event(new 
		Device_Vocal_event(base_time, get_device_ptr(), utterance, duration));
		}
	// send the utterance to the auditory system as well, in either case, but with a stream depending on overt/covertif
	Processor * auditory_physical_store_ptr = human_ptr->get_auditory_physical_store_ptr();
//	auditory_physical_store_ptr->make_speech_sound_event(Symbol("Internal_speech_item"), Symbol("Overt"), utterance, loudness, duration);
/* 	Coordinator::get_instance().schedule_event(new 
		Auditory_Speech_event(base_time, auditory_physical_store_ptr, 
		(overt) ? Symbol("Overt_speech_item") : Symbol("Covert_speech_item"), 
		(overt) ? Overt_c : Covert_c, 
		utterance, 
		Male_c, Symbol("Self"),	// gender and speaker-id
		(overt) ? overt_loudness_c : 0., 
		duration));
*/
	Symbol speech_item_name = (overt) ? concatenate_to_Symbol("Overt_speech_item", "_", ++utterance_counter) :
		concatenate_to_Symbol("Covert_speech_item", "_", ++utterance_counter);
	
	Speech_word word;
//	word.name = (overt) ? Symbol("Overt_speech_item") : Symbol("Covert_speech_item");
	word.name = speech_item_name;
	word.stream_name = (overt) ? Overt_c : Covert_c;
	word.time_stamp = 0;
	word.loudness = (overt) ? overt_loudness_c : 0.;
	word.pitch = 100.;
	word.duration = duration;
	word.content = utterance;
	word.speaker_gender = Male_c;
	word.speaker_id = Symbol("Self");
	
 	Coordinator::get_instance().schedule_event(new 
		Auditory_Speech_event(base_time, auditory_physical_store_ptr, word));	
	
// 	tell cog that movement is finished at movement_completion_time
	movement_description.pop_back();	// discard "Started" term
	movement_description.push_back(Finished_c);
	Coordinator::get_instance().schedule_event(new 
		Cognitive_Add_Clause_event(movement_completion_time, cognitive_ptr, movement_description));
 	Coordinator::get_instance().schedule_event(new 
		Cognitive_Delete_Clause_event(movement_completion_time + get_processor_ptr()->get_efferent_deletion_delay(),
		cognitive_ptr, movement_description));

	return movement_completion_time;
}

 // return the number of ms to articulate the utterance
long Vocal_Speak_action::articulation_time()
{
	long syllable_time = get_processor_ptr()->get_parameter_ptr("Syllable_time")->get_long_value();
	
	return syllable_time * count_total_syllables(utterance.str());
}	
