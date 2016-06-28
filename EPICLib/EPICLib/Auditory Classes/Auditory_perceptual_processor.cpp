#include "Auditory_perceptual_processor.h"
#include "Auditory_event_types.h"
#include "Auditory_sensory_store.h"
#include "Auditory_perceptual_store.h"
#include "Auditory_encoder_base.h"
#include "Ear_processor.h"  // for parametric services
#include "Epic_exception.h"
#include "Output_tee_globals.h"

#include "Stream_base.h"
#include "Stream_tracker_base.h"
#include "Stream_tracker_MinDistNoisy.h"
#include "Stream_tracker_MinDist.h"
#include "Stream_tracker_ClosestFirst.h"

#include "Symbol_utilities.h"
#include "Numeric_utilities.h"
#include "Random_utilities.h"
#include "Standard_Symbols.h"
#include "Epic_standard_symbols.h"

#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include <limits>
#include <algorithm>
#include <memory>
//#include <functional>
#include <typeinfo>

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


/*** Exception classes ***/
// this is a user-interpretable error
class Unknown_perceptual_property : public Epic_exception {
public:
Unknown_perceptual_property(Processor * proc_ptr, const Symbol& propname)
	{
		msg = proc_ptr->processor_info() + 
		"Unknown perceptual property received: " + propname.str();
	}
};

const Symbol Category_c ("Category");
const int max_n_sounds_present = 4;

const Symbol Acoustic_status_c("Acoustic_status");
const Symbol Masked_c("Masked");
const Symbol Content_masked_c("Content_masked");

const Symbol Self_c("Self");
const Symbol Stream_assignment_c("Stream_assignment");



void Auditory_perceptual_processor::setup()
{
	recoding_times[Pitch_c] = 100;
	recoding_times[Loudness_c] = 100;
	recoding_times[Timbre_c] = 100;
	recoding_times[Content_c] = 100;
	recoding_times[Gender_c] = 100;
	recoding_times[Speaker_c] = 100;
	recoding_times[Category_c] = 100; // gets added to duration ...
	recoding_times[Start_time_c] = 60;
	recoding_times[End_time_c] = 60;
	add_parameter(appearance_window);
	add_parameter(appearance_delay);
	add_parameter(disappearance_window);
	add_parameter(disappearance_delay);
	add_parameter(location_delay);
	add_parameter(recoded_location_delay);
	add_parameter(property_window);
//	add_parameter(property_time_fluctuation);
	add_parameter(recoding_time_fluctuation);
	add_parameter(transient_decay_time);
	add_parameter(recoding_failure_rate1);
	add_parameter(effective_snr_loudness_weight);
	add_parameter(effective_snr_pitch_weight);
	add_parameter(effective_snr_location_weight);
	add_parameter(pitch_difference_max);
	add_parameter(stream_loudness_weight);   // formerly 1-stream_lambda
	add_parameter(stream_pitch_weight);   // formerly stream_lambda
	add_parameter(stream_location_weight);
	add_parameter(stream_theta);
    
    
    // choose our stream tracker at this point
    stream_tracker_ptr = std::move(std::unique_ptr<Stream_tracker_base>(new Stream_tracker_MinDistNoisy("Averaging_stream")));

}

// set overall state
void Auditory_perceptual_processor::initialize()
{
    // point to upstream and downstream stores
    sensory_store_ptr = get_human_ptr()->get_Auditory_sensory_store_ptr();
    perceptual_store_ptr = get_human_ptr()->get_Auditory_perceptual_store_ptr();

    Assert(sensory_store_ptr);
    Assert(perceptual_store_ptr);
    Assert(stream_tracker_ptr);

	Human_subprocessor::initialize();
	stream_tracker_ptr->clear();
//	set_trace(true);
//	set_randomize_when_used(true);
//	set_randomize(true);
//	randomize();
//	describe_parameters(Normal_out);
}

void Auditory_perceptual_processor::set_auditory_encoder_ptr(Auditory_encoder_base * auditory_encoder_ptr_)
{
	auditory_encoder_ptr = auditory_encoder_ptr_;
}

