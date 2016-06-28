#include "Statistics.h"
#include <cmath>

using namespace std;


double Mean_accumulator::get_sample_var() const
{
	if(n < 2)
		return 0.0;
	double mean = get_mean();
	
	return (total2 / n - mean * mean);
}

double Mean_accumulator::get_sample_sd() const
{
	return sqrt(get_sample_var());
}

double Mean_accumulator::get_est_var() const
{
	return (n < 2) ? 0.0 : (double(n) / (n -1)) * get_sample_var();
}

double Mean_accumulator::get_est_sd() const
{
	return sqrt(get_est_var());
}

double Mean_accumulator::get_sdm() const
{
	return (n < 2) ? 0.0 : get_est_sd() / sqrt(n);
}

double Mean_accumulator::get_half_95_ci() const
{
	return (n < 2) ? 0.0 : 1.96 * get_sdm();
}

void Distribution_accumulator::add_counts(const Distribution_accumulator& other)
{
//    assert(n_bins == other.n_bins);
//    assert(bin_size == other.bin_size);
    for(int i = 0; i < n_bins; i++)
        bins[i] += other.bins[i];
    n += other.n;
}

double Correl_accumulator::get_r() const
{
	double numerator = n * sumxy - sumx * sumy;
	double denominator1 = n * sumx2 - sumx * sumx;
	double denominator2 = n * sumy2 - sumy * sumy;
	double r = (denominator1 > 0. && denominator2 > 0.) ?
		numerator / (sqrt(denominator1) * sqrt(denominator2)) : 0.0;
	return r;
}


