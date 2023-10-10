
// WAITE.CPP
//
// A program to draw an animated letter "W"
// on the mode 13h display.
//
// Copyright 1993 by Christopher Lampton and
// The Waite Group Press
//
// 8/21/95 mickRacky - update to BorlandC++ 4.5 ,cleanup memory and
//                      change from shape_type structure into Shape class

#include	<stdio.h>
#include	<dos.h>
#include	<conio.h>
#include	"screen.h"
#include  "wire.h"

// Data for shapes:

// Vertex data for W:

vertex_type W_vertices[]={      // Vertices for W
	{-25,-15,10,1,					      // Vertex 0
		0,0,0,1,
		0,0,0,1},
	 {	-15,-15,10,1,					      // Vertex 1
		0,0,0,1,
		0,0,0,1},
	 {-10,0,10,1,						      // Vertex 2
		0,0,0,1,
		0,0,0,1},
	 {-5,-15,10,1,					      // Vertex 3
		0,0,0,1,
		0,0,0,1},
	  {5,-15,10,1,				          // Vertex 4
		0,0,0,1,
		0,0,0,1},
		{10,0,10,1,					        // Vertex 5
		0,0,0,1,
		0,0,0,1},
		{15,-15,10,1,					      // Vertex 6
		0,0,0,1,
		0,0,0,1},
		{25,-15,10,1,					      // Vertex 7
		0,0,0,1,
		0,0,0,1},
	 {20,15,10,1,                 // Vertex 8
	 0,0,0,1,
	 0,0,0,1},
	 {7,15,10,1,                  // Vertex 9
	 0,0,0,1,
	 0,0,0,1},
	 {0,0,10,1,                   // Vertex 10
	 0,0,0,1,
	 0,0,0,1},
	 {-7,15,10,1,                 // Vertex 11
	 0,0,0,1,
	 0,0,0,1},
	 {-20,15,10,1,                // Vertex 12
	 0,0,0,1,
	 0,0,0,1},
	{-25,-15,-10,1,					    // Vertex 13
		0,0,0,1,
		0,0,0,1},
	  {-15,-15,-10,1,					    // Vertex 14
		0,0,0,1,
		0,0,0,1},
		{-10,0,-10,1,						    // Vertex 15
		0,0,0,1,
		0,0,0,1},
		{-5,-15,-10,1,					      // Vertex 16
		0,0,0,1,
		0,0,0,1},
		{5,-15,-10,1,				        // Vertex 17
		0,0,0,1,
		0,0,0,1},
		{10,0,-10,1,					        // Vertex 18
		0,0,0,1,
		0,0,0,1},
		{15,-15,-10,1,					      // Vertex 19
		0,0,0,1,
		0,0,0,1},
		{25,-15,-10,1,					      // Vertex 20
		0,0,0,1,
		0,0,0,1},
	 {20,15,-10,1,                // Vertex 21
	 0,0,0,1,
	 0,0,0,1},
	 {7,15,-10,1,                 // Vertex 22
	 0,0,0,1,
	 0,0,0,1},
	 {0,0,-10,1,                  // Vertex 23
	 0,0,0,1,
	 0,0,0,1},
	 {-7,15,-10,1,                // Vertex 24
	 0,0,0,1,
	 0,0,0,1},
	 {-20,15,-10,1,               // Vertex 25
	 0,0,0,1,
	 0,0,0,1}
};

// Line data for W:

line_type W_lines[]={
	 {0,1},                       // Line 0
	 {1,2},                       // Line 1
	 {2,3},                       // Line 2
	 {3,4},                       // Line 3
	 {4,5},                       // Line 4
	 {5,6},                       // Line 5
	 {6,7},                       // Line 6
	 {7,8},							  // Line 7
	 {8,9},                      // Line 8
	 {9,10},                      // Line 9
	 {10,11},                     // Line 10
	 {11,12},                     // Line 11
	 {12,0},                      // Line 12
	 {13,14},                     // Line 13
	 {14,15},                     // Line 14
	 {15,16},                     // Line 15
	 {16,17},                     // Line 16
	 {17,18},                     // Line 17
	 {18,19},                     // Line 18
	 {19,20},                     // Line 19
	 {20,21},                     // Line 20
	 {21,22},                     // Line 21
	 {22,23},                     // Line 22
	 {23,24},                     // Line 23
	 {24,25},                     // Line 24
	 {25,13},                     // Line 25
	 {0,13},                      // Line 26
	 {1,14},                      // Line 27
	 {2,15},                      // Line 28
	 {3,16},                      // Line 29
	 {4,17},                      // Line 30
	 {5,18},                      // Line 31
	 {6,19},                      // Line 32
	 {7,20},                      // Line 33
	 {8,21},                      // Line 34
	 {9,22},                      // Line 35
	 {10,23},                     // Line 36
	 {11,24},                     // Line 37
	 {12,25}                      // Line 38
};

// Shape data for W:

Shape W_shape(26, W_vertices, 39, W_lines);

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
	 scale(1);                       // Create scaling
												//  matrix
	 rotate(xangle,yangle,zangle);   // Create rotation
												//  matrices
	 xangle+=xrot;                   // Increment rotation
	 yangle+=yrot;                   //  angles
	 zangle+=zrot;
	 translate(0,0,60);              // Create translation
												//  matrix
	 W_shape.Transform();            // Transform SHAPE
												//  using MATRIX
	 W_shape.Project(160);          // Perform perspective
												//  projection
	 W_shape.Draw(screen_buffer); // Draw transformed
													//  shape
	 putwindow(0,0,320,200,screen_buffer); // Put on screen
  }
	setgmode(oldmode);                 // Reset previous video
												//  mode & end
	if( screen_buffer)
		delete [] screen_buffer;
}
