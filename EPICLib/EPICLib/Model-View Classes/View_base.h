#ifndef VIEW_BASE_H
#define VIEW_BASE_H

#include <list>
#include <string>
#include "Geometry.h"
namespace GU = Geometry_Utilities;

/* Following the Model-View-Controller pattern, View_base.h is an abstract 
base class for views used in the EPIC software. 
Each derived class will respond to a particular kind of event notification 
provided by the Model. 
*/


class Human_processor;
class Human_subprocessor;
class Symbol;
struct Speech_word;

class View_base {
public:
	View_base() :
		human_ptr(0)
		{}
	virtual ~View_base();

	void set_attached_human(Human_processor * human_ptr_)
		{human_ptr = human_ptr_;}
		
	virtual void clear() = 0; // must override

	virtual void notify_eye_movement(GU::Point)
		{}
	virtual void notify_object_appear(const Symbol&, GU::Point, GU::Size)
		{}
//	virtual void notify_object_appear(const Symbol&, GU::Point)
//		{}
	virtual void notify_object_disappear(const Symbol&)
		{}
	virtual void notify_object_reappear(const Symbol&)
		{}
	virtual void notify_erase_object(const Symbol&)
		{}
	virtual void notify_visual_location_changed(const Symbol&, GU::Point)
		{}
	virtual void notify_visual_size_changed(const Symbol&, GU::Size)
		{}
	virtual void notify_visual_property_changed(const Symbol&, const Symbol&, const Symbol&)
		{}
	virtual void notify_auditory_stream_appear(const Symbol&, double, double, GU::Point)
		{}
	virtual void notify_auditory_stream_disappear(const Symbol&)
		{}
	virtual void notify_auditory_stream_location_changed(const Symbol&, GU::Point)
		{}
	virtual void notify_auditory_stream_pitch_changed(const Symbol&, double)
		{}
	virtual void notify_auditory_stream_loudness_changed(const Symbol&, double)
		{}
	virtual void notify_auditory_stream_size_changed(const Symbol&, GU::Size)
		{}
	virtual void notify_auditory_stream_property_changed(const Symbol&, const Symbol&, const Symbol&)
		{}
	virtual void notify_auditory_sound_start(const Symbol&, const Symbol&, long, GU::Point, const Symbol&, double)
		{}
//	virtual void notify_auditory_speech_start(const Symbol&, const Symbol&, long, const Symbol&, const Symbol&, const Symbol&, double)
//		{}
	virtual void notify_auditory_speech_start(const Speech_word&)
		{}
	virtual void notify_auditory_sound_stop(const Symbol&)
		{}
	virtual void notify_erase_sound(const Symbol&)
		{}
	virtual void notify_auditory_sound_property_changed(const Symbol&, const Symbol&, const Symbol&)
		{}
	virtual void notify_append_text(const std::string&)
		{}
	virtual void notify_time(long)
		{}
	
	// attach or detach this View to or from the specified processor
	void attach_to(Human_subprocessor * processor_ptr);
	void detach_from(Human_subprocessor * processor_ptr);
	// detach this view from all current processors
	void detach_from_all();

protected:
	Human_processor * human_ptr;	// a reference to the connected model - assumed to exist when constructed
	// list of subprocessors that this view has been attached to - used by dtor
	std::list<Human_subprocessor *> processor_ptrs;
};

#endif

