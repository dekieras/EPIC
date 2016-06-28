#include "Standard_Symbols.h"
#include "Standard_utility_symbols.h"

// A "standard" visible object created or maintained by the device should use
// one of these as the identity name of the object, and the architecture assumes
// the existence of these objects. The actual name is the symbol name without "_name_c"

const Symbol Cursor_name_c("Cursor");			// the cursor visual object
const Symbol Tracking_cursor_name_c("Tracking_cursor");	// the tracking cursor visual object
const Symbol Mouse_name_c("Mouse");				// the mouse device itself
const Symbol Mouse_Left_Button_c("Mouse_Left_Button");	// use for single mouse button
const Symbol Mouse_Middle_Button_c("Mouse_Middle_Button");
const Symbol Mouse_Right_Button_c("Mouse_Right_Button");
const Symbol Keyboard_name_c("Keyboard");		// the keyboard device as a whole

// used as a property name - but might be reserved
const Symbol Status_c("Status");

// standard symbols for status and changes
const Symbol Visual_c("Visual");
const Symbol Visual_status_c("Visual_status");
const Symbol New_c("New");
const Symbol Visible_c("Visible");
const Symbol Disappearing_c("Disappearing");
const Symbol Reappearing_c("Reappearing");
const Symbol Disappeared_object_c("Disappeared_object");
const Symbol Change_c("Change");
const Symbol Color_changed_c("Color_changed");
const Symbol BgColor_changed_c("BgColor_changed");
const Symbol FgColor_changed_c("FgColor_changed");

// standard visual property names
const Symbol Color_c("Color");
const Symbol Color_Vague_c("Color_Vague");
const Symbol Targetable_c ("Targetable");
const Symbol BgColor_c("BgColor");
const Symbol FgColor_c("FgColor");
const Symbol Shape_c("Shape");
const Symbol Size_c("Size");	// for actual size
const Symbol Encoded_size_c("Encoded_size");
const Symbol Label_c("Label");
const Symbol Text_c("Text");
const Symbol Position_c("Position");
const Symbol Vposition_c("Vposition");
const Symbol Hposition_c("Hposition");
const Symbol Location_c("Location");
const Symbol Orientation_c("Orientation");
const Symbol Distance_c("Distance");
const Symbol Depth_c("Depth");
const Symbol Encoding_c("Encoding");
const Symbol Leader_c("Leader");

// values for other standard visual properties
const Symbol Horizontal_c("Horizontal");
const Symbol Vertical_c("Vertical");
const Symbol Top_c("Top");
const Symbol Bottom_c("Bottom");

// Type_c and standard values
const Symbol Type_c("Type");
const Symbol Screen_c("Screen");
const Symbol Window_c("Window");
// const Symbol Button_c("Button"); // see shapes
const Symbol Dialog_c("Dialog");
const Symbol Menu_c("Menu");
const Symbol Menu_item_c("Menu_item");
const Symbol Field_c("Field");

// standard Color propery values
const Symbol Aqua_c ("Aqua");
const Symbol Black_c ("Black");
const Symbol Blue_c ("Blue");
const Symbol Brown_c ("Brown");
const Symbol Chartreuse_c ("Chartreuse");
const Symbol Cyan_c ("Cyan");
const Symbol DarkBlue_c ("DarkBlue");
const Symbol DarkGray_c ("DarkGray");
const Symbol DarkGreen_c ("DarkGreen");
const Symbol DarkRed_c ("DarkRed");
const Symbol DarkViolet_c ("DarkViolet");
const Symbol Gainsboro_c ("Gainsboro");	// a very light gray
const Symbol Green_c ("Green");
const Symbol Gray_c ("Gray");
const Symbol Fuchsia_c ("Fuchsia");
const Symbol Gold_c ("Gold");
const Symbol GoldenRod_c ("GoldenRod");
const Symbol LightBlue_c ("LightBlue");
const Symbol LightGray_c ("LightGray");
const Symbol Magenta_c ("Magenta");
const Symbol Maroon_c ("Maroon");
const Symbol Navy_c ("Navy");
const Symbol Olive_c ("Olive");
const Symbol Pink_c ("Pink");
const Symbol Purple_c ("Purple");
const Symbol Red_c ("Red");
const Symbol RoyalBlue_c ("RoyalBlue");
const Symbol SlateGray_c ("SlateGray");
const Symbol Teal_c ("Teal");
const Symbol Turquoise_c ("Turquoise");
const Symbol Violet_c ("Violet");
const Symbol White_c ("White");
const Symbol Yellow_c ("Yellow");
const Symbol Vague_c ("Vague");

