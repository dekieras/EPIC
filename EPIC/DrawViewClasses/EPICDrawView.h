//
//  EPICDrawView.h
//  EPIC
//
//  Created by David Kieras on 8/7/13.
//  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
//

#ifndef EPICDRAWVIEW_H
#define EPICDRAWVIEW_H
#include "EPICLib/View_base.h" 
#include <string>
@class CEDrawView;


// an object adapter to interface EPICLib's View_base to EPICApp's CEDrawView

class EPICDrawView : public View_base {
public:
    EPICDrawView(CEDrawView* theDrawView_);
	// call from Cocoa NSView drawRect method to redraw the objects in the view
	// this provides the connection from the cpp view class back into the Cocoa drawing environment
	virtual void draw_content(CGContextRef ctx) = 0; // make pure virtual
	void set_changed()
		{changed = true;}
	void refresh();
	// set display scale
	virtual void set_scale(float scale_) {}
	// set calibration grid display state
	virtual void set_grid_on(bool grid_on_) {}
protected:
	CEDrawView* getDrawView()
		{return theDrawView;}
	CGSize get_view_size() const; // in untransformed coordinates
//	CGContextRef getCGContext();

private:
    CEDrawView* theDrawView;
	bool changed;	// whether the display changed content - not the same as needs display
};


#endif
