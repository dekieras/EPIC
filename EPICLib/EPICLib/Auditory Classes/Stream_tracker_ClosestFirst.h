//
//  Stream_tracker_ClosestFirst.h
//  EPICXAF
//
//  Created by David Kieras on 2/11/13.
//
//

#ifndef STREAM_TRACKER_CLOSESTFIRST_H
#define STREAM_TRACKER_CLOSESTFIRST_H

#include "Stream_tracker_base.h"

class Stream_tracker_ClosestFirst : public Stream_tracker_base {
public:
	Stream_tracker_ClosestFirst(const std::string& stream_type_name_);
	void assign_sounds_to_streams() override;
    
private:
    // calculating distances between sounds and stream values
	double get_distance(double pitch, double loudness, std::shared_ptr<Stream_base> stream) const;
	double get_distance(double pitch1, double loudness1, double pitch2, double loudness2) const;
};

#endif 
