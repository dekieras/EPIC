//
//  Stream_tracker_MinDistNoisy.h
//  EPICXAF
//
//  Created by David Kieras on 2/11/13.
//
//

#ifndef STREAM_TRACKER_MINDISTNOISY_H
#define STREAM_TRACKER_MINDISTNOISY_H

#include "Stream_tracker_base.h"

class Stream_tracker_MinDistNoisy : public Stream_tracker_base {
public:
	Stream_tracker_MinDistNoisy(const std::string& stream_type_name_);
	void assign_sounds_to_streams() override;
	void set_loudness_weight(double loudness_weight_) override;
	virtual void set_pitch_weight(double pitch_weight_) override;
	virtual void set_location_weight(double location_weight_) override;
	void set_theta(double theta_) override;
	
private:
	double loudness_weight;
	double pitch_weight;
    double location_weight;
	double theta;   // threshold for distance to be use rather than random assignment
    double get_total_distance(std::vector<int> assignments) const;
    // calculating distances between sounds and stream values
	double get_distance(double pitch, double loudness, double azimuth, std::shared_ptr<Stream_base> stream) const;
	double get_distance(double pitch1, double loudness1, double azimuth1, double pitch2, double loudness2, double azimuth2) const;
};

#endif 
