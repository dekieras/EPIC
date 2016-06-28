//
//  Random_utilities.cpp
//  EPICXAF
//
//  Created by David Kieras on 4/19/13.
//
//

#include "Random_utilities.h"
#include "Assert_throw.h"
#include <random>
#include <cassert>

using namespace std;

// This code uses C++11's <random> library
// functions declared here for inline purposes

/*
typedef std::mt19937 Random_engine_t;
//typedef std::minstd_rand0 Random_engine_t;
//typedef std::knuth_b Random_engine_t;
//typedef std::default_random_engine Random_engine_t;

// The static local variable in this function is the global random engine, hidden inside the unnamed namespace
namespace {
Random_engine_t& get_Random_engine()
{
    static Random_engine_t engine;
    return engine;
}
}
*/


Random_engine_t& get_Random_engine()
{
    static Random_engine_t engine;
    return engine;
}


void set_random_number_generator_seed(unsigned long seed_)
{
    get_Random_engine().seed(static_cast<Random_engine_t::result_type>(seed_));
}

/* Random variable generation - using C++11 <random> */


// this returns a random integer in the range [0, range)
// that is, 0 ... range - 1

int random_int(int range)
{
    // uniform_int_distribution returns a <= x <= b
    static std::uniform_int_distribution<int> uid;
    using parm_t = decltype(uid)::param_type;
    int result = uid(get_Random_engine(), parm_t(0, range - 1));
    Assert(result != range);
    return result;
}

// Returns true with probability p
bool biased_coin_flip(double p)
{
	double rv = unit_uniform_random_variable();
	return (rv <= p);
}

double unit_uniform_random_variable()
{
    static std::uniform_real_distribution<double> uurd(0., 1.);
    return uurd(get_Random_engine());
}

// return a random variable that is uniformly distributed
// on each side of the mean +/- the deviation
double uniform_random_variable(double mean, double deviation)
{
	return 2. * deviation * unit_uniform_random_variable() - deviation + mean;
}

double unit_normal_random_variable()
{
    static std::normal_distribution<double> und(0., 1.);
    return und(get_Random_engine());
}


double normal_random_variable(double mean, double sd)
{
    static std::normal_distribution<double> nd;
    using parm_t = decltype(nd)::param_type;
    return nd(get_Random_engine(), parm_t(mean, sd));
}

double exponential_random_variable(double theta)
{
	return -theta * log(unit_uniform_random_variable());
}



double log_normal_random_variable(double m, double s)
{
	return m * exp(s * unit_normal_random_variable());
}

// This code is based on the alias method as described by Keith Schwarz at
// http://keithschwarz.com/darts-dice-coins/
// and is very closely directly transcribed into C++ from his Java code at
// http://www.keithschwarz.com/interesting/code/?dir=alias-method

// caller's vector needs to be copied
Discrete_distribution::Discrete_distribution(vector<double> probabilities) :
    // initialize the two arrays to have the same number of cells as the input
    n(int(probabilities.size())), probability(n), alias(n)
{
    assert(n);   // must be non-empty
    
    // average probability
    double average = 1.0 / n;
    
    // sort the probabilites into a large and small list
    // they might be of different lengths
    vector<int> small;
    vector<int> large;
    
    for(int i = 0; i < n; i++) {
        if(probabilities[i] >= average)
            large.push_back(i);
        else
            small.push_back(i);
        }
    
    // go through the lists together as long as there are entries in both,
    // and get the index of a large and small probability
    while(!small.empty() && !large.empty()) {
        int less = small.back();
        small.pop_back();
        int more = large.back();
        large.pop_back();
        
        // scale them up so that 1/n has weight 1.0
        probability[less] = probabilities[less] * n;
        alias[less] = more;
        // decrease the larger probability by the appropriate amount
        probabilities[more] = probabilities[more] + probabilities[less] - average;
        
        // put the new probability in the large or small list and continue the loop.
        if(probabilities[more] >= 1.0 / n)
            large.push_back(more);
        else
            small.push_back(more);
        }
    
    // at this point there may be left-over values in either list, so set those
    // indices to 1 in the probability table.
    while(!small.empty()) {
        probability[small.back()] = 1.0;
        small.pop_back();
        }
    while(!large.empty()) {
        probability[large.back()] = 1.0;
        large.pop_back();
        }
}

