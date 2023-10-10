
// VIEW.CPP - 01/20/93
//
// A set of functions to display a database of three-dimensional
// polygon-fill objects from a selected position and angle. Draws
// an horizon if requested.
//
// Copyright 1993 by Christopher Lampton and The Waite Group Press

#include  <stdio.h>
#include  <alloc.h>
#include  <math.h>
#include	"fix.h"
#include  "poly.h"
#include  "view.h"
#include  "drawpoly.h"
#include	"screen.h"

#define   ONE    (1L<<SHIFT)

int screen_width,screen_height;
polygon_list_type polylist;

void View::setview(int xo,int yo,int xmn,int ymn,
                     int xmx,int ymx,int dist,
										 int grnd,int sk,unsigned char *screen_buf)

// Set size and screen coordinates of window,
//  plus screen origin and viewer distance from screen

{
	xorigin=xo;	       // X coordinate of screen origin
	yorigin=yo;	       // Y coordinate of screen origin
	xmin=xmn;		       // X coordinate of upper left
                           //  corner of window
	xmax=xmx;		       // X coordinate of lower right
                           //  corner of window
	ymin=ymn;		       // Y coordinate of upper left
                           //  corner of window
	ymax=ymx;		       // Y coordinate of lower right
                           //  corner of window
	distance=dist;	       // Distance of viewer from display
	ground=grnd;	       // Ground color
	sky=sk;		       // Sky color
	screen_buffer=screen_buf; // Buffer address for screen
	screen_width=(xmax-xmin)/2;
	screen_height=(ymax-ymin)/2;
}

void View::setworld(world_type wrld,int polycount)
{
  // Registers current world database for use by the
  // view system

  world=wrld;
  polylist.polygon = new polygon_type[polycount];
}

void View::display(view_type curview,int horizon_flag)
{
  // Clear the viewport:

	clrwin(xmin,ymin,xmax-xmin+1,ymax-ymin+1,screen_buffer);

  // If horizon desired, draw it:

	if (horizon_flag) draw_horizon(curview.xangle,
       curview.yangle,curview.zangle,screen_buffer);

  // Update all object vertices to current positions:

	for (int i=0; i<world.number_of_objects; i++) {
	  update(&world.obj[i]);
	}

  // Set aligned coordinates to current view position:

	alignview(&world,curview);

  // Set up the polygon list:

	make_polygon_list(world,&polylist);

  // Perform depth sort on the polygon list:

	z_sort(&polylist);

  // Draw the polygon list:

	draw_polygon_list(&polylist,screen_buffer);
}

void View::cproject(clipped_polygon_type *clip)
{

// Project clipped polygon onto screen using perspective projection

	int z;

	clip_type *vptr=clip->vertex;
	for (int v=0; v<clip->number_of_vertices; v++) {
    // Loop through vertices
		z=ABS(vptr[v].z);
		vptr[v].x=(float)distance*((float)vptr[v].x/(float)z)
      +xorigin;  // ...divide world x&y coords
		vptr[v].y=(float)distance*((float)vptr[v].y/(float)z)
      +yorigin;  // ...by z coordinates
  }
}

