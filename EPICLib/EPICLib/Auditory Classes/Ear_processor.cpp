#include "Ear_processor.h"
#include "Epic_exception.h"
#include "Output_tee_globals.h"

#include "Coordinator.h"
#include "Auditory_physical_store.h"
#include "Auditory_sensory_store.h"
#include "Stream_base.h"
#include "Stream_tracker_base.h"
#include "Stream_tracker_MinDistNoisy.h"
#include "Stream_tracker_MinDist.h"
#include "Stream_tracker_ClosestFirst.h"
#include "Auditory_event_types.h"
#include "Symbol_utilities.h"
#include "View_base.h"
#include "Numeric_utilities.h"
#include "Random_utilities.h"
#include "Standard_Symbols.h"

#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include <limits>
#include <algorithm>
#include <memory>
//#include <functional>

using std::shared_ptr;
using std::make_shared;
using std::vector;
using std::string;
using std::list;
using std::endl;
using std::istringstream;
using std::map;	using std::make_pair;
using std::numeric_limits;
using std::remove_copy;
using std::min_element;

//using std::for_each;
//using std::mem_fun; using std::bind2nd;

/*** Exception classes ***/
class Unknown_physical_property : public Epic_exception {
public:
Unknown_physical_property(Processor * proc_ptr, const Symbol& propname)
	{
		msg = proc_ptr->processor_info() + 
		"Unknown physical property received: " + propname.str();
	}
};



//const int max_n_sounds_present = 4;

/*** Ear_processor ***/

void Ear_processor::setup()
{
//	Auditory_physical_store& physical_store = *(get_human_ptr()->get_Auditory_physical_store_ptr());

	add_parameter(appearance_window);
	add_parameter(appearance_delay);
	add_parameter(disappearance_window);
	add_parameter(disappearance_delay);
	add_parameter(location_delay);
	add_parameter(property_window);
	add_parameter(property_delay);

}

// set overall state
void Ear_processor::initialize()
{
    physical_store_ptr = get_human_ptr()->get_Auditory_physical_store_ptr();
    sensory_store_ptr = get_human_ptr()->get_Auditory_sensory_store_ptr();

    Assert(physical_store_ptr);
    Assert(sensory_store_ptr);

	Human_subprocessor::initialize();
    name_map.clear();
	speech_item_counter = 0;

//	set_trace(true);
//	set_randomize_when_used(true);
//	set_randomize(true);
//	randomize();
//	describe_parameters(Normal_out);
}

//  parameter modifiers
// specification contains <property> <time> or word content list for a Category
void Ear_processor::set_parameter(const Parameter_specification& param_spec)
{
/*	if(param_spec.parameter_name == "Content_detection") {
		istringstream iss(param_spec.specification);
		// first term is category name
		string category_name;
		iss >> category_name;
		if(!iss)
			Parameter::throw_parameter_error("Unable to read Category name in parameter specification", param_spec);
		Symbol cat_name(category_name);
	// second, third, fourth term is detection function mean, sd, lapse rate
		double mean, sd, lapse_rate;
		iss >> mean >> sd >> lapse_rate;
		if(!iss)
			Parameter::throw_parameter_error("Unable to read mean, sd, lapse rate in parameter specification", param_spec);
		detection_parameters[cat_name] = Detection{mean, sd, lapse_rate};
		}
	else if(param_spec.parameter_name == "Category") {
		istringstream iss(param_spec.specification);
		// first term is property name
		string category_name;
		iss >> category_name;
		if(!iss)
			Parameter::throw_parameter_error("Unable to read Category name in parameter specification", param_spec);
		Symbol cat_name(category_name);
	// second through nth term is content of word to be assign to that category, must be at least one
		string word_content;
		int count = 0;
		while(iss >> word_content) {
				category_recodings[word_content] = cat_name;
			count++;
			}
		if(count <= 0)
			Parameter::throw_parameter_error("Did not find at least one word_content for category", param_spec);
		}
	else {
		Human_subprocessor::set_parameter(param_spec);
		}
    */
    Human_subprocessor::set_parameter(param_spec);

}

void Ear_processor::describe_parameters(Output_tee& ot) const
{
	Human_subprocessor::describe_parameters(ot);

/*	ot<< "Categories:" << endl;
	for (const auto& pair : category_recodings)
		ot << "    " << pair.first << ": classified as " << pair.second << endl;
	
	ot<< "Detection parameters:" << endl;
	for (const auto& pair : detection_parameters)
		ot << "    " << pair.first << ": mean = " << pair.second.mean <<
			", sd = " << pair.second.sd << ", lapse rate = " << pair.second.lapse_rate << endl;
*/
}



