//
//  Dylib_manager.h
//  EPIC
//
//  Created by David Kieras on 8/5/13.
//  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
//
// This class loads dylibs and keeps track of the handles for them;
// it unloads the dylibs destroys the objects when changed and when this object is destroyed


#ifndef DYLIB_OBJECT_MANAGER_H
#define DYLIB_OBJECT_MANAGER_H

class Device_base;
class Visual_encoder_base;
class Auditory_encoder_base;

class Dylib_object_manager {
public:
    Dylib_object_manager();
    ~Dylib_object_manager();
    
    // the C++ types of the device and encoder class factories
    typedef Device_base* (*create_device_fn_t)();
    typedef void (*destroy_device_fn_t)(Device_base*);
    typedef Visual_encoder_base* (*create_visual_encoder_fn_t)();
    typedef void (*destroy_visual_encoder_fn_t)(Visual_encoder_base*);
    typedef Auditory_encoder_base* (*create_auditory_encoder_fn_t)();
    typedef void (*destroy_auditory_encoder_fn_t)(Auditory_encoder_base*);

	Device_base* load_and_create_device(const char* fullpathname);
	void destroy_and_unload_device();

	Visual_encoder_base* load_and_create_visual_encoder(const char* fullpathname);
    void destroy_and_unload_visual_encoder();

	Auditory_encoder_base* load_and_create_auditory_encoder(const char* fullpathname);
	void destroy_and_unload_auditory_encoder();
    
private:
    Device_base * device_ptr;	// pointer to the current device
    void * device_lib_handle;	// pointer to loaded library
    Visual_encoder_base * visual_encoder_ptr;	// pointer to the current encoder
    void * visual_encoder_lib_handle;	
    Auditory_encoder_base * auditory_encoder_ptr;	// pointer to the current encoder
    void * auditory_encoder_lib_handle;
        
    template <typename T, typename F>
    T* load_library_and_create_object(const char* fullpathname, const char* creation_function_name, void*& lib_handle);
    
    template <typename T, typename F>
    void destroy_object_and_unload_library(T*& object_ptr, const char* destruction_function_name, void*& lib_handle);

void unload_library(void*& lib_handle);


};

#endif