void View::zclip(polygon_type *polygon,
              clipped_polygon_type *clip)
{

// Clip polygon against edges of window with coordinates
//  xmin,ymin,xmax,ymax

  // Create pointer to vertices of clipped polygon
  //  structure:

	clip_type *pcv=clip->vertex;

  // Transfer information from polygon structure to
  //  clipped polygon structure:

	clip->color=polygon->color;
	clip->xmin=polygon->xmin;
	clip->xmax=polygon->xmax;
	clip->xmin=polygon->zmin;
	clip->xmax=polygon->zmax;
	clip->xmin=polygon->ymin;
	clip->xmax=polygon->ymax;

	// Clip against front of window view volume:

	int cp = 0; // Point to current vertex of clipped polygon
	int zmin = 2;  // Set minimum z coordinate

  // Initialize pointer to last vertex:

	int v1=polygon->number_of_vertices-1;

	// Loop through all edges of polygon

	for (int v2=0; v2<polygon->number_of_vertices; v2++) {
		vertex_type *pv1=polygon->vertex[v1];
		vertex_type *pv2=polygon->vertex[v2];

		// Categorize edges by type:

		if ((pv1->az >= zmin) && (pv2->az >= zmin)) {

      // Entirely inside front

			pcv[cp].x   = pv2->ax;
			pcv[cp].y   = pv2->ay;
			pcv[cp++].z = pv2->az;
		}
		if ((pv1->az < zmin) && (pv2->az < zmin)){

      // Edge is entirely past front, so do nothing

		}
		if ((pv1->az >= zmin) && (pv2->az < zmin)) {

      // Edge is leaving view volume

			float t=(float)(zmin - pv1->az) /
          (float)(pv2->az - pv1->az);
			pcv[cp].x = pv1->ax + (pv2->ax - pv1->ax) * t;
			pcv[cp].y = pv1->ay + (pv2->ay - pv1->ay) * t;
			pcv[cp++].z = zmin;
		}
		if ((pv1->az < zmin) && (pv2->az >= zmin)) {

      // Line is entering view volume

			float t=(float)(zmin - pv1->az) /
          (float)(pv2->az - pv1->az);
			pcv[cp].x = pv1->ax + (pv2->ax - pv1->ax) * t;
			pcv[cp].y = pv1->ay + (pv2->ay - pv1->ay) * t;
			pcv[cp++].z=zmin;
			pcv[cp].x = pv2->ax;
			pcv[cp].y = pv2->ay;
			pcv[cp++].z = pv2->az;
		}
		v1=v2; // Advance to next vertex
	}

  // Put number of vertices in clipped polygon structure:

	clip->number_of_vertices = cp;
}

