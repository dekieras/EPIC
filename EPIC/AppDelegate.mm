//
//  AppDelegate.mm
//  EPIC
//
//  Created by David Kieras on 8/4/13.
//  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
//

#import "AppDelegate.h"
#import "AppGlobals.h"
#import "Dylib_object_manager.h"
#import "CETextWindow.h"
#import "EPICTextView.h"
#import "CEDrawViewWindow.h"
#import "CEDrawView.h"
#import "EPICDrawView.h"
#import "VisualView.h"
#import "AuditoryView.h"
#import "RunControlsDialog.h"
#import "OutputControlsDialog.h"
#import "StringWithState.h"
#import "BreakControlsDialog.h"
#import "TraceControlsDialog.h"
#import "LogControlsDialog.h"
#import "EPICLib/Model.h"
#include "EPICLib/Output_tee_globals.h"
#include "EPICLib/PPS_globals.h"
#include "EPICLib/Device_exception.h"
#include <string>
#include <list>

using namespace std;

class Device_base;

enum Run_duration_e {RUN_FOR_TIME, RUN_UNTIL_TIME, RUN_UNTIL_DONE};
enum Refresh_e {EACH_STEP, AFTER_EVERY_TIME, NONE_DURING_RUN};
enum Run_state_e {UNREADY, RUNNABLE, RUNNING, PAUSED};


@implementation AppDelegate
{
	CETextWindow* normal_outTextWindow;
    View_base* normal_out_ptr;
	CETextWindow* trace_outTextWindow;
    View_base* trace_out_ptr;

    CEDrawViewWindow* theVisualPhysicalDrawingWindow;
//    EPICDrawView* visual_physical_window_ptr;
    CEDrawViewWindow* theVisualSensoryDrawingWindow;
//    EPICDrawView* visual_sensory_window_ptr;
    CEDrawViewWindow* theVisualPerceptualDrawingWindow;
//    EPICDrawView* visual_perceptual_window_ptr;
    CEDrawViewWindow* theAuditoryPhysicalDrawingWindow;
//    EPICDrawView* auditory_physical_window_ptr;
    CEDrawViewWindow* theAuditorySensoryDrawingWindow;
//    EPICDrawView* auditory_sensory_window_ptr;
    CEDrawViewWindow* theAuditoryPerceptualDrawingWindow;
//    EPICDrawView* auditory_perceptual_window_ptr;
    list<EPICDrawView*> epic_draw_view_ptrs;
    
// application state
	Run_state_e run_state;
	Run_duration_e run_duration_setting;
	Refresh_e refresh_setting;
	bool run_continuously;
	long sim_time_per_step;	// e.g. 50 for 50 ms
	double real_time_per_step;	// e.g. .05 for "real time" rate
	long run_for_time;
	long run_until_time;
	long time_per_refresh;		// 200 for every fourth cycle
    long current_time;      // copy from Model
	
	bool run_result;
	long run_time;				// keeps time (in ms) from Run command
	long run_time_limit;
	long run_time_per_refresh;
	
	int pixels_per_degree;
	bool calibration_grid_state;

    NSTimer *timer;

}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	// Set initial values for application state
    run_state = UNREADY;
    run_duration_setting = RUN_FOR_TIME;
    run_continuously = false;
    refresh_setting = EACH_STEP;
	// integral time values are in ms
    sim_time_per_step = 50;
    real_time_per_step = 0.00001;
    run_for_time = 5000;
    run_until_time = 600000;
    time_per_refresh = 1000;
    run_result = false;
    run_time = 0;
    run_time_limit = 0;
    run_time_per_refresh = 0;
	
	pixels_per_degree = 10;
	calibration_grid_state = false;
    trace_out_ptr = nullptr;
	// Open the Normal_out view
    [self openNormal_outWindow];
    
    // create the Model instance and dylib manager - bail if something fails
    try {
    model_ptr = new Model;
    dylib_object_manager_ptr = new Dylib_object_manager;
        }
	catch(Exception& x) {
		Normal_out << x.what() << endl;
        return;
		}
    // Assertion_exception is derived from Exception
/*	catch (Assertion_exception& x) {
		Normal_out << x.what() << endl;
        return;
		}
*/
	catch (...) {
		Normal_out << "unknown exception caught" << endl;
        return;
		}
        
    [self createInitialWindows];
    
}

