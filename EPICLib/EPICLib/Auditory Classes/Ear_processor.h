#ifndef EAR_PROCESSOR_H
#define EAR_PROCESSOR_H

//#include <map>

//#include "Output_tee.h"
#include "Human_subprocessor.h"
#include "Auditory_store_processor.h"
#include "Name_map.h"
#include "Parameter.h"
#include "Auditory_physical_store.h"
#include "Auditory_event_types.h"
#include "Statistics.h"
#include "Geometry.h"
namespace GU = Geometry_Utilities;
#include <map>

class Human_processor;
class Symbol;
struct Speech_word;
//class Stream_tracker_base;

/* The Ear_processor accepts direct inputs from Auditory_physical_store, and emits events
to Auditory_sensory_store after the appropriate delays. It stores no information of its own.
*/

class Ear_processor : public Auditory_processor {
public:
	Ear_processor(Human_processor * human_ptr_) :
		Auditory_processor ("Ear", human_ptr_),
        // transduction and simultaneity windows are very short
		// delay for new streams and sounds to be passed on
		appearance_window("Appearance_window_time", 10),
//		appearance_delay("Appearance_transduction_time", 25),
		appearance_delay("Appearance_transduction_time", 5),
		disappearance_window("Disappearance_window_time", 10),
		// delay for cessation of streams and sounds to be passed on
		// this stretches out the effective duration of a stream or sound
//		disappearance_delay("Disappearance_transduction_time", 100),
		disappearance_delay("Disappearance_transduction_time", 5),
		// delay for passing on a change in stream location or size
		location_delay("Location_change_transduction_time", 5),
		// delay for passing on a change in a property
//		property_window("Property_window_time", 15),
		property_window("Property_window_time", 10),
//		property_delay("Property_transduction_time", 100),
		property_delay("Property_transduction_time", 5),
        physical_store_ptr(nullptr),sensory_store_ptr(nullptr)
		{setup();}

	void initialize() override;
	
	void set_parameter(const Parameter_specification& param_spec) override;
	void describe_parameters(Output_tee& ot) const override;

	/* direct inputs -
	   object name is supplied along with all relevant information - no backwards lookup necessary
	   returns true if a change was made to data base (and so input should be processed further) and false otherwise.
	   Some functions return the previous value, and do nothing unless it is different from the new value.
	*/
	// sound inputs
	virtual void make_new_sound_start(const Symbol& sound_name, const Symbol& stream_name, long time_stamp, GU::Point location, const Symbol& timbre, double loudness, long intrinsic_duration);
//	virtual void make_new_speech_start(const Symbol& sound_name, const Symbol& stream_name, long time_stamp, 
//		const Symbol& content, const Symbol& speaker_gender, const Symbol& speaker_id, double loudness, long duration);
	void make_new_speech_start(const Speech_word& word) override;
    void make_sound_stop(const Symbol& sound_name) override;
	Symbol set_sound_property(const Symbol& sound_name, const Symbol& propname, const Symbol& propvalue) override;


	// event interface
//	void accept_event(const Start_event *) override;
	void accept_event(const Stop_event *) override;
	void accept_event(const Auditory_event *) override;
	
	// event interface to produce processing window
	void handle_event(const Auditory_Sound_Start_event *) override;
	void handle_event(const Auditory_Speech_Start_event *) override;
	void handle_event(const Auditory_Sound_Stop_event *) override;
	void handle_event(const Auditory_Sound_Set_Property_event * event_ptr) override;

	// allow outsiders to access and modify the name map - e.g. to remove names
	Name_map& get_name_map()
		{return name_map;}
    
    // parametric services needed by other processors
    static double get_auditory_location_sd (double azimuth);
    static double get_scaled_perceived_azimuth_difference(double azimuth1, double azimuth2);


private:
	// state
	long speech_item_counter;	// counter for new speech input identification symbols
	
	Name_map name_map;
	// parameters
	Parameter appearance_window;
	Parameter appearance_delay;
	Parameter disappearance_window;
	Parameter disappearance_delay;
	Parameter location_delay;
	Parameter property_window;
	Parameter property_delay;
    
    // pointers to upstream and downstream stores
    Auditory_physical_store * physical_store_ptr;
    Auditory_sensory_store * sensory_store_ptr;
    
	// helper functions
	void setup();
	Symbol create_sound_name(const Symbol& physical_name);
	
	virtual void make_new_sound_start_f(const Symbol& sound_name, const Symbol& stream_name, long time_stamp, GU::Point location,
		const Symbol& timbre, double loudness, long intrinsic_duration);
	virtual void make_new_speech_start_f(const Speech_word& word);
	Symbol set_sound_property_f(const Symbol& sound_name, const Symbol& prop_name, const Symbol& prop_value);
	void make_sound_stop_f(const Symbol& sound_name);
    
	double Shaw_2500_function(double azimuth_);
	double KEMAR_2500_function(double azimuth_);
	double LISTEN_avg_function(double azimuth_);
	double LISTEN_band_importance_function(double azimuth_);
    double apply_head_shadowing_attenuation(double original_loudness, GU::Point location);
    GU::Point apply_auditory_location_error(GU::Point original_location);
    double modify_azimuth(double);
    double compute_bias(double);
    double compute_reversal(double);


};

#endif