// standard Shape property values
const Symbol Circle_c ("Circle");
const Symbol Empty_Circle_c ("Empty_Circle");
const Symbol Filled_Circle_c ("Filled_Circle");
const Symbol Top_Semicircle_c ("Top_Semicircle");
const Symbol Empty_Top_Semicircle_c ("Empty_Top_Semicircle");
const Symbol Filled_Top_Semicircle_c ("Filled_Top_Semicircle");
const Symbol Rectangle_c ("Rectangle");
const Symbol Empty_Rectangle_c ("Empty_Rectangle");
const Symbol Filled_Rectangle_c ("Filled_Rectangle");
const Symbol Square_c ("Square");
const Symbol Empty_Square_c ("Empty_Square");
const Symbol Filled_Square_c ("Filled_Square");
const Symbol Button_c ("Button");
const Symbol Empty_Button_c ("Empty_Button");
const Symbol Filled_Button_c ("Filled_Button");
const Symbol Triangle_c("Triangle");
const Symbol Empty_Triangle_c("Empty_Triangle");
const Symbol Filled_Triangle_c("Filled_Triangle");
const Symbol Diamond_c("Diamond");
const Symbol Empty_Diamond_c("Empty_Diamond");
const Symbol Filled_Diamond_c("Filled_Diamond");
const Symbol House_c("House");
const Symbol Empty_House_c("Empty_House");
const Symbol Filled_House_c("Filled_House");
const Symbol Inv_House_c("Inv_House");
const Symbol Inv_Empty_House_c("Inv_Empty_House");
const Symbol Inv_Filled_House_c("Inv_Filled_House");
const Symbol Cross_c("Cross");
const Symbol Empty_Cross_c("Empty_Cross");
const Symbol Filled_Cross_c("Filled_Cross");
const Symbol Bar_c("Bar");
const Symbol Empty_Bar_c("Empty_Bar");
const Symbol Filled_Bar_c("Filled_Bar");
const Symbol Clover_c("Clover");
const Symbol Empty_Clover_c("Empty_Clover");
const Symbol Filled_Clover_c("Filled_Clover");
const Symbol Clover3_c("Clover3");
const Symbol Empty_Clover3_c("Empty_Clover3");
const Symbol Filled_Clover3_c("Filled_Clover3");
const Symbol Inv_Clover3_c("Inv_Clover3");
const Symbol Inv_Empty_Clover3_c("Inv_Empty_Clover3");
const Symbol Inv_Filled_Clover3_c("Inv_Filled_Clover3");
const Symbol Heart_c("Heart");
const Symbol Empty_Heart_c("Empty_Heart");
const Symbol Filled_Heart_c("Filled_Heart");
const Symbol Hill_c("Hill");
const Symbol Empty_Hill_c("Empty_Hill");
const Symbol Filled_Hill_c("Filled_Hill");
const Symbol Inv_Hill_c("Inv_Hill");
const Symbol Inv_Empty_Hill_c("Inv_Empty_Hill");
const Symbol Inv_Filled_Hill_c("Inv_Filled_Hill");
const Symbol Cross_Hairs_c ("Cross_Hairs");
const Symbol Cursor_Arrow_c ("Cursor_Arrow");
const Symbol Up_Arrow_c ("Up_Arrow");
const Symbol Down_Arrow_c ("Down_Arrow");
const Symbol Left_Arrow_c ("Left_Arrow");
const Symbol Right_Arrow_c ("Right_Arrow");
const Symbol Line_c("Line");
const Symbol Polygon_c("Polygon");
const Symbol Empty_Polygon_c("Empty_Polygon");
const Symbol Filled_Polygon_c("Filled_Polygon");

// special Shape values
const Symbol North_Leader_c("North_Leader");
const Symbol North_c("North");
const Symbol South_Leader_c("South_Leader");
const Symbol South_c("South");
const Symbol East_Leader_c("East_Leader");
const Symbol East_c("East");
const Symbol West_Leader_c("West_Leader");
const Symbol West_c("West");

// special Shape values for visual search targets
extern const Symbol T000_c("T000");
extern const Symbol T090_c("T090");
extern const Symbol T135_c("T135");
extern const Symbol T180_c("T180");
extern const Symbol T270_c("T270");
extern const Symbol L000_c("L000");
extern const Symbol L090_c("L090");
extern const Symbol L135_c("L135");
extern const Symbol L180_c("L180");
extern const Symbol L270_c("L270");

extern const Symbol Block_X_c("Block_X");
extern const Symbol Block_Y_c("Block_Y");

// standard visual property names that have another object name as the value
const Symbol Left_of_c("Left_of");
const Symbol Right_of_c("Right_of");
const Symbol Above_c("Above");
const Symbol Below_c("Below");
const Symbol In_center_of_c("In_center_of");
const Symbol Inside_c("Inside");
const Symbol Outside_c("Outside");
const Symbol In_row_c("In_row");
const Symbol In_col_c("In_col");
const Symbol Placed_on_c("Placed_on");
const Symbol Pointing_to_c("Pointing_to");

/* Auditory properties and values */
// property names and values
const Symbol Auditory_c("Auditory");
const Symbol Auditory_status_c("Auditory_status");
const Symbol Stream_c("Stream");
const Symbol Azimuth_c("Azimuth");
const Symbol Audible_c("Audible");
const Symbol Fading_c("Fading");
const Symbol Pitch_c("Pitch");
const Symbol Loudness_c("Loudness");
const Symbol Timbre_c("Timbre");
const Symbol Present_c("Present");
const Symbol Next_c("Next");
const Symbol Time_stamp_c("Time_stamp");
const Symbol External_c("External");	// use when distinct localized stream objects unnecessary
const Symbol Default_physical_stream_c("DefaultStream");
const Symbol Default_psychological_stream_c("SrcDefaultStream");
const Symbol Speech_c("Speech");
const Symbol Content_c("Content");
const Symbol Gender_c("Gender");
const Symbol Male_c("Male");
const Symbol Female_c("Female");
const Symbol Speaker_c("Speaker");

/* Other standard Symbols */
// standard signal values
const Symbol Signal_c("Signal");
const Symbol Start_c("Start");
const Symbol Stop_c("Stop");
const Symbol Halt_c("Halt");
const Symbol End_c("End");					// For temporal
const Symbol Increment_c("Increment");		// For temporal


