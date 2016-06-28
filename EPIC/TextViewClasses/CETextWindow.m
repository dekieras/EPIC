//
//  MTVWindowController.m
//  MyMultiTextView
//
//  Created by David Kieras on 7/29/13.
//  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
//

#import "CETextWindow.h"

@interface CETextWindow ()

@end

@implementation CETextWindow

- (id) init
{
	NSPoint default_window_position = {0, 100};
    return [self initWithWindowTitle:@"Default Text Window Name" position:default_window_position];
}

- (id) initWithWindowTitle:(NSString *)awindowTitle position:(NSPoint) awindowposition
{
//	NSLog(@"in initWithWindowTitle with %@", awindowTitle);
    self = [super initWithWindowNibName:@"CETextWindow"];
    if (self) {
        // Initialization code here.
		windowtitle = awindowTitle;
        windowposition = awindowposition;
    }
//	NSLog(@"in initWithWindowTitle with %@", windowtitle);

    return self;
}

- (void)windowDidLoad
{
    [super windowDidLoad];
	[[self window] setTitle:windowtitle];
	[[self window] setFrameTopLeftPoint:windowposition];
    
    [_textview setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
    [[_textview textContainer] setContainerSize:NSMakeSize(FLT_MAX, FLT_MAX)];
    [[_textview textContainer] setWidthTracksTextView:NO];
    [[_textview enclosingScrollView] setHasHorizontalScroller:YES];
    [_textview setHorizontallyResizable:YES];
}

// we should disable the red close button
- (void)windowWillClose:(NSNotification*)notification
{
   NSLog(@"in windowWillClose of CETextWindow");
	 // the red close button comes here
//	model_ptr->remove_view([_theDrawView getEpicDrawView]);
	// who is responsible for deleting it?
}



- (void) writeInView:(NSString*)str
{
    NSAttributedString* attr = [[NSAttributedString alloc] initWithString: str];
    [[_textview textStorage] beginEditing];  // seems to be recommended
    [[_textview textStorage] appendAttributedString: attr];
    [[_textview textStorage] endEditing]; // seems to be recommended
    [_textview scrollRangeToVisible:NSMakeRange([[_textview string] length], 0)];
}

- (void) clearView
{
    [[_textview textStorage] beginEditing];
    [[_textview textStorage] deleteCharactersInRange:NSMakeRange(0, [[_textview string] length])];
    [[_textview textStorage] endEditing];

}



@end
