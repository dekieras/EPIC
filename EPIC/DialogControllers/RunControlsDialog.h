//
//  RunControlsDialog.h
//  EPIC
//
//  Created by David Kieras on 8/15/13.
//  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
//

#import <Cocoa/Cocoa.h>

@interface RunControlsDialog : NSWindowController
{
    NSInteger dialogCloseButtonValue;
    int theCommandDurationState;
    double theRunForTime;
    double theRunUntilTime;
    BOOL theRunContinuouslyState;
    int theDisplayRefreshState;
    double theDisplayRefreshTime;
    NSString* theDeviceParameterString;
}
- (id) initWithRunComDurState:(int)theCommandDurationState_ runForTime:(double)theRunForTime_
    runUntilTime:(double)theRunUntilTime_
	runContinuously:(BOOL)theRunContinuouslyState_
	refreshDisplayState:(int) theDisplayRefreshState_
    refreshDisplayTime:(double)theDisplayRefreshTime_
	deviceParameterString:(NSString*)theDeviceParameterString_;
- (int) getRunCommandDurationState;
- (double) getRunForTime;
- (double) getRunUtilTime;
- (BOOL) getRunContinuouslyState;
- (int) getDisplayRefreshState;
- (double) getDisplayRefreshTime;
- (NSString*) getDeviceParmeterString;
@property (nonatomic, strong) IBOutlet NSTextField *fieldRunFor;
@property (nonatomic, strong) IBOutlet NSTextField *fieldRunUntil;
@property (nonatomic, strong) IBOutlet NSTextField *fieldDisplayRefresh;
@property (nonatomic, strong) IBOutlet NSTextField *fieldDeviceParameterString;
@property (nonatomic, strong) IBOutlet NSButton *checkBoxRunContinuously;
@property (nonatomic, strong) IBOutlet NSMatrix *radioButtonCommandDuration;
@property (nonatomic, strong) IBOutlet NSMatrix *radioButtonDisplayRefresh;
- (IBAction)pushOKButton:(id)sender;
- (IBAction)pushCancelButton:(id)sender;
// called when window appears from RunModal
//- (void)windowDidBecomeKey:(NSNotification*)notification;
// called when window will be closed, even from red button
- (void)windowWillClose:(NSNotification*)notification;

@end
