// ROTATE2.CPP
// This program is same functionality as ROTATE but it uses matrix math
//
// Copyright 1993 by Christopher Lampton and
// The Waite Group Press
//
// 8/31/95 mickRacky rewrite/update shape_type to class
//
#include	<stdio.h>
#include	<dos.h>
#include	<conio.h>
#include	<math.h>
#include	"bresnham.h"
#include	"screen.h"

int const DISTANCE=10;

// Variable structures for shape data:

struct vertex_type {	    // Structure for vertices
	int lx,ly,lt;    // local coordinates of vertex
	int sx,sy,st;	 // screen coordinates of vertex
};

class shape_type {
private:
  int color;					    // Color of shape
  int	number_of_vertices;	// Number of vertices in shape
  vertex_type *vertex;		// Array of vertex descriptor
public:
	shape_type(int num, vertex_type *vert,int col=15):vertex(vert),
			number_of_vertices(num),color(col) {;}
	void Draw(char far *screen);
	// The Translate,Scale and Rotate functions are 
	// all combined into one transformation function:
	void Transform(float scale_factor,int xtrans, int ytrans, float angle);
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

void shape_type::Transform(float scale_factor,int xtrans, int ytrans,float angle)
{
	float matrix[3][3];	// transformation matrix
	float smatrix[3][3]; // scaling matrix
	float rtmatrix[3][3]; // rotation & translation matrix

	// Initialize scaling matrix
	smatrix[0][0]= scale_factor;
	smatrix[0][1]= 0; 
	smatrix[0][2]= 0; 

	smatrix[1][0]= 0; 
	smatrix[1][1]= scale_factor; 
	smatrix[1][2]= 0; 

	smatrix[2][0]= 0; 
	smatrix[2][1]= 0; 
	smatrix[2][2]= 1; 

	// Initialize rotation & translation matrix:
	rtmatrix[0][0] = cos(angle);
	rtmatrix[0][1] = -sin(angle);
	rtmatrix[0][2] = 0;

	rtmatrix[1][0] = sin(angle);
	rtmatrix[1][1] = cos(angle);
	rtmatrix[1][2] = 0;

	rtmatrix[2][0] = xtrans;
	rtmatrix[2][1] = ytrans;
	rtmatrix[2][2] = 0;

	// multiply together to get the transformation matrix

	for(int i=0; i<3; i++) {
		for(int j=0; j<3; j++) {
			matrix[i][j]=0;
			for(int k=0; k<3;k++)
				matrix[i][j]+= smatrix[i][k] * rtmatrix[k][j];
		}
	}

	// multiply all vertices by transformation matrix:

	for(int v=0; v< number_of_vertices;v++) {

		// initialize temporary variables:
		int temp0 = 0;
		int temp1 = 0;
		int temp2 = 0;

		// accummulate results in temporary variables:
		temp0 += vertex[v].lx * matrix[0][0] +
			vertex[v].ly * matrix[1][0] + matrix[2][0];
		temp1 += vertex[v].lx * matrix[0][1] +
			vertex[v].ly * matrix[1][1] + matrix[2][1];
		temp2 += vertex[v].lx * matrix[0][2] +
			vertex[v].ly * matrix[1][2] + matrix[2][2];

		// Transfer results to screen coordinates

		vertex[v].sx = temp0;
		vertex[v].sy = temp1;
		vertex[v].st = temp2;
	}
}


// Data for rectangle:

vertex_type rectangle_array[]={
	 {-10,-10,1,            // First vertex
	 0,0,1},
	 {10,-10,1,	            // Second vertex
	 0,0,1},
	 {10,10,1,              // Third vertex
	 0,0,1},
	 {-10,10,1,             // Fourth vertex
	 0,0,1}
};

// Rectangle:
//  4 vertices and  rectangle_array  is Pointer to vertex array
shape_type shape(4, rectangle_array);

void main()
{
	char far *screen =
	 (char far *)MK_FP(0xa000,0);	  // Create pointer to
												//  video RAM
	cls(screen);											// Clear screen
	int oldmode=*(int *)MK_FP(0x40,0x49); // Save previous
													 //  video mode
	setgmode(0x13);								// Set mode 13h

	shape.Transform(1.5,160,100,0.7);
	shape.Draw(screen);					// Draw shape on
												//  display

	while (!kbhit());									// Wait for key, then
												//  terminate
	setgmode(oldmode);									// Reset previous video
												//  mode & end
}


