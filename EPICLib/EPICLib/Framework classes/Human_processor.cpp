//#define NDEBUG	// turn off asserts

#include "Human_processor.h"
#include "Assert_throw.h"
#include "Event_types.h"
#include "Visual_physical_store.h"
#include "Eye_processor.h"
#include "Visual_sensory_store.h"
#include "Visual_perceptual_processor.h"
#include "Visual_perceptual_store.h"
#include "Visual_encoder_base.h"
#include "Speech_word.h"
#include "Auditory_physical_store.h"
#include "Ear_processor.h"
#include "Auditory_sensory_store.h"
#include "Auditory_perceptual_processor.h"
#include "Auditory_perceptual_store.h"
#include "Auditory_encoder_base.h"
#include "Cognitive_processor.h"
#include "Manual_processor.h"
#include "Ocular_processor.h"
#include "Invol_ocular_processor.h"
#include "Vocal_processor.h"
#include "Temporal_processor.h"
//#include "GOMS_Cognitive_processor.h"
//#include "WM_processor.h"
//#include "LTM_processor.h"
//#include "Task_processor.h"
#include "Device_processor.h"
#include "View_base.h"
#include "Output_tee_globals.h"


#include <iostream>
//#include <typeinfo>
#include <string>
#include <list>
#include <algorithm>

using std::cout;	using std::endl;
using std::string;
using std::list;
using std::for_each;	using std::mem_fun;	using std::bind2nd;

// A human processor has a specific id, a coordinator,no container, 
// and serves as its own broadcaster. It assembles some subprocessors into the
// appropriate configuration, serving as the container and broadcaster for them.
// All pointers to other components should be initialized here to zero.
Human_processor::Human_processor(const std::string& name_) : 
	Processor(name_ + std::string(":Human")),
	visual_physical_store_ptr(0), eye_ptr(0), visual_sensory_store_ptr(0), visual_perceptual_processor_ptr(0), visual_perceptual_store_ptr(0), 
	auditory_physical_store_ptr(0), ear_ptr(0), auditory_sensory_store_ptr(0), auditory_perceptual_processor_ptr(0), auditory_perceptual_store_ptr(0), 
	cognitive_ptr(0), manual_ptr(0), ocular_ptr(0), invol_ocular_ptr(0),
	vocal_ptr(0), temporal_ptr(0), ltm_ptr(0), task_ptr(0),
	trace_all(false), trace_visual(false), trace_auditory(false), trace_cognitive(false), trace_manual(false), trace_ocular(false), trace_vocal(false), trace_temporal(false)
{
	// processors connect themselves by calling for the desired pointer 
	// through Human_subprocessor::get_human_ptr()
	// processors add themselves to Coordinator's list upon creation
	// add each processor to the processors container

	visual_physical_store_ptr = new Visual_physical_store(this);
	processors[visual_physical_store_ptr->get_name()] = visual_physical_store_ptr;

	eye_ptr = new Eye_processor(this);
	processors[eye_ptr->get_name()] = eye_ptr;

	visual_sensory_store_ptr = new Visual_sensory_store(this);
	processors[visual_sensory_store_ptr->get_name()] = visual_sensory_store_ptr;

	visual_perceptual_processor_ptr = new Visual_perceptual_processor(this);
	processors[visual_perceptual_processor_ptr->get_name()] = visual_perceptual_processor_ptr;

	visual_perceptual_store_ptr = new Visual_perceptual_store(this);
	processors[visual_perceptual_store_ptr->get_name()] = visual_perceptual_store_ptr;

	auditory_physical_store_ptr = new Auditory_physical_store(this);
	processors[auditory_physical_store_ptr->get_name()] = auditory_physical_store_ptr;

	ear_ptr = new Ear_processor(this);
	processors[ear_ptr->get_name()] = ear_ptr;

	auditory_sensory_store_ptr = new Auditory_sensory_store(this);
	processors[auditory_sensory_store_ptr->get_name()] = auditory_sensory_store_ptr;

	auditory_perceptual_processor_ptr = new Auditory_perceptual_processor(this);
	processors[auditory_perceptual_processor_ptr->get_name()] = auditory_perceptual_processor_ptr;

	auditory_perceptual_store_ptr = new Auditory_perceptual_store(this);
	processors[auditory_perceptual_store_ptr->get_name()] = auditory_perceptual_store_ptr;

	cognitive_ptr = new Cognitive_processor(this);
	processors[cognitive_ptr->get_name()] = cognitive_ptr;

	manual_ptr = new Manual_processor(this);
	processors[manual_ptr->get_name()] = manual_ptr;

	ocular_ptr = new Ocular_processor(this);
	processors[ocular_ptr->get_name()] = ocular_ptr;

	invol_ocular_ptr = new Invol_ocular_processor(this);
	processors[invol_ocular_ptr->get_name()] = invol_ocular_ptr;

	vocal_ptr = new Vocal_processor(this);
	processors[vocal_ptr->get_name()] = vocal_ptr;

	temporal_ptr = new Temporal_processor(this);	// ajh & teh
	processors[temporal_ptr->get_name()] = temporal_ptr;

}

