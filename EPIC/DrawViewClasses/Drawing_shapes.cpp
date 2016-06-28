/*
 *  Drawing_shapes.cpp
 *  AppClassesDev
 *
 *  Created by David Kieras on 11/19/06.
 *  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
 *
 */

#include "Drawing_shapes.h"
#include "EPICLib/Geometry.h"
#include <cmath>

namespace {

inline CGRect center_size_to_CGRect(float x, float y, float h, float v)
{
	return CGRectMake(x - h/2, y - v/2, h, v);
}

inline CGRect center_size_to_CGRect(CGPoint center, CGSize size)
{
	CGRect r = {{center.x - size.width/2, center.y - size.height/2}, size};
	return r;
}


// trig degrees and radians start as 0 at right, 90 or pi/2 at top, 
// 180 or pi at left, 270 or 3pi/2 at bottom, 360 or 2pi at right again
inline float to_radians(float degrees)
{
    return degrees * GU_pi / 180.;
}

void draw_rounded_rectangle(CGContextRef ctx, CGRect rect, float corner_size);
void draw_cross(CGContextRef ctx, CGRect rect, float thickness);
void draw_arrow(CGContextRef ctx, CGPoint center, float size, float angle);
void draw_clover(CGContextRef ctx, CGRect rect);
void draw_top_clover(CGContextRef ctx, CGRect rect);
void draw_bot_clover(CGContextRef ctx, CGRect rect);
void draw_heart(CGContextRef ctx, CGRect rect);
void draw_T_object(CGContextRef ctx, CGPoint center, float size, float angle);
void draw_L_object(CGContextRef ctx, CGPoint center, float size, float angle);

} // end unnamed namespace


/* -------------- Line Drawing Shapes ----------- */
void Drawing_shape_line::setup(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
	CGContextSaveGState(ctx);
	// if disappearing, make the colors transparent
	if(obj_ptr->get_disappearing())
		CGContextSetAlpha(ctx, 0.3);
	else
		CGContextSetAlpha(ctx, 1.0);
	
	CGContextSetStrokeColorWithColor(ctx, obj_ptr->get_color());
	CGContextSetLineWidth(ctx, 2./20.);
}

void Drawing_shape_line::finish(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
	CGContextStrokePath(ctx);
	CGContextRestoreGState(ctx);	// restore the global alpha
}

// A line is specified by the location of the start point, with the end point being given by the size
// The end point x is the start point x + the width; y is the start point y + the height.
void Line_shape::drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
	CGPoint start = obj_ptr->get_location();
	CGContextMoveToPoint(ctx, start.x, start.y);
	CGContextAddLineToPoint(ctx, start.x + obj_ptr->get_size().width, start.y + obj_ptr->get_size().height);
}

// a fixed leader is a line from the location of the object in one of the four specified directions,
// with a length equal to twice the greater of the object width or height.
void Leader_north::drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
	CGPoint start = obj_ptr->get_location();
	CGSize sz = obj_ptr->get_size();
	float len = 2. * ((sz.width > sz.height) ? sz.width : sz.height);
	CGContextMoveToPoint(ctx, start.x, start.y);
	CGContextAddLineToPoint(ctx, start.x, start.y + len);
}

void Leader_south::drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
	CGPoint start = obj_ptr->get_location();
	CGSize sz = obj_ptr->get_size();
	float len = 2. * ((sz.width > sz.height) ? sz.width : sz.height);
	CGContextMoveToPoint(ctx, start.x, start.y);
	CGContextAddLineToPoint(ctx, start.x, start.y - len);
}

void Leader_east::drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
	CGPoint start = obj_ptr->get_location();
	CGSize sz = obj_ptr->get_size();
	float len = 2. * ((sz.width > sz.height) ? sz.width : sz.height);
	CGContextMoveToPoint(ctx, start.x, start.y);
	CGContextAddLineToPoint(ctx, start.x + len, start.y);
}

void Leader_west::drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
	CGPoint start = obj_ptr->get_location();
	CGSize sz = obj_ptr->get_size();
	float len = 2. * ((sz.width > sz.height) ? sz.width : sz.height);
	CGContextMoveToPoint(ctx, start.x, start.y);
	CGContextAddLineToPoint(ctx, start.x - len, start.y);
}


