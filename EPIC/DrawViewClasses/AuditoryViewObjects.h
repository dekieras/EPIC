/*
 *  AuditoryViewObject.h
 *  EPICX
 *
 *  Created by David Kieras on 6/6/08.
 *  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
 *
 */
#ifndef AUDITORYVIEWOBJECTS_H
#define AUDITORYVIEWOBJECTS_H
#include "EPICLib/Symbol.h"
#include <map>
#include <ApplicationServices/ApplicationServices.h>


// a stream has a name, location, size, and properties
class AuditoryViewStream {
public:
	AuditoryViewStream();
/* disabled 5/9/12
	AuditoryViewStream(const Symbol& name, CGPoint location_, CGSize size_);

	AuditoryViewStream(const Symbol& name_, float x, float y, float width, float height);
*/
	AuditoryViewStream(const Symbol& name, double pitch, double loudness, CGPoint location_);
	AuditoryViewStream(const Symbol& name_, double pitch, double loudness, float x, float y);
	void drawself(CGContextRef ctx, float scale) const;
	void set_location(CGPoint location_);
	void set_location(float x, float y);
	void set_pitch(double pitch_);
	void set_loudness(double loudness_);
	void set_size(CGSize size_);
	void set_size(float width, float height);
	void set_property(const Symbol& propname, const Symbol& propvalue);
	
private:	
	Symbol name;
	double pitch;
	double loudness;
	CGPoint location;
	CGSize size;
	typedef std::map<Symbol, Symbol> properties_t;
	properties_t properties;
};

// a sound has a name, stream, timbre, loudness, and properties
class AuditoryViewSound {
public:
	AuditoryViewSound();

	AuditoryViewSound(const Symbol& sound_name, const Symbol& stream_name, long time_stamp, float x, float y, const Symbol& timbre, double loudness);
	AuditoryViewSound(const Symbol& sound_name, const Symbol& stream_name, long time_stamp, float x, float y, const Symbol& timbre, const Symbol& content, const Symbol& speaker_gender, const Symbol& speaker_id, double pitch, double loudness);
	void drawself(CGContextRef ctx, float scale) const;
	void set_disappearing()
		{disappearing = true;}
	void set_property(const Symbol& propname, const Symbol& propvalue);
	bool operator< (const AuditoryViewSound& rhs) const
		{return time_stamp < rhs.time_stamp;}
	
private:	
	Symbol name;
	Symbol stream;
	long time_stamp;
	CGPoint location;
	Symbol timbre;
	Symbol content;
	Symbol speaker_gender;
	Symbol speaker_id;
	double pitch;
	double loudness;
	bool disappearing;	// whether the sound is fading or not
	typedef std::map<Symbol, Symbol> properties_t;
	properties_t properties;
};

#endif
