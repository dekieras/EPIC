#ifndef STANDARD_UTILITY_SYMBOLS_H
#define STANDARD_UTILITY_SYMBOLS_H

#include "Symbol.h"

// These are standard names known throughout the architecture as symbols
// in fundamental utility functions and containers.
// The string-representation is normally the same as the name with the "_c" suffix removed.
// See Standard_utility_symbols.cpp for the actual definition.
// As new names are used in the architecture, they should be added to this set,
// and the code should use these variable names instead of explicit strings.

extern const Symbol Default_c;
extern const Symbol Absent_c;
extern const Symbol Unknown_c;
extern const Symbol None_c;
extern const Symbol Nil_c;
extern const Symbol Empty_string_c;

#endif
