#include "Epic_standard_symbols.h"

// These are standard names known throughout the architecture as symbols
// used in perceptual and motor processing. The string-representation
// of the system is normally the same as the name with the "_c" suffix removed.
// See Epic_standard_names.cpp for the actual definition.
// As new names are used in the architecture, they should be added to this set,
// and the code should use these variable names instead of explicit strings.


// Standard names for visual and auditory properties:
const Symbol Detection_c("Detection");
const Symbol Onset_c("Onset");
const Symbol Offset_c("Offset");
const Symbol Start_time_c("Start_time");
const Symbol End_time_c("End_time");

const Symbol Eccentricity_c("Eccentricity");
const Symbol Eccentricity_zone_c("Eccentricity_zone");
const Symbol Fovea_c("Fovea");
const Symbol Periphery_c("Periphery");

// Standard names for motor command terms:
const Symbol Perform_c("Perform");
const Symbol Prepare_c("Prepare");
const Symbol Manual_c("Manual");
const Symbol Keystroke_c("Keystroke");
const Symbol Hold_c("Hold");
const Symbol Release_c("Release");
const Symbol Punch_c("Punch");
const Symbol Ply_c("Ply");
const Symbol Point_c("Point");
const Symbol Click_on_c("Click_on");
const Symbol Ocular_c("Ocular");
const Symbol Move_c("Move");
const Symbol Look_for_c("Look_for");
const Symbol Vocal_c("Vocal");
const Symbol Speak_c("Speak");
const Symbol Subvocalize_c("Subvocalize");
const Symbol Set_mode_c("Set_mode");
const Symbol Enable_c("Enable");
const Symbol Disable_c("Disable");
const Symbol Centering_c("Centering");
const Symbol Reflex_c("Reflex");

// Auditory stream names for speech
const Symbol Overt_c("Overt");
const Symbol Covert_c("Covert");

// Hand names
const Symbol Right_c("Right");
const Symbol Left_c("Left");
const Symbol Thumb_c("Thumb");
const Symbol Index_c("Index");
const Symbol Middle_c("Middle");
const Symbol Ring_c("Ring");
const Symbol Little_c("Little");

// Motor processor signal constants
const Symbol Motor_c("Motor");
const Symbol Modality_c("Modality");
const Symbol Processor_c("Processor");
const Symbol Preparation_c("Preparation");
const Symbol Execution_c("Execution");
const Symbol Busy_c("Busy");
const Symbol Free_c("Free");
const Symbol Started_c("Started");
const Symbol Finished_c("Finished");

// Temporal processor
const Symbol Temporal_c("Temporal");
const Symbol Ticks_c("Ticks");

// symbols used for standard items in production rules
const Symbol Tag_c("Tag");
const Symbol WM_c("WM");