void Cross_hairs_shape::drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
	CGRect rect = center_size_to_CGRect(obj_ptr->get_location(), obj_ptr->get_size());
//	draw_cross(ctx, rect, 2.); // doesn't work!
	// start at top center
	CGContextMoveToPoint(ctx, CGRectGetMidX(rect), CGRectGetMaxY(rect));
	// down to bottom center
	CGContextAddLineToPoint(ctx, CGRectGetMidX(rect), CGRectGetMinY(rect));
	// then start at middle left
	CGContextMoveToPoint(ctx, CGRectGetMinX(rect), CGRectGetMidY(rect));
	// go to middle right
	CGContextAddLineToPoint(ctx, CGRectGetMaxX(rect), CGRectGetMidY(rect));

}

T_object_shape::T_object_shape(float angle_) : angle(to_radians(angle_)) {}

// a T shape rotated 0 looks like a regular T; 180 should be rotate left, 270 should be rotate right
void T_object_shape::drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
/*	CGRect rect = center_size_to_CGRect(obj_ptr->get_location(), obj_ptr->get_size());
	// start at top left
	CGContextMoveToPoint(ctx, CGRectGetMinX(rect), CGRectGetMaxY(rect));
	// over to top right
	CGContextAddLineToPoint(ctx, CGRectGetMaxX(rect), CGRectGetMaxY(rect));
	// back to top center
	CGContextMoveToPoint(ctx, CGRectGetMidX(rect), CGRectGetMaxY(rect));
	// down to bottom center
	CGContextAddLineToPoint(ctx, CGRectGetMidX(rect), CGRectGetMinY(rect));
*/
	CGRect rect = {obj_ptr->get_location(), obj_ptr->get_size()};
	float height = CGRectGetHeight(rect);
	float width = CGRectGetWidth(rect);
	float size = (height < width) ? height : width;
	draw_T_object(ctx, rect.origin, size, angle);
}

L_object_shape::L_object_shape(float angle_) : angle(to_radians(angle_)) {}

void L_object_shape::drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
/*	CGRect rect = center_size_to_CGRect(obj_ptr->get_location(), obj_ptr->get_size());
	// start at top left
	CGContextMoveToPoint(ctx, CGRectGetMinX(rect), CGRectGetMaxY(rect));
	// down to bottom left
	CGContextAddLineToPoint(ctx, CGRectGetMinX(rect), CGRectGetMinY(rect));
	// over to bottom right
	CGContextAddLineToPoint(ctx, CGRectGetMaxX(rect), CGRectGetMinY(rect));
*/
	CGRect rect = {obj_ptr->get_location(), obj_ptr->get_size()};
	float height = CGRectGetHeight(rect);
	float width = CGRectGetWidth(rect);
	float size = (height < width) ? height : width;
	draw_L_object(ctx, rect.origin, size, angle);
}

void Block_X_shape::drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
	CGRect rect = center_size_to_CGRect(obj_ptr->get_location(), obj_ptr->get_size());
    CGContextBeginPath(ctx);
	CGContextMoveToPoint(ctx, CGRectGetMinX(rect), CGRectGetMaxY(rect));	// move to top left
	CGContextAddLineToPoint(ctx, CGRectGetMaxX(rect), CGRectGetMinY(rect));	// go to bottom right
	CGContextMoveToPoint(ctx, CGRectGetMaxX(rect), CGRectGetMaxY(rect));	// move go to top right
	CGContextAddLineToPoint(ctx, CGRectGetMinX(rect), CGRectGetMinY(rect));	// go to bottom left
}

void Block_Y_shape::drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
	CGRect rect = center_size_to_CGRect(obj_ptr->get_location(), obj_ptr->get_size());
    CGContextBeginPath(ctx);
	CGContextMoveToPoint(ctx, CGRectGetMinX(rect), CGRectGetMaxY(rect));	// move to top left
	CGContextAddLineToPoint(ctx, CGRectGetMidX(rect), CGRectGetMidY(rect));	// go to center
	CGContextMoveToPoint(ctx, CGRectGetMaxX(rect), CGRectGetMaxY(rect));	// move go to top right
	CGContextAddLineToPoint(ctx, CGRectGetMidX(rect), CGRectGetMidY(rect));	// go to center
	CGContextAddLineToPoint(ctx, CGRectGetMidX(rect), CGRectGetMinY(rect));	// go to bottom center
}