void Ear_processor::accept_event(const Stop_event *)
{
//	physical_space.display_contents(Normal_out);
}

void Ear_processor::accept_event(const Auditory_event * event_ptr)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << typeid(*event_ptr).name() << " received" << endl;
	broadcast_to_recorders(event_ptr);
	// tell the event to handle itself with itself!
	event_ptr->handle_self(this);
}

Symbol Ear_processor::create_sound_name(const Symbol& physical_name)
{
	Symbol psychological_name = concatenate_to_Symbol("Snd", physical_name, "_", ++speech_item_counter);
	name_map.add_names(physical_name, psychological_name);
	return psychological_name;
}	


void Ear_processor::make_new_sound_start(const Symbol& physical_name, const Symbol& stream_name, long time_stamp,
	GU::Point location, const Symbol& timbre, double loudness, long intrinsic_duration)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "start sound " << physical_name << endl;
	
	Symbol psychological_name = create_sound_name(physical_name);

	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "start sound " << physical_name << '/' << psychological_name << endl;		
	schedule_event(new Auditory_Sound_Start_event(
		get_time() + appearance_window.get_long_value(), this,
		psychological_name, stream_name, time_stamp, location, timbre, loudness, intrinsic_duration));

/*	// transmit the event forward to the next store
	schedule_event(new Auditory_Sound_Start_event(
		get_time() + appearance_delay.get_value(), sensory_store_ptr,
		name, stream, timbre, loudness, time_stamp, intrinsic_duration));
*/		
	// tell the oculomotor processor about the appearance of a new object
	// MUST BE IMPLEMENTED 
	// if reflex enabled and on, and Invol_ocular processor is free,
	// make a reflex saccade to the new object, allowing for time for appearance information to get processed
//	if(reflex_enabled && reflex_on && free_to_move()) {
//		involuntary_movement_in_progress = true;
//		Smart_Pointer<Motor_action> action_ptr = new Invol_ocular_Saccade_action(get_human_ptr(), psychological_name);
//		action_ptr->dispatch(get_time() + appearance_delay.get_value() + inform_invol_ocular_delay.get_value());
//		}

}
// here after the time window delay
void  Ear_processor::handle_event(const Auditory_Sound_Start_event * event_ptr)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "handle start sound event " << event_ptr->sound_name << endl;
		
	make_new_sound_start_f(event_ptr->sound_name, event_ptr->stream_name, event_ptr->time_stamp, event_ptr->location,
		event_ptr->timbre, event_ptr->loudness, event_ptr->intrinsic_duration);
}

/* void  Ear_processor::handle_event(const Auditory_Speech_Start_event * event_ptr)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "handle start speech event " << event_ptr->sound_name << endl;
		
	make_new_speech_start_f(event_ptr->sound_name, event_ptr->stream_name, event_ptr->time_stamp, 
		event_ptr->content, event_ptr->speaker_gender, event_ptr->speaker_id, 
		event_ptr->loudness, event_ptr->duration);
} */


const Symbol Acoustic_status_c("Acoustic_status");
const Symbol Masked_c("Masked");
const Symbol Content_masked_c("Content_masked");


