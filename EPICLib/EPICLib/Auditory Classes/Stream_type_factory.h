//
//  Stream_type_factory.h
//  EPICXAF
//
//  Created by David Kieras on 1/24/13.
//
//

#ifndef STREAM_TYPE_FACTORY_H
#define STREAM_TYPE_FACTORY_H

#include "Stream_base.h"
#include "Symbol.h"
#include <string>
#include <memory>

std::shared_ptr<Stream_base> create_stream(const std::string& type_name, const Symbol& stream_name);


#endif