Human_processor::~Human_processor()
{
	processors.clear();	// to remove those dangling pointers.
	delete visual_physical_store_ptr;
	delete eye_ptr;
	delete visual_sensory_store_ptr;
	delete visual_perceptual_processor_ptr;
	delete visual_perceptual_store_ptr;
	delete auditory_physical_store_ptr;
	delete ear_ptr;
	delete auditory_sensory_store_ptr;
	delete auditory_perceptual_processor_ptr;
	delete auditory_perceptual_store_ptr;
	delete cognitive_ptr;
	delete manual_ptr;
	delete ocular_ptr;
	delete invol_ocular_ptr;
	delete vocal_ptr;
	delete temporal_ptr;
}

void Human_processor::initialize()
{
	// paranoid checks
	Assert(visual_physical_store_ptr);
	Assert(eye_ptr);
	Assert(visual_sensory_store_ptr);
	Assert(visual_perceptual_processor_ptr);
	Assert(visual_perceptual_store_ptr);
	Assert(auditory_physical_store_ptr);
	Assert(ear_ptr);
	Assert(auditory_sensory_store_ptr);
	Assert(auditory_perceptual_processor_ptr);
	Assert(auditory_perceptual_store_ptr);
	Assert(cognitive_ptr);
	Assert(manual_ptr);
	Assert(ocular_ptr);
	Assert(invol_ocular_ptr);
	Assert(vocal_ptr);
//	set_parameters();  // was here, now do after a compile ... 7/25/14
	initialize_recorders();
}

bool Human_processor::compile(const string& prs_filename_)
{
	prs_filename = prs_filename_;
	bool result = cognitive_ptr->compile(prs_filename);
	// load up the parameters now instead of at initialization time
	set_parameters();
	return result;
}

void Human_processor::describe_parameters(Output_tee& ot) const
{
	ot << "Parameters for " << get_name() << ":" << endl;
	ot << "Production rule file: " << prs_filename << endl;
	visual_physical_store_ptr->describe_parameters(ot);
	eye_ptr->describe_parameters(ot);
	visual_sensory_store_ptr->describe_parameters(ot);
	visual_perceptual_processor_ptr->describe_parameters(ot);
	visual_perceptual_store_ptr->describe_parameters(ot);
	auditory_physical_store_ptr->describe_parameters(ot);
	ear_ptr->describe_parameters(ot);
	auditory_sensory_store_ptr->describe_parameters(ot);
	auditory_perceptual_processor_ptr->describe_parameters(ot);
	auditory_perceptual_store_ptr->describe_parameters(ot);
	cognitive_ptr->describe_parameters(ot);
	temporal_ptr->describe_parameters(ot);
	manual_ptr->describe_parameters(ot);
	ocular_ptr->describe_parameters(ot);
	invol_ocular_ptr->describe_parameters(ot);
	vocal_ptr->describe_parameters(ot);
}

