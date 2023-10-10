
// WORLD.CPP
//   A set of functions to perform lighting calculations for polygons.
// copyright (c) 1995 Waite Group Press
// 7/19/95 Dale Sather        lighting functions created
// 9/20/95 mracky@sirius.com  update to C++; created World class

#include  <stdio.h>
#include  <math.h>
#include  <string.h>
#include  "poly2.h"
#include  "world.h"
#include  "screen.h"

// ***********************************************
// WORLD
World::~World()
{
	if(obj)
		delete [] obj;
}
void World::AdjustLighting()
{

	SetWorldPalette();
	// Calc lengths of polygon normals
  int unshaded = palette.GetUnshadedCount();
  for (int o=0; o< number_of_objects; o++)
	obj[o].MeasureNormals(unshaded);
}


void World::CalcPolygonColor(Polygon *polyptr)
{
  int ambient=source.ambient;
  int temp = polyptr->GetOriginalColor();

  if (temp < palette.GetUnshadedCount())
		polyptr->SetColor( temp);
  else {
		vertex_type *v0=polyptr->GetVerticePtr(0);
		vertex_type *v1=polyptr->GetVerticePtr(1);
		vertex_type *v2=polyptr->GetVerticePtr(2);
        long ax=v0->wx-v1->wx;  // Vector for edge from vertex 1 to vertex 0
        long ay=v0->wy-v1->wy;
        long az=v0->wz-v1->wz;
        long bx=v2->wx-v1->wx;  // Vector for edge from vertex 1 to vertex 2
        long by=v2->wy-v1->wy;
        long bz=v2->wz-v1->wz;
        long nx=ay*bz-az*by;   // Normal is cross product of the two vectors
        long ny=az*bx-ax*bz;
        long nz=ax*by-ay*bx;
		int brightness=(int)(((nx*source.x+ny*source.y+nz*source.z)*(100-ambient))/(source.length*polyptr->GetNormalLen()));
        if (brightness<0)
          brightness=ambient;  // Ambient applies to polygons not facing the source
        else
          brightness+=ambient; // Add ambient for polygons facing the source
		polyptr->SetColor(palette.ColorIndex(temp,brightness,ambient));
  }
}

#ifdef LIGHTING_DEMO
void World::Draw(int objnum, unsigned char far *screen,int xorigin, int yorigin)
{
	if( objnum < number_of_objects) {
	 Object *optr = &obj[objnum];
	 optr->Transform();       // Transform OBJECT using MATRIX
	 CalcObjectColor(optr);
	 optr->Project(400, xorigin, yorigin); // Perform perspective projection
	 optr->Draw(screen);  // Draw transformed object
	 }
}
void World::Draw(unsigned char far *screen,int xorigin, int yorigin)
{
	for(int i=0; i < number_of_objects; i++) {
		Draw(i, screen,xorigin,yorigin);
	 }
}

void World::CalcObjectColor(Object *object)
{
  for (int p=0; p< object->GetPolygonCount(); p++) {
	  CalcPolygonPtr(object->GetPolygonPtr(p));
	}

}
void World::CalcColors()
{
  // Calculate the palette index for each polygon in WORLD:

  for (int o=0; o< number_of_objects; o++) {
	CalcObjectColor(&obj[o]);
  }
}

#endif