-(void) createInitialWindows
{
      // the nib draw window frame is 440 X 340 with a 400 X 300 view inside it
//    NSRect theFrame;
//    theFrame = [[NSScreen mainScreen] frame];
//    NSLog(@"main screen frame is %f, %f, %f, %f\n", theFrame.origin.x, theFrame.origin.y,theFrame.size.width, theFrame.size.height);

    NSRect theVisibleFrame;
    theVisibleFrame = [[NSScreen mainScreen] visibleFrame];
//    NSLog(@"main screen visible frame is %f, %f, %f, %f\n", theVisibleFrame.origin.x, theVisibleFrame.origin.y,theVisibleFrame.size.width, theVisibleFrame.size.height);
    NSPoint topleft = {theVisibleFrame.origin.x, theVisibleFrame.origin.y + theVisibleFrame.size.height};

    theVisualPhysicalDrawingWindow = [self openVisualDrawingWindowWithTitle:@"Visual Physical" position:topleft];
    EPICDrawView* visual_physical_window_ptr = [theVisualPhysicalDrawingWindow getEpicDrawView];
    model_ptr->add_visual_physical_view(visual_physical_window_ptr);
	visual_physical_window_ptr->set_changed();
    

    topleft.x = topleft.x + 440;
    theVisualSensoryDrawingWindow = [self openVisualDrawingWindowWithTitle:@"Visual Sensory" position:topleft];
    EPICDrawView* visual_sensory_window_ptr = [theVisualSensoryDrawingWindow getEpicDrawView];
    model_ptr->add_visual_sensory_view(visual_sensory_window_ptr);
	visual_sensory_window_ptr->set_changed();

    topleft.x = topleft.x + 440;
    theVisualPerceptualDrawingWindow = [self openVisualDrawingWindowWithTitle:@"Visual Perceptual" position:topleft];
    EPICDrawView* visual_perceptual_window_ptr = [theVisualPerceptualDrawingWindow getEpicDrawView];
    model_ptr->add_visual_perceptual_view(visual_perceptual_window_ptr);
	visual_perceptual_window_ptr->set_changed();

    topleft = {theVisibleFrame.origin.x, theVisibleFrame.origin.y + theVisibleFrame.size.height - 365};
    theAuditoryPhysicalDrawingWindow = [self openAuditoryDrawingWindowWithTitle:@"Auditory Physical" position:topleft];
    EPICDrawView* auditory_physical_window_ptr = [theAuditoryPhysicalDrawingWindow getEpicDrawView];
    model_ptr->add_auditory_physical_view(auditory_physical_window_ptr);
	auditory_physical_window_ptr->set_changed();

    topleft.x = topleft.x + 440;
    theAuditorySensoryDrawingWindow = [self openAuditoryDrawingWindowWithTitle:@"Auditory Sensory" position:topleft];
    EPICDrawView* auditory_sensory_window_ptr = [theAuditorySensoryDrawingWindow getEpicDrawView];
    model_ptr->add_auditory_sensory_view(auditory_sensory_window_ptr);
	auditory_sensory_window_ptr->set_changed();

    topleft.x = topleft.x + 440;
    theAuditoryPerceptualDrawingWindow = [self openAuditoryDrawingWindowWithTitle:@"Auditory Perceptual" position:topleft];
    EPICDrawView* auditory_perceptual_window_ptr = [theAuditoryPerceptualDrawingWindow getEpicDrawView];
    model_ptr->add_auditory_perceptual_view(auditory_perceptual_window_ptr);
	auditory_perceptual_window_ptr->set_changed();

}

- (void) removeEPICView:(EPICDrawView*)theView
{

    NSLog(@"in removeEPICView in AppDelegate");
	model_ptr->remove_view(theView);
    epic_draw_view_ptrs.remove(theView);
    delete theView;
}

-(void) dealloc
{
	// discard the view pointers from model
	model_ptr->remove_all_views();
	// since this module allocated them, we need to delete them here
	for(auto ptr : epic_draw_view_ptrs) {
		delete ptr;
		}
	// discard the normal out view
	Normal_out.remove_view(normal_out_ptr);
	delete normal_out_ptr;
	// model has pointers to device, but does not own them
    delete model_ptr;
	// this should unload and delete devices
    delete dylib_object_manager_ptr;


}

- (CEDrawViewWindow*) openVisualDrawingWindowWithTitle:(NSString*)windowtitle position:(NSPoint)windowposition
{
    CEDrawViewWindow* theDrawingWindow = [[CEDrawViewWindow alloc] initWithWindowTitle:windowtitle];
    [theDrawingWindow setTopLeftPoint:windowposition];
	[theDrawingWindow showWindow: nil];
    
    EPICDrawView* drawing_window_ptr = new VisualView([theDrawingWindow theDrawView]);
	[theDrawingWindow setEpicDrawView: drawing_window_ptr];
    epic_draw_view_ptrs.push_back(drawing_window_ptr);
    return theDrawingWindow;
}

- (CEDrawViewWindow*) openAuditoryDrawingWindowWithTitle:(NSString*)windowtitle position:(NSPoint)windowposition
{
    CEDrawViewWindow* theDrawingWindow = [[CEDrawViewWindow alloc] initWithWindowTitle:windowtitle];
    [theDrawingWindow setTopLeftPoint:windowposition];
	[theDrawingWindow showWindow: nil];
    
    EPICDrawView* drawing_window_ptr = new AuditoryView([theDrawingWindow theDrawView]);
	[theDrawingWindow setEpicDrawView: drawing_window_ptr];
    epic_draw_view_ptrs.push_back(drawing_window_ptr);
    return theDrawingWindow;
}

