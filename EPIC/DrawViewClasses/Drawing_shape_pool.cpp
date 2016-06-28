/*
 *  Drawing_shape_pool.cpp
 *  AppClassesDev
 *
 *  Created by David Kieras on 12/20/06.
 *  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
 *
 */

#include "Drawing_shape_pool.h"
#include "Drawing_shapes.h"
#include "EPICLib/Standard_Symbols.h"

// A Meyers Singleton
Drawing_shape_pool& Drawing_shape_pool::get_instance()
{
	static Drawing_shape_pool the_shape_map;
	return the_shape_map;
}

Drawing_shape_pool::Drawing_shape_pool()
{
	shape_map[Symbol()] =			new Circle_shape(false);
	shape_map[Unknown_c] =			new Circle_shape(false);
	shape_map[Nil_c] =				new Circle_shape(false);
	shape_map[None_c] =				new No_Shape;
	shape_map[Circle_c] = 			new Circle_shape(true);
	shape_map[Empty_Circle_c] =		new Circle_shape(false);
	shape_map[Filled_Circle_c] =	new Circle_shape(true);
	shape_map[Top_Semicircle_c] = 			new Top_Semicircle_shape(true);
	shape_map[Empty_Top_Semicircle_c] =		new Top_Semicircle_shape(false);
	shape_map[Filled_Top_Semicircle_c] =	new Top_Semicircle_shape(true);
	shape_map[Rectangle_c] =		new Rectangle_shape(true);
	shape_map[Empty_Rectangle_c] = 	new Rectangle_shape(false);
	shape_map[Filled_Rectangle_c] =	new Rectangle_shape(true);
	shape_map[Square_c] =			new Rectangle_shape(true);
	shape_map[Empty_Square_c] =		new Rectangle_shape(false);
	shape_map[Filled_Square_c] =	new Rectangle_shape(true);
	shape_map[Button_c] = 			new Button_shape(true);
	shape_map[Empty_Button_c] = 	new Button_shape(false);
	shape_map[Filled_Button_c] = 	new Button_shape(true);
	shape_map[Triangle_c] =			new Triangle_shape(true);
	shape_map[Empty_Triangle_c] = 	new Triangle_shape(false);
	shape_map[Filled_Triangle_c] = 	new Triangle_shape(true);
	shape_map[Diamond_c] = 			new Diamond_shape(true);
	shape_map[Empty_Diamond_c] =	new Diamond_shape(false);
	shape_map[Filled_Diamond_c] = 	new Diamond_shape(true);
	shape_map[House_c] =			new House_shape(true);
	shape_map[Empty_House_c] =		new House_shape(false);
	shape_map[Filled_House_c] = 	new House_shape(true);
	shape_map[Inv_House_c] =		new Inv_House_shape(true);
	shape_map[Inv_Empty_House_c] =	new Inv_House_shape(false);
	shape_map[Inv_Filled_House_c] = new Inv_House_shape(true);
	shape_map[Cross_c] =			new Cross_shape(true);
	shape_map[Empty_Cross_c] =		new Cross_shape(false);
	shape_map[Filled_Cross_c] =		new Cross_shape(true);
	shape_map[Bar_c] =				new Bar_shape(true);
	shape_map[Empty_Bar_c] =		new Bar_shape(false);
	shape_map[Filled_Bar_c] =		new Bar_shape(true);
	shape_map[Clover_c] = 			new Clover_shape(true);
	shape_map[Empty_Clover_c] = 	new Clover_shape(false);
	shape_map[Filled_Clover_c] = 	new Clover_shape(true);
	shape_map[Clover3_c] =			new Clover3_shape(true);
	shape_map[Empty_Clover3_c] =	new Clover3_shape(false);
	shape_map[Filled_Clover3_c] =	new Clover3_shape(true);
	shape_map[Inv_Clover3_c] =		new Inv_Clover3_shape(true);
	shape_map[Inv_Empty_Clover3_c] = new Inv_Clover3_shape(false);
	shape_map[Inv_Filled_Clover3_c] = new Inv_Clover3_shape(true);
	shape_map[Heart_c] = 			new Heart_shape(true);
	shape_map[Empty_Heart_c] =		new Heart_shape(false);
	shape_map[Filled_Heart_c] = 	new Heart_shape(true);
	shape_map[Hill_c] =				new Hill_shape(true);
	shape_map[Empty_Hill_c] =		new Hill_shape(false);
	shape_map[Filled_Hill_c] =		new Hill_shape(true);
	shape_map[Inv_Hill_c] =			new Inv_Hill_shape(true);
	shape_map[Inv_Empty_Hill_c] =	new Inv_Hill_shape(false);
	shape_map[Inv_Filled_Hill_c] =	new Inv_Hill_shape(true);
	shape_map[Cross_Hairs_c] = 		new Cross_hairs_shape;
	shape_map[Cursor_Arrow_c] =		new Arrow_shape(true, 120);
	shape_map[Up_Arrow_c] =			new Arrow_shape(true, 90);
	shape_map[Down_Arrow_c] = 		new Arrow_shape(true, 270);
	shape_map[Left_Arrow_c] = 		new Arrow_shape(true, 180);
	shape_map[Right_Arrow_c] = 		new Arrow_shape(true, 0);
	shape_map[Line_c] =				new Line_shape;

	shape_map[North_Leader_c] =		new Leader_north;
	shape_map[North_c] =			new Leader_north;
	shape_map[South_Leader_c] =		new Leader_south;
	shape_map[South_c] =			new Leader_south;
	shape_map[East_Leader_c] = 		new Leader_east;
	shape_map[East_c] =				new Leader_east;
	shape_map[West_Leader_c] = 		new Leader_west;
	shape_map[West_c] =				new Leader_west;

	shape_map[Polygon_c] = 			new Polygon_shape(false);
	shape_map[Empty_Polygon_c] = 	new Polygon_shape(false);
	shape_map[Filled_Polygon_c] = 	new Polygon_shape(true);
	
	shape_map[T000_c] =				new T_object_shape(0);
	shape_map[T090_c] =				new T_object_shape(90);
	shape_map[T135_c] =				new T_object_shape(135);
	shape_map[T180_c] =				new T_object_shape(180);
	shape_map[T270_c] =				new T_object_shape(270);
	shape_map[L000_c] =				new L_object_shape(0);
	shape_map[L090_c] =				new L_object_shape(90);
	shape_map[L135_c] =				new L_object_shape(135);
	shape_map[L180_c] =				new L_object_shape(180);
	shape_map[L270_c] =				new L_object_shape(270);

	shape_map[Block_X_c] =			new Block_X_shape;
	shape_map[Block_Y_c] =			new Block_Y_shape;
}

Drawing_shape_pool::~Drawing_shape_pool()
{
	for(shape_map_t::iterator it = shape_map.begin(); it != shape_map.end(); ++it)
		delete it->second;
}

Drawing_shape * Drawing_shape_pool::get_shape_ptr(const Symbol& shape_name) const
{
	shape_map_t::const_iterator it = shape_map.find(shape_name);
	if(it == shape_map.end())
		return get_shape_ptr(Unknown_c);
	return it->second;
}

