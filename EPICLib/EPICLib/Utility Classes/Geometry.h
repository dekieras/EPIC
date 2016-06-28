#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "Point.h"
#include "Assert_throw.h"

#include <iosfwd>
#include <vector>
#include <cmath>


// calculate a value for pi - this has internal linkage
const double GU_pi = 2. * std::atan2(1., 0.);

namespace Geometry_Utilities {
using std::operator<<;	// bring in global declarations
struct Point;
struct Size;
struct Cartesian_vector;
struct Polar_vector;

// Output operators
std::ostream& operator<< (std::ostream& os, const Point& p);
std::ostream& operator<< (std::ostream& os, const Size& s);
std::ostream& operator<< (std::ostream& os, const Cartesian_vector& cv);
std::ostream& operator<< (std::ostream& os, const Polar_vector& pv);

/*
This set of simple classes is used to compute positions and directions in the plane, 
in both cartesian and polar coordinates, and also with respect to line segments
and polygon regions.

Most classes are defined with "struct" to make all members public by default.

These classes make no assumptions about units of measurement of distance. Angles
can be specified in radians, but radians can be converted to and from 
trigonometry degrees in which 0 degrees corresponds to (x > 0, y = 0), 
and 90 degrees corresponds to (x = 0, y > 0).

Point is a set of (x, y) coordinates. 

A Cartesian_vector is (delta_x, delta_y) - a displacement in Cartesian coordinates.

A Polar_vector is (r, theta) - a displacement in polar coordinates using radians.

Various overloaded operators support computations of positions and directions.

A Line_segment represents a line that passes through two Points in 
both parametric and general form. Functions are provided (mostly inline for speed)
for computing distances of a Point from the line, and intersections between lines.
There are two definitions supported for distance of a point from the line:
	1. The distance from the line extended infinitely past the endpoints of the segment.
	2. The distance from the line segment only, which is either:
		a. the length of the line that both passes through the point and is perpendicular
		to the line segment and intersects the line segment between the endpoints. 
		b. the distance from the closest endpoint if the intersecting perpendicular does
		not intersect between the endpoints.

The Polygon class represents a polygon as a series of Points for vertices that make up
the endpoints of a set of line segments. The last Point wraps around to the first point
to define the last line segment making up the polygon. A distance_inside function
is used to compute whether a Point is inside the polygon and the distance of the Point
from the nearest line segment.

*/

// calculate a value for pi - this should have internal linkage
// const double pi = 2. * std::atan2(1., 0.);
//extern const double pi;

// angle units conversion functions
double to_radians(double theta_d);
double to_degrees(double theta_r);

// use these functions for things like visual angle per pixel
double degrees_subtended(double size_measure, double distance_measure);
double degrees_subtended_per_unit(double units_per_measure, double distance_measure);
double units_per_degree_subtended(double units_per_measure, double distance_measure);

// forward class declarations
struct Cartesian_vector;
struct Polar_vector;


/* 
// A Point contains an (x, y) pair to represent coordinates
struct Point
{
	double x;
	double y;
	
	Point (double in_x = 0., double in_y = 0.) :
		x(in_x), y(in_y)
		{}

	// compare two Points
	bool operator== (const Point& rhs) const
		{return (x == rhs.x && y == rhs.y);}
 
	bool operator!= (const Point& rhs) const
		{return !(*this == rhs);}
		
	bool operator< (const Point& rhs) const
		{return (x == rhs.x) ? (y < rhs.y) : (x < rhs.x);}
		
	bool operator<= (const Point& rhs) const
		{return (*this < rhs) || (*this == rhs);}
		
	bool operator> (const Point& rhs) const
		{return !(*this <= rhs);}

	bool operator>= (const Point& rhs) const
		{return !(*this < rhs);}
};
*/

/* Size */
// A Size contains an (h, v) pair to represent the horizontal
// and vertical size of an object, which is different from
// a Point or a Cartesian_vector.
struct Size
{
	double h;
	double v;
	
	explicit Size (double in_h = 0., double in_v = 0.) :
		h(in_h), v(in_v)
		{}

	// compare two Sizes
	bool operator== (const Size& rhs) const
		{
			return (h == rhs.h && v == rhs.v);
		}
 
	bool operator!= (const Size& rhs) const
		{
			return (h != rhs.h || v != rhs.v);
		}
};


// return the distance between two Points
double cartesian_distance (const Point& p1, const Point& p2);

/* Cartesian_vector */
// A Cartesian_vector contains an x, y displacement
struct Cartesian_vector
{
	double delta_x;
	double delta_y;

	Cartesian_vector (double in_delta_x = 0., double in_delta_y = 0.) : 
		delta_x(in_delta_x), delta_y(in_delta_y)
	{}

