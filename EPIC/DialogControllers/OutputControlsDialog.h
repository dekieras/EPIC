//
//  OutputControlsDialog.h
//  EPIC
//
//  Created by David Kieras on 8/16/13.
//  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
//

#import <Cocoa/Cocoa.h>

@interface OutputControlsDialog : NSWindowController
- (id) initWithMemoryContents:(BOOL)theMemoryContentsState_
	runMessages: (BOOL)theRunMessagesState_
	compilerMessages: (BOOL)theCompilerMessagesState_
	compilerDetails: (BOOL)theCompilerDetailsState_
	runDetails: (BOOL)theRunDetailsState_
	pixelsPerDegree:(int)thePixelsPerDegree_
	calibrationGridState:(BOOL)theCalibrationGridState_;
- (BOOL) getMemoryContentsState;
- (BOOL) getRunMessagesState;
- (BOOL) getCompilerMessagesState;
- (BOOL) getCompilerDetailsState;
- (BOOL) getRunDetailsState;
- (int) getPixelsPerDegree;
- (BOOL) getCalibratioGridState;

@property (nonatomic, strong) IBOutlet NSButton *checkBoxMemoryContents;
@property (nonatomic, strong) IBOutlet NSButton *checkBoxRunMessages;
@property (nonatomic, strong) IBOutlet NSButton *checkBoxCompilerMessages;
@property (nonatomic, strong) IBOutlet NSButton *checkBoxCompilerDetails;
@property (nonatomic, strong) IBOutlet NSButton *checkBoxRunDetails;
@property (nonatomic, strong) IBOutlet NSTextField *fieldPixelsPerDegree;
@property (nonatomic, strong) IBOutlet NSButton *checkBoxCalibrationGrid;
- (IBAction)pickedOKButton:(id)sender;
- (IBAction)pickedCancelButton:(id)sender;

@end
