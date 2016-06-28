// this is a class of Human_subprocessor that simply have direct calls for store updates to call
// added.

#ifndef VISUAL_STORE_PROCESSOR_H
#define VISUAL_STORE_PROCESSOR_H

#include "Human_subprocessor.h"
#include "Geometry.h"
namespace GU = Geometry_Utilities;

class Symbol;

class Visual_store_processor : public Human_subprocessor {
public:
	Visual_store_processor(const std::string& name_, Human_processor * human_ptr_) :
		Human_subprocessor (name_, human_ptr_)
		{}
	virtual ~Visual_store_processor()
		{}
	// direct inputs - object name is supplied along with all relevant information - no backwards lookup necessary
	// one-name form refers to whatever name is available or relevant
	virtual void make_object_appear(const Symbol&, GU::Point, GU::Size) {}
//	virtual void make_object_appear(const Symbol&, const Symbol&, GU::Point, GU::Size) {}
	virtual void make_object_disappear(const Symbol&) {}	// makes this an ABC
	virtual void make_object_disappear(const Symbol&, const Symbol&) {}	// makes this an ABC
	virtual void set_object_location(const Symbol&, GU::Point) {}
	virtual void set_object_size(const Symbol&, GU::Size) {}
	virtual void set_object_property(const Symbol&, const Symbol&, const Symbol&) {}

private:
	// no default copy, assignment
	Visual_store_processor(const Visual_store_processor&);
	Visual_store_processor& operator= (const Visual_store_processor&);


};


#endif
