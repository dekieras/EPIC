//
//  AppGlobals.h
//  EPIC
//
//  Created by David Kieras on 8/5/13.
//  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
//

#ifndef APPGLOBALS_H
#define APPGLOBALS_H
#include <iosfwd>

class Model;
class Dylib_object_manager;

extern Model* model_ptr; // pointer to the model object - initialized during construction	
extern Dylib_object_manager* dylib_object_manager_ptr;
extern std::ofstream logfile_stream;  // global object for logging file

#endif