	// construct a Cartesian_vector from two Points,
	// showing the vector from p1 to p2 
	// that is, p1 + cv => p2
	Cartesian_vector(const Point& p1, const Point& p2);

	// construct a Cartesian_vector from a Polar_vector
	Cartesian_vector(const Polar_vector& pv);
};


/* Polar_vector */
// Polar_vector describes a displacement in terms of polar coordinates
// with angle in radians
struct Polar_vector
{
	double r;
	double theta;

	Polar_vector (double in_r = 0., double in_theta = 0.) : 
		r(in_r), theta(in_theta)
	{}

	// construct a Polar_vector from two Points,
	// showing the vector from p1 to p2 
	// that is, p1 + pv => p2
	Polar_vector(const Point& p1, const Point& p2);

	// construct a Polar_vector from a Cartesian_vector
	Polar_vector(const Cartesian_vector& cv);

};




// *** Overloaded Operators ***
// For ease in documentation, all overloaded operators are non-member functions

// Subtract two Points to get a Cartesian_vector
// p2's components are subtracted from p1
Cartesian_vector operator- (const Point& p1, const Point& p2);

// Add a Point and a Cartesian_vector to get the displaced Point
Point operator+ (const Point& p, const Cartesian_vector& cv);
Point operator+ (const Cartesian_vector& cv, const Point& p);
	
// Add a Point and a Polar_vector to get the displaced Point
Point operator+ (const Point& p, const Polar_vector& pv);
Point operator+ (const Polar_vector& pv, const Point& p);

// Adding or subtracting two Cartesian_vectors adds or subtracts the components
Cartesian_vector operator+ (const Cartesian_vector& cv1, const Cartesian_vector& cv2);
Cartesian_vector operator- (const Cartesian_vector& cv1, const Cartesian_vector& cv2);

// divide a Cartesian_vector by a double: divide each component by the double
Cartesian_vector operator/ (const Cartesian_vector& cv, double d);
Cartesian_vector operator/ (double d, const Cartesian_vector& cv);

// divide a Polar_vector by a double: divide r component by the double
Polar_vector operator/ (const Polar_vector& pv, double d);
Polar_vector operator/ (double d, const Polar_vector& pv);

// multiply a Cartesian_vector by a double: multiply each component by the double
Cartesian_vector operator* (const Cartesian_vector& cv, double d);
Cartesian_vector operator* (double d, const Cartesian_vector& cv);

// multiply a Polar_vector by a double: multiply r component by the double
Polar_vector operator* (const Polar_vector& pv, double d);
Polar_vector operator* (double d, const Polar_vector& pv);

/*
The Line_segment class stores two endpoints and the terms of the general form,
and precomputes for speed some values used in various calculations.

In the parametric form, line is specified in terms of its endpoints, p1 and p2, 
the differences dx = p2.x - p1.x, and dy = p2.y - p1.y, and a parameter t
that describes where on the line a particular point is, starting from p1.

A point is on the line if its (x, y) coordinates can be specified as
    x = p1.x + t * dx
    y = p1.y + t * dy 
    for some value of t.
Note that p1 corresponds to t = 0, p2 to t = 1. So for a given (x, y), a value
of t between 0 and 1 means that the point lies between the endpoints. A value
of t < 0 means that t comes "before" p1


In the general form, a point (x, y) is on the line if it satisfies the equation
	A * x + B * y + C == 0, where
	
	len = distance between p1 and p2
	A = -dy/len;
	B = dx/len;
	C = -(-dy*x1 + dx*y1)/len

For speed, most functions are inline, and dx, dy, len, and c (numerator of C) 
are computed and saved when Line_segment is initialized. 

Note that x and y values are doubles, and if they have fractional values, some
functions and tests that assume exact values will not be accurate. 
But if all coordinates have integral values, then functions such as is_horizontal 
should be correct.
*/

class Line_segment {
public:
	Line_segment() : // default is all values zero - normally not used.
		dx(0.), dy(0.), c(0.), len(0.)
		{}
	Line_segment(Point in_p1, Point in_p2);
	
	Point get_p1() const
		{return p1;}

	Point get_p2() const
		{return p2;}

	// return the center of the bounding box
	Point get_center() const
		{return center;}
	// return the Size of the bounding box
	Size get_size() const
		{return size;}

	double get_dx() const
		{return dx;}

	double get_dy() const
		{return dy;}

	double get_A() const
		{return -dy/len;}

	double get_B() const
		{return dx/len;}

	double get_C() const
		{return c/len;}

	double get_length() const
		{return len;}

	bool is_horizontal() const
		{return dy == 0.;}

	bool is_vertical() const
		{return dx == 0.;}
	
