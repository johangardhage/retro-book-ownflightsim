#ifndef LIGHT_H

#define LIGHT_H

#include "palette.h"

// Structure for light source data:

struct source_type { // Structure for light source
  long x,y,z;        // Incident vector
  long length;       // Length of incident vector
  int  ambient;      // Amount of ambient light
};

#endif
