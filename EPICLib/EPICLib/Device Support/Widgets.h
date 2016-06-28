#ifndef WIDGETS_H
#define WIDGETS_H

#include "Symbol.h"
#include "Geometry.h"
#include "Standard_Symbols.h"

#include <map>
#include <string>
#include <memory>

namespace GU = Geometry_Utilities;

class Device_base;
class Container_widget;


// widgets work in pixel coordinates, but present things in visual angle coordinates
// If the containers are used, the Widgets should always referred to with std::shared_ptrs.

class Widget : public std::enable_shared_from_this<Widget> {
public:
	Widget(Device_base * device_ptr_, const Symbol& widget_name_, GU::Point location_, GU::Size size_);
	virtual ~Widget() = 0;
	
	const Symbol& get_name() const
		{return widget_name;}
	GU::Point get_location() const
		{return location;}
	GU::Size get_size() const
		{return size;}
	bool is_presented() const
		{return presented == PRESENTED;}
	std::shared_ptr<Container_widget> get_container() const;
	
	virtual void set_location(GU::Point new_location);
	virtual void set_size(GU::Size new_size);
	bool is_inside_container() const;
	void present_property(const Symbol& property_name_, const Symbol& property_value_);
	void set_container(std::shared_ptr<Container_widget> container_ptr_);
	
	// present() first presents all the objects, and then presents their properties as set by Object_widget's set_property.
	// present_object() can certainly present object properties, but should not present relational properties unless it
	// can guarantee that the objects appear before the relation properties are set.
	// This is to ensure that any relational properties, such as Above, that refer to another object by name,
	// are added after the relevant objects are known.
	void present();				// NVI Template Method calls present_object, then present_properties
	virtual void present_object();		// present the object
	virtual void present_properties();	// present the properties of the object - Object_widget's properties
	virtual void depresent();

	// the screen distance and pixel spacing must be set before any presentations are done
	static void set_screen_distance_units(double distance_)
		{screen_distance_units = distance_; compute_visual_constants();}
	static void set_screen_pixels_per_unit(double pixels_per_unit_)
		{screen_pixels_per_unit = pixels_per_unit_;  compute_visual_constants();}
	// set the center point of the pixel space to a dva location - default value of (0, 0) can be used
	static void set_screen_pixel_visual_origin(GU::Point screen_pixel_visual_origin_)
		{screen_pixel_visual_origin = screen_pixel_visual_origin_; compute_visual_constants();}
	static void set_screen_pixels_per_character(GU::Size screen_pixels_per_character_)
		{screen_pixels_per_character = screen_pixels_per_character_;}
	// the upper-left corner of the screen is always (0, 0) in pixel coordinates,
	// but the origin is the pixel coordinates that corresponds to straight-ahead (0, 0) in dva coordinates	
	static GU::Point get_screen_pixel_visual_origin()
		{return screen_pixel_visual_origin;}
	static GU::Size get_screen_pixel_size()
		{return GU::Size(screen_hor_pixel_size, screen_vert_pixel_size);}
		
	// static members for computing pixels to degrees va
	static double get_screen_distance_units()
		{return screen_distance_units;}
	static double get_screen_pixels_per_unit()
		{return screen_pixels_per_unit;}
	static double get_degrees_va_per_pixel()
		{return degrees_va_per_pixel;}
	static double get_pixels_per_degrees_va()
		{return pixels_per_degrees_va;}
	static GU::Point pixel_point_to_dva_point(GU::Point);
	static GU::Point dva_point_to_pixel_point(GU::Point);
	static GU::Size pixel_size_to_dva_size(GU::Size);
	// compute the size that will hold the string value of the symble
	static GU::Size string_to_size(const Symbol& s);
	
	static void set_add_widget_type_property(bool add_if_true)
		{add_widget_type_property = add_if_true;}
	static bool get_add_type() 
		{return add_widget_type_property;}
	
protected:
	// should be called only during construction, when values require complex computation
	void set_initial_location(GU::Point location_)
		{location = location_;}
	void set_initial_size(GU::Size size_)
		{size = size_;}
//	void set_name(const Symbol& name_)
//		{widget_name = name_;}
	Device_base * get_device_ptr() const
		{return device_ptr;}
	GU::Point align_inside_top_center(GU::Size inner, GU::Size outer_sz);
	