//  parameter modifiers
// specification contains <property> <time> or word content list for a Category
void Auditory_perceptual_processor::set_parameter(const Parameter_specification& param_spec)
{
	if(param_spec.parameter_name == "Recoding_time") {
		istringstream iss(param_spec.specification);
		// first term is property name
		string property_name;
		iss >> property_name;
		if(!iss)
			Parameter::throw_parameter_error("Unable to read property name in parameter specification", param_spec);
		Symbol prop_name(property_name);
	// second term is delay value
		long delay;
		iss >> delay;
		if(!iss)
			Parameter::throw_parameter_error("Unable to read recoding delay in parameter specification", param_spec);
		recoding_times[prop_name] = delay;
		}
	else if(param_spec.parameter_name == "Detection_category") {
		istringstream iss(param_spec.specification);
		// first term is property name
		string detection_category_name;
		iss >> detection_category_name;
		if(!iss)
			Parameter::throw_parameter_error("Unable to read Detection_category name in parameter specification", param_spec);
		Symbol cat_name(detection_category_name);
	// second through nth term is content of word to be assign to that category, must be at least one
		string word_content;
		int count = 0;
		while(iss >> word_content) {
				detection_categories[word_content] = cat_name;
			count++;
			}
		if(count <= 0)
			Parameter::throw_parameter_error("Did not find at least one word_content for Detection_category", param_spec);
		}
	else if(param_spec.parameter_name == "Content_detection") {
		istringstream iss(param_spec.specification);
		// first term is detection category name
		string detection_category_name;
		iss >> detection_category_name;
		if(!iss)
			Parameter::throw_parameter_error("Unable to read Detection Category name in parameter specification", param_spec);
		Symbol cat_name(detection_category_name);
	// second, third, fourth term is detection function mean, sd, lapse rate
		double mean, sd, lapse_rate;
		iss >> mean >> sd >> lapse_rate;
		if(!iss)
			Parameter::throw_parameter_error("Unable to read mean, sd, lapse rate in parameter specification", param_spec);
		detection_parameters[cat_name] = Detection{mean, sd, lapse_rate};
		}
	else if(param_spec.parameter_name == "Recoding_category") {
		istringstream iss(param_spec.specification);
		// first term is property name
		string category_name;
		iss >> category_name;
		if(!iss)
			Parameter::throw_parameter_error("Unable to read Recoding_category name in parameter specification", param_spec);
		Symbol cat_name(category_name);
	// second through nth term is content of word to be assign to that category, must be at least one
		string word_content;
		int count = 0;
		while(iss >> word_content) {
				category_recodings[word_content] = cat_name;
			count++;
			}
		if(count <= 0)
			Parameter::throw_parameter_error("Did not find at least one word_content for Recoding_category", param_spec);
		}
	else {
		Human_subprocessor::set_parameter(param_spec);
		}
}

void Auditory_perceptual_processor::describe_parameters(Output_tee& ot) const
{
	Human_subprocessor::describe_parameters(ot);

	ot<< "Category_recoding:" << endl;
	for (const auto& pair : category_recodings)
		ot << "    " << pair.first << ": recoded to " << pair.second << endl;
	
	ot<< "Recoding_time:" << endl;
	for(Recoding_times_t::const_iterator it = recoding_times.begin(); it != recoding_times.end(); ++it) {
		ot << "    " << it->first << ": recoded after " << it->second << " ms." << endl;
		}

	ot<< "Categories:" << endl;
	for (const auto& pair : category_recodings)
		ot << "    " << pair.first << ": classified as " << pair.second << endl;
	
	ot<< "Detection parameters:" << endl;
	for (const auto& pair : detection_parameters)
		ot << "    " << pair.first << ": mean = " << pair.second.mean <<
			", sd = " << pair.second.sd << ", lapse rate = " << pair.second.lapse_rate << endl;
}

void Auditory_perceptual_processor::accept_event(const Auditory_event * event_ptr)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << typeid(*event_ptr).name() << " received" << endl;
	broadcast_to_recorders(event_ptr);
	// tell the event to handle itself with itself!
	event_ptr->handle_self(this);
}

void Auditory_perceptual_processor::accept_event(const Stop_event *)
{
//	perceptual_space.display_contents(Normal_out);
}

/*
* Auditory sound functions. 
*These impose a delay window by scheduling an event to come back so that simultaneous sounds can be
*detected and processed since the other sound will be present also.
*/

void Auditory_perceptual_processor::make_new_sound_start(const Symbol& sound_name, const Symbol& stream_name, long time_stamp,
	GU::Point location, const Symbol& timbre, double loudness, long intrinsic_duration)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "start sound " << sound_name << endl;
	
	schedule_event(new Auditory_Sound_Start_event(
		get_time() + appearance_window.get_long_value(), this,
		sound_name, stream_name, time_stamp, location, timbre, loudness, intrinsic_duration));
}


// here after the time window delay
void  Auditory_perceptual_processor::handle_event(const Auditory_Sound_Start_event * event_ptr)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "handle start sound event " << event_ptr->sound_name << endl;
		
	make_new_sound_start_f(event_ptr->sound_name, event_ptr->stream_name, event_ptr->time_stamp, event_ptr->location,
		event_ptr->timbre, event_ptr->loudness, event_ptr->intrinsic_duration);
}