void Ear_processor::make_new_sound_start_f(const Symbol& psychological_name, const Symbol& stream_name, long time_stamp,
    GU::Point original_location, const Symbol& timbre, double loudness, long intrinsic_duration)
{
	if(get_trace() && Trace_out) {
		Symbol_list_t sounds_present = physical_store_ptr->get_sound_name_list();
		Trace_out << "sounds now present: " << sounds_present << endl;
		}
    
	// apply the localization error function
    GU::Point location = apply_auditory_location_error(original_location);

/*	// flip a coin to decide whether to send the sound forward
	double x = unit_uniform_random_variable();
	double threshold = simple_content_masking_probability.get_double_value();
	if(x < threshold) {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << "sound " << psychological_name << " is masked" << endl;
		// mark the sound as masked
		Auditory_physical_store * physical_ptr = get_human_ptr()->get_Auditory_physical_store_ptr();
		const Symbol& physical_name = physical_ptr->get_name_map().get_physical_name(psychological_name);
		Smart_Pointer<Auditory_sound> sound_ptr = physical_ptr->get_sound_ptr(physical_name);
		sound_ptr->set_property(Acoustic_status_c, Content_masked_c);
		return; // don't send forward
		}

	// the sound is not masked; flip a coin to see if the stream name will be passed forward
	x = unit_uniform_random_variable();
	threshold = simple_stream_masking_probability.get_double_value();
	Symbol stream_id = stream;
	if(x < threshold) {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << "sound " << psychological_name << " stream id " << stream << " is masked" << endl;
		stream_id = Symbol();
		}
*/
	/* flip independent coins to decide whether to mask content and/or stream ID - dk 5/25/10 */

/// no masking of sounds at this time dk 8/2/2010

/*
	double x = unit_uniform_random_variable();
	double threshold = simple_content_recognition_probability.get_double_value();
	if(x < threshold) {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << "sound " << psychological_name << " content is masked" << endl;
		// mark the sound as masked
		Auditory_physical_store * physical_ptr = get_human_ptr()->get_Auditory_physical_store_ptr();
		const Symbol& physical_name = physical_ptr->get_name_map().get_physical_name(psychological_name);
		Smart_Pointer<Auditory_sound> sound_ptr = physical_ptr->get_sound_ptr(physical_name);
		sound_ptr->set_property(Acoustic_status_c, Content_masked_c);
		}

	x = unit_uniform_random_variable();
	threshold = simple_stream_recognition_probability.get_double_value();
	Symbol stream_id = stream_name;
	if(x < threshold) {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << "sound " << psychological_name << " stream id " << stream_name << " is masked" << endl;
		stream_id = Symbol();
		}
*/	
	
	// transmit the event forward to the next store
	schedule_event(new Auditory_Sound_Start_event(
		get_time() + appearance_delay.get_long_value(), sensory_store_ptr,
		psychological_name, stream_name, time_stamp, location, timbre, loudness, intrinsic_duration));
		
	// tell the oculomotor processor about the appearance of a new object
	// MUST BE IMPLEMENTED 
	// if reflex enabled and on, and Invol_ocular processor is free,
	// make a reflex saccade to the new object, allowing for time for appearance information to get processed
//	if(reflex_enabled && reflex_on && free_to_move()) {
//		involuntary_movement_in_progress = true;
//		Smart_Pointer<Motor_action> action_ptr = new Invol_ocular_Saccade_action(get_human_ptr(), psychological_name);
//		action_ptr->dispatch(get_time() + appearance_delay.get_value() + inform_invol_ocular_delay.get_value());
//		}

}

	
void Ear_processor::make_sound_stop(const Symbol& physical_name)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "make sound stop " << physical_name << endl;
    
    // look up psychological name
    const Symbol& psychological_name = name_map.get_psychological_name(physical_name);

// do not impose a time window on stop events but do check to see if we got masking before sending it on
	make_sound_stop_f(psychological_name);

/*
	schedule_event(new Auditory_Sound_Stop_event(
		get_time() + disappearance_window.get_value(), this,
		psychological_name));
*/
	// transmit it forward 
//	schedule_event(new Auditory_Sound_Stop_event( 
//		get_time() + disappearance_delay.get_value(), sensory_store_ptr, 
//		psychological_name));
}

// here after the time window delay
void  Ear_processor::handle_event(const Auditory_Sound_Stop_event * event_ptr)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "handle sound stop event " << event_ptr->sound_name << endl;
		
	make_sound_stop_f(event_ptr->sound_name);
}

void Ear_processor::make_sound_stop_f(const Symbol& psychological_name)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "make sound stop " << psychological_name << endl;
/* since we flipped independent coins to decide whether to mask content and/or stream ID, sound was always propagated,
so that it is stopped should be also always propagated - dk 5/25/10 */
/*
	// check to see if the sound is masked
	bool was_masked = false;
	Auditory_physical_store * physical_ptr = get_human_ptr()->get_Auditory_physical_store_ptr();
	// ifthe sound psychological_name is present, access the physical name and see if it was masked;
	// if it is not present, assume that this is not an error but that it is gone because of masking
	if(physical_ptr->get_name_map().is_psychological_name_present(psychological_name)) {
		const Symbol& physical_name = physical_ptr->get_name_map().get_physical_name(psychological_name);
		Smart_Pointer<Auditory_sound> sound_ptr = physical_ptr->get_sound_ptr(physical_name);
		was_masked = (sound_ptr->get_property_value(Acoustic_status_c) == Masked_c);
		}
	else
		was_masked = true;
		
	// if masked, do nothing
	if(was_masked) {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << "sound stop " << psychological_name 
				<< " suppressed due to masking" << endl;
		return;
		}
		
	// otherwise, transmit it forward 
*/
	schedule_event(new Auditory_Sound_Stop_event( 
		get_time() + disappearance_delay.get_long_value(), sensory_store_ptr,
		psychological_name));
}

