//
//  StringWithState.m
//
//  Created by David Kieras on 8/1/13.
//  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
//

#import "StringWithState.h"

@implementation StringWithState
- (id)initWithString:(NSString*)aString state:(BOOL)aState
{
    if ((self = [super init])) {
        self.theString = aString;
        self.theState = aState;
        }
    return self;
}

- (void)setStringState:(BOOL)aState
{
    self.theState = aState;
}


@end
