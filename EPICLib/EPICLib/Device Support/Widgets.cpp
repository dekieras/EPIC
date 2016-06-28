#include "Widgets.h"
#include "Device_base.h"
#include "Device_exception.h"
#include "Symbol_utilities.h"
#include "Geometry.h"

#include <utility>
#include <sstream>
#include <vector>
#include <iostream>
#include <memory>

using std::make_pair;
using std::string;
using std::ostringstream;
using std::vector;
using std::cout;	using std::endl;
using std::shared_ptr;
using std::dynamic_pointer_cast;

// widgets work in pixel coordinates, but present things in visual angle coordinates
// upper left corner of display is pixel (0, 0)
// the stored location and size are in pixel units

// the following values are inputs from the physical situation - specific values from Marshall task setup
double Widget::screen_hor_pixel_size = 1024;
double Widget::screen_vert_pixel_size = 768;
double Widget::screen_hor_unit_size = 14.22;
double Widget::screen_distance_units = 24.4;	// e.g. inches between eyes and screen
GU::Size Widget::screen_pixels_per_character = GU::Size(5., 7.);	// a reasonable default
// the screen pixel visual orgin is the pixel coordinates for the straight ahead (0, 0) dva origin
// assumes center of screen is at visual origin
GU::Point Widget::screen_pixel_visual_origin = 
	GU::Point(screen_hor_pixel_size / 2., screen_vert_pixel_size / 2.);

// the following values will be recomputed if any of the input values are changed
double Widget::screen_pixels_per_unit = 72;
double Widget::degrees_va_per_pixel = .0321;
double Widget::pixels_per_degrees_va = 31.15;

bool Widget::add_widget_type_property = false;

Widget::Widget(Device_base * device_ptr_, const Symbol& widget_name_, GU::Point location_, GU::Size size_) : 
	device_ptr(device_ptr_), widget_name(widget_name_), location(location_), size(size_), presented(NEVER_PRESENTED)
{
}


Widget::~Widget()
{
}

void Widget::compute_visual_constants()
{
	screen_pixels_per_unit = screen_hor_pixel_size/screen_hor_unit_size;
	degrees_va_per_pixel = GU::degrees_subtended_per_unit(screen_pixels_per_unit, screen_distance_units);
	pixels_per_degrees_va = 1. / degrees_va_per_pixel;	
}

// translate a point using the screen visual origin, then scale to degrees va
GU::Point Widget::pixel_point_to_dva_point(GU::Point p)
{
	double x = (p.x - screen_pixel_visual_origin.x) / pixels_per_degrees_va;
	double y = (screen_pixel_visual_origin.y - p.y) / pixels_per_degrees_va;
	return GU::Point(x, y);
}


// scale a dva point to pixels, and then adjust using the screen visual origin
GU::Point Widget::dva_point_to_pixel_point(GU::Point p)
{
	double x = p.x / degrees_va_per_pixel + screen_pixel_visual_origin.x;
	double y = screen_pixel_visual_origin.y - p.y / degrees_va_per_pixel;
	return GU::Point(x, y);
}

GU::Size Widget::pixel_size_to_dva_size(GU::Size s)
{
	double h = s.h / pixels_per_degrees_va;
	double v = s.v / pixels_per_degrees_va;
	return GU::Size(h, v);
}

GU::Size Widget::string_to_size(const Symbol& s)
{
	double h = 2. * screen_pixels_per_character.h * s.str().length();
	return GU::Size(h, 2. * screen_pixels_per_character.v);
}

// return the relative location for the inner object such that it is aligned inside the top center of the outer
GU::Point Widget::align_inside_top_center(GU::Size inner_sz, GU::Size outer_sz)
{
	double x = (outer_sz.h - inner_sz.h) / 2.;
	return GU::Point(x, 0.);
}

// this only has an effect if the object has been presented
// if the add-type flag is off, and the property is Type, nothing is done
void Widget::present_property(const Symbol& property_name, const Symbol& property_value)
{
	if(presented == PRESENTED) {
		if(!get_add_type() && property_name == Type_c)
			return;
		device_ptr->set_visual_object_property(widget_name, property_name, property_value);
		}
}