void View::xyclip(clipped_polygon_type *clip)
{
	// Clip against sides of viewport

	int temp; // Miscellaneous temporary storage
	clip_type *pcv=clip->vertex;

	// Clip against left edge of viewport:

	int cp = 0;

  // Initialize pointer to last vertex:

	int v1=clip->number_of_vertices-1;
	for (int v2=0; v2<clip->number_of_vertices; v2++) {

		// Categorize edges by type:

		if ((pcv[v1].x >= xmin) && (pcv[v2].x >= xmin)) {

      // Edge isn't off left side of viewport

			pcv[cp].x1   = pcv[v2].x;
			pcv[cp++].y1 = pcv[v2].y;
		}
		else if ((pcv[v1].x < xmin) && (pcv[v2].x < xmin)){

      // Edge is entirely off left side of viewport,
      //  so don't do anything

		}
		if ((pcv[v1].x >= xmin) && (pcv[v2].x < xmin)) {

      // Edge is leaving viewport

			float m=(float)(pcv[v2].y-pcv[v1].y) /
          (float)(pcv[v2].x-pcv[v1].x);
			pcv[cp].x1 = xmin;
			pcv[cp++].y1 =
				pcv[v1].y + m * (xmin - pcv[v1].x);
		}
		if ((pcv[v1].x < xmin) && (pcv[v2].x >= xmin)) {

      // Edge is entering viewport

			float m=(float)(pcv[v2].y-pcv[v1].y) /
          (float)(pcv[v2].x-pcv[v1].x);
			pcv[cp].x1 = xmin;
			pcv[cp++].y1 =
				pcv[v1].y + m * (xmin - pcv[v1].x);
			pcv[cp].x1 = pcv[v2].x;
			pcv[cp++].y1 = pcv[v2].y;
		}

		v1=v2;
	}
	clip->number_of_vertices = cp;

	// Clip against right edge of viewport:

	cp = 0;

  // Initialize pointer to last vertex:

	v1=clip->number_of_vertices-1;
	for (v2=0; v2<clip->number_of_vertices; v2++) {

		// Categorize edges by type:

		if ((pcv[v1].x1 <= xmax) && (pcv[v2].x1 <= xmax)) {

      // Edge isn't off right side of viewport

			pcv[cp].x = pcv[v2].x1;
			pcv[cp++].y = pcv[v2].y1;
		}
		if ((pcv[v1].x1 > xmax) && (pcv[v2].x1 > xmax)){

      // Edge is entirely off right side of viewport,
      //  so do nothing

		}
		if ((pcv[v1].x1 <= xmax) && (pcv[v2].x1 > xmax)) {

      // Edge if leaving viewport

			float m=(float)(pcv[v2].y1-pcv[v1].y1) /
          (float)(pcv[v2].x1-pcv[v1].x1);
			pcv[cp].x = xmax;
			pcv[cp++].y =
				pcv[v1].y1 + m * (xmax - pcv[v1].x1);
		}
		if ((pcv[v1].x1 > xmax) && (pcv[v2].x1 <= xmax)) {

      // Edge is entering viewport

			float m=(float)(pcv[v2].y1-pcv[v1].y1) /
          (float)(pcv[v2].x1-pcv[v1].x1);
			pcv[cp].x = xmax;
			pcv[cp++].y =
				pcv[v1].y1 + m * (xmax - pcv[v1].x1);
			pcv[cp].x = pcv[v2].x1;
			pcv[cp++].y = pcv[v2].y1;
		}
		v1=v2;
	}
	clip->number_of_vertices = cp;

	// Clip against upper edge of viewport:

	cp = 0;

  // Initialize pointer to last vertex:

	v1=clip->number_of_vertices-1;
	for (v2=0; v2<clip->number_of_vertices; v2++) {

		// Categorize edges by type:

		if ((pcv[v1].y >= ymin) && (pcv[v2].y >= ymin)) {

      // Edge is not off top off viewport

			pcv[cp].x1 = pcv[v2].x;
			pcv[cp++].y1 = pcv[v2].y;
		}
		else if ((pcv[v1].y < ymin) && (pcv[v2].y < ymin)){

      // Edge is entirely off top of viewport,
      //  so don't do anything

		}
		if ((pcv[v1].y >= ymin) && (pcv[v2].y < ymin)) {

      // Edge is leaving viewport

			if ((temp=pcv[v2].x-pcv[v1].x)!=0) {
				float m=(float)(pcv[v2].y-pcv[v1].y)/(float)temp;
				pcv[cp].x1 =
					pcv[v1].x + (ymin - pcv[v1].y) / m;
			}
			else pcv[cp].x1 = pcv[v1].x;
			pcv[cp++].y1 = ymin;
		}
		if ((pcv[v1].y < ymin) && (pcv[v2].y >= ymin)) {

      // Edge is entering viewport

			if ((temp=pcv[v2].x-pcv[v1].x)!=0) {
				float m=(float)(pcv[v2].y-pcv[v1].y)/(float)temp;
				pcv[cp].x1 =
					pcv[v1].x + (ymin - pcv[v1].y) / m;
			}
			else pcv[cp].x1 = pcv[v1].x;
			pcv[cp++].y1 = ymin;
			pcv[cp].x1 = pcv[v2].x;
			pcv[cp++].y1 = pcv[v2].y;
		}
		v1=v2;
	}
	clip->number_of_vertices = cp;

	// Clip against lower edge of viewport:

	cp = 0;

  // Initialize pointer to last vertex:

	v1=clip->number_of_vertices-1;
	for (v2=0; v2<clip->number_of_vertices; v2++) {

		// Categorize edges by type:

		if ((pcv[v1].y1 <= ymax) && (pcv[v2].y1 <= ymax)) {

      // Edge is not off bottom of viewport

			pcv[cp].x = pcv[v2].x1;
			pcv[cp++].y = pcv[v2].y1;
		}
		if ((pcv[v1].y1 > ymax) && (pcv[v2].y1 > ymax)){

      // Edge is entirely off bottom of viewport,
      //  so don't do anything

		}
		if ((pcv[v1].y1 <= ymax) && (pcv[v2].y1 > ymax)) {

      // Edge is leaving viewport

			if ((temp=pcv[v2].x1-pcv[v1].x1)!=0) {
        float m=(float)(pcv[v2].y1-pcv[v1].y1)/(float)temp;
				pcv[cp].x =
					pcv[v1].x1 + (ymax - pcv[v1].y1) / m;
			}
			else pcv[cp].x = pcv[v1].x1;
			pcv[cp++].y = ymax;
		}
		if ((pcv[v1].y1 > ymax) && (pcv[v2].y1 <= ymax)) {

      // Edge is entering viewport

			if ((temp=pcv[v2].x1-pcv[v1].x1)!=0) {
				float m=(float)(pcv[v2].y1-pcv[v1].y1)/(float)temp;
				pcv[cp].x =
					pcv[v1].x1 + (ymax - pcv[v1].y1) / m;
			}
			else pcv[cp].x = pcv[v1].x1;
			pcv[cp++].y = ymax;
			pcv[cp].x = pcv[v2].x1;
			pcv[cp++].y = pcv[v2].y1;
		}
		v1=v2;
	}
	clip->number_of_vertices = cp;
}