// sound property changes are also window-delayed so that masking might happen
Symbol Ear_processor::set_sound_property(const Symbol& obj_name, const Symbol& prop_name, 
	const Symbol& prop_value)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "set sound " << obj_name 
			<< " property " << prop_name << " to " << prop_value << endl;

	schedule_event(new Auditory_Sound_Set_Property_event(
		get_time() + property_window.get_long_value(), this,
		obj_name, prop_name, prop_value));

	return prop_value;
}

// here after the time window delay
void  Ear_processor::handle_event(const Auditory_Sound_Set_Property_event * event_ptr)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "handle set property event " << event_ptr->sound_name << endl;
		
	set_sound_property_f(event_ptr->sound_name, event_ptr->property_name, event_ptr->property_value);
}

// this might not be needed

Symbol Ear_processor::set_sound_property_f(const Symbol& psychological_name, const Symbol& prop_name, 
	const Symbol& prop_value)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "set sound " << psychological_name 
			<< " property " << prop_name << " to " << prop_value << endl;
	// go head and send the property forward
	long delay = property_delay.get_long_value();
	// queue a message to the sensory store
	schedule_event(new Auditory_Sound_Set_Property_event(get_time() + delay, 
		sensory_store_ptr, psychological_name, prop_name, prop_value));
	return prop_value;
}



void  Ear_processor::handle_event(const Auditory_Speech_Start_event * event_ptr)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "handle start speech event " << event_ptr->word.name << endl;
		
	make_new_speech_start_f(event_ptr->word);
}

void Ear_processor::make_new_speech_start(const Speech_word& physical_word)
{
	// copy the data, then change to psychological values
	Speech_word psychological_word = physical_word;
//	psychological_word.stream_name = stream_psychological_name; // 5/7/12

	long time_stamp = Coordinator::get_time();
	psychological_word.time_stamp = time_stamp;
	psychological_word.name = create_sound_name(physical_word.name);
	
	if(get_trace() && Trace_out)
//		Trace_out << processor_info() << "start speech sound: " << physical_word.name << ' ' << stream_physical_name << ' '  
		Trace_out << processor_info() << "schedule speech start event: " << physical_word.name << ' ' << physical_word.stream_name << ' '  
		<< physical_word.content << ' ' << physical_word.speaker_gender << ' ' << physical_word.speaker_id << ' ' 
		<< physical_word.pitch << ' ' << physical_word.loudness << ' ' << physical_word.duration << endl;		

	// create an event to come back to us at the end of the appearance_window time.
/*	schedule_event(new Auditory_Speech_Start_event(
		get_time() + appearance_window.get_value(), this,
		sound_name, stream_name, time_stamp, content, speaker_gender, speaker_id, loudness, duration)); */
	schedule_event(new Auditory_Speech_Start_event(
		get_time() + appearance_window.get_long_value(), this,
		psychological_word));

/*	// transmit the event forward to the next store
	schedule_event(new Auditory_Sound_Start_event(
		get_time() + appearance_delay.get_value(), sensory_store_ptr,
		name, stream, timbre, loudness, time_stamp, intrinsic_duration));
*/		
	// tell the oculomotor processor about the appearance of a new object
	// MUST BE IMPLEMENTED 
	// if reflex enabled and on, and Invol_ocular processor is free,
	// make a reflex saccade to the new object, allowing for time for appearance information to get processed
//	if(reflex_enabled && reflex_on && free_to_move()) {
//		involuntary_movement_in_progress = true;
//		Smart_Pointer<Motor_action> action_ptr = new Invol_ocular_Saccade_action(get_human_ptr(), psychological_name);
//		action_ptr->dispatch(get_time() + appearance_delay.get_value() + inform_invol_ocular_delay.get_value());
//		}

}



/*
make_new_speech_start_f
*/