- (void) openNormal_outWindow
{
    // the Normal_out window is attached to an Output_tee global, but not to Model
    // but Output_tee assumes it has the EPIC Model View interface
    NSRect theVisibleFrame = [[NSScreen mainScreen] visibleFrame];
    NSPoint topleft = {theVisibleFrame.origin.x, theVisibleFrame.origin.y + theVisibleFrame.size.height - 730};
	normal_outTextWindow = [self openTextWindowWithTitle:@"Normal output" position:topleft];
    normal_out_ptr = new EPICTextView(normal_outTextWindow);
    // add to the Normal_out output_tee stream simulator
    Normal_out.add_view(normal_out_ptr);
    Normal_out << "EPIC Normal!" << endl;
    PPS_out.add_view(normal_out_ptr);
    PPS_out << "PPS Output!" << endl;
}

- (void) openTrace_outWindow
{
    // the Trace_out window is attached to an Output_tee global, but not to Model
    // but Output_tee assumes it has the EPIC Model View interface
    NSRect theVisibleFrame = [[NSScreen mainScreen] visibleFrame];
    NSPoint topleft = {theVisibleFrame.origin.x + 100, theVisibleFrame.origin.y + theVisibleFrame.size.height - 730};
	trace_outTextWindow = [self openTextWindowWithTitle:@"Trace output" position:topleft];
    trace_out_ptr = new EPICTextView(trace_outTextWindow);
    // add to the Trace_out output_tee stream simulator
    Trace_out.add_view(trace_out_ptr);
    Trace_out << "Trace!" << endl;
}

- (void) closeTrace_outWindow
{
    Trace_out.remove_view(trace_out_ptr);
    delete trace_out_ptr;
    [trace_outTextWindow close];
}

- (CETextWindow*) openTextWindowWithTitle:(NSString*)title position:(NSPoint)windowposition
{
	CETextWindow* theview = [[CETextWindow alloc] initWithWindowTitle:title position:windowposition];
	[theview showWindow: nil];
	return theview;
}


- (BOOL)validateMenuItem:(NSMenuItem *)menuItem
{
    if ([menuItem action] == @selector(pickedGoOneStep:) ||
        [menuItem action] == @selector(pickedRun:)) {
        if (run_state == RUNNABLE || run_state == PAUSED)
            return YES;
        else
            return NO;
    }
    
    if ([menuItem action] == @selector(pickedPause:)) {
        if (run_state == RUNNING)
            return YES;
        else
            return NO;
    }
    
    if ([menuItem action] == @selector(pickedHalt:)) {
        if (run_state == RUNNING || run_state == PAUSED)
            return YES;
        else
            return NO;
    }

    if ([menuItem action] == @selector(pickedRunControls:)) {
        if (run_state == RUNNABLE || run_state == PAUSED)
            return YES;
        else
            return NO;
    }
    
    if ([menuItem action] == @selector(pickedBreaksSettings:)) {
        if (model_ptr->get_compiled())
            return YES;
        else
            return NO;
    }
    
    if ([menuItem action] == @selector(pickedTracingSettings:)) {
        if (run_state == RUNNABLE || run_state == PAUSED)
            return YES;
        else
            return NO;
    }
    
    return YES;
}



- (IBAction)pickedLoadDevice:(id)sender
{
    // Create a File Open Dialog object.
    NSOpenPanel* openDlg = [NSOpenPanel openPanel];
    [openDlg setTitle:@"Choose device dyblib"];
    [openDlg setAllowedFileTypes:@[@"dylib"]]; // defaults to only open files
    NSInteger result = [openDlg runModal];
    if(result == NSFileHandlingPanelOKButton) {
        NSString* fileNamePath = [[[openDlg URLs] objectAtIndex:0] path];
        NSString* deviceDylibDirectoryPath = [fileNamePath stringByDeletingLastPathComponent];
        if ([[NSFileManager defaultManager] changeCurrentDirectoryPath:deviceDylibDirectoryPath]) {
                Normal_out << "cwd is now " << [[[NSFileManager defaultManager] currentDirectoryPath] UTF8String] << endl;
                }
            else
                Normal_out << "Could not change working directory to " << [deviceDylibDirectoryPath UTF8String] << endl;

        Device_base* device_ptr = dylib_object_manager_ptr->load_and_create_device([fileNamePath UTF8String]);
        if(device_ptr) {
			try {
				model_ptr->set_device_ptr(device_ptr);
//				Normal_out << "Device " << [fileNamePath UTF8String] << " loaded" << endl;
				Normal_out << "Device " << [[fileNamePath lastPathComponent] UTF8String] << " loaded" << endl;
                run_state = (model_ptr->get_device_ptr() && model_ptr->get_compiled()) ? RUNNABLE : UNREADY;
 
  //               Normal_out << "current directory is now " << [currentPath UTF8String] << endl;
				}
			catch(exception& x) {
				Normal_out << x.what() << endl;
				return;
				}
            }
        else
            Normal_out << "No device dyblib was loaded" << endl;
        }
    else
        Normal_out << "No device dyblib was loaded" << endl;

/*
    // this method basically runs in its own thread, so it
    // needs to be the last thing in this method ...
    [openDlg beginWithCompletionHandler:^(NSInteger result) {
        if(result == NSFileHandlingPanelOKButton) {
            NSString* fileNamePath = [[[openDlg URLs] objectAtIndex:0] path];
//            NSLog(@"user picked file with name %@", fileNamePath);
            // set the current working directory to the directory we found the device dylib in
//            Normal_out << "cwd is now " << [[[NSFileManager defaultManager] currentDirectoryPath] UTF8String] << endl;
            NSString* deviceDylibDirectoryPath = [fileNamePath stringByDeletingLastPathComponent];
            if ([[NSFileManager defaultManager] changeCurrentDirectoryPath:deviceDylibDirectoryPath]) {
                    Normal_out << "cwd is now " << [[[NSFileManager defaultManager] currentDirectoryPath] UTF8String] << endl;
                    }
                else
                    Normal_out << "Could not change working directory to " << [deviceDylibDirectoryPath UTF8String] << endl;

            Device_base* device_ptr = dylib_object_manager_ptr->load_and_create_device([fileNamePath UTF8String]);
			if(device_ptr) {
			try {
				model_ptr->set_device_ptr(device_ptr);
//				Normal_out << "Device " << [fileNamePath UTF8String] << " loaded" << endl;
				Normal_out << "Device " << [[fileNamePath lastPathComponent] UTF8String] << " loaded" << endl;
                run_state = (model_ptr->get_device_ptr() && model_ptr->get_compiled()) ? RUNNABLE : UNREADY;
 
  //               Normal_out << "current directory is now " << [currentPath UTF8String] << endl;
				}
			catch(exception& x) {
				Normal_out << x.what() << endl;
				return;
				}
			}
            }}];
*/
}

