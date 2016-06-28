/* 
Geometry.cpp implementation file
See Geometry.h for comments
*/
#include "Geometry.h"
#include "Assert_throw.h"

#include <iostream>
#include <vector>
#include <cmath>

using std::ostream;
using std::vector;
using std::size_t;
using std::sqrt;	using std::cos;	using std::sin;	using std::atan2;	using std::fabs;


//const double Geometry_Utilities::pi = 2. * std::atan2(1., 0.);

namespace Geometry_Utilities {



// *** Member function definitions ***

// return the distance between two Points
double cartesian_distance (const Point& p1, const Point& p2)
{
	double xd = p2.x - p1.x;
	double yd = p2.y - p1.y;
	double d = sqrt(xd * xd + yd * yd);
	return d;
}

// Cartesian_vector members
// construct a Cartesian_vector from two Points,
// showing the vector from p1 to p2 
// that is, p1 + cv => p2
Cartesian_vector::Cartesian_vector(const Point& p1, const Point& p2)
{
	delta_x = p2.x - p1.x;
	delta_y = p2.y - p1.y;
}

// construct a Cartesian_vector from a Polar_vector
Cartesian_vector::Cartesian_vector(const Polar_vector& pv)
{
	delta_x = pv.r * cos(pv.theta);
	delta_y = pv.r * sin(pv.theta);
}

// Polar_vector members
// construct a Polar_vector from a Cartesian_vector
Polar_vector::Polar_vector(const Cartesian_vector& cv)
{
	r = sqrt ((cv.delta_x * cv.delta_x) + (cv.delta_y * cv.delta_y));
	// atan2 will return neg angle for Quadrant III, IV.
	theta = atan2 (cv.delta_y, cv.delta_x);
	if (theta < 0.)
		theta = 2. * GU_pi + theta; // normalize theta positive
}

// construct a Polar_vector from two Points,
// showing the vector from p1 to p2 
// that is, p1 + pv => p2
Polar_vector::Polar_vector(const Point& p1, const Point& p2)
{
	Polar_vector pv (Cartesian_vector(p1, p2));
	r = pv.r;
	theta = pv.theta;
}




// *** Overloaded Operators ***
// For ease in documentation, all overloaded operators are non-member functions

// Subtract two Points to get a Cartesian_vector
// p2's components are subtracted from p1
Cartesian_vector operator- (const Point& p1, const Point& p2)
{
	return Cartesian_vector(p1.x - p2.x, p1.y - p2.y);
}

// Add a Point and a Cartesian_vector to get the displaced Point
Point operator+ (const Point& p, const Cartesian_vector& cv)
{
	return Point(p.x + cv.delta_x, p.y + cv.delta_y);
}

Point operator+ (const Cartesian_vector& cv, const Point& p)
{
	return p + cv;
}
	
// Add a Point and a Polar_vector to get the displaced Point
Point operator+ (const Point& p, const Polar_vector& pv)
{
	Cartesian_vector cv (pv);
	return cv + p;
}

Point operator+ (const Polar_vector& pv, const Point& p)
{
	return p + pv;
}

// Adding or subtracting two Cartesian_vectors adds or subtracts the components
Cartesian_vector operator+ (const Cartesian_vector& cv1, const Cartesian_vector& cv2)
{
	return Cartesian_vector(cv1.delta_x + cv2.delta_x, cv1.delta_y + cv2.delta_y);
}

Cartesian_vector operator- (const Cartesian_vector& cv1, const Cartesian_vector& cv2)
{
	return Cartesian_vector(cv1.delta_x - cv2.delta_x, cv1.delta_y - cv2.delta_y);
}

// divide a Cartesian_vector by a double: divide each component by the double
Cartesian_vector operator/ (const Cartesian_vector& cv, double d)
{
	return Cartesian_vector(cv.delta_x / d, cv.delta_y / d);
}

Cartesian_vector operator/ (double d, const Cartesian_vector& cv)
{
	return cv / d;
}

// divide a Polar_vector by a double: divide r component by the double
Polar_vector operator/ (const Polar_vector& pv, double d)
{
	return Polar_vector(pv.r / d, pv.theta);
}
Polar_vector operator/ (double d, const Polar_vector& pv)
{
	return pv / d;
}

// multiply a Cartesian_vector by a double: multiply each component by the double
Cartesian_vector operator* (const Cartesian_vector& cv, double d)
{
	return Cartesian_vector(cv.delta_x * d, cv.delta_y * d);
}

Cartesian_vector operator* (double d, const Cartesian_vector& cv)
{
	return cv * d;
}

// multiply a Polar_vector by a double: multiply r component by the double
Polar_vector operator* (const Polar_vector& pv, double d)
{
	return Polar_vector(pv.r * d, pv.theta);
}

Polar_vector operator* (double d, const Polar_vector& pv)
{
	return pv * d;
}

// Output operators
// output a Point as "(x, y)"
ostream& operator<< (ostream& os, const Point& p)
{
	os << '(' << p.x << ", " << p.y << ')';
	return os;
}
	
// output a Size as "[h, v]"
ostream& operator<< (ostream& os, const Size& s)
{
	os << '[' << s.h << ", " << s.v << ']';
	return os;
}

// output a Cartesian_vector as "<x, y>"
ostream& operator<< (ostream& os, const Cartesian_vector& cv)
{
	os << '<' << cv.delta_x << ", " << cv.delta_y << '>';
	return os;
}

// output a Polar_vector as "P<r, theta>"
ostream& operator<< (ostream& os, const Polar_vector& pv)
{
	os << "P<" << pv.r << ", " << pv.theta << '>';
	return os;
}

/***** Utility function definitions *****/
// There are 2pi radians in 360 degrees
double to_radians (double theta_d)
{
//const double pi = 2. * std::atan2(1., 0.);
	return 2. * GU_pi  * (theta_d / 360);
}

double to_degrees (double theta_r)
{
	const double pi = 2. * std::atan2(1., 0.);
//	double temp = 360. * theta_r / (2. * GU_pi);
	double temp = 360. * theta_r / (2. * pi);
	return temp;
}

// size_measure and distance_measure must be in the same units (e.g. pixels, inches)
// if signed, atan2 uses these to determine the quadrants involved.
double degrees_subtended(double size_measure, double distance_measure)
{
	double radians = 2. * atan2(size_measure, 2. * distance_measure);
	return to_degrees(radians);
}

// I am not sure of the purpose of this definition - seems roundabout
double degrees_subtended_per_unit(double units_per_measure, double distance_measure)
{
	double radians = 2. * atan2( 1./units_per_measure, 2. * distance_measure);
	return to_degrees(radians);
}

double units_per_degree_subtended(double units_per_measure, double distance_measure)
	{return 1./degrees_subtended_per_unit(units_per_measure, distance_measure);}

/*
The Line_segment class stores two endpoints and the terms of the general form,
and precomputes for speed some values used in various calculations.
*/
Line_segment::Line_segment(Point in_p1, Point in_p2) :
	p1(in_p1), p2(in_p2)
{
	dx = p2.x - p1.x;
	dy = p2.y - p1.y;
	len = sqrt(dx * dx + dy * dy);
	c = -(-dy * p1.x + dx * p1.y);
	
	center.x = (p1.x + p2.x) / 2.;
	center.y = (p1.y + p2.y) / 2.;

	size.h = fabs(dx);
	size.v = fabs(dy);

//	center.x = p1. x + (dx > 0.) ? size.h / 2.0 : - size.h / 2.0;
//	center.y = p1. y + (dy > 0.) ? size.v / 2.0 : - size.v / 2.0;
}

// Return the distance from the line to a point
double Line_segment::distance_from_infinite_line(Point p) const
{
	return fabs(-dy * p.x + dx * p.y + c) / len;
}

// output a Line_segment as "p1 -> p2"
ostream& operator<< (ostream& os, const Line_segment& ls)
{
	os << ls.get_p1() << " -> " << ls.get_p2();
	return os;
}

// Given a point and a rectangle specified as the center of the rectangle, and its size
// return true if the point is inside the rectangle, false if not.
bool is_point_inside_rectangle(Point p, Point rect_loc, Size rect_size)
{
    double xmin = rect_loc.x - rect_size.h / 2.;
    double xmax = rect_loc.x + rect_size.h / 2.;
    if(p.x < xmin || p.x > xmax)
        return false;
    double ymin = rect_loc.y - rect_size.v / 2.;
    double ymax = rect_loc.y + rect_size.v / 2.;
    if(p.y < ymin || p.y > ymax)
        return false;
    return true;
}



// Given a line and a rectangle, compute and return the line segment that is the line clipped to the rectangle.
// Based on the Cohen-Sutherland clipping algorithm (see Foley, van Damm, Feiner, & Hughes).

Line_segment clip_line_to_rectangle(const Line_segment& line, Point rect_loc, Size rect_size)
{
	double xmin = rect_loc.x;
	double xmax = rect_loc.x + rect_size.h;
	double ymin = rect_loc.y;
	double ymax = rect_loc.y + rect_size.v;
	Point p1 = line.get_p1();
	Point p2 = line.get_p2();
	double dx = line.get_dx();
	double dy = line.get_dy();
	
	double txmax = line.parameter_given_x(xmax);
	double txmin = line.parameter_given_x(xmin);
	double tymax = line.parameter_given_y(ymax);
	double tymin = line.parameter_given_y(ymin);
	
	Point cp1, cp2;
	// line goes from top-right (p1) towards bottom left (p2) (updside-down y)	
	if(dx < 0. && dy > 0.) {
		// choose cp1
		if(txmax < 0. && tymin < 0.) {
			// xmax and ymin are before p1, so use p1
			cp1 = p1;
			}
		else if(txmax < 0. && tymin > 0. && tymin < 1.0) {
			// xmax is before p1, clip from tymin (upside-down, remember!)
			cp1 = line.point_on_line(tymin);
			}
		else if(tymin < 0. && txmax > 0. && txmax < 1.0) {
			// ymin is before p1, clip from txmax
			cp1 = line.point_on_line(txmax);
			}
		// choose cp2
		if(txmin > 1.0 && tymax > 1.0) {
			// xmin and ymax are "after" p2, use p2
			cp2 = p2;
			}
		else if(tymax > 1.0 && txmin > 0. && txmin < 1.0) {
			// ymax is "after" p2, clip to txmin
			cp2 = line.point_on_line(txmin);
			}
		else if(txmin > 1.0 && tymax > 0. && tymax < 1.0) {
			// xmin is "after" p2, clip to tymax
			cp2 = line.point_on_line(tymax);
			}
		}
	// line goes from bottom-left towards top-right		
	else if(dx > 0. && dy < 0.) {
		// choose cp1
		if(txmin < 0. && tymax < 0.) {
			// xmin and ymax are before p1, so use p1
			cp1 = p1;
			}
		// choose cp2
		if(txmax > 1. && tymin > 0. && tymin < 1.0) {
			// xmax is after p2, but not ymin, so use tymin
			cp2 = line.point_on_line(tymin);
			}
		else if(txmax > 1.0 && tymin > 1.0) {
			// both xmax and ymin are after p2, so use p2
			cp2 = p2;
			}
		}
		
	// line goes from top-left(p1) towards bottom-right (p2)
	else if(dx > 0. && dy > 0.) {
		// choose cp1
		if(txmin < 0. && tymin < 0.) {
			// xmin and ymin are before p1, so use p1
			cp1 = p1;
			}
		// choose cp2
		if(tymax > 1.0 && txmax > 1.0) {
			// ymax and xmax are both after p2, so use p2
			cp2 = p2;
			}
		else if(tymax > 1. && txmax > 0. && txmax < 1.0) {
			// ymax is after p2, but not xmax, so use txmax
			cp2 = line.point_on_line(txmax);
			}
		else if(txmax > 1. && tymax > 0. && tymax < 1.0) {
			// xmax is after p2, but not ymax, so use tymax
			cp2 = line.point_on_line(tymax);
			}
		else if(txmax > 0. && txmax < 1.0 && tymax > 0. && tymax < 1.0 && txmax < tymax) {
			// both xmax and ymax are before p2, but xmax is closer to p1 than ymax
			cp2 = line.point_on_line(txmax);
			}
		else if(txmax > 0. && txmax < 1.0 && tymax > 0. && tymax < 1.0 && tymax < txmax) {
			// both xmax and ymax are before p2, but ymax is closer to p1 than xmax
			cp2 = line.point_on_line(tymax);
			}
		}

	
	Line_segment clipped_line(cp1, cp2);

	return clipped_line;

}


/*
Calculate the line that intersects a rectangle through its center, using a specialization 
of the Cohen-Sutherland clipping algorithm (see Foley, van Damm, Feiner, & Hughes).

The start-to-center line goes from the start point P0 to the end point P1, 
which is the center of the rectangular target region, whose boundaries are 
min_edges.x, max_edges.x, min_edges.y, max_edges.y.

Since P1 is always inside the rectangle, we check only for whether P0 is also inside,
and then which edge of the rectangle the line crosses, and compute the point of
intersection.

clipped_line is the line ending in the center that intersects the edge of rectangle.
return true if starting point is outside and d and s values are valid; false otherwise
*/

bool compute_center_intersecting_line(const Line_segment& start_to_center, const Size rect_size, Line_segment& clipped_line)
{
	// the start of the line is p0, the starting point, and will get moved to points of intersection during the computation
	Point p0(start_to_center.get_p1());

	// the end of the line is p1, the center of the target, and stays fixed.
	const Point p1(start_to_center.get_p2()); 

	// max and min_edges are the boundaries of the target rectangle
	// max edges are the top right coordinates
	Point max_edges(p1.x + rect_size.h/2, p1.y + rect_size.v/2);
	// min edges are the lower left coordinates
	Point min_edges(p1.x - rect_size.h/2, p1.y - rect_size.v/2);

	// Where does the line p0-p1 intersect the rectangle? Calculate intersection accordingly.
	// if the point is not outside, then start point must be inside the rectangle, 
	// and no movement should be made
	
	Point p; // point of intersection
	bool top, bottom, right, left;
	bool first = true;

	while(true) {
	top = bottom = left = right = false;
	// calculate where p0 is outside
	if (p0.y > max_edges.y) {
		top = true;
		}
	else if (p0.y < min_edges.y) {
		bottom = true;
		}
	if (p0.x > max_edges.x) {
		right = true;
		}
	else if (p0.x < min_edges.x) {
		left = true;
		}
	// if p0 is not outside, we are done
	// If it was the first value of p0, then starting point was inside, and result is invalid
	// otherwise stop and compute the final result
	// if p0 is still outside, continue the computation with the new value
	if (!(top || bottom || right || left)) {
		if(first)
			return false;
		else
			break;
		}
	else
		first = false;
	// Where does the line intersect the rectangle?
	// On the top
	if (top) {
		p.x = p1.x + (p0.x - p1.x) * (max_edges.y - p1.y)/(p0.y - p1.y);
		p.y = max_edges.y;
		}

	// On the bottom
	else if (bottom) {
		p.x = p1.x + (p0.x - p1.x) * (min_edges.y - p1.y)/(p0.y - p1.y);
		p.y = min_edges.y;
		}

	// On the right
	else if (right) {
		p.x = max_edges.x;
		p.y = p1.y + (p0.y - p1.y) * (max_edges.x - p1.x)/ (p0.x - p1.x);
		}

	// On the left
	else if (left) {
		p.x = min_edges.x;
		p.y = p1.y + (p0.y - p1.y) * (min_edges.x - p1.x)/ (p0.x - p1.x);
		}
	
	// move p0 to the point of intersection and repeat
	p0 = p;
	}	// main loop
	// return the line segment between the intersection and the center
	clipped_line = Line_segment(p0, p1);
	return true;
}

// Compute the closest distance from p to the rectangle given by center, size
double closest_distance(Point p, Point rect_center, Size rect_size)
{
	// max and min_edges are the boundaries of the target rectangle
	// tr edges are the top right coordinates
	Point tr_edges(rect_center.x + rect_size.h/2, rect_center.y + rect_size.v/2);
	// ll edges are the lower left coordinates
	Point ll_edges(rect_center.x - rect_size.h/2, rect_center.y - rect_size.v/2);
	
	double x_ll_distance = fabs(p.x - ll_edges.x);
	double x_tr_distance = fabs(p.x - tr_edges.x);
	double y_ll_distance = fabs(p.y - ll_edges.y);
	double y_tr_distance = fabs(p.y - tr_edges.y);
	
	Point vseg_p1, vseg_p2, hseg_p1, hseg_p2;
		
	if(x_ll_distance < x_tr_distance) {
		// closest vertical edge is at left
		vseg_p1.x = vseg_p2.x = ll_edges.x;
		vseg_p1.y = ll_edges.y;
		vseg_p2.y = tr_edges.y;
		}
	else {
		// closest vertical edge is at right
		vseg_p1.x = vseg_p2.x = tr_edges.x;
		vseg_p1.y = ll_edges.y;
		vseg_p2.y = tr_edges.y;
		}
	
	if(y_ll_distance < y_tr_distance) {
		// closest horizontal edge is the lower one
		hseg_p1.y = hseg_p2.y = ll_edges.y;
		hseg_p1.x = ll_edges.x;
		hseg_p2.x = tr_edges.x;
		}
	else {
		// closest horizontal edge is the top one
		hseg_p1.y = hseg_p2.y = tr_edges.y;
		hseg_p1.x = ll_edges.x;
		hseg_p2.x = tr_edges.x;
		}
	// construct the two closest segments
	Line_segment vseg(vseg_p1, vseg_p2);
	Line_segment hseg(hseg_p1, hseg_p2);
	
	// compute the distances, and return whichever is least
	double dist_vseg = vseg.distance_from_segment(p);
	double dist_hseg = hseg.distance_from_segment(p);
	return (dist_vseg < dist_hseg) ? dist_vseg : dist_hseg;
}


/* This function uses a special-cased version of the "odd-even" rule for determining
whether a point is inside a shape. We intersect the shape with a ray 
from the point that is horizontal to the right.

We iterate through the line segments making up the side and count the number of 
intersections of a line segment and a ray from the point that is horizontal to the right. 
If the number is odd, the point is inside the shape. If zero or even, the 
point is outside the shape.  Special cases appear if the ray intersects one of the
vertices of the polygon.

In the real plane with computed coordinates, it is unlikely that
we will get a ray intersecting exactly on a vertex. But it could happen.
And of course, if the vertices and points have integral values, it is much more likely.
*/

double Polygon::distance_inside(const Point p) const
{
	const size_t num_pts = vertices.size();
	const size_t num_segments = num_pts - 1;
	double mindist = 0;
	int count = 0;
	double previous_dy = 0.0;

	// Extend a ray to the right of p and count the intersections
	// We go through the vertices a pair at a time, constructing each line segment.
	// i will be the "start" point p1, i + 1 the "stop" point, p2, of the segment.
	// The last point wraps around to use the first point.
	
	for (size_t ptno = 0; ptno < num_pts; ptno++) {
		// Construct the line segment - wrap around for the last one
		size_t second_ptno = (ptno == num_segments) ? 0 : ptno + 1;
		Line_segment ls(vertices[ptno], vertices[second_ptno]);
		// get the distance and keep track of the minimum
		// we need this information regardless of whether the point is inside or not,
		// and must always be the closest segment regardless.
		double dist = ls.distance_from_segment(p);
		if (ptno == 0 || dist < mindist) 
			mindist = dist;
		// skip if this segment is horizontal
		if(ls.is_horizontal())
			continue;
		// find intersection of horizontal ray with line segment
		double t = ls.parameter_given_y(p.y);
		// if ray doesn't intersect the line segment, skip it	
		if ( t < 0.0 || t > 1.0) {
			previous_dy = ls.get_dy();
			continue;
			}
		// get value of x for intersection point; skip if not to the right
		if (ls.x_given_parameter(t) < p.x) {
			previous_dy = ls.get_dy();
			continue;
			}

		// The ray intersects the line segment, but check for special cases.
		// If point of intersection is at p1, count only if 
		//    1. this is the first segment (previous dy not set)
		//    or 
		//    2. dy has the same sign as in the previous segment
		// is point of intersection at p1?
		if (t == 0.0) {
			if ((ptno == 0) || (ls.get_dy() * previous_dy > 0.0))
				count++;
			}
		// If point of intersection is at p2, do not count - will check at next p1;
		// otherwise, count the intersection and go to the next segment
		else if (t != 1.0)
			count++;
		previous_dy = ls.get_dy();
		}
	// p is inside if we counted an odd number of intersections.
	// return positive distance if p is inside, negative if outside
	if (count % 2)
		return mindist;
	else
		return -mindist;
}

// return the center of the bounding box
Point Polygon::get_center() const
{
	if(!cache_good)
		recompute();
	return center;
}

// return the Size of the bounding box
Size Polygon::get_size() const
{
	if(!cache_good)
		recompute();
	return size;
}

void Polygon::recompute() const
{
	if(vertices.empty()) {
		size.h = size.v = 0.;
		center.x = center.y = 0.;
		cache_good = true;
		return;
		}
	
	// find largest and smallest values in each dimension
	double xmin = vertices[0].x;
	double xmax = xmin;
	double ymin = vertices[0].y;
	double ymax = ymin;
	for(size_t i = 1; i < vertices.size(); ++i) {
		Point p = vertices[i];
		if(p.x < xmin)
			xmin = p.x;
		else if(p.x > xmax)
			xmax = p.x;
		if(p.y < ymin)
			ymin = p.y;
		else if(p.y > ymax)
			ymax = p.y;
		}
	size.h = fabs(xmax - xmin);
	size.v = fabs(ymax - ymin);
	center.x = xmin + size.h / 2.;
	center.y = ymin + size.v / 2.;
	return;
}


} // end namespace
