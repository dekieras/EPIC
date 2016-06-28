/*
 *  Drawing_shapes.h
 *  AppClassesDev
 *
 *  Created by David Kieras on 11/19/06.
 *  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
 *
 */

#ifndef DRAWING_SHAPES_H
#define DRAWING_SHAPES_H
#include "Drawing_shape.h"
#include "VisualViewObject.h"

// This shapes consist only of line drawings - they have no interior
class Drawing_shape_line : public Drawing_shape {
protected:
	virtual void setup(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;			// set initial stroke colors
	virtual void finish(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;		// stroke the path
};

// These shapes have interiors, and may or may not be filled - a read-only member determines this
class Drawing_shape_fillable : public Drawing_shape {
public:
	Drawing_shape_fillable(bool filled_) : filled(filled_) {}
protected:
	virtual void setup(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;			// set initial stroke colors
	virtual void finish(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;		// stroke the path
	bool get_filled() const {return filled;}
private:
	bool filled;

};


/* -------------- Line Drawing Shapes ----------- */
class No_Shape : public Drawing_shape_line {
public:
	// this shape does nothing in any of its functions
	virtual void setup(CGContextRef, const VisualViewObject * const) const {}			// set initial stroke and fill colors
	virtual void drawself(CGContextRef, const VisualViewObject * const) const {}		// construct the path
	virtual void finish(CGContextRef, const VisualViewObject * const) const {}			// stroke the path with optional fill
};

class Line_shape : public Drawing_shape_line {
public:
	virtual void drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;
};

// a fixed leader is a line from the location of the object in one of the four specified directions,
// with a length equal to the greater of the object width or height.
class Leader_north : public Drawing_shape_line {
public:
	virtual void drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;
};

class Leader_south : public Drawing_shape_line {
public:
	virtual void drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;
};

class Leader_east : public Drawing_shape_line {
public:
	virtual void drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;
};

class Leader_west : public Drawing_shape_line {
public:
	virtual void drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;
};

class Cross_hairs_shape : public Drawing_shape_line {
public:
	virtual void drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;
};

// Special shapes for targets if visual search tasks - rotated Ts and Ls and their segments

// For the following classes, angle is in degrees, go clockwise through quadrants from 0 == straight up
class T_object_shape : public Drawing_shape_line {
public:
	T_object_shape(float angle_);
	virtual void drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;
private:
	float angle;
};

class L_object_shape : public Drawing_shape_line {
public:
	L_object_shape(float angle_);
	virtual void drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;
private:
	float angle;
};

// Special shapes for Q's task - letters, but must be used as a graphic rather than text
class Block_X_shape : public Drawing_shape_line {
public:
	virtual void drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;
};
class Block_Y_shape : public Drawing_shape_line {
public:
	virtual void drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;
};



/* -------------- Empty/Filled Drawing Shapes ----------- */
class Rectangle_shape : public Drawing_shape_fillable {
public:
	Rectangle_shape(bool filled_) : Drawing_shape_fillable(filled_) {}
	virtual void drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;
};

class Circle_shape : public Drawing_shape_fillable {
public:
	Circle_shape(bool filled_) : Drawing_shape_fillable(filled_) {}
	virtual void drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;
};

class Top_Semicircle_shape : public Drawing_shape_fillable {
public:
	Top_Semicircle_shape(bool filled_) : Drawing_shape_fillable(filled_) {}
	virtual void drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;
};

class Triangle_shape : public Drawing_shape_fillable {
public:
	Triangle_shape(bool filled_) : Drawing_shape_fillable(filled_) {}
	virtual void drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;
};

class Diamond_shape : public Drawing_shape_fillable {
public:
	Diamond_shape(bool filled_) : Drawing_shape_fillable(filled_) {}
	virtual void drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;
};

class House_shape : public Drawing_shape_fillable {
public:
	House_shape(bool filled_) : Drawing_shape_fillable(filled_) {}
	virtual void drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;
};

class Inv_House_shape : public Drawing_shape_fillable {
public:
	Inv_House_shape(bool filled_) : Drawing_shape_fillable(filled_) {}
	virtual void drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;
};

class Button_shape : public Drawing_shape_fillable {
public:
	Button_shape(bool filled_) : Drawing_shape_fillable(filled_) {}
	virtual void drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;
};

class Cross_shape : public Drawing_shape_fillable {
public:
	Cross_shape(bool filled_) : Drawing_shape_fillable(filled_) {}
	virtual void drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;
};

class Bar_shape : public Drawing_shape_fillable {
public:
	Bar_shape(bool filled_) : Drawing_shape_fillable(filled_) {}
	virtual void drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;
};

class Clover_shape : public Drawing_shape_fillable {
public:
	Clover_shape(bool filled_) : Drawing_shape_fillable(filled_) {}
	virtual void drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;
};

class Clover3_shape : public Drawing_shape_fillable {
public:
	Clover3_shape(bool filled_) : Drawing_shape_fillable(filled_) {}
	virtual void drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;
};

class Inv_Clover3_shape : public Drawing_shape_fillable {
public:
	Inv_Clover3_shape(bool filled_) : Drawing_shape_fillable(filled_) {}
	virtual void drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;
};

class Heart_shape : public Drawing_shape_fillable {
public:
	Heart_shape(bool filled_) : Drawing_shape_fillable(filled_) {}
	virtual void drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;
};

class Hill_shape : public Drawing_shape_fillable {
public:
	Hill_shape(bool filled_) : Drawing_shape_fillable(filled_) {}
	virtual void drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;
};

class Inv_Hill_shape : public Drawing_shape_fillable {
public:
	Inv_Hill_shape(bool filled_) : Drawing_shape_fillable(filled_) {}
	virtual void drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;
};

class Polygon_shape : public Drawing_shape_fillable {
public:
	Polygon_shape(bool filled_);
	virtual void drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;
private:
	float angle;
};

// For the following classes, angle is in degrees, go clockwise through quadrants from 0 == straight up
class Arrow_shape : public Drawing_shape_fillable {
public:
	Arrow_shape(bool filled_, float angle_);
	virtual void drawself(CGContextRef ctx, const VisualViewObject * const obj_ptr) const;
private:
	float angle;
};


#endif