	// These functions treat the segment as an infinite line
	// Return true if p is on the infinite line described by the segment
	bool is_on_infinite_line(Point p) const
		{
			if((-dy * p.x + dx * p.y + c) == 0)
				return true;
			return false;
		}

	// Compute the closest point on the infinite line
	Point closest_point_on_infinite_line(Point p) const
		{return point_on_line(parameter(p));}

	// Return the distance from the line to a point
	double distance_from_infinite_line(Point p) const;

	// Return the parameter value for the point. 
	// The value will be between 0 and 1 if the point is between the endpoints.
	// Otherwise, t corresponds to the position of the point along the line.
	// t < 0 means p is "before" p1, t > 1, "after" p2.
	double parameter(Point p) const
		{return (-(p1.x - p.x) * dx - (p1.y - p.y) * dy) / (len * len);}

	// Compute t given a value for x
	double parameter_given_x(double x) const
		{
			Assert (dx != 0.); // vertical line is indeterminate!
			return (x - p1.x) / dx;
		}

	// Return x of point on the line specified by t
	double x_given_parameter(double t) const
		{return p1.x + t * dx;}

	// Compute t given a value for y
	double parameter_given_y(double y) const
		{
			Assert(dy != 0.);	// horizontal line is indeterminate!
			return (y - p1.y) / dy;
		}

	// Return y of point on the line specified by t
	double y_given_parameter(double t) const
		{return p1.y + t * dy;}

	// Return the point on the line specified by t
	Point point_on_line(double t) const
		{return Point(p1.x + t * dx, p1.y + t * dy);}

	// Return the closest point on the line segment to p;
	// This is either a point on the line, or the closest endpoint
	Point closest_point_on_segment(Point p) const
		{
			double t = parameter(p);
			if (t <= 0.)
				return p1;
			else if (t >= 1.)
				return p2;
			else
				return point_on_line(t);
		}

	// Return the distance from p to the closest point on the line segment
	double distance_from_segment(Point p) const
		// {return distance(p, closest_point_on_segment(p));}
		{
			double t = parameter(p);
			if (t <= 0.)
				return cartesian_distance(p, p1);
			else if (t >= 1.)
				return cartesian_distance(p, p2);
			else {
				return distance_from_infinite_line(p);
				}
		}

private:
	Point p1;	// the "start" point
	Point p2;	// the "stop" point
	double dx;	// (x2 - x1) is -a, A is -a/len
	double dy;	// (y2 - y1) is b, B is b/len
	double c;	// C is c/len
	double len;	// length of the line segment

	Point center;	// center of the bounding box
	Size size;	// size of the bounding box

};

std::ostream& operator<< (std::ostream& os, const Line_segment& ls);

// Given a point and a rectangle, return true if the point is inside the rectangle,
// false if not.
bool is_point_inside_rectangle(Point p, Point rect_loc, Size rect_size);

// Given a line and a rectangle, compute and return the line segment that is the line clipped to the rectangle.
Line_segment clip_line_to_rectangle(const Line_segment& line, Point rect_loc, Size rect_size);

// Given a line from a point through the center of a rectangle, calculate
// the line that goes from the closest point of intersection on the rectangle
// to the center of the rectangle. Use for e.g. Fitts ID calculations.
bool compute_center_intersecting_line(const Line_segment& start_to_center, Size rect_size, Line_segment& clipped_line);

// Compute the closest distance from p to the rectangle given by center, size
double closest_distance(Point p, Point rect_center, Size rect_size);

// The polygon class stores a vector of points corresponding to adjacent vertices
// of the polygon - the last point "wraps" to the first point to close the polygon.
class Polygon {
public:
	// an empty polygon has default size and center
	Polygon ()	: cache_good(true) 
		{}
	// Can initialize with a vector of points if desired
	Polygon(const std::vector<Point>& in_vertices) :
		vertices(in_vertices), cache_good(false)
		{}
	// create the polygon by adding points in order
	void add_vertex(Point p)
		{vertices.push_back(p); cache_good = false;}
	// empty the polygon
	void clear()
		{vertices.clear(); cache_good = false;}
	// supply a const reference to the vertices
	const std::vector<Point>& get_vertices() const
		{return vertices;}
	// return the center of the bounding box
	Point get_center() const;
	// return the Size of the bounding box
	Size get_size() const;
	// compute the distance of p from the edge of the polygon. 
	// A negative value means that p is outside the polygon, positive means p is inside
	double distance_inside(const Point p) const;

private:
	std::vector<Point> vertices;
	// cached values are recomputed if points changed since last calculation
	mutable bool cache_good;	// false if size and center need to be computed anew
	mutable Point center;
	mutable Size size;
	void recompute() const;	// const since called from const functions
};

} // end namespace


#endif
