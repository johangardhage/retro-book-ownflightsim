
// DRAWPOLG.CPP
//   Draws a polygon with an arbitrary number of sides in a specified color
//    using gouraud shading.
// Copyright 1993 by Christopher Lampton and The Waite Group Press
//
// 7/19/95 Dale Sather       optimized and included gouraud shading
// 9/20/95 mracky@sirius.com updated to C++ function

#include  "poly_g.h"
#include  "palette.h"

void Polygon::DrawPoly(unsigned char far *screen_buffer,Palette *palette,int ambient)
{
 // Draw polygon in structure POLYGON in SCREEN_BUFFER

// Uninitialized variables:

  unsigned int start,        // Starting offset of line between edges
               offset1,      // Offset of current pixel in edges 1 & 2
               offset2;      
               
	int ydiff1,ydiff2,         // Difference between starting x and ending x
      xdiff1,xdiff2,         // Difference between starting y and ending y
      cdiff1,cdiff2,         // Difference between starting color and ending color
      length,                // Distance from edge 1 to edge 2
      errorterm1,errorterm2, // Error terms for edges 1 & 2
      count1,count2,         // Increment count for edges 1 & 2
      xunit1,xunit2,         // Unit to advance x offset for edges 1 & 2
		intincr1,intincr2,     // Standard integer increments for x on edges 1 & 2
      errorincr1,errorincr2, // Error increments for x on edges 1 & 2
      cstart1,cstart2,       // Starting edge colors
      cend1,cend2,           // Ending edge colors
      cerrorterm1,cerrorterm2,// Error terms for color for edges 1 & 2
      cunit1,cunit2,         // Unit to advance color for edges 1 & 2
      cintincr1,cintincr2,   // Standard integer color increments on edges 1 & 2
		cerrorincr1,cerrorincr2, // Error color increments on edges 1 & 2
      cdiff, 
      cstart,                // Starting edge colors
      cend,                  // Ending edge colors
      cerrorterm,            // Error terms for color for edges 1 & 2
      cunit,                 // Unit to advance color for edges 1 & 2
      cintincr,              // Standard integer color increments on edges 1 & 2
		cerrorincr;            // Error color increments on edges 1 & 2

  // Initialize count of number of edges drawn:

	int edgecount=number_of_vertices-1;

  // Determine which vertex is at top of polygon:

	int firstvert=0;           // Start by assuming vertex 0 is at top
	int min_amt=vertex[0]->sy; // Find y coordinate of vertex 0
	for (int i=1; i<number_of_vertices; i++) {  // Search thru vertices
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
  char calc1=1;
  char calc2=1;

 	while (edgecount>0) {    // Continue drawing until all edges drawn

    if (calc1) {
      calc1=0;
      offset1=320*ystart1+xstart1;  // Offset of edge 1
		errorterm1=0;                 // Initialize error term
      if ((ydiff1=yend1-ystart1)<0) ydiff1=-ydiff1; // Get abs value of y length
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

		  cstart1=vertex[startvert1]->brightness;
		  cend1=vertex[endvert1]->brightness;
		  cerrorterm1=0;
		  cdiff1=cend1-cstart1;
		  cintincr1=cdiff1/ydiff1;
		  if (cdiff1<0) {
			 cunit1=-1;
			 cerrorincr1=-cdiff1%ydiff1;
		  }
        else {
          cunit1=1;
          cerrorincr1=cdiff1%ydiff1;
        }
		}
	}
    
    if (calc2) {
      calc2=0;
      offset2=320*ystart2+xstart2;  // Offset of edge 2
      errorterm2=0;
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
        
		  cstart2=vertex[startvert2]->brightness;
		  cend2=vertex[endvert2]->brightness;
		  cerrorterm2=0;
		  cdiff2=cend2-cstart2;
		  cintincr2=cdiff2/ydiff2;
		  if (cdiff2<0) {
			 cunit2=-1;
			 cerrorincr2=-cdiff2%ydiff2;
		  }
		  else {
			 cunit2=1;
			 cerrorincr2=cdiff2%ydiff2;
		  }
	  }
	 }

	 for (; count1&&count2; count1--,count2--) {
		length=offset2-offset1; // Determine length of horizontal line
		if (length<0) {         // If negative...
		  length=-length;       // Make it positive
		  start=offset2;        // And set START to edge 2
		  cstart=cstart2;
		  cend=cstart1;
		}
		else {
		  start=offset1;     // Else set START to edge 1
		  cstart=cstart1;
        cend=cstart2;
		}
      length++;
      
      cerrorterm=0;
      cdiff=cend-cstart;
      cintincr=cdiff/length;
      if (cdiff<0) {
		  cunit=-1;
        cerrorincr=-cdiff%length;
      }
		else {
        cunit=1;
        cerrorincr=cdiff%length;
      }

      int l=length;
      for (unsigned char far *p=&screen_buffer[start];l--;)
      {
		*p++=palette->ColorIndex(original_color,cstart,ambient);
        cstart+=cintincr;
        cerrorterm+=cerrorincr;
		  if (cerrorterm>=length) {
          cerrorterm-=length;
          cstart+=cunit;
        }
		}
              
      offset1+=intincr1+320;
		errorterm1+=errorincr1;
      if (errorterm1>=ydiff1) {
        errorterm1-=ydiff1;
        offset1+=xunit1;
      }
      cstart1+=cintincr1;
      cerrorterm1+=cerrorincr1;
		if (cerrorterm1>=ydiff1) {
        cerrorterm1-=ydiff1;
        cstart1+=cunit1;
      }
      
      offset2+=intincr2+320;
      errorterm2+=errorincr2;
		if (errorterm2>=ydiff2) {
        errorterm2-=ydiff2;
        offset2+=xunit2;
      }
      cstart2+=cintincr2;
      cerrorterm2+=cerrorincr2;
      if (cerrorterm2>=ydiff2) {
		  cerrorterm2-=ydiff2;
        cstart2+=cunit2;
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
      calc1=1;
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
      calc2=1;
		}
	}
}
