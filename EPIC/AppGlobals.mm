//
//  AppGlobals.mm
//  EPIC
//
//  Created by David Kieras on 8/5/13.
//  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
//

#include "AppGlobals.h"
#include <fstream>

Model* model_ptr = nullptr;
Dylib_object_manager* dylib_object_manager_ptr = nullptr;
std::ofstream logfile_stream;
