/*
 *  Drawing_shape.h
 *  AppClassesDev
 *
 *  Created by David Kieras on 11/19/06.
 *  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
 *
 */

#ifndef DRAWING_SHAPE_H
#define DRAWING_SHAPE_H
#include <ApplicationServices/ApplicationServices.h>

class VisualViewObject;

// a Flyweight-pattern abstract base class 
// with non-virtual interface
class Drawing_shape {
public:
	void draw(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
			{
			setup(ctx, obj_ptr);
			drawself(ctx, obj_ptr);
			finish(ctx, obj_ptr);
			}
	virtual ~Drawing_shape() {}
protected:
	virtual void setup(CGContextRef ctx, const VisualViewObject * const obj_ptr) const = 0;			// set initial stroke and fill colors
	virtual void drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const = 0;		// construct the path
	virtual void finish(CGContextRef ctx, const VisualViewObject * const obj_ptr) const = 0;		// stroke the path with optional fill
};


#endif

