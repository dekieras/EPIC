/*
 *  VisualViewObject.h
 *  AppClassesDev
 *
 *  Created by David Kieras on 11/17/06.
 *  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
 *
 */

#ifndef VISUALVIEWOBJECT_H
#define VISUALVIEWOBJECT_H
#include <string>
#include <vector>
#include "EPICLib/Symbol.h"
#include <ApplicationServices/ApplicationServices.h>

class Drawing_shape;
// VisualViewObjects have a location, which is their geometric center,
// and a size, both in VA units with straight-ahead being the (0, 0) origin.
// They have a changable shape, specified by name, but mapped to
// a pointer to a FlyWeight Drawing_shape object.
// Depending on the kind of shape, the size might govern the proportions
// as well as the overall size of the object.
// In addition, they have a color and a label.
// The outline of the object represents whether it is diappearing or not.
// VisualViewObjects can also have a leader - a Radar display property.
// The FlyWeight Drawing_shape accesses properties of the VisualViewObject.

class VisualViewObject {
public:
	VisualViewObject(CGPoint location_, CGSize size_, 
		const Symbol& label_, CGColorRef color_, const Symbol& shape_);
	VisualViewObject(float x, float y, float width, float height, 
		const Symbol& label_, CGColorRef color_, const Symbol& shape_);
	// minimal constructors for new objects with unknown properties
	VisualViewObject(CGPoint location_, CGSize size_);
	VisualViewObject(float x, float y, float width, float height);
		
	void set_location(CGPoint location_)
		{location = location_;}
	void set_location(float x, float y)
		{location.x = x; location.y = y;}
	void set_size(CGSize size_)
		{size = size_;}
	void set_size(float width, float height)
		{size.width = width; size.height = height;}
	void set_leader(const Symbol& leader_name);
	void set_label(const Symbol& label_)
		{label = label_;}
	void set_color(const Symbol& color_);
	void set_shape(const Symbol& shape_);
	void set_disappearing(bool disappearing_)
		{disappearing = disappearing_;}
	void clear_poly_points()
		{poly_points.clear();}
	void add_poly_point(float x_, float y_) {CGPoint p = {x_, y_}; poly_points.push_back(p);}
		
	// accessors for Drawing_shape
	CGPoint get_location() const {return location;}
	CGSize get_size() const {return size;}
	CGColorRef get_color() const {return color;}
	bool get_disappearing() const {return disappearing;}
//	const GU::Polygon& get_shape_poly() const {return shape_poly;}
	// get a rect containing the center origin and size
	CGRect get_center_CGRect() const {CGRect r = {location, size}; return r;}
	// get a rect containing the left-bottom origin and size
	CGRect get_LB_CGRect() const {return CGRectMake(location.x - size.width/2, location.y - size.height/2, size.width, size.height);}
	const std::vector<CGPoint> get_poly_points() const {return poly_points;}
	
	void drawself(CGContextRef ctx, float scale) const;
private:
	CGPoint location;
	CGSize size;
	Symbol label;
	CGColorRef color;
	bool disappearing;
	Drawing_shape * shape_ptr;
	Drawing_shape * leader_ptr;
	typedef std::vector<CGPoint> CGPoint_vector_t;
	CGPoint_vector_t poly_points;
	
	// don't copy or assign
	VisualViewObject(const VisualViewObject&);
	VisualViewObject& operator= (const VisualViewObject&);
};
	



#endif