/* -------------- Empty/Filled Drawing Shapes ----------- */

void Drawing_shape_fillable::setup(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
	CGContextSaveGState(ctx);
	
	// if disappearing, make the colors transparent
	if(obj_ptr->get_disappearing())
		CGContextSetAlpha(ctx, 0.3);
	else
		CGContextSetAlpha(ctx, 1.0);
	// the stroke and fill colors are the same as the object color
	CGContextSetStrokeColorWithColor(ctx, obj_ptr->get_color());
	// set the fill color
	if(get_filled()) {
		CGContextSetLineWidth(ctx, 2./20.);
		CGContextSetFillColorWithColor(ctx, obj_ptr->get_color());
		}
	else {
		CGContextSetLineWidth(ctx, 2./20.);
		}
}

void Drawing_shape_fillable::finish(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
	if(get_filled())
		CGContextDrawPath(ctx, kCGPathFillStroke);
	else
		CGContextStrokePath(ctx);
	
	CGContextRestoreGState(ctx);	// restore the global alpha
}

void Rectangle_shape::drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
	CGRect rect = center_size_to_CGRect(obj_ptr->get_location(), obj_ptr->get_size());
	CGContextAddRect(ctx, rect);
}

void Circle_shape::drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
	CGRect rect = center_size_to_CGRect(obj_ptr->get_location(), obj_ptr->get_size());
	CGContextAddEllipseInRect(ctx, rect);
}

void Top_Semicircle_shape::drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
	CGRect rect = center_size_to_CGRect(obj_ptr->get_location(), obj_ptr->get_size());
    float radius = CGRectGetWidth(rect) / 2.;
    CGContextBeginPath(ctx);
	CGContextMoveToPoint(ctx, CGRectGetMaxX(rect), CGRectGetMinY(rect));
	CGContextAddArc(ctx, CGRectGetMidX(rect), CGRectGetMinY(rect),  radius, 0, GU_pi, 0);
	CGContextClosePath(ctx);
}

void Triangle_shape::drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
	CGRect rect = center_size_to_CGRect(obj_ptr->get_location(), obj_ptr->get_size());
    CGContextBeginPath(ctx);
	CGContextMoveToPoint(ctx, rect.origin.x, rect.origin.y);				// goto bottom left
	CGContextAddLineToPoint(ctx, CGRectGetMidX(rect), CGRectGetMaxY(rect));	// go to top center
	CGContextAddLineToPoint(ctx, CGRectGetMaxX(rect), CGRectGetMinY(rect));	// go to bottom right
	CGContextClosePath(ctx);
}

void Diamond_shape::drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
	CGRect rect = center_size_to_CGRect(obj_ptr->get_location(), obj_ptr->get_size());
    CGContextBeginPath(ctx);
	CGContextMoveToPoint(ctx, CGRectGetMinX(rect), CGRectGetMidY(rect));	// goto mid left
	CGContextAddLineToPoint(ctx, CGRectGetMidX(rect), CGRectGetMaxY(rect));	// go to top center
	CGContextAddLineToPoint(ctx, CGRectGetMaxX(rect), CGRectGetMidY(rect));	// go to mid right
	CGContextAddLineToPoint(ctx, CGRectGetMidX(rect), CGRectGetMinY(rect));	// go to bottom center
	CGContextClosePath(ctx);
}

void House_shape::drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
	CGRect rect = center_size_to_CGRect(obj_ptr->get_location(), obj_ptr->get_size());
    CGContextBeginPath(ctx);
	CGContextMoveToPoint(ctx, CGRectGetMinX(rect), CGRectGetMinY(rect));	// goto left bottom
	CGContextAddLineToPoint(ctx, CGRectGetMinX(rect), CGRectGetMidY(rect));	// go to left mid
	CGContextAddLineToPoint(ctx, CGRectGetMidX(rect), CGRectGetMaxY(rect));	// go to top center
	CGContextAddLineToPoint(ctx, CGRectGetMaxX(rect), CGRectGetMidY(rect));	// go to mid right
	CGContextAddLineToPoint(ctx, CGRectGetMaxX(rect), CGRectGetMinY(rect));	// go to bottom right
	CGContextClosePath(ctx);
}