void Widget::present()	// this is a template method for getting all of an object on to the screen
{
	present_object();
	present_properties();
}

void Widget::present_object()
{
	Assert(presented != PRESENTED);
	// if it is inside a container, we need to be sure it is visible first
	if(container_ptr && !is_inside_container())
		return;
	// compute dva, visual object coordinates
	// translate from top-left location to center location
	GU::Point center_location(location.x + size.h / 2., location.y + size.v / 2.);
	device_ptr->make_visual_object_appear(widget_name, 
		pixel_point_to_dva_point(center_location), pixel_size_to_dva_size(size));
	presented = PRESENTED;
}

void Widget::present_properties()
{
}

shared_ptr<Container_widget> Widget::get_container() const
{
	return container_ptr;
}

void Widget::set_container(shared_ptr<Container_widget> container_ptr_)
{
	container_ptr = container_ptr_;
}

void Widget::depresent()
{
	if(presented != PRESENTED) {
		ostringstream oss;
		oss << "Attempt to depresent unpresented widget: " << widget_name << endl;
		throw Device_exception(oss.str());
		}

	// catch and discard any exception thrown by this, in case the 
	// simulated human no longer has the object
	try {
	device_ptr->make_visual_object_disappear(widget_name);
	} catch(...) {}
	presented = NOT_PRESENTED;
}

// if the Widget is in a container, and is no longer within the containers area, depresent it automatically!
void Widget::set_location(GU::Point new_location)
{
	location = new_location;
	if(presented == PRESENTED) {
		if(!container_ptr || (container_ptr && is_inside_container())) {
			// translate from top-left location to center location
			GU::Point center_location(location.x + size.h / 2., location.y + size.v / 2);
			device_ptr->set_visual_object_location(widget_name, pixel_point_to_dva_point(center_location));
			}
		else if (container_ptr) {
			depresent();	// no longer visible, so remove it
			}
		}
	else {
		if(container_ptr && is_inside_container()) {
		// if this widget has never been presented before, do nothing to it
		// if not visible before, but is now, so if container is presented, this should be also
			if(presented == NOT_PRESENTED && container_ptr->is_presented())
				present();
			}
			// do nothing if still not visible
		}		
}

// if the Widget is in a container, and is no longer within the containers area, depresent it automatically!
void Widget::set_size(GU::Size new_size)
{
	size = new_size;
	if(presented == PRESENTED) {
		if(!container_ptr || (container_ptr && is_inside_container())) {
			device_ptr->set_visual_object_size(widget_name, size);
			}
		else if (container_ptr) {
			depresent();	// no longer visible, so remove it
			}
		}
	else {
		if(container_ptr && is_inside_container()) {
		// if this widget has never been presented before, do nothing to it
		// if not visible before, but is now, so if container is presented, this should be also
			if(presented == NOT_PRESENTED && container_ptr->is_presented())
				present();
			}
			// do nothing if still not visible
		}		
}


// whole object must be inside of rectangle, since we don't actually clip at pixel level.
bool Widget::is_inside_container() const
{
	Assert(container_ptr);		// this function should be called only if there is a container
	
	double top_edge = location.y;	// origin is y = 0
	double bottom_edge = location.y + size.v; 	// origin is y = 0
	double left_edge = location.x;
	double right_edge = location.x + size.h;
	
	GU::Point c_location = container_ptr->get_location();
	GU::Size c_size = container_ptr->get_size();
	
	double c_top_edge = c_location.y;	// origin is y = 0
	double c_bottom_edge = c_location.y + c_size.v; 	// origin is y = 0
	double c_left_edge = c_location.x;
	double c_right_edge = c_location.x + c_size.h;
	
	return(
		top_edge >= c_top_edge && bottom_edge <= c_bottom_edge &&
		left_edge >= c_left_edge && right_edge <= c_right_edge
		);
}

