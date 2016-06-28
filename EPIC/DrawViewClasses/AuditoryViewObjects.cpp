/*
 *  AuditoryViewObject.cpp
 *  EPICX
 *
 *  Created by David Kieras on 6/6/08.
 *  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
 *
 */

#include "AuditoryViewObjects.h"
#include "EPICLib/Standard_Symbols.h"
#include <string>
#include <sstream>
#include <iomanip>

using std::string;
using std::ostringstream;
using std::ostringstream;	using std::fixed;	using std::setprecision;



/*** AuditoryViewStream ***/

AuditoryViewStream::AuditoryViewStream() :
	name("default initialized AuditoryViewStream"), pitch(0.), loudness(0.), location(CGPointZero), size(CGSizeZero)
{
}
/*
AuditoryViewStream::AuditoryViewStream(const Symbol& name_, CGPoint location_, CGSize size_) :
	name(name_), location(location_), size(size_)
{}

AuditoryViewStream::AuditoryViewStream(const Symbol& name_, double pitch_, double loudness_,  CGPoint location_) :
	name(name_), pitch(pitch_), loudness(loudness_), location
{}
*/
AuditoryViewStream::AuditoryViewStream(const Symbol& name_, double pitch_, double loudness_, float x, float y) :
	name(name_), pitch(pitch_), loudness(loudness_)
{
	location.x = x;
	location.y = y;
	size.width = 0.;
	size.height = 0.;
}

/*
AuditoryViewStream::AuditoryViewStream(const Symbol& name_, float x, float y, float width, float height) :
	name(name_)
{
	location.x = x;
	location.y = y;
	size.width = width;
	size.height = height;
}
*/

void AuditoryViewStream::set_location(CGPoint location_)
{
	location = location_;
}

void AuditoryViewStream::set_location(float x, float y)
{
	location.x = x;
	location.y = y;
}

void AuditoryViewStream::set_pitch(double pitch_)
{
	pitch = pitch_;
}

void AuditoryViewStream::set_loudness(double loudness_)
{
	loudness = loudness_;
}

void AuditoryViewStream::set_size(CGSize size_)
{
	size = size_;
}

void AuditoryViewStream::set_size(float width, float height)
{
	size.width = width;
	size.height = height;
}

// set the property to the value; if the property value  is nothing or nil, remove it from the container
void AuditoryViewStream::set_property(const Symbol& propname, const Symbol& propvalue)
{
	properties_t::iterator it = properties.find(propname);
	if(it == properties.end())
		properties.insert(properties_t::value_type(propname, propvalue));
	else if(propvalue == Nil_c || propvalue  == Empty_string_c)
		properties.erase(it);
	else
		properties[propname] = propvalue;
}

void AuditoryViewStream::drawself(CGContextRef ctx, float scale) const
{
//	assert(shape_ptr);
//	shape_ptr->draw(ctx, this);
	
/*	if(disappearing)
		CGContextSetAlpha(ctx, 0.3);
	else
		CGContextSetAlpha(ctx, 1.0);
*/

	// create a string with all the information in it
	ostringstream oss; 
//	oss << name << " (" << location.x << ", " << location.y << ") [" << size.width << ", " << size.height << "] ";
	oss << name << " " << pitch << " " << loudness << " (" << location.x << ", " << location.y << ")";
	for(properties_t::const_iterator it = properties.begin(); it != properties.end(); it++) {
		oss << it->first << ':' << it->second << ' ';
		}
	CGContextShowText(ctx, oss.str().c_str(), oss.str().length());	
}


AuditoryViewSound::AuditoryViewSound() :
		name("default initialized AuditoryViewSound"), disappearing(false)
{
}


AuditoryViewSound::AuditoryViewSound(const Symbol& name_, const Symbol& stream_, long time_stamp_, float x, float y, const Symbol& timbre_, double loudness_) :
	name(name_), stream(stream_), time_stamp(time_stamp_), timbre(timbre_), pitch(0.), loudness(loudness_), disappearing(false)
{
	location.x = x;
	location.y = y;
}

AuditoryViewSound::AuditoryViewSound(const Symbol& name_, const Symbol& stream_, long time_stamp_, float x, float y, const Symbol& timbre_, 
	const Symbol& content_, const Symbol& speaker_gender_, const Symbol& speaker_id_, double loudness_, double pitch_) :
	name(name_), stream(stream_), time_stamp(time_stamp_), timbre(timbre_), content(content_), speaker_gender(speaker_gender_), speaker_id(speaker_id_),
	loudness(loudness_), pitch(pitch_), disappearing(false)
{
	location.x = x;
	location.y = y;
}

// set the property to the value; if the property value  is nothing or nil, remove it from the container
void AuditoryViewSound::set_property(const Symbol& propname, const Symbol& propvalue)
{
	properties_t::iterator it = properties.find(propname);
	if(it == properties.end())
		properties.insert(properties_t::value_type(propname, propvalue));
	else if(propvalue == Nil_c || propvalue == Empty_string_c)
		properties.erase(it);
	else
		properties[propname] = propvalue;
}


void AuditoryViewSound::drawself(CGContextRef ctx, float scale) const
{
	// create a string with all the information in it
	ostringstream oss;
	oss << name << ' ' << stream << ' ' << " (" << location.x << ", " << location.y << ")" << ' ' 
		<< timbre << ' ' << speaker_gender << ' ' << speaker_id  << ' ' 
		<< pitch << ' ' << loudness << ' ' << content << ' ';
	if(disappearing)
		oss << "Fading ";
	for(properties_t::const_iterator it = properties.begin(); it != properties.end(); it++) {
		oss << it->first << ':' << it->second << ' ';
		}
	const string& str = oss.str();
	CGContextShowText(ctx, str.c_str(), str.length());
//	CGContextShowTextAtPoint(ctx, CGRectGetMinX(rect), CGRectGetMidY(rect), str.c_str(), str.length());		
}
