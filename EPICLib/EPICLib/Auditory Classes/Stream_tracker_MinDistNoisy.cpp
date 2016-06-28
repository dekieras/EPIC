//
//  Stream_tracker_MinDist.cpp
//  EPICXAF
//
//  Created by David Kieras on 1/16/13.
//
//

#include "Stream_tracker_MinDistNoisy.h"
#include "Stream_base.h"
#include "Stream_type_factory.h"
#include "Ear_processor.h" // for parametric services
#include "Symbol_utilities.h"
#include "Standard_Symbols.h"
#include "Numeric_utilities.h"
#include "Random_utilities.h"
#include "Assert_throw.h"
#include "Output_tee_globals.h"
#include <limits>
#include <algorithm>

using std::vector;
using std::endl;
using std::shuffle;
using std::next_permutation;

const Symbol Stream_assignment_c("Stream_assignment");

Stream_tracker_MinDistNoisy::Stream_tracker_MinDistNoisy(const std::string& stream_type_name_)
    : Stream_tracker_base(stream_type_name_), loudness_weight(0.2), pitch_weight(0.8),
	location_weight(0.8), theta(0.35)
{
}

void Stream_tracker_MinDistNoisy::set_loudness_weight(double loudness_weight_)
{
	loudness_weight = loudness_weight_;
}

void Stream_tracker_MinDistNoisy::set_pitch_weight(double pitch_weight_)
{
	pitch_weight = pitch_weight_;
}

void Stream_tracker_MinDistNoisy::set_location_weight(double location_weight_)
{
	location_weight = location_weight_;
}

void Stream_tracker_MinDistNoisy::set_theta(double theta_)
{
	theta = theta_;
}



// call for each word set after the first, to assign the words to the streams
void Stream_tracker_MinDistNoisy::assign_sounds_to_streams()
{
    // We need to assign each word to one stream so that each stream has one word.
    // Veridical stream_ID in word is ignored
    
    // this work does not require distingishing "this_sound" from the other sounds
    // randomize them so that the assignment won't default to "good" case if attributes are tied
//     std::random_shuffle(sounds.begin(), sounds.end());
    shuffle(sounds.begin(), sounds.end(), get_Random_engine());
    // calculate simple distance of each sound to each stream in a 2-d array
//    const int max_n_sounds_present = 4;
    // we will find the minimum total distance assignment of sounds to streams by
    // generating all permutations of sound-to-stream assignment,
    // and which one produces the minimum total.
    
    // the subscript of this vector represents the stream index;
    // the values in a cell represent the sound assigned to that stream.
    // start with the cells containing the sound indicies in sorted order.
    Assert(sounds.size() == streams.size());
    vector<int> assignments;
    for(int sound_index = 0; sound_index < sounds.size(); sound_index++)
        assignments.push_back(sound_index);
    Assert(assignments.size() == streams.size());
    // get the total distance of the starting arrangement, and save that arrangement
    double min_total_dist = get_total_distance(assignments);
    // we will be tracking the maximum distance found as well
    double max_total_dist = get_total_distance(assignments);
    vector<int> min_total_distance_assignment(assignments);
    // this loop continues until the next_permutation is the same as the starting in-order assignment
    while(next_permutation(assignments.begin(), assignments.end())) {
        double total_dist = get_total_distance(assignments);
        if(total_dist > max_total_dist) {
            max_total_dist = total_dist;
            }
        if(total_dist < min_total_dist) {
            min_total_dist = total_dist;
            min_total_distance_assignment = assignments;
            }
        }
    // at this point, min_total_dist and min_total_distance_assignments should have the final minimum values
    // and max_total_dist will have the maximum value obtained in the assignments
    // does the difference between max_total_dist and min_total_dist exceed the threshold?
    // if not, then pick from a random shuffle of the assignment
    Assert(max_total_dist >= min_total_dist);
    if((max_total_dist - min_total_dist) <= theta) {
		// for two streams, interchange the two cells
//		Assert(min_total_distance_assignment.size() == 2);
//		int temp = min_total_distance_assignment[0];
//		min_total_distance_assignment[0] = min_total_distance_assignment[1];
//		min_total_distance_assignment[1] = temp;
        // below is supposed to be correct 052114
 //       random_shuffle(min_total_distance_assignment.begin(), min_total_distance_assignment.end());
        shuffle(min_total_distance_assignment.begin(), min_total_distance_assignment.end(), get_Random_engine());
        // save the best assignment
 //       assignments = min_total_distance_assignment;
        // randomly shuffle the assignment until a different value is obtained
 //      do {
//           random_shuffle(min_total_distance_assignment.begin(), min_total_distance_assignment.end());
//           shuffle(min_total_distance_assignment.begin(), min_total_distance_assignment.end(), get_Random_engine());
//           } while (assignments == min_total_distance_assignment);
        }
// obsolete 10/27/14
/*    else if(biased_coin_flip(alpha)) {
// 		Assert(min_total_distance_assignment.size() == 2);
//		int temp = min_total_distance_assignment[0];
//		min_total_distance_assignment[0] = min_total_distance_assignment[1];
//		min_total_distance_assignment[1] = temp;
//      random_shuffle(min_total_distance_assignment.begin(), min_total_distance_assignment.end());
//      shuffle(min_total_distance_assignment.begin(), min_total_distance_assignment.end(), get_Random_engine());
        // below was supposed to be correct 052114
        // randomly shuffle the assignment until a different value is obtained
        // save the best assignment
        assignments = min_total_distance_assignment;
        do {
//            random_shuffle(min_total_distance_assignment.begin(), min_total_distance_assignment.end());
            shuffle(min_total_distance_assignment.begin(), min_total_distance_assignment.end(), get_Random_engine());
           } while (assignments == min_total_distance_assignment);
        }
*/

    // go through the final assignment and assign the sounds to the corresponding streams
    for(int stream_index = 0; stream_index < min_total_distance_assignment.size(); stream_index++) {
        int sound_index = min_total_distance_assignment[stream_index];
        sounds[sound_index]->set_property(Stream_assignment_c, streams[stream_index]->get_name());
//        if(streams[stream_index]->get_last_utterance_id() != sounds[sound_index]->get_utterance_id()) {
/*       if(streams[stream_index]->get_last_veridical_stream() != sounds[sound_index]->get_stream()) {
            Normal_out << "Stream switch: Stream, utterance for "
                << streams[stream_index]->get_name() << " was "
                << streams[stream_index]->get_last_veridical_stream() << ' ' << streams[stream_index]->get_last_utterance_id()
                << " now " << sounds[sound_index]->get_stream() << ' ' << sounds[sound_index]->get_utterance_id()
                << " for " << sounds[sound_index]->get_content() << endl;
            } */
        streams[stream_index]->update(sounds[sound_index]->get_pitch(), sounds[sound_index]->get_loudness(),
            sounds[sound_index]->get_location().x, // should return the azimuth
            sounds[sound_index]->get_stream(), sounds[sound_index]->get_utterance_id());
 //       Normal_out << "sound " << sound_index << ' ' << sounds[sound_index]->get_utterance_id() << " assigned to stream " << stream_index << endl;
        }
/*

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
    // array has the distance for each sound/stream pair.  Pick the assignment that has the minimum total distance.
    // This version only works for two stream case.
    Assert(sounds.size() == 2);
    double dist11 = distances[0][0] + distances[1][1]; // strm1, sound1; strm2, sound2
    double dist12 = distances[0][1] + distances[1][0]; // strm1, sound2; strm2, sound1
    if(dist11 <= dist12) {
        sounds[0]->set_property(Stream_assignment_c, streams[0]->get_name());
        streams[0]->update(sounds[0]->get_pitch(), sounds[0]->get_loudness());
        sounds[1]->set_property(Stream_assignment_c, streams[1]->get_name());
        streams[1]->update(sounds[1]->get_pitch(), sounds[1]->get_loudness());
        }
    else {
        sounds[0]->set_property(Stream_assignment_c, streams[1]->get_name());
        streams[1]->update(sounds[0]->get_pitch(), sounds[0]->get_loudness());
        sounds[1]->set_property(Stream_assignment_c, streams[0]->get_name());
        streams[0]->update(sounds[1]->get_pitch(), sounds[1]->get_loudness());
        }
*/
}

