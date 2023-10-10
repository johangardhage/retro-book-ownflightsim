
// RAINBOW.CPP
// A program to display the default VGA palette
//
// Copyright 1993 by Christopher Lampton and The Waite Group Press

#include	<stdio.h>
#include	<conio.h>
#include	<dos.h>
#include	"screen.h"

void main()
{

  // Clear the VGA display:

	cls((char *)MK_FP(0xa000,0));

  // Save the previous video mode:

	int oldmode=*(int *)MK_FP(0x40,0x49);

  // Set the current mode to 320x200x256:

	setgmode(0x13);

  // Create a pointer to video memory:

	char far *screen=(char far *)MK_FP(0xa000,0);

  // Loop through all 200 pixels in a column:

	for (int i=0; i<200; i++)

  // Loop though all 256 colors:

		for (int j=0; j<256; j++)

  // Putting one pixel of each color in each column:

			screen[i*320+j]=j;

  // Wait for key to be pressed:

	while (!kbhit());

  // Then reset old mode and exit to DOS:

	setgmode(oldmode);
}
