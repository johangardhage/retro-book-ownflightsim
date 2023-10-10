// TEXTDEMO.CPP
// copyright (c) 1995 Waite Group Press
//
// 7/19/95 Dale Sather       add texture routines
// 9/20/95 mracky@sirius.com update to C++

#include	<dos.h>
#include	<conio.h>
#include <iostream.h>
#include  <stdlib.h>
#include  "world_tx.h"
#include  "screen.h"
#include  "texture.h"


World world;
Texture texturePcx;

void main(int argc,char* argv[])
{
	int xangle=0,yangle=0,zangle=0; // X,Y&Z angles for rotation
									//  object
	int xrot=2,yrot=2,zrot=0;       // X,Y&Z rotations increments
	unsigned char *screen_buffer;     // Offscreen drawing
	int resolution = 30;

	if (argc!=3) {                  // Read command-line arguments
		cerr << "Wrong number of arguments."<< endl;
		cerr << "TEXTDEMO object.txt texture.pcx" << endl;
		exit(-1);
	}
	if( world.LoadPoly(argv[1])< 1)  {  // Load object description(s)
		cerr << "Failure loading object descriptor file into world." << endl;
		exit(-1);
	}
	if (texturePcx.Load(argv[2])) {// Get name of PCX
		// Can't open it?
		cerr <<"Cannot load " << argv[2] << " PCX file"<< endl;        
    	exit(-1);    // Abort w/error
	}

	screen_buffer=new unsigned char[64000L]; // Create offscreen buffer
  	int oldmode=*(int *)MK_FP(0x40,0x49);			// Save previous video mode
  	setgmode(0x13);					        // Set mode 13h

	//Set the objects to the texture :
	for(int oo=0;oo < world.GetObjectCount(); oo++){
		(world.GetObjectPtr(oo))->SetTexture(&texturePcx);
	}

	setpalette(texturePcx.Palette());     // Set PCX palette
	int curobj =0;
	int scalefactor=1;
	int zdistance=800;
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
		world.Draw(curobj,screen_buffer,XORIGIN,YORIGIN, resolution);
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
				case '*':

          // "*": Increase resolution

					resolution++;
					if(resolution > 300) resolution = 300;
					break;

				case '/':

          // "/": Decrease resolution

					resolution--;
					if(resolution < 10)
						resolution = 10;
					break;
			}
		}
  }
	setgmode(oldmode);                       // Reset previous video mode & end
	if( screen_buffer)
		delete [] screen_buffer;
}