void View::make_polygon_list(world_type world,
             polygon_list_type *polylist)

// Create a list of all polygons potentially visible in
//  the viewport, removing backfaces and polygons outside
//  of the viewing pyramid in the process

{
	int count=0;  // Determine number of polygons in list

  // Loop through all objects in world:

	for (int objnum=0; objnum<world.number_of_objects;
        objnum++) {

    // Create pointer to current object:

		object_type *objptr=&world.obj[objnum];

    // Loop through all polygons in current object:

		for (int polynum=0; polynum<objptr->number_of_polygons;
          polynum++) {

      // Create pointer to current polygon:

			polygon_type *polyptr=&objptr->polygon[polynum];

      // If polygon isn't a backface, consider it for list:

			if (!backface(*polyptr)) {

        // Find maximum & minimum coordinates for polygon:

				int pxmax=-32767;  // Initialize all mins & maxes
				int pxmin=32767;   //  to highest and lowest
				int pymax=-32767;  //  possible values
				int pymin=32767;
				int pzmax=-32767;
				int pzmin=32767;

        // Loop through all vertices in polygon, to find
        //  ones with higher and lower coordinates than
        //  current min & max:

				for (int v=0; v<polyptr->number_of_vertices; v++) {
					if (polyptr->vertex[v]->ax > pxmax) {
						pxmax=polyptr->vertex[v]->ax;
					}
					if (polyptr->vertex[v]->ax < pxmin) {
						pxmin=polyptr->vertex[v]->ax;
					}
					if (polyptr->vertex[v]->ay > pymax) {
						pymax=polyptr->vertex[v]->ay;
					}
					if (polyptr->vertex[v]->ay < pymin) {
						pymin=polyptr->vertex[v]->ay;
					}
					if (polyptr->vertex[v]->az > pzmax) {
						pzmax=polyptr->vertex[v]->az;
					}
					if (polyptr->vertex[v]->az < pzmin) {
						pzmin=polyptr->vertex[v]->az;
					}
				}

        // Put mins & maxes in polygon descriptor:

				polyptr->xmin=pxmin;
				polyptr->xmax=pxmax;
				polyptr->ymin=pymin;
				polyptr->ymax=pymax;
				polyptr->zmin=pzmin;
				polyptr->zmax=pzmax;

        // Calculate center of polygon z extent:

        float xcen=(pxmin+pxmax)/2.0;
        float ycen=(pymin+pymax)/2.0;
        float zcen=(pzmin+pzmax)/2.0;
        polyptr->distance=xcen*xcen+ycen*ycen+zcen*zcen;

        // If polygon is in front of the view plane,
        //  add it to the polygon list:

				if (pzmax > 1) {
						polylist->polygon[count++]=*polyptr;
				}
			}
		}
	}

  // Put number of polygons in polylist structure:

	polylist->number_of_polygons=count;
}

void View::z_sort(polygon_list_type *polylist)
{
	int swapflag=-1;
	while (swapflag) {
		swapflag=0;
		for (int i=0; i<(polylist->number_of_polygons-1); i++) {
    if (polylist->polygon[i].distance
         < polylist->polygon[i+1].distance) {
			polygon_type temp=polylist->polygon[i];
			polylist->polygon[i]=polylist->polygon[i+1];
			polylist->polygon[i+1]=temp;
			swapflag=-1;
			}
		}
	}
}

int View::z_overlap(polygon_type poly1,polygon_type poly2)

// Check for overlap in the z extent between POLY1 and
//  POLY2.

