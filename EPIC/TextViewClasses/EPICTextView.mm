//
//  EPICTextView.cpp
//  EPIC
//
//  Created by David Kieras on 8/5/13.
//  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
//

#include "EPICTextView.h"
#import "CETextWindow.h"

EPICTextView::EPICTextView(CETextWindow* the_window_) :
    the_window(the_window_)
{}

void EPICTextView::clear()
{
    [the_window clearView];
}

void EPICTextView::notify_append_text(const std::string& str)
{
    [the_window writeInView:[NSString stringWithUTF8String:str.c_str()]];
}
 