void Auditory_perceptual_processor::make_new_sound_start_f(const Symbol& sound_name, const Symbol& stream_name, long time_stamp, GU::Point location,
	const Symbol& timbre, double loudness, long intrinsic_duration)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "make sound " << sound_name << " start" << endl;
	if(get_trace() && Trace_out) {
		Symbol_list_t sounds_present = sensory_store_ptr->get_sound_name_list();
		Trace_out << "sounds now present: " << sounds_present << endl;
		}
    
	long appearance_delay_time = appearance_delay.get_long_value();
	
	// if an encoder has been set, apply it to the location
	if(auditory_encoder_ptr) {
		GU::Point new_location = auditory_encoder_ptr->recode_location(location);
		location = new_location;
		appearance_delay_time = recoded_location_delay.get_long_value();
		}

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


	// transmit the start event forward to the next store
	schedule_event(new Auditory_Sound_Start_event(
		get_time() + appearance_delay_time, perceptual_store_ptr,
		sound_name, stream_name, time_stamp, location, timbre, loudness, intrinsic_duration));
	// create and send a transient property forward - Onset arrives at same time, then disappears
	schedule_event(new Auditory_Sound_Set_Property_event(
		get_time() + appearance_delay_time, perceptual_store_ptr,
		sound_name, Detection_c, Onset_c));
	// arrange to remove the transient after its delay
	schedule_event(new Auditory_Sound_Set_Property_event(
		get_time() + appearance_delay_time + transient_decay_time.get_long_value(), perceptual_store_ptr,
		sound_name, Detection_c, Nil_c));

}


// This function works for both speech and non-speech sounds

void Auditory_perceptual_processor::make_sound_stop(const Symbol& sound_name)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "make sound stop " << sound_name << endl;

// do not impose a time window on stop events but do check to see if we got masking before sending it on
	make_sound_stop_f(sound_name);
}


// here after the time window delay
void  Auditory_perceptual_processor::handle_event(const Auditory_Sound_Stop_event * event_ptr)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "handle sound stop event " << event_ptr->sound_name << endl;
		
	make_sound_stop_f(event_ptr->sound_name);
}


void Auditory_perceptual_processor::make_sound_stop_f(const Symbol& sound_name)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "make sound stop " << sound_name << endl;
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
		get_time() + disappearance_delay.get_long_value(), perceptual_store_ptr,
		sound_name));
	
	// create and send a transient property forward
	schedule_event(new Auditory_Sound_Set_Property_event(
		get_time() + disappearance_delay.get_long_value(), perceptual_store_ptr,
		sound_name, Detection_c, Offset_c));
	// arrange to remove the transient after its delay
	schedule_event(new Auditory_Sound_Set_Property_event(
		get_time() + disappearance_delay.get_long_value() + transient_decay_time.get_long_value(), perceptual_store_ptr,
		sound_name, Detection_c, Nil_c));

	// get the stream (if any) for this sound
	shared_ptr<Auditory_sound> sound_ptr = sensory_store_ptr->get_sound_ptr(sound_name);
	const Symbol& stream_name = sound_ptr->get_property_value(Stream_assignment_c);
	// if there is no stream associated with this sound, then quit
	if(stream_name == Nil_c)
		return;
	long destroy_time = get_time() + stream_destroy_delay.get_long_value();
	stream_tracker_ptr->set_stream_disappearance_time(stream_name, destroy_time);
	
	// schedule event to destroy our stream object in the future if not refreshed
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "destroy event for " << stream_name << " scheduled for " << destroy_time << endl;
	schedule_event(new Auditory_Stream_Destroy_event(
		destroy_time, this,
		stream_name));

}

// These functions apply to both speech and non-speech sounds
// sound property changes are also window-delayed so that masking might happen
Symbol Auditory_perceptual_processor::set_sound_property(const Symbol& obj_name, const Symbol& prop_name,
	const Symbol& prop_value)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "set sound " << obj_name 
			<< " property " << prop_name << " to " << prop_value << endl;

	schedule_event(new Auditory_Sound_Set_Property_event(
		get_time() + property_window.get_long_value(), this,
		obj_name, prop_name, prop_value));

/*		
	// go head and send the property forward
	long delay = property_delay.get_value();
	// queue a message to the sensory store
	schedule_event(new Auditory_Sound_Set_Property_event(get_time() + delay, 
		get_human_ptr()->get_auditory_sensory_store_ptr(), obj_name, prop_name, prop_value));
*/
	return prop_value;
}

// here after the time window delay
void  Auditory_perceptual_processor::handle_event(const Auditory_Sound_Set_Property_event * event_ptr)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "handle set property event " << event_ptr->sound_name << endl;
		
	set_sound_property_f(event_ptr->sound_name, event_ptr->property_name, event_ptr->property_value);
}

// this might not be needed
// basic assumption: non-core properties are sent after a recoding time that is the intrinsic duration of the sound plus a recoding time
// not clear if this is sensible if a property changes well after the sound starts -- dk 4/19/10

