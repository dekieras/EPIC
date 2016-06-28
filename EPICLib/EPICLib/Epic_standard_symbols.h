#ifndef EPIC_STANDARD_SYMBOLS_H
#define EPIC_STANDARD_SYMBOLS_H

#include "Symbol.h"
#include "Standard_Symbols.h"
#include "Standard_utility_symbols.h"

// These are standard names known inside the architecture as symbols
// used in perceptual and motor processing. The string-representation
// of the system is normally the same as the name with the "_c" suffix removed.
// See Epic_standard_names.cpp for the actual definition.
// As new names are used in the architecture, they should be added to this set,
// and the code should use these variable names instead of explicit strings.

// Standard names for visual and auditory properties:
extern const Symbol Detection_c;
extern const Symbol Onset_c;
extern const Symbol Offset_c;
extern const Symbol Start_time_c;
extern const Symbol End_time_c;

extern const Symbol Eccentricity_c;
extern const Symbol Eccentricity_zone_c;
extern const Symbol Fovea_c;
extern const Symbol Periphery_c;


// Standard names for motor command terms:
extern const Symbol Perform_c;
extern const Symbol Prepare_c;
extern const Symbol Manual_c;
extern const Symbol Keystroke_c;
extern const Symbol Hold_c;
extern const Symbol Release_c;
extern const Symbol Punch_c;
extern const Symbol Ply_c;
extern const Symbol Point_c;
extern const Symbol Click_on_c;
extern const Symbol Ocular_c;
extern const Symbol Move_c;
extern const Symbol Look_for_c;
extern const Symbol Vocal_c;
extern const Symbol Speak_c;
extern const Symbol Subvocalize_c;
extern const Symbol Set_mode_c;
extern const Symbol Enable_c;
extern const Symbol Disable_c;
extern const Symbol Centering_c;
extern const Symbol Reflex_c;

// Auditory stream names for speech
extern const Symbol Overt_c;
extern const Symbol Covert_c;

// Hand names
extern const Symbol Right_c;
extern const Symbol Left_c;
extern const Symbol Thumb_c;
extern const Symbol Index_c;
extern const Symbol Middle_c;
extern const Symbol Ring_c;
extern const Symbol Little_c;

// Motor processor signal constants
extern const Symbol Motor_c;
extern const Symbol Modality_c;
extern const Symbol Processor_c;
extern const Symbol Preparation_c;
extern const Symbol Execution_c;
extern const Symbol Busy_c;
extern const Symbol Free_c;
extern const Symbol Started_c;
extern const Symbol Finished_c;

// Temporal processor
extern const Symbol Temporal_c;
extern const Symbol Ticks_c;

// symbols used for standard items in production rules
extern const Symbol Tag_c;
extern const Symbol WM_c;



#endif
