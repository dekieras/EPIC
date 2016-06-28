#ifndef STATISTICS_H
#define STATISTICS_H

#include <vector>
#include <limits>

/*
Use these classes to accumulate running statistics values easily.
reset() - zero the internal variables
update() - add a new data value, updating current average
get_n, mean - return the current values
*/

class Mean_accumulator{
public:
	Mean_accumulator()
		{
			reset();
		}
	void reset()
		{
			n = 0;
			total = 0.0;
			total2 = 0.0;
		}
	
	long get_n() const
		{return n;}
	double get_mean() const
		{return (n > 0) ? (total / n) : 0.0;}
	long get_total() const
		{return total;}
	double get_sample_var() const;
	double get_sample_sd() const;
	double get_est_var() const;
	double get_est_sd() const;
	double get_sdm() const;
	double get_half_95_ci() const;
			
	void update(double x)
		{
			total += x;
			total2 += x*x;
			n++;
		}
			
private:
	long n;
	double total;
	double total2;
};

// accumulates the total number of updates, 
// and the proportion of those updates called with a true argument
class Proportion_accumulator{
public:
	Proportion_accumulator()
		{
			reset();
		}
	void reset()
		{
			count = 0;
			n = 0;
		}
	
	long get_count() const
		{return count;}
	long get_n() const
		{return n;}
	double get_proportion() const
		{return (n > 0) ? double(count) / n : 0.;}
			
	void update(bool count_it)
		{
			n++;
			if(count_it)
				count++;
		}
			
private:
	long count;
	long n;
};

// Accumulate data values into bins and provide the proportion in each bin.
// Initialize with the number of bins and the size in each bin.
// The first bin starts at 0.  Values too big or too small are accumulated
// in the smallest or largest bin.
// Output for( i = 0; i < n_bins; i++) then i*bin_size is the upper bound on values in that bin
// so for bins 0, 25, 50, count for first is number < 0; second is number >= 0 and < 25, third is >= 25 and < 50
class Distribution_accumulator{
public:
	Distribution_accumulator(int n_bins_, double bin_size_) :
        n_bins(n_bins_), bin_size(bin_size_),
        min_value(std::numeric_limits<double>::max()),
        max_value(std::numeric_limits<double>::min())
		{
			reset();
		}
	void reset()
		{
			n = 0;
            bins.clear();
            bins.resize(n_bins);
		}

    // These accessor provide individual bins, or the whole distribution
    int get_n_bins() const
        {return n_bins;}
    double get_bin_size() const
        {return bin_size;}
	long get_n() const
		{return n;}
	double get_min() const
		{return min_value;}
	double get_max() const
		{return max_value;}
    int get_bin_count(int bin) const
        {
            return bins[bin];
        }
    double get_bin_proportion(int bin) const
        {
            return (n) ? double(bins[bin]) / n : 0.0;
        }
    std::vector<double> get_distribution()
        {
            std::vector<double> result(n_bins);
            for(int i = 0; i < n_bins; i++)
                result[i] = (n) ? double(bins[i]) / n : 0.0;
            return result;
        }
			
	void update(double x)
		{
			n++;
            if(x < 0.) {
                bins[0]++;
                return;
                }
            int i = int(x / bin_size);
            if(i < 0)
                bins[0]++;
            else if(i >= n_bins)
                bins[n_bins - 1]++;
            else
                bins[i]++;
            if(x < min_value)
                min_value = x;
            if(x > max_value)
                max_value = x;
		}
    // add the other's counts to this accumulator
    // (n_bins, bin_size must be the same)
    void add_counts(const Distribution_accumulator& other);
			
private:
    int n_bins;
    double bin_size;
	long n;
    double min_value;
    double max_value;
    std::vector<int> bins;
};

// Accumulate data for a correlation coeficient and regression line
// Like the others, this class uses the one-pass approach which
// can be numerically unreliable under some conditions
class Correl_accumulator {
public:
	Correl_accumulator()
		{reset();}
	void reset()
		{
			n = 0;
			sumx = 0;
			sumy = 0;
			sumxy = 0;
			sumx2 = 0;
			sumy2 = 0;
		}

	void update(double x, double y)
		{
			n++;
			sumx += x;
			sumy += y;
			sumxy += x*y;
			sumx2 += x*x;
			sumy2 += y*y;
		}

	int get_n() const
		{return n;}

	double get_r() const;

	double get_slope() const
		{
			double numerator = n * sumxy - sumx * sumy;
			double denominator = n * sumx2 - sumx * sumx;
			return (denominator > 0.) ? numerator / denominator : 0.0;
		}

	double get_intercept() const
		{
			return (n) ? (sumy - get_slope() * sumx) / n : 0.0;
		}

	double get_rsq() const
		{
			double r = get_r();
			return r*r;
		}

private:
	int n;
	double sumx;
	double sumy;
	double sumxy;
	double sumx2;
	double sumy2;
};

// Give this class object a series of predicted and observed values,
// and then get the goodness-of-fit metrics for them
// using regression fit and simple average absolute error
class PredObs_accumulator {
public:
	PredObs_accumulator() : sum_error_prop(0.), total_error(0.)
		{}
	void reset()
		{
			corr.reset();
			sum_error_prop = 0;
			total_error = 0;
		}

	void update(double predicted, double observed);
    
	int get_n() const
		{return corr.get_n();}

	double get_rsq() const
		{return corr.get_rsq();}

	double get_slope() const
		{return corr.get_slope();}

	double get_intercept() const
		{return corr.get_intercept();}

	double get_avg_abs_error() const
		{return sum_error_prop / corr.get_n() * 100.;}

	double get_rmse() const;

private:
	Correl_accumulator corr;
	double sum_error_prop;
    double total_error;
};



#endif