Symbol Auditory_perceptual_processor::set_sound_property_f(const Symbol& sound_name, const Symbol& prop_name,
	const Symbol& prop_value)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "set sound " << sound_name
			<< " property " << prop_name << " to " << prop_value << endl;
	// check to see if the sound is masked
	shared_ptr<Auditory_sound> sound_ptr = sensory_store_ptr->get_sound_ptr(sound_name);
	// if content is masked, do nothing - dk 5/22/10
	if(sound_ptr->get_property_value(Acoustic_status_c) == Content_masked_c) {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << "set sound " << sound_name
				<< " property " << prop_name << " to " << prop_value << " suppressed due to content masking" << endl;
		return prop_value;
		}
		
	// look up recoding time - no substantive recoding here - MUST FIX!
	Recoding_times_t::iterator it = recoding_times.find(prop_name);
	if(it == recoding_times.end())
		throw Unknown_perceptual_property(this, prop_name);
	long property_recoding_time = it->second;
    /* temp!!! */
    long intrinsic_duration = 0;
    
	long recoding_time = long((property_recoding_time + intrinsic_duration) * recoding_time_fluctuation.get_double_value());
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << sound_name
			<< " property " << prop_name << " recognized after " << intrinsic_duration << " + " <<  property_recoding_time
			<< " for fluctuated total of " << recoding_time << endl;


	// transmit the event forward to the next store
	schedule_event(new Auditory_Sound_Set_Property_event(
		get_time() + recoding_time, perceptual_store_ptr,
		sound_name, prop_name, prop_value));

	return prop_value;
}


/*
* Auditory speech functions.
*These impose a delay window by scheduling an event to come back so that simultaneous sounds can be
*detected and processed since the other sound will be present also.

make_sound_stop and set_sound_property defined above apply to both non-speech and speech sounds
*/

void Auditory_perceptual_processor::make_new_speech_start(const Speech_word& word)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "schedule start speech event" << word.name << endl;
	
	// create an event to come back to us at the end of the appearance_window time.
	schedule_event(new Auditory_Speech_Start_event(
		get_time() + appearance_window.get_long_value(), this,
		word));
}

void  Auditory_perceptual_processor::handle_event(const Auditory_Speech_Start_event * event_ptr)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "handle start speech event " << event_ptr->word.name << endl;
		
	make_new_speech_start_f(event_ptr->word);
}

/*
make_new_speech_start_f 
--- 5/22/15
sound location is perturbed with localization in orginal word before arriving here,
correspond to how pitch and loudness already have noise in them.

--- as of 10/2014 - removed hardwired content categories and removed Brungart condition-specifying kludge
--- as of 2/2012
Implemented "stream tracker" in which each word is assigned to a stream object based on loudness and pitch,
and then the stream object is updated with the loudness and pitch of its assigned word.

--- 1/16/2012
For sanity, earlier code that was commented out has been removed. See previous versions if needed.

This version adopts a much simpler preliminary masking model. The speaker-gender, and speaker-id field are neither
processed nor passed along. Instead loudness and pitch are used by the cognitive processor to associate words to streams.

As of 1/16/2012 the following rules implemented:

1. loudness and pitch will vary at the source (the device) stochastically according to speaker/gender.
2. loudness and pitch are always passed through - no masking at this time.
3. content is probability masked using a function of SNR only at this time

Assume we have two streams, stream1 and stream2.
--- as of 4/27/2012
attempt to scale up to four streams, stream1, stream2, stream3, stream4

*/