- (IBAction)pickedLoadVisualEncoder:(id)sender
{
    // Create a File Open Dialog object.
    NSOpenPanel* openDlg = [NSOpenPanel openPanel];
    [openDlg setTitle:@"Choose visual encoder dyblib"];
    [openDlg setAllowedFileTypes:@[@"dylib"]];
    NSInteger result = [openDlg runModal];
//    [openDlg beginWithCompletionHandler:^(NSInteger result) {
    if(result == NSFileHandlingPanelOKButton) {
        NSString* fileNamePath = [[[openDlg URLs] objectAtIndex:0] path];
 //          NSLog(@"user picked file with name %@", fileNamePath);
        Visual_encoder_base* visual_encoder_ptr = dylib_object_manager_ptr->load_and_create_visual_encoder([fileNamePath UTF8String]);
        if(visual_encoder_ptr) {
			try {
				model_ptr->set_visual_encoder_ptr(visual_encoder_ptr);
				Normal_out << "Visual encoder " << [[fileNamePath lastPathComponent] UTF8String] << " loaded" << endl;
				}
			catch(exception& x) {
				Normal_out << x.what() << endl;
				return;
				}
			}
        else
            Normal_out << "No visual encoder dyblib was loaded" << endl;
        }
    else
        Normal_out << "No visual encoder dyblib was loaded" << endl;
}

- (IBAction)pickedLoadAuditoryEncoder:(id)sender
{
    // Create a File Open Dialog object.
    NSOpenPanel* openDlg = [NSOpenPanel openPanel];
    [openDlg setTitle:@"Choose auditory encoder dyblib"];
    [openDlg setAllowedFileTypes:@[@"dylib"]];
    NSInteger result = [openDlg runModal];
//    [openDlg beginWithCompletionHandler:^(NSInteger result) {
    if(result == NSFileHandlingPanelOKButton) {
        NSString* fileNamePath = [[[openDlg URLs] objectAtIndex:0] path];
 //          NSLog(@"user picked file with name %@", fileNamePath);
        Auditory_encoder_base* auditory_encoder_ptr = dylib_object_manager_ptr->load_and_create_auditory_encoder([fileNamePath UTF8String]);
        if(auditory_encoder_ptr) {
			try {
				model_ptr->set_auditory_encoder_ptr(auditory_encoder_ptr);
				Normal_out << "Auditory encoder " << [[fileNamePath lastPathComponent] UTF8String] << " loaded" << endl;
				}
			catch(exception& x) {
				Normal_out << x.what() << endl;
				return;
				}
			}
        else
            Normal_out << "No auditory encoder dyblib was loaded" << endl;
        }
    else
        Normal_out << "No auditory encoder dyblib was loaded" << endl;

/*    // Create a File Open Dialog object.
    NSOpenPanel* openDlg = [NSOpenPanel openPanel];
    [openDlg setAllowedFileTypes:@[@"dylib"]];
    
    [openDlg beginWithCompletionHandler:^(NSInteger result) {
        if(result == NSFileHandlingPanelOKButton) {
            NSString* fileNamePath = [[[openDlg URLs] objectAtIndex:0] path];
//            NSLog(@"user picked file with name %@", fileNamePath);
            Auditory_encoder_base* auditory_encoder_ptr = dylib_object_manager_ptr->load_and_create_auditory_encoder([fileNamePath UTF8String]);
			if(auditory_encoder_ptr) {
			try {
				model_ptr->set_auditory_encoder_ptr(auditory_encoder_ptr);
				Normal_out << "Auditory encoder " << [[fileNamePath lastPathComponent] UTF8String] << " loaded" << endl;
				}
			catch(exception& x) {
				Normal_out << x.what() << endl;
				return;
				}
			}
            }}];
*/
}

