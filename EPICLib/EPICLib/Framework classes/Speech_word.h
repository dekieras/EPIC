/*
 *  Speech_word.h
 *  EPICXAF
 *
 *  Created by David Kieras on 1/11/12.
 *  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
 *
 */

#ifndef SPEECH_WORD_H
#define SPEECH_WORD_H

#include "Symbol.h"
#include "Point.h"

// A package of speech word attributes to allow speech-handling
// functions to have a fixed signature even if attributes are
// added or removed.
struct Speech_word {
	Symbol name;		// unique name for each word object
	Symbol stream_name;	// verdical stream name
	long time_stamp;
	GU::Point location;
	double pitch;       // semitones
	double loudness;    // dB - is level of physical, perceived loudness if not
	double duration;
    double level_left{0.};  // added by ear processor based on location
    double level_right{0.}; // added by ear processor based on location
	Symbol content;
	Symbol speaker_gender;
	Symbol speaker_id;
    int utterance_id;     // this integer identifies the complete utterance in the corpus
};





#endif