{
  // If the minimum z of POLY1 is greater than or equal to
  //  the maximum z of POLY2 or the minimum z or POLY2 is
  //  equal to or greater than the maximum z or POLY1 then
  //  return zero, indicating no overlap in the z extent:

	if ((poly1.zmin>=poly2.zmax) || (poly2.zmin>=poly1.zmax))
			return 0;

  // Return non-zero, indicating overlap in the z extent:

	return -1;
}

int View::should_be_swapped(polygon_type poly1,
                             polygon_type poly2)

// Check to see if POLY1 and POLY2 are in the wrong order
//  for the Painter's Algorithm.

{

  // Check for overlap in the x and/or y extents:

	if (!xy_overlap(poly1,poly2)) return 0;

  // Check to see if poly1 is on the correct side of poly2:

	if (surface_outside(poly1,poly2)) return 0;

  // Check to see if poly2 is on the correct side of poly1:

	if (surface_inside(poly1,poly2)) return 0;

  // If we've made it this far, all tests have been
  //  flunked, so return non-zero.

	return -1;
}

int View::xy_overlap(polygon_type poly1,polygon_type poly2)

// Check for overlap in the x and y extents, return
//  non-zero if both are found, otherwise return zero.

{
  // If no overlap in the x extent, return zero:

	if ((poly1.xmin>poly2.xmax) || (poly2.xmin>poly1.xmax))
			return 0;

  // If no overlap in the y extent, return zero:

	if ((poly1.ymin>poly2.ymax) || (poly2.ymin>poly1.ymax))
			return 0;

  // If we've gotten this far, overlap must exist in both
  //  x and y, so return non-zero:

	return -1;
}

int View::surface_outside(polygon_type poly1,
                 polygon_type poly2)

// Check to see if poly2 is inside the surface of poly1.

{
  long double surface;

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

  // Plug the vertices of poly1 into the plane equation
  //  of poly2, one by one:

	int flunked=0;
	for (int v=0; v<poly1.number_of_vertices; v++) {
		if((surface=a*(poly1.vertex[v]->ax)+b*(poly1.vertex[v]->ay)
                 +c*(poly1.vertex[v]->az)+d)<0) {
			flunked=-1; // If less than 0, we flunked
			break;
		}
  int zz=1;
	}

  // Return 0 if flunked, -1 if not flunked.

	return !flunked;
}

int View::surface_inside(polygon_type poly1,
             polygon_type poly2)
{
  // Determine the coefficients of poly1:

  float surface;

	float x1=poly1.vertex[0]->ax;
	float y1=poly1.vertex[0]->ay;
	float z1=poly1.vertex[0]->az;
	float x2=poly1.vertex[1]->ax;
	float y2=poly1.vertex[1]->ay;
	float z2=poly1.vertex[1]->az;
	float x3=poly1.vertex[2]->ax;
	float y3=poly1.vertex[2]->ay;
	float z3=poly1.vertex[2]->az;
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
  int zz=1;
	}

  // Return 0 if flunked, -1 if not flunked.

	return !flunked;
}

void View::draw_polygon_list(polygon_list_type *polylist,
             char far *screen)

// Draw all polygons in polygon list to screen buffer

{
  // Loop through polygon list:

  for (int i=0; i<polylist->number_of_polygons; i++) {

  // Clip against front of view volume:

  zclip(&polylist->polygon[i],&clip_array);

  // Check to make sure polygon wasn't clipped out of
  //  existence

  if (clip_array.number_of_vertices>0) {

    // Perform perspective projection:

    cproject(&clip_array);

    // Clip against sides of viewport:

    xyclip(&clip_array);

    // Check to make sure polygon wasn't clipped out of
    //  existence:

    if (clip_array.number_of_vertices>0)

    // Draw polygon:

      drawpoly(&clip_array,screen);
    }
  }
}

