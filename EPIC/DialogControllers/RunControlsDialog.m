//
//  RunControlsDialog.m
//  EPIC
//
//  Created by David Kieras on 8/15/13.
//  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
//

#import "RunControlsDialog.h"

@interface RunControlsDialog ()

@end

@implementation RunControlsDialog

- (id) init
{
    return
        [self initWithRunComDurState:0 runForTime:0.0 runUntilTime:0.0
        runContinuously:NO refreshDisplayState:0 refreshDisplayTime:0.0        deviceParameterString:@"Default Initial Contents"];
}

- (id) initWithRunComDurState:(int)theCommandDurationState_ runForTime:(double)theRunForTime_
    runUntilTime:(double)theRunUntilTime_
	runContinuously:(BOOL)theRunContinuouslyState_
	refreshDisplayState:(int)theDisplayRefreshState_
    refreshDisplayTime:(double)theDisplayRefreshTime_ deviceParameterString:(NSString*)theDeviceParameterString_
{
    self = [super initWithWindowNibName:@"RunControlsDialog"];
    if (self) {
        theCommandDurationState = theCommandDurationState_;
        theRunForTime = theRunForTime_;
        theRunUntilTime = theRunUntilTime_;
		theRunContinuouslyState = theRunContinuouslyState_;
        theDisplayRefreshState = theDisplayRefreshState_;
        theDisplayRefreshTime = theDisplayRefreshTime_;
        theDeviceParameterString = theDeviceParameterString_;
//        NSLog(@"NSApp has %@ and %@\n", (AppDelegate*)[NSApp delegate].topStringValue, NSApp.topBoolValue);
		dialogCloseButtonValue = NSModalResponseCancel;
        }
    return self;
}

- (void)windowDidLoad
{
    [super windowDidLoad];
    
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
    // set the state of the radio button - the "tag" is set in IB
    // let top one be 0, then 1, then 2 - returned value is YES if OK
    [_radioButtonCommandDuration selectCellWithTag:theCommandDurationState];
 
    NSString* runforstring = [[NSString alloc] initWithFormat:@"%10.3lf", theRunForTime];
    [_fieldRunFor setStringValue:runforstring];
    NSString* rununtilstring = [[NSString alloc] initWithFormat:@"%10.3lf", theRunUntilTime];
    [_fieldRunUntil setStringValue:rununtilstring];
	[_checkBoxRunContinuously setState:theRunContinuouslyState];
    // to set the state - set in IB 0 is top cell - returned value is YES if OK
    [_radioButtonDisplayRefresh selectCellWithTag:theDisplayRefreshState];
    
    NSString* displayrefreshtimestring = [[NSString alloc] initWithFormat:@"%10.3lf", theDisplayRefreshTime];
    [_fieldDisplayRefresh setStringValue:displayrefreshtimestring];

    [_fieldDeviceParameterString setStringValue:theDeviceParameterString];

    [[self window] center];
}

- (IBAction)pushOKButton:(id)sender
{
    // if user hit OK, then we save the values
    // get the state of the radio button
    NSButtonCell *selCell = [_radioButtonCommandDuration selectedCell];
    
//    NSLog(@"Selected cell is %ld", (long)[selCell tag]);
    theCommandDurationState = (int)[selCell tag];
//    NSLog(@"theCommandDurationState is %d\n", theCommandDurationState);

    theRunForTime = [[_fieldRunFor stringValue] doubleValue];
    theRunUntilTime = [[_fieldRunUntil stringValue] doubleValue];
    theRunContinuouslyState = [_checkBoxRunContinuously state];

    theDisplayRefreshState = (int)[[_radioButtonDisplayRefresh selectedCell] tag];
//    NSLog(@"displayRefreshState is %d\n", theDisplayRefreshState);
    theDisplayRefreshTime = [[_fieldDisplayRefresh stringValue] doubleValue];
    
    theDeviceParameterString = [_fieldDeviceParameterString stringValue];

    dialogCloseButtonValue = NSModalResponseOK;
    [[self window] close];
}


- (IBAction)pushCancelButton:(id)sender
{
    dialogCloseButtonValue = NSModalResponseCancel;
    [[self window] close];
}

- (void)windowWillClose:(NSNotification*)notification
{
    // the red close button comes here with whatever CloseButtonValue was set
    [NSApp stopModalWithCode:dialogCloseButtonValue];
}

- (int) getRunCommandDurationState
{
	return theCommandDurationState;
}
- (double) getRunForTime
{
	return theRunForTime;
}
- (double) getRunUtilTime;
{
	return theRunUntilTime;
}
- (BOOL) getRunContinuouslyState
{
	return theRunContinuouslyState;
}
- (int) getDisplayRefreshState
{
	return theDisplayRefreshState;
}
- (double) getDisplayRefreshTime
{
	return theDisplayRefreshTime;
}
- (NSString*) getDeviceParmeterString
{
	return theDeviceParameterString;
}







@end
/*
doubleValue
Returns the floating-point value of the receiver’s text as a double.

- (double)doubleValue
Return Value
The floating-point value of the receiver’s text as a double. Returns HUGE_VAL or –HUGE_VAL on overflow, 0.0 on underflow. Returns 0.0 if the receiver doesn’t begin with a valid text representation of a floating-point number.

Discussion
This method skips any whitespace at the beginning of the string. This method uses formatting information stored in the non-localized value; use an NSScanner object for localized scanning of numeric values from a string.

integerValue
Returns the NSInteger value of the receiver’s text.

- (NSInteger)integerValue
Return Value
The NSInteger value of the receiver’s text, assuming a decimal representation and skipping whitespace at the beginning of the string. Returns 0 if the receiver doesn’t begin with a valid decimal text representation of a number.

Discussion
This method uses formatting information stored in the non-localized value; use an NSScanner object for localized scanning of numeric values from a string.




A group of radio buttons or checkboxes is programmatically an NSMatrix object whose constituent objects are NSButtonCell objects. Matrix objects are a special kind of control. Each of its cells can have its own target object and action selector specified. Additionally, an NSMatrix may have its own target and action selector. (For more on target-action in relation to matrix objects, see Matrix Programming Guide.)

To find out which radio button or checkbox a user selected—at the moment he or she clicks it—you could specify a target and a different action selector for each cell in the matrix, and then implement the corresponding action method. However, a more efficient way to query the current selection in matrices of radio buttons or checkboxes is to implement target-action for the NSMatrix object itself, and in the action method determine which cell (or cells) are now selected. The NSMatrix methods for this are selectedCell and selectedCells.

Listing 1 shows an implementation of an action method that responds to a selection in a matrix of radio buttons.

Listing 1  Querying a matrix object for the selected radio-button cell
- (IBAction)findSelectedButton:(id)sender { // sender is NSMatrix object
    NSButtonCell *selCell = [sender selectedCell];
    NSLog(@"Selected cell is %d", [selCell tag]);
}
This code snippet illustrates another technique you can apply when handling selection of cells in matrices. You can assign numeric tags to each cell in a matrix to identify it, and then query for those tag values when handling selections.

cells numbered from 0 at top/left



*/