void Object_widget::set_property(const Symbol& property_name, const Symbol& property_value)
{
	properties[property_name] = property_value;
	if(is_presented())
		present_property(property_name, property_value);
}

void Object_widget::present_object()
{
	Widget::present_object();
}

void Object_widget::present_properties()
{
	if(!is_presented()) {
		ostringstream oss;
		oss << "Attempt to present properties of unpresented Object_widget: " << get_name() << endl;
		throw Device_exception(oss.str());
		}

	for(properties_t::const_iterator it = properties.begin(); it != properties.end(); ++it)
		present_property(it->first, it->second);
}

Polygon_widget::Polygon_widget(Device_base * device_ptr_, const Symbol& widget_name_, 
		const GU::Polygon& polygon, const Symbol& color_) :
	Object_widget(device_ptr_, widget_name_, GU::Point(), polygon.get_size())
{
	// the get_center from a polygon is the center location, but widgets by convention use a top-left origin
	GU::Size size = polygon.get_size();
	GU::Point center = polygon.get_center();
	// set the initial location to the top-left of the polygon
	GU::Point top_left(center.x - size.h / 2., center.y - size.v / 2.);
	set_initial_location(top_left);

	set_property(Color_c, color_);

	// the visual system needs the DVA version
	// scale the polygon, and set this object's shape with it.
	vector<GU::Point> v = polygon.get_vertices();
	for(size_t i = 0; i < v.size(); ++i)
		v[i] = pixel_point_to_dva_point(v[i]);
	
	Symbol shape("Polygon", v);
	set_property(Shape_c, shape);	
}

// A line widget creates a polygon with only two points
Line_widget::Line_widget(Device_base * device_ptr_, const Symbol& widget_name_, 
		const GU::Line_segment& line, const Symbol& color_) :
	Object_widget(device_ptr_, widget_name_, GU::Point(), line.get_size())
{
	// the get_center from a line is the center location, but widgets by convention use a top-left origin
	GU::Size size = line.get_size();
	GU::Point center = line.get_center();
	// set the initial location to the top-left of the polygon
	GU::Point top_left(center.x - size.h / 2., center.y - size.v / 2.);
	set_initial_location(top_left);

	set_property(Color_c, color_);

	// the visual system needs the DVA version
	// scale the line, and set this object's shape with it.		
	vector<GU::Point> v(2);
	v[0] = pixel_point_to_dva_point(line.get_p1());
	v[1] = pixel_point_to_dva_point(line.get_p2());
	
	Symbol shape("Polygon", v);
	set_property(Shape_c, shape);
}

/*void Polygon_widget::present()
{
	Widget::present();
	present_property(Color_c, color);
	// compute a polygon in VA coordinates using current settings
	const vector<GU::Point> & v = polygon.get_vertices();
	GU::Polygon presentation_poly;
	for(size_t i = 0; i < v.size(); ++i)
		presentation_poly.add_vertex(pixel_point_to_dva_point(v[i]);
		
	present_polygon(Polygon_c, presentation_poly);
}
*/

void Cursor_widget::present_object()
{
	Assert(!is_presented());
	// compute dva, visual object coordinates
	// translate from top-left location to center location
	GU::Point center_location(get_location().x + get_size().h / 2., get_location().y + get_size().v / 2);
	get_device_ptr()->make_visual_object_appear(get_name(), 
		pixel_point_to_dva_point(center_location), pixel_size_to_dva_size(get_size()));
	get_device_ptr()->set_visual_object_property(get_name(), Shape_c, Cursor_Arrow_c);
	get_device_ptr()->set_visual_object_property(get_name(), Color_c, Black_c);
	if(get_add_type())
		get_device_ptr()->set_visual_object_property(get_name(), Type_c, Cursor_name_c);
	set_presented(PRESENTED);
}

void Cursor_widget::depresent()
{
	Assert(is_presented());
	get_device_ptr()->make_visual_object_disappear(get_name());
	set_presented(NOT_PRESENTED);
}

