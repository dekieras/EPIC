//
//  MyDrawViewWindowController.m
//  MyMultiDrawView
//
//  Created by David Kieras on 7/29/13.
//  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
//

#import "CEDrawViewWindow.h"
#import "AppGlobals.h"
#import "AppDelegate.h"
#import "EPICDrawView.h"
#include "EPICLib/Model.h"

@interface CEDrawViewWindow ()

@end

@implementation CEDrawViewWindow

- (id) init
{
	return [self initWithWindowTitle:@"Default Window Name"];

}

- (id) initWithWindowTitle:(NSString *)awindowTitle
{
    self = [super initWithWindowNibName:@"CEDrawViewWindow"];
    if (self) {
        // Initialization code here.
		windowtitle = awindowTitle;
    }

    return self;
}

- (void)windowDidLoad
{
    [super windowDidLoad];
	[[self window] setTitle:windowtitle];
}

- (void)windowWillClose:(NSNotification*)notification
{
   NSLog(@"in windowWillClose of CEDrawViewWindow");
	 // the red close button comes here
//	model_ptr->remove_view([_theDrawView getEpicDrawView]);
	// who is responsible for deleting it?
// here here here deleted because error in Xcode 6.1
//	[[NSApp delegate] removeEPICView:[_theDrawView getEpicDrawView]];
	AppDelegate *appdelegate = (AppDelegate*)[NSApp delegate];
	[appdelegate removeEPICView:[_theDrawView getEpicDrawView]];
}


- (void) setTopLeftPoint:(NSPoint)topLeftPoint
{
    [[self window] setFrameTopLeftPoint:topLeftPoint];
}

- (void) setEpicDrawView:(EPICDrawView*) theEPICDrawView
{
    [_theDrawView setEpicDrawView:theEPICDrawView];
}

- (EPICDrawView*) getEpicDrawView
{
    return [_theDrawView getEpicDrawView];
}


@end
