
// RANDLINE.CPP
//
// A program to draw random lines on the mode 13h
// display using the line drawing function in the
// file BRESNHAM.ASM.
//
// Copyright 1993 by Christopher Lampton and
// The Waite Group Press
// updated 8/30/95 mickRacky

#include	<stdio.h>
#include	<dos.h>
#include	<conio.h>
#include	<stdlib.h>
#include	<time.h>
#ifdef USE_BRESN_CPP
#include	"bresn.h"
#else
#include "bresnham.h"
#endif
#include	"screen.h"

const NUMCOLORS=16;					    // Line colors

void main()
{
	randomize();						  // Initialize random
											 //  numbers
	char far *screen =
	 (char far *)MK_FP(0xa000,0);	// Point to video RAM
	int oldmode =                   // Save previous video
	 *(int *)MK_FP(0x40,0x49);			//  mode
	cls(screen);										// Clear mode 13h
											 //  display;
	setgmode(0x13);									// Set 320x200x256-color
											 //  graphics
	int x1=random(320);							// Initialize line to
											 //  random
	int y1=random(200);							// ...starting values
	int color = WHITE;
	while (!kbhit()) {							// Draw lines until
											 //  keypress
		int x2=random(320);						// Continue line to
		int y2=random(200);						// ...random end point
		linedraw(x1,y1,x2,y2,color,screen); // Draw line
		x1=x2;						  					// Start next line at
		y1=y2;						  					// ...end point of last
		//color = (color+1)%NUMCOLORS;
											 //  line
	}
	setgmode(oldmode);								// Reset previous video
											 //  mode & end
}
