// DRAWPOL2.CPP
//   Draws function for polygon_type class 
// 9/1/95 mickRacky updated to member of polygon_type class

#include	<stdio.h>
#include	<dos.h>
#include  <mem.h>
#include  "poly.h"

void polygon_type::DrawPoly(unsigned char far *screen_buffer)
{

// Draw polygon in POLYGON_TYPE in SCREEN_BUFFER

	int ydiff1,ydiff2,xdiff1,xdiff2;
	int	ptr,start,length,errorterm1,errorterm2;
	int offset1,offset2,count1,count2;
	int xunit1,xunit2;
	int edgecount=number_of_vertices-2;
	int firstvert=0;
	int min_amt=vertex[0]->sy;
	for (int i=1; i<number_of_vertices; i++) {
		if ((vertex[i]->sy) < min_amt) {
			firstvert=i;
			min_amt=vertex[i]->sy;
		}
	}
	int startvert1=firstvert;
	int startvert2=firstvert;
	int xstart1=vertex[startvert1]->sx;
	int ystart1=vertex[startvert1]->sy;
	int xstart2=vertex[startvert2]->sx;
	int ystart2=vertex[startvert2]->sy;
	int endvert1=startvert1-1;
	if (endvert1<0) endvert1=number_of_vertices-1;
	int xend1=vertex[endvert1]->sx;
	int yend1=vertex[endvert1]->sy;
	int endvert2=startvert2+1;
	if (endvert2==(number_of_vertices)) endvert2=0;
	int xend2=vertex[endvert2]->sx;
	int yend2=vertex[endvert2]->sy;
	while (edgecount>=0) {
		offset1=320*ystart1+xstart1+FP_OFF(screen_buffer);
		offset2=320*ystart2+xstart2+FP_OFF(screen_buffer);
		errorterm1=0;
		errorterm2=0;
		if ((ydiff1=yend1-ystart1)<0) ydiff1=-ydiff1;
		if ((ydiff2=yend2-ystart2)<0) ydiff2=-ydiff2;
		if ((xdiff1=xend1-xstart1)<0) {
			xunit1=-1;
			xdiff1=-xdiff1;
		}
		else {
			xunit1=1;
		}
		if ((xdiff2=xend2-xstart2)<0) {
			xunit2=-1;
			xdiff2=-xdiff2;
		}
		else {
			xunit2=1;
		}
		if (xdiff1>ydiff1) {
			if (xdiff2>ydiff2) {
			// xx
				count1=xdiff1+1;
				count2=xdiff2+1;
				while (count1 && count2) {
					while ((errorterm1<xdiff1)&&(count1>0)) {
						if (--count1) {
							offset1+=xunit1;
							xstart1+=xunit1;
						}
						errorterm1+=ydiff1;
						if (errorterm1<xdiff1) {
							screen_buffer[offset1]=color;
						}
					}
					errorterm1-=xdiff1;
					while ((errorterm2<xdiff2)&&(count2>0)) {
						if (--count2) {
							offset2+=xunit2;
							xstart2+=xunit2;
						}
						errorterm2+=ydiff2;
						if (errorterm2<xdiff2) {
							screen_buffer[offset2]=color;
						}
					}
					errorterm2-=xdiff2;
					length=offset2-offset1;
					if (length<0) {
						length=-length;
						start=offset2;
					}
					else start=offset1;
          setmem(&screen_buffer[start],length+1,color);
					offset1+=320;
					ystart1++;
					offset2+=320;
					ystart2++;
        }
			}
			else {
			// xy
				count1=xdiff1+1;
				count2=ydiff2+1;
				while (count1 && count2) {
					while ((errorterm1<xdiff1)&&(count1>0)) {
						if (--count1) {
							offset1+=xunit1;
							xstart1+=xunit1;
						}
						errorterm1+=ydiff1;
						if (errorterm1<xdiff1) {
							screen_buffer[offset1]=color;
						}
					}
					errorterm1-=xdiff1;
					errorterm2+=xdiff2;
					if (errorterm2 >= ydiff2)  {
						errorterm2-=ydiff2;
						offset2+=xunit2;
						xstart2+=xunit2;
					}
					length=offset2-offset1;
					if (length<0) {
						length=-length;
						start=offset2;
					}
					else start=offset1;
          setmem(&screen_buffer[start],length+1,color);
					offset1+=320;
					ystart1++;
					offset2+=320;
					ystart2++;
					--count2;
				}
			}
		}
		else {
			if (xdiff2>ydiff2) {
			// yx
				count1=ydiff1+1;
				count2=xdiff2+1;
				while(count1 && count2) {
					errorterm1+=xdiff1;
					if (errorterm1 >= ydiff1)  {
						errorterm1-=ydiff1;
						offset1+=xunit1;
						xstart1+=xunit1;
					}
					while ((errorterm2<xdiff2)&&(count2>0)) {
						if (--count2) {
							offset2+=xunit2;
							xstart2+=xunit2;
						}
						errorterm2+=ydiff2;
						if (errorterm2<xdiff2) {
							screen_buffer[offset2]=color;
						}
					}
					errorterm2-=xdiff2;
					length=offset2-offset1;
					if (length<0) {
						length=-length;
						start=offset2;
					}
					else start=offset1;
          setmem(&screen_buffer[start],length+1,color);
					offset1+=320;
					ystart1++;
					offset2+=320;
					ystart2++;
					--count1;
        }
			}
			else {
			// yy
				count1=ydiff1+1;
				count2=ydiff2+1;
				while(count1 && count2) {
					errorterm1+=xdiff1;
					if (errorterm1 >= ydiff1)  {
						errorterm1-=ydiff1;
						offset1+=xunit1;
						xstart1+=xunit1;
					}
					errorterm2+=xdiff2;
					if (errorterm2 >= ydiff2)  {
						errorterm2-=ydiff2;
						offset2+=xunit2;
						xstart2+=xunit2;
					}
					length=offset2-offset1;
					if (length<0) {
						length=-length;
						start=offset2;
					}
					else start=offset1;
          setmem(&screen_buffer[start],length+1,color);
					offset1+=320;
					ystart1++;
					offset2+=320;
					ystart2++;
					--count1;
					--count2;
				}
			}
		}
		if (!count1) {
			--edgecount;
			startvert1=endvert1;
			--endvert1;
			if (endvert1<0) endvert1=number_of_vertices-1;
			xend1=vertex[endvert1]->sx;
			yend1=vertex[endvert1]->sy;
		}
		if (!count2) {
			--edgecount;
			startvert2=endvert2;
			endvert2++;
			if (endvert2==(number_of_vertices)) endvert2=0;
			xend2=vertex[endvert2]->sx;
			yend2=vertex[endvert2]->sy;
		}
	}
}