void Human_processor::set_parameters()
{
	const Parameter_specification_list_t& parameter_specs = cognitive_ptr->get_parameter_specifications();

	for(Parameter_specification_list_t::const_iterator spec_it = parameter_specs.begin();
			spec_it != parameter_specs.end(); ++spec_it) {
		Processors_t::iterator proc_it = processors.find(spec_it->processor_name);
		if(proc_it == processors.end())
			Parameter::throw_parameter_error("Unrecognized processor name in parameter specification", *spec_it);
		proc_it->second->set_parameter(*spec_it);
		}		
}

void Human_processor::set_parameter(const std::string& proc_name, const std::string& param_name, const std::string& spec)
{
	Parameter_specification param_spec = {proc_name, param_name, spec};

	Processors_t::iterator proc_it = processors.find(param_spec.processor_name);
		if(proc_it == processors.end())
			Parameter::throw_parameter_error("Unrecognized processor name in parameter specification", param_spec);
		proc_it->second->set_parameter(param_spec);
}

void Human_processor::set_parameters(const Parameter_specification_list_t& parameter_specs)
{	
	for(Parameter_specification_list_t::const_iterator spec_it = parameter_specs.begin();
		spec_it != parameter_specs.end(); ++spec_it) {
		Processors_t::iterator proc_it = processors.find(spec_it->processor_name);
		if(proc_it == processors.end())
			Parameter::throw_parameter_error("Unrecognized processor name in parameter specification", *spec_it);
		proc_it->second->set_parameter(*spec_it);
	}		
}


Processor * Human_processor::get_visual_physical_store_ptr() const {return visual_physical_store_ptr;}

Processor * Human_processor::get_eye_ptr() const {return eye_ptr;}

Processor * Human_processor::get_visual_sensory_store_ptr() const {return visual_sensory_store_ptr;}

Processor * Human_processor::get_visual_perceptual_processor_ptr() const {return visual_perceptual_processor_ptr;}

Processor * Human_processor::get_visual_perceptual_store_ptr() const {return visual_perceptual_store_ptr;}

Processor * Human_processor::get_auditory_physical_store_ptr() const {return auditory_physical_store_ptr;}

Processor * Human_processor::get_ear_ptr() const {return ear_ptr;}

Processor * Human_processor::get_auditory_sensory_store_ptr() const {return auditory_sensory_store_ptr;}

Processor * Human_processor::get_auditory_perceptual_processor_ptr() const {return auditory_perceptual_processor_ptr;}

Processor * Human_processor::get_auditory_perceptual_store_ptr() const {return auditory_perceptual_store_ptr;}

Processor * Human_processor::get_cognitive_ptr() const {return cognitive_ptr;}

Processor * Human_processor::get_manual_ptr() const {return manual_ptr;}

Processor * Human_processor::get_ocular_ptr() const {return ocular_ptr;}

Processor * Human_processor::get_invol_ocular_ptr() const {return invol_ocular_ptr;}

Processor * Human_processor::get_vocal_ptr() const {return vocal_ptr;}

Processor * Human_processor::get_temporal_ptr() const {return temporal_ptr;}

//Processor * Human_processor::get_task_ptr() const {return task_ptr;}

Processor * Human_processor::get_device_ptr() const {return device_ptr;}

Visual_encoder_base * Human_processor::get_visual_encoder_ptr() const
{
	return visual_perceptual_processor_ptr->get_visual_encoder_ptr();
}

void Human_processor::set_visual_encoder_ptr(Visual_encoder_base * visual_encoder_ptr)
{
	visual_perceptual_processor_ptr->set_visual_encoder_ptr(visual_encoder_ptr);
	// connect the encoder back
	visual_encoder_ptr->connect(visual_perceptual_processor_ptr);
}

Auditory_encoder_base * Human_processor::get_auditory_encoder_ptr() const
{
	return auditory_perceptual_processor_ptr->get_auditory_encoder_ptr();
}

void Human_processor::set_aditory_encoder_ptr(Auditory_encoder_base * auditory_encoder_ptr)
{
	auditory_perceptual_processor_ptr->set_auditory_encoder_ptr(auditory_encoder_ptr);
	// connect the encoder back
	auditory_encoder_ptr->connect(auditory_perceptual_processor_ptr);
}


// inputs are forwarded to the relevant processor
void Human_processor::make_visual_object_appear(const Symbol& obj_name, GU::Point location, GU::Size size)
{
	visual_physical_store_ptr->make_object_appear(obj_name, location, size);
}