void Label_widget::present_object()
{
	Widget::present_object();
	present_property(Color_c, color);
//	present_property(Shape_c, Empty_Rectangle_c);
	present_property(Shape_c, None_c);
	present_property(Text_c, label);
	if(get_add_type())
		present_property(Type_c, Label_c);
}

void Button_widget::present_object()
{
	Object_widget::present_object();
	if(state)
		present_property(Color_c, on_color);
	else
		present_property(Color_c, off_color);
	present_property(Shape_c, Button_c);
	present_property(Text_c, label);
	if(get_add_type())
		present_property(Type_c, Button_c);
}

void Button_widget::set_state(bool state_)
{
	state = state_;
	if(state)
		present_property(Color_c, on_color);
	else
		present_property(Color_c, off_color);
}

void Field_widget::set_string(const Symbol& contents_)
{
	contents = contents_;
	if(is_presented())
		present_property(Text_c, contents);
}

void Field_widget::present_object()
{
	Widget::present_object();
	present_property(Color_c, color);
	present_property(Shape_c, Empty_Rectangle_c);
	present_property(Text_c, contents);
	if(get_add_type())
		present_property(Type_c, Field_c);
}

Labeled_field_widget::Labeled_field_widget(Device_base * device_ptr, const Symbol& widget_name, 
	GU::Point location, GU::Size label_size, GU::Size field_size, 
	const Symbol& label, const Symbol& contents, const Symbol& color) :
		Widget(device_ptr, widget_name, location, 
			GU::Size(label_size.h + field_size.h, 
			((label_size.v > field_size.v) ? label_size.v : field_size.v)
			))
{
	string field_name = widget_name.str();
	string label_name = field_name + "_label";
	string value_name = field_name + "_value";
	label_ptr.reset(new Label_widget(device_ptr, label_name, location,  label_size, label, color));
	field_ptr.reset(new Field_widget(device_ptr, value_name,
		GU::Point(location.x + label_size.h, location.y), field_size, contents, color));
}

Labeled_field_widget::~Labeled_field_widget()
{
	if(is_presented())
		depresent();
}
	
// get the difference between old and new locations, and adjust all of the 
// subobjects by that difference
void Labeled_field_widget::set_location(GU::Point new_location)
{
	GU::Cartesian_vector displacement(get_location(), new_location);
	Widget::set_location(new_location);
	GU::Point new_label_location = label_ptr->get_location() + displacement;
	label_ptr->set_location(new_label_location);
	GU::Point new_field_location = field_ptr->get_location() + displacement;
	field_ptr->set_location(new_field_location);
}

void Labeled_field_widget::present_object()
{
	Widget::present_object();
	present_property(Shape_c, None_c);	// Overall widget has no shape of its own
	label_ptr->present();
	field_ptr->present();
	label_ptr->present_property(Left_of_c, field_ptr->get_name());
	field_ptr->present_property(Right_of_c, label_ptr->get_name());
}

void Labeled_field_widget::depresent()
{
	label_ptr->depresent();
	field_ptr->depresent();
	Widget::depresent();
}

 Container_widget::~Container_widget()
{
	if(is_presented())
		depresent();
}

void Container_widget::add_widget(shared_ptr<Widget> widget_ptr)
{
//	cout << "adding widget " << widget_ptr->get_widget_name() << " to container " << get_widget_name() << endl;
//	if(widget_ptr->get_widget_name() == "trk1")
//		cout << "trk1 is being added" << endl;

	Assert(widgets.find(widget_ptr->get_name()) == widgets.end());
//	widget_ptr->set_container(dynamic_pointer_cast<Container_widget>(shared_from_this()));
	widgets.insert(make_pair(widget_ptr->get_name(), widget_ptr));
	// adjust the widget location from container-relative to absolute
	GU::Point wp = widget_ptr->get_location();
	GU::Point cp = get_location();
	widget_ptr->set_location(GU::Point(cp.x + wp.x, cp.y + wp.y));
}

