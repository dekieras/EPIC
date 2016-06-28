//
//  Stream_tracker_base.cpp
//  EPICXAF
//
//  Created by David Kieras on 2/8/13.
//
//

#include "Stream_tracker_base.h"
#include "Stream_base.h"
#include "Stream_type_factory.h"
#include "Symbol_utilities.h"
#include "Standard_Symbols.h"
#include "Numeric_utilities.h"
#include "Random_utilities.h"
#include "Assert_throw.h"
#include <limits>
#include <algorithm>
#include <iostream>
#include "Output_tee_globals.h"

using std::find_if;
using std::shuffle;
using std::endl;

const Symbol Stream_assignment_c("Stream_assignment");


Stream_tracker_base::Stream_tracker_base(const std::string& stream_type_name_) :
    stream_type_name(stream_type_name_)
{
}


// clear everything to prepare for new messages
void Stream_tracker_base::clear()
{
	streams.clear();
	sounds.clear();
}

// clear the sounds currently present to prepare for new words
void Stream_tracker_base::clear_sounds()
{
	sounds.clear();
}

// populate an array of sound pointers to be processed along with the array of streams
// keep in order for the moment
void Stream_tracker_base::add_sound(std::shared_ptr<Auditory_sound> sound_ptr)
{
	sounds.push_back(sound_ptr);
}

// call for first word set to create the streams, one for each currently present sound
void Stream_tracker_base::create_initial_streams() {
	// for simplicity, just do them in order - 1st sound present -> streams[0]
	// set stream assignment for each word, and update each stream with the assigned word - we don't need identity of current word for this process
	int stream_index = 0;
	for(auto it = sounds.begin(); it != sounds.end(); ++it, stream_index++) {
//        std::shared_ptr<Stream_base> new_stream = Two_point_linear_stream::create(concatenate_to_Symbol("Stream", stream_index+1));
//        std::shared_ptr<Stream_base> new_stream = Averaging_stream::create(concatenate_to_Symbol("Stream", stream_index+1));
        std::shared_ptr<Stream_base> new_stream = create_stream(stream_type_name, (concatenate_to_Symbol("Stream", stream_index+1)));
        streams.push_back(new_stream);
        }
    Assert(streams.size() == sounds.size());
	// assign sounds in order to streams
	// randomize them so that the ordering function won't default to "good" case if attributes are tied
//	std::random_shuffle(sounds.begin(), sounds.end());
	shuffle(sounds.begin(), sounds.end(), get_Random_engine());
	// put the sounds in order by the ordering function
	// assign to streams in that order; update stream object with attributes of the word ...
	for(int sound_index = 0; sound_index < sounds.size(); sound_index++) {
		sounds[sound_index]->set_property(Stream_assignment_c, streams[sound_index]->get_name());
		// update stream objects to reflect the current word
		streams[sound_index]->update(sounds[sound_index]->get_pitch(), sounds[sound_index]->get_loudness(),
            sounds[sound_index]->get_location().x, // should be the azimuth
            sounds[sound_index]->get_stream(), sounds[sound_index]->get_utterance_id());
 /*       Normal_out << "Initial stream: " << streams[sound_index]->get_name() << ' '
            << streams[sound_index]->get_last_veridical_stream() << ' '
            << streams[sound_index]->get_last_utterance_id() << ' '
            << sounds[sound_index]->get_content() << endl; */
		}
}




// set the disappearance time for the named stream
void Stream_tracker_base::set_stream_disappearance_time(const Symbol& stream_name, long disappearance_time)
{
	auto it = find_if(streams.begin(), streams.end(),
		[stream_name](std::shared_ptr<Stream_base> the_stream){return the_stream->get_name() == stream_name;});
    (*it)->set_disappearance_time(disappearance_time);
}

void Stream_tracker_base::delete_stream(const Symbol& stream_name)
{
	auto it = find_if(streams.begin(), streams.end(),
		[stream_name](std::shared_ptr<Stream_base> the_stream){return the_stream->get_name() == stream_name;});
	streams.erase(it);
}

