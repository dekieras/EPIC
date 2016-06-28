//
//  Stream_tracker_MinDist.h
//  EPICXAF
//
//  Created by David Kieras on 2/11/13.
//
//

#ifndef STREAM_TRACKER_MINDIST_H
#define STREAM_TRACKER_MINDIST_H

#include "Stream_tracker_base.h"

class Stream_tracker_MinDist : public Stream_tracker_base {
public:
	Stream_tracker_MinDist(const std::string& stream_type_name_);
	void assign_sounds_to_streams() override;
	virtual void set_lambda(double lambda_);
    
private:
	double lambda;
    double get_total_distance(std::vector<int> assignments) const;
    // calculating distances between sounds and stream values
	double get_distance(double pitch, double loudness, std::shared_ptr<Stream_base> stream) const;
	double get_distance(double pitch1, double loudness1, double pitch2, double loudness2) const;
};

#endif 
