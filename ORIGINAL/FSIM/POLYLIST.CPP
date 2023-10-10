// POLYLIST.CPP
// created 9/8/95 mickRacky for Waite Group Press Flights of Fantasy , 2nd Edition
#include <stdlib.h>
#include "polylist.h"


// ***********************************************
// POLYGONLIST
void PolygonList::MakePolygonList(const World& world)

// Create a list of all polygons potentially visible in
//  the viewport, removing backfaces and polygons outside
//  of the viewing pyramid in the process

{
	int count=0;  // Determine number of polygons in list

  	// Loop through all objects in world:

	for (int objnum=0; objnum< world.GetObjectCount();
		  objnum++) {

	 	// Create pointer to current object:
		Object *objptr=world.GetObjectPtr(objnum);

	 	// Loop through all polygons in current object:
		for (int polynum=0; polynum<objptr->GetPolygonCount();
          polynum++) {

			// Create pointer to current polygon:

			Polygon *polyptr= objptr->GetPolygonPtr(polynum);

			// If polygon isn't a backface, consider it for list:

			if (!polyptr->Backface()) {

		  // Find maximum & minimum coordinates for polygon:

				int pxmax=-32768; // Initialize all mins & maxes
				int pxmin=32767;   //  to highest and lowest
				int pymax=-32768;  //  possible values
				int pymin=32767;
				int pzmax=-32768;
				int pzmin=32767;

		  // Loop through all vertices in polygon, to find
		  //  ones with higher and lower coordinates than
		  //  current min & max:

				for (int v=0; v<polyptr->GetVertexCount(); v++) {
					vertex_type *vptr = polyptr->GetVerticePtr(v);
					if (vptr->ax > pxmax) {
						pxmax=(int)vptr->ax;
					}
					if (vptr->ax < pxmin) {
						pxmin=(int)vptr->ax;
					}
					if (vptr->ay > pymax) {
						pymax=(int)vptr->ay;
					}
					if (vptr->ay < pymin) {
						pymin=(int)vptr->ay;
					}
					if (vptr->az > pzmax) {
						pzmax=(int)vptr->az;
					}
					if (vptr->az < pzmin) {
						pzmin=(int)vptr->az;
					}
				}

#ifdef HID_SURF_TEST5
		  // If we plan on using the 5 additional tests for
		  // hidden surface removal, then
		  // put mins & maxes in polygon descriptor:
				polyptr->SetMin(pxmin, pymin, pzmin);
				polyptr->SetMax(pxmax, pymax, pzmax);
#endif
		  // Calculate center of polygon z extent:

		  float xcen=(pxmin+pxmax)*0.5;
		  float ycen=(pymin+pymax)*0.5;
		  float zcen=(pzmin+pzmax)*0.5;
		  polyptr->SetDistance(xcen*xcen+ycen*ycen+zcen*zcen);

		  // If polygon is in front of the view plane,
		//  add it to the polygon list:

				if (pzmax > 1) {
					polylist[count++]= polyptr;
				}
			}
		}
	}

  // Put number of polygons in polylist structure:

	number_of_polygons=count;
}

#ifdef OLD_SORT
void PolygonList::z_sort()
{
	int swapflag;
	do{
		swapflag=0;
		// Loop thorugh polygon list:
		for (int i=0; i<(number_of_polygons-1); i++) {
			// Are polygons out of order?
			if (polylist[i]->GetDistance()
				< polylist[i+1]->GetDistance()) {
				// if so , swap them...
				//  we're swapping pointers and not the
				// actual objects!
				Polygon *temp = polylist[i];
				polylist[i] = polylist[i+1];
				polylist[i+1] = temp;
				swapflag=-1;   // indicate that swap was performed
			}
		}
	}while(swapflag);
}
#else
/*
if elem 1 < elem2 return -
if elem1 == elem2 return 0
if elem1 > elem2 return +
*/
static int _PolyCMP(const void *elem1, const void * elem2)
{
	Polygon *p1 = *((Polygon **)elem1);
	Polygon *p2 = *((Polygon **)elem2);
 	double result = p1->GetDistance() - p2->GetDistance() ;
 	if( result > 0.0 )
		return 1;
	else if( result < 0.0)
		return -1;
	return 0;
}

void PolygonList::z_sort()
{
	qsort(polylist,number_of_polygons,sizeof(Polygon *), _PolyCMP);
}
#endif
void PolygonList::Create(unsigned int numPoly)
{
	polylist = new Polygon *[numPoly];
}

PolygonList::~PolygonList()
{
	if(polylist)
		delete [] polylist;
}

#ifdef HID_SURF_TEST5
int Polygon::z_overlap(const Polygon& poly2)

