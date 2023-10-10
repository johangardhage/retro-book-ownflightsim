
#ifndef FIX_H

#define FIX_H
#define ABS(X) (X<0?-X:X)
#define COS(X) cos_table[ABS(X)&255]
#define SIN(X) sin_table[ABS(X)&255]

const int NUMBER_OF_DEGREES =256;
const int SHIFT = 9;
const long SHIFT_MULT = 1L<<SHIFT;

extern long cos_table[NUMBER_OF_DEGREES];
extern long sin_table[NUMBER_OF_DEGREES];

#endif
