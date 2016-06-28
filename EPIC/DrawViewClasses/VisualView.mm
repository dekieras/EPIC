/*
 *  VisualView.cpp
 *
 *  Created by David Kieras on 12/20/06.
 *  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
 *
 */

#include "VisualView.h"
#include "VisualViewObject.h"
#include "AppColorUtilities.h"
#include "Color_map.h"
#include "EPICLib/Standard_Symbols.h"
#include "EPICLib/Utility_templates.h"
#include <iostream>
#include <iomanip>


#include <string>
#include <sstream>
#include <functional>
#include <algorithm>

using std::string;
using std::ostringstream;	using std::fixed;	using std::setprecision;
using std::list;
using std::vector;
using std::make_pair;
using std::for_each;
using std::mem_fun;	using std::bind2nd;
using std::cout;	using std::endl;

VisualView::VisualView(CEDrawView* theDrawView_) : EPICDrawView(theDrawView_),
	origin(CGPointZero), scale(1.0), old_scale(0), grid_on(false), current_time(0)
{
	old_view_size = CGSizeZero;
	set_origin(0., 0.);
	set_scale(10);

// create the constant objects
	CGPoint fov_loc = {0., 0.};
	CGSize fov_sz = {1., 1.};
	fov_obj_ptr = new VisualViewObject(fov_loc, fov_sz, Nil_c, gColor_Gray, Filled_Circle_c);
	fov_obj_ptr->set_disappearing(true);
	CGPoint cal_loc = {0., 0.};
	CGSize cal_sz = {10., 10.};
	cal_obj_ptr = new VisualViewObject(cal_loc, cal_sz, Nil_c, gColor_Gray, Empty_Circle_c);
	cal_obj_ptr->set_disappearing(true);
}

VisualView::~VisualView()
{
	// empty the list of objects
	for_each(objects.begin(), objects.end(), Delete_second());
//	for(objects_t::iterator it = objects.begin(); it != objects.end(); ++it)
//		delete it->second;
	objects.clear();
	delete fov_obj_ptr;
	delete cal_obj_ptr;
}

void VisualView::initialize()
{
	fov_obj_ptr->set_location(0., 0.);
	cal_obj_ptr->set_location(0., 0.);
	// empty the list of objects
	for(objects_t::iterator it = objects.begin(); it != objects.end(); ++it)
		delete it->second;
	objects.clear();
}

void VisualView::set_origin(float x_, float y_)
{
	origin.x = x_; 
	origin.y = y_;
	update_info();
}

void VisualView::set_scale(float scale_)
{
	scale = scale_;
	update_info();
}

void VisualView::update_info()
{
	ostringstream oss;
	oss << scaled_view_size.width << " X " << scaled_view_size.height 
		<< " DVA, (" << origin.x << ", " << origin.y << "), " << scale << " p/DVA";
	view_info = oss.str();
	set_changed();
}
	
void VisualView::draw_content(CGContextRef ctx)
{	
	CGSize view_size = get_view_size(); // in untransformed coordinates
	if(!CGSizeEqualToSize(view_size, old_view_size) || scale != old_scale) {
		old_scale = scale;
		old_view_size = view_size;
		scaled_view_size.height = view_size.height / scale;
		scaled_view_size.width = view_size.width / scale;
		update_info();
		}
	// set our origin to put the (0., 0) point in the view center
	CGContextTranslateCTM(ctx, view_size.width / 2., view_size.height / 2.);
	// change the scale for drawing
	CGContextScaleCTM(ctx, scale, scale);
	// change so that the origin point is now centered
	CGContextTranslateCTM(ctx, -origin.x,  -origin.y);

	// move so that the window center is at the origin point as scaled
//	CGContextTranslateCTM(ctx, (view_size.width / 2.) - origin.x * scale, (view_size.height / 2.) - origin.y * scale);
	
	// draw some calibration lines
	
	const int yrange = int(scaled_view_size.height / 2.);
	const int xrange = int(scaled_view_size.width / 2.);
	
	if(grid_on) {
		CGContextSaveGState(ctx);
		CGContextSetStrokeColorWithColor(ctx, gColor_LightBlue);
		CGContextSetLineWidth(ctx, 1./scale);
		const int div = 1.;
		for(float x = -xrange; x <= xrange; x += div)
			for(float y = -yrange; y <= yrange; y += div) {
				CGContextMoveToPoint(   ctx, x, -yrange);
				CGContextAddLineToPoint(ctx, x,  yrange);
				CGContextStrokePath(ctx);
				CGContextMoveToPoint(   ctx, -xrange, y);
				CGContextAddLineToPoint(ctx,  xrange, y);
				CGContextStrokePath(ctx);
				}
		CGRect origcircRect = CGRectMake(-.5, -.5, 1., 1.);
		CGContextSetLineWidth(ctx, 2./scale);
		CGContextStrokeEllipseInRect(ctx, origcircRect);
		CGContextRestoreGState(ctx);
		}

	// write the current origin and scale information
	CGContextSelectFont(ctx, "Monaco", 9./scale, kCGEncodingMacRoman);
	CGContextShowTextAtPoint(ctx, -xrange, -yrange, view_info.c_str(), view_info.size());
	// prepare and write the time information
	ostringstream oss;
	oss << fixed << setprecision(2) << double(current_time) / 1000.;
	CGContextShowTextAtPoint(ctx, -xrange, yrange - (9./scale), oss.str().c_str(), oss.str().size());
	
	
	for(objects_t::iterator it = objects.begin(); it != objects.end(); ++it)
		(it->second)->drawself(ctx, scale);
	
	fov_obj_ptr->drawself(ctx, scale);
	cal_obj_ptr->drawself(ctx, scale);
}


