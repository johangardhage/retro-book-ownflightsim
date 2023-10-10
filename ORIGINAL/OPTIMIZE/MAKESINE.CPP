// MAKESINE.CPP
//
// 8/23/95 mickRacky updated to C++

#include  <fstream.h>
#include  <math.h>
#include  <conio.h>

const NUMBER_OF_DEGREES = 256; // Degrees in a circle
const SHIFT = 9;               // Fixed point shift
const SHIFT_MULT = 1<<SHIFT;   // Fixed point shift as
                               //  a multiplication
void main()
{
	float radians=0;

  // Create file FIX.H for constants and macros:

	fstream finout("fix.h",ios::out | ios::trunc);
	finout << "\n#ifndef FIX_H\n\n#define FIX_H\n";
	finout << "#define ABS(X) (X<0?-X:X)\n";
	finout << "#define COS(X) cos_table[ABS(X)&255]\n";
	finout << "#define SIN(X) sin_table[ABS(X)&255]\n";
	finout << "\nconst int NUMBER_OF_DEGREES =" << NUMBER_OF_DEGREES << ";\n";
	finout << "const int SHIFT = " << SHIFT << ";\n";
	finout << "const long SHIFT_MULT = 1L<<SHIFT;\n\n";
	finout << "extern long cos_table[NUMBER_OF_DEGREES];\n";
	finout << "extern long sin_table[NUMBER_OF_DEGREES];\n";
	finout << "\n#endif\n";
	finout.close();

  // Create file FIX.CPP for sine and cosine tables:

	finout.open("fix.cpp",ios::out | ios::trunc);
    finout << "\n//FIX.CPP\n" << "//  Fixed point math tables\n\n"<<
	        "#include \"fix.h\"\n";

  // Create cosine table:

	finout << "long cos_table[NUMBER_OF_DEGREES]={\n    ";
	int count=0;
	for (int i=0; i<NUMBER_OF_DEGREES; i++) {
		finout << long(cos(radians)*SHIFT_MULT) << ", ";
		radians += 6.28/NUMBER_OF_DEGREES;
		count++;
		if (count>=8) {
			finout << endl << "    ";
			count=0;
		}
	}
	finout << "};\n\n";

  // Create sine table:

	finout << "long sin_table[NUMBER_OF_DEGREES]={\n    ";
	count=0;
	for (i=0; i<NUMBER_OF_DEGREES; i++) {
		finout << long(sin(radians)*SHIFT_MULT) << ", ";
		radians += 6.28/NUMBER_OF_DEGREES;
		count++;
    if (count>=8) {
      finout << endl << "    ";
      count=0;
    }
	}
  finout << "};\n";
  finout.close();
}
