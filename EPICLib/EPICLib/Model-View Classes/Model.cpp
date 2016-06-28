// refactoring 4/30/03 - Model should know only about Human_processor and Device_processor - shouldn't need anything else!

//#include <iostream>
//#include <iomanip>
#include <string>
#include <list>
#include <vector>
#include <algorithm>

#include "Model.h"

#include <iostream>
#include <fstream>
//#include <profiler.h>

#include "Coordinator.h"
#include "Human_processor.h"
#include "Device_processor.h"
#include "Device_base.h"
#include "Output_tee.h"
#include "Output_tee_globals.h"
#include "PPS_globals.h"
#include "Assert_throw.h"
#include "Exception.h" 
#include "Parameter.h"
#include "Random_utilities.h"

//using namespace std;
using std::cout;	using std::endl;
using std::set;
using std::string;
using std::clock_t;	using std::clock;

// create the model
Model::Model(Device_base *device_ptr_) :
	device_ptr(device_ptr_), device_processor_ptr(0), human_ptr(0),  
	prs_filename_set(false), compiled(false), prs_filename_unchanged(false), preserve_break_names(true), 
	initialized(false), running(false), random_number_generator_seed(0)

{
	// we have a try-catch here in case top level is not prepared to catch exceptions
	try {
	// create the Human_processsor and Device_base class device
	human_ptr = new Human_processor("Human1");
	
	set_trace_all(false);	// turn off all tracing
		
	}
	catch(Exception& x) {
		Normal_out << x.what() << endl;
		throw;
		}
}


// Assumes:
// Any previous Device_base and Device_processor object has already been deleted.
// The Device_base has already been connected to a Device_processor and added to the simulation.  
void Model::connect_device_to_human(Device_base * dev_base_ptr)
{
	Assert(dev_base_ptr);
	device_ptr = dev_base_ptr;
	// following is obsolete - if device is changed, it has already been
	// deleted and the pointer to it must be discarded.
	// Assert(!device_processor_ptr);
	// get the automatically created device processor
	device_processor_ptr = device_ptr->get_device_proc_ptr();
	Assert(device_processor_ptr);	// should already be connected

	Assert(human_ptr);
	device_processor_ptr->connect(human_ptr);
	human_ptr->connect(device_processor_ptr);
}

Visual_encoder_base * Model::get_visual_encoder_ptr() const
{
	Assert(human_ptr);
	return human_ptr->get_visual_encoder_ptr();
}

void Model::set_visual_encoder_ptr(Visual_encoder_base * visual_encoder_ptr)
{
	Assert(human_ptr);
	return human_ptr->set_visual_encoder_ptr(visual_encoder_ptr);
}

Auditory_encoder_base * Model::get_auditory_encoder_ptr() const
{
	Assert(human_ptr);
	return human_ptr->get_auditory_encoder_ptr();
}

void Model::set_auditory_encoder_ptr(Auditory_encoder_base * auditory_encoder_ptr)
{
	Assert(human_ptr);
	return human_ptr->set_aditory_encoder_ptr(auditory_encoder_ptr);
}



// destroy the model
// Views are assumed to have been removed beforehand, because
// their existence relative to the model is unclear
Model::~Model()
{
	delete human_ptr;
	delete device_ptr;
}

// the filename is set to be the same the first time, and unset only if later changed
// to recompile the same file, it should suffice to simply call compile again.
void Model::set_prs_filename(const std::string& fn)
{
	prs_filename_unchanged = !prs_filename_set || (prs_filename_set && (prs_filename == fn));
	prs_filename = fn;
	prs_filename_set = true;
}


// Compile is supposed to reset the model so that it is ready to run
// but views will only be cleared when it is initialized
// return true if compilation was successful, false if not
bool Model::compile()
{
	Assert(prs_filename_set);
	compiled = false;
	initialized = false;
	running = false;
	// save the break rule names
	if(preserve_break_names && prs_filename_unchanged) {
		const Symbol_list_t& rule_names = get_rule_names();
		for(Symbol_list_t::const_iterator it = rule_names.begin(); it != rule_names.end(); ++it) {
			Symbol rule_name = *it;
			if(get_rule_break_state(rule_name))
				break_rule_names.push_back(rule_name);
			}
		}
	// compile the production rules
	compiled = human_ptr->compile(prs_filename);
//	if(compiled)
//		Normal_out << prs_filename << " compiled." << endl;
	// reset the break rules
	if(preserve_break_names && prs_filename_unchanged) {
		for(Symbol_list_t::const_iterator it = break_rule_names.begin(); it != break_rule_names.end(); ++it)
			if(is_rule_name_valid(*it))
				set_rule_break_state(*it, true);
		break_rule_names.clear();
		}
	return compiled;
}

bool Model::initialize()
{
	Assert(device_ptr);
	clear_all_views();
	Coordinator::get_instance().initialize();
	initialized = true;
	running = true;
	// seed the random number generator, but only if the seed has been set to a non-zero value
	// otherwise the random number generator will be using the default seed to start and continue
	// the random sequence from that default seed thereafter.
	if(random_number_generator_seed != 0)
    	set_random_number_generator_seed(random_number_generator_seed);
	Normal_out << "*** Simulation is starting ***" << endl;
	return true;
}