- (IBAction)pickedCompile:(id)sender
{
    // Create a File Open Dialog object.
    NSOpenPanel* openDlg = [NSOpenPanel openPanel];
    [openDlg setTitle:@"Choose production rule file"];
    [openDlg setAllowedFileTypes:@[@"prs"]];
    NSInteger result = [openDlg runModal];
    NSString* fileNamePath;
    if(result == NSFileHandlingPanelOKButton) {
        fileNamePath = [[[openDlg URLs] objectAtIndex:0] path];
        try {
            model_ptr->set_prs_filename([fileNamePath UTF8String]);
            if(model_ptr->compile())
                Normal_out << "Production rule file " << [[fileNamePath lastPathComponent] UTF8String] << " compiled" << endl;
            run_state = (model_ptr->get_device_ptr() && model_ptr->get_compiled()) ? RUNNABLE : UNREADY;

            }
        catch(exception& x) {
            Normal_out << x.what() << endl;
            return;
            }
        }
    else
        Normal_out << "No file was selected" << endl;
 /*
     [openDlg beginWithCompletionHandler:^(NSInteger result) {
        if(result == NSFileHandlingPanelOKButton) {
            NSString* fileNamePath = [[[openDlg URLs] objectAtIndex:0] path];
 //           NSLog(@"user picked file with name %@", fileNamePath);
			
			try {
				model_ptr->set_prs_filename([fileNamePath UTF8String]);
				if(model_ptr->compile())
					Normal_out << "Production rule file " << [[fileNamePath lastPathComponent] UTF8String] << " compiled" << endl;
                run_state = (model_ptr->get_device_ptr() && model_ptr->get_compiled()) ? RUNNABLE : UNREADY;

				}
			catch(exception& x) {
				Normal_out << x.what() << endl;
				return;
				}
			}
            }];
*/
}

- (IBAction)pickedRecompile:(id)sender
{
	try {
		if(model_ptr->compile()) {
			Normal_out << "Production rule file " << model_ptr->get_prs_filename() << " recompiled" << endl;
            }
		}
	catch(exception& x) {
		Normal_out << x.what() << endl;
		return;
		}
    run_state = (model_ptr->get_device_ptr() && model_ptr->get_compiled()) ? RUNNABLE : UNREADY;
}

- (IBAction)pickedGoOneStep:(id)sender
{
    // temporary for testing
    //model_ptr->set_output_run_messages(true);
    
    try {// run one step, then refresh the views
	run_result = model_ptr->run_time(sim_time_per_step);
    current_time = model_ptr->get_time();
    [self refreshViews];
	if(!run_result) 
		run_state = RUNNABLE;	// run was completed, model is stopped
	else
		run_state = PAUSED;
    }
 	catch(exception& x) {
		Normal_out << x.what() << endl;
		return;
		}   
}

- (IBAction)pickedRun:(id)sender
{
	// in either state RUNNABLE or PAUSED at this point
	switch(run_state) {
		case RUNNABLE:{ // starting up - model automatically initializes
			normal_out_ptr->clear();  // clear the normal out window ... was missing 8/19/14 ...
			switch(run_duration_setting) {
				case RUN_FOR_TIME:
					run_time_limit = run_for_time;
					break;
				case RUN_UNTIL_TIME:
				case RUN_UNTIL_DONE:
				default:
					run_time_limit = 0;
					break;
					}
				}
			break;
		case PAUSED:	// we were already running - we might have changed settings
			current_time = model_ptr->get_time();
			switch(run_duration_setting) {
				case RUN_FOR_TIME:
					// go again if we ran for the specified amount of time or we paused and then changed
					if(current_time >= run_time_limit)
						run_time_limit = current_time + run_for_time;	// go again
					break;
				case RUN_UNTIL_TIME:
					// are we past the specified time or did we pause and change from run until done?
					if(current_time >= run_until_time) {
						NSBeep();		// don't let the user go again without changing settings
						return;
						}
					run_time_limit = 0;
					break;
				case RUN_UNTIL_DONE:
				default:
					run_time_limit = 0;
					break;
				}
		default:
			break;
		}
		
	switch(refresh_setting) {
		case EACH_STEP:
			run_time_per_refresh = sim_time_per_step;
			break;
		case AFTER_EVERY_TIME:
			run_time_per_refresh = time_per_refresh;
			break;
		case NONE_DURING_RUN:
			run_time_per_refresh = LONG_MAX;
			break;
		default:
			break;
		}
	
	run_state = RUNNING;
    // here to inform OS that this is an initiated run, no App Nap, please
    /*
@property (strong) id activity;
and then used this implementation.

    if ([[NSProcessInfo processInfo] respondsToSelector:@selector(beginActivityWithOptions:reason:)]) {
    self.activity = [[NSProcessInfo processInfo] beginActivityWithOptions:0x00FFFFFF reason:@"receiving OSC messages"];
}

    if ([[NSProcessInfo processInfo] respondsToSelector:@selector(beginActivityWithOptions:reason:)]) 
    {
        self.activity = [[NSProcessInfo processInfo] beginActivityWithOptions:NSActivityUserInitiated reason:@"starting simulation run"];

[[NSProcessInfo processInfo] endActivity:self.activity];

*/
    // Turn off App Nap so this will run in the background, so the user can do something else in the meantime.
/*    if ([[NSProcessInfo processInfo] respondsToSelector:@selector(beginActivityWithOptions:reason:)])
    {
        self.activity = [[NSProcessInfo processInfo] beginActivityWithOptions:NSActivityUserInitiated reason:@"starting simulation run"];
        NSLog(@"activity set to %@", self.activity);
    }
*/
    [self turnAppNapOff];

	if(run_continuously) {
		[self doRunContinuously];	// no timer
		}
	else {
        timer = [NSTimer scheduledTimerWithTimeInterval:real_time_per_step target:self selector:@selector(run_next_cycle_and_refresh_periodically) userInfo:Nil repeats:YES];
		}

}