void Human_processor::make_visual_object_disappear(const Symbol& obj_name)
{
	visual_physical_store_ptr->make_object_disappear(obj_name);
}

void Human_processor::set_visual_object_location(const Symbol& obj_name, GU::Point location)
{
	visual_physical_store_ptr->set_object_location(obj_name, location);
}

void Human_processor::set_visual_object_size(const Symbol& obj_name, GU::Size size)
{
	visual_physical_store_ptr->set_object_size(obj_name, size);
}

void Human_processor::set_visual_object_property(const Symbol& obj_name, const Symbol& propname, const Symbol& propvalue)
{
	visual_physical_store_ptr->set_object_property(obj_name, propname, propvalue);
}


/* void Human_processor::create_auditory_stream(const Symbol& name, GU::Point location, GU::Size size)
{
	auditory_physical_store_ptr->create_stream(name, location, size);
}
*/
/*
void Human_processor::destroy_auditory_stream(const Symbol& name)
{
	auditory_physical_store_ptr->destroy_stream(name);
}
*/
void Human_processor::set_auditory_stream_location(const Symbol& name, GU::Point location)
{
	auditory_physical_store_ptr->set_stream_location(name, location);
}
void Human_processor::set_auditory_stream_size(const Symbol& name, GU::Size size)
{
	auditory_physical_store_ptr->set_stream_size(name, size);
}
void Human_processor::set_auditory_stream_property(const Symbol& name, const Symbol& propname, const Symbol& propvalue)
{
	auditory_physical_store_ptr->set_stream_property(name, propname, propvalue);
}

	
void Human_processor::make_auditory_sound_start(const Symbol& name, const Symbol& stream, GU::Point location,
		const Symbol& timbre, double loudness, long intrinsic_duration)
{
	auditory_physical_store_ptr->make_sound_start(name, stream, location, timbre, loudness, intrinsic_duration);
}
void Human_processor::make_auditory_sound_stop(const Symbol& name)
{
	auditory_physical_store_ptr->make_sound_stop(name);
}
void Human_processor::make_auditory_sound_event(const Symbol& name, const Symbol& stream_name, GU::Point location,
		const Symbol& timbre, double loudness, long duration, long intrinsic_duration)
{
	auditory_physical_store_ptr->make_sound_event(name, stream_name, location, timbre, loudness, duration, intrinsic_duration);
}
/*
void Human_processor::make_auditory_speech_event(const Symbol& sound_name, const Symbol& stream_name, 
		const Symbol& content, const Symbol& speaker_gender, const Symbol& speaker_id, double loudness, long duration)
{
	auditory_physical_store_ptr->make_speech_event(sound_name, stream_name, content, speaker_gender, speaker_id, loudness, duration);
}
*/

void Human_processor::make_auditory_speech_event(const Speech_word& word)
{
	auditory_physical_store_ptr->make_speech_event(word);
}

void Human_processor::set_auditory_sound_property(const Symbol& name, const Symbol& propname, const Symbol& propvalue)
{
	auditory_physical_store_ptr->set_sound_property(name, propname, propvalue);
}


/* View maintenance and notification functions */

void Human_processor::add_visual_physical_view(View_base * view_ptr)
{
	view_ptr->attach_to(visual_physical_store_ptr);
	view_ptr->attach_to(eye_ptr);
	view_ptr->set_attached_human(this);
}
	
void Human_processor::add_visual_sensory_view(View_base * view_ptr)
{
	view_ptr->attach_to(visual_sensory_store_ptr);
	view_ptr->attach_to(eye_ptr);
	view_ptr->set_attached_human(this);
}
	
void Human_processor::add_visual_perceptual_view(View_base * view_ptr)
{
	view_ptr->attach_to(visual_perceptual_store_ptr);
	view_ptr->attach_to(eye_ptr);
	view_ptr->set_attached_human(this);
}
	
void Human_processor::remove_visual_physical_view(View_base * view_ptr)
{
	view_ptr->detach_from(visual_physical_store_ptr);
	view_ptr->detach_from(eye_ptr);
}