long Model::get_time() const
{
//	return Coordinator::get_instance().get_time();
	return Coordinator::get_time();
}

void Model::display_memory_contents()
{
    human_ptr->display_memory_contents();
}

// return true if simulation is still in run state, false if not (it terminated)
bool Model::run_time(long duration)
{
		
//	cout << "adjust window, hit any key" << endl;
//	cin.get();
	Assert(compiled);
	
	if(!initialized) {
		initialize();
		human_ptr->describe_parameters(Normal_out);
		}
	
	if(running && Coordinator::get_instance().is_finished()) {
		initialized = false;
		running = false;
		Normal_out << "*** Simulation is finished ***" << endl;
		return false;
		}
//	Normal_out << "Symbol memory: " << Symbol_memory::get_instance().get_symbol_set_number() << ' '
//		<< Symbol_memory::get_instance().get_symbol_set_memory() << endl;
	Coordinator::get_instance().run_for(duration);

	if(Coordinator::get_instance().is_finished()) {
		// don't run any more past the finished state
		initialized = false;
		running = false;
		}

	else if(Coordinator::get_instance().is_paused()) {
		// leave initialized and running, but return false
		return false;
		}
	// here if Coordinator simply timed out
//	Normal_out << "Symbol memory: " << Symbol_memory::get_instance().get_symbol_set_number() << ' '
//		<< Symbol_memory::get_instance().get_symbol_set_memory() << endl;

	return running;
//	outfile.close();
}
	
// stop the simulation; it is no longer initialized or running,
// but it stays in the compiled state
bool Model::stop()
{
	Coordinator::get_instance().stop(); // unconditionally terminate
	initialized = false;
	running = false;
	return running;
}

/* View maintenance and notification functions */
// forwarded to human processor to attach to correct subprocessors

void Model::add_visual_physical_view(View_base * view_ptr)
{
	human_ptr->add_visual_physical_view(view_ptr);
	view_ptrs.insert(view_ptr);
}
	
void Model::add_visual_sensory_view(View_base * view_ptr)
{
	human_ptr->add_visual_sensory_view(view_ptr);
	view_ptrs.insert(view_ptr);
}
	
void Model::add_visual_perceptual_view(View_base * view_ptr)
{
	human_ptr->add_visual_perceptual_view(view_ptr);
	view_ptrs.insert(view_ptr);
}
	
void Model::remove_visual_physical_view(View_base * view_ptr)
{
	human_ptr->remove_visual_physical_view(view_ptr);
	view_ptrs.erase(view_ptr);
}

void Model::remove_visual_sensory_view(View_base * view_ptr)
{
	human_ptr->remove_visual_sensory_view(view_ptr);
	view_ptrs.erase(view_ptr);
}
	
void Model::remove_visual_perceptual_view(View_base * view_ptr)
{
	human_ptr->remove_visual_perceptual_view(view_ptr);
	view_ptrs.erase(view_ptr);
}

void Model::add_auditory_physical_view(View_base * view_ptr)
{
	human_ptr->add_auditory_physical_view(view_ptr);
	view_ptrs.insert(view_ptr);
}
	
void Model::add_auditory_sensory_view(View_base * view_ptr)
{
	human_ptr->add_auditory_sensory_view(view_ptr);
	view_ptrs.insert(view_ptr);
}
	
void Model::add_auditory_perceptual_view(View_base * view_ptr)
{
	human_ptr->add_auditory_perceptual_view(view_ptr);
	view_ptrs.insert(view_ptr);
}
	
void Model::remove_auditory_physical_view(View_base * view_ptr)
{
	human_ptr->remove_auditory_physical_view(view_ptr);
	view_ptrs.erase(view_ptr);
}

void Model::remove_auditory_sensory_view(View_base * view_ptr)
{
	human_ptr->remove_auditory_sensory_view(view_ptr);
	view_ptrs.erase(view_ptr);
}
	
void Model::remove_auditory_perceptual_view(View_base * view_ptr)
{
	human_ptr->remove_auditory_perceptual_view(view_ptr);
	view_ptrs.erase(view_ptr);
}

// use this function to add a view that is not specific to a particular processor
// such as the view used for Normal output
void Model::add_view(View_base * view_ptr)
{
	view_ptrs.insert(view_ptr);
}

// tell this view to detach itself and then take it out of the list
void Model::remove_view(View_base * view_ptr)
{
	view_ptr->detach_from_all();
	view_ptrs.erase(view_ptr);
}

// tell all views to clear themselves
void Model::clear_all_views()
{
	for(set<View_base *>::iterator it = view_ptrs.begin(); it != view_ptrs.end(); it++)
		(*it)->clear();
}



// tell all views to detach themselves from all processors,then empty the list
void Model::remove_all_views()
{
	for(set<View_base *>::iterator it = view_ptrs.begin(); it != view_ptrs.end(); it++)
		(*it)->detach_from_all();

	view_ptrs.clear();
}

