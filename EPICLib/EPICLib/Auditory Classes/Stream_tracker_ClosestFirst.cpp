//
//  Stream_tracker_AvgMinDist.cpp
//  EPICXAF
//
//  Created by David Kieras on 1/16/13.
//
//

#include "Stream_tracker_ClosestFirst.h"
#include "Stream_base.h"
#include "Stream_type_factory.h"
#include "Symbol_utilities.h"
#include "Standard_Symbols.h"
#include "Numeric_utilities.h"
#include "Random_utilities.h"
#include "Assert_throw.h"
#include "Output_tee_globals.h"
#include <iostream>
#include <limits>
#include <algorithm>

using std::endl;
using std::shuffle;


const Symbol Stream_assignment_c("Stream_assignment");

Stream_tracker_ClosestFirst::Stream_tracker_ClosestFirst(const std::string& stream_type_name_)
    : Stream_tracker_base(stream_type_name_)
{
}

// call for each word set after the first, to assign the words to the streams
void Stream_tracker_ClosestFirst::assign_sounds_to_streams()
{
    // We need to assign each word to one stream so that each stream has one word.
    // Veridical stream_ID in word is ignored
    
    // this work does not require distingishing "this_sound" from the other sounds
    // randomize them so that the assignment won't default to "good" case if attributes are tied
//    std::random_shuffle(sounds.begin(), sounds.end());
    shuffle(sounds.begin(), sounds.end(), get_Random_engine());
    // calculate simple distance of each sound to each stream in a 2-d array
    const int max_n_sounds_present = 4;
    double distances[max_n_sounds_present][max_n_sounds_present]; // first index is sound index, second is stream index
    // n_sounds_present should also be the number of streams present
    Assert(sounds.size() == streams.size());
    for(int sound_index = 0; sound_index < sounds.size(); sound_index++)
        for(int stream_index = 0; stream_index < sounds.size(); stream_index++) {
            double distance = get_distance(
                sounds[sound_index]->get_pitch(), sounds[sound_index]->get_loudness(),
                streams[stream_index]);
            distances[sound_index][stream_index] = distance;
        }

    // assign each word to a stream, using closest distance first, keeping track of which streams have already been used
    bool sound_used[max_n_sounds_present] = {false, false, false, false};
    bool stream_used[max_n_sounds_present] = {false, false, false, false};
    int n_assignments_made = 0;	// as a check
    // outer loop is to do an assignment - sounds.size() are required
    for(int assignments = 0; assignments < sounds.size(); assignments++) {
        double closest_distance = std::numeric_limits<double>::max(); // huge ugly kludge value   numeric_limits<double>::max()  <limits>
        int closest_sound = -1; // for debugging
        int closest_stream = -1;
        // find the smallest unused distance, keeping track of sound and stream index for it
        for(int sound_index = 0; sound_index < sounds.size(); sound_index++) {
            if(sound_used[sound_index]) // skip if this sound has been used
                continue;
            for(int stream_index = 0; stream_index < sounds.size(); stream_index++) {
                if(stream_used[stream_index]) // skip if this stream has been used
                    continue;
                if(distances[sound_index][stream_index] < closest_distance) {
                    closest_distance = distances[sound_index][stream_index];
                    closest_sound = sound_index;
                    closest_stream = stream_index;
                    }
                }
            }
        // closest things should now be determined; assign sound to stream and update
		if(closest_sound == -1 || closest_stream == -1) {
			Normal_out << closest_sound << ' ' << closest_stream << ' ' << closest_distance << endl;
			for(int i = 0; i < sounds.size(); i++) {
				Normal_out << sound_used[i] << ' ' << stream_used[i] << ' ';
				}
			Normal_out << endl; 
			for(int i = 0; i < sounds.size(); i++)
				for(int j = 0; j < sounds.size(); j++){
					Normal_out << distances[i][j] << ' ';
					}
			Normal_out << endl;
			}
			
				
		Assert(closest_sound != -1);
		Assert(closest_stream != -1);
//        if(!(closest_sound >= 0 && closest_stream >= 0))
//            Assert(closest_sound >= 0 && closest_stream >= 0);
        sounds[closest_sound]->set_property(Stream_assignment_c, streams[closest_stream]->get_name());
        streams[closest_stream]->update(sounds[closest_sound]->get_pitch(), sounds[closest_sound]->get_loudness(),
            sounds[closest_sound]->get_location().x, // should be the azimuth
            sounds[closest_sound]->get_stream(), sounds[closest_sound]->get_utterance_id());
        // mark this sound and stream as used
        sound_used[closest_sound] = true;
        stream_used[closest_stream] = true;
        n_assignments_made++;
        }
        // did everybody get assigned?
        Assert(n_assignments_made == sounds.size());

}

// get distance of supplied sound from specified stream
double Stream_tracker_ClosestFirst::get_distance(double pitch, double loudness, std::shared_ptr<Stream_base> stream) const
{
//	return get_distance(pitch, loudness, stream.get_pitch_mean(), stream.get_loudness_mean());
	return get_distance(pitch, loudness, stream->get_predicted_pitch(), stream->get_predicted_loudness());
}


// simple calculation of distance between two sounds
double Stream_tracker_ClosestFirst::get_distance(double pitch1, double loudness1, double pitch2, double loudness2) const
{
	double pitch_diff = pitch1 - pitch2; // distance in semitones
	double loudness_diff = loudness1 - loudness2;

//	pitch_distance = pitch_distance * 0.75;
//	loudness_distance = loudness_distance * 0.25;
//	pitch_distance = pitch_distance * 0.5;
//	loudness_distance = loudness_distance * 0.5;
//	pitch_distance = pitch_distance * 0.25;
//	loudness_distance = loudness_distance * 0.75;
//	pitch_distance = pitch_distance * 0.1;
//	loudness_distance = loudness_distance * 0.9;
	pitch_diff = pitch_diff * 0.0;
	loudness_diff = loudness_diff * 1.0;

	double sum = (pitch_diff * pitch_diff + loudness_diff * loudness_diff);
	return (sum > 0.0) ? sqrt(sum) : 0.0;  // avoid computing nan from a sqrt
}


