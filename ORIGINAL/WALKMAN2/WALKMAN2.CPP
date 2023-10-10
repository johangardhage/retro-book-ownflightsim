// WALKMAN2.CPP
//
// A program to display an animated figure, with
// joystick, keyboard and mouse control.
//
// Copyright 1993 by Christopher Lampton and The Waite Group Press
//
//  8/21/95 mickRacky - updated to C++ i/o style

#include	<dos.h>
#include <conio.h>
#include	<mem.h>
#include <iostream.h>
#include	"sprite.h"
#include	"pcx.h"
#include	"screen.h"
#include	"evntmngr.h"

const int NUM_SPRITES=4;    // Number of sprite images
const int SPRITE_WIDTH=24;  // Width of sprite in pixels
const	int SPRITE_HEIGHT=24; // Height of sprite in pixels
const int MAX_SEQUENCE=4;   // Maximum length of animation
									 //  sequence
const int WALK_LEFT=0,      // Animation sequences
          STAND_STILL=1,
          WALK_RIGHT=2;
const int XJUMP=5;					// Movement factor for sprite
const long DELAY=10000;			// Delay factor to determine
									 //  animation speed
									 // (Adjust this factor to find
									 //  the proper speed for your
                            //  machine)

// Function prototypes:
void putbuffer(unsigned char far *screenbuf,int y1,int height);
// Structure types:

// Structure for animation sequences:

struct animation_structure {
	int seq_length;							// Length of sequence
	int sequence[MAX_SEQUENCE];	// Sequence array
};

// Global variable declarations:

unsigned char far *screenbuf;	// 64000 byte screen buffer
// Animation sequences:
struct animation_structure walkanim[3]={
		4,0,1,2,1,			// Walking left sequence
		1,3,0,0,0,			// Standing still sequence
		4,4,5,6,5				// Walking right sequence
};
int cur_sequence=STAND_STILL;	// Current animation sequence
int cur_frame=0;							// Current frame of sequence
int lastevent=0; 							// Last event received from
										//  event manager
int xpos=150;									// Initial x coordinate
int ypos=130;									// Initial y coordinate

Pcx	walkman,walkbg;		// Pcx objects to load bitmaps
Sprite walksprite(NUM_SPRITES,SPRITE_WIDTH,
                      SPRITE_HEIGHT);	// Sprite object

void main()
{
	int oldmode;	// Storage for old video mode number
	int event_mask;	// Input events requested by program

	screenbuf = 0; // Initalize to 0

	EventManager evelyn; // Initialize event manager in constructor!

  // Select control device:
  cout << "Type number for input device:" << endl <<
		  "  (1) Keyboard"                << endl <<
		  "  (2) Joystick"                << endl <<
		  "  (3) Mouse"                   << endl;

  char key=0;
  while ((key<'1') || (key>'3')) cin.get(key);
  switch (key) {
	case '1': event_mask=KEYBOARD_EVENTS; break;
	case '2': event_mask=JOYSTICK_EVENTS; break;
	case '3': event_mask=MOUSE_EVENTS;    break;
  }

	// Calibrate the user's joystick:

  if (event_mask&JOYSTICK_EVENTS) {
	cout << endl << "Center your joystick and press button one" << endl;
	evelyn.setcenter(); // Calibrate the center position
	cout << "Move your joystick to the upper lefthand corner " << endl <<
			"and press button one." << endl;
	evelyn.setmin();    // Calibrate the minimum position
	cout << "Move your joystick to the lower righthand corner" << endl <<
			"and press button one." << endl;
	evelyn.setmax();    // Calibrate the maximum position
  }

	//Load PCX file for background, abort if not found:

	if (walkman.load("walkman.pcx"))
	 cout << "Cannot load PCX file." << endl;
	else {

		// Set up for animation:

		cls((char *)MK_FP(0xa000,0));	// Clear the screen
		oldmode=*(int *)MK_FP(0x40,0x49);	// Save old video mode
		setgmode(0x13);								// Set video mode to 13h
		setpalette(walkman.Palette(),0,256);	// Set to PCX palette
		walkbg.load("walkbg.pcx");	// Load sprite PCX
		screenbuf=new unsigned char[64000];	// Create video
													 //  buffer
		memmove(screenbuf,walkbg.Image(),64000);	// Move background
													 //  image into
													 //  buffer
		putbuffer(screenbuf,0,SCREEN_HEIGHT);	// Move offscreen
														//  buffer to
														//  vidram

		// Grab seven sprite bitmaps from PCX bitmap:
		for(int i=0; i<7; i++) walksprite.grab(walkman.Image(),i,
										 i*SPRITE_WIDTH,0);

		// Animation loop:

		for(;;) {	// Loop indefinitely


		// Point to animation sequence:

		animation_structure *anim=&walkanim[cur_sequence];

			// Put next image on screen and advance one frame:

		walksprite.put(anim->sequence[cur_frame++],
							  xpos,ypos,screenbuf);

			// Check if next frame is beyond end of sequence:

			if (cur_frame>=anim->seq_length) cur_frame=0;

			// Advance screen position of sprite, if moving
			//  and not at edge of screen:

			if ((cur_sequence==WALK_RIGHT) &&
			 ((xpos+XJUMP)<(320-SPRITE_WIDTH))) xpos+=XJUMP;
			if ((cur_sequence==WALK_LEFT) && ((xpos-XJUMP) > 0))
					xpos-=XJUMP;

			// Move segment of video buffer into vidram:

			putbuffer(screenbuf,ypos,SPRITE_HEIGHT);

      // Hold image on the screen for DELAY count:

			for(long d=0; d<DELAY; d++);
			walksprite.erase(screenbuf);	// Erase sprite image

			// Process events from event manager:

			int event=evelyn.getevent(event_mask);		// Get next event

		// If left button, terminate:

			if (event&LBUTTON) break;

			// If no event, put sprite in standing still position:

			if ((event&(RIGHT+LEFT))==NOEVENT) {
				cur_sequence=STAND_STILL;
				cur_frame=0;
			}

			// Else check for RIGHT and LEFT events:

			else {
				if (event&RIGHT) {			// Is it a RIGHT event?
          // If so, go from standing still to walking right:
					if (cur_sequence==STAND_STILL) {
						cur_sequence=WALK_RIGHT;
				cur_frame=0;
					}
          // Or from walking left to standing still:
					if (cur_sequence==WALK_LEFT) {
						cur_sequence=STAND_STILL;
						cur_frame=0;
					}
				}
				if (event&LEFT) {				// Is it a LEFT event?
			 // If so, go from standing still to walking left:
					if (cur_sequence==STAND_STILL) {
						cur_sequence=WALK_LEFT;
						cur_frame=0;
					}
			 // Or from walking right to standing still:
					if (cur_sequence==WALK_RIGHT) {
						cur_sequence=STAND_STILL;
						cur_frame=0;
          }
        }
      }
	 }
	 if( screenbuf )
		delete [] screenbuf; // cleanup memory

  setgmode(oldmode);	// Restore old video mode
  }
}

void putbuffer(unsigned char far *screenbuf,int y1,int height)

// Move offscreen video buffer into vidram

{
  char far *screen=(char far *)MK_FP(0xa000,0); // Point
                                                // at vidram
  // Call MEMMOVE() function to move data into vidram:
  memmove(screen+y1*SCREEN_WIDTH,screenbuf+y1*SCREEN_WIDTH,
				height*SCREEN_WIDTH);
}