void Human_processor::remove_visual_sensory_view(View_base * view_ptr)
{
	view_ptr->detach_from(visual_sensory_store_ptr);
	view_ptr->detach_from(eye_ptr);
}
	
void Human_processor::remove_visual_perceptual_view(View_base * view_ptr)
{
	view_ptr->detach_from(visual_perceptual_store_ptr);
	view_ptr->detach_from(eye_ptr);
}

void Human_processor::add_auditory_physical_view(View_base * view_ptr)
{
	view_ptr->attach_to(auditory_physical_store_ptr);
	view_ptr->set_attached_human(this);
}
	
void Human_processor::add_auditory_sensory_view(View_base * view_ptr)
{
	view_ptr->attach_to(auditory_sensory_store_ptr);
	view_ptr->set_attached_human(this);
}
	
void Human_processor::add_auditory_perceptual_view(View_base * view_ptr)
{
	view_ptr->attach_to(auditory_perceptual_store_ptr);
	view_ptr->set_attached_human(this);
}
	
void Human_processor::remove_auditory_physical_view(View_base * view_ptr)
{
	view_ptr->detach_from(auditory_physical_store_ptr);
}

void Human_processor::remove_auditory_sensory_view(View_base * view_ptr)
{
	view_ptr->detach_from(auditory_sensory_store_ptr);
}
	
void Human_processor::remove_auditory_perceptual_view(View_base * view_ptr)
{
	view_ptr->detach_from(auditory_perceptual_store_ptr);
}

/*
void Human_processor::add_view(View_base * view_ptr)
{
	views.push_back(view_ptr);
}
	
void Human_processor::remove_view(View_base * view_ptr)
{
	views.remove(view_ptr);
}

void Human_processor::clear_views()
{
	// tell all the associated views to clear themselves
	for(list<View_base *>::iterator it = views.begin(); it != views.end(); it++)
		(*it)->clear();
}
*/

// Event interface to rest of the system

void Human_processor::accept_event(const Start_event *)
{
	if(Normal_out)
		Normal_out << "Human received Start_event" << endl;
}

void Human_processor::accept_event(const Stop_event *)
{
//	if(Normal_out)
//		Normal_out << "Human received Stop_event" << endl;
}

void Human_processor::add_to_recorder_list(Processor * proc_ptr)
{
	recorder_list.push_back(proc_ptr);
}

void Human_processor::remove_from_recorder_list(Processor * proc_ptr)
{
	recorder_list.remove(proc_ptr);
}

void Human_processor::initialize_recorders()
{
	list<Processor *>::iterator iter;
	for_each(recorder_list.begin(), recorder_list.end(), mem_fun(&Processor::initialize));
	
//	for (iter = recorder_list.begin(); iter != recorder_list.end(); ++iter)
//		(*iter)->initialize();
}

void Human_processor::broadcast_to_recorders(const Event *
		 event_ptr)
{
	list<Processor *>::iterator iter;
	
	for (iter = recorder_list.begin(); iter != recorder_list.end(); ++iter)
		event_ptr->send_self(*iter);
}


void Human_processor::display_memory_contents()
{
    // output contents of current PS WM
    cognitive_ptr->display_memory_contents();
}

// PPS output controls
bool Human_processor::get_output_compiler_messages() const
{
	return cognitive_ptr->get_output_compiler_messages();
}

void Human_processor::set_output_compiler_messages(bool flag)
{
	cognitive_ptr->set_output_compiler_messages(flag);
}

bool Human_processor::get_output_compiler_details() const
{
	return cognitive_ptr->get_output_compiler_details();
}

void Human_processor::set_output_compiler_details(bool flag)
{
	cognitive_ptr->set_output_compiler_details(flag);
}

bool Human_processor:: get_output_run_messages() const
{
	return cognitive_ptr->get_output_run_messages();
}

void Human_processor::set_output_run_messages(bool flag)
{
	cognitive_ptr->set_output_run_messages(flag);
}

bool Human_processor:: get_output_run_details() const
{
	return cognitive_ptr->get_output_run_details();
}

void Human_processor::set_output_run_details(bool flag)
{
	cognitive_ptr->set_output_run_details(flag);
}