// return a random integer (0, n) based on the original probabilities
// by first picking a "column" in the 2D table, and then using probability
// in that "column" to flip a biased coin, and return either the "column" index
// or the alias at that index.
int Discrete_distribution::get_random_value() const
{
    int column = random_int(n);
    if(biased_coin_flip(probability[column]))
        return column;
    else
        return alias[column];
}


// returns true with a probability = p
bool uniform_detection_function(double p)
{
	double rv = unit_uniform_random_variable();
	return (rv <= p);
}

// As x increases, the probability that true is returned increases according to a Normal dbn from 0. to 1.0.
bool gaussian_detection_function(double x, double mean, double sd)
{
	double z = (x - mean) / sd;	// compute z-score
	double rv = unit_normal_random_variable();
	return (rv <= z);
}

// With lapse_probability, return false; else return the gaussian_detection_function result.
bool lapsed_gaussian_detection_function(double x, double mean, double sd, double lapse_probability)
{
    if(biased_coin_flip(lapse_probability))
        return false;
    return gaussian_detection_function(x, mean, sd);
}


// As x increases, the probability that true is returned increases according to a Normal dbn from base to 1.0.
bool based_gaussian_detection_function(double x, double base, double mean, double sd)
{
	double rv = unit_uniform_random_variable();
	if(rv <= base)
		return true;
	return gaussian_detection_function(x, mean, sd);
}

// As x increases, the probability that true is returned increases according to a Normal dbn from 0 to cap.
bool capped_gaussian_detection_function(double x, double cap, double mean, double sd)
{
	double rv = unit_uniform_random_variable();
	if(rv <= 1.0 - cap)
		return false;
	return gaussian_detection_function(x, mean, sd);
}

// As x increases, the probability that true is returned increases according to an exponential dbn from 0 to 1.0
// the lambda parameter is assumed to be the "small" definition - 
// e.g. a value of lambda = .5 gives p(x < 1) = .3868
// cf lambda = 2 giving p(x < 1) = .8706
bool exponential_detection_function(double x, double lambda)
{
	double rv = -log(unit_uniform_random_variable()) / lambda;
	return (rv <= x);
}

// As x increases, the probability that true is returned increases according to a exponential dbn from base to 1.0.
bool based_exponential_detection_function(double x, double base, double lambda)
{
	double rv = unit_uniform_random_variable();
	if(rv <= base)
		return true;
	return exponential_detection_function(x, lambda);
}



// given two z-scores, return the cumulative probability from the bivariate normal.
// e.g. (-3, +3) returns  0.001348; (+3, +3) returns 0.997302.
// (+1, +2) and (+2, +1) return 0.822204

/* implementation for get_bivariate_normal_cdf as fast table look-up */
// these constants should all have internal linkage

