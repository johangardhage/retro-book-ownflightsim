
#ifndef FIX_H

// FIX.H  10-22-95 mracky  fixed
#define FIX_H
#define ABS(X) (X<0?-X:X)
#define SIN_MASK 0x3ff

const int NUMBER_OF_DEGREES =1024;
const int SHIFT = 9;
const long SHIFT_MULT = 1L<<SHIFT;

const int HALF_CIRCLE= 512;
const int QUARTER_CIRCLE= 256;

extern long cos_table[NUMBER_OF_DEGREES];
extern long sin_table[NUMBER_OF_DEGREES];

inline long COS(long X)
{return (cos_table[ABS(X)&SIN_MASK]); }
inline long SIN(long X)
{return (sin_table[ABS(X)&SIN_MASK]); }

#endif
