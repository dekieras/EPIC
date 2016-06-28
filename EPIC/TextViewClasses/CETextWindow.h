//
//  MTVWindowController.h
//  MyMultiTextView
//
//  Created by David Kieras on 7/29/13.
//  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
//

#import <Cocoa/Cocoa.h>

@interface CETextWindow : NSWindowController
{
	NSString* windowtitle;
    NSPoint windowposition;
}
- (id) init;
- (id) initWithWindowTitle:(NSString *)awindowTitle position:(NSPoint) awindowposition;
- (void) writeInView:(NSString*)str;
- (void) clearView;
@property (nonatomic, strong) IBOutlet NSTextView *textview;

@end
