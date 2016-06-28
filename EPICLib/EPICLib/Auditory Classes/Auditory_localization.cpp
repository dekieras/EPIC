//
//  Auditory_localization.cpp
//  EPIC
//
//  Created by David Kieras on 9/9/13.
//  Copyright (c) 2013 David Kieras. All rights reserved.
//

/*
 This uses an localization function worked out by Greg Wakefield circa June 30, 2013
 using a combination of results from Oldfield and Middlebrooks.
 It flips a coin to decide whether there is a front-back reversal.
*/

#include "Auditory_localization.h"
#include "Standard_Symbols.h"
#include "Numeric_utilities.h"
#include "Random_utilities.h"
#include "Output_tee_globals.h"

// add error variability to location of a an auditory object
// returned value is erroneous location

static double modify_azimuth(double);
static double compute_bias(double);
static double compute_reversal(double);
static double compute_sd(double);

// disabled 6/26/14 for NSMRL work
// enabled Mill1968 version for BrungartSimpson2004 work

GU::Point apply_auditory_location_error(GU::Point original_location)
{
/*
    return original_location;
*/
    // location error in azimuth only
    double x = original_location.x;
	if (x <= 180.) { // for right side
		x = modify_azimuth(x);
		}
	else {  // for left side
		x = 360. - x; // transform to same scale as right side		if(debug_messages)
		x = modify_azimuth(x);
        x = 360. - x; // transform back
		}

	return GU::Point(x, original_location.y);

}

// modify the supplied azimuth with noise, biased, and flipped in some order
double modify_azimuth(double original_azimuth)
{
	double sd = compute_sd(original_azimuth);
	double bias = compute_bias(original_azimuth);
	double biased_azimuth = original_azimuth + bias;
	double x = normal_random_variable(biased_azimuth, sd);
	x = compute_reversal(x);
	return x;
}

/*
 use approximation based on Mills1968, front quadrant only - undefined for elsewhere

 pMAA approximation
Azimuth			slope		intercept
0 <= x <= 45	0.021233333	1.075
45 < x <= 60	0.053466667	-0.3755
60 < x <= 90	0.349833333	-18.1575
		
SD=pMMA/1.36 for sd corresponding to 25%, 75% ??? check
*/

double compute_sd (double x)
{
	double pMMA;
	if(0. <= x && x <= 45.)
		pMMA = 0.021233333 * x + 1.075;
	else if(45. < x && x <= 60.)
		pMMA = 0.053466667 * x - 0.3755;
	else if(60. < x && x <= 90.)
		pMMA = 0.349833333 * x - 18.1575;
	else
		pMMA = 0.1 * x - 5;  // some made-up values

	return pMMA / 1.36;	// approximate sd
}

// no bias at this time
// return the biased_azimuth computed as a function of the azimuth
double compute_bias(double x)
{
	return 0.;
}

// no reversal at this time
double compute_reversal(double original_azimuth)
{
	return original_azimuth;
}

/*
// return the sd for the guassian error sampling
double compute_sd (double x)
{
// based on Middlebrooks
// =-0.00000007337*A2^4+0.00001787*A2^3-0.001513*A2^2+0.1231*A2+2.29
	const double a4 = -0.0000000733;
	const double a3 = +0.00001787;
	const double a2 = -0.001513;
	const double a1 = +0.1231;
	const double a0 = +2.29;
	
	double sd = a4 * (x * x * x * x) + a3 * (x * x *x) + a2 * (x * x) + a1 * (x) + a0;
	return sd;
}

// return the biased_azimuth computed as a function of the azimuth
double compute_bias(double x)
{
// based on combined Oldfield & Middlebrooks data
// =0.0000002801*A17^4-0.00007509*A17^3+0.004356*A17^2+0.006655*A17+0.1454
	const double a4 = 0.0000002801;
	const double a3 = -0.00007509;
	const double a2 = +0.004356;
	const double a1 = +0.006655;
	const double a0 = +0.1454;
	
	double biased = a4 * (x * x * x * x) + a3 * (x * x *x) + a2 * (x * x) + a1 * (x) + a0;
	return biased;
}


// Flip a coin that follows a bell curve probability roughly centered on 90 degrees
// The probability of the coin follows a gaussian curve, but this is not a gaussian distribution.
// Return either the original value or the flipped value.
double compute_reversal(double original_azimuth)
{
	const double m = 90.;
	const double s = 57.9;
	const double a = 62.54;
	const double b = 0.01318;
	const double c = 0.278;
	
	// compute value to compare to uniform random variable
	double az = (original_azimuth - m) / s;
	double z = (az - b) / c;
	double p = a * exp(-(z*z));
	
	double x = unit_uniform_random_variable();
	if(x <= p)
		return 180. - original_azimuth;// reverse
	else
		return original_azimuth;
}

*/
