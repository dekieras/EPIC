#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <set>
#include "Symbol.h"
#include "Parameter.h"

// forward declarations
class View_base;
class Device_base;
class Device_processor;
class Human_processor;
class Device_processor;
class Visual_encoder_base;
class Auditory_encoder_base;

class Model {
public:
	// constructor requires a pointer to a Device_base object
	// that is used to construct a Device_processor object
	Model(Device_base *device_ptr_ = 0);
	~Model();

	Human_processor* get_human_ptr() const
		{return human_ptr;}
	Device_base * get_device_ptr() const
		{return device_ptr;}
	void set_device_ptr(Device_base * device_ptr_) 
		{connect_device_to_human(device_ptr_);}
	Device_processor * get_device_processor_ptr() 
		{return device_processor_ptr;}
	// the supplied Device_base is already connected to a Device_processor
	void connect_device_to_human(Device_base * dev_base_ptr);

	Visual_encoder_base * get_visual_encoder_ptr() const;
	void set_visual_encoder_ptr(Visual_encoder_base * visual_encoder_ptr); 

	Auditory_encoder_base * get_auditory_encoder_ptr() const;
	void set_auditory_encoder_ptr(Auditory_encoder_base * auditory_encoder_ptr); 

	/* functions that control the model from outside 
	they return true or false to show the run state of the model 
	the corresponding state variable is also set and can be accessed */
	bool compile();	// returns true if compile successful, false if not
	bool initialize();
	// these return true if the model is still runnable, false if not
	// run for a time
	bool run_time(long);
	// run until done ...
	// bool run();
	// stop the model, after letting it run to completion ...
	bool stop();

	long get_time() const;
    void display_memory_contents(); // output contents of current PS WM

	// accessors for state of the model
	const std::string& get_prs_filename() const
		{return prs_filename;}
	void set_prs_filename(const std::string&);
	bool get_prs_filename_set() const
		{return prs_filename_set;}
	bool get_compiled() const
		{return compiled;}
	bool get_initialized() const
		{return initialized;}
	bool get_running() const
		{return running;}

	/* View maintenance and notification functions */
	// add, remove Views to be notified of changes
	// forwarded to human processor to attach to correct subprocessors
	void add_visual_physical_view(View_base * view_ptr);
	void add_visual_sensory_view(View_base * view_ptr);
	void add_visual_perceptual_view(View_base * view_ptr);

	void remove_visual_physical_view(View_base * view_ptr);
	void remove_visual_sensory_view(View_base * view_ptr);
	void remove_visual_perceptual_view(View_base * view_ptr);

	void add_auditory_physical_view(View_base * view_ptr);
	void add_auditory_sensory_view(View_base * view_ptr);
	void add_auditory_perceptual_view(View_base * view_ptr);

	void remove_auditory_physical_view(View_base * view_ptr);
	void remove_auditory_sensory_view(View_base * view_ptr);
	void remove_auditory_perceptual_view(View_base * view_ptr);

	// use this function to add a view that is not specific to a particular processor
	// such as the view used for Normal output
	void add_view(View_base * view_ptr);
	void remove_view(View_base * view_ptr);
	void remove_all_views();
	void clear_all_views();

	// tracing accessors - forwarded to human processor
	bool get_trace_all() const
		{return trace_all;}
	void set_trace_all(bool);
	bool get_trace_visual() const;
	void set_trace_visual(bool);
	bool get_trace_auditory() const;
	void set_trace_auditory(bool);
	bool get_trace_cognitive() const;
	void set_trace_cognitive(bool);
	bool get_trace_ocular() const;
	void set_trace_ocular(bool);
	bool get_trace_manual() const;
	void set_trace_manual(bool);
	bool get_trace_vocal() const;
	void set_trace_vocal(bool);
	bool get_trace_temporal() const;
	void set_trace_temporal(bool);
	bool get_trace_device() const
		{return trace_device;}
	void set_trace_device(bool);
	
	// functions to get/set output flags in Production System
	bool get_output_compiler_messages() const;
	void set_output_compiler_messages(bool flag);

	bool get_output_compiler_details() const;
	void set_output_compiler_details(bool flag);

	bool get_output_run_messages() const;
	void set_output_run_messages(bool flag);

	bool get_output_run_details() const;
	void set_output_run_details(bool flag);

	bool get_output_run_memory_contents() const;
	void set_output_run_memory_contents(bool flag);
	
	// break and fault accessors
	const Symbol_list_t& get_rule_names() const;
	bool is_rule_name_valid(const Symbol& rule_name);
	bool get_break_enabled() const;
	void set_break_enabled(bool);
	void set_rule_break_state(const Symbol& rule_name, bool state);
	bool get_rule_break_state(const Symbol& rule_name) const;
	const Symbol_list_t& get_break_rule_names() const;
	bool get_fault_enabled() const;
	void set_fault_enabled(bool);
	void set_rule_fault_state(const Symbol& rule_name, bool state);
	bool get_rule_fault_state(const Symbol& rule_name) const;
	const Symbol_list_t& get_fault_rule_names() const;
	void set_parameters(const Parameter_specification_list_t& parameter_specs);
	void set_parameter(const std::string& proc_name, const std::string& param_name, const std::string& spec);


	// control the EPICLib random number generator
    void set_random_number_generator_seed(unsigned long seed);
    unsigned long get_random_number_generator_seed() const; 
	
	// device accessors
	void set_device_parameter_string(const std::string&);
	std::string get_device_parameter_string() const;
	
private:
	std::string prs_filename;	// holds the current production rule file name	
	Device_base * device_ptr;
	Device_processor * device_processor_ptr;
	Human_processor * human_ptr;

	std::set<View_base *> view_ptrs;	// pointers to all attached views - set for uniqueness

	
	bool prs_filename_set;	// production rule set filename has been set
	bool compiled;			// production rules are compiled
	bool prs_filename_unchanged;	// filename same as before
	bool preserve_break_names;	// if filename unchanged, then after compile, reset breaks for rules of same name
	bool initialized;		// simulation has been initialized
	bool running;			// simulation is running

	bool trace_all;
	bool trace_device;
	
	std::list<Symbol> break_rule_names;
	
	void connect_device();
	
    unsigned long random_number_generator_seed;
	
	// forbid copy and assignment
	Model(const Model&);
	Model& operator= (const Model&);

};

#endif
