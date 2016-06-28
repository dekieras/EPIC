//
//  Stream_base.cpp
//  EPICXAF
//
//  Created by David Kieras on 1/16/13.
//
//

#include "Stream_base.h"

void Stream_base::reset()
{
    disappearance_time = 0;
    last_utterance_id = 0;
    do_reset();
}

void Stream_base::update(double pitch, double loudness, double azimuth, const Symbol& last_veridical_stream_, int last_utterance_id_)
{
	disappearance_time = 0;
    last_veridical_stream = last_veridical_stream_;
    last_utterance_id = last_utterance_id_;
    do_update(pitch, loudness, azimuth);
}


