
#define ABS(X) (X<0?-X:X)
#define COS(X) cos_table[ABS(X)&255]
#define SIN(X) sin_table[ABS(X)&255]

const NUMBER_OF_DEGREES = 256;
const SHIFT = 9;
const SHIFT_MULT = 1<<SHIFT;

extern long cos_table[256];
extern long sin_table[256];