void Auditory_perceptual_processor::make_new_speech_start_f(const Speech_word& word)
{
	Symbol_list_t sounds_present = sensory_store_ptr->get_not_fading_sound_name_list();

	if(get_trace() && Trace_out) {
		Trace_out << "non-fading sounds now present: " << sounds_present << endl;
		}
    // make a copy of the speech word to be propagated - all attributes are copied
    Speech_word word_propagated = word;
	
	// defaults for pass-through
	Symbol content_propagated = word.content;
	Symbol stream_propagated = word.stream_name;

	// if more than one sound present, apply masking mechanism
	// and stream tracking mechanism
	int n_sounds_present = int(sounds_present.size());
	if(n_sounds_present > 1) {
		Assert(n_sounds_present <= max_n_sounds_present);

        const double effective_snr_loudness_difference_weight = effective_snr_loudness_weight.get_double_value();
        const double effective_snr_pitch_difference_weight = effective_snr_pitch_weight.get_double_value();
        const double effective_snr_location_difference_weight = effective_snr_location_weight.get_double_value();
        const double pitch_difference_cap = pitch_difference_max.get_double_value();
        double loudness_weight = stream_loudness_weight.get_double_value();
        double pitch_weight = stream_pitch_weight.get_double_value();
        double location_weight = stream_location_weight.get_double_value();
        double theta = stream_theta.get_double_value();
        Symbol this_name = word.name;
        
        // get a list of the other sounds present - this is a list of their names
        Symbol_list_t other_sounds_present;
        remove_copy(sounds_present.begin(), sounds_present.end(), back_inserter(other_sounds_present), this_name);
        Assert(other_sounds_present.size() <= n_sounds_present - 1);

        // collect the relevant properties from the object in sensory store
        shared_ptr<Auditory_sound> this_sound_ptr = sensory_store_ptr->get_sound_ptr(word.name);
		const Symbol& this_content = this_sound_ptr->get_content();
		double this_loudness = this_sound_ptr->get_loudness();
		double this_level_right = this_sound_ptr->get_level_right();
		double this_level_left = this_sound_ptr->get_level_left();
//        if(this_level_right != this_level_left)
//            Normal_out << "left and right levels are different " << this_level_left << ' ' << this_level_right << endl;
        // all the pitches are being supplied as semitones
		double this_pitch = this_sound_ptr->get_pitch();
        // only using azimuth of location
		double this_azimuth = this_sound_ptr->get_location().x;
        Symbol this_gender = this_sound_ptr->get_gender();
        Symbol this_speaker_id = this_sound_ptr->get_speaker_id();

		// calculate effective loudness snr based on other sounds present
		double total_masking_power = 0.;
		double total_left_masking_power = 0.;
		double total_right_masking_power = 0.;
        // depending on the conditions, these might not have a single value, but should be well defined compared to
        // the attributes for "this" sound.
		pitch_stats.reset();
		location_stats.reset();
//        vector<double> pitch_differences;
//        vector<double> azimuth_differences;
		for(Symbol_list_t::iterator it = other_sounds_present.begin(); it != other_sounds_present.end(); ++it) {
			shared_ptr<Auditory_sound> other_sound = sensory_store_ptr->get_sound_ptr(*it);
			total_masking_power += pow(10., other_sound->get_loudness() / 10.);
			total_right_masking_power += pow(10., other_sound->get_level_right() / 10.);
			total_left_masking_power += pow(10., other_sound->get_level_left() / 10.);
			// compute pitch statistics for other sounds, note that semitones are present
			pitch_stats.update(other_sound->get_pitch());
            // compute location statistics for other sounds, azimuth only; 0 azimuth is straight ahead
            double other_azimuth = other_sound->get_location().x;
//            double azimuth_difference = fabs((this_azimuth < other_azimuth) ? other_azimuth - this_azimuth : this_azimuth - other_azimuth);
            // we are computing the average azimuth for the other sounds, so for 0, 60 degrees, it will be 30 degrees
            location_stats.update(other_azimuth);
			}

		double total_masking_db = 10. * log10(total_masking_power);
		double diotic_snr = this_loudness - total_masking_db;
//        Normal_out << "this_loudness, diotic_snr " << this_name << ' ' << this_loudness << ' ' << diotic_snr << endl;
		double total_right_masking_db = 10. * log10(total_right_masking_power);
		double right_snr = this_level_right - total_right_masking_db;
//        Normal_out << "this_level_right, right_snr " << this_level_right << ' ' << right_snr << endl;
		
		double total_left_masking_db = 10. * log10(total_left_masking_power);
		double left_snr = this_level_left - total_left_masking_db;
//        Normal_out << "this_level_left, left_snr " << this_level_left << ' ' << left_snr << endl;
		
		double loudness_snr = (right_snr > left_snr) ? right_snr : left_snr;
		
//		Normal_out << "best ear snr " << loudness_snr << endl;


		double pitch_difference = fabs(this_pitch - pitch_stats.get_mean());
		if(pitch_difference > pitch_difference_cap)
			pitch_difference = pitch_difference_cap;
        
        double other_azimuth_mean = location_stats.get_mean();

        // OK with this as long as location weight for effective SNR is zero
        double azimuth_difference = Ear_processor::get_scaled_perceived_azimuth_difference(this_azimuth, other_azimuth_mean);
        
        // combine as weighted sum of loudness, pitch, and location differences
		double snr = effective_snr_loudness_difference_weight * loudness_snr +
            effective_snr_pitch_difference_weight * pitch_difference +
            effective_snr_location_difference_weight * azimuth_difference;

		// SPECIAL ??? FIX FIX FIX temporary kludge to force tracking-only processing; force SNR to be zero regardless
		/////////snr = 0.0;
    
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << "processing " << this_sound_ptr->get_name() << " db " 
				<< this_loudness << " masking " << total_masking_db << " " << loudness_snr << " "
				<< this_pitch << " compared to " << pitch_stats.get_mean() << " " << pitch_difference << " "
                << this_azimuth << " compared to " << location_stats.get_mean() << " " << azimuth_difference << " "
                << snr << endl;

		// Because we wait until all words have arrived, at this point they all have,
        // but we have to process them one at a time.
        // So we have to check on whether we have assigned words to the sreams yet. 
		// determine if all words are unassigned
        // this would be faster if we had a container of sound_ptrs instead.
		bool words_unassigned = true;
		for(Symbol_list_t::iterator it = sounds_present.begin(); it != sounds_present.end(); ++it) {
			shared_ptr<Auditory_sound> sound_ptr = sensory_store_ptr->get_sound_ptr(*it);
			// all sounds must be unassigned for words_unassigned to end up as true
			words_unassigned = words_unassigned && (sound_ptr->get_property_value(Stream_assignment_c) == Nil_c);
			}
			       
		// We need to assign each word to one stream so that each stream has one word.
		// Veridical stream_ID in word is ignored
		if(words_unassigned) {
            // give the stream tracker the sounds
            stream_tracker_ptr->clear_sounds();
            for(Symbol_list_t::iterator it = sounds_present.begin(); it != sounds_present.end(); ++it)
                stream_tracker_ptr->add_sound(sensory_store_ptr->get_sound_ptr(*it));
 
            // now ready to do stream assignment work - have we started the streams yet?
            // note that long delay will result in streams being deleted, so "resets" automatically
			if(!stream_tracker_ptr->are_streams_started()) {
				stream_tracker_ptr->create_initial_streams();
				// get streams after assignment
				const vector<std::shared_ptr<Stream_base>>& streams = stream_tracker_ptr->get_streams();
				// request creation of a stream object in the auditory store for each stream
				for(auto the_stream : streams) {
					create_stream(
						the_stream->get_name(),
						the_stream->get_predicted_pitch(),
						the_stream->get_predicted_loudness(),
						GU::Point(the_stream->get_predicted_azimuth(), 0.)
//						GU::Point() // default location is center
						);
					}

				} // done with creating initial streams

			// do this only after the first word
			else if (stream_tracker_ptr->are_streams_started()) { // assign sounds to existing streams
				// this work does not require distingishing "this_sound" from the other sounds
				// set parameters in the stream tracker
				stream_tracker_ptr->set_loudness_weight(loudness_weight);
				stream_tracker_ptr->set_pitch_weight(pitch_weight);
				stream_tracker_ptr->set_location_weight(location_weight);
				stream_tracker_ptr->set_theta(theta);
				
				stream_tracker_ptr->assign_sounds_to_streams();
				// get streams after assignment
				const vector<std::shared_ptr<Stream_base>>& streams = stream_tracker_ptr->get_streams();
				// update the stream object in the auditory store for each stream
				for(auto the_stream : streams) {
					set_stream_pitch(the_stream->get_name(), the_stream->get_predicted_pitch());
					set_stream_loudness(the_stream->get_name(), the_stream->get_predicted_loudness());
					set_stream_location(the_stream->get_name(), GU::Point(the_stream->get_predicted_azimuth(), 0.));
					}
							
				} // done with assigning sounds to existing streams
			} // done with stream assignment work

		// at this point the stream assignment/updating has been done for both this sound / word and the other sound present as well.
		// the stream id to be propagated for the current word has been saved with the sound
		// and is one of the arbitrary symbols in the form of "Stream1" etc.
		stream_propagated = this_sound_ptr->get_property_value(Stream_assignment_c);
		Assert(stream_propagated != Nil_c);
        
        // get the detection category for the content
        auto detection_category_it = detection_categories.find(this_content);
        Symbol this_detection_category = (detection_category_it != detection_categories.end()) ? detection_category_it->second : Nil_c;

		bool content_detected = false;
		bool stream_detected = true; // stream attributes always passed through (loudness, pitch) along with stream_id
		
		// kluge for verbal WM models where one of the speakers is Self - covert rehearsal - no masking!
/*		if(this_sound_ptr->get_speaker_id() == Self_c || other_sound_ptr->get_speaker_id() == Self_c) {	
// Covert speech during input
			content_detected = true;
			speaker_detected = true;
			reloud_detected = true;
			stream_detected = true;
			}
*/

// content is passed through as a function of effective snr only
    //	simple detection model
        {
            auto detection_it = detection_parameters.find(this_detection_category);
            if (detection_it == detection_parameters.end())
                Assert(!"Detection category unknown!");
            Detection detection_parms = detection_it->second;
	
            double detection_mean = detection_parms.mean;
            double detection_sd = detection_parms.sd;
            double detection_lapse_rate = detection_parms.lapse_rate;
	
            content_detected = lapsed_gaussian_detection_function(snr, detection_mean, detection_sd, detection_lapse_rate);
        }
 
		// apply the content masking result
		content_propagated = (content_detected) ? word.content : Nil_c;
		if(!content_detected) {
			if(get_trace() && Trace_out)
				Trace_out << processor_info() << "speech sound " << word.name << " content " << word.content << " is masked" << endl;
			// mark this speech sound as masked
			this_sound_ptr->set_property(Acoustic_status_c, Content_masked_c);
			}
		
		// apply the stream masking result
		if(!stream_detected) {
			if(get_trace() && Trace_out)
				Trace_out << processor_info() << "speech sound " <<word.name << " stream id " << word.stream_name << " is masked" << endl;
			}
	
	}
	
	// populate the speech event to be propagated
	word_propagated.stream_name = stream_propagated;
	word_propagated.content = content_propagated;

	schedule_event(new Auditory_Speech_Start_event(
		get_time() + appearance_delay.get_long_value(), perceptual_store_ptr,
		word_propagated));
 
    	// create and send a transient property forward - Onset arrives at same time, then disappears
	schedule_event(new Auditory_Sound_Set_Property_event(
		get_time() + appearance_delay.get_long_value(), perceptual_store_ptr,
		word.name, Detection_c, Onset_c));
	// arrange to remove the transient after its delay
	schedule_event(new Auditory_Sound_Set_Property_event(
		get_time() + appearance_delay.get_long_value() + transient_decay_time.get_long_value(), perceptual_store_ptr,
		word.name, Detection_c, Nil_c));
		
	// send categorized content forward after a delay, but only if a recoding is found
	auto recoding_category_it = category_recodings.find(word.content);
	Recoding_times_t::iterator time_it = recoding_times.find(Category_c);
	if(recoding_category_it != category_recodings.end() && time_it != recoding_times.end()) {
		Symbol recoded_content = recoding_category_it->second;
		long property_recoding_time = time_it->second;
//		long recoding_time = long((property_recoding_time + duration) * recoding_time_fluctuation.get_double_value());
		long recoding_time = long((property_recoding_time) * recoding_time_fluctuation.get_double_value()); // dk 8/11/11 to speed things up

		schedule_event(new Auditory_Sound_Set_Property_event(
			get_time() + recoding_time, perceptual_store_ptr,
			word.name, Category_c, recoded_content));
		}
}


