
// WALKMAN.CPP
//
// A program to display an animated figure.
//
// Copyright 1993 by Christopher Lampton and The Waite Group Press
//
// 8/21/95 mickRacky update C++ 

#include  <iostream.h>
#include  <dos.h>
#include  <conio.h>  // kbhit()
#include  <alloc.h>
#include  <mem.h>
#include  "sprite.h"
#include  "pcx.h"
#include  "screen.h"


const int NUM_SPRITES=4;		// Number of sprite images
const int SPRITE_WIDTH=24;	// Width of sprite in pixels
const int SPRITE_HEIGHT=24; // Height of sprite in pixels
const long DELAY=40000;  // Delay factor to determine
								 //  animation speed. (Adjust
								 //  this factor to find the
								 //  proper speed for your machine)

// Function prototypes:
void putbuffer(unsigned char far *screenbuf,int y1,
						int height);
// Global variable declarations:

unsigned char far *screenbuf;	 // 64000 byte array to hold
										 // screen image
int	walk_sequence[]={0,1,2,1}; // Sequence of images for
										 // animation

Pcx	walkman,walkbg;		      // Pcx objects to load bitmaps
										// A Sprite object
Sprite walksprite(NUM_SPRITES,SPRITE_WIDTH, SPRITE_HEIGHT);

void main()
{
  int oldmode;	// Storage for old video mode number

  //Load PCX file for background, abort if not found:

	if (walkman.load("walkman.pcx")) {
		cout << "Cannot load WALKMAN.PCX file." << endl;
	}
	else {
		// Set up for animation:

		cls((char *)MK_FP(0xa000,0));	    // Clear the screen
		oldmode=*(int *)MK_FP(0x40,0x49);	// Save old mode
		setgmode(0x13);								    // Set video mode to
												  //  13h
		setpalette(walkman.Palette());	        // Set VGA palette to
												  //  PCX palette
		walkbg.load("walkbg.pcx");	  // Load sprite PCX
		screenbuf=new unsigned char[64000];	// Create offscreen
													 //  video buffer
		memmove(screenbuf,walkbg.Image(),64000);	// Move background
													 //  image into
													 //  buffer
		putbuffer(screenbuf,0,SCREEN_HEIGHT); // Move offscreen
														  // buffer to vidram

		// Grab three sprite bitmaps from PCX bitmap:

		for(int i=0; i<3; i++)
			walksprite.grab(walkman.Image(),i,i*SPRITE_WIDTH+4*24,0);

		// Loop repeatedly through animation frames, moving
		//  the image from the left side of the display to the
		//  right:

		for(;;) {	                  // Loop indefinitely
			for(int j=0; j<15; j++) {	// Display fifteen frames
										  //  across screen
				for(i=0; i<4; i++) {		// Loop through four
										  //  different sprite images
					// Put next image on screen:

					walksprite.put(walk_sequence[i],j*20+i*5,100,
									screenbuf);
				 // Move segment of video buffer into
				 //  video memory:

					putbuffer(screenbuf,100,SPRITE_HEIGHT);
					for(long d=0; d<DELAY; d++); // Hold image on the
													// screen for count
					walksprite.erase(screenbuf);	// Erase image
					if (kbhit()) break;	// Check for keypress,
										//  abort if detected
				}
				if (kbhit()) break;	// Check for keypress,
								  //  abort if detected
			}
			if(kbhit()) break; // Check for keypress,
								 //  abort if detected
		}
		if(screenbuf)
			delete [] screenbuf;
		setgmode(oldmode);	// Restore old video mode
	}
}

void putbuffer(unsigned char far *screenbuf, int y1, int height)
{
	// Move offscreen video buffer into vidram

	// Point at video memory:
	char far *screen= (char far *)MK_FP(0xa000,0);

  // Call MEMMOVE() library function to move data
  //  into video memory:
  memmove(screen+y1*SCREEN_WIDTH,screenbuf+y1*SCREEN_WIDTH,
				 height*SCREEN_WIDTH);
}
