
// OPTDEMO.CPP
//  Demonstrate optimized polygon-fill graphics animation
//  code
//
// 8/23/95 mickRacky cleanup/update to C++

#include	<dos.h>
#include	<conio.h>
#include <iostream.h>
#include  <stdlib.h>
#include  "poly.h"
#include  "screen.h"
#include	"pcx.h"

const XORIGIN=80;
const YORIGIN=80;
const WIND_WIDTH=7*21;
const WIND_HEIGHT=7*21;

world_type world;
Pcx bgobject;

void main(int argc,char* argv[])
{
	int key;
	int xangle=0,yangle=0,zangle=0; // X,Y&Z angles of
									//  object
	int xrot=0,yrot=0,zrot=0;       // X,Y&Z rotations
	unsigned char *screen_buffer;     // Offscreen drawing
												//  buffer

  // Read arguments from command line. If wrong number,
  //  print message and abort:

	if (argc!=2) {
		cerr << "Wrong number of arguments."<< endl;
		exit(-1);
	}

  // Load background image:

	if (bgobject.load("3dbg2.pcx")) {
		cerr << "Cannot load PCX file."<< endl;
		exit(-1);
	}
	if( world.loadpoly(argv[1]) )  {               // Load object description(s)
		cerr << "Failure loading polygons into World." << endl;
		exit(-1);
  }
	screen_buffer=new unsigned char[64000]; // Create buffer
	int oldmode=*(int *)MK_FP(0x40,0x49);	  // Save previous
														//  video mode
	setgmode(0x13);						      // Set mode 13h
	setpalette(bgobject.Palette());

	unsigned char *ptr = bgobject.Image();
	for(long i=0; i<64000; i++)             // Put background
	 screen_buffer[i]=*ptr++;           	 //  in buffer

	int curobj=0;                           // First object
    int scalefactor=1;
	 int zdistance=600;

	 // paint the background to the video screen:
	putwindow(0,0,320,200,FP_OFF(screen_buffer), FP_SEG(screen_buffer));
	while (key!=27) {
		clrwin(10,8,WIND_WIDTH,WIND_HEIGHT,screen_buffer);
		inittrans();          // Initialize transformations
		scale(scalefactor);   // Create scaling matrix
		rotate(xangle,yangle,zangle); // Create rotation matrix

    // Rotate object one increment:

		xangle+=xrot;
		yangle+=yrot;
		zangle+=zrot;

	 // Check for 256 degree wrap around:

		if (xangle>255) xangle=0;
		if (xangle<0) xangle=255;
		if (yangle>255) yangle=0;
		if (yangle<0) yangle=255;
		if (zangle>255) zangle=0;
		if (zangle<0) zangle=255;

    // Translate object:

		translate(0,0,zdistance);

	 // Call the Draw world loop

		world.Draw(curobj,screen_buffer,XORIGIN,YORIGIN);

	 // Put the viewport out to the video display:
		putwindow(10,8,WIND_WIDTH,WIND_HEIGHT,FP_OFF(screen_buffer), FP_SEG(screen_buffer));

	 // Watch for user input:

		if (kbhit()) {  // If input received....
			key=getch();  // Read the key code
			switch(key) {
				case 13:

          // ENTER: Go to next object

					curobj++;
					if (curobj>=world.GetObjectCount())
						curobj=0;
					break;

				case 55:

          // "7": Speed up x rotation

					xrot++;
					break;

				case 52:

          // "4": Stop x rotation

					xrot=0;
					break;

				case 49:

          // "1": Slow down x rotation

					--xrot;
					break;

				case 56:

          // "8": Speed up y rotation

					yrot++;
					break;

				case 53:

          // "5": Stop y rotation

					yrot=0;
					break;

				case 50:

          // "2": Slow down y rotation

					--yrot;
					break;

				case 57:

          // "9": Speed up z rotation

					zrot++;
					break;

				case 54:

          // "6": Stop z rotation

					zrot=0;
					break;

				case 51:

          // "3": Slow down z rotation

					--zrot;
					break;

				case '+':

          // "+": Increase distance

					zdistance+=30;
					break;

				case '-':

          // "-": Decrease distance

					if (zdistance>530) zdistance-=30;
					break;
			}
		}
	}
	setgmode(oldmode); // Reset video and exit

	if( screen_buffer)
		delete [] screen_buffer;
}