- (void)turnAppNapOff
{
    // here to inform OS that this is an initiated run, no App Nap, please
    /*
@property (strong) id activity;
and then used this implementation.

    if ([[NSProcessInfo processInfo] respondsToSelector:@selector(beginActivityWithOptions:reason:)]) {
    self.activity = [[NSProcessInfo processInfo] beginActivityWithOptions:0x00FFFFFF reason:@"receiving OSC messages"];
}

    if ([[NSProcessInfo processInfo] respondsToSelector:@selector(beginActivityWithOptions:reason:)]) 
    {
        self.activity = [[NSProcessInfo processInfo] beginActivityWithOptions:NSActivityUserInitiated reason:@"starting simulation run"];

[[NSProcessInfo processInfo] endActivity:self.activity];

*/
    // Turn off App Nap so this will run in the background, so the user can do something else in the meantime.
    if ([[NSProcessInfo processInfo] respondsToSelector:@selector(beginActivityWithOptions:reason:)])
    {
        self.activity = [[NSProcessInfo processInfo] beginActivityWithOptions:NSActivityUserInitiated reason:@"starting simulation run"];
//        NSLog(@"activity set to %@", self.activity);
    }

}

- (void)turnAppNapBackOn
{
    // Re-enable App Nap.
//    NSLog(@"activity reset from %@", self.activity);
    [[NSProcessInfo processInfo] endActivity:self.activity];

}

- (void)run_next_cycle_and_refresh_periodically
{
//   assert(_run_state == RUNNING);
    try {
        run_result = model_ptr->run_time(sim_time_per_step);
        }
    catch(exception& x) {
        Normal_out << "Error:" << x.what() << endl;
        model_ptr->display_memory_contents();   //dump current PS memory contents to help debugging
        [self refreshViews]; // show current view contents
        [self haltSimulation];
        return;
        }
    
    current_time = model_ptr->get_time();
	if(run_result && (
        (run_duration_setting == RUN_FOR_TIME && current_time < run_time_limit) ||
        (run_duration_setting == RUN_UNTIL_TIME && current_time < run_until_time) ||
        (run_duration_setting == RUN_UNTIL_DONE))) {
            if(current_time % run_time_per_refresh == 0) {
                [self refreshViews];
                }
            }
	else {
		[timer invalidate];
        [self refreshViews];
        [self turnAppNapBackOn];
		// is the run complete? if true returned, can keep running so we are paused
		run_state = (run_result) ? PAUSED : RUNNABLE;
		if(run_state == PAUSED) {
			if(run_duration_setting == RUN_UNTIL_TIME || run_duration_setting == RUN_FOR_TIME)
				Normal_out << "Run command complete" << endl;
            }
		if(run_state == RUNNABLE) {
			Normal_out << "Run command complete" << endl;
            }
        }
}

- (void) refreshViews
{
    for(auto v_ptr : epic_draw_view_ptrs) {
            v_ptr->notify_time(model_ptr->get_time());
            v_ptr->refresh();
        }
}

- (void) clearViews
{
    for(auto v_ptr : epic_draw_view_ptrs) {
            v_ptr->notify_time(model_ptr->get_time());
            v_ptr->clear();
        }
}

- (void) haltSimulation
{
//	assert(run_state == RUNNING || run_state == PAUSED);
	if(run_state == RUNNING) {
        [timer invalidate];
        [self turnAppNapBackOn];
		}
	model_ptr->stop();
	run_state = RUNNABLE;
	Normal_out << "Run halted" << endl;
}



