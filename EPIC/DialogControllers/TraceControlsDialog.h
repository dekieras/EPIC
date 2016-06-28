//
//  TraceControlsDialog.h
//  EPIC
//
//  Created by David Kieras on 8/22/13.
//  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
//

#import <Cocoa/Cocoa.h>

@interface TraceControlsDialog : NSWindowController
{
    NSInteger dialogCloseButtonValue;
}
@property (nonatomic, strong) IBOutlet NSButton *checkBoxVisual;
@property (nonatomic, strong) IBOutlet NSButton *checkBoxAuditory;
@property (nonatomic, strong) IBOutlet NSButton *checkBoxCognitive;
@property (nonatomic, strong) IBOutlet NSButton *checkBoxOcular;
@property (nonatomic, strong) IBOutlet NSButton *checkBoxManual;
@property (nonatomic, strong) IBOutlet NSButton *checkBoxVocal;
@property (nonatomic, strong) IBOutlet NSButton *checkBoxTemporal;
@property (nonatomic, strong) IBOutlet NSButton *checkBoxDevice;
- (IBAction)pickedOKButton:(id)sender;
- (IBAction)pickedCancelButton:(id)sender;
- (void)windowWillClose:(NSNotification*)notification;

@end
