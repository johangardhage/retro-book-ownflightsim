
// PCXVIEW.CPP - 1/30/93
//
// A program to display a PCX file specified at the command line.
//
// Copyright 1993 by Christopher Lampton and The Waite Group Press
//
// 8/15/95 mickRacky update to C++

#include	<stdio.h>
#include	<stdlib.h>
#include	<conio.h>
#include	<dos.h>
#include <iostream.h>
#include	"screen.h"
#include	"pcx.h"

class PcxView : public Pcx
{
	public:
		PcxView():Pcx() {;}
		void Draw(unsigned char *screen);
};

void PcxView::Draw(unsigned char *screen)
{
	// make a pointer to the image data (from base Pcx class)
  unsigned char *ptr = Image();
  // Loop through all 64,000 bytes, displaying each:
  //
  for(long i=0; i<64000; i++) screen[i]=*ptr++;
	
}

PcxView pcxobject;      // PCX class object

void main(int argc,char* argv[])
{
	if (argc!=2) { // Are there 2 arguments on command line?
		cerr << "Wrong number of arguments." << endl;	 // If not...
		exit(0);                              // Exit w/error
	}
	if (pcxobject.load(argv[1])) { // Get name of PCX
		cerr << "Cannot load PCX file." << endl;     // Can't open it?
		exit(0);                             // Abort w/error
	}
  //
  // Display the PCX:
  //
	cls((char *)MK_FP(0xa000,0));         // Clear the screen
	int oldmode=*(int *)MK_FP(0x40,0x49); // Set VGA mode...
	setgmode(0x13);                       // to 320x200x256
	setpalette(pcxobject.Palette());     // Set PCX palette
  //
  // Create pointer to video display:
  //
	unsigned char far *screen=(unsigned char far *)MK_FP(0xa000,0);

  // pass pointer to PcxView object's Draw function
  pcxobject.Draw(screen);

  //
  // Hold screen until a key is pressed, then reset
  //  graphics mode and exit to DOS
  //
	while (!kbhit());  // Wait for key press
	setgmode(oldmode); // Reset old graphics mode
}
