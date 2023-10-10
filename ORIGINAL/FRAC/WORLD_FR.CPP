
// WORLD_FR.CPP
//   A set of functions to perform lighting calculations for polygons.
// copyright (c) 1995 Waite Group Press
// 7/19/95 Dale Sather        lighting functions created
// 9/20/95 mracky@sirius.com  update to C++; created World class

#include  <stdio.h>
#include  <math.h>
#include  <string.h>
#include  "poly_fr.h"
#include  "world_fr.h"
#include  "screen.h"
#include  "polylsfr.h"

#define DEPTHSORT
// ***********************************************
// WORLD
World::~World()
{
	if(obj)
		delete [] obj;
}
void World::SetAmbient(int amb)
{
	if( amb < 1 )
		amb = 1;
	if( amb > 99)
		amb = 99;
	source.ambient = amb;
	AdjustLighting();
}

void World::SetLighting(Object *objptr)
{
	objptr->MeasureNormals(palette.GetUnshadedCount());

}
void World::AdjustLighting()
{
  palette.Install(source.ambient); // Set up the palette

	// Calc lengths of polygon normals
  for (int o=0; o< number_of_objects; o++)
  	if( obj[o].isVisible())
		SetLighting(&obj[o]);
}

void World::CalcPolygonColor(Polygon *polyptr)
{
  int ambient=source.ambient;
  int temp = polyptr->GetOriginalColor();

  if (temp < palette.GetUnshadedCount())
		polyptr->SetColor( temp);
  else {
		vertex_type *v0=polyptr->GetVertexPtr(0);
		vertex_type *v1=polyptr->GetVertexPtr(1);
		vertex_type *v2=polyptr->GetVertexPtr(2);
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

void World::Draw(Object *objptr, unsigned char far *screen,int xorigin, int yorigin)
{
  if(objptr->isVisible()) {
	 objptr->Transform();       // Transform OBJECT using MATRIX
#ifdef DEPTHSORT
	 inittrans();
	 objptr->Atransform();

	 	// temporary polygon list
	 PolygonList polygon_list;

	 polygon_list.Create(objptr->GetPolygonCount());

	 polygon_list.MakePolygonList(objptr);

     // Perform depth sort on the polygon list:

	 objptr->Project(400, xorigin, yorigin); // Perform perspective projection

	 polygon_list.z_sort();
	 for (int i=0; i<polygon_list.GetPolygonCount(); i++) {
	 	Polygon *pptr = polygon_list.GetPolygonPtr(i);
		if (objptr->isConvex()) {
			if(!pptr->backface()) {
		 		CalcPolygonColor(pptr);
				pptr->DrawPoly(screen);
			}
		}
		else
			pptr->DrawPoly(screen);
	 }
#else
	 CalcObjectColor(objptr);
	 objptr->Project(400, xorigin, yorigin); // Perform perspective projection
	 objptr->Draw(screen);  // Draw transformed object
#endif
  }
}

void World::CalcObjectColor(Object *object)
{
  // Calculate the palette index for each polygon in the OBJECT:
  for (int p=0; p< object->GetPolygonCount(); p++) {
	  CalcPolygonColor(object->GetPolygonPtr(p));
	}
}



