//
//  MyDrawViewWindowController.h
//  MyMultiDrawView
//
//  Created by David Kieras on 7/29/13.
//  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
//

#import <Cocoa/Cocoa.h>
#import "CEDrawView.h"
class EPICDrawView;

@interface CEDrawViewWindow : NSWindowController
{
	NSString* windowtitle;
}
- (id) init;
- (id) initWithWindowTitle:(NSString *)awindowTitle;
- (void) setEpicDrawView:(EPICDrawView*) theEPICDrawView;
- (EPICDrawView*) getEpicDrawView;
- (void) setTopLeftPoint:(NSPoint)topLeftPoint;

@property (nonatomic, strong) IBOutlet CEDrawView *theDrawView;


@end