- (void) doRunContinuously
{
	do {
		run_result = model_ptr->run_time(sim_time_per_step);
		current_time = model_ptr->get_time();
		} while (run_result && (
			(run_duration_setting == RUN_FOR_TIME && current_time < run_time_limit) ||
			(run_duration_setting == RUN_UNTIL_TIME && current_time < run_until_time) ||
			(run_duration_setting == RUN_UNTIL_DONE)
			)
		);
	[self refreshViews];
    [self turnAppNapBackOn];
	if(!run_result) 
		run_state = RUNNABLE;	// run was completed, model is stopped
	else
		run_state = PAUSED;
	if(run_state == RUNNABLE) {
		Normal_out << "Run Completed!" << endl;
		}
}

- (IBAction)pickedPause:(id)sender
{
	//assert(run_state == RUNNING);
    [timer invalidate];
	run_state = PAUSED;
	Normal_out << "Run paused" << endl;
}

- (IBAction)pickedHalt:(id)sender
{
    [self haltSimulation];

//	assert(run_state == RUNNING || run_state == PAUSED);
	if(run_state == RUNNING) {
        [timer invalidate];
		}
	model_ptr->stop();
	run_state = RUNNABLE;
	Normal_out << "Run halted" << endl;
}

- (IBAction)pickedRunControls:(id)sender
{
 /*
enum Run_duration_e {RUN_FOR_TIME, RUN_UNTIL_TIME, RUN_UNTIL_DONE};
enum Refresh_e {EACH_STEP, AFTER_EVERY_TIME, NONE_DURING_RUN};

    run_state = UNREADY;
    run_duration_setting = RUN_FOR_TIME;
    run_continuously = false;
    refresh_setting = EACH_STEP;
	// integral time values are in ms
    sim_time_per_step = 50;
    real_time_per_step = 0.00001;
    run_for_time = 5000;
    run_until_time = 600000;
    time_per_refresh = 1000;
    run_result = false;
    run_time = 0;
    run_time_limit = 0;
    run_time_per_refresh = 0;
*/

    // radio buttons are tagged as 0, 1, 2, 3 from the top - explicitly map them
    // along with other run controls
    int rundurstate = 0;
    switch(run_duration_setting) {
        case RUN_FOR_TIME:
            rundurstate = 0;
            break;
        case RUN_UNTIL_TIME:
            rundurstate = 1;
            break;
        case RUN_UNTIL_DONE:
            rundurstate = 2;
            break;
		default:
			if(run_continuously)
				rundurstate = 3;
			else
				rundurstate = 0;
			break;
            }
	int refreshsetting = 0;
	
	switch(refresh_setting) {
		default:
		case EACH_STEP:
			refreshsetting = 0;
			break;
		case AFTER_EVERY_TIME:
			refreshsetting = 1;
			break;
		case NONE_DURING_RUN:
			refreshsetting = 2;
			break;
		}
    NSString* deviceparameterstring =
		[NSString stringWithUTF8String:model_ptr->get_device_parameter_string().c_str()];
	
    RunControlsDialog* dialogController = [[RunControlsDialog alloc]
        initWithRunComDurState:rundurstate runForTime:run_for_time/1000.
		runUntilTime:run_until_time/1000.
		runContinuously:run_continuously 
        refreshDisplayState:refreshsetting
		refreshDisplayTime:time_per_refresh/1000.
        deviceParameterString:deviceparameterstring];
    
    NSInteger return_code = [NSApp runModalForWindow:[dialogController window]];
	if(return_code == NSModalResponseOK) {
		switch([dialogController getRunCommandDurationState]) {
			default:
			case 0:
				run_duration_setting = RUN_FOR_TIME;
				break;
			case 1:
				run_duration_setting = RUN_UNTIL_TIME;
				break;
			case 2:
				run_duration_setting = RUN_UNTIL_DONE;
				break;
			}
		
		run_for_time = [dialogController getRunForTime] * 1000.;
		run_until_time = [dialogController getRunUtilTime] * 1000.;
		run_continuously = [dialogController getRunContinuouslyState];
		switch([dialogController getDisplayRefreshState]) {
			default:
			case 0:
				refresh_setting = EACH_STEP;
				break;
			case 1:
				refresh_setting = AFTER_EVERY_TIME;
				break;
			case 2:
				refresh_setting = NONE_DURING_RUN;
				break;
			}
		time_per_refresh = [dialogController getDisplayRefreshTime] * 1000.;
        string new_parameter_string = [[dialogController getDeviceParmeterString] UTF8String];
        
        // wrap a try-catch around this to handle error exceptions thrown by device
        try {
            model_ptr->set_device_parameter_string(new_parameter_string);
            }
        catch(exception& x) {
            Normal_out << "Error: " << x.what() << endl;
            }
		}
}

