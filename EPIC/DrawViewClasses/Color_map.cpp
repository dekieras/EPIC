/*
 *  Color_map.cpp
 *  EPICApp
 *
 *  Created by David Kieras on 1/14/07.
 *  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
 *
 */

#include "Color_map.h"
#include "AppColorUtilities.h"
#include "EPICLib/Standard_Symbols.h"

// Meyers Singleton
Color_map& Color_map::get_instance()
{
	static Color_map the_map;
	return the_map;
}

CGColorRef Color_map::get_color(const Symbol& color_name)
{
	color_map_t::iterator it = color_map.find(color_name);
	if(it == color_map.end()) {
		it = color_map.find(Unknown_c);
		if(it == color_map.end())
			return gColor_LightGray;
		}
	return it->second;
}

Color_map::Color_map()
{
	color_map[Symbol()] =		gColor_LightGray;
	color_map[Unknown_c] =		gColor_LightGray;
	color_map[Aqua_c] =			gColor_Aqua;
	color_map[Black_c] =		gColor_Black;
	color_map[Blue_c] =			gColor_Blue;
	color_map[Brown_c] =		gColor_Brown;
	color_map[Chartreuse_c] =	gColor_Chartreuse;
	color_map[Cyan_c] =			gColor_Cyan;
	color_map[DarkBlue_c] =		gColor_DarkBlue;
	color_map[DarkGray_c] =		gColor_DarkGray;
	color_map[DarkGreen_c] =	gColor_DarkGreen;
	color_map[DarkRed_c] =		gColor_DarkRed;
	color_map[DarkViolet_c] =	gColor_DarkViolet;
	color_map[Gainsboro_c] =	gColor_Gainsboro;	// a very light gray
	color_map[Green_c] =		gColor_Green;
	color_map[Gray_c] =			gColor_Gray;
	color_map[Fuchsia_c] =		gColor_Fuchsia;
	color_map[Gold_c] =			gColor_Gold;
	color_map[GoldenRod_c] =	gColor_GoldenRod;
	color_map[LightBlue_c] =	gColor_LightBlue;
	color_map[LightGray_c] =	gColor_LightGray;
	color_map[Magenta_c] =		gColor_Magenta;
	color_map[Maroon_c] =		gColor_Maroon;
	color_map[Navy_c] =			gColor_Navy;
	color_map[Olive_c] =		gColor_Olive;
	color_map[Pink_c] =			gColor_Pink;
	color_map[Purple_c] =		gColor_Purple;
	color_map[Red_c] =			gColor_Red;
	color_map[RoyalBlue_c] =	gColor_RoyalBlue;
	color_map[SlateGray_c] =	gColor_SlateGray;
	color_map[Teal_c] =			gColor_Teal;
	color_map[Turquoise_c] =	gColor_Turquoise;
	color_map[Violet_c] =		gColor_Violet;
	color_map[White_c] =		gColor_White;
	color_map[Yellow_c] =		gColor_Yellow;
}
