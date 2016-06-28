//
//  StringWithStateg.h
//
//  Created by David Kieras on 8/1/13.
//  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
//
// A simple class containing a boolean and an NSString

#import <Foundation/Foundation.h>

@interface StringWithState : NSObject
@property (nonatomic, strong) NSString *theString;
@property (nonatomic, assign) BOOL theState;
- (id)initWithString:(NSString*)aString state:(BOOL)aState;
- (void)setStringState:(BOOL)aState;

@end
