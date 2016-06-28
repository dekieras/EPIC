/*
 *  AppColor_utilities.h
 *  AppClassesDev
 *
 *  Created by David Kieras on 11/29/06.
 *  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
 *
 */
#ifndef APPCOLORUTILITIES_H
#define APPCOLORUTILITIES_H
#include <ApplicationServices/ApplicationServices.h>

extern CGColorSpaceRef gColorSpace;

extern CGColorRef gColor_Aqua;
extern CGColorRef gColor_Black;
extern CGColorRef gColor_Blue;
extern CGColorRef gColor_Brown;
extern CGColorRef gColor_Chartreuse;
extern CGColorRef gColor_Cyan;
extern CGColorRef gColor_DarkBlue;
extern CGColorRef gColor_DarkGray;
extern CGColorRef gColor_DarkGreen;
extern CGColorRef gColor_DarkRed;
extern CGColorRef gColor_DarkViolet;
extern CGColorRef gColor_Gainsboro;	// a very light gray
extern CGColorRef gColor_Green;
extern CGColorRef gColor_Gray;
extern CGColorRef gColor_Gray_transparent;
extern CGColorRef gColor_Fuchsia;
extern CGColorRef gColor_Gold;
extern CGColorRef gColor_GoldenRod;
extern CGColorRef gColor_LightBlue;
extern CGColorRef gColor_LightGray;
extern CGColorRef gColor_Magenta;
extern CGColorRef gColor_Maroon;
extern CGColorRef gColor_Navy;
extern CGColorRef gColor_Olive;
extern CGColorRef gColor_Pink;
extern CGColorRef gColor_Purple;
extern CGColorRef gColor_Red;
extern CGColorRef gColor_RoyalBlue;
extern CGColorRef gColor_SlateGray;
extern CGColorRef gColor_Teal;
extern CGColorRef gColor_Turquoise;
extern CGColorRef gColor_Violet;
extern CGColorRef gColor_White;
extern CGColorRef gColor_Yellow;

// create and return a extern CGColorRef gColor_iven the R,G,B, and alpha
CGColorRef create_color(float red, float green, float blue, float alpha);

// create and return a extern CGColorRef gColor_iven the R,G,B, expressed as a single hex number
// as in web colors, and with a specified alpha
CGColorRef create_color(long hexcolor, float alpha);
// create and return a CGColorRef gColor_iven the R,G,B, expressed as a single hex number
// as in web colors, and with an alpha of 1.0
CGColorRef create_color(long hexcolor);

#endif
