
// DRAWPOLY.CPP
//   Draws a polygon with an arbitrary number of sides in a specified color
// Copyright 1993 by Christopher Lampton and The Waite Group Press
//
// 9/20/95 mracky@sirius.com updated to C++ function

#include	<stdio.h>
#include	<dos.h>
#include  <mem.h>
#include  "poly_fr.h"

void Polygon::DrawPoly(unsigned char far *screen_buffer)
{

// Draw polygon in structure POLYGON in SCREEN_BUFFER

// Uninitialized variables:

  unsigned int start;        // Starting offset of line between edges
               
	int ydiff1,ydiff2,         // Difference between starting x and ending x
      xdiff1,xdiff2,         // Difference between starting y and ending y
      length,                // Distance from edge 1 to edge 2
      xunit1,xunit2,         // Unit to advance x offset for edges 1 & 2
	  intincr1,intincr2,     // Standard integer increments for x on edges 1 & 2
      errorincr1,errorincr2; // Error increments for x on edges 1 & 2

  // Initialize count of number of edges drawn:

	int edgecount=number_of_vertices-1;

  // Determine which vertex is at top of polygon:

	int firstvert=0;           // Start by assuming vertex 0 is at top
	int min_amt=vertex[0]->sy; // Find y coordinate of vertex 0
	for (int i=1; i< number_of_vertices; i++) {  // Search thru vertices
		if ((vertex[i]->sy) < min_amt) {  // Is another vertex higher?
			firstvert=i;                   // If so, replace previous top vertex
			min_amt=vertex[i]->sy;
		}
	}
  // Finding starting and ending vertices of first two edges:

	int startvert1=firstvert;      // Get starting vertex of edge 1
	int startvert2=firstvert;      // Get starting vertex of edge 2
	int xstart1=vertex[startvert1]->sx;
	int ystart1=vertex[startvert1]->sy;
	int xstart2=vertex[startvert2]->sx;
	int ystart2=vertex[startvert2]->sy;

	int endvert1=startvert1-1;                   // Get ending vertex of edge 1
	if (endvert1<0) endvert1=number_of_vertices-1;  // Check for wrap
	int xend1=vertex[endvert1]->sx;      // Get x & y coordinates
	int yend1=vertex[endvert1]->sy;      // of ending vertices
	int endvert2=startvert2+1;                   // Get ending vertex of edge 2
	if (endvert2==(number_of_vertices)) endvert2=0;  // Check for wrap
	int xend2=vertex[endvert2]->sx;      // Get x & y coordinates
	int yend2=vertex[endvert2]->sy;      // of ending vertices
  
  // Draw the polygon:
  unsigned char hue = (unsigned char)color;

  unsigned int offset1=320*ystart1+xstart1;  // Offset of edge 1
  int errorterm1=0;           // Initialize error terms
  if ((ydiff1=yend1-ystart1)<0) ydiff1=-ydiff1; // Get absolute value of
  int count1=ydiff1;
  if (count1) {
    xdiff1=xend1-xstart1;
    intincr1=xdiff1/ydiff1;
    if (xdiff1<0) {                               // Get value of length
      xunit1=-1;                                  // Calculate X increment
      errorincr1=-xdiff1%ydiff1;
    }
    else {
      xunit1=1;
      errorincr1=xdiff1%ydiff1;
    }
  }
  
  unsigned int offset2=320*ystart2+xstart2;  // Offset of edge 2
  int errorterm2=0;           // for edges 1 & 2
  if ((ydiff2=yend2-ystart2)<0) ydiff2=-ydiff2; // x & y lengths of edges
  int count2=ydiff2;
  if (count2) {
    xdiff2=xend2-xstart2;
    intincr2=xdiff2/ydiff2;
    if (xdiff2<0) {                               // Get value of length
      xunit2=-1;                                  // Calculate X increment
      errorincr2=-xdiff2%ydiff2;
    }
    else {
      xunit2=1;
      errorincr2=xdiff2%ydiff2;
    }
  }
    
 	while (edgecount>0) {    // Continue drawing until all edges drawn
    for (; count1&&count2; count1--,count2--) {
      length=offset2-offset1; // Determine length of horizontal line
      if (length<0) {         // If negative...
        length=-length;       // Make it positive
        start=offset2;        // And set START to edge 2
      }
      else start=offset1;     // Else set START to edge 1
      length++;
      for (unsigned char far *p=&screen_buffer[start];length--;)
        *p++=hue;
              
      offset1+=intincr1+320;
      errorterm1+=errorincr1;
      if (errorterm1>=ydiff1) {
        errorterm1-=ydiff1;
        offset1+=xunit1;
      }
      
      offset2+=intincr2+320;
      errorterm2+=errorincr2;
      if (errorterm2>=ydiff2) {
        errorterm2-=ydiff2;
        offset2+=xunit2;
      }
    }
    
    // Another edge (at least) is complete. Start next edge, if any.

		if (!count1) {           // If edge 1 is complete...
			--edgecount;           // Decrement the edge count
			startvert1=endvert1;   // Make ending vertex into start vertex
			--endvert1;            // And get new ending vertex
			if (endvert1<0) endvert1=number_of_vertices-1; // Check for wrap
      xstart1=xend1;
      ystart1=yend1;
			xend1=vertex[endvert1]->sx;  // Get x & y of new end vertex
			yend1=vertex[endvert1]->sy;
      
      offset1=320*ystart1+xstart1;  // Offset of edge 1
      errorterm1=0;           // Initialize error terms
      if ((ydiff1=yend1-ystart1)<0) ydiff1=-ydiff1; // Get absolute value of
      count1=ydiff1;
      if (count1) {
        xdiff1=xend1-xstart1;
        intincr1=xdiff1/ydiff1;
        if (xdiff1<0) {                               // Get value of length
          xunit1=-1;                                  // Calculate X increment
          errorincr1=-xdiff1%ydiff1;
        }
        else {
          xunit1=1;
          errorincr1=xdiff1%ydiff1;
        }
      }
		}
		if (!count2) {          // If edge 2 is complete...
			--edgecount;          // Decrement the edge count
			startvert2=endvert2;  // Make ending vertex into start vertex
			endvert2++;           // And get new ending vertex
			if (endvert2==(number_of_vertices)) endvert2=0; // Check for wrap
      xstart2=xend2;
      ystart2=yend2;
			xend2=vertex[endvert2]->sx;  // Get x & y of new end vertex
			yend2=vertex[endvert2]->sy;
      
      offset2=320*ystart2+xstart2;  // Offset of edge 2
      errorterm2=0;           // for edges 1 & 2
      if ((ydiff2=yend2-ystart2)<0) ydiff2=-ydiff2; // x & y lengths of edges
      count2=ydiff2;
      if (count2) {
        xdiff2=xend2-xstart2;
        intincr2=xdiff2/ydiff2;
        if (xdiff2<0) {                               // Get value of length
          xunit2=-1;                                  // Calculate X increment
          errorincr2=-xdiff2%ydiff2;
        }
        else {
          xunit2=1;
          errorincr2=xdiff2%ydiff2;
        }
      }
		}
	}
}
