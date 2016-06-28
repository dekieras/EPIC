//
//  Dylib_object_manager.cpp
//  EPIC
//
//  Created by David Kieras on 8/5/13.
//  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
//

#include "Dylib_object_manager.h"
#include "EPICLib/Output_tee_globals.h"
#include "EPICLib/Device_base.h"

#include <iostream>
#include <dlfcn.h> 

using namespace std;

Dylib_object_manager::Dylib_object_manager() :
    device_ptr(nullptr), device_lib_handle(nullptr),
	visual_encoder_ptr(nullptr), visual_encoder_lib_handle(nullptr),
	auditory_encoder_ptr(nullptr), auditory_encoder_lib_handle(nullptr)
{}

Dylib_object_manager::~Dylib_object_manager()
{
    destroy_and_unload_device();
    destroy_and_unload_visual_encoder();
    destroy_and_unload_auditory_encoder();
}

Device_base* Dylib_object_manager::load_and_create_device(const char* fullpathname)
{
    try {
    destroy_and_unload_device();

    // remember the object here
    device_ptr = load_library_and_create_object<Device_base, create_device_fn_t>(fullpathname, "create_device", device_lib_handle);
    return device_ptr;
        }
	catch(Exception& x) {
		Normal_out << x.what() << endl;
        return nullptr;
		}
// caught by previous catch
/*	catch (Assertion_exception& x) {
		Normal_out << x.what() << endl;
        return nullptr;
		}
*/
	catch (...) {
		Normal_out << "unknown exception caught" << endl;
        return nullptr;
		}
        
}

void Dylib_object_manager::destroy_and_unload_device()
{
    destroy_object_and_unload_library<Device_base, destroy_device_fn_t>(device_ptr, "destroy_device", device_lib_handle);
}

Visual_encoder_base* Dylib_object_manager::load_and_create_visual_encoder(const char* fullpathname)
{
    destroy_and_unload_visual_encoder();

    // remember the object here
    visual_encoder_ptr = load_library_and_create_object<Visual_encoder_base, create_visual_encoder_fn_t>(fullpathname, "create_visual_encoder", visual_encoder_lib_handle);
    return visual_encoder_ptr;
}

void Dylib_object_manager::destroy_and_unload_visual_encoder()
{
    destroy_object_and_unload_library<Visual_encoder_base, destroy_visual_encoder_fn_t>(visual_encoder_ptr, "destroy_visual_encoder", visual_encoder_lib_handle);
}

Auditory_encoder_base* Dylib_object_manager::load_and_create_auditory_encoder(const char* fullpathname)
{
    destroy_and_unload_auditory_encoder();

    // remember the object here
    auditory_encoder_ptr = load_library_and_create_object<Auditory_encoder_base, create_auditory_encoder_fn_t>(fullpathname, "create_auditory_encoder", auditory_encoder_lib_handle);
    return auditory_encoder_ptr;
}

void Dylib_object_manager::destroy_and_unload_auditory_encoder()
{
    destroy_object_and_unload_library<Auditory_encoder_base, destroy_auditory_encoder_fn_t>(auditory_encoder_ptr, "destroy_auditory_encoder", auditory_encoder_lib_handle);
}


template <typename T, typename F>
T* Dylib_object_manager::load_library_and_create_object(const char* fullpathname, const char* creation_function_name, void*& lib_handle)
{	
    T* object_ptr;
    F creation_fn_ptr;
    
	dlerror();
	// load the device library
	lib_handle = dlopen(fullpathname, RTLD_LAZY);
	if (!lib_handle) {
		Normal_out << "Failed to load library: " << fullpathname << '\n' << dlerror() << endl;
		return nullptr;
		}
 
	// reset errors
	dlerror();
	// load the symbol for the creation function
	// Note: dlsym returns a void *, which by Standard, can't be converted to a function pointer.
	creation_fn_ptr = reinterpret_cast<F> (dlsym(lib_handle, creation_function_name));
	const char * dlsym_error = dlerror();
	if (dlsym_error) {
		Normal_out << "Cannot load symbol \"" << creation_function_name << "\"\n" << dlsym_error << endl;
		// dump the library - something is wrong, need to start over
		unload_library(lib_handle);
		return nullptr;
		}
    
	// create an instance of the class
	object_ptr = creation_fn_ptr();
    return object_ptr;
}

template <typename T, typename F>
void Dylib_object_manager::destroy_object_and_unload_library(T*& object_ptr, const char* destruction_function_name, void*& lib_handle)
{
    F destruction_fn_ptr;
    dlerror(); // reset errors
    if(lib_handle) {
		destruction_fn_ptr = reinterpret_cast<F> (dlsym(lib_handle, destruction_function_name));
		const char * dlsym_error = dlerror();
		if (dlsym_error) {
			Normal_out << "Cannot load symbol \"" << destruction_function_name << "\"\n" << dlsym_error << endl;
			// something is wrong - unload the library and for better or worse, reset the object pointer
			unload_library(lib_handle);
			object_ptr = nullptr;
            return;
            }
        destruction_fn_ptr(object_ptr);
        object_ptr = nullptr;
		
		unload_library(lib_handle);
		}
}

void Dylib_object_manager::unload_library(void*& lib_handle)
{
        dlerror(); // reset errors
        
 		dlclose(lib_handle);
		const char * dlsym_error = dlsym_error = dlerror();
		if (dlsym_error) {
			Normal_out << "Failed to unload  library: " << dlsym_error << endl;
			lib_handle = nullptr; // for better or worse ...
			return;
			}
		lib_handle = nullptr; 
}


    
