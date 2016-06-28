/*
 *  AuditoryView.cpp
 *  EPICX
 *
 *  Created by David Kieras on 6/6/08.
 *  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
 *
 */

#include "AuditoryView.h"
#include "AuditoryViewObjects.h"
#include "EPICLib/Standard_Symbols.h"
#include "EPICLib/Speech_word.h"
#include "AppColorUtilities.h"

#include <iostream>
#include <iomanip>


#include <string>
#include <vector>
#include <sstream>
#include <functional>
#include <algorithm>

using std::string;
using std::vector;
using std::ostringstream;	using std::fixed;	using std::setprecision;
using std::list;
using std::vector;
using std::for_each;
using std::mem_fun;	using std::bind2nd;
using std::cout;	using std::endl;

AuditoryView::AuditoryView(CEDrawView* theDrawView_) : EPICDrawView(theDrawView_),
	origin(CGPointZero), scale(1.0), old_scale(0), grid_on(false), current_time(0)
{
	old_view_size = CGSizeZero;
//	set_origin(0., 0.);
//	set_scale(10);
}

AuditoryView::~AuditoryView()
{
}

void AuditoryView::initialize()
{
	// empty the list of objects
	streams.clear();
	sounds.clear();
}

/*
void AuditoryView::set_origin(float x_, float y_)
{
	origin.x = x_; 
	origin.y = y_;
	update_info();
}

void AuditoryView::set_scale(float scale_)
{
	scale = scale_;
	update_info();
}
*/

void AuditoryView::update_info()
{
	ostringstream oss;
	oss << scaled_view_size.width << " X " << scaled_view_size.height 
		<< " DVA, (" << origin.x << ", " << origin.y << "), " << scale << " p/DVA";
	view_info = oss.str();
	set_changed();
}
	
void AuditoryView::draw_content(CGContextRef ctx)
{	
/* 
This has the code for drawing spatially - like the VisualView - but
the relevant things are commented out. For now, it just writes text for 
streams and sounds, starting with lines at the top of the window and going down.
*/

	CGSize view_size = get_view_size(); // in untransformed coordinates
	if(!CGSizeEqualToSize(view_size, old_view_size) || scale != old_scale) {
		old_scale = scale;
		old_view_size = view_size;
		scaled_view_size.height = view_size.height / scale;
		scaled_view_size.width = view_size.width / scale;
		update_info();
		}
	// set our origin to put the (0., 0) point in the view center
	CGContextTranslateCTM(ctx, view_size.width / 2., view_size.height / 2.);
	// change the scale for drawing
	CGContextScaleCTM(ctx, scale, scale);
	// change so that the origin point is now centered
	CGContextTranslateCTM(ctx, -origin.x,  -origin.y);
	
	// these are the maximum scaled values for x and y with (0, 0) at window center
	const int yrange = int(scaled_view_size.height / 2.);
	const int xrange = int(scaled_view_size.width / 2.);
	
	// draw some calibration lines
	
	if(grid_on) {
		CGContextSaveGState(ctx);
		CGContextSetStrokeColorWithColor(ctx, gColor_LightBlue);
		CGContextSetLineWidth(ctx, 1./scale);
		const int div = 1.;
		for(float x = -xrange; x <= xrange; x += div)
			for(float y = -yrange; y <= yrange; y += div) {
				CGContextMoveToPoint(   ctx, x, -yrange);
				CGContextAddLineToPoint(ctx, x,  yrange);
				CGContextStrokePath(ctx);
				CGContextMoveToPoint(   ctx, -xrange, y);
				CGContextAddLineToPoint(ctx,  xrange, y);
				CGContextStrokePath(ctx);
				}
		CGRect origcircRect = CGRectMake(-.5, -.5, 1., 1.);
		CGContextSetLineWidth(ctx, 2./scale);
		CGContextStrokeEllipseInRect(ctx, origcircRect);
		CGContextRestoreGState(ctx);
		}

	// set the standard font 
	CGContextSelectFont(ctx, "Monaco", 9./scale, kCGEncodingMacRoman);

	// write the current origin and scale information
//	CGContextShowTextAtPoint(ctx, -xrange, -yrange, view_info.c_str(), view_info.size());
	// prepare and write the time information
	ostringstream oss;
	oss << fixed << setprecision(2) << double(current_time) / 1000.;
	CGContextShowTextAtPoint(ctx, -xrange, yrange - (9./scale), oss.str().c_str(), oss.str().size());
			
	// this is pretty kludgy - magic number for the line_height
	float line_height = 10./scale;
	CGPoint linepos;
	linepos.x = -xrange;
	linepos.y = yrange - 2 * line_height;
		
	// tell the streams  to draw themselves - using current font and CG settings
	for(streams_t::iterator it = streams.begin(); it != streams.end(); it++) {
		CGContextSetTextPosition(ctx, linepos.x, linepos.y);
		it->second.drawself(ctx, scale);
		linepos.y -= line_height;
		}
		
	// prepare a vector containing a copy of the sound objects ordered by time
	typedef vector<AuditoryViewSound> sorted_sounds_t;
	sorted_sounds_t sorted_sounds;
	for(sounds_t::iterator it = sounds.begin(); it != sounds.end(); it++) {
		sorted_sounds.push_back(it->second);
		}
	stable_sort(sorted_sounds.begin(), sorted_sounds.end());
	
	// leave a blank line
	linepos.y -= line_height;
	// tell the sounds to draw themselves
/*	for(sounds_t::iterator it = sounds.begin(); it != sounds.end(); it++) {
		CGContextSetTextPosition(ctx, linepos.x, linepos.y);
		it->second.drawself(ctx, scale);
		linepos.y -= line_height;
		}
*/
	for(sorted_sounds_t::iterator it = sorted_sounds.begin(); it != sorted_sounds.end(); it++) {
		CGContextSetTextPosition(ctx, linepos.x, linepos.y);
		it->drawself(ctx, scale);
		linepos.y -= line_height;
		}
}


