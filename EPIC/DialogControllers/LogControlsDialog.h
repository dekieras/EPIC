//
//  LogControlsDialog.h
//  EPIC
//
//  Created by David Kieras on 8/23/13.
//  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
//

#import <Cocoa/Cocoa.h>

@interface LogControlsDialog : NSWindowController
{
    NSInteger dialogCloseButtonValue;
}
@property (nonatomic, strong) IBOutlet NSButton *checkBoxNormalOutput;
@property (nonatomic, strong) IBOutlet NSButton *checkBoxTraceOutput;
@property (nonatomic, strong) IBOutlet NSButton *openFileButton;
- (IBAction)pickedFileButton:(id)sender;
- (IBAction)pickedOKButton:(id)sender;
- (IBAction)pickedCancelButton:(id)sender;

@end
