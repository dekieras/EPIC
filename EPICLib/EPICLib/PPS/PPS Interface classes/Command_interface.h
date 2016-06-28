#ifndef COMMAND_INTERFACE_H
#define COMMAND_INTERFACE_H

#include "Symbol.h"
#include "Symbol_utilities.h"

// this class acts to couple between the PPS Rule-node executor and the rest of the EPIC
// system by providing a layer of indirection between the two.

class Human_processor;

class Command_interface {
public:
	Command_interface(Human_processor * human_ptr_) :
		human_ptr(human_ptr_)
		{}
		
	void dispatch(const Symbol_list_t& parameters);
    // return true if the pattern should be added, false if not
	bool dispatch(const Symbol_list_t& parameters, const Symbol_list_t& pattern);

private:
const Symbol& get_next_argument(const Symbol_list_t& arguments, Symbol_list_t::const_iterator& it);

Human_processor * human_ptr;

};

#endif
