//MOUNTBG.CPP
//      
// Copyright 1993 by Christopher Lampton and The Waite Group Press
//
//  10/19/95  mracky 	created file
//
/*
	the main objective is to use the previously created idea from the 
	chapter about mountain's to create a mountain in the far background.
	if we assume that these mountains will ALWAYS be in the background and 
	never get any closer, then we can imagine a mountain range in a 360
	degree circle.  Each view from our plane shows a 90 degree section of 
	that circle.  Thus it is the job of the mountain bg to construct a
	polygon which will be painted to the background page.

	The first of these functions initiates a background mountain range. 
	The second grabs the background polygon from the 

*/
#include  <stdio.h>
#include  <alloc.h>
#include  <math.h>
#include  "fix.h"
#include  "poly2.h"
#include  "view.h"
#include  "screen.h"

#define MOUNTAIN_PTS (4*SCREEN_WIDTH)

struct _pt {
	int x,y;
};
class MountainRange
{
	private:
	_pt	pointtab[MOUNTAIN_PTS+SCREEN_WIDTH];
	int current;
	void MountainRange::LineGen(_pt lf, _pt rt, int depth, int vary);
	public:
		MountainRange(){;}
		_pt& FindAngle(int degree);
		void Generate();
};

// this degree is in fixed angle radius (255)
_pt& MountainRange::FindAngle(int degree)
{
		
}

void MountainRange::Generate()
{
	current = 0;			
}

void MountainRange::LineGen(int h1, int h2, int v1, int v2, int depth, int vary)
{
	if( depth <= 0)
	{
		pointtab[current].x = h2;
		pointtab[current++].y = v2;
	}
	else
	{
		int midv = (v1+v2)/2 + random(vary)-vary/2;
		int midh = (h1+h2)/2;
    	// Call recursively with random perturbation:
		
	    LineGen(h1,midh,v1,midv,depth-1, vary/2);
    	LineGen(midh,h2,midv,v2,depth-1,vary/2);
	}
}

#pragma argsused
void View::DrawHorizon(int xangle,int yangle,int zangle,
						 unsigned char *screen)
{
  long rx1,rx2,temp_rx1,temp_rx2;
  long ry1,ry2,temp_ry1,temp_ry2;
  long rz1,rz2,temp_rz1,temp_rz2;

  vertex_type vert[4];
  vert[0].az=distance;
  vert[1].az=distance;
  vert[2].az=distance;
  vert[3].az=distance;

  ClippedPolygon hclip;
  localPolygon hpoly(4, vert);

  // Map rotation angle to remove backward wrap-around:

  int flip=0;
  xangle&=255;
  if ((xangle>64) && (xangle<193)) {
	xangle=(xangle+128) & 255;
	flip++;
  }
  zangle&=255;
  if ((zangle>64) && (zangle<193)) {
	flip++;
  }

  // Create initial horizon line:

  rx1=-100;  ry1=0; rz1=distance;
  rx2=100;   ry2=0; rz2=distance;

  // Rotate around viewer's X axis:

  temp_ry1=(ry1*COS(xangle) - rz1*SIN(xangle)) >> SHIFT;
  temp_ry2=(ry2*COS(xangle) - rz2*SIN(xangle)) >> SHIFT;
  temp_rz1=(ry1*SIN(xangle) + rz1*COS(xangle)) >> SHIFT;
  temp_rz2=(ry2*SIN(xangle) + rz2*COS(xangle)) >> SHIFT;
  ry1=temp_ry1;
  ry2=temp_ry2;
  rz1=temp_rz1;
  rz2=temp_rz2;

	 // Rotate around viewer's Z axis:

  temp_rx1=(rx1*COS(zangle) - ry1*SIN(zangle)) >> SHIFT;
  temp_ry1=(rx1*SIN(zangle) + ry1*COS(zangle)) >> SHIFT;
  temp_rx2=(rx2*COS(zangle) - ry2*SIN(zangle)) >> SHIFT;
  temp_ry2=(rx2*SIN(zangle) + ry2*COS(zangle)) >> SHIFT;
  rx1=temp_rx1;
  ry1=temp_ry1;
  rx2=temp_rx2;
  ry2=temp_ry2;

  // Adjust for perspective

  float z=(float)rz1;
  if (z<10.0) z=10.0;

  // Divide world x,y coordinates by z coordinates
  //  to obtain perspective:

  rx1=(float)distance*((float)rx1/z)+xorigin;
  ry1=(float)distance*((float)ry1/z)+yorigin;
  rx2=(float)distance*((float)rx2/z)+xorigin;
  ry2=(float)distance*((float)ry2/z)+yorigin;

  // Create sky and ground polygons,
  //  then clip to screen window

  // Obtain delta x and delta y:

  int dx = (int)(rx2 - rx1);
  int dy = (int)(ry2 - ry1);

  // Cheat to avoid divide error:

  if (!dx) dx++;

  // Obtain slope of line:

  float slope = (float)dy/(float)dx;

  // Calculate line of horizon:

  vert[0].ax=xmin;
  vert[0].ay=slope*(xmin-rx1)+ry1;
  vert[1].ax=xmax;
  vert[1].ay=slope*(xmax-rx1)+ry1;

  // Create ground polygon:

  if (flip&1) hpoly.SetColor(sky);

  // If flipped, it's the sky polygon:

  else hpoly.SetColor(ground);

  // Set vertex coordinates:

  vert[2].ax=32767;
  vert[2].ay=32767;
  vert[3].ax=-32768;
  vert[3].ay=32767;

  // Clip ground polygon:

  ZClip(&hpoly,&hclip);
  XYClip(&hclip);

  // Draw ground polygon:

  if (hclip.GetVertexCount())
	hclip.DrawPoly(screen);

  // Create sky polygon:
  if (flip&1) hpoly.SetColor(ground);

  // If flipped it's the ground polygon:

  else hpoly.SetColor(sky);

  // Set vertex coordinates:

  vert[2].ax=32767;
  vert[2].ay=-32768;
  vert[3].ax=-32768;
  vert[3].ay=-32768;

  // Clip sky polygon:

  ZClip(&hpoly,&hclip);
  XYClip(&hclip);

  // Draw sky polygon:

  if (hclip.GetVertexCount())
	hclip.DrawPoly(screen);
}