/*
	A delay event serves the purpose of allow a property to be transmitted after a time delay, 
	during which it can be cancelled by intervening events. A delay event contains
	the object, property, and values for a property-change event, 
	and the property-event-time at which the property is supposed to be delivered.
	If the delay event refers to a non-existent object, it is discarded.
	If the delay event arrives at a time that does not match the delivery time for the
	property that was stored with the object, it is discarded. Thus if the property event time
	has been set to zero, it means te delivery should be canceled. If the property event time
	mismatches and is larger, it means that a subsequent delivery has been scheduled, and this
	delivery should not be done.
	
	above copied, not edited, from original place in Visual_perceptual_processor
*/
void Auditory_perceptual_processor::handle_event(const Auditory_Delay_event * event_ptr)
{
	Symbol object_name = event_ptr->object_name;
	Symbol property_name = event_ptr->property_name;
	Symbol property_value = event_ptr->property_value;
	// check to see if the object is still present, do nothing if not
	if(!(sensory_store_ptr->is_sound_present(object_name))) {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << object_name 
				<< " no longer present for delay event of " << property_value << " set to " << property_value << endl;
		return;
		}

	// if an encoder has been set, apply it and do nothing more if it wants to override the default behavior
	if(auditory_encoder_ptr && auditory_encoder_ptr->handle_Delay_event(object_name, property_name, property_value))
		return;

	// access the object
//	std::shared_ptr<Auditory_sound> obj_ptr = get_sensory_object_ptr(object_name);
    std::shared_ptr<Auditory_sound> obj_ptr = sensory_store_ptr->get_sound_ptr(object_name);
	// check to see if the object's event time for the property and the property value matches this event; if not, do nothing
//	long object_time = obj_ptr->get_property_event_time(property_name);
	Symbol object_property_value = obj_ptr->get_property_value(property_name);
/*	if(object_time != event_ptr->get_time() || object_property_value != property_value) {
		if(get_trace() && Trace_out)
			Trace_out << processor_info() << "Delay event discarded for " << object_name 
				<< " because event time and value " << event_ptr->get_time() << " " << property_value << " does not match stored " << object_time << " " << object_property_value << endl;
		return;
		}
*/
	// here if event matches - reset the stored event time, and transmit the information immediately to the perceptual store and 
	// transmit the event forward to the next store
//	obj_ptr->remove_property_event_time(property_name);

	schedule_event(new Auditory_Sound_Set_Property_event(
		get_time(), perceptual_store_ptr,
		object_name, property_name, property_value));

}


