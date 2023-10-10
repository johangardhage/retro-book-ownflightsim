
// VIEW.CPP - 01/20/93
//
// A set of functions to display a database of three-dimensional
// polygon-fill objects from a selected position and angle. Draws
// an horizon if requested.
//
// Copyright 1993 by Christopher Lampton and The Waite Group Press
//
// 9/17/95 mracky@sirius.com  update to C++

#include  <stdio.h>
#include  <alloc.h>
#include  <math.h>
#include  "fix.h"
#include  "poly2.h"
#include  "view.h"
#include  "screen.h"


int screen_width,screen_height;

void View::SetView(int xo,int yo,int xmn,int ymn,
                     int xmx,int ymx,int dist,
										 int grnd,int sk,unsigned char *screen_buf)

// Set size and screen coordinates of window,
//  plus screen origin and viewer distance from screen

{
	xorigin=xo;	       // X coordinate of screen origin
	yorigin=yo;	       // Y coordinate of screen origin
	xmin=xmn;		   // X coordinate of upper left
                       //  corner of window
	xmax=xmx;		   // X coordinate of lower right
                       //  corner of window
	ymin=ymn;		   // Y coordinate of upper left
                       //  corner of window
	ymax=ymx;		   // Y coordinate of lower right
                       //  corner of window
	distance=dist;	   // Distance of viewer from display
	ground=grnd;	   // Ground color
	sky=sk;		       // Sky color
	screen_buffer=screen_buf; // Buffer address for screen
	screen_width=(xmax-xmin)/2;
	screen_height=(ymax-ymin)/2;
}


int View::SetWorld(const char *worldfilename)
{
  // Registers current world database for use by the
  // view system
	int polycount= world.LoadPoly( worldfilename );
	if(polycount > 0 )
	{
		polygon_list.Create(polycount);
		// adjust the palette
		world.AdjustLighting();
		return 0;
	}
	//else
	//{
	// Error in Loading should throw error exception WITHIN world loadpoly
	//}
	return -1;
}

void View::Display(view_type curview,int horizon_flag)
{
  // Clear the viewport:

	clrwin(xmin,ymin,xmax-xmin+1,ymax-ymin+1,screen_buffer);

  // If horizon desired, draw it:

	if (horizon_flag) DrawHorizon(curview.xangle,
		 curview.yangle,curview.zangle,screen_buffer);

  // Update all object vertices to current positions:

	for (int i=0; i<world.GetObjectCount(); i++) {
	  Update(world.GetObjectPtr(i));
	}

  // Set aligned coordinates to current view position:

	AlignView(&world,curview);

  // Set up the polygon list:

	polygon_list.MakePolygonList(world);

  // Perform depth sort on the polygon list:

	polygon_list.z_sort();

  // Draw the polygon list:

	DrawPolygonList(screen_buffer);
}		    


// Draw all polygons in polygon list to screen buffer
void View::DrawPolygonList(unsigned char far *screen)
{
	// Loop through polygon list:

	for (int i=0; i<polygon_list.GetPolygonCount(); i++) {


	  world.CalcPolygonColor(polygon_list.GetPolygonPtr(i)); 
	  // Clip against front of view volume:

	  ZClip(polygon_list.GetPolygonPtr(i),&clip_array,2);

	  // Check to make sure polygon wasn't clipped out of
	  //  existence

	  if (clip_array.GetVertexCount()>0) {

			// Perform perspective projection:

			clip_array.Project(distance, xorigin, yorigin);

			// Clip against sides of viewport:

			XYClip(&clip_array);

			// Check to make sure polygon wasn't clipped out of
			//  existence:

			if (clip_array.GetVertexCount()>0)
			{
				// Draw polygon:
				clip_array.DrawPoly(screen);
			}	
		}

	}
}



