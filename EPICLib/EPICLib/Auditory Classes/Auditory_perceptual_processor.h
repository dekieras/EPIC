/* The auditory perceptual processor accepts direct calls from the Auditory_sensory_store,
and emits events to the Auditory_perceptual_store. It does any recoding (currently none)
and creates some transient events sent downstream (which probably should be done by the ear
processor).

*/

/* 
Auditory_sensory_store does direct calls to this processor, which recodes the information
and sends scheduled event to Auditory_perceptual_store, with various delays as appropriate,
and also transient properties ???

At this time, all perceptual recoding is done here - the Auditory_perceptual_store
simply passes-through relevant information to the cognitive processor.
*/


#ifndef AUDITORY_PERCEPTUAL_PROCESSOR_H
#define AUDITORY_PERCEPTUAL_PROCESSOR_H

#include "Auditory_processor.h"
#include "Auditory_event_types.h"
#include "Stream_tracker_base.h"
#include "Parameter.h"
#include "Statistics.h"
#include "Geometry.h"
namespace GU = Geometry_Utilities;
#include <map>
#include <memory>

class Human_processor;
class Symbol;
class Auditory_Delay_event;
class Auditory_encoder_base;
class Auditory_sensory_store;
class Auditory_perceptual_store;
struct Speech_word;
class Auditory_sound;

//class Stream_tracker_base;

class Auditory_perceptual_processor : public Auditory_processor {
public:
	Auditory_perceptual_processor(Human_processor * human_ptr_) :
		Auditory_processor ("Auditory_perceptual_processor", human_ptr_),
		// delay for new streams and sounds to be passed on
		appearance_window("Appearance_window_time", 10),
		appearance_delay("Appearance_recoding_time", 50),
		disappearance_window("Disappearance_window_time", 10),
		// delay for cessation of streams - needs to be greater than a word length
		stream_destroy_delay("Stream_destroy_delay", 1000),
		// this stretches out the effective duration of a stream or sound
		disappearance_delay("Disappearance_delay", 50),
		// delay for passing on a change in stream location or size
		location_delay("Location_change_recoding_time", 50),
		recoded_location_delay("Recoded_location_delay", 100),
		// fluctuations for delay for passing on a change in a property
//		property_time_fluctuation("Property_delay_fluctuation_factor", Parameter::Normal, Parameter::Never, 1.0, .5),
		property_window("Property_window_time", 15),
		recoding_time_fluctuation("Recoding_time_fluctuation_factor", Parameter::Normal, Parameter::Never, 1.0, .5),
		recoding_failure_rate1("Recoding_failure_rate1", 0.5), // Custom failure rate #1
		effective_snr_loudness_weight("Effective_snr_loudness_weight", 1.0),
		effective_snr_pitch_weight("Effective_snr_pitch_weight", 2.0),
		effective_snr_location_weight("Effective_snr_location_weight", 2.0),
		pitch_difference_max("Pitch_difference_cap", 4.0),
		stream_loudness_weight("Stream_loudness_weight", 0.20),   // formerly 1-stream_lambda
		stream_pitch_weight("Stream_pitch_weight", 0.80),    // formerly stream_lambda
		stream_location_weight("Stream_location_weight", 0.00),
		stream_theta("Stream_theta", 0.10),

		// time that transient signals last - e.g. onsets, offsets, changes
		transient_decay_time("Transient_decay_time", 75),
        sensory_store_ptr(nullptr),
        perceptual_store_ptr(nullptr),
        // choose our stream tracker at this point
        auditory_encoder_ptr(nullptr)
		{
			setup();
		}
		
    void initialize() override;
	
	// set externally with an address supplied by a dynamic library - connects this processor back to it.
	void set_auditory_encoder_ptr(Auditory_encoder_base * auditory_encoder_ptr_);
	Auditory_encoder_base * get_auditory_encoder_ptr() const
		{return auditory_encoder_ptr;}

	void set_parameter(const Parameter_specification& param_spec) override;
	void describe_parameters(Output_tee& ot) const override;
	
	/* direct inputs - 
	   object name is supplied along with all relevant information - no backwards lookup necessary
	   returns true if a change was made to data base (and so input should be processed further) and false otherwise.
	   Some functions return the previous value, and do nothing unless it is different from the new value.
	*/
	// stream inputs
//	virtual void create_stream(const Symbol& stream_name, GU::Point location, GU::Size size = GU::Size());
    virtual void create_stream(const Symbol& stream_name, double pitch, double loudness, GU::Point location); // not in base class!
	void destroy_stream(const Symbol& stream_name) override;
//	virtual void erase_stream(const Symbol& stream_name);
	virtual GU::Point set_stream_location(const Symbol& stream_name, GU::Point location) override;
	virtual double set_stream_pitch(const Symbol& stream_name, double pitch);
	virtual double set_stream_loudness(const Symbol& stream_name, double loudness);
    GU::Size set_stream_size(const Symbol& stream_name, GU::Size size) override;
	Symbol set_stream_property(const Symbol& stream_name, const Symbol& propname, const Symbol& propvalue) override;