const int table_size = 25;
const double table[table_size][table_size] = {
/*          0         1         2         3         4         5         6         7         8         9         10        11        12        13        14        15        16        17        18        19        20        21        22        23        24 */
/* 0  */{0.000002, 0.000004, 0.000008, 0.000017, 0.000031, 0.000054, 0.000090, 0.000143, 0.000214, 0.000306, 0.000416, 0.000542, 0.000675, 0.000808, 0.000933, 0.001044, 0.001136, 0.001207, 0.001260, 0.001296, 0.001319, 0.001333, 0.001342, 0.001346, 0.001348}, 
/* 1  */{0.000004, 0.000009, 0.000019, 0.000036, 0.000068, 0.000119, 0.000199, 0.000315, 0.000473, 0.000675, 0.000919, 0.001196, 0.001490, 0.001784, 0.002060, 0.002304, 0.002507, 0.002665, 0.002781, 0.002860, 0.002912, 0.002943, 0.002961, 0.002971, 0.002976}, 
/* 2  */{0.000008, 0.000019, 0.000039, 0.000076, 0.000141, 0.000249, 0.000415, 0.000656, 0.000985, 0.001407, 0.001916, 0.002492, 0.003105, 0.003718, 0.004294, 0.004802, 0.005224, 0.005554, 0.005795, 0.005961, 0.006068, 0.006134, 0.006171, 0.006191, 0.006201}, 
/* 3  */{0.000017, 0.000036, 0.000076, 0.000149, 0.000278, 0.000490, 0.000817, 0.001292, 0.001939, 0.002770, 0.003772, 0.004906, 0.006112, 0.007319, 0.008453, 0.009454, 0.010285, 0.010933, 0.011408, 0.011735, 0.011946, 0.012075, 0.012149, 0.012188, 0.012208}, 
/* 4  */{0.000031, 0.000068, 0.000141, 0.000278, 0.000518, 0.000911, 0.001520, 0.002404, 0.003609, 0.005156, 0.007019, 0.009129, 0.011375, 0.013621, 0.015731, 0.017594, 0.019141, 0.020347, 0.021230, 0.021839, 0.022233, 0.022472, 0.022609, 0.022682, 0.022719}, 
/* 5  */{0.000054, 0.000119, 0.000249, 0.000490, 0.000911, 0.001605, 0.002676, 0.004232, 0.006356, 0.009079, 0.012360, 0.016075, 0.020030, 0.023984, 0.027699, 0.030981, 0.033704, 0.035827, 0.037383, 0.038454, 0.039148, 0.039569, 0.039810, 0.039940, 0.040005}, 
/* 6  */{0.000090, 0.000199, 0.000415, 0.000817, 0.001520, 0.002676, 0.004463, 0.007058, 0.010599, 0.015140, 0.020613, 0.026809, 0.033404, 0.039998, 0.046195, 0.051667, 0.056208, 0.059749, 0.062344, 0.064131, 0.065287, 0.065991, 0.066392, 0.066608, 0.066717}, 
/* 7  */{0.000143, 0.000315, 0.000656, 0.001292, 0.002404, 0.004232, 0.007058, 0.011162, 0.016762, 0.023943, 0.032597, 0.042397, 0.052825, 0.063253, 0.073053, 0.081707, 0.088888, 0.094488, 0.098592, 0.101418, 0.103246, 0.104358, 0.104994, 0.105335, 0.105507}, 
/* 8  */{0.000214, 0.000473, 0.000985, 0.001939, 0.003609, 0.006356, 0.010599, 0.016762, 0.025171, 0.035956, 0.048951, 0.063667, 0.079328, 0.094988, 0.109704, 0.122700, 0.133484, 0.141893, 0.148056, 0.152300, 0.155046, 0.156716, 0.157670, 0.158182, 0.158441}, 
/* 9  */{0.000306, 0.000675, 0.001407, 0.002770, 0.005156, 0.009079, 0.015140, 0.023943, 0.035956, 0.051360, 0.069923, 0.090944, 0.113314, 0.135683, 0.156704, 0.175267, 0.190672, 0.202684, 0.211487, 0.217549, 0.221472, 0.223857, 0.225220, 0.225952, 0.226321}, 
/* 10 */{0.000416, 0.000919, 0.001916, 0.003772, 0.007019, 0.012360, 0.020613, 0.032597, 0.048951, 0.069923, 0.095195, 0.123814, 0.154269, 0.184723, 0.213342, 0.238614, 0.259586, 0.275941, 0.287925, 0.296178, 0.301518, 0.304766, 0.306622, 0.307618, 0.308121}, 
/* 11 */{0.000542, 0.001196, 0.002492, 0.004906, 0.009129, 0.016075, 0.026809, 0.042397, 0.063667, 0.090944, 0.123814, 0.161037, 0.200647, 0.240257, 0.277480, 0.310350, 0.337626, 0.358897, 0.374484, 0.385218, 0.392164, 0.396388, 0.398802, 0.400098, 0.400752}, 
/* 12 */{0.000675, 0.001490, 0.003105, 0.006112, 0.011375, 0.020030, 0.033404, 0.052825, 0.079328, 0.113314, 0.154269, 0.200647, 0.250000, 0.299353, 0.345731, 0.386686, 0.420672, 0.447175, 0.466596, 0.479970, 0.488625, 0.493888, 0.496895, 0.498510, 0.499325}, 
/* 13 */{0.000808, 0.001784, 0.003718, 0.007319, 0.013621, 0.023984, 0.039998, 0.063253, 0.094988, 0.135683, 0.184723, 0.240257, 0.299353, 0.358449, 0.413983, 0.463023, 0.503718, 0.535453, 0.558708, 0.574723, 0.585086, 0.591387, 0.594989, 0.596922, 0.597898}, 
/* 14 */{0.000933, 0.002060, 0.004294, 0.008453, 0.015731, 0.027699, 0.046195, 0.073053, 0.109704, 0.156704, 0.213342, 0.277480, 0.345731, 0.413983, 0.478120, 0.534758, 0.581758, 0.618410, 0.645268, 0.663763, 0.675732, 0.683010, 0.687169, 0.689402, 0.690529}, 
/* 15 */{0.001044, 0.002304, 0.004802, 0.009454, 0.017594, 0.030981, 0.051667, 0.081707, 0.122700, 0.175267, 0.238614, 0.310350, 0.386686, 0.463023, 0.534758, 0.598105, 0.650673, 0.691666, 0.721706, 0.742392, 0.755778, 0.763919, 0.768570, 0.771068, 0.772329}, 
/* 16 */{0.001136, 0.002507, 0.005224, 0.010285, 0.019141, 0.033704, 0.056208, 0.088888, 0.133484, 0.190672, 0.259586, 0.337626, 0.420672, 0.503718, 0.581758, 0.650673, 0.707861, 0.752457, 0.785137, 0.807641, 0.822204, 0.831060, 0.836120, 0.838838, 0.840209}, 
/* 17 */{0.001207, 0.002665, 0.005554, 0.010933, 0.020347, 0.035827, 0.059749, 0.094488, 0.141893, 0.202684, 0.275941, 0.358897, 0.447175, 0.535453, 0.618410, 0.691666, 0.752457, 0.799862, 0.834601, 0.858523, 0.874004, 0.883417, 0.888797, 0.891685, 0.893143}, 
/* 18 */{0.001260, 0.002781, 0.005795, 0.011408, 0.021230, 0.037383, 0.062344, 0.098592, 0.148056, 0.211487, 0.287925, 0.374484, 0.466596, 0.558708, 0.645268, 0.721706, 0.785137, 0.834601, 0.870849, 0.895810, 0.911963, 0.921785, 0.927398, 0.930412, 0.931933}, 
/* 19 */{0.001296, 0.002860, 0.005961, 0.011735, 0.021839, 0.038454, 0.064131, 0.101418, 0.152300, 0.217549, 0.296178, 0.385218, 0.479970, 0.574723, 0.663763, 0.742392, 0.807641, 0.858523, 0.895810, 0.921486, 0.938102, 0.948206, 0.953980, 0.957080, 0.958645}, 
/* 20 */{0.001319, 0.002912, 0.006068, 0.011946, 0.022233, 0.039148, 0.065287, 0.103246, 0.155046, 0.221472, 0.301518, 0.392164, 0.488625, 0.585086, 0.675732, 0.755778, 0.822204, 0.874004, 0.911963, 0.938102, 0.955017, 0.965304, 0.971181, 0.974338, 0.975931}, 
/* 21 */{0.001333, 0.002943, 0.006134, 0.012075, 0.022472, 0.039569, 0.065991, 0.104358, 0.156716, 0.223857, 0.304766, 0.396388, 0.493888, 0.591387, 0.683010, 0.763919, 0.831060, 0.883417, 0.921785, 0.948206, 0.965304, 0.975700, 0.981642, 0.984832, 0.986442}, 
/* 22 */{0.001342, 0.002961, 0.006171, 0.012149, 0.022609, 0.039810, 0.066392, 0.104994, 0.157670, 0.225220, 0.306622, 0.398802, 0.496895, 0.594989, 0.687169, 0.768570, 0.836120, 0.888797, 0.927398, 0.953980, 0.971181, 0.981642, 0.987619, 0.990829, 0.992449}, 
/* 23 */{0.001346, 0.002971, 0.006191, 0.012188, 0.022682, 0.039940, 0.066608, 0.105335, 0.158182, 0.225952, 0.307618, 0.400098, 0.498510, 0.596922, 0.689402, 0.771068, 0.838838, 0.891685, 0.930412, 0.957080, 0.974338, 0.984832, 0.990829, 0.994049, 0.995674}, 
/* 24 */{0.001348, 0.002976, 0.006201, 0.012208, 0.022719, 0.040005, 0.066717, 0.105507, 0.158441, 0.226321, 0.308121, 0.400752, 0.499325, 0.597898, 0.690529, 0.772329, 0.840209, 0.893143, 0.931933, 0.958645, 0.975931, 0.986442, 0.992449, 0.995674, 0.997302} 
/*        -3        -2.75     -2.5      -2.25     -2.0      -1.75     -1.5      -1.25     -1.0      -.75      -.50      -.25      0         +.25      +.5       +.75      +1.0      +1.25     +1.5      + 1.75    +2.0      +2.25      +2.50    +2.75     +3.0 */
};

