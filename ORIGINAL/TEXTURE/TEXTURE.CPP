
// TEXTURE.CPP
//   A set of functions to perform texture mapping calculations for polygons.
//
// Copyright 1995 by Dale Sather and The Waite Group Press
//
// 9/20/95 mracky@sirius.com  updated to C++: Texture class created 

#include  <stdio.h>
#include  <math.h>
#include  <string.h>
#include  <dos.h>
#include  "poly_tx.h"
#include  "texture.h"
#include  "screen.h"

unsigned char Texture::MapPixel(int x,int y,int color,int distance)
{
  x-=XORIGIN;
  y-=YORIGIN;
  float ao=ox+x*oz/distance;
  float bo=oy+y*oz/distance;
  float ai=ix+x*iz/distance;
  float bi=iy+y*iz/distance;
  float aj=jx+x*jz/distance;
  float bj=jy+y*jz/distance;
  int u=(int)((bj*ao-aj*bo-bj*x+aj*y)/(bi*aj-ai*bj));
  int v;
  if (fabs(aj)>fabs(bj))
    v=(int)((x-ao-u*ai)/aj);
  else
    v=(int)((y-bo-u*bi)/bj);
  return MapPoint(u,v,color);
}


#pragma argsused
unsigned char Texture::MapPoint(int x,int y,int color)
{
  // color parameter is not used in this implimentation
  // offset the texture map from the center of the bitmap:
  int width = header.xmax - header.xmin +1;
  x=x+width/2;
  y=y+(header.ymax- header.ymin+1)/2;
  return image[x+y*width];
}


void Texture::CalcMapVectors(Polygon *polygon,int resolution)
{
  // Origin is average of all vertices
  ox=0;
  oy=0;
  oz=0;
	// retrieve the number of vertices from the polygon:
  int vcount = polygon->GetVertexCount();
  for (int i=0; i< vcount; i++) {
	vertex_type *vptr = polygon->GetVerticePtr(i);
	ox+=vptr->wx;
	oy+=vptr->wy;
	oz+=vptr->wz;
  }
  ox/=vcount;
  oy/=vcount;
  oz/=vcount;

  // Generate raw i and j vectors
  vertex_type *vp0 = polygon->GetVerticePtr(0);
  vertex_type *vp1 = polygon->GetVerticePtr(1);
  vertex_type *vp2 = polygon->GetVerticePtr(2);
  ix=vp0->wx-vp1->wx;
  iy=vp0->wy-vp1->wy;
  iz=vp0->wz-vp1->wz;
  jx=vp2->wx-vp1->wx;
  jy=vp2->wy-vp1->wy;
  jz=vp2->wz-vp1->wz;

  // Make j perpendicular to i used projection formula
  float scale=(ix*jx+iy*jy+iz*jz)/
			  (ix*ix+iy*iy+iz*iz);
  jx=jx-scale*ix;
  jy=jy-scale*iy;
  jz=jz-scale*iz;

  // Scale i and j
  scale=(float)resolution/10*sqrt(ix*ix+iy*iy+iz*iz);

  ix/=scale;
  iy/=scale;
  iz/=scale;
  scale=(float)resolution/10*sqrt(jx*jx+jy*jy+jz*jz);
  jx/=scale;
  jy/=scale;
  jz/=scale;
}