// Check for overlap in the z extent between this and  POLY2.

{
  // If the minimum z of POLY1 is greater than or equal to
  //  the maximum z of POLY2 or the minimum z or POLY2 is
  //  equal to or greater than the maximum z or POLY1 then
  //  return zero, indicating no overlap in the z extent:

	if ((zmin>=poly2.zmax) || (poly2.zmin>=zmax))
			return 0;

  // Return non-zero, indicating overlap in the z extent:

	return -1;
}

int Polygon::should_be_swapped( const Polygon& poly2)

// Check to see if POLY1 and POLY2 are in the wrong order
//  for the Painter's Algorithm.

{

  // Check for overlap in the x and/or y extents:

	if (! xy_overlap(poly2)) return 0;

  // Check to see if poly1 is on the correct side of poly2:

	if ( surface_outside(poly2)) return 0;

  // Check to see if poly2 is on the correct side of poly1:

	if ( surface_inside(poly2)) return 0;

  // If we've made it this far, all tests have been
  //  flunked, so return non-zero.

	return -1;
}
int Polygon::xy_overlap(const Polygon& poly2)

// Check for overlap in the x and y extents, return
//  non-zero if both are found, otherwise return zero.

{
  // If no overlap in the x extent, return zero:

	if ((xmin>poly2.xmax) || (poly2.xmin>xmax))
			return 0;

  // If no overlap in the y extent, return zero:

	if ((ymin>poly2.ymax) || (poly2.ymin>ymax))
			return 0;

  // If we've gotten this far, overlap must exist in both
  //  x and y, so return non-zero:

	return -1;
}


int Polygon::surface_outside(const Polygon& poly2)

// Check to see if poly2 is inside the surface of poly1.

{

  // Determine the coefficients of poly2:

	float x1=poly2.vertex[0]->ax;
	float y1=poly2.vertex[0]->ay;
	float z1=poly2.vertex[0]->az;
	float x2=poly2.vertex[1]->ax;
	float y2=poly2.vertex[1]->ay;
	float z2=poly2.vertex[1]->az;
	float x3=poly2.vertex[2]->ax;
	float y3=poly2.vertex[2]->ay;
	float z3=poly2.vertex[2]->az;
	float a=y1*(z2-z3)+y2*(z3-z1)+y3*(z1-z2);
	float b=z1*(x2-x3)+z2*(x3-x1)+z3*(x1-x2);
	float c=x1*(y2-y3)+x2*(y3-y1)+x3*(y1-y2);
	float d=-x1*(y2*z3-y3*z2)-x2*(y3*z1-y1*z3)-x3*(y1*z2-y2*z1);

  // Plug the vertices of this poly into the plane equation
  //  of poly2, one by one:

	int flunked=0;
	for (int v=0; v<number_of_vertices; v++) {
		if(( a*(vertex[v]->ax)+b*(vertex[v]->ay)
					  +c*(vertex[v]->az)+d)<0) {
			flunked=-1; // If less than 0, we flunked
			break;
		}
	   //	int zz = 1; //meaningless statement for breakpoint
	}

  // Return 0 if flunked, -1 if not flunked.

	return !flunked;
}

int Polygon::surface_inside(const Polygon& poly2)
{
  // Determine the coefficients of poly1:

  float surface;

	float x1=vertex[0]->ax;
	float y1=vertex[0]->ay;
	float z1=vertex[0]->az;
	float x2=vertex[1]->ax;
	float y2=vertex[1]->ay;
	float z2=vertex[1]->az;
	float x3=vertex[2]->ax;
	float y3=vertex[2]->ay;
	float z3=vertex[2]->az;
	float a=y1*(z2-z3)+y2*(z3-z1)+y3*(z1-z2);
	float b=z1*(x2-x3)+z2*(x3-x1)+z3*(x1-x2);
	float c=x1*(y2-y3)+x2*(y3-y1)+x3*(y1-y2);
	float d=-x1*(y2*z3-y3*z2)-x2*(y3*z1-y1*z3)-x3*(y1*z2-y2*z1);

  // Plug the vertices of poly2 into the plane equation
  //  of poly1, one by one:

	int flunked=0;
	for (int v=0; v<poly2.number_of_vertices; v++) {
		if((surface=a*(poly2.vertex[v]->ax)+b*(poly2.vertex[v]->ay)
					  +c*(poly2.vertex[v]->az)+d) > 0) {
			flunked=-1;  // If greater than 0, we flunked
			break;
		}
	   //	int zz = 1; //meaningless statement for breakpoint
	}

  // Return 0 if flunked, -1 if not flunked.

	return !flunked;
}



#endif
