//
//  EPICTextView.h
//  EPIC
//
//  Created by David Kieras on 8/5/13.
//  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
//

#ifndef EPICTEXTVIEW_H
#define EPICTEXTVIEW_H
#include "EPICLib/View_base.h"
#include <string>
@class CETextWindow;


// an object adapter to interface EPICLib's View_base to EPICApp's CETextWindow

class EPICTextView : public View_base {
public:
    EPICTextView(CETextWindow* the_window_);
	virtual void clear() override;
	virtual void notify_append_text(const std::string& str) override;    
    
private:
    CETextWindow* the_window;

};

#endif
