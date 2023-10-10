
// SHAPE.CPP
//  A short program to draw shapes on the mode 13h
// display based on data contained in a structure
// of type SHAPE_TYPE
//
// Copyright 1993 by Christopher Lampton and
// The Waite Group Press

#include	<stdio.h>
#include	<dos.h>
#include	<conio.h>
#include	"bresnham.h"
#include	"screen.h"

int const DISTANCE=10;

// Variable structures for shape data:

struct vertex_type {	    // Structure for vertices
  int x,y;	              // X & Y coordinates for vertex
};

class shape_type {		    // Structure to hold shape data
private:
  int color;					    // Color of shape
  int	number_of_vertices;	// Number of vertices in shape
  vertex_type *vertex;		// Array of vertex descriptor
public:
	shape_type(int num, vertex_type *vert,int col=15):vertex(vert),
			number_of_vertices(num),color(col) {;}
	void Draw(char far *screen);
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

		linedraw(vertex[i].x,vertex[i].y,
				  vertex[p2].x,vertex[p2].y,
				  color,screen);
	}
}

// Data for triangle:

vertex_type triangle_array[]={
	 160,70,		            // First vertex
    190,130,	            // Second vertex
    130,130		            // Third vertex
};

// Triangle:
//  3 vertices and    triangle_array  is Pointer to vertex array
shape_type shape(3, triangle_array);

void main()
{
	char far *screen =
	 (char far *)MK_FP(0xa000,0);	  // Create pointer to
                                    //  video RAM
	cls(screen);											// Clear screen
	int oldmode=*(int *)MK_FP(0x40,0x49); // Save previous
													 //  video mode
	setgmode(0x13);										// Set mode 13h
	shape.Draw(screen);					// Draw shape on
												//  display

	while (!kbhit());									// Wait for key, then
												//  terminate
	setgmode(oldmode);									// Reset previous video
												//  mode & end
}