int	View::backface(polygon_type p)
{

// 	 Returns 0 if POLYGON is visible, -1 if not.
//   POLYGON must be part of a convex polyhedron

  vertex_type *v0,*v1,*v2;  // Pointers to three vertices
  // Point to vertices:
  v0=p.vertex[0];
  v1=p.vertex[1];
  v2=p.vertex[2];
  float x1=v0->ax;
  float x2=v1->ax;
  float x3=v2->ax;
  float y1=v0->ay;
  float y2=v1->ay;
  float y3=v2->ay;
  float z1=v0->az;
  float z2=v1->az;
  float z3=v2->az;

  // Calculate dot product:

  float c=(x3*((z1*y2)-(y1*z2)))+
          (y3*((x1*z2)-(z1*x2)))+
          (z3*((y1*x2)-(x1*y2)));
  return(c<0);
}

void View::alignview(world_type *world,view_type view)
{
  // Initialize transformation matrices:

  inittrans();

  // Set up translation matrix to shift objects relative
  // to viewer:

  translate(-view.copx,-view.copy,-view.copz);

  // Rotate all objects in universe around origin:

  rotate(-view.xangle,-view.yangle,-view.zangle);

  // Now perform the transformation on every object
  //  in the universe:

  for (int i=0; i<world->number_of_objects; i++) {
    atransform(&world->obj[i]);  // Transform object i
  }
}

void View::update(object_type *object)
{
  if (object->update) {

    // Initialize transformations:

    inittrans();

    // Create scaling matrix:

    scale(object->xscale,object->yscale,object->zscale);

    // Create rotation matrix:

    rotate(object->xangle,object->yangle,object->zangle);

    // Create translation matrix:

    translate(object->x,object->y,object->z);

    // Transform OBJECT with master transformation matrix:

    transform(object);

    // Indicate update complete:

    object->update = 0;
  }
}

void View::draw_horizon(int xangle,int yangle,int zangle,
                         unsigned char *screen)
{
  long rx1,rx2,temp_rx1,temp_rx2;
  long ry1,ry2,temp_ry1,temp_ry2;
  long rz1,rz2,temp_rz1,temp_rz2;

  vertex_type vert[4];
  polygon_type hpoly;
  clipped_polygon_type hclip;

  // Allocate memory for polygon and vertices

  hpoly.vertex=new vertex_type*[4];
  hpoly.vertex[0]=&vert[0];
  hpoly.vertex[1]=&vert[1];
  hpoly.vertex[2]=&vert[2];
  hpoly.vertex[3]=&vert[3];
  vert[0].az=distance;
  vert[1].az=distance;
  vert[2].az=distance;
  vert[3].az=distance;

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

  int z=rz1;
  if (z<10) z=10;

  // Divide world x,y coordinates by z coordinates
  //  to obtain perspective:

  rx1=(float)distance*((float)rx1/(float)z)+xorigin;
  ry1=(float)distance*((float)ry1/(float)z)+yorigin;
  rx2=(float)distance*((float)rx2/(float)z)+xorigin;
  ry2=(float)distance*((float)ry2/(float)z)+yorigin;

  // Create sky and ground polygons,
  //  then clip to screen window

  // Obtain delta x and delta y:

  int dx = rx2 - rx1; int dy = ry2 - ry1;
  int line_ready = 0;
  hpoly.number_of_vertices=4;

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

  if (flip&1) hpoly.color=sky;

  // If flipped, it's the sky polygon:

  else hpoly.color=ground;

  // Set vertex coordinates:

  vert[2].ax=32767;
  vert[2].ay=32767;
  vert[3].ax=-32767;
  vert[3].ay=32767;

  // Clip ground polygon:

  zclip(&hpoly,&hclip);
  xyclip(&hclip);

  // Draw ground polygon:

  if (hclip.number_of_vertices)
    drawpoly(&hclip,screen);

  // Create sky polygon:

  if (flip&1) hpoly.color=ground;

  // If flipped it's the ground polygon:

  else hpoly.color=sky;

  // Set vertex coordinates:

  vert[2].ax=32767;
  vert[2].ay=-32767;
  vert[3].ax=-32767;
  vert[3].ay=-32767;

  // Clip sky polygon:

  zclip(&hpoly,&hclip);
  xyclip(&hclip);

  // Draw sky polygon:

  if (hclip.number_of_vertices)
    drawpoly(&hclip,screen);

  // Release memory used for polygons:

  farfree(hpoly.vertex);
}
