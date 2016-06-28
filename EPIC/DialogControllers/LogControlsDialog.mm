//
//  LogControlsDialog.m
//  EPIC
//
//  Created by David Kieras on 8/23/13.
//  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
//

#import "LogControlsDialog.h"
#include "EPICLib/Output_tee_globals.h"
#include "EPICLib/PPS_globals.h"
#include "AppGlobals.h"
#include <fstream>

using std::ofstream;

@interface LogControlsDialog ()

@end

@implementation LogControlsDialog

- (id) init
{
    self = [super initWithWindowNibName:@"LogControlsDialog"];
    if (self) {
    // no initializations needed
    }
    
    return self;
}

- (void)windowDidLoad
{
    [super windowDidLoad];
    if(logfile_stream.is_open()) 
        [_openFileButton setTitle:@"Close Log File"];
    else
        [_openFileButton setTitle:@"Open Log File"];

//    if(Normal_out.is_present(logfile_stream))
//        [_checkBoxNormalOutput setState:YES];
    [_checkBoxNormalOutput setState:Normal_out.is_present(logfile_stream)];
    [_checkBoxTraceOutput setState:Trace_out.is_present(logfile_stream)];

}

- (IBAction)pickedFileButton:(id)sender
{
    if(logfile_stream.is_open()) {
        logfile_stream.close();
        [_openFileButton setTitle:@"Open Log File"];
        return;
        }
    // user wants to open a new log file
    NSString* filename = [NSString stringWithUTF8String:"EPIC_log.txt"];
    
    NSSavePanel* saveDlg = [NSSavePanel savePanel];
    [saveDlg setNameFieldStringValue:filename];
    NSInteger result = [saveDlg runModal];
    NSString* fileNamePath;
    if(result == NSFileHandlingPanelOKButton) {
        fileNamePath = [[saveDlg URL] path];
        NSLog(@"path selected is %@", fileNamePath);
        logfile_stream.open([fileNamePath UTF8String]);
        if(logfile_stream.is_open()) {
            Normal_out << "Now logging to " << [fileNamePath UTF8String] << std::endl;
            [_openFileButton setTitle:@"Close Log File"];
            return;
            }
        else {
            Normal_out << "Could not open file " << [fileNamePath UTF8String] << std::endl;
            return;
            }
        }
}

- (IBAction)pickedOKButton:(id)sender
{
    if([_checkBoxNormalOutput state]) {
        if(!Normal_out.is_present(logfile_stream)) {
            Normal_out.add_stream(logfile_stream);
            PPS_out.add_stream(logfile_stream);
            }
        }
    else {
        Normal_out.remove_stream(logfile_stream);
        PPS_out.remove_stream(logfile_stream);
        }
        

    if([_checkBoxTraceOutput state]) {
        if(!Trace_out.is_present(logfile_stream)) {
            Trace_out.add_stream(logfile_stream);
            }
        }
    else {
        Trace_out.remove_stream(logfile_stream);
        }


    dialogCloseButtonValue = NSModalResponseOK;
    [[self window] close];
}

- (IBAction)pickedCancelButton:(id)sender
{
    dialogCloseButtonValue = NSModalResponseCancel;
    [[self window] close];
}

- (void)windowWillClose:(NSNotification*)notification
{
    // the red close button comes here with whatever CloseButtonValue was set
    [NSApp stopModalWithCode:dialogCloseButtonValue];
}


@end
