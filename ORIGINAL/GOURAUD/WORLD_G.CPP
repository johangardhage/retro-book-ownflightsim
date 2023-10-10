
// WORLD_G.CPP
//   A set of functions to perform lighting calculations for polygons.
// copyright (c) 1995 Waite Group Press
// 
// 7/19/95 Dale Sather        color algorithms added
// 9/20/95 mracky@sirius.com  update to C++; World class created 

#include  <stdio.h>
#include  <math.h>
#include  <string.h>
#include  "world_g.h"
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
  palette.Install(source.ambient); // Set up the palette

  	// Calc lengths of polygon normals
  int unshaded = palette.GetUnshadedCount();
  // Calculate normal vectors for each vertex in WORLD:
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
		int brightness=(int)(((nx*source.x+ny*source.y+nz*source.z)*(100-ambient))/
			(source.length*polyptr->GetNormalLen()));
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
	 // Draw transformed object used GOURAUD shading
	 optr->Draw(screen,&palette,source.ambient);
	 }
}
void World::Draw(unsigned char far *screen,int xorigin, int yorigin)
{
	for(int i=0; i < number_of_objects; i++) {
		Draw(i, screen,xorigin,yorigin);
	 }
}

// GOURAUD version of CALCOBJECTCOLOR
void World::CalcObjectColor(Object *object)
{
		// dereference the source fields to local variables
	int ambient= source.ambient;
	long xx=source.x;
	long yy=source.y;
	long zz=source.z;
	long length=source.length;

	  // Calculate brightness for each vertex in the OBJECT:
	  // the GOURAUD example uses direct vertice adjustment
	  // instead of using the polygon plane
	vertex_type *vptr= object->GetVertexPtr(0);
	for (int v=0; v< object->GetVertexCount(); v++,vptr++) {
		int brightness =
		  (int)(((vptr->nwx*xx+vptr->nwy*yy+vptr->nwz*zz)*
				(100-ambient))/(length*vptr->nlength));
		if (brightness<0)
			brightness=ambient;  // Ambient applies to surfaces not facing the source
		else
			brightness+=ambient; // Add ambient for polygons facing the source
		vptr->brightness = brightness;
	 }
}



void World::CalcColors()
{
  // Calculate the palette index for each object in the WORLD:

  for (int o=0; o< number_of_objects; o++) {
	CalcObjectColor(&obj[o]);
  }
}


#endif
