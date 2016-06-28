//
//  Stream_holders.cpp
//  EPICXAF
//
//  Created by David Kieras on 1/23/13.
//
//

#include "Stream_types.h"
#include "Assert_throw.h"

/* ----------- Two_point_linear_stream ----------- */

void Two_point_linear_stream::do_reset()
{
    pitches.clear();
    loudnesses.clear();
    azimuths.clear();
}
void Two_point_linear_stream::do_update(double pitch, double loudness, double azimuth)
{
    update_with_value(pitches, pitch);
    update_with_value(loudnesses, loudness);
    update_with_value(azimuths, azimuth);
}

int Two_point_linear_stream::do_get_prediction_n() const
{
	Assert(pitches.size() == loudnesses.size());
	return int(pitches.size());
}

// make a linear prediction based on last two data points
// equal to last data point if only one
double Two_point_linear_stream::do_get_predicted_pitch() const
{
    return predict_next_value(pitches);
}

double Two_point_linear_stream::do_get_predicted_loudness() const
{
    return predict_next_value(loudnesses);
}
double Two_point_linear_stream::do_get_predicted_azimuth() const
{
    return predict_next_value(azimuths);
}


// predict the next value from the position of the previous value and the slope of the last two
double Two_point_linear_stream::predict_next_value(const std::vector<double>& v) const
{
    Assert(v.size() > 0);
    if(v.size() == 1)
        return v[0];
    return (v[1] - v[0]) + v[1];
}

// add the new value; if already have two, discard the oldest one
void Two_point_linear_stream::update_with_value(std::vector<double>& v, double x)
{
    if(v.size() == 2) {
        v[0] = v[1];  // discard oldest value
        v[1] = x;
        }
    else
        v.push_back(x);
}

/* ----------- Averaging_stream ----------- */

void Averaging_stream::do_reset()
{
    n = 0;
    pitch_sum = 0.;
    loudness_sum = 0.;
    azimuth_sum = 0.;
}

void Averaging_stream::do_update(double pitch, double loudness, double azimuth)
{
    n++;
    pitch_sum += pitch;
    loudness_sum +=loudness;
    azimuth_sum +=azimuth;
}

int Averaging_stream::do_get_prediction_n() const 
{
	return n;
}

// predicted value is current average
double Averaging_stream::do_get_predicted_pitch() const
{
    return pitch_sum / n;
}

double Averaging_stream::do_get_predicted_loudness() const
{
    return loudness_sum / n;
}

double Averaging_stream::do_get_predicted_azimuth() const
{
    return azimuth_sum / n;
}