void Ear_processor::make_new_speech_start_f(const Speech_word& word)
{
    // make a copy of the speech word to be propagated - all attributes are copied
    Speech_word word_propagated = word;
	   
    // apply an adjustment to the loudness level for the azimuth; assuming that azimuth is in range [0, 90],
    // and so we compute a head shadowing effect where attenuation is maximum at 90 degrees, and 0 at 0 degrees.
    // use the actual, not perceived, location.
    
    //  7/22/15
    // For BS2004 experiment assuming all sources are on the right, and the levels were normalized to be
    // equal loudness at the right ear, so the right ear level is the original "loudness". The left ear level
    // is supplied by the head shadowing function in its special form here
    // leave the "loudness" equal to the supplied loudness
    word_propagated.level_right = word.loudness;
    // 5/4/16 try disabling shaw function level adjustments
//    word_propagated.level_left = word.loudness;
    word_propagated.level_left = apply_head_shadowing_attenuation(word.loudness, word.location);
    // word.loudness (which is used for the stream tracker) is the left level instead - contralateral level was not normalized
    word_propagated.loudness = word_propagated.level_left;
//	Normal_out << "Ear_processor: word loudness, level_right, level_left " << word_propagated.loudness << ' ' << word_propagated.level_right << ' ' << word_propagated.level_left << endl;
    
	// apply the localization error function
    word_propagated.location = apply_auditory_location_error(word.location);
    
    // just pass it on - masking, stream tracking for speech happens in perceptual processor
	
	schedule_event(new Auditory_Speech_Start_event(
		get_time() + appearance_delay.get_long_value(), sensory_store_ptr,
		word_propagated));

	// tell the oculomotor processor about the appearance of a new object
	// MUST BE IMPLEMENTED 
	// if reflex enabled and on, and Invol_ocular processor is free,
	// make a reflex saccade to the new object, allowing for time for appearance information to get processed
//	if(reflex_enabled && reflex_on && free_to_move()) {
//		involuntary_movement_in_progress = true;
//		Smart_Pointer<Motor_action> action_ptr = new Invol_ocular_Saccade_action(get_human_ptr(), psychological_name);
//		action_ptr->dispatch(get_time() + appearance_delay.get_value() + inform_invol_ocular_delay.get_value());
//		}

}

double Ear_processor::apply_head_shadowing_attenuation(double original_loudness, GU::Point location)
{
    double azimuth = location.x;
	// azimuths are assumed to be in right front quadrant, positive values 0 through 90
    Assert(azimuth >= 0.0 && azimuth <= 90.0);

//    double attenuation = 6.0 * (azimuth / 90.0);
//    return original_loudness - attenuation;
	
	// first correct for different gains on the ipsilateral side for BS2004 normalization
	// we are assuming that source has original loudness at ear
	// Shaw-type function is positive for positive (ipsi) azimuth, negative for negative (contra) azimuth
//	double source_level = original_loudness - Shaw_2500_function(azimuth); // attenuate, since gain increases
	double source_level = original_loudness - LISTEN_band_importance_function(azimuth); // attenuate, since gain increases
//	double source_level = original_loudness - KEMAR_2500_function(azimuth); // attenuate, since gain increases
	// then apply function to contralateral
//	double contra_level = source_level + Shaw_2500_function(-azimuth); // attenuate with negative gain
	double contra_level = source_level + LISTEN_band_importance_function(-azimuth); // attenuate with negative gain
//	double contra_level = source_level + KEMAR_2500_function(-azimuth); // attenuate with negative gain
	// under the normalization assumption, contra level will have better SNR, so return that
//	Normal_out << "azimuth, original, source, contra " << azimuth << ' ' << original_loudness << ' ' << source_level << ' ' << contra_level << endl;
	return contra_level;
}

// return the gain for an azimuth, positive if azimuth is positive, negative otherwise
// positive azimuth means source is ipsilateral to ear, negative is contralateral
double Ear_processor::Shaw_2500_function(double azimuth_)
{
	//force azimuth values to integral values
	long azimuth = round(azimuth_);
	Assert(azimuth >= -90 && azimuth <= +90);

	// transcribed values from Shaw 1974 Fig. 6, 2.5 kHZ
	// 15 degree intervals from -90 to +90 inclusive
	double gains[13] = {-8.249, -6.812, -6.59, -5.905, -3.906, -1.942, 0, 1.523, 2.767, 3.521, 4.085, 3.673, 2.047
};
	double gain_minus_5 = -0.697;
	double gain_plus_5 = 0.043;
	// special case the +/- 5 degree
	if(azimuth == -5)
		return gain_minus_5;
	else if(azimuth == +5)
		return gain_plus_5;
	// since azimuth is integral, should result in values [0 ... 12]
	long index = (azimuth + 90) / 15;
	Assert(index >= 0 && index < sizeof(gains));
	return gains[index];
}

