//
//  DialogWindowController.h
//  TestSingleXibCheckBoxListDialog
//
//  Created by David Kieras on 8/2/13.
//  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
//

#import <Cocoa/Cocoa.h>

@interface BreakControlsDialog : NSWindowController <NSTableViewDelegate>
{
    NSInteger dialogCloseButtonValue;
	BOOL theCheckBoxState;
}
@property (nonatomic, strong) NSMutableArray* stringsWithState;
- (id) initWithStringsWithState:(NSMutableArray*)rules_ enabled:(BOOL)enabled_;
- (BOOL) getCheckBoxState;
- (void)windowDidLoad;
- (void)windowWillClose:(NSNotification*)notification;
- (void)setupTableView;
@property (nonatomic, strong) IBOutlet NSButton *enableCheckBox;
- (IBAction)pressOKButton:(id)sender;
- (IBAction)pressCancelButton:(id)sender;

@end