// the subscript of this vector represents the stream index;
// the values in a cell represent the sound assigned to that stream.
double Stream_tracker_MinDistNoisy::get_total_distance(vector<int> assignments) const
{
    double total = 0;
    for(int stream_index = 0; stream_index < assignments.size(); stream_index++) {
        int sound_index = assignments[stream_index];
        total += get_distance(
                sounds[sound_index]->get_pitch(), sounds[sound_index]->get_loudness(),
                sounds[sound_index]->get_location().x, // should be the azimuth
                streams[stream_index]);
        }
    return total;
}


// get distance of supplied sound from specified stream
double Stream_tracker_MinDistNoisy::get_distance(double pitch, double loudness, double azimuth, std::shared_ptr<Stream_base> stream) const
{
//	return get_distance(pitch, loudness, stream.get_pitch_mean(), stream.get_loudness_mean());
	return get_distance(pitch, loudness, azimuth, stream->get_predicted_pitch(), stream->get_predicted_loudness(), stream->get_predicted_azimuth());
}


// simple calculation of distance between a sound and stream characterisitics
double Stream_tracker_MinDistNoisy::get_distance(double sound_pitch, double sound_loudness, double sound_azimuth,
    double stream_pitch, double stream_loudness, double stream_azimuth) const
{
	double pitch_diff = fabs(sound_pitch - stream_pitch); // difference in semitones  range from +/- 0 to around 13 between sexes, could be more
    pitch_diff = (pitch_diff > 4) ? 4. : pitch_diff;	// cap at 4
	double loudness_diff = fabs(sound_loudness - stream_loudness); // difference in db, range from +/- 0 to 18 or so
//    Normal_out << "tracker loudness: " << loudness1 << ' ' << loudness2 << ' ' << loudness_diff << endl;

/*    // convert the sound azimuth to a z-score using the stream mean azimuth and the sd for that mean
    double azimuth_diff = fabs((sound_azimuth - stream_azimuth) / Ear_processor::get_auditory_location_sd(stream_azimuth));
    // don't use this; no need for that complex a function 6/1/16
*/
    // use scaling function for azimuth difference defined in Ear_processor
    double azimuth_diff = Ear_processor::get_scaled_perceived_azimuth_difference(sound_azimuth, stream_azimuth);


	pitch_diff = pitch_diff * pitch_weight;
	loudness_diff = loudness_diff * loudness_weight;    
    azimuth_diff = azimuth_diff * location_weight;

	
	double sum = (pitch_diff * pitch_diff + loudness_diff * loudness_diff + azimuth_diff * azimuth_diff);
//	if(azimuth1 != azimuth2)
//		Normal_out << pitch_diff << ' ' << loudness_diff << ' ' << azimuth_diff << ' ' << ((sum > 0.0) ? sqrt(sum) : 0.0) << endl;
	return (sum > 0.0) ? sqrt(sum) : 0.0;  // avoid computing nan from a sqrt
}


