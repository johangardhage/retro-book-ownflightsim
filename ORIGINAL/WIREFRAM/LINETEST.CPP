// LINETEST.CPP
// A program to draw random lines on the mode 13h
// display using the line drawing function in the
// file BRESN.CPP.
//
// Copyright 1993 by Christopher Lampton and The Waite Group Press
//
// 8/30/95 mickRacky  - Note that this file was missing from the first edition
// 		distribution disk and was reconstructed

#include <stdio.h>
#include <dos.h>
#include <conio.h>
#include "bresn.h"
#include "screen.h"

void main()
{
	char far *screen =
		(char far *)MK_FP(0xa000,0); // point to video RAM
	int oldmode = *(int *)MK_FP(0x40,0x49); // Save previous
												// video mode
	cls(screen);                     // Clear mode 13h display
	setgmode(0x13);                   // Set 320x200x256-
												//  color graphics
	linedraw(0,89,319,124,WHITE,screen); // draw line
	while(!kbhit());                 // Wait for keypress
	setgmode(oldmode);                // Reset previous
											  // video mode & end
}