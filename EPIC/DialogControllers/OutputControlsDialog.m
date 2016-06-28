//
//  OutputControlsDialog.m
//  EPIC
//
//  Created by David Kieras on 8/16/13.
//  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
//

#import "OutputControlsDialog.h"

@interface OutputControlsDialog ()
{
    NSInteger dialogCloseButtonValue;
	BOOL theMemoryContentsState;
	BOOL theRunMessagesState;
	BOOL theCompilerMessagesState;
	BOOL theCompilerDetailsState;
	BOOL theRunDetailsState;
	int thePixelsPerDegree;
	BOOL theCalibrationGridState;
}

@end

@implementation OutputControlsDialog

- (id) init
{
    return
        [self initWithMemoryContents:NO
			runMessages:NO compilerMessages:NO
			compilerDetails:NO runDetails:NO
			pixelsPerDegree:1
			calibrationGridState:NO];
}

- (id) initWithMemoryContents:(BOOL)theMemoryContentsState_
	runMessages: (BOOL)theRunMessagesState_
	compilerMessages: (BOOL)theCompilerMessagesState_
	compilerDetails: (BOOL)theCompilerDetailsState_
	runDetails: (BOOL)theRunDetailsState_
	pixelsPerDegree:(int)thePixelsPerDegree_
	calibrationGridState:(BOOL)theCalibrationGridState_
{
    self = [super initWithWindowNibName:@"OutputControlsDialog"];
    if (self) {
		theMemoryContentsState = theMemoryContentsState_;
		theRunMessagesState = theRunMessagesState_;
		theCompilerMessagesState = theCompilerMessagesState_;
		theCompilerDetailsState = theCompilerDetailsState_;
		theRunDetailsState = theRunDetailsState_;
		thePixelsPerDegree = thePixelsPerDegree_;
		theCalibrationGridState = theCalibrationGridState_;
		dialogCloseButtonValue = NSModalResponseCancel;

    }
    
    return self;
}
- (BOOL) getMemoryContentsState {return theMemoryContentsState;}
- (BOOL) getRunMessagesState {return theRunMessagesState;}
- (BOOL) getCompilerMessagesState {return theCompilerMessagesState;}
- (BOOL) getCompilerDetailsState {return theCompilerDetailsState;}
- (BOOL) getRunDetailsState {return theRunDetailsState;}
- (int) getPixelsPerDegree {return thePixelsPerDegree;}
- (BOOL) getCalibratioGridState {return theCalibrationGridState;}



- (void)windowDidLoad
{
    [super windowDidLoad];
    
	[_checkBoxMemoryContents setState:theMemoryContentsState];
	[_checkBoxRunMessages setState:theRunMessagesState];
	[_checkBoxCompilerMessages setState:theCompilerMessagesState];
	[_checkBoxCompilerDetails setState:theCompilerDetailsState];
	[_checkBoxRunDetails setState:theRunDetailsState];
    NSString* pixelsperdegreestring = [[NSString alloc] initWithFormat:@"%d", thePixelsPerDegree];
    [_fieldPixelsPerDegree setStringValue:pixelsperdegreestring];
	[_checkBoxCalibrationGrid setState:theCalibrationGridState];
	
	[[self window] center];
}

- (IBAction)pickedOKButton:(id)sender
{
    // if user hit OK, then we save the values
	theMemoryContentsState = [_checkBoxMemoryContents state];
	theRunMessagesState = [_checkBoxRunMessages state];
	theCompilerMessagesState = [_checkBoxCompilerMessages state];
	theCompilerDetailsState = [_checkBoxCompilerDetails state];
	theRunDetailsState = [_checkBoxRunDetails state];
	theCalibrationGridState = [_checkBoxCalibrationGrid state];

	thePixelsPerDegree = [[_fieldPixelsPerDegree stringValue] intValue];

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
    // the red close button comes here, treat as cancel???
    NSLog(@"windowWillClose");
    [NSApp stopModalWithCode:dialogCloseButtonValue];
//    dialogCloseButtonValue = NSCancelButton;
}

@end
