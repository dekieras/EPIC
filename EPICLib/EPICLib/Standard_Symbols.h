#ifndef STANDARD_SYMBOLS_H
#define STANDARD_SYMBOLS_H

#include "Symbol.h"
#include "Standard_utility_symbols.h"

// include this file in any C++ code that needs to use Symbols commonly used in models
// The actual symbol string value is the same as the name without the "_c" suffix.
// Add new values to this set if they are likely to be re-used in either GLEAN or EPIC
// Architecture-specific Symbols are defined in <architecture>_standard_symbols


/* Visual objects, properties, and values */
// standard visual objects
// A "standard" visible object created or maintained by the device should use
// one of these as the identity name of the object, and the architecture assumes
// the existence of these objects. The actual name is the symbol name without "_name_c"
extern const Symbol Cursor_name_c;			// the cursor visual object
extern const Symbol Tracking_cursor_name_c;	// the tracking cursor visual object
extern const Symbol Mouse_name_c;			// the mouse device itself
extern const Symbol Mouse_Left_Button_c;	// use for single mouse button
extern const Symbol Mouse_Middle_Button_c;
extern const Symbol Mouse_Right_Button_c;
extern const Symbol Keyboard_name_c;		// the keyboard device as a whole

// used as a property name - but might be reserved
extern const Symbol Status_c;

// standard symbols for visual status and changes
extern const Symbol Visual_c;
extern const Symbol Visual_status_c;
extern const Symbol New_c;
extern const Symbol Visible_c;
extern const Symbol Targetable_c;
extern const Symbol Disappearing_c;
extern const Symbol Reappearing_c;
extern const Symbol Disappeared_object_c;
extern const Symbol Change_c;
extern const Symbol Color_changed_c;
extern const Symbol BgColor_changed_c;
extern const Symbol FgColor_changed_c;

// standard visual property names
extern const Symbol Color_c;
extern const Symbol Color_Vague_c;
extern const Symbol BgColor_c;
extern const Symbol FgColor_c;
extern const Symbol Shape_c;
extern const Symbol Size_c;	// for actual size
extern const Symbol Encoded_size_c;
extern const Symbol Label_c;
extern const Symbol Text_c;
extern const Symbol Position_c;
extern const Symbol Vposition_c;
extern const Symbol Hposition_c;
extern const Symbol Location_c;
extern const Symbol Orientation_c;
extern const Symbol Distance_c;
extern const Symbol Depth_c;
extern const Symbol Leader_c;	// for radar display leader lines

// values for orther standard visual properties
extern const Symbol Horizontal_c;
extern const Symbol Vertical_c;
extern const Symbol Top_c;
extern const Symbol Bottom_c;

// Type_c and standard values
extern const Symbol Type_c;
extern const Symbol Screen_c;
extern const Symbol Window_c;
// extern const Symbol Button_c; // see shapes
extern const Symbol Dialog_c;
extern const Symbol Menu_c;
extern const Symbol Menu_item_c;
extern const Symbol Field_c;

// standard Color propery values
extern const Symbol Aqua_c;
extern const Symbol Black_c;
extern const Symbol Blue_c;
extern const Symbol Brown_c;
extern const Symbol Chartreuse_c;
extern const Symbol Cyan_c;
extern const Symbol DarkBlue_c;
extern const Symbol DarkGray_c;
extern const Symbol DarkGreen_c;
extern const Symbol DarkRed_c;
extern const Symbol DarkViolet_c;
extern const Symbol Gainsboro_c;
extern const Symbol Green_c;
extern const Symbol Gray_c;
extern const Symbol Fuchsia_c;
extern const Symbol Gold_c;
extern const Symbol GoldenRod_c;
extern const Symbol LightBlue_c;
extern const Symbol LightGray_c;
extern const Symbol Magenta_c;
extern const Symbol Maroon_c;
extern const Symbol Navy_c;
extern const Symbol Olive_c;
extern const Symbol Pink_c;
extern const Symbol Purple_c;
extern const Symbol Red_c;
extern const Symbol RoyalBlue_c;
extern const Symbol SlateGray_c;
extern const Symbol Teal_c;
extern const Symbol Turquoise_c;
extern const Symbol Violet_c;
extern const Symbol White_c;
extern const Symbol Yellow_c;
extern const Symbol Vague_c;

