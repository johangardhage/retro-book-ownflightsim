// TRANSLAT.CPP
//
// Copyright 1993 by Christopher Lampton and
// The Waite Group Press
//
// 8/31/95 mickRacky updated shape_type to class
#include	<stdio.h>
#include	<dos.h>
#include	<conio.h>
#include	"bresnham.h"
#include	"screen.h"

int const DISTANCE=10;

// Variable structures for shape data:

struct vertex_type {	    // Structure for vertices
	int lx,ly;  // local coordinates of vertex
	int sx,sy;	// dvreen coordinates of vertex
};

class shape_type {		    // Class to hold shape data
private:
  int color;					    // Color of shape
  int	number_of_vertices;	// Number of vertices in shape
  vertex_type *vertex;		// Array of vertex descriptor
public:
	shape_type(int num, vertex_type *vert,int col=15):vertex(vert),
			number_of_vertices(num),color(col) {;}
	void Draw(char far *screen);
	void Translate(int xtrans, int ytrans);
};

// Draws the shape
void shape_type::Draw(char far *screen)
{

	// Loop through vertices in shape:

	for (int i=0; i<number_of_vertices; i++) {

	 // Calculate offset of next vertex:

		int p2=i+1;

	 // Wrap to 0:

		if (p2>=number_of_vertices) p2=0;

		// Draw line from this vertex to next vertex:

		linedraw(vertex[i].sx,vertex[i].sy,
				  vertex[p2].sx,vertex[p2].sy,
				  color,screen);
	}
}

void shape_type:: Translate(int xtrans, int ytrans)
{
	// Translate each point in shape by xtrans/ytrans

	for(int i=0;i<number_of_vertices;i++) {
		vertex[i].sx = vertex[i].lx+xtrans;
		vertex[i].sy = vertex[i].ly+ytrans;
		}
}
// Data for rectangle:

vertex_type rectangle_array[]={
	 {0,0,		            // First vertex
	 0,0},
	 {0,20,	            // Second vertex
	 0,20},
	 {20,20,		            // Third vertex
	 20,20},
	 {20,0,                  // Fourth vertex
	 20,0}
};

// Rectangle:
//  4 vertices and  rectangle_array  is Pointer to vertex array
// 	draw it in bright red (12)
shape_type shape(4, rectangle_array, 12);

void main()
{
	char far *screen =
	 (char far *)MK_FP(0xa000,0);	  // Create pointer to
												//  video RAM
	cls(screen);											// Clear screen
	int oldmode=*(int *)MK_FP(0x40,0x49); // Save previous
													 //  video mode
	setgmode(0x13);								// Set mode 13h
	shape.Translate(150,90);
	shape.Draw(screen);					// Draw shape on
												//  display

	while (!kbhit());									// Wait for key, then
												//  terminate
	setgmode(oldmode);									// Reset previous video
												//  mode & end
}


