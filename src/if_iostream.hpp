#ifndef IF_IOSTREAM_H
#define IF_IOSTREAM_H

#include <iostream>
#include "measurement.hpp"

Measurement measure(std::istream&);
std::ostream& operator<<(std::ostream&, Measurement const&);

#endif