// return the gain for an azimuth, positive if azimuth is positive, negative otherwise
// positive azimuth means source is ipsilateral to ear, negative is contralateral
double Ear_processor::KEMAR_2500_function(double azimuth_)
{
	//force azimuth values to integral values
	long azimuth = round(azimuth_);
	Assert(azimuth >= -90 && azimuth <= +90);

	// KEMAR values from Greg (need cite), at 5 deg azimumth intervals
	double gains[37] = {
        -7.4431,	-8.6509,	-9.8394,	-10.757,	-11.61,     -11.365,
        -11.018,	-9.8432,	-8.5888,	-7.5132,	-6.4306,	-5.4092,
        -4.3788,	-3.6004,	-2.8106,	-1.7451,	-0.66817,	-0.34874,
        0.0,
        0.3719,     0.77029,	1.3152,     1.8667,     2.175,      2.4918,
        2.6518,     2.8123,     2.9494,     3.0889,     3.115,      3.1473,
        2.9789,     2.8168,     2.4039,     1.9974,     1.6652,     1.3393
        };
	// since azimuth is integral, should result in values [0 ... 36]
	int index = int((azimuth + 90) / 5);
	Assert(index >= 0 && index < sizeof(gains));
	return gains[index];
}

// return the gain for an azimuth, positive if azimuth is positive, negative otherwise
// positive azimuth means source is ipsilateral to ear, negative is contralateral
double Ear_processor::LISTEN_avg_function(double azimuth_)
{
	//force azimuth values to integral values
	long azimuth = round(azimuth_);
	Assert(azimuth >= -90 && azimuth <= +90);

	// LISTEN average values from Greg (need cite), at 5 deg azimumth intervals
	double gains[37] = {
        -3.8569,	-4.6028,	-5.3039,	-5.9152,	-6.393,     -6.6978,
        -6.7914,	-6.6506,	-6.3143,	-5.8365,	-5.2663,	-4.633,
        -3.9603,	-3.2706,	-2.5779,	-1.8946,	-1.2319,	-0.59735,
        0.0027228,
        0.56164,	1.0722,     1.527,      1.9208,     2.2564,     2.539,
        2.7734,     2.9641,     3.1153,     3.2305,     3.309,      3.3496,
        3.3514,     3.3178,     3.2525,     3.1605,     3.0502,     2.9306
        };
	// since azimuth is integral, should result in values [0 ... 36]
	int index = int((azimuth + 90) / 5);
	Assert(index >= 0 && index < sizeof(gains));
	return gains[index];
}

// return the gain for an azimuth, positive if azimuth is positive, negative otherwise
// positive azimuth means source is ipsilateral to ear, negative is contralateral
double Ear_processor::LISTEN_band_importance_function(double azimuth_)
{
	//force azimuth values to integral values
	long azimuth = round(azimuth_);
	Assert(azimuth >= -90 && azimuth <= +90);

	// LISTEN band importance average values from Greg (need cite), at 5 deg azimumth intervals
	double gains[37] = {
        -6.0042,	-6.3154,	-6.5939,	-6.8073,	-6.9263,	-6.9348,
        -6.8203,	-6.5751,	-6.2128,	-5.7521,	-5.2117,	-4.61,
        -3.9653,	-3.295,     -2.6132,	-1.9331,	-1.2669,	-0.62238,
        -0.0065867,
        0.57396,	1.1142,     1.6093,     2.0552,     2.4496,     2.7909,
        3.0778,     3.3117,     3.4946,     3.6282,     3.7144,     3.7547,
        3.7515,     3.7088,     3.6313,     3.5247,     3.398,      3.2613
        };
	// since azimuth is integral, should result in values [0 ... 36]
	int index = int((azimuth + 90) / 5);
	Assert(index >= 0 && index < sizeof(gains));
	return gains[index];
}




// add error variability to location of a an auditory object
// returned value is erroneous location

// disabled 6/26/14 for NSMRL work
// enabled Mill1968 version for BrungartSimpson2004 work
// enable Perrott1984 version for above

GU::Point Ear_processor::apply_auditory_location_error(GU::Point original_location)
{
/*
    return original_location;
*/
    // location error in azimuth only
    double x = original_location.x;
	if (x <= 180.) { // for right side
		x = modify_azimuth(x);
		}
	else {  // for left side
		x = 360. - x; // transform to same scale as right side		if(debug_messages)
		x = modify_azimuth(x);
        x = 360. - x; // transform back
		}

	return GU::Point(x, original_location.y);

}

// modify the supplied azimuth with noise, biased, and flipped in some order
double Ear_processor::modify_azimuth(double original_azimuth)
{
	double sd = get_auditory_location_sd(original_azimuth);
	double bias = compute_bias(original_azimuth);
	double biased_azimuth = original_azimuth + bias;
	double x = normal_random_variable(biased_azimuth, sd);
	x = compute_reversal(x);
	return x;
}