	typedef enum {NEVER_PRESENTED, NOT_PRESENTED, PRESENTED} Presented_t;
	void set_presented(Presented_t presented_)
		{presented = presented_;}
	
private:
	Device_base * device_ptr;
	Symbol widget_name;
	GU::Point location;
	GU::Size size;
	Presented_t presented;
	std::shared_ptr<Container_widget> container_ptr;	// pointer to a container widget, if any
	
	static bool add_widget_type_property;			// if true, the widget type property is included

	// widgets work in pixel coordinates, but present things in visual angle coordinates
	// upper left corner of display is pixel (0, 0)

	// the following values are inputs from the physical situation
	static double screen_hor_pixel_size;
	static double screen_vert_pixel_size;
	static double screen_hor_unit_size;
	static double screen_distance_units;	// e.g. inches between eyes and screen
	static GU::Size screen_pixels_per_character;
	// the screen pixel visual orgin is the pixel coordinates for the straight ahead (0, 0) dva origin
	// assumes center of screen is at visual origin
	static GU::Point screen_pixel_visual_origin;

	// the following values will be recomputed if any of the input values are changed
	static double screen_pixels_per_unit;	// e.g. pixels per inch
	static double degrees_va_per_pixel;
	static double pixels_per_degrees_va;
	
	static void compute_visual_constants();
};


struct Widget_Sptr_widget_name_less {
	bool operator() (const std::shared_ptr<Widget> wp1, const std::shared_ptr<Widget> wp2) const
		{return wp1->get_name() < wp2->get_name();}
};

// an Object_Widget can move, has color, shape, etc, and other properties which are stored with the object
// presenting the object presents all its current properties; changing a property changes it both in
// storage and on-screen, if present.
class Object_widget : public Widget {
public:
	Object_widget(Device_base * device_ptr_, const Symbol& widget_name_, GU::Point location_, GU::Size size_) : 
		Widget(device_ptr_, widget_name_, location_, size_)
		{}
	void set_property(const Symbol& property_name_, const Symbol& property_value_);
	virtual void present_object();
	virtual void present_properties();
private:
	typedef std::map<Symbol, Symbol> properties_t;
	properties_t properties;

};

// A Polygon Widget sets the shape to correspond to the supplied polygon - at present, should not be moved or resized, or reshaped
class Polygon_widget: public Object_widget {
public:
	Polygon_widget(Device_base * device_ptr_, const Symbol& widget_name_, const GU::Polygon& polygon_, const Symbol& color_ = Black_c);
};

// A Line Widget sets the shape to correspond to the supplied Line_segment - at present, should not be moved or resized, or reshaped
class Line_widget: public Object_widget {
public:
	Line_widget(Device_base * device_ptr_, const Symbol& widget_name_, const GU::Line_segment& line, const Symbol& color_ = Black_c);
};


// Cursor_widget has is own unique and constant physical and local name and properties
class Cursor_widget : public Widget {
public:
	Cursor_widget(Device_base * device_ptr_, GU::Point location_, GU::Size size_) : 
		Widget(device_ptr_, Cursor_name_c, location_, size_)
		{}
	virtual void present_object();
	virtual void depresent();
};

// a Label widget is an object with text on it, normally non-interactive and static
class Label_widget : public Widget {
public:
	Label_widget(Device_base * device_ptr_, const Symbol& widget_name_, GU::Point location_, GU::Size size_, 
		const Symbol& label_, const Symbol& color_ = Black_c) : 
		Widget(device_ptr_, widget_name_, location_, size_), label(label_), color(color_)
		{}
	Label_widget(Device_base * device_ptr_, const Symbol& widget_name_, GU::Point location_,  
		const Symbol& label_, const Symbol& color_ = Black_c) : 
		Widget(device_ptr_, widget_name_, location_, Widget::string_to_size(label_)), label(label_), color(color_)
		{}
	virtual void present_object();
private:
	Symbol label;
	Symbol color;
};


class Button_widget : public Object_widget {
public:
	Button_widget(Device_base * device_ptr_, const Symbol& widget_name_, GU::Point location_, GU::Size size_, 
		const Symbol& label_, const Symbol& on_color_, const Symbol& off_color_, bool state_) :
		Object_widget(device_ptr_, widget_name_, location_, size_), label(label_), 
		on_color(on_color_), off_color(off_color_), state(state_)
		{}
	Button_widget(Device_base * device_ptr_, const Symbol& widget_name_, GU::Point location_, GU::Size size_, 
		const Symbol& label_, const Symbol& off_color_) :
		Object_widget(device_ptr_, widget_name_, location_, size_), label(label_), 
		on_color(Black_c), off_color(off_color_), state(false)
		{}
		
