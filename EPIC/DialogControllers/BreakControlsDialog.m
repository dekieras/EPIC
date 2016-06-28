//
//  DialogWindowController.m
//  TestSingleXibCheckBoxListDialog
//
//  Created by David Kieras on 8/2/13.
//  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
//

#import "BreakControlsDialog.h"
#import "StringWithState.h"

@interface BreakControlsDialog ()

@end

@implementation BreakControlsDialog


- (id) init
{
    NSMutableArray* emptyarray;
    return
        [self initWithStringsWithState:emptyarray enabled:NO];
}

- (id) initWithStringsWithState:(NSMutableArray*)rules_ enabled:(BOOL)enabled_
{
    self = [super initWithWindowNibName:@"BreakControlsDialog"];
    if (self) {
//        NSLog(@"init with %@", self);
		theCheckBoxState = enabled_;
        _stringsWithState = rules_;
//        NSLog(@"number of strings with state is %ld", [_stringsWithState count]);
		}
    
    return self;
}

- (BOOL) getCheckBoxState
{
    return theCheckBoxState;
}

- (void)windowDidLoad
{
    [super windowDidLoad];
   [[self window] center];
//	[self setupTableView];
    [_enableCheckBox setState:theCheckBoxState];
}


- (void)windowWillClose:(NSNotification*)notification
{
    // the red close button comes here
//    NSLog(@"windowWillClose");
    [NSApp stopModalWithCode:dialogCloseButtonValue];
    dialogCloseButtonValue = NSModalResponseCancel;
}


- (IBAction)pressOKButton:(id)sender {
    theCheckBoxState = [_enableCheckBox state];
    dialogCloseButtonValue = NSModalResponseOK;
    [[self window] close];
}

- (IBAction)pressCancelButton:(id)sender {
    dialogCloseButtonValue = NSModalResponseCancel;
    [[self window] close];
}

- (void)setupTableView
{
 // 1. Create the View Controller
//    self.checkBoxListViewController = [[CheckBoxListViewController alloc] initWithNibName:@"CheckBoxListViewController" bundle:nil];

 
// 3. Add the view controller to the Window's content view
//    [self.window.contentView addSubview:self.checkBoxListViewController.view];
//   [self.window.contentView addSubview:self.checkBoxListViewController.view  positioned:NSWindowAbove relativeTo:(NSView *)_theOKButton];

 //   self.checkBoxListViewController.view.frame = ((NSView*)self.window.contentView).bounds;



}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView {
//    NSLog(@"in numberofRowsInTableView with %lu", (unsigned long)[self.strings count]);
    return [self.stringsWithState count];
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
 
 //   NSLog(@"in tableView objectValueforTableColumn with %@, %ld", tableColumn.identifier, row);
    
    NSButtonCell *buttonCell = [tableColumn dataCell];
    if( [tableColumn.identifier isEqualToString:@"StringColumn"] ) {

        NSString* theString = [_stringsWithState[row] theString];
        BOOL theState = [_stringsWithState[row] theState];
    
        [buttonCell setButtonType:NSSwitchButton];
        [buttonCell setTitle:theString];
        [buttonCell setState:(theState ? NSOnState : NSOffState)];
    
        return buttonCell;
        }
    
    return buttonCell;
}

- (void)tableView:(NSTableView *)aTableView setObjectValue:(id)anObject forTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex
{
    BOOL newState = [anObject boolValue];
    [_stringsWithState[rowIndex] setStringState:newState];
 //   NSLog(@"state for string %ld is now %d", rowIndex, newState);
 //    [_model setBreakState:[anObject boolValue] forRule:ruleArray[rowIndex]];
}

@end