// standard Shape property values
extern const Symbol Circle_c ;
extern const Symbol Empty_Circle_c ;
extern const Symbol Filled_Circle_c ;
extern const Symbol Top_Semicircle_c ;
extern const Symbol Empty_Top_Semicircle_c ;
extern const Symbol Filled_Top_Semicircle_c ;
extern const Symbol Rectangle_c ;
extern const Symbol Empty_Rectangle_c ;
extern const Symbol Filled_Rectangle_c ;
extern const Symbol Square_c ;
extern const Symbol Empty_Square_c ;
extern const Symbol Filled_Square_c ;
extern const Symbol Button_c ;
extern const Symbol Empty_Button_c ;
extern const Symbol Filled_Button_c ;
extern const Symbol Triangle_c;
extern const Symbol Empty_Triangle_c;
extern const Symbol Filled_Triangle_c;
extern const Symbol Diamond_c;
extern const Symbol Empty_Diamond_c;
extern const Symbol Filled_Diamond_c;
extern const Symbol House_c;
extern const Symbol Empty_House_c;
extern const Symbol Filled_House_c;
extern const Symbol Inv_House_c;
extern const Symbol Inv_Empty_House_c;
extern const Symbol Inv_Filled_House_c;
extern const Symbol Cross_c;
extern const Symbol Empty_Cross_c;
extern const Symbol Filled_Cross_c;
extern const Symbol Bar_c;
extern const Symbol Empty_Bar_c;
extern const Symbol Filled_Bar_c;
extern const Symbol Clover_c;
extern const Symbol Empty_Clover_c;
extern const Symbol Filled_Clover_c;
extern const Symbol Clover3_c;
extern const Symbol Empty_Clover3_c;
extern const Symbol Filled_Clover3_c;
extern const Symbol Inv_Clover3_c;
extern const Symbol Inv_Empty_Clover3_c;
extern const Symbol Inv_Filled_Clover3_c;
extern const Symbol Heart_c;
extern const Symbol Empty_Heart_c;
extern const Symbol Filled_Heart_c;
extern const Symbol Hill_c;
extern const Symbol Empty_Hill_c;
extern const Symbol Filled_Hill_c;
extern const Symbol Inv_Hill_c;
extern const Symbol Inv_Empty_Hill_c;
extern const Symbol Inv_Filled_Hill_c;
extern const Symbol Cross_Hairs_c;
extern const Symbol Cursor_Arrow_c;
extern const Symbol Up_Arrow_c;
extern const Symbol Down_Arrow_c;
extern const Symbol Left_Arrow_c;
extern const Symbol Right_Arrow_c;
extern const Symbol Line_c;
extern const Symbol Polygon_c;
extern const Symbol Empty_Polygon_c;
extern const Symbol Filled_Polygon_c;

// special Shape values
extern const Symbol North_Leader_c;
extern const Symbol North_c;
extern const Symbol South_Leader_c;
extern const Symbol South_c;
extern const Symbol East_Leader_c;
extern const Symbol East_c;
extern const Symbol West_Leader_c;
extern const Symbol West_c;

// special Shape values for visual search targets
extern const Symbol T000_c;
extern const Symbol T090_c;
extern const Symbol T135_c;
extern const Symbol T180_c;
extern const Symbol T270_c;
extern const Symbol L000_c;
extern const Symbol L090_c;
extern const Symbol L135_c;
extern const Symbol L180_c;
extern const Symbol L270_c;
extern const Symbol Block_X_c;
extern const Symbol Block_Y_c;

// standard visual property names that have another object name as the value
extern const Symbol Left_of_c;
extern const Symbol Right_of_c;
extern const Symbol Above_c;
extern const Symbol Below_c;
extern const Symbol Inside_c;
extern const Symbol In_center_of_c;
extern const Symbol Outside_c;
extern const Symbol In_row_c;
extern const Symbol In_col_c;
extern const Symbol Placed_on_c;
extern const Symbol Pointing_to_c;

/* Auditory properties and values */
extern const Symbol Auditory_c;
extern const Symbol Auditory_status_c;
extern const Symbol Stream_c;
extern const Symbol Azimuth_c;
extern const Symbol Audible_c;
extern const Symbol Fading_c;
extern const Symbol Pitch_c;
extern const Symbol Loudness_c;
extern const Symbol Timbre_c;
extern const Symbol Present_c;
extern const Symbol Next_c;
extern const Symbol Time_stamp_c;
extern const Symbol External_c;	// use when distinct localized stream objects unnecessary
extern const Symbol Default_physical_stream_c;
extern const Symbol Default_psychological_stream_c;
extern const Symbol Speech_c;
extern const Symbol Content_c;
extern const Symbol Gender_c;
extern const Symbol Male_c;
extern const Symbol Female_c;
extern const Symbol Speaker_c;


/* Other standard Symbols */
// standard signal values
extern const Symbol Signal_c;
extern const Symbol Start_c;
extern const Symbol Stop_c;
extern const Symbol Halt_c;
extern const Symbol End_c;				// For temporal
extern const Symbol Increment_c;		// For temporal

#endif


