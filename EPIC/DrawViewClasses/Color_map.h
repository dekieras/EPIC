/*
 *  Color_map.h
 *  EPICApp
 *
 *  Created by David Kieras on 1/14/07.
 *  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
 *
 */

#ifndef COLORMAP_H
#define COLORMAP_H
#include <map>
#include "EPICLib/Symbol.h"
#include <ApplicationServices/ApplicationServices.h>

// a Singleton that returns a CGColorRef to the named color given a Symbol
class Color_map {
public:
	static Color_map& get_instance();
	CGColorRef get_color(const Symbol& color_name);
	
	
private:
	Color_map();
	
	typedef std::map<Symbol, CGColorRef> color_map_t;
	color_map_t color_map;
};

#endif