	// sound inputs
	virtual void make_new_sound_start(const Symbol& sound_name, const Symbol& stream_name, long time_stamp, GU::Point location, const Symbol& timbre, double loudness, long intrinsic_duration);
//	virtual void make_new_speech_start(const Symbol& sound_name, const Symbol& stream_name, long time_stamp, 
//		const Symbol& content, const Symbol& speaker_gender, const Symbol& speaker_id, double loudness, long duration);
    void make_new_speech_start(const Speech_word& word) override;
    void make_sound_stop(const Symbol& sound_name) override;
//	virtual void make_sound_event(const Symbol& sound_name, const Symbol& stream_name, const Symbol& timbre, double loudness, long duration, long intrinsic_duration = 0);
//	virtual void make_speech_sound_event(const Symbol& sound_name, const Symbol& stream_name, const Symbol& utterance, double loudness, long duration);
//	virtual void erase_sound(const Symbol& sound_name);
    Symbol set_sound_property(const Symbol& sound_name, const Symbol& propname, const Symbol& propvalue) override;


	// event interface
//	virtual void accept_event(const Start_event *);
	void accept_event(const Stop_event *) override;
	void accept_event(const Auditory_event *) override;
	
	// event interface to produce processing window
	void handle_event(const Auditory_Sound_Start_event *) override;
	void handle_event(const Auditory_Speech_Start_event *) override;
	void handle_event(const Auditory_Sound_Stop_event *) override;
	void handle_event(const Auditory_Sound_Set_Property_event * event_ptr) override;
	
	// to handle stream erasure delay
	void handle_event(const Auditory_Stream_Destroy_event *) override;
	void handle_event(const Auditory_Delay_event *) override;
	
	// allow Auditory_encoder_base to have access
	friend class Auditory_encoder_base;


private:
	// parameters
	Parameter appearance_window;
	Parameter appearance_delay;
	Parameter disappearance_window;
	Parameter stream_destroy_delay;
	Parameter disappearance_delay;
	Parameter location_delay;
	Parameter recoded_location_delay;
	Parameter property_window;
//	Parameter property_time_fluctuation;
	Parameter recoding_time_fluctuation;
	Parameter recoding_failure_rate1;
	Parameter transient_decay_time;
	Parameter effective_snr_loudness_weight;
	Parameter effective_snr_pitch_weight;
	Parameter effective_snr_location_weight;
	Parameter pitch_difference_max;
	Parameter stream_loudness_weight;
	Parameter stream_pitch_weight;  // formerly stream_lambda
	Parameter stream_location_weight;
	Parameter stream_theta;
	struct Detection {
		double mean;
		double sd;
		double lapse_rate;
		};
	using Detection_categories_t = std::map<Symbol, Symbol>;
	Detection_categories_t detection_categories;
	using Detection_parameters_t = std::map<Symbol, Detection>;
	Detection_parameters_t detection_parameters;
	using Category_recodings_t = std::map<Symbol, Symbol>;
	Category_recodings_t category_recodings;
	typedef std::map<Symbol, long> Recoding_times_t;
	Recoding_times_t recoding_times;
	
	// pointers to upstream and downstream stores
    Auditory_sensory_store * sensory_store_ptr;
    Auditory_perceptual_store * perceptual_store_ptr;
    
	// pointer to custom encoder object
	Auditory_encoder_base * auditory_encoder_ptr;	// set externally with address supplied by a dynamic library

	// a pointer to the stream tracker in use
	std::unique_ptr<Stream_tracker_base> stream_tracker_ptr;
		
	// helper functions
	void setup();
	std::shared_ptr<Auditory_sound> get_sensory_object_ptr(const Symbol& object_name);
//	void initialize_recodings();
	
	virtual void make_new_sound_start_f(const Symbol& sound_name, const Symbol& stream_name, long time_stamp, GU::Point location,
		const Symbol& timbre, double loudness, long intrinsic_duration);
//	virtual void make_new_speech_start_f(const Symbol& sound_name, const Symbol& stream_name, long time_stamp, 
//		const Symbol& content, const Symbol& speaker_gender, const Symbol& speaker_id, double loudness, long duration);
	virtual void make_new_speech_start_f(const Speech_word& word);
	Symbol set_sound_property_f(const Symbol& sound_name, const Symbol& prop_name, const Symbol& prop_value);
	void make_sound_stop_f(const Symbol& sound_name);	
	
	Mean_accumulator pitch_stats;
	Mean_accumulator location_stats;
};

#endif
