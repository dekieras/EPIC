//
//  EPICDrawView.cpp
//  EPIC
//
//  Created by David Kieras on 8/7/13.
//  Copyright(c) 2016 The Regents of the University of Michigan. See LICENSE.txt
//

#include "EPICDrawView.h"
#include "CEDrawView.h"

EPICDrawView::EPICDrawView(CEDrawView* theDrawView_) :
	theDrawView(theDrawView_), changed(false)
{}

void EPICDrawView::refresh()
{
	if(changed) {
		[theDrawView setNeedsDisplay:YES];
		changed = false;
		}
}

CGSize EPICDrawView::get_view_size() const // in untransformed coordinates
{
    NSRect bounds= [theDrawView bounds];
	return bounds.size;
//    {return [theDrawView bounds].size;}
}


/*

CGContextRef EPICDrawView::getCGContext()
{
	return [theDrawView CGContext];


}
*/
