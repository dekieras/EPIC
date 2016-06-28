#ifndef OUTPUT_TEE_H
#define OUTPUT_TEE_H

#include "View_base.h"
#include "Assert_throw.h"

#include <iostream>
#include <sstream>
#include <list>

/*

The Output_tee class allows you to split output among as many output streams
and View_bases as you wish.  It keeps a list of pointers to ostream objects and View_bases, 
and overloads operator<< to output to each stream in the list, and to an internal
ostringstring object whose contents are written a line-at-a-time to each View_base.

This class assumes the pointed-to stream objects and View_bases exist. 
If the stream object or View_base is deallocated, it should first be taken out of the list.
No check is made for putting the same stream or View_base in the list more than once. 

Note that the ostringstream object is always present, but is only written to or manipulated if
there are View_bases in the list. 
*/


class Output_tee {
public:
	// return true if the list of streams or View_bases is non-empty;
	// if the list of streams is non-empty, at least the first must be good
	// usage:
	// if(Normal_out)
	//	Normal_out << stuff;
	operator bool() const
		{
			return (
				(!stream_ptr_list.empty() && stream_ptr_list.front()->good())
			 	|| 
			 	!view_ptr_list.empty());
		}
	
	// put a stream into the list
	void add_stream(std::ostream& os)
		{
			std::ostream * os_ptr = &os;
			stream_ptr_list.push_back(os_ptr); 
		}

	// take a stream out of the list		
	void remove_stream(std::ostream& os)
		{
			stream_ptr_list.remove(&os);
		}
	// is a stream in the list
	bool is_present(std::ostream& os)
		{
			for(stream_ptr_list_t::iterator it = stream_ptr_list.begin(); it != stream_ptr_list.end(); ++it)
				if(*it == &os)
					return true;
			return false;
		}

	// put an View_base into the list
	void add_view(View_base * view_ptr)
		{
			view_ptr_list.push_back(view_ptr); 
		}

	// take an View_base out of the list		
	void remove_view(View_base * view_ptr)
		{
			view_ptr_list.remove(view_ptr);
		}

	// is a view in the list
	bool is_present(View_base * view_ptr)
		{
			for(view_ptr_list_t::iterator it = view_ptr_list.begin(); it != view_ptr_list.end(); ++it)
				if(*it == view_ptr)
					return true;
			return false;
		}

	// check if there are stream pointers in the stream pointer list - or window pointers - if so, output being produced
	bool output_enabled()
		{
			return !stream_ptr_list.empty() || !view_ptr_list.empty();
		}
	
	// templated member function applies for any type being output
	template <class T>
	Output_tee& operator<< (const T& x)
	{
		std::ostream * os_ptr;
		for (stream_ptr_list_t::iterator it = stream_ptr_list.begin(); it != stream_ptr_list.end(); it++) {
			os_ptr = *it;
			if ((*os_ptr).good())
				*os_ptr << x;
			}

		// do nothing further if no views in the list
		if(view_ptr_list.empty())
			return *this;
		
		view_line_buffer << x;

		return *this;
	}

	// specialize for manipulators defined on ios_base
	Output_tee& operator<< (std::ios_base& (*manip) (std::ios_base&))
	{
		std::ostream * os_ptr;
		for (stream_ptr_list_t::iterator it = stream_ptr_list.begin(); it != stream_ptr_list.end(); it++) {
			os_ptr = *it;
			if ((*os_ptr).good())
				manip(*os_ptr);
			}

		// do nothing further if no views in the list
		if(view_ptr_list.empty())
			return *this;

		os_ptr = &view_line_buffer;
		manip(*os_ptr);

		return *this;	
	}

	// specialize for manipulators defined on ostream
	Output_tee& operator<< (std::ostream& (*manip) (std::ostream&))
	{
		std::ostream * os_ptr;
		for (stream_ptr_list_t::iterator it = stream_ptr_list.begin(); it != stream_ptr_list.end(); it++) {
			os_ptr = *it;
			if ((*os_ptr).good())
				manip(*os_ptr);
			}
			
		// do nothing further if no views in the list
		if(view_ptr_list.empty())
			return *this;
		
		os_ptr = &view_line_buffer;
		manip(*os_ptr);
		
		// if last character was a newline, then write the line to the views and 
		// empty the stream (simulates effect of "endl")
		const std::string& s = view_line_buffer.str();
		std::size_t n = s.size();
		if(s[n - 1] != '\n')
			return *this;

		for (view_ptr_list_t::iterator it = view_ptr_list.begin(); it != view_ptr_list.end(); it++) {
			View_base * window_ptr = *it;
			window_ptr->notify_append_text(view_line_buffer.str());
			}

		view_line_buffer.str("");
			
		return *this;	
	}

	friend class Output_tee_format_saver;

private:
	typedef std::list<std::ostream *> stream_ptr_list_t;
	stream_ptr_list_t stream_ptr_list;
	typedef std::list<View_base *> view_ptr_list_t;
	view_ptr_list_t view_ptr_list;
	std::ostringstream view_line_buffer;

};

// This class provides for saving and restoring the stream formatting flags.
// Instantiating it saves the current state of the stream formatting in the Output_tee
// and then restores it when destructed. Use in any function that alters the stream formatting.
// It is assumed that the formatting for the first stream or the internal ostreamstringg 
// applies to all streams; if no streams or views are associated with the Output_tee at 
// the time of construction, the default settings are saved. The settings of the first stream
// are saved; if no streams, then the settings of the internal stringstream are saved.
// At the time of destruction, the streams or views will be set to the saved settings.
// So for consistency and clarity, the number of streams or views associated with the Output_tee 
// should not be altered during the lifetime of an Output_tee_format_saver.


// Usage example:
//	void func(Output_tee& ot)
//	{
//		Output_tee_format_saver s(ot);
//		ot << fixed << setprecision(2) << x << endl;
//	}	// state of ot restored at exit

class Output_tee_format_saver {
public:
	Output_tee_format_saver(Output_tee& output_tee_) :
		output_tee(output_tee_), old_flags(std::ios::fmtflags()), old_precision(0)
		{
			std::ostream * os_ptr = 0;
			// if there is an output stream, save the state of the first one; 
			// otherwise save the ostringstream state
			if(!output_tee.stream_ptr_list.empty())
				os_ptr = output_tee.stream_ptr_list.front();
			else if(!output_tee.view_ptr_list.empty())
				os_ptr = &output_tee.view_line_buffer;
			else
				return;
			
			old_flags = os_ptr->flags();
			old_precision = os_ptr->precision();
		}
	
	~Output_tee_format_saver()
		{
			// apply the saved formatting to all streams
			for (std::list<std::ostream *>::iterator it = output_tee.stream_ptr_list.begin(); it != output_tee.stream_ptr_list.end(); it++) {
				std::ostream * os_ptr = *it;
				os_ptr->flags(old_flags);
				os_ptr->precision(old_precision);
				}
			// apply the saved formatting to the ostringstream if views are present
			if(!output_tee.view_ptr_list.empty()) {
				output_tee.view_line_buffer.flags(old_flags);
				output_tee.view_line_buffer.precision(old_precision);
				}
		}
private:
	Output_tee& output_tee;
	std::ios::fmtflags old_flags;
	std::streamsize old_precision;
};


	
#endif
