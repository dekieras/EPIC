/*
 *  VisualView.h
 *
 *  Created by David Kieras on 12/20/06.
 *  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
 *
 */

#ifndef VISUALVIEW_H
#define VISUALVIEW_H
#include "EPICDrawView.h"
#include "EPICLib/View_base.h"
#include "EPICLib/Symbol.h"

#include <map>
#include <string>

//#include <objc/objc-runtime.h>

#include <ApplicationServices/ApplicationServices.h>

class VisualViewObject;

class VisualView : public EPICDrawView {
public:
	VisualView(CEDrawView* theDrawView_);

	virtual ~VisualView();

	// called from base class to redraw the window
	void draw_content(CGContextRef ctx) override;

	// accessors to control the display metrics
	void set_origin(float x_, float y_);
	void set_scale(float scale_) override;
	// set calibration grid display state
	void set_grid_on(bool grid_on_) override {grid_on = grid_on_;}
	
	// notifications from Model
	void clear() override;
	void notify_eye_movement(GU::Point new_eye_location) override;
    void notify_object_appear(const Symbol& object_name, GU::Point location, GU::Size size) override;
    void notify_object_disappear(const Symbol&) override;
    void notify_object_reappear(const Symbol&) override;
    void notify_erase_object(const Symbol&) override;
    void notify_visual_location_changed(const Symbol& object_name, GU::Point location) override;
    void notify_visual_size_changed(const Symbol& object_name, GU::Size size) override;
    void notify_visual_property_changed(const Symbol&, const Symbol&, const Symbol&) override;
	void notify_time(long current_time_) override;

private:
	// metric state
	CGPoint origin;	// location of lower-left in terms of object coordinates
	float scale;
	float old_scale;
	bool grid_on;	// if true, display calibration grid
	CGSize old_view_size;
	CGSize scaled_view_size;
	std::string view_info;
	long current_time;
	// pointers to always-present objects
	VisualViewObject * fov_obj_ptr;
	VisualViewObject * cal_obj_ptr;
	typedef std::map<Symbol, VisualViewObject *> objects_t;
	objects_t objects;

	
	void initialize();
	void update_info();
};

#endif
