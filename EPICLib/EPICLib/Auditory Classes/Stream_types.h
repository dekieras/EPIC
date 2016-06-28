//
//  Stream_types.h
//  EPICXAF
//
//  Created by David Kieras on 1/23/13.
//
//

#ifndef STREAM_TYPES_H
#define STREAM_TYPES_H

#include "Stream_base.h"
#include <memory>

class Two_point_linear_stream : public Stream_base {
public:
	static std::shared_ptr<Stream_base> create(const Symbol& name_)
		{return std::shared_ptr<Stream_base>(new Two_point_linear_stream(name_));}

private:
    Two_point_linear_stream(const Symbol& name_) : Stream_base(name_){}

    // customization interface
 	virtual void do_reset() override;
    virtual void do_update(double pitch, double loudness, double azimuth) override;
	virtual int do_get_prediction_n() const override;
    virtual double do_get_predicted_pitch()const override;
    virtual double do_get_predicted_loudness() const override;
    virtual double do_get_predicted_azimuth() const override;


    std::vector<double> loudnesses;
    std::vector<double> pitches;
    std::vector<double> azimuths;

    double predict_next_value(const std::vector<double>&) const;
    void update_with_value(std::vector<double>&, double x);
};


class Averaging_stream : public Stream_base {
public:
    static std::shared_ptr<Stream_base> create(const Symbol& name_)
        {return std::shared_ptr<Stream_base>(new Averaging_stream(name_));}
private:
	Averaging_stream(const Symbol& name_) : Stream_base(name_),
    n(0), pitch_sum(0.), loudness_sum(0.), azimuth_sum(0.0)
    {}

    // customization interface
 	virtual void do_reset() override;
    virtual void do_update(double pitch, double loudness, double azimuth) override;
	virtual int do_get_prediction_n() const override;
    virtual double do_get_predicted_pitch()const override;
    virtual double do_get_predicted_loudness() const override;
    virtual double do_get_predicted_azimuth() const override;

	int n;
    double pitch_sum;
    double loudness_sum;
    double azimuth_sum;
};


#endif 