	bool get_state() const
		{return state;}
	void set_state(bool state_);
	void toggle_state();
		
	virtual void present_object();
		
private:
	Symbol label;
	Symbol on_color;
	Symbol off_color;
	bool state;	// true if button is "on" - showing the on_color
};


// presents a field whose contents are a string
class Field_widget : public Widget {
public:
	Field_widget(Device_base * device_ptr_, const Symbol& widget_name_, GU::Point location_, GU::Size size_, 
		const Symbol& contents_ = Empty_string_c, const Symbol& color_ = Black_c) :
		Widget(device_ptr_, widget_name_, location_, size_), contents(contents_), color(color_)
		{}

	void set_string(const Symbol& contents_);
	Symbol get_string() const
		{return contents;}

	virtual void present_object();
			
private:
	Symbol contents;
	Symbol color;
};

// creates two subwidgets, a label on the left, and a field on the right, adjacent
class Labeled_field_widget: public Widget {
public:
	Labeled_field_widget(Device_base * device_ptr_, const Symbol& widget_name_, GU::Point location_, GU::Size label_size_, 
		GU::Size field_size_, const Symbol& label_, const Symbol& contents_ = Empty_string_c, const Symbol& color_ = Black_c);
	~Labeled_field_widget();
	
	void set_string(const Symbol& contents)
		{field_ptr->set_string(contents);}
	Symbol get_string() const
		{return field_ptr->get_string();}

	virtual void set_location(GU::Point new_location);

	virtual void present_object();
	virtual void depresent();

private:
	std::shared_ptr<Label_widget> label_ptr;
	std::shared_ptr<Field_widget> field_ptr;
};

// a Container_widget contains pointers to other widgets, and presents and removes the whole set
// It first presents all the objects, and then presents their properties.
// This is to ensure that any relational properties, such as Above, that refer to another object by name,
// are added after the relevant objects are known.
class Container_widget : public Widget {
public:
	Container_widget(Device_base * device_ptr_, const Symbol& widget_name_, GU::Point location_, GU::Size size_) :
		Widget(device_ptr_, widget_name_, location_, size_)
		{}
		
	~Container_widget() = 0;	// an abstract class
		
	virtual void set_location(GU::Point new_location);
	// the widget's pixel coordinates are assumed to be relative to this window
	// and so are adjusted when added to be the absolute pixel coordinates
	void add_widget(std::shared_ptr<Widget>);
	// remove this widget from the display and the container
	void remove_widget(std::shared_ptr<Widget>);
	// new location is relative to this window, and so is adjusted
	void set_relative_location(std::shared_ptr<Widget>, GU::Point);
	
	std::shared_ptr<Widget> get_widget_ptr(const Symbol& widget_name) const;
	
	virtual void remove(std::shared_ptr<Widget> widget_ptr);

	virtual void present_object();
	virtual void depresent();

private:
	typedef std::map<Symbol, std::shared_ptr<Widget> > Widget_Sptrs_t;
	Widget_Sptrs_t widgets;	// container of widgets keyed by name
};

// a concrete class for the whole screen
class Screen_widget : public Container_widget {
public:
	Screen_widget(Device_base * device_ptr_, const Symbol& widget_name, GU::Point location, GU::Size size);
	virtual void present_object();
};

// a concrete class for a window - derive from this for dialogs or specific window types
class Window_widget : public Container_widget {
public:
	Window_widget(Device_base * device_ptr_, const Symbol& widget_name, GU::Point location, GU::Size size, 
		const Symbol& title);
	virtual void present_object();		
};


/*

// a Dialog widget contains pointers to other widgets
// when added, their location is relative to the Dialog origin
class Dialog_widget : public Widget {
public:
	Dialog_widget(Device_base * device_ptr_, const Symbol& widget_name_, GU::Point location_, GU::Size size_, 
		const Symbol& label_) :
		Widget(device_ptr_, widget_name_, location_, size_), label(label_)
		{}
private:

	
};

*/
// add a widget, but use its location as relative to the top-left corner of the current widget, and adjust accordingly

#endif
