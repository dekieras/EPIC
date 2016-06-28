//
//  TraceControlsDialog.m
//  EPIC
//
//  Created by David Kieras on 8/22/13.
//  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
//

#import "TraceControlsDialog.h"
#include "AppGlobals.h"
#include "EPICLib/Model.h"

@interface TraceControlsDialog ()

@end

@implementation TraceControlsDialog

- (id) init
{
    self = [super initWithWindowNibName:@"TraceControlsDialog"];
    if (self) {
    // no initializations needed
    }
    
    return self;
}

- (void)windowDidLoad
{
    [super windowDidLoad];
    [_checkBoxVisual setState:model_ptr->get_trace_visual()];    
    [_checkBoxAuditory setState:model_ptr->get_trace_auditory()];
    [_checkBoxCognitive setState:model_ptr->get_trace_cognitive()];
    [_checkBoxOcular setState:model_ptr->get_trace_ocular()];
    [_checkBoxManual setState:model_ptr->get_trace_manual()];
    [_checkBoxVocal setState:model_ptr->get_trace_vocal()];
    [_checkBoxTemporal setState:model_ptr->get_trace_temporal()];
    [_checkBoxDevice setState:model_ptr->get_trace_device()];
}

- (IBAction)pickedOKButton:(id)sender
{
    model_ptr->set_trace_visual([_checkBoxVisual state]);
    model_ptr->set_trace_auditory([_checkBoxAuditory state]);
    model_ptr->set_trace_cognitive([_checkBoxCognitive state]);
    model_ptr->set_trace_ocular([_checkBoxOcular state]);
    model_ptr->set_trace_manual([_checkBoxManual state]);
    model_ptr->set_trace_vocal([_checkBoxVocal state]);
    model_ptr->set_trace_temporal([_checkBoxTemporal state]);
    model_ptr->set_trace_device([_checkBoxDevice state]);

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