/*
 use approximation based on Mills1968, front quadrant only - undefined for elsewhere

 pMAA approximation
Azimuth			slope		intercept
0 <= x <= 45	0.021233333	1.075
45 < x <= 60	0.053466667	-0.3755
60 < x <= 90	0.349833333	-18.1575
		
SD=pMMA/1.36 for sd corresponding to 25%, 75%
correction: MMA defined as half the distance between 25th and 75th percentiles (Mills1958)
SD = pMMA / 0.68

use results from Perrot1984
pMMA fit by 4.4226*EXP(0.0333*X)
MMA defined as
point where discrimination is correct 75% of the time

SD = pMMA /0.68

Hartman & Rakerd (1989): the pMMA is sqrt(2) bigger so multiply
the SD from pmma by 1.4142 to get underlying sd

*/

double Ear_processor::get_auditory_location_sd (double azimuth_)
{
    // if negative, make it positive - reflect around 0 degrees, variability assumed to be symmetrical
    azimuth_ = (azimuth_ < 0) ? -azimuth_ : azimuth_;
    // 5/12/16 do NOT cap at +90
    // azimuth = (azimuth > 90.) ? 90. : azimuth;


/*
//    Makous & Middlebrooks (1998) Figure 8 transcribed
    double sd = 0.07 * azimuth + 2.8147;
    return sd;
*/

// calculate SD value based on Minimum Audible Angle decision process

//	double pMMA; // predicted MMA at the azimuth

/*
    // use fit to Mills (1958) data
	if(0. <= azimuth && azimuth <= 45.) // Mills(1958)
		pMMA = 0.021233333 * azimuth + 1.075;
	else if(45. < azimuth && azimuth <= 60.)
		pMMA = 0.053466667 * azimuth - 0.3755;
	else if(60. < azimuth && azimuth <= 90.)
		pMMA = 0.349833333 * azimuth - 18.1575;
	else
		pMMA = 0.4 * azimuth - 20;  // some made-up values
*/

//	pMMA = 1.3354 * exp(0.0216 * azimuth); // Hefner & Hefner (1988) fit
//	pMMA = 4.4226*exp(0.0333 * azimuth); // Perrott(1984)
//	pMMA = 4.0 * exp(0.05 * azimuth); // guesstimate 405
//	pMMA = 4.0 * exp(0.03 * azimuth); // guesstimate 403
//	pMMA = 4.0 * exp(0.025 * azimuth); // guesstimate 4025
//	pMMA = 4.0 * exp(0.02 * azimuth); // guesstimate 402
//	pMMA = 3.0*exp(0.04 * azimuth); // guesstimate 304
//	pMMA = 3.0*exp(0.02 * azimuth); // guesstimate 302
//	pMMA = 2.0*exp(0.03 * azimuth); // guesstimate 203
//	pMMA = 2.0*exp(0.04 * azimuth); // guesstimate 204
//	pMMA = 1.0*exp(0.05 * azimuth); // guesstimate 105
//	pMMA = 1.0*exp(0.04 * azimuth); // guesstimate 104
/*
    // get the sd of the ogive gaussian for the decision
    double decision_sd = pMMA / 0.68;
    
    // get the sd of the underlying single-observation distribution
    double underlying_sd = decision_sd / 1.4142;

	return underlying_sd;
 */
//    return 3.2*exp(0.0228 * azimuth) / 1.0399; // guesstimate 320228 from Greg 6/7/16
    
    //force azimuth values to integral values
	long azimuth = round(azimuth_);
	Assert(azimuth >= -90 && azimuth <= +90);

	// Values from Greg 061616 for 5 degrees and 15 degree intervals from 0 through +90 inclusive
    const double sds[7] = {3, 5, 6, 7, 10, 15, 30};
	const double sd_plus_5 = 3;
	// special case the +5 degree
	if(azimuth == 5)
		return sd_plus_5;
	// since azimuth is integral, should result in values [0 ... 6]
	long index = azimuth / 15;
	Assert(index >= 0 && index < sizeof(sds));
	return sds[index];
}

// no bias at this time
// return the biased_azimuth computed as a function of the azimuth
double Ear_processor::compute_bias(double x)
{
	return 0.;
}

// no reversal at this time
double Ear_processor::compute_reversal(double original_azimuth)
{
	return original_azimuth;
}

// calculate metric for azimuth difference as |Athis - Aother| / sqrt(SDthis * SDother)
// get absolute value of azimuth difference, allowing for being any mixture of positive or negative values

