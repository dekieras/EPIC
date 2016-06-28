#include "Standard_utility_symbols.h"

// These are standard names known throughout the architecture as symbols
// in fundamental utility functions and containers.
// The string-representation is normally the same as the name with the "_c" suffix removed.
// See Standard_utility_symbols.cpp for the actual definition.
// As new names are used in the architecture, they should be added to this set,
// and the code should use these variable names instead of explicit strings.

const Symbol Default_c("Default");
const Symbol Absent_c("Absent");
const Symbol Unknown_c("Unknown");
const Symbol None_c("None");
const Symbol Nil_c;
const Symbol Empty_string_c("");

