#ifndef VISUAL_CONSTANTS_H
#define VISUAL_CONSTANTS_H

/* Some basic visual constants are defined here - these values are used in various places,
primarily to initialize default parameter settings.
But they might be overridden by specific parameter settings.
Note that #including these will normally produce internal linkage.
*/

const double standard_fovea_radius = 1.0;
const double standard_parafovea_radius = 7.5;	// 'standard' from Lisp-EPIC era
//const double standard_peripheral_radius = 60.0; // original standard value
const double standard_peripheral_radius = 70.0;  // increase to handle apparent go-out-of view bug in HZ13 model with large saccade length noise 3/31/16
const long standard_short_delay = 25;
const long standard_delay = 50;
const long standard_long_delay = 100;



#endif
