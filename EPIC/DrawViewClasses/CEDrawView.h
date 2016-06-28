//
//  CEDrawView.mm
//  Cocoa EPIC App
//
//  Created by David Kieras on 7/29/13.
//  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
//
// This class represents the Cocoa view in which drawing actually happens.
// It is connected to the EPICDrawView that provides the interface to the EPIC View
// and the C++ methods used to draw objects in a graphics context.

#import <Cocoa/Cocoa.h>
class EPICDrawView;

@interface CEDrawView : NSView
- (void) setEpicDrawView:(EPICDrawView*) theEPICDrawView;
- (EPICDrawView*) getEpicDrawView;
@end
