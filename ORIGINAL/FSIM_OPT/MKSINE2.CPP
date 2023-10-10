// MAKESINE.CPP
//
// 8/23/95 mickRacky updated to C++

#include  <fstream.h>
#include  <stdlib.h>
#include  <math.h>
#include  <conio.h>

const SHIFT = 9;               // Fixed point shift
const SHIFT_MULT = 1<<SHIFT;   // Fixed point shift as
							   //  a multiplication
static int SIN_MASK = 0x1ff;        //

 // Degrees in a circle
static int NUMBER_OF_DEGREES = 512;
void main(int argc, char *argv[])
{

	if( argc > 1)
	{
		int tmp = atoi(argv[1]);
		if( tmp < 256)
			tmp = 256;
		int ii ;
		for( ii =0; ii < 16; ii++)
		{
			int match = 1<<ii;
			if( tmp == match)
				break;
		}
		if( ii >= 16)
		{
			cerr << "ARG must be a power of 2!\n";
			exit(0);
		}
		NUMBER_OF_DEGREES = tmp;
		SIN_MASK = (NUMBER_OF_DEGREES - 1);
	}

  // Create file FIX.H for constants and macros:

	fstream finout("fix.h",ios::out | ios::trunc);
	finout << "\n#ifndef FIX_H\n\n// FIX.H  10-22-95 mracky  fixed\n#define FIX_H\n";
	finout << "#define ABS(X) (X<0?-X:X)\n";
	finout << "#define SIN_MASK 0x" << hex<< SIN_MASK << "\n";
	finout << "\nconst int NUMBER_OF_DEGREES =" << dec << NUMBER_OF_DEGREES << ";\n";
	finout << "const int SHIFT = " << SHIFT << ";\n";
	finout << "const long SHIFT_MULT = 1L<<SHIFT;\n\n";
	finout << "const int HALF_CIRCLE= " << int(NUMBER_OF_DEGREES/2) << ";\n";
	finout << "const int QUARTER_CIRCLE= " << int(NUMBER_OF_DEGREES/4) << ";\n";
	finout << "\nextern long cos_table[NUMBER_OF_DEGREES];\n";
	finout << "extern long sin_table[NUMBER_OF_DEGREES];\n";
	finout << "\ninline long COS(long X)\n{return (cos_table[ABS(X)&SIN_MASK]); }\n";
	finout << "inline long SIN(long X)\n{return (sin_table[ABS(X)&SIN_MASK]); }\n";
	finout << "\n#endif\n";
	finout.close();

  // Create file FIX.CPP for sine and cosine tables:

	finout.open("fix.cpp",ios::out | ios::trunc);
	finout << "\n//FIX.CPP\n" << "//  Fixed point math tables\n\n"<<
			"#include \"fix.h\"\n";

  // Create cosine table:

	finout << "long cos_table[NUMBER_OF_DEGREES]={\n    ";
	float radians=0.0;
	int count=0;
	for (int i=0; i<NUMBER_OF_DEGREES; i++) {
		finout << long(cos(radians)*SHIFT_MULT) << ", ";
		radians += 6.283/NUMBER_OF_DEGREES;
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
	radians=0.0;
	for (i=0; i<NUMBER_OF_DEGREES; i++) {
		finout << long(sin(radians)*SHIFT_MULT) << ", ";
		radians += 6.283/NUMBER_OF_DEGREES;
		count++;
	if (count>=8) {
	  finout << endl << "    ";
	  count=0;
	}
	}
  finout << "};\n";
  finout.close();
}
