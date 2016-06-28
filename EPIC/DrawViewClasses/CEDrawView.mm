//
//  CEDrawView.mm
//  Cocoa EPIC App
//
//  Created by David Kieras on 7/29/13.
//  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
//

#import "CEDrawView.h"
#import "EPICDrawView.h"
//#import "EPIClib/Geometry.h"

@implementation CEDrawView
{
	EPICDrawView* epic_draw_view_ptr;
	NSBezierPath *path;
    NSBezierPath *path2;
}

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code here.
		epic_draw_view_ptr = nullptr;
//		NSLog(@"my value of PI is %20.18lf \n M_PI is %20.18lf\n", GU_pi, M_PI);
        NSRect theRect = {20, 20, 60, 80};
        path = [NSBezierPath bezierPathWithRect:theRect];
        [path setLineWidth: 2.0];
    }
    
    return self;
}

- (void) setEpicDrawView:(EPICDrawView*) theEPICDrawView
{
	epic_draw_view_ptr = theEPICDrawView;
}
- (EPICDrawView*) getEpicDrawView
{
    return epic_draw_view_ptr;
}


- (void)drawRect:(NSRect)dirtyRect
{
//    NSLog(@"in drawRect with %f, %f, %f, %f",
//        dirtyRect.origin.x, dirtyRect.origin.y, dirtyRect.size.width, dirtyRect.size.height);

/*    // Drawing code here.
    NSRect bounds= [self bounds];
    [[NSColor greenColor] set];
    [NSBezierPath fillRect:bounds];

    [[NSColor redColor] set];
    [path stroke];
    [[NSColor blueColor] set];
    [path2 stroke];
	
	[[NSColor whiteColor] setFill];
    NSRectFill(dirtyRect);
    
    [[NSColor blackColor] setFill];
 */   
    CGContextRef ctx = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextSetTextMatrix(ctx, CGAffineTransformMake(1.0,0.0, 0.0, 1.0, 0.0, 0.0));

	if(epic_draw_view_ptr)
		epic_draw_view_ptr->draw_content(ctx);

	
	/* from Yunfeng's
@implementation DrawingViewWindow

- (void)drawRect:(NSRect)dirtyRect
{
    [[NSColor whiteColor] setFill];
    NSRectFill(dirtyRect);
    
    [[NSColor blackColor] setFill];
    
    CGContextRef ctx = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextSetTextMatrix(ctx, CGAffineTransformMake(1.0,0.0, 0.0, 1.0, 0.0, 0.0));

    [_view drawContent:ctx];
}

- (void)setDrawingViewWrapper:(DrawingViewWrapper *)vw
{
    _view = vw;
    [self setNeedsDisplay:YES];
}

@end
	
	
	*/
}


@end