- (IBAction)pickedOutputSettings:(id)sender
{
	OutputControlsDialog* dialogController =
	[[OutputControlsDialog alloc]
		initWithMemoryContents:model_ptr->get_output_run_memory_contents()
		runMessages: model_ptr->get_output_run_messages()
		compilerMessages: model_ptr->get_output_compiler_messages()
		compilerDetails: model_ptr->get_output_compiler_details()
		runDetails: model_ptr->get_output_run_details()
		pixelsPerDegree:pixels_per_degree
		calibrationGridState:calibration_grid_state];

    NSInteger return_code = [NSApp runModalForWindow:[dialogController window]];
	if(return_code == NSModalResponseOK) {
		model_ptr->set_output_run_memory_contents([dialogController getMemoryContentsState]);
		model_ptr->set_output_run_messages([dialogController getRunMessagesState]);
		model_ptr->set_output_compiler_messages([dialogController getCompilerMessagesState]);
		model_ptr->set_output_compiler_details([dialogController getCompilerDetailsState]);
		model_ptr->set_output_run_details([dialogController getRunDetailsState]);
		// send these to the visual views
		pixels_per_degree = [dialogController getPixelsPerDegree];
		for(auto view_ptr : epic_draw_view_ptrs) 
			view_ptr->set_scale(pixels_per_degree);
		calibration_grid_state = [dialogController getCalibratioGridState];
		for(auto view_ptr : epic_draw_view_ptrs)
			view_ptr->set_grid_on(calibration_grid_state);
		[self refreshViews];
		}
}

- (IBAction)pickedTracingSettings:(id)sender
{
	TraceControlsDialog* dialogController =
	[[TraceControlsDialog alloc] init];
    NSInteger return_code = [NSApp runModalForWindow:[dialogController window]];
	if(return_code == NSModalResponseOK) {
        // anybody being traced?
        bool trace_on =
            model_ptr->get_trace_visual() ||
            model_ptr->get_trace_auditory() ||
            model_ptr->get_trace_cognitive() ||
            model_ptr->get_trace_ocular() ||
            model_ptr->get_trace_manual() ||
            model_ptr->get_trace_vocal() ||
            model_ptr->get_trace_device() ||
            model_ptr->get_trace_temporal();
        if(trace_on && !trace_out_ptr) {
            // open a trace out text window
            [self openTrace_outWindow];
            }
        if(!trace_on && trace_out_ptr) {
            // close the trace out window
            [self closeTrace_outWindow];
            }
        // otherwise the window is either closed or is needed.
        }
}

- (IBAction)pickedLoggingSettings:(id)sender
{
    LogControlsDialog* dialogController =
	[[LogControlsDialog alloc] init];
    // the dialog window takes care of everything because the objects involved
    // are global objects -no need for us to do anything here
    [NSApp runModalForWindow:[dialogController window]];



/*    NSString* filename = [NSString stringWithUTF8String:"Normal_out_log.txt"];
    
    NSSavePanel* saveDlg = [NSSavePanel savePanel];
    [saveDlg setNameFieldStringValue:filename];
    NSInteger result = [saveDlg runModal];
    NSString* fileNamePath;
    if(result == NSFileHandlingPanelOKButton) {
        fileNamePath = [[saveDlg URL] path];
        NSLog(@"path selected is %@", fileNamePath);
        }
*/
/*    [panel beginSheetModalForWindow:window completionHandler:^(NSInteger result){
        if (result == NSFileHandlingPanelOKButton)
        {
            NSURL*  theFile = [panel URL];
 
            // Write the contents in the new format.
        }
    }];
 */ 
 }

- (IBAction)pickedBreaksSettings:(id)sender
{
 	NSMutableArray *strings = [[NSMutableArray alloc] init];
    // get rule names from the model
    const Symbol_list_t rule_names = model_ptr->get_rule_names();
    // create StringWithState items for each rule name and set the state
    for(const Symbol& rule_name : rule_names) {
        NSString* name = [NSString stringWithUTF8String:rule_name.c_str()];
        BOOL state = model_ptr->get_rule_break_state(rule_name);
        StringWithState* newstring = [[StringWithState alloc] initWithString:name state:state];
        [strings addObject:newstring];
        }
    BOOL break_enabled = model_ptr->get_break_enabled();
     BreakControlsDialog*  breakcontrolsdialogcontroller = [[BreakControlsDialog alloc] initWithStringsWithState:strings enabled:break_enabled];
    
//	if(!breakcontrolsdialogcontroller) {
 //       breakcontrolsdialogcontroller = [[BreakControlsDialog alloc] init];
//        }

    NSInteger return_code = [NSApp runModalForWindow:[breakcontrolsdialogcontroller window]];
    if(return_code == NSModalResponseOK) {
        // shouldn't have to compare rule names - they should not have changed
        // can do in order
        NSMutableArray* the_results = [breakcontrolsdialogcontroller stringsWithState];
        auto it = rule_names.begin();
        for(int i = 0; i < [the_results count]; i++, ++it) {
            StringWithState* thestring = [the_results objectAtIndex:i];
            BOOL new_state = thestring.theState;
            model_ptr->set_rule_break_state(*it, new_state);
            }
        BOOL new_enable = [breakcontrolsdialogcontroller getCheckBoxState];
        model_ptr->set_break_enabled(new_enable);
        }
}
@end
