//
//  Stream_base.h
//  EPICXAF
//
//  Created by David Kieras on 1/16/13.
//
//

#ifndef STREAM_BASE_H
#define STREAM_BASE_H

#include "Statistics.h"
#include "Symbol.h"
#include <vector>

// an almost-pure interface class; uses NVI pattern
class Stream_base {
public:
	Symbol get_name() const {return name;}
    // reset the tracking data and the disappearance time
	void reset();
    // update the tracking data and reset the disappearance time
    void update(double pitch, double loudness, double azimuth, const Symbol& last_veridical_stream_, int last_utterance_id_);
    // return the number of data points that the prediction is based on
	int get_prediction_n() const
		{return do_get_prediction_n();}
    double get_predicted_pitch() const
		{return do_get_predicted_pitch();}
    double get_predicted_loudness() const
		{return do_get_predicted_loudness();}
    double get_predicted_azimuth() const
		{return do_get_predicted_azimuth();}
    Symbol get_last_veridical_stream() const
        {return last_veridical_stream;}
    int get_last_utterance_id() const
        {return last_utterance_id;}

	long get_disappearance_time() const
		{return disappearance_time;}
	void set_disappearance_time(long disappearance_time_) 
		{disappearance_time = disappearance_time_;}
protected:
    // for named constructor idiom
	Stream_base(const Symbol& name_) : name(name_), disappearance_time(0) {}    
private:
	// customization interface for derived classes to modify
 	virtual void do_reset() = 0;
    virtual void do_update(double pitch, double loudness, double azimuth) = 0;
	virtual int do_get_prediction_n() const = 0;
    virtual double do_get_predicted_pitch()const = 0;
    virtual double do_get_predicted_loudness() const = 0;
    virtual double do_get_predicted_azimuth() const = 0;

	Symbol name;
	long disappearance_time;
    Symbol last_veridical_stream;  // stores last veridical stream name from assigned word
    int last_utterance_id;  // stores last utterance id assigned
};



#endif
