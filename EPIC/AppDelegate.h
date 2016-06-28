//
//  AppDelegate.h
//  EPIC
//
//  Created by David Kieras on 8/4/13.
//  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
//

#import <Cocoa/Cocoa.h>
class EPICDrawView;

@interface AppDelegate : NSObject <NSApplicationDelegate>
{}
@property (nonatomic, strong) id activity;  // for App Nap control
- (IBAction)pickedLoadDevice:(id)sender;
- (IBAction)pickedLoadVisualEncoder:(id)sender;
- (IBAction)pickedLoadAuditoryEncoder:(id)sender;
- (IBAction)pickedCompile:(id)sender;
- (IBAction)pickedRecompile:(id)sender;
- (IBAction)pickedGoOneStep:(id)sender;
- (IBAction)pickedRun:(id)sender;
- (IBAction)pickedPause:(id)sender;
- (IBAction)pickedHalt:(id)sender;
- (IBAction)pickedRunControls:(id)sender;
- (IBAction)pickedOutputSettings:(id)sender;
- (IBAction)pickedTracingSettings:(id)sender;
- (IBAction)pickedLoggingSettings:(id)sender;
- (IBAction)pickedBreaksSettings:(id)sender;
- (BOOL) validateMenuItem:(NSMenuItem*)item;
- (void) removeEPICView:(EPICDrawView*)theView;


@end
