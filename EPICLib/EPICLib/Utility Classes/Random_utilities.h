//
//  Random_utilities.h
//  EPICXAF
//
//  Created by David Kieras on 4/19/13.
//
//

#ifndef RANDOM_UTILITIES
#define RANDOM_UTILITIES

#include <random>
#include <vector>

// specify type of random engine here
typedef std::mt19937 Random_engine_t;
//typedef std::mt19937_64 Random_engine_t;
//typedef std::minstd_rand0 Random_engine_t;
//typedef std::knuth_b Random_engine_t;
//typedef std::default_random_engine Random_engine_t;

// accessor for the engine - same engine used for all randomization calls
Random_engine_t& get_Random_engine();

void set_random_number_generator_seed(unsigned long seed);

/* Random variable generation */
// Returns a random integer in the range 0 ... range - 1 inclusive
int random_int(int range);
// Returns true with probability p
bool biased_coin_flip(double p);


double unit_uniform_random_variable();
// return a random variable that is uniformly distributed
// on each side of the mean +/- the deviation
double uniform_random_variable(double mean, double deviation);
double unit_normal_random_variable();
// do not call this function if sd == 0
double normal_random_variable(double mean, double sd);
//double exponential_random_variable(double theta);
//double floored_exponential_random_variable(double theta, double floor);
//double gamma_random_variable(double theta, int n);
double log_normal_random_variable(double m, double s);

// This class generates a random value for an arbitrary discrete distribution described
// in the input vector of size n; it returns a value (0, n] using the probabilities in the vector.
// The values can be any probabilities, including zero, as long as they sum to one.
// The complex setup is done by the constructor and should not be repeated unnecessarily; getting a random value is then very fast.
class Discrete_distribution {
public:
    // copy the caller's vector because it will be modified during setup
    Discrete_distribution(std::vector<double> probabilities);
    
    // return a random index based on the probabilities
    int get_random_value() const;

private:
    int n;  // the number of probabilities supplied
    // these are the two tables used by the Alias Method
    std::vector<double> probability;
    std::vector<int> alias;

};

// Returns true with a probability = p
bool uniform_detection_function(double p);

// As x increases, the probability that true is returned increases according to a Normal dbn from 0. to 1.0.
bool gaussian_detection_function(double x, double mean, double sd);

// With lapse_probability, return false; else return the gaussian_detection_function result.
bool lapsed_gaussian_detection_function(double x, double mean, double sd, double lapse_probability);

// As x increases, the probability that true is returned increases according to a Normal dbn from base to 1.0.
bool based_gaussian_detection_function(double x, double base, double mean, double sd);

// As x increases, the probability that true is returned increases according to a Normal dbn from 0 to cap.
bool capped_gaussian_detection_function(double x, double cap, double mean, double sd);

// As x increases, the probability that true is returned increases according to an exponential dbn from 0 to 1.0
// the lambda parameter is assumed to be the "small" definition - 
// e.g. a value of lambda = .5 gives p(x < 1) = .3868
// cf lambda = 2 giving p(x < 1) = .8706
bool exponential_detection_function(double x, double lambda);

// As x increases, the probability that true is returned increases according to a exponential dbn from base to 1.0.
bool based_exponential_detection_function(double x, double base, double lambda);

// given two z-scores, return the cumulative probability from the bivariate normal.
// e.g. (-3, +3) returns  0.001348; (+3, +3) returns 0.997302.
// (+1, +2) and (+2, +1) return 0.822204

double get_bivariate_normal_cdf(double z1, double z2);


#endif