void View::ZClip(Polygon *polygon, ClippedPolygon *clip, int zmin)
{

// Clip polygon against front of window at zmin coordinates

  // Transfer information from polygon's polybase class to
  //  clipped polyBase class:
	*clip = *polygon;

	// Clip against front of window view volume:

	int cp = 0; // Index to current vertex of clipped polygon

  	// Create pointer to clip_type vertices of clipped polygon class:

	clip_type *pcv = clip->GetClipPtr(0);

  	// Initialize pointer to last vertex:

	int v1=polygon->GetVertexCount()-1;

	// Loop through all edges of polygon

	for (int v2=0; v2<polygon->GetVertexCount(); v2++) {
		vertex_type *pv1=polygon->GetVerticePtr(v1);
		vertex_type *pv2=polygon->GetVerticePtr(v2);

		// Categorize edges by type:

		if ((pv1->az >= zmin) && (pv2->az >= zmin)) {

	  // Entirely inside front

			pcv[cp].x   = pv2->ax;
			pcv[cp].y   = pv2->ay;
			pcv[cp++].z = pv2->az;
		}
		//if ((pv1->az < zmin) && (pv2->az < zmin)){
	        // Edge is entirely past front, so do nothing
		//}
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

  	// Put number of vertices into clipped polygon:

	clip->SetVertexCount(cp);
}



void View::XYClip(ClippedPolygon *clip)
{
	// Clip against sides of viewport

	int temp; // Miscellaneous temporary storage
	clip_type *pcv=clip->GetClipPtr(0);

	// Clip against left edge of viewport:

	int cp = 0;

  // Initialize pointer to last vertex:

	int v1=clip->GetVertexCount()-1;
	for (int v2=0; v2<clip->GetVertexCount(); v2++) {

		// Categorize edges by type:

		if ((pcv[v1].x >= xmin) && (pcv[v2].x >= xmin)) {

      // Edge isn't off left side of viewport

			pcv[cp].x1   = pcv[v2].x;
			pcv[cp++].y1 = pcv[v2].y;
		}
		//else if ((pcv[v1].x < xmin) && (pcv[v2].x < xmin)){
	  	// Edge is entirely off left side of viewport,
	  	//  so don't do anything
		// }
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
	clip->SetVertexCount(cp);

	// CLip against right edge of viewport:

        // Initialize index to last vertex:
	v1=cp-1;

	// reset the counter:
	cp = 0;

	for (v2=0; v2<clip->GetVertexCount(); v2++) {

		// Categorize edges by type:

		if ((pcv[v1].x1 <= xmax) && (pcv[v2].x1 <= xmax)) {

	  // Edge isn't off right side of viewport

			pcv[cp].x = pcv[v2].x1;
			pcv[cp++].y = pcv[v2].y1;
		}
		//if ((pcv[v1].x1 > xmax) && (pcv[v2].x1 > xmax)){
	      	// Edge is entirely off right side of viewport,
	      	//  so do nothing
		//}
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
	clip->SetVertexCount(cp);

	// Clip against upper edge of viewport:

	// Initialize pointer to last vertex:

	v1=cp-1;

	// Reset current index:

	cp = 0;

	for (v2=0; v2<clip->GetVertexCount(); v2++) {

		// Categorize edges by type:

		if ((pcv[v1].y >= ymin) && (pcv[v2].y >= ymin)) {

		      // Edge is not off top off viewport

			pcv[cp].x1 = pcv[v2].x;
			pcv[cp++].y1 = pcv[v2].y;
		}
		//else if ((pcv[v1].y < ymin) && (pcv[v2].y < ymin)){
	        // Edge is entirely off top of viewport,
	        //  so don't do anything
		//}
		if ((pcv[v1].y >= ymin) && (pcv[v2].y < ymin)) {

		      // Edge is leaving viewport

			temp=(int)(pcv[v2].x-pcv[v1].x);
			if (temp!=0) {
				float m=(float)(pcv[v2].y-pcv[v1].y)/(float)temp;
				pcv[cp].x1 =
					pcv[v1].x + (ymin - pcv[v1].y) / m;
			}
			else pcv[cp].x1 = pcv[v1].x;
			pcv[cp++].y1 = ymin;
		}
		if ((pcv[v1].y < ymin) && (pcv[v2].y >= ymin)) {

      // Edge is entering viewport

			temp=(int)(pcv[v2].x-pcv[v1].x);
			if (temp!=0) {
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
	clip->SetVertexCount(cp);

	// Clip against lower edge of viewport:

	// Initialize pointer to last vertex:

	v1=cp-1;
	cp = 0;

	for (v2=0; v2<clip->GetVertexCount(); v2++) {

		// Categorize edges by type:

		if ((pcv[v1].y1 <= ymax) && (pcv[v2].y1 <= ymax)) {

	      // Edge is not off bottom of viewport

			pcv[cp].x = pcv[v2].x1;
			pcv[cp++].y = pcv[v2].y1;
		}
		//if ((pcv[v1].y1 > ymax) && (pcv[v2].y1 > ymax)){
	     	// Edge is entirely off bottom of viewport,
		//  so don't do anything
		//}
		if ((pcv[v1].y1 <= ymax) && (pcv[v2].y1 > ymax)) {

	  // Edge is leaving viewport
			temp=(int)(pcv[v2].x1-pcv[v1].x1);
			if (temp!=0) {
			float m=(float)(pcv[v2].y1-pcv[v1].y1)/(float)temp;
				pcv[cp].x =
					pcv[v1].x1 + (ymax - pcv[v1].y1) / m;
			}
			else pcv[cp].x = pcv[v1].x1;
			pcv[cp++].y = ymax;
		}
		if ((pcv[v1].y1 > ymax) && (pcv[v2].y1 <= ymax)) {
	  // Edge is entering viewport

			temp=(int)(pcv[v2].x1-pcv[v1].x1);
			if (temp!=0) {
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
	clip->SetVertexCount(cp);
}




void View::AlignView(World *world,view_type view)
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
  Object *optr = world->GetObjectPtr(0);
  for (int i=0; i<world->GetObjectCount(); i++, optr++) {
	optr->Atransform();  // Transform object i
  }
}

void View::Update(Object *object)
{
  if (object->NeedsUpdate()) {

	// Initialize transformations:

	inittrans();

	// Create scaling matrix:

	object->Scale();

	// Create rotation matrix:

	object->Rotate();

	// Create translation matrix:

	object->Translate();

	// Transform OBJECT with master transformation matrix:

	object->Transform();

	// Indicate update complete:

	object->Complete();
  }
}

// LOCALPOLYGON
// a local class for constructing an on the fly Polygon to
// use for the horizon.
class localPolygon: public Polygon
{
	public:
		localPolygon(int num, vertex_type *vertlist);
		// the base class (poygon_type) destroys allocated vertex ptr array
		~localPolygon(){;}
};

localPolygon::localPolygon(int num, vertex_type *vertlist)
{
  // Allocate memory for polygon and vertices
	vertex = new vertex_type *[num];
	number_of_vertices = num;
	for(int ii=0; ii< num;ii++)
		vertex[ii]=&vertlist[ii];

}
const int THREEQ_CIRCLE = (NUMBER_OF_DEGREES-QUARTER_CIRCLE);

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
  xangle&=SIN_MASK;
  if ((xangle>QUARTER_CIRCLE) && (xangle<=THREEQ_CIRCLE)) {
  //	xangle=(xangle+HALF_CIRCLE) & SIN_MASK;
	flip++;
  }
  zangle&=SIN_MASK;
  if ((zangle>QUARTER_CIRCLE) && (zangle<=THREEQ_CIRCLE)) {
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
  vert[3].ax=-32767;
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
  vert[2].ay=-32767;
  vert[3].ax=-32767;
  vert[3].ay=-32767;

  // Clip sky polygon:

  ZClip(&hpoly,&hclip);
  XYClip(&hclip);

  // Draw sky polygon:

  if (hclip.GetVertexCount())
	hclip.DrawPoly(screen);
}