// overrides of View_base
void AuditoryView::clear()
{
	initialize();
	set_changed();
}

void AuditoryView::notify_auditory_stream_appear(const Symbol& name, double pitch, double loudness, GU::Point location)
{
	
	streams[name] = AuditoryViewStream(name, pitch, loudness, location.x, location.y);
	set_changed();
}


void AuditoryView::notify_auditory_stream_disappear(const Symbol& name)
{
	streams.erase(name);
	set_changed();
}

void AuditoryView::notify_auditory_stream_pitch_changed(const Symbol& name, double pitch)
{
	streams[name].set_pitch(pitch);
	set_changed();
}

void AuditoryView::notify_auditory_stream_loudness_changed(const Symbol& name, double loudness)
{
	streams[name].set_loudness(loudness);
	set_changed();
}

void AuditoryView::notify_auditory_stream_location_changed(const Symbol& name, GU::Point location)
{
	streams[name].set_location(location.x, location.y);
	set_changed();
}

void AuditoryView::notify_auditory_stream_size_changed(const Symbol&name, GU::Size size)
{
	streams[name].set_size(size.h, size.v);
	set_changed();
}

void AuditoryView::notify_auditory_stream_property_changed(const Symbol& name, const Symbol& prop, const Symbol& value)
{
	streams[name].set_property(prop, value);
	set_changed();
}

void AuditoryView::notify_auditory_sound_start(const Symbol& sound_name, const Symbol& stream_name, long time_stamp, GU::Point location, const Symbol& timbre, double loudness)
{
	sounds[sound_name] = AuditoryViewSound(sound_name, stream_name, time_stamp, location.x, location.y, timbre, loudness);
	set_changed();
}

/* void AuditoryView::notify_auditory_speech_start(const Symbol& sound_name, const Symbol& stream_name, long time_stamp, 
	const Symbol& content, const Symbol& speaker_gender, const Symbol& speaker_id, double loudness)
{
	sounds[sound_name] = AuditoryViewSound(sound_name, stream_name, time_stamp, Speech_c, content, speaker_gender, speaker_id, loudness);
	set_changed();
} */

void AuditoryView::notify_auditory_speech_start(const Speech_word& word)
{
	sounds[word.name] = AuditoryViewSound(word.name, word.stream_name, word.time_stamp, word.location.x, word.location.y, Speech_c, word.content, word.speaker_gender, word.speaker_id, word.loudness, word.pitch);
	set_changed();
}

void AuditoryView::notify_auditory_sound_stop(const Symbol& name)
{
	sounds[name].set_disappearing();
	set_changed();
}

void AuditoryView::notify_erase_sound(const Symbol& name)
{
	sounds.erase(name);
	set_changed();
}

void AuditoryView::notify_auditory_sound_property_changed(const Symbol& name, const Symbol& prop, const Symbol& value)
{
	sounds[name].set_property(prop, value);
	set_changed();
}

void AuditoryView::notify_time(long current_time_)
{
	current_time = current_time_;
	set_changed();
}
