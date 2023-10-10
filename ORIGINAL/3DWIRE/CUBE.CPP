
// CUBE.CPP
//
// A program to draw an animated cube on
// the mode 13h display.
//
// Copyright 1993 by Christopher Lampton
//
// 8/21/95 mickRacky - update to BorlandC++ 4.5, cleanup memory
//                      and change shape_type to class Shape

#include	<stdio.h>
#include	<dos.h>
#include	<conio.h>
#include	"screen.h"
#include  "wire.h"

// Data for shapes:

// Vertex data for cube:

vertex_type cube_vertices[]={  // Vertices for cube
		{-10,-10,10,1,					     // Vertex 0
		0,0,0,1,
		0,0,0,1},
		{10,-10,10,1,					     // Vertex 1
		0,0,0,1,
		0,0,0,1},
		{10,10,10,1,						     // Vertex 2
		0,0,0,1,
		0,0,0,1},
		{-10,10,10,1,					     // Vertex 3
		0,0,0,1,
		0,0,0,1},
		{-10,-10,-10,1,				     // Vertex 4
		0,0,0,1,
		0,0,0,1},
		{10,-10,-10,1,					     // Vertex 5
		0,0,0,1,
		0,0,0,1},
		{10,10,-10,1,					     // Vertex 6
		0,0,0,1,
		0,0,0,1},
		{-10,10,-10,1,					     // Vertex 7
		0,0,0,1,
		0,0,0,1}
};

line_type cube_lines[]={
	 {0,1},                       // Line 0
	 {1,2},                       // Line 1
	 {2,3},                       // Line 2
	 {3,0},                       // Line 3
	 {4,5},                       // Line 4
	 {5,6},                       // Line 5
	 {6,7},                       // Line 6
	 {7,4},                       // Line 7
	 {0,4},                       // Line 8
	 {1,5},                       // Line 9
	 {2,6},                       // Line 10
	 {3,7}                        // Line 11
};

// Shape data for cube:

Shape cube(8,cube_vertices,12,cube_lines);

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
	 cube.Transform();               // Transform SHAPE
												//  using MATRIX
	 cube.Project(100);             // Perform perspective
												//  projection
	 cube.Draw(screen_buffer); // Draw transformed
												//  shape
	 putwindow(0,0,320,200,screen_buffer); // Put on screen
  }
	setgmode(oldmode);                 // Reset previous video
												//  mode & end
	if( screen_buffer)
		delete [] screen_buffer;
}
