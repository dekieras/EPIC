#ifndef PPS_RULE_COMPILER_INTERFACE_H
#define PPS_RULE_COMPILER_INTERFACE_H

#include "PPS_rule_compiler_classes.h"

#include <list>

namespace Parsimonious_Production_System {

// forward declaration 
class Production_System;

void compile_rules(Production_System * production_system_ptr, Compiler_rule_data_list_t& production_list);

} // end namespace

#endif