// range of table is -3 to +3 z-score in each dimension, 25 cells, .25 in each cell
const double min_z = -3.;
const double max_z = +3;
const double cell_size = 0.25;

static inline int compute_index(double z)
{
	int i;
	if(z <= min_z)
		i = 0;
	else if (z >= max_z)
		i = table_size - 1;
	else
		i = int((z - min_z) / cell_size);
	assert(i >= 0 && i < table_size);
	return i;
}

double get_bivariate_normal_cdf(double z1, double z2)
{
	int i1 = compute_index(z1);
	int i2 = compute_index(z2);
		
//	cout << "i1, i2: " << i1 << ' ' << i2 << endl;

	return table[i1][i2];

}

/*
int random_int(int range)
{
	int result;
	do {
		double x = double(rand())/RAND_MAX;
		// note! it is possible for x to end up as 1.00000...
		result = int(double(range) * x);
		// so resample if it happens
		} while (result == range);
		
//	Assert(result != range);
	return result;
//	return int(double(range) * x);
}
*/
/*
double unit_uniform_random_variable()
{
	double x;
	do {
		x = double(rand())/RAND_MAX;
		} while (x == 0.0 || x == 1.0);
	return x;
}
*/
/*
double unit_normal_random_variable()
{
	double sum = -6.0;
	for(int i = 0; i < 12; i++)
		sum += unit_uniform_random_variable();
	return sum;
}
*/
/*
double normal_random_variable(double mean, double sd)
{
	return mean + sd * unit_normal_random_variable();
}
*/
/*
;return a random variable that follows the exponential distribution
(DEFUN EXPONENTIAL-RANDOM-VARIABLE (THETA)
(* (- THETA) (LOG (UNIT-UNIFORM-RANDOM-VARIABLE)))
)

;Since EPIC times are measured in milliseconds, this delivers an integer
;Theta should be in millisecond units also!
(DEFUN EXPONENTIAL-RANDOM-INTEGER (THETA)
(TRUNCATE (EXPONENTIAL-RANDOM-VARIABLE THETA))
)

;return a random variable that follows the exponential distribution
;plus a minimum floor
(DEFUN FLOORED-EXPONENTIAL-RANDOM-VARIABLE (THETA FLOOR)
(+ (* (- THETA) (LOG (UNIT-UNIFORM-RANDOM-VARIABLE))) FLOOR)
)
*/

