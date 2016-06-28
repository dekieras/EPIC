//
//  Stream_tracker_base.h
//  EPICXAF
//
//  Created by David Kieras on 2/8/13.
//
//

#ifndef STREAM_TRACKER_BASE_H
#define STREAM_TRACKER_BASE_H

#include "Stream_base.h"
#include "Auditory_physical_store.h"
#include <vector>
#include <memory>
#include <string>

class Stream_tracker_base {
public:
	Stream_tracker_base(const std::string& stream_type_name_);
    virtual ~Stream_tracker_base() {}
    // clear everything to prepare for new messages
    void clear();
	// clear the sounds currently present to prepare for new words
	void clear_sounds();
    // add a sound - all sounds must be added before streams can be created or assigned.
    void add_sound(std::shared_ptr<Auditory_sound> sound_ptr);
	// return the number of sounds present
	int get_number_of_sounds_present() const
        {return int(sounds.size());}
	// call for first word set to create the streams, one for each currently present sound, using specified stream_type, and simply assigning in order
	virtual void create_initial_streams();
	// true if streams have been started (after first words) from call to create_initial_streams.
	bool are_streams_started() const
		{return !streams.empty();}
	// call for each word set after the first, to assign the words to the streams
	virtual void assign_sounds_to_streams() = 0;
    // set the disappearance time for the named stream
    void set_stream_disappearance_time(const Symbol& stream_name, long disappearance_time);
	// remove the stream of that name
    void delete_stream(const Symbol& stream_name);
    // public accessor for streams
	const std::vector<std::shared_ptr<Stream_base>>& get_streams() const
		{return streams;}
	virtual void set_loudness_weight(double loudness_weight_) {}
	virtual void set_pitch_weight(double pitch_weight_) {}
	virtual void set_location_weight(double location_weight_) {}
	virtual void set_theta(double theta_) {}
	
protected:
    // These member variables are protected instead of private because
    // 1. Derived classes will always use them heavily, making direct access especially convenient.
    // 2. They need to be accessed by public interface of this class, making definition here convenient.
    // 3. This class does not impose any constraints on them requiring controlled access.
    static const int max_n_sounds_present = 4;
	std::vector<std::shared_ptr<Stream_base>> streams;
    std::vector<std::shared_ptr<Auditory_sound>> sounds;

    // services for derived classes
    const std::string& get_stream_type_name()
        {return stream_type_name;}   // for use in Stream_type_factory;
    
private:
     std::string stream_type_name;   // for use in Stream_type_factory;

};



#endif

