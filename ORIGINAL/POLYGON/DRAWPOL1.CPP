
// DRAWPOLY.CPP
//   Draws a polygon with an arbitrary number of sides in a specified color
// 9/1/95 mickRacky updated to member of polygon_type class

#include	<stdio.h>
#include	<dos.h>
#include  <mem.h>
#include  "poly.h"

void polygon_type::DrawPoly(unsigned char far *screen_buffer)
{

// Draw polygon in POLYGON_TYPE in SCREEN_BUFFER

// Uninitialized variables:

	int ydiff1,ydiff2,         // Difference between starting x and ending x
		xdiff1,xdiff2,         // Difference between starting y and ending y
		 start,                 // Starting offset of line between edges
		length,                // Distance from edge 1 to edge 2
		errorterm1,errorterm2, // Error terms for edges 1 & 2
		 offset1,offset2,       // Offset of current pixel in edges 1 & 2
		count1,count2,         // Increment count for edges 1 & 2
		 xunit1,xunit2;         // Unit to advance x offset for edges 1 & 2

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

	while (edgecount>0) {    // Continue drawing until all edges drawn
		offset1=320*ystart1+xstart1+FP_OFF(screen_buffer);  // Offset of edge 1
		offset2=320*ystart2+xstart2+FP_OFF(screen_buffer);  // Offset of edge 2
		errorterm1=0;           // Initialize error terms
		errorterm2=0;           // for edges 1 & 2
		if ((ydiff1=yend1-ystart1)<0) ydiff1=-ydiff1; // Get absolute value of
		if ((ydiff2=yend2-ystart2)<0) ydiff2=-ydiff2; // x & y lengths of edges
		if ((xdiff1=xend1-xstart1)<0) {               // Get value of length
			xunit1=-1;                                  // Calculate X increment
			xdiff1=-xdiff1;
		}
		else {
			xunit1=1;
		}
		if ((xdiff2=xend2-xstart2)<0) {               // Get value of length
			xunit2=-1;                                  // Calculate X increment
			xdiff2=-xdiff2;
		}
		else {
			xunit2=1;
		}

	 // Choose which of four routines to use:

		if (xdiff1>ydiff1) {    // If X length of edge 1 is greater than y length
			if (xdiff2>ydiff2) {  // If X length of edge 2 is greater than y length

			// Increment edge 1 on X and edge 2 on X:

				count1=xdiff1;    // Count for x increment on edge 1
				count2=xdiff2;    // Count for x increment on edge 2
				while (count1 && count2) {  // Continue drawing until one edge is done

			 // Calculate edge 1:

					while ((errorterm1<xdiff1)&&(count1>0)) { // Finished w/edge 1?
						if (count1--) {     // Count down on edge 1
							offset1+=xunit1;  // Increment pixel offset
							xstart1+=xunit1;
						}
						errorterm1+=ydiff1; // Increment error term
						if (errorterm1<xdiff1) {  // If not more than XDIFF
							screen_buffer[offset1]=color; // ...plot a pixel
						}
					}
					errorterm1-=xdiff1; // If time to increment X, restore error term

			 // Calculate edge 2:

					while ((errorterm2<xdiff2)&&(count2>0)) {  // Finished w/edge 2?
						if (count2--) {     // Count down on edge 2
							offset2+=xunit2;  // Increment pixel offset
							xstart2+=xunit2;
						}
						errorterm2+=ydiff2; // Increment error term
						if (errorterm2<xdiff2) {  // If not more than XDIFF
							screen_buffer[offset2]=color;  // ...plot a pixel
						}
					}
					errorterm2-=xdiff2; // If time to increment X, restore error term

			 // Draw line from edge 1 to edge 2:

					length=offset2-offset1; // Determine length of horizontal line
					if (length<0) {         // If negative...
						length=-length;       // Make it positive
						start=offset2;        // And set START to edge 2
					}
					else start=offset1;     // Else set START to edge 1
			 for (int i=start; i<start+length+1; i++)  // From edge to edge...
				screen_buffer[i]=color;         // ...draw the line
					offset1+=320;           // Advance edge 1 offset to next line
					ystart1++;
					offset2+=320;           // Advance edge 2 offset to next line
					ystart2++;
		  }
			}
			else {

			// Increment edge 1 on X and edge 2 on Y:

				count1=xdiff1;    // Count for X increment on edge 1
				count2=ydiff2;    // Count for Y increment on edge 2
				while (count1 && count2) {  // Continue drawing until one edge is done

			 // Calculate edge 1:

					while ((errorterm1<xdiff1)&&(count1>0)) { // Finished w/edge 1?
						if (count1--) {     // Count down on edge 1
							offset1+=xunit1;  // Increment pixel offset
							xstart1+=xunit1;
						}
						errorterm1+=ydiff1; // Increment error term
						if (errorterm1<xdiff1) {  // If not more than XDIFF
							screen_buffer[offset1]=color; // ...plot a pixel
						}
					}
					errorterm1-=xdiff1; // If time to increment X, restore error term

			 // Calculate edge 2:

					errorterm2+=xdiff2; // Increment error term
					if (errorterm2 >= ydiff2)  { // If time to increment Y...
						errorterm2-=ydiff2;        // ...restore error term
						offset2+=xunit2;           // ...and advance offset to next pixel
						xstart2+=xunit2;
					}
			 --count2;

			 // Draw line from edge 1 to edge 2:

					length=offset2-offset1; // Determine length of horizontal line
					if (length<0) {         // If negative...
						length=-length;       // ...make it positive
						start=offset2;        // And set START to edge 2
					}
					else start=offset1;     // Else set START to edge 1
			 for (int i=start; i<start+length+1; i++)  // From edge to edge
				screen_buffer[i]=color;         // ...draw the linee
					offset1+=320;           // Advance edge 1 offset to next line
					ystart1++;
					offset2+=320;           // Advance edge 2 offset to next line
					ystart2++;
				}
			}
		}
		else {
			if (xdiff2>ydiff2) {

		// Increment edge 1 on Y and edge 2 on X:

				count1=ydiff1;  // Count for Y increment on edge 1
				count2=xdiff2;  // Count for X increment on edge 2
				while(count1 && count2) {  // Continue drawing until one edge is done

			 // Calculate edge 1:

					errorterm1+=xdiff1; // Increment error term
					if (errorterm1 >= ydiff1)  {  // If time to increment Y...
						errorterm1-=ydiff1;         // ...restore error term
						offset1+=xunit1;            // ...and advance offset to next pixel
						xstart1+=xunit1;
					}
			 --count1;

			 // Calculate edge 2:

					while ((errorterm2<xdiff2)&&(count2>0)) { // Finished w/edge 1?
						if (count2--) {     // Count down on edge 2
							offset2+=xunit2;  // Increment pixel offset
							xstart2+=xunit2;
						}
						errorterm2+=ydiff2; // Increment error term
						if (errorterm2<xdiff2) {  // If not more than XDIFF
							screen_buffer[offset2]=color; // ...plot a pixel
						}
					}
					errorterm2-=xdiff2;  // If time to increment X, restore error term

			 // Draw line from edge 1 to edge 2:

					length=offset2-offset1; // Determine length of horizontal line
					if (length<0) {    // If negative...
						length=-length;  // ...make it positive
						start=offset2;   // And set START to edge 2
					}
					else start=offset1;  // Else set START to edge 1
			 for (int i=start; i<start+length+1; i++) // From edge to edge...
				screen_buffer[i]=color;        // ...draw the line
					offset1+=320;         // Advance edge 1 offset to next line
					ystart1++;
					offset2+=320;         // Advance edge 2 offset to next line
					ystart2++;
		  }
			}
			else {

			// Increment edge 1 on Y and edge 2 on Y:

				count1=ydiff1;  // Count for Y increment on edge 1
				count2=ydiff2;  // Count for Y increment on edge 2
				while(count1 && count2) {  // Continue drawing until one edge is done

			 // Calculate edge 1:

					errorterm1+=xdiff1;  // Increment error term
					if (errorterm1 >= ydiff1)  {  // If time to increment Y
						errorterm1-=ydiff1;         // ...restore error term
						offset1+=xunit1;            // ...and advance offset to next pixel
						xstart1+=xunit1;
					}
			 --count1;

			 // Calculate edge 2:

					errorterm2+=xdiff2; // Increment error term
					if (errorterm2 >= ydiff2)  {  // If time to increment Y
						errorterm2-=ydiff2;         // ...restore error term
						offset2+=xunit2;            // ...and advance offset to next pixel
						xstart2+=xunit2;
					}
			 --count2;

			 // Draw line from edge 1 to edge 2:

					length=offset2-offset1;  // Determine length of horizontal line
					if (length<0) {          // If negative...
						length=-length;        // ...make it positive
						start=offset2;         // And set START to edge 2
					}
					else start=offset1;      // Else set START to edge 1
			 for (int i=start; i<start+length+1; i++)   // From edge to edge
				screen_buffer[i]=color;          // ...draw the linee
					offset1+=320;            // Advance edge 1 offset to next line
					ystart1++;
					offset2+=320;            // Advance edge 2 offset to next line
					ystart2++;
				}
			}
		}
	 // Another edge (at least) is complete. Start next edge, if any.

		if (!count1) {           // If edge 1 is complete...
			--edgecount;           // Decrement the edge count
			startvert1=endvert1;   // Make ending vertex into start vertex
			--endvert1;            // And get new ending vertex
			if (endvert1<0) endvert1=number_of_vertices-1; // Check for wrap
			xend1=vertex[endvert1]->sx;  // Get x & y of new end vertex
			yend1=vertex[endvert1]->sy;
		}
		if (!count2) {          // If edge 2 is complete...
			--edgecount;          // Decrement the edge count
			startvert2=endvert2;  // Make ending vertex into start vertex
			endvert2++;           // And get new ending vertex
			if (endvert2==(number_of_vertices)) endvert2=0; // Check for wrap
			xend2=vertex[endvert2]->sx;  // Get x & y of new end vertex
			yend2=vertex[endvert2]->sy;
		}
	}
}