shared_ptr<Auditory_sound> Auditory_perceptual_processor::get_sensory_object_ptr(const Symbol& object_name)
{
	return sensory_store_ptr->get_sound_ptr(object_name);
/*	Auditory_sound * p = get_human_ptr()->get_Auditory_sensory_store_ptr()->get_sound_ptr(object_name);
	Smart_Pointer<Auditory_sound> obj_ptr = dynamic_cast<Auditory_sound *>(p);
	if(!obj_ptr)
		throw Epic_internal_error(this, "object was not an Auditory_sound");
	return obj_ptr;
*/
}


/*
* Stream representation functions
* These functions manipulate the Stream object representation in perceptual store. 
* The actual stream tracking is done with objects that are part of this processor.
*/
void Auditory_perceptual_processor::create_stream(const Symbol& name, double pitch_, double loudness_, GU::Point location)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "create stream " << name << " with " << pitch_ << ' ' << loudness_ << endl;
	
	// transmit the event forward to the next store
	schedule_event(new Auditory_Stream_Create_event(
		get_time() + appearance_delay.get_long_value(), get_human_ptr()->get_auditory_perceptual_store_ptr(),
		name, pitch_, loudness_, location));
}

void Auditory_perceptual_processor::destroy_stream(const Symbol& name)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "destroy stream " << name << endl;

	// transmit the event forward to the next store
	schedule_event(new Auditory_Stream_Destroy_event(
		get_time() + disappearance_delay.get_long_value(), perceptual_store_ptr,
		name));
}


