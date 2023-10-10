
// TRIANGLE.PRJ
//   A short program to draw a triangle
//    on the mode 13h display
//
// Copyright 1993 by Christopher Lampton and
// The Waite Group Press

#include	<stdio.h>
#include	<dos.h>
#include	<conio.h>
#include	"screen.h"
#include	"bresnham.h"

void main()
{

	// Create pointer to video RAM:

	char far *screen=(char far *)MK_FP(0xa000,0);

	cls(screen);											    // Clear video RAM
	int oldmode=*(int *)MK_FP(0x40,0x49);	// Save previous
                                        //  video mode
	setgmode(0x13); 										    // Set mode 13h

	// Draw triangle:

	linedraw(160,70,190,130,15,screen);
	linedraw(190,130,130,130,15,screen);
	linedraw(130,130,160,70,15,screen);

	while (!kbhit());							 // Loop until key pressed
	setgmode(oldmode);						   // Reset previous video
                                 //  mode & end
}
