#ifndef SYMBOL_GEOMETRY_UTILITIES_H
#define SYMBOL_GEOMETRY_UTILITIES_H

#include "Symbol.h"
#include "Geometry.h"

inline Geometry_Utilities::Point to_Point(const Symbol& s)
{
	return Geometry_Utilities::Point(s.get_x(), s.get_y());
}

inline Geometry_Utilities::Size to_Size(const Symbol& s)
{
	return Geometry_Utilities::Size(s.get_x(), s.get_y());
}

inline Symbol to_Symbol(Geometry_Utilities::Point pt)
{
	return Symbol(pt.x, pt.y);
}

inline Symbol to_Symbol(Geometry_Utilities::Size sz)
{
	return Symbol(sz.h, sz.v);
}

#endif