void Auditory_perceptual_processor::handle_event(const Auditory_Stream_Destroy_event * event_ptr)
{
	// find the stream holder object with this name and get its disappearance time
	// if same as event time, then delete this stream object and send a stream destroy event downstream
	long event_time = event_ptr->get_time();
	Symbol stream_name = event_ptr->stream_name;
	// find the stream holder object with this name and disappearance time and get rid of it
    // need a copy of the stream container, because we modify the original in this process
   // updating the stream is supposed to reset its disappearance_time to 0, so if it has been
    // updated after the destroy even was schedule, it should fail to match the ccurrent time.
    vector<std::shared_ptr<Stream_base>> streams = stream_tracker_ptr->get_streams();
    for(auto the_stream : streams) {
        if(the_stream->get_name() == stream_name && the_stream->get_disappearance_time() == event_time) {
            if(get_trace() && Trace_out)
                Trace_out << processor_info() << "stream holder" << the_stream->get_name() << " deleted stream scheduled to be destroyed" << endl;
            stream_tracker_ptr->delete_stream(stream_name);
            schedule_event(new Auditory_Stream_Destroy_event( 
                    get_time() + disappearance_delay.get_long_value(), perceptual_store_ptr,
                    stream_name));
            return;  // only one stream event supposed to be possible
			}
		}



	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "destroy event for " << stream_name << " discarded because " << event_time << " not matched" << endl;
}




// simply update the location stored with the object - 
GU::Point Auditory_perceptual_processor::set_stream_location(const Symbol& name, GU::Point location)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "set stream " << name
			<< " location to " << location << endl;
			
	
	// transmit the event forward to the next store
	schedule_event(new Auditory_Stream_Set_Location_event(
		get_time() + location_delay.get_long_value(), perceptual_store_ptr,
		name, location));
	return location;
}

// simply update the pitch stored with the object - 
double Auditory_perceptual_processor::set_stream_pitch(const Symbol& name, double pitch)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "set stream " << name
			<< " pitch to " << pitch << endl;
			
	
	// transmit the event forward to the next store
	schedule_event(new Auditory_Stream_Set_Pitch_event(
		get_time() + location_delay.get_long_value(), perceptual_store_ptr,
		name, pitch));
	return pitch;
}

double Auditory_perceptual_processor::set_stream_loudness(const Symbol& name, double loudness)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "set stream " << name
			<< " loudness to " << loudness << endl;
			
	
	// transmit the event forward to the next store
	schedule_event(new Auditory_Stream_Set_Loudness_event(
		get_time() + location_delay.get_long_value(), perceptual_store_ptr,
		name, loudness));
	return loudness;
}

// simply update the location stored with the object - 
GU::Size Auditory_perceptual_processor::set_stream_size(const Symbol& name, GU::Size size)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "set stream " << name
			<< " size to " << size << endl;
			
	
	// transmit the event forward to the next store
	schedule_event(new Auditory_Stream_Set_Size_event(
		get_time() + location_delay.get_long_value(), perceptual_store_ptr,
		name, size));
	return size;
}

Symbol Auditory_perceptual_processor::set_stream_property(const Symbol& object_name, const Symbol& property_name, const Symbol& property_value)
{
	if(get_trace() && Trace_out)
		Trace_out << processor_info() << "set stream " << object_name 
			<< " property " << property_name << " to " << property_value << endl;

	// look up recoding time - no substantive recoding here - MUST FIX!
	Recoding_times_t::iterator it = recoding_times.find(property_name);
	if(it == recoding_times.end())
		throw Unknown_perceptual_property(this, property_name);
	long recoding_time = it->second;
	recoding_time = long(recoding_time * recoding_time_fluctuation.get_double_value());
	// transmit the event forward to the next store
	schedule_event(new Auditory_Stream_Set_Property_event(
		get_time() + recoding_time, perceptual_store_ptr,
		object_name, property_name, property_value));
	return property_value;
}


