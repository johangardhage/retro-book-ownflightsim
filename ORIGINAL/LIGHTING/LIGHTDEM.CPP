// LIGHTDEM.CPP
// copyright (c) 1995 Waite Group Press
//
// 7/19/95 Dale Sather       add lighting routines
// 9/20/95 mracky@sirius.com update to C++

#include	<dos.h>
#include	<conio.h>
#include <iostream.h>
#include  <stdlib.h>
#include  "world.h"
#include  "screen.h"

const int XORIGIN=(SCREEN_WIDTH/2);          // Origin of coordinate system X axis
const int YORIGIN=(SCREEN_HEIGHT/2);          // Origin of coordinate system Y axis
const WIND_WIDTH=7*21;
const WIND_HEIGHT=7*21;


World world;

void main(int argc,char* argv[])
{
	int xangle=0,yangle=0,zangle=0; // X,Y&Z angles for rotation
									//  object
	int xrot=2,yrot=2,zrot=0;       // X,Y&Z rotations increments
	unsigned char *screen_buffer;     // Offscreen drawing

	if (argc!=2) {                  // Read command-line arguments
		cerr << "Wrong number of arguments."<< endl;
		cerr << "LIGHTDEM object.txt" << endl;
		exit(-1);
	}
	if( world.LoadPoly(argv[1])< 1 )  {  // Load object description(s)
		cerr << "Failure loading object descriptor file into world." << endl;
		exit(-1);
	}

	screen_buffer=new unsigned char[64000L]; // Create offscreen buffer
	int oldmode=*(int *)MK_FP(0x40,0x49);			// Save previous video mode
	setgmode(0x13);					        // Set mode 13h
	world.AdjustLighting();                   // adjust the palette by the source

	int curobj =0;
	 int scalefactor=1;
	int zdistance=600;
	int key = 0;
				// Loop until escape key is pressed
	while (key != 27) {
		cls(screen_buffer);
		inittrans();                            // Initialize translations
		scale(scalefactor);                               // Create scaling matrix
		rotate(xangle,yangle,zangle);           // Create rotation matrices

		// Increment rotation angles
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

		translate(0,0,zdistance);          // Create translation matrix
	 // Call the Draw world loop
		world.Draw(curobj,screen_buffer,XORIGIN,YORIGIN);
	 // Put the viewport out to the video display:
		putwindow(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,FP_OFF(screen_buffer), FP_SEG(screen_buffer));
	 // Watch for user input:

		if (kbhit()) {  // If input received....
			key=getch();  // Read the key code
			switch(key) {
				case '\r':

		  // ENTER: Go to next object

					curobj++;
					if (curobj>=world.GetObjectCount())
						curobj=0;
					break;

				case '7':

		  // "7": Speed up x rotation

					xrot++;
					break;

				case '4':

		  // "4": Stop x rotation

					xrot=0;
					break;

				case '1':

		  // "1": Slow down x rotation

					--xrot;
					break;

				case '8':

		  // "8": Speed up y rotation

					yrot++;
					break;

				case '5':

		  // "5": Stop y rotation

					yrot=0;
					break;

				case '2':

		  // "2": Slow down y rotation

					--yrot;
					break;

				case '9':

		  // "9": Speed up z rotation

					zrot++;
					break;

				case '6':

		  // "6": Stop z rotation

					zrot=0;
					break;

				case '3':

		  // "3": Slow down z rotation

					--zrot;
					break;

				case '0':

		  // "0": Shut off all rotation

					zrot = xrot = yrot = 0;
					zangle = xangle = yangle = 0;
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
	setgmode(oldmode);                       // Reset previous video mode & end
	if( screen_buffer)
		delete [] screen_buffer;
}
