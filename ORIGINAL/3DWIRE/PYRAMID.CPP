
// PYRAMID.CPP
//
// A program to draw an animated pyramid on
// on the mode 13h display.
//
// Copyright 1993 by Christopher Lampton and
// The Waite Group Press
//
// 8/21/95 mickRacky - update and cleanup memory

#include	<stdio.h>
#include	<dos.h>
#include	<conio.h>
#include	"screen.h"
#include  "wire.h"

// Data for shapes:

// Vertex data for pyramid:

vertex_type pyramid_vertices[]={  // Vertices for pyramid
		{0,-10,0,1,  					     // Vertex 0
		0,0,0,1,
		0,0,0,1},
		{10,10,10,1,						     // Vertex 2
		0,0,0,1,
		0,0,0,1},
		{-10,10,10,1,					     // Vertex 3
		0,0,0,1,
		0,0,0,1},
		{-10,10,-10,1,					     // Vertex 4
		0,0,0,1,
		0,0,0,1},
		{10,10,-10,1,					     // Vertex 5
		0,0,0,1,
		0,0,0,1}
};

// Line data for pyramid:

line_type pyramid_lines[]={
	 {0,1},                       // Line 0
	 {0,2},                       // Line 1
	 {0,3},                       // Line 2
	 {0,4},                       // Line 3
	 {1,2},                       // Line 4
	 {2,3},                       // Line 5
	 {3,4},                       // Line 6
	 {4,1}                        // Line 7
};

// Shape data for pyramid:

Shape pyramid(5,pyramid_vertices,8, pyramid_lines);

void main()
{
  float xangle=0,yangle=0,zangle=0; // X,Y&Z angles to
												//  rotate shape
  float xrot=0.1,yrot=0.1,zrot=0.1; // X,Y&Z rotation
												//  increments
  unsigned char *screen_buffer;     // Offscreen drawing
												//  buffer

  screen_buffer=new unsigned char[64000];
	int oldmode =
	 *(int *)MK_FP(0x40,0x49);	      // Save previous video
												//  mode
	setgmode(0x13);									  // Set mode 13h
  while (!kbhit()) {                // Loop until key is
												//  pressed
	 cls(screen_buffer);             // Clear screen buffer
	 inittrans();                    // Initialize
												//  translations
	 scale(1.5);                     // Create scaling
												//  matrix
	 rotate(xangle,yangle,zangle);   // Create rotation
												//  matrices
	 xangle+=xrot;                   // Increment rotation
	 yangle+=yrot;                   //  angles
	 zangle+=zrot;
	 translate(0,0,50);              // Create translation
												//  matrix
	 pyramid.Transform();            // Transform SHAPE
												//  using MATRIX
	 pyramid.Project(100);          // Perform perspective
												//  projection
	 pyramid.Draw(screen_buffer); // Draw transformed
													//  shape
	 putwindow(0,0,320,200,screen_buffer); // Put on screen
  }
	setgmode(oldmode);                 // Reset previous video
												//  mode & end
	if( screen_buffer)
		delete [] screen_buffer;
}