/*
double floored_exponential_random_variable(double theta, double floor)
{
	return floor + (-theta) * log(unit_uniform_random_variable());
}
*/

/*
;Since EPIC times are measured in milliseconds, this delivers an integer
;Theta and floor should be in millisecond units also!
(DEFUN FLOORED-EXPONENTIAL-RANDOM-INTEGER (THETA FLOOR)
(TRUNCATE (FLOORED-EXPONENTIAL-RANDOM-VARIABLE THETA FLOOR))
)

;return a gamma random variable with parameters N and THETA
(DEFUN GAMMA-RANDOM-VARIABLE(THETA N)
(LET ((SUM 0.))
(DOTIMES (I N)
  (INCF SUM (EXPONENTIAL-RANDOM-VARIABLE THETA)))
SUM
))
*/

/*
double gamma_random_variable(double theta, int n)
{
	double sum = 0.;
	for(int i = 0; i < n; i++)
		sum += exponential_random_variable(theta);
	return sum;
}
*/

/*
;Since EPIC times are measured in milliseconds, this delivers an integer
;Theta should be in millisecond units also!
(DEFUN GAMMA-RANDOM-INTEGER (THETA N)
(TRUNCATE (GAMMA-RANDOM-VARIABLE THETA N))
)

;return a log-normal random variable with median M and parameter S
(DEFUN LOG-NORMAL-RANDOM-VARIABLE(M S)
(* M (EXP (* S (UNIT-NORMAL-RANDOM-VARIABLE))))
)
*/
/*
;Since EPIC times are measured in milliseconds, this delivers an integer
;M and S should be in millisecond units also!
(DEFUN LOG-NORMAL-RANDOM-INTEGER (M S)
(TRUNCATE (LOG-NORMAL-RANDOM-VARIABLE M S))
)


;**** RANDOM VARIABLE GENERATORS ***
;this function returns a number in the open interval (0, 1)
;by excluding the zero possibility from (random n)
(DEFUN UNIT-UNIFORM-RANDOM-VARIABLE ()
(LET (X)
(LOOP
  (SETQ X (RANDOM 100000))
  (IF (> X 0) (RETURN)))
(/ (FLOAT X) 100000.)
))

(DEFUN RANDOM-UNIFORM-DEVIATION (R)
(- (* R (UNIT-UNIFORM-RANDOM-VARIABLE)) (/ R 2.))
)

;return a unit normal variate
(DEFUN UNIT-NORMAL-RANDOM-VARIABLE ()
(LET ((SUM 0.))
(DOTIMES (I 12)
  (INCF SUM (UNIT-UNIFORM-RANDOM-VARIABLE)))
(- SUM 6.0)
))

;collect n samples by evaluating dbn-expression, and count them
;in the bins defined by max-value and number-bins.  All values
;above the top bin are included in the top bin.
;assumes that the distribution has a theoretical minimum value of zero, so
;only positive values will be obtained.
;sample:
;  (test-distribution '(LOG-NORMAL-RANDOM-INTEGER 3500 .25) 5000 20 1000) ;note quote!
(defun test-distribution (dbn-expression max-value number-bins n)
(let (bins bin-index step-size x min_x max_x (p 0.) (sum 0.) (totalp 0.))

(setq bins (make-array number-bins :initial-element 0))
(setq step-size (/ max-value number-bins))
(dotimes (i n)
  (setq x (eval dbn-expression))
  (incf sum x)
  (if (or (not min_x) (< x min_x)) (setq min_x x))
  (if (or (not max_x) (> x max_x)) (setq max_x x))
  (setq bin-index (truncate (/ x step-size)))
  (if (> bin-index (1- number-bins)) (setq bin-index (1- number-bins)))
  (incf (aref bins bin-index))
)

(format t "     x    f(x)   F(x)~%")
(setq x step-size)
(dotimes (i number-bins)
  (setq p (/ (aref bins i) n))
  (incf totalp p)
  (format t "   ~4,1f  ~5,3f  ~5,3f~%" x p totalp)
  (setq x (+ x step-size))
)
(format t "~%mean = ~5,2f, min = ~5,2f, max = ~5,2f~%" 
        (/ sum n) min_x max_x)
))

*/