// overrides of View_base
void VisualView::clear()
{
	initialize();
	set_changed();
}

void VisualView::notify_eye_movement(GU::Point new_eye_location)
{
	fov_obj_ptr->set_location(new_eye_location.x, new_eye_location.y);
	cal_obj_ptr->set_location(new_eye_location.x, new_eye_location.y);
	set_changed();
}

// this function does nothing of the named object already exists.
// The architecture elsewhere will signal an error.
void VisualView::notify_object_appear(const Symbol& object_name, GU::Point location, GU::Size size)
{			
	objects.insert(make_pair(object_name, new VisualViewObject(location.x, location.y, size.h, size.v)));
//	objects[object_name] = new VisualViewObject(location.x, location.y, size.h, size.v);
	set_changed();
}

// In this class, these functions do nothing if the referred to object does not exist.
// The architecture elsewhere will signal an error.

void VisualView::notify_object_disappear(const Symbol& object_name)
{
	objects_t::iterator it = objects.find(object_name);
	if(it != objects.end()) {
		it->second->set_disappearing(true);
//		objects[object_name]->set_disappearing(true);		
		set_changed();
		}
}

void VisualView::notify_object_reappear(const Symbol& object_name)
{
	objects_t::iterator it = objects.find(object_name);
	if(it != objects.end()) {
		objects[object_name]->set_disappearing(false);		
		it->second->set_disappearing(false);		
		set_changed();
		}
}

void VisualView::notify_erase_object(const Symbol& object_name)
{
//	delete objects[object_name];
//	objects.erase(object_name);
//	set_changed();
	objects_t::iterator it = objects.find(object_name);
	if(it != objects.end()) {
		delete it->second;
		objects.erase(it);
		set_changed();
		}
}

void VisualView::notify_visual_location_changed(const Symbol& object_name, GU::Point location)
{
	objects_t::iterator it = objects.find(object_name);
	if(it != objects.end()) {
		it->second->set_location(location.x, location.y);
		set_changed();
		}
//	VisualViewObject * ptr = objects[object_name];
//	assert(ptr);
//	if(ptr)
//		ptr->set_location(location.x, location.y);
//	else {
//		const char * x = object_name.c_str();
//		assert(x);
//		}
//	objects[object_name]->set_location(location.x, location.y);
//	set_changed();
}

void VisualView::notify_visual_size_changed(const Symbol& object_name, GU::Size size)
{	
	objects_t::iterator it = objects.find(object_name);
	if(it != objects.end()) {
		it->second->set_size(size.h, size.v);;
		set_changed();
		}
//	objects[object_name]->set_size(size.h, size.v);
//	set_changed();
}


void VisualView::notify_visual_property_changed(const Symbol& object_name, const Symbol& prop_name, const Symbol& prop_value)
{
	objects_t::iterator it = objects.find(object_name);
	if(it == objects.end())
		return;
	VisualViewObject * ptr = it->second;
	if(prop_name == Color_c) {
		ptr->set_color(prop_value);
		}
	else if (prop_name == Text_c) {
		ptr->set_label(prop_value);
		}
	else if (prop_name == Leader_c) {
		ptr->set_leader(prop_value);
		}
	else if (prop_name == Shape_c) {
//		cout << object_name << ' ' << prop_name << ' ' << prop_value << endl;
		ptr->set_shape(prop_value);
		// if a polygon, extract and save the polygon points
//		cout << object_name << ' ' << prop_name << ' ' << prop_value << endl;
		if(prop_value.c_str() == Polygon_c.c_str()) {
//			cout << "supposed to be Polygon " << object_name << ' ' << prop_name << ' ' << prop_value << endl;
			ptr->clear_poly_points();
			typedef vector<GU::Point> GUPoint_vector_t;
			const GUPoint_vector_t& gu_points = prop_value.get_Point_vector();
			// translate and copy the values
			for(GUPoint_vector_t::const_iterator it = gu_points.begin(); it != gu_points.end(); ++it) {
				ptr->add_poly_point(it->x, it->y);
				}
			}
		}
	set_changed();
}

void VisualView::notify_time(long current_time_)
{
	current_time = current_time_;
	set_changed();
}