void Inv_House_shape::drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
	CGRect rect = center_size_to_CGRect(obj_ptr->get_location(), obj_ptr->get_size());
    CGContextBeginPath(ctx);
	CGContextMoveToPoint(ctx, CGRectGetMinX(rect), CGRectGetMaxY(rect));	// goto left top
	CGContextAddLineToPoint(ctx, CGRectGetMinX(rect), CGRectGetMidY(rect));	// go to left mid
	CGContextAddLineToPoint(ctx, CGRectGetMidX(rect), CGRectGetMinY(rect));	// go to bottom center
	CGContextAddLineToPoint(ctx, CGRectGetMaxX(rect), CGRectGetMidY(rect));	// go to mid right
	CGContextAddLineToPoint(ctx, CGRectGetMaxX(rect), CGRectGetMaxY(rect));	// go to top right
	CGContextClosePath(ctx);

}

void Button_shape::drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
	CGRect rect = center_size_to_CGRect(obj_ptr->get_location(), obj_ptr->get_size());
	float height = CGRectGetHeight(rect);
	float width = CGRectGetWidth(rect);
	float corner_size = (height <= width) ? height/2. : width/2;
	draw_rounded_rectangle(ctx, rect, corner_size);
}

void Cross_shape::drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
	CGRect rect = center_size_to_CGRect(obj_ptr->get_location(), obj_ptr->get_size());
	float height = CGRectGetHeight(rect);
	float width = CGRectGetWidth(rect);
	float thickness = (height < width) ? height / 3. : width / 3.;
	draw_cross(ctx, rect, thickness);
}

void Bar_shape::drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
	CGRect rect = center_size_to_CGRect(obj_ptr->get_location(), obj_ptr->get_size());
	float height = rect.size.height / 3.;
    float t = height/2.;
	CGContextBeginPath(ctx);
	// start at top left-hand corner 
	CGContextMoveToPoint(ctx, CGRectGetMinX(rect), CGRectGetMidY(rect) + t);
	// go right
	CGContextAddLineToPoint(ctx, CGRectGetMaxX(rect), CGRectGetMidY(rect) + t);
	// go down
	CGContextAddLineToPoint(ctx, CGRectGetMaxX(rect), CGRectGetMidY(rect) - t);
	// go left
	CGContextAddLineToPoint(ctx, CGRectGetMinX(rect), CGRectGetMidY(rect) - t);
	// finish
	CGContextClosePath(ctx);
}

void Clover_shape::drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
	CGRect rect = center_size_to_CGRect(obj_ptr->get_location(), obj_ptr->get_size());
	draw_clover(ctx, rect);
}

void Clover3_shape::drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
	CGRect rect = center_size_to_CGRect(obj_ptr->get_location(), obj_ptr->get_size());
	draw_top_clover(ctx, rect);
}

void Inv_Clover3_shape::drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
	CGRect rect = center_size_to_CGRect(obj_ptr->get_location(), obj_ptr->get_size());
	draw_bot_clover(ctx, rect);
}

void Heart_shape::drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
	CGRect rect = center_size_to_CGRect(obj_ptr->get_location(), obj_ptr->get_size());
	draw_heart(ctx, rect);
}

void Hill_shape::drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
	CGRect rect = center_size_to_CGRect(obj_ptr->get_location(), obj_ptr->get_size());
	float cpx = CGRectGetMidX(rect);
	float cpy = CGRectGetMaxY(rect) + CGRectGetHeight(rect);
    CGContextBeginPath(ctx);
	CGContextMoveToPoint(ctx, CGRectGetMaxX(rect), CGRectGetMinY(rect));
	CGContextAddQuadCurveToPoint(ctx, cpx, cpy, CGRectGetMinX(rect), CGRectGetMinY(rect));
	CGContextClosePath(ctx);
}

void Inv_Hill_shape::drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
	CGRect rect = center_size_to_CGRect(obj_ptr->get_location(), obj_ptr->get_size());
	float cpx = CGRectGetMidX(rect);
	float cpy = CGRectGetMinY(rect) - CGRectGetHeight(rect);
    CGContextBeginPath(ctx);
	CGContextMoveToPoint(ctx, CGRectGetMaxX(rect), CGRectGetMaxY(rect));
	CGContextAddQuadCurveToPoint(ctx, cpx, cpy, CGRectGetMinX(rect), CGRectGetMaxY(rect));
	CGContextClosePath(ctx);
}

