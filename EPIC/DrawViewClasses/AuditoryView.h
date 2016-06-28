/*
 *  AuditoryView.h
 *
 *  Created by David Kieras on 06/06/08.
 *  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
 *
 */
#ifndef AUDITORYVIEW_H
#define AUDITORYVIEW_H
#include "EPICDrawView.h"
#include "EPICLib/View_base.h"
#include "EPICLib/Symbol.h"

#include <map>
#include <string>
#include <ApplicationServices/ApplicationServices.h>

struct Speech_word;
class AuditoryViewStream;
class AuditoryViewSound;


class AuditoryView : public EPICDrawView {
public:
	AuditoryView(CEDrawView* theDrawView_);

	virtual ~AuditoryView();

	// called from base class to redraw the window
	void draw_content(CGContextRef ctx) override;

	// accessors to control the display metrics
//	void set_origin(float x_, float y_);
//	void set_scale(float scale_);
	// set calibration grid display state
//	void set_grid_on(bool grid_on_) {grid_on = grid_on_;}
	
	// notifications from Model
	void clear() override;
	void notify_auditory_stream_appear(const Symbol&, double pitch, double loudness, GU::Point) override;
	void notify_auditory_stream_disappear(const Symbol&) override;
	void notify_auditory_stream_pitch_changed(const Symbol&, double pitch) override;
	void notify_auditory_stream_loudness_changed(const Symbol&, double loudness) override;
	void notify_auditory_stream_location_changed(const Symbol& object_name, GU::Point location) override;
	void notify_auditory_stream_size_changed(const Symbol& object_name, GU::Size size) override;
	void notify_auditory_stream_property_changed(const Symbol&, const Symbol&, const Symbol&) override;
	void notify_auditory_sound_start(const Symbol&, const Symbol&, long, GU::Point, const Symbol&, double) override;
//	void notify_auditory_speech_start(const Symbol& sound_name, const Symbol& stream_name, long time_stamp,
//		const Symbol& content, const Symbol& speaker_gender, const Symbol& speaker_id, double loudness) override;
	void notify_auditory_speech_start(const Speech_word& word) override;
	void notify_auditory_sound_stop(const Symbol&) override;
	void notify_erase_sound(const Symbol&) override;
	void notify_auditory_sound_property_changed(const Symbol&, const Symbol&, const Symbol&) override;
	void notify_time(long current_time_) override;

private:
	// metric state
	CGPoint origin;	// location of lower-left in terms of object coordinates
	float scale;
	float old_scale;
	bool grid_on;	// if true, display calibration grid
	CGSize old_view_size;
	CGSize scaled_view_size;
	std::string view_info;
	long current_time;

	typedef std::map<Symbol, AuditoryViewStream> streams_t;
	typedef std::map<Symbol, AuditoryViewSound> sounds_t;
	streams_t streams;
	sounds_t sounds;
	
	void initialize();
	void update_info();
};

#endif