double Ear_processor::get_scaled_perceived_azimuth_difference(double azimuth1, double azimuth2)
{
    // calculate Normlized difference metric for azimuth difference as |Athis - Aother| / sqrt(SDthis * SDother)
    // get absolute value of azimuth difference, allowing for being any mixture of positive or negative values
//    double raw_azimuth_diff = fabs((azimuth1 < azimuth2) ? azimuth2 - azimuth1 : azimuth1 - azimuth2);
/*
    // we want a signed difference here - assume that if in different quadrants, that's what we want
    double raw_azimuth_diff = fabs(azimuth1 - azimuth2);
    double azimuth1_sd = get_auditory_location_sd(azimuth1);
    double azimuth2_sd = get_auditory_location_sd(azimuth2);
    double azimuth_difference = raw_azimuth_diff / sqrt(azimuth1_sd * azimuth2_sd);
    return azimuth_difference;
  */
	
    // below obsolete 5/11/16
    /*
    // azimuth: straight ahead is 0. sounds can be either to the left, or right. azimuth difference is absolute degrees between the two, in either
    // direction.
    // check range - only allowing azimuths in range [-90, +90]
	// but noise can put them out of this range!
//    Assert(azimuth1 >= -90. && azimuth1 <= +90. && azimuth2 >= -90. && azimuth2 <= +90.);
    // -60,  0 => 60
    //   0,  60 => 60
    // -60, -60 => 0
    // -60, +60 => 120
    // +60, -60 => 120
*/

/*    // logarithmic compression
    double raw_azimuth_diff = fabs(azimuth1 - azimuth2);
    // put it on a logarithmic scale
    double log_azimuth_diff = (raw_azimuth_diff > 0.) ? log10(raw_azimuth_diff) : 0.;
    // cut it off with a minimum value of 0 instead of negative
    log_azimuth_diff = (log_azimuth_diff > 0.) ? log_azimuth_diff : 0.;
    // multiply the scale to make roughly similar to the others
    //log_azimuth_diff = 10.0 * log_azimuth_diff; // scale is 0 to 20.79 or so
    return log_azimuth_diff;
*/
    // root compression
    const double root = 0.25;
    // scale each azimuth using a root to give a compressed scale, retaining the sign
    double scaled_azimuth1 = (azimuth1 > 0) ? pow(azimuth1, root) : - pow(fabs(azimuth1), root);
    double scaled_azimuth2 = (azimuth2 > 0) ? pow(azimuth2, root) : - pow(fabs(azimuth2), root);
    return scaled_azimuth1 - scaled_azimuth2;
}

/*
// return the sd for the guassian error sampling
double compute_sd (double x)
{
// based on Middlebrooks
// =-0.00000007337*A2^4+0.00001787*A2^3-0.001513*A2^2+0.1231*A2+2.29
	const double a4 = -0.0000000733;
	const double a3 = +0.00001787;
	const double a2 = -0.001513;
	const double a1 = +0.1231;
	const double a0 = +2.29;
	
	double sd = a4 * (x * x * x * x) + a3 * (x * x *x) + a2 * (x * x) + a1 * (x) + a0;
	return sd;
}

// return the biased_azimuth computed as a function of the azimuth
double compute_bias(double x)
{
// based on combined Oldfield & Middlebrooks data
// =0.0000002801*A17^4-0.00007509*A17^3+0.004356*A17^2+0.006655*A17+0.1454
	const double a4 = 0.0000002801;
	const double a3 = -0.00007509;
	const double a2 = +0.004356;
	const double a1 = +0.006655;
	const double a0 = +0.1454;
	
	double biased = a4 * (x * x * x * x) + a3 * (x * x *x) + a2 * (x * x) + a1 * (x) + a0;
	return biased;
}


// Flip a coin that follows a bell curve probability roughly centered on 90 degrees
// The probability of the coin follows a gaussian curve, but this is not a gaussian distribution.
// Return either the original value or the flipped value.
double compute_reversal(double original_azimuth)
{
	const double m = 90.;
	const double s = 57.9;
	const double a = 62.54;
	const double b = 0.01318;
	const double c = 0.278;
	
	// compute value to compare to uniform random variable
	double az = (original_azimuth - m) / s;
	double z = (az - b) / c;
	double p = a * exp(-(z*z));
	
	double x = unit_uniform_random_variable();
	if(x <= p)
		return 180. - original_azimuth;// reverse
	else
		return original_azimuth;
}

*/

