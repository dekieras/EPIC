//
//  Stream_type_factory.cpp
//  EPICXAF
//
//  Created by David Kieras on 1/24/13.
//
//

#include "Stream_type_factory.h"
#include "Stream_types.h"
#include "Epic_exception.h"

std::shared_ptr<Stream_base> create_stream(const std::string& type_name, const Symbol& stream_name)
{
	if(type_name == "Averaging_stream")
		return Averaging_stream::create(stream_name);
	else if(type_name == "Two_point_linear_stream")
		return Two_point_linear_stream::create(stream_name);
	
	// invalid type_name supplied
	throw Epic_internal_error("Invalid stream type name supplied to Stream_type_factory");
}