// tracing accessors  // why has this been disabled?  - dk 033108
void Model::set_trace_all(bool trace_)
{
	trace_all = trace_;
//	human_ptr->set_trace_all(trace_);
//	set_trace_device(trace_);
}


bool Model::get_trace_visual() const
{
	return human_ptr->get_trace_visual();
}

void Model::set_trace_visual (bool trace_)
{
	human_ptr->set_trace_visual(trace_);
}

void Model::set_trace_auditory (bool trace_)
{
	human_ptr->set_trace_auditory(trace_);
}

bool Model::get_trace_auditory() const
{
	return human_ptr->get_trace_auditory();
}

void Model::set_trace_cognitive (bool trace_)
{
	human_ptr->set_trace_cognitive(trace_);
}

bool Model::get_trace_cognitive() const
{
	return human_ptr->get_trace_cognitive();
}

void Model::set_trace_manual (bool trace_)
{
	human_ptr->set_trace_manual(trace_);
}

bool Model::get_trace_manual() const
{
	return human_ptr->get_trace_manual();
}

void Model::set_trace_ocular (bool trace_)
{
	human_ptr->set_trace_ocular(trace_);
}

bool Model::get_trace_ocular() const
{
	return human_ptr->get_trace_ocular();
}

void Model::set_trace_vocal (bool trace_)
{
	human_ptr->set_trace_vocal(trace_);
}

bool Model::get_trace_vocal() const
{
	return human_ptr->get_trace_vocal();
}

void Model::set_trace_temporal (bool trace_)
{
	human_ptr->set_trace_temporal(trace_);
}

bool Model::get_trace_temporal() const
{
	return human_ptr->get_trace_temporal();
}

void Model::set_trace_device (bool trace_)
{
	trace_device = trace_;
	device_ptr->set_trace(trace_);
}


bool Model::get_output_compiler_messages() const
{return human_ptr->get_output_compiler_messages();}

void Model::set_output_compiler_messages(bool flag)
{human_ptr->set_output_compiler_messages(flag);}

bool Model::get_output_compiler_details() const
{return human_ptr->get_output_compiler_details();}

void Model::set_output_compiler_details(bool flag)
{human_ptr->set_output_compiler_details(flag);}

bool Model:: get_output_run_messages() const
{return human_ptr->get_output_run_messages();}

void Model::set_output_run_messages(bool flag)
{human_ptr->set_output_run_messages(flag);}

bool Model:: get_output_run_details() const
{return human_ptr->get_output_run_details();}

void Model::set_output_run_details(bool flag)
{human_ptr->set_output_run_details(flag);}

bool Model:: get_output_run_memory_contents() const
{return human_ptr->get_output_run_memory_contents();}

void Model::set_output_run_memory_contents(bool flag)
{human_ptr->set_output_run_memory_contents(flag);}

const Symbol_list_t& Model::get_rule_names() const
{
	return human_ptr->get_rule_names();
}

bool Model::is_rule_name_valid(const Symbol& rule_name)
{
	return human_ptr->is_rule_name_valid(rule_name);
}

bool Model::get_break_enabled() const
{
	return human_ptr->get_break_enabled();
}

void Model::set_break_enabled(bool state)
{
	return human_ptr->set_break_enabled(state);
}

void Model::set_rule_break_state(const Symbol& rule_name, bool state)
{
	human_ptr->set_rule_break_state(rule_name, state);
}

bool Model::get_rule_break_state(const Symbol& rule_name) const
{
	return human_ptr->get_rule_break_state(rule_name);
}

const Symbol_list_t& Model::get_break_rule_names() const
{
	return human_ptr->get_break_rule_names();
}


bool Model::get_fault_enabled() const
{
	return human_ptr->get_fault_enabled();
}

void Model::set_fault_enabled(bool state)
{
	return human_ptr->set_fault_enabled(state);
}

void Model::set_rule_fault_state(const Symbol& rule_name, bool state)
{
	human_ptr->set_rule_fault_state(rule_name, state);
}

bool Model::get_rule_fault_state(const Symbol& rule_name) const
{
	return human_ptr->get_rule_fault_state(rule_name);
}

const Symbol_list_t& Model::get_fault_rule_names() const
{
	return human_ptr->get_fault_rule_names();
}

void Model::set_parameters(const Parameter_specification_list_t& parameter_specs)
{
	human_ptr->set_parameters(parameter_specs);
}

void Model::set_parameter(const std::string& proc_name, const std::string& param_name, const std::string& spec)
{
	human_ptr->set_parameter(proc_name, param_name, spec);
}

void Model::set_device_parameter_string(const string& str_)
{
	device_ptr->set_parameter_string(str_);
}

string Model::get_device_parameter_string() const
{
	return device_ptr->get_parameter_string();
}

void Model::set_random_number_generator_seed(unsigned long seed)
{
    random_number_generator_seed = seed;
}

unsigned long Model::get_random_number_generator_seed() const
{
    return random_number_generator_seed;
}