void Container_widget::remove_widget(shared_ptr<Widget> widget_ptr)
{
	Assert(widgets.find(widget_ptr->get_name()) != widgets.end());
	if(widget_ptr->is_presented())
		widget_ptr->depresent();
	widgets.erase(widget_ptr->get_name());
}

// get the difference between old and new locations, and adjust all of the 
// subobjects by that difference
void Container_widget::set_location(GU::Point new_location)
{
	GU::Cartesian_vector displacement(get_location(), new_location);
	Widget::set_location(new_location);
	for(Widget_Sptrs_t::iterator it = widgets.begin(); it != widgets.end(); ++it) {
		shared_ptr<Widget> widget_ptr = it->second;
		GU::Point new_widget_location = widget_ptr->get_location() + displacement;
		widget_ptr->set_location(new_widget_location);
		}
}


void Container_widget::set_relative_location(shared_ptr<Widget> widget_ptr, GU::Point new_location)
{
	GU::Point cp = get_location();
	widget_ptr->set_location(GU::Point(cp.x + new_location.x, cp.y + new_location.y));
}

// returns a 0 pointer if the named widget is not present
shared_ptr<Widget> Container_widget::get_widget_ptr(const Symbol& widget_name) const
{
	Widget_Sptrs_t::const_iterator it = widgets.find(widget_name);
	if(it == widgets.end())
		return 0;
	return it->second;
}

// present all the widgets and save them under their physical names
void Container_widget::present_object()
{
	Widget::present_object();
	for(Widget_Sptrs_t::iterator it = widgets.begin(); it != widgets.end(); ++it) {
		shared_ptr<Widget> widget_ptr = it->second;
		widget_ptr->present_object();
		widgets.insert(make_pair(widget_ptr->get_name(), widget_ptr));
		}
	for(Widget_Sptrs_t::iterator it = widgets.begin(); it != widgets.end(); ++it) {
		shared_ptr<Widget> widget_ptr = it->second;
		widget_ptr->present_properties();
		}
}

void Container_widget::remove(shared_ptr<Widget> widget_ptr)
{
	widget_ptr->depresent();
	widgets.erase(widget_ptr->get_name());
}

void Container_widget::depresent()
{
	for(Widget_Sptrs_t::iterator it = widgets.begin(); it != widgets.end(); ++it)
		it->second->depresent();
	Widget::depresent();
}

Screen_widget::Screen_widget(Device_base * device_ptr, const Symbol& widget_name, GU::Point location, GU::Size size) : 
	Container_widget(device_ptr, widget_name, location, size)
{
}

// present all the widgets and save them under their physical names
void Screen_widget::present_object()
{
	Container_widget::present_object();
	present_property(Color_c, Black_c);
	present_property(Shape_c, Empty_Rectangle_c);
	if(get_add_type())
		present_property(Type_c, Screen_c);
}

Window_widget::Window_widget(Device_base * device_ptr, const Symbol& widget_name, GU::Point location, GU::Size size, 
		const Symbol& title) : Container_widget(device_ptr, widget_name, location, size)
{
	// create and add a label subwidget as the title
	string title_widget_name_s = widget_name.str() + "_window_title";
	GU::Size title_size = string_to_size(title);
	title_size.v = 2. * title_size.v;
	GU::Point title_loc = align_inside_top_center(title_size, size);
	shared_ptr<Widget> ptr(new Label_widget(device_ptr, Symbol(title_widget_name_s), title_loc, title_size, title));
	add_widget(ptr);
}

// present all the widgets and save them under their physical names
void Window_widget::present_object()
{
	Container_widget::present_object();
	present_property(Color_c, Black_c);
	present_property(Shape_c, Empty_Rectangle_c);
	if(get_add_type())
		present_property(Type_c, Window_c);
}



/*

Dialog_widget::Dialog_widget(Device_base * device_ptr_, const Symbol& widget_name_, GU::Point location_, GU::Size size_, 
		const Symbol& label_) :
		Widget(device_ptr_, widget_name_, location_, size_), label(label_)
		{}
*/

// add a widget, but use its location as relative to the top-left corner of the current widget, and adjust accordingly