Polygon_shape::Polygon_shape(bool filled_) : Drawing_shape_fillable(filled_) {}

// draw the polygon by drawing a path connecting each point in the vector of points.
void Polygon_shape::drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
	using std::vector;
	const vector<CGPoint>& points = obj_ptr->get_poly_points();
	vector<CGPoint>::const_iterator it = points.begin();
	if(it == points.end())
		return;
    CGContextBeginPath(ctx);
	CGContextMoveToPoint(ctx, it->x, it->y);	// goto first point
	for(++it; it != points.end(); ++it)
		CGContextAddLineToPoint(ctx, it->x, it->y);	// next point
	CGContextClosePath(ctx);
}

Arrow_shape::Arrow_shape(bool filled_, float angle_) : Drawing_shape_fillable(filled_), angle(to_radians(angle_)) {}

void Arrow_shape::drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const
{
//	CGRect rect = center_size_to_CGRect(obj_ptr->get_location(), obj_ptr->get_size());
	CGRect rect = {obj_ptr->get_location(), obj_ptr->get_size()};
	float height = CGRectGetHeight(rect);
	float width = CGRectGetWidth(rect);
	float size = (height < width) ? height : width;
	draw_arrow(ctx, rect.origin, size, angle);
}

// internal-linkage functions
namespace {

void draw_rounded_rectangle(CGContextRef ctx, CGRect rect, float corner_size)
{
    CGContextSaveGState(ctx);
    CGContextBeginPath(ctx);
    CGContextTranslateCTM(ctx, CGRectGetMinX(rect), CGRectGetMinY(rect));
    CGContextScaleCTM(ctx, corner_size, corner_size);
    float fw = CGRectGetWidth(rect) / corner_size;
    float fh = CGRectGetHeight(rect) / corner_size;

    CGContextMoveToPoint(ctx, fw, fh/2);
    CGContextAddArcToPoint(ctx, fw, fh, fw/2, fh, 1);
    CGContextAddArcToPoint(ctx, 0, fh, 0, fh/2, 1);
    CGContextAddArcToPoint(ctx, 0, 0, fw/2, 0, 1);
    CGContextAddArcToPoint(ctx, fw, 0, fw, fh/2, 1);
    CGContextClosePath(ctx);
    CGContextRestoreGState(ctx);
}

void draw_cross(CGContextRef ctx, CGRect rect, float thickness)
{
    float t = thickness/2.;
	CGContextBeginPath(ctx);
	// start at top right-hand corner of top arm
	CGContextMoveToPoint(ctx, CGRectGetMidX(rect) + t, CGRectGetMaxY(rect));
	// down to the center
	CGContextAddLineToPoint(ctx, CGRectGetMidX(rect) + t, CGRectGetMidY(rect) + t);
	// out to the top of the right arm
	CGContextAddLineToPoint(ctx, CGRectGetMaxX(rect), CGRectGetMidY(rect) + t);
	// and down
	CGContextAddLineToPoint(ctx, CGRectGetMaxX(rect), CGRectGetMidY(rect) - t);
	// left to the center
	CGContextAddLineToPoint(ctx, CGRectGetMidX(rect) + t, CGRectGetMidY(rect) - t);
	// down to right of the bottom arm
	CGContextAddLineToPoint(ctx, CGRectGetMidX(rect) + t, CGRectGetMinY(rect));
	// and left
	CGContextAddLineToPoint(ctx, CGRectGetMidX(rect) - t, CGRectGetMinY(rect));
	// up to the center
	CGContextAddLineToPoint(ctx, CGRectGetMidX(rect) - t, CGRectGetMidY(rect) - t);
	// left to the bottom of the left arm
	CGContextAddLineToPoint(ctx, CGRectGetMinX(rect), CGRectGetMidY(rect) - t);
	// and up
	CGContextAddLineToPoint(ctx, CGRectGetMinX(rect), CGRectGetMidY(rect) + t);
	// right to the center
	CGContextAddLineToPoint(ctx, CGRectGetMidX(rect) - t, CGRectGetMidY(rect) + t);
	// up to the left of the top arm
	CGContextAddLineToPoint(ctx, CGRectGetMidX(rect) - t, CGRectGetMaxY(rect));
	// finish
	CGContextClosePath(ctx);
}

void draw_arrow(CGContextRef ctx, CGPoint center, float size, float angle)
{
	CGContextSaveGState(ctx);

	CGRect rect = center_size_to_CGRect(0, 0, .5, 1);	// proportions of our arrow
	float t = CGRectGetWidth(rect)/8;
	angle -= GU_pi / 2.;
		
    CGContextTranslateCTM(ctx, center.x, center.y);	// move to new origin
    CGContextScaleCTM(ctx, size, size);			// magnify by size
	CGContextRotateCTM(ctx, angle);
	CGContextBeginPath(ctx);
	CGContextMoveToPoint(ctx, CGRectGetMidX(rect), CGRectGetMaxY(rect));
	CGContextAddLineToPoint(ctx, CGRectGetMidX(rect), CGRectGetMaxY(rect));
	CGContextAddLineToPoint(ctx, CGRectGetMaxX(rect), CGRectGetMidY(rect));
	CGContextAddLineToPoint(ctx, CGRectGetMidX(rect) + t, CGRectGetMidY(rect));
	CGContextAddLineToPoint(ctx, CGRectGetMidX(rect) + t, CGRectGetMinY(rect));
	CGContextAddLineToPoint(ctx, CGRectGetMidX(rect) - t, CGRectGetMinY(rect));
	CGContextAddLineToPoint(ctx, CGRectGetMidX(rect) - t, CGRectGetMidY(rect));
	CGContextAddLineToPoint(ctx, CGRectGetMinX(rect), CGRectGetMidY(rect));
	CGContextClosePath(ctx);
	CGContextRestoreGState(ctx);
}

void draw_clover(CGContextRef ctx, CGRect rect)
{
    CGContextSaveGState(ctx);
    CGContextTranslateCTM(ctx, CGRectGetMidX(rect), CGRectGetMidY(rect));
	CGContextScaleCTM(ctx, CGRectGetWidth(rect) / 4, CGRectGetHeight(rect) / 4);
	float radius = 1.;

    CGContextBeginPath(ctx);
	CGContextMoveToPoint(ctx, radius, -radius);
	CGContextAddArc(ctx,  radius,      0,  radius, 6*GU_pi/4, 2*GU_pi/4, 0);
	CGContextAddArc(ctx,      0,  radius,  radius, 0*GU_pi/4, 4*GU_pi/4, 0);
	CGContextAddArc(ctx, -radius,      0,  radius, 2*GU_pi/4, 6*GU_pi/4, 0);
	CGContextAddArc(ctx,      0, -radius,  radius, 4*GU_pi/4, 0*GU_pi/4, 0);
	CGContextClosePath(ctx);
	CGContextRestoreGState(ctx);
}

void draw_top_clover(CGContextRef ctx, CGRect rect)
{
    CGContextSaveGState(ctx);
    CGContextTranslateCTM(ctx, CGRectGetMidX(rect), CGRectGetMidY(rect));
	CGContextScaleCTM(ctx, CGRectGetWidth(rect) / 4, CGRectGetHeight(rect) / 4);
	float radius = 1.;

    CGContextBeginPath(ctx);
	CGContextMoveToPoint(ctx, radius, -radius);
	CGContextAddArc(ctx,  radius,      0,  radius, 6*GU_pi/4, 2*GU_pi/4, 0);
	CGContextAddArc(ctx,      0,  radius,  radius, 0*GU_pi/4, 4*GU_pi/4, 0);
	CGContextAddArc(ctx, -radius,      0,  radius, 2*GU_pi/4, 6*GU_pi/4, 0);
	CGContextClosePath(ctx);
	CGContextRestoreGState(ctx);
}

void draw_bot_clover(CGContextRef ctx, CGRect rect)
{
    CGContextSaveGState(ctx);
    CGContextTranslateCTM(ctx, CGRectGetMidX(rect), CGRectGetMidY(rect));
	CGContextScaleCTM(ctx, CGRectGetWidth(rect) / 4, CGRectGetHeight(rect) / 4);
	float radius = 1.;

    CGContextBeginPath(ctx);
	CGContextMoveToPoint(ctx, radius, -radius);
	CGContextAddArc(ctx,  radius,      0,  radius, 6*GU_pi/4, 2*GU_pi/4, 0);
	CGContextMoveToPoint(ctx, radius, radius);
	CGContextAddLineToPoint(ctx, -radius, radius);
//	CGContextAddArc(ctx,      0,  radius,  radius, 0*GU_pi/4, 4*GU_pi/4, 0);
	CGContextAddArc(ctx, -radius,      0,  radius, 2*GU_pi/4, 6*GU_pi/4, 0);
	CGContextAddArc(ctx,      0, -radius,  radius, 4*GU_pi/4, 0*GU_pi/4, 0);
	CGContextClosePath(ctx);
	CGContextRestoreGState(ctx);
}

void draw_heart(CGContextRef ctx, CGRect rect)
{
	// The points and control points were found in an unexplained example at:
	// http://flylib.com/books/en/3.310.1.19/1/
	CGContextSaveGState(ctx);
    CGContextTranslateCTM(ctx, CGRectGetMidX(rect), CGRectGetMidY(rect));
	// these scaling factors were determined empirically
	CGContextScaleCTM(ctx, CGRectGetWidth(rect) / 110, CGRectGetHeight(rect) / 110);

    CGContextBeginPath(ctx);
	CGContextMoveToPoint(ctx, 0, 21); 
	CGContextAddCurveToPoint(ctx, -16, 49.8, -53.2, 41.0, -49.6, 5.8); 
	CGContextAddCurveToPoint(ctx, -46, -29.4, -9.4, -53.4, 0, -69.8); 
	CGContextAddCurveToPoint(ctx, 9.4, -53.4, 46, -29.4, 49.6, 5.8); 
	CGContextAddCurveToPoint(ctx, 53.2, 41, 16, 49.8, 0, 21); 
	CGContextClosePath(ctx);
	CGContextRestoreGState(ctx);
}

void draw_T_object(CGContextRef ctx, CGPoint center, float size, float angle)
{
	CGContextSaveGState(ctx);

	CGRect rect = center_size_to_CGRect(0, 0, 1, 1);	// proportions of our object
//	angle -= GU_pi / 2.;
		
    CGContextTranslateCTM(ctx, center.x, center.y);	// move to new origin
    CGContextScaleCTM(ctx, size, size);			// magnify by size
	CGContextRotateCTM(ctx, angle);
//	CGContextBeginPath(ctx);
	// go to top left - at 0 angle
	CGContextMoveToPoint(ctx, CGRectGetMinX(rect), CGRectGetMaxY(rect));
	// go to top right
	CGContextAddLineToPoint(ctx, CGRectGetMaxX(rect), CGRectGetMaxY(rect));
	// go to  top middle
	CGContextMoveToPoint(ctx, CGRectGetMidX(rect), CGRectGetMaxY(rect));
	// down to bottom middle
	CGContextAddLineToPoint(ctx, CGRectGetMidX(rect), CGRectGetMinY(rect));
//	CGContextClosePath(ctx);
	CGContextRestoreGState(ctx);
}

void draw_L_object(CGContextRef ctx, CGPoint center, float size, float angle)
{
	CGContextSaveGState(ctx);

	CGRect rect = center_size_to_CGRect(0, 0, 1, 1);	// proportions of our object
//	angle -= GU_pi / 2.;
		
    CGContextTranslateCTM(ctx, center.x, center.y);	// move to new origin
    CGContextScaleCTM(ctx, size, size);			// magnify by size
	CGContextRotateCTM(ctx, angle);
//	CGContextBeginPath(ctx);
	// go to top left - at 0 angle
	CGContextMoveToPoint(ctx, CGRectGetMinX(rect), CGRectGetMaxY(rect));
	// go to bottom left
	CGContextAddLineToPoint(ctx, CGRectGetMinX(rect), CGRectGetMinY(rect));
	// down to bottom right
	CGContextAddLineToPoint(ctx, CGRectGetMaxX(rect), CGRectGetMinY(rect));
//	CGContextClosePath(ctx);
	CGContextRestoreGState(ctx);
}

} // end unnamed namespace