bool Human_processor:: get_output_run_memory_contents() const
{
	return cognitive_ptr->get_output_run_memory_contents();
}

void Human_processor::set_output_run_memory_contents(bool flag)
{
	cognitive_ptr->set_output_run_memory_contents(flag);
}

// tracing accessors - these can modify the trace state of multiple subprocessors
void Human_processor::set_trace_all(bool trace_)
{
	trace_all = trace_;
	set_trace_visual(trace_);
	set_trace_auditory(trace_);
	set_trace_cognitive(trace_);
	set_trace_manual(trace_);
	set_trace_ocular(trace_);
	set_trace_manual(trace_);
	set_trace_vocal(trace_);
	set_trace_temporal(trace_);
}

void Human_processor::set_trace_visual (bool trace_)
{
	trace_visual = trace_;
	visual_physical_store_ptr->set_trace(trace_);
	eye_ptr->set_trace(trace_ || trace_ocular);
	visual_sensory_store_ptr->set_trace(trace_);
	visual_perceptual_processor_ptr->set_trace(trace_);
	visual_perceptual_store_ptr->set_trace(trace_);
	cognitive_ptr->set_trace(trace_);
}

void Human_processor::set_trace_auditory (bool trace_)
{
	trace_auditory = trace_;
	auditory_physical_store_ptr->set_trace(trace_);
	ear_ptr->set_trace(trace_);
	auditory_sensory_store_ptr->set_trace(trace_);
	auditory_perceptual_processor_ptr->set_trace(trace_);
	auditory_perceptual_store_ptr->set_trace(trace_);
	cognitive_ptr->set_trace(trace_);
}

void Human_processor::set_trace_cognitive (bool trace_)
{
	trace_cognitive = trace_;
	cognitive_ptr->set_trace(trace_);
}

void Human_processor::set_trace_manual (bool trace_)
{
	trace_manual = trace_;
	manual_ptr->set_trace(trace_);
}

void Human_processor::set_trace_ocular (bool trace_)
{
	trace_ocular = trace_;
	eye_ptr->set_trace(trace_ || trace_visual);
	ocular_ptr->set_trace(trace_);
	invol_ocular_ptr->set_trace(trace_);
}

void Human_processor::set_trace_vocal (bool trace_)
{
	trace_vocal = trace_;
	vocal_ptr->set_trace(trace_);
}

void Human_processor::set_trace_temporal (bool trace_)
{
	trace_temporal = trace_;
	temporal_ptr->set_trace(trace_);
}

// break and fault accessors
const Symbol_list_t& Human_processor::get_rule_names() const
{
	return cognitive_ptr->get_rule_names();
}

bool Human_processor::is_rule_name_valid(const Symbol& rule_name)
{
	return cognitive_ptr->is_rule_name_valid(rule_name);
}

bool Human_processor::get_break_enabled() const
{
	return cognitive_ptr->get_break_enabled();
}

void Human_processor::set_break_enabled(bool state)
{
	return cognitive_ptr->set_break_enabled(state);
}

void Human_processor::set_rule_break_state(const Symbol& rule_name, bool state)
{
	cognitive_ptr->set_rule_break_state(rule_name, state);
}

bool Human_processor::get_rule_break_state(const Symbol& rule_name) const
{
	return cognitive_ptr->get_rule_break_state(rule_name);
}

const Symbol_list_t& Human_processor::get_break_rule_names() const
{
	return cognitive_ptr->get_break_rule_names();
}

bool Human_processor::get_fault_enabled() const
{
	return cognitive_ptr->get_fault_enabled();
}

void Human_processor::set_fault_enabled(bool state)
{
	return cognitive_ptr->set_fault_enabled(state);
}

void Human_processor::set_rule_fault_state(const Symbol& rule_name, bool state)
{
	cognitive_ptr->set_rule_fault_state(rule_name, state);
}

bool Human_processor::get_rule_fault_state(const Symbol& rule_name) const
{
	return cognitive_ptr->get_rule_fault_state(rule_name);
}

const Symbol_list_t& Human_processor::get_fault_rule_names() const
{
	return cognitive_ptr->get_fault_rule_names();
}


