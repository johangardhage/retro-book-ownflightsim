#ifndef POLY_G_H

// POLY2.H
// 9/12/95 -mickRacky - created from POLY.H

#define POLY_G_H

// Variable structures to hold shape data:

struct vertex_type { // Structure for individual vertices
  long lx,ly,lz,lt;	 // Local coordinates of vertex
  long wx,wy,wz,wt;  // World coordinates of vertex
  long ax,ay,az,at;  // World coordinates aligned with view
#ifdef LIGHTING_DEMO
  long sx,sy,st;		 // Screen coordinates of vertex
#endif
  long nlx,nly,nlz,nlt;  // Untransformed normal vector
  long nwx,nwy,nwz,nwt;  // Transformed normal vector
  long nlength;          // Length of normal vector
  int  brightness;       // Brightness for Gouraud shading
};

struct clip_type {
	long x,y,z;
	long x1,y1,z1;
};

// For experimenting with the five hidden surface tests in depth sorting,
// define HID_SURF_TEST5 which has the following effects:
// xmin, xmax, ymin, ymax, zmin and zmax int fields are added to the polyBase
// structure. These are copied when the polyBase is copied, 
// the polygon_type contains methods as found in Chapter 10 of FOF

// #define HID_SURF_TEST5

class polyBase {
protected:
	int	number_of_vertices;	// Number of vertices in polygon
	int original_color;     // Color of polygon
	int	color;              // Drawing color of polygon (after light applied)
	long nlength;           // normal length (for lighting)
#ifdef HID_SURF_TEST5
	int zmax,zmin;			// Maximum and minimum z coordinates of polygon
	int xmax,xmin;
	int ymax,ymin;
#endif
public:
	polyBase(){;}
	int GetVertexCount() const
		{ return number_of_vertices; }
	void SetVertexCount(int count)
		{ number_of_vertices = count; }
	int GetOriginalColor() const
		{ return original_color; }
	void SetColor(int val)
		{ color = val; }
	int GetColor() const
		{ return color; }
	long GetNormalLen()
		{ return nlength; }
#ifdef HID_SURF_TEST5
		// for outside access to set the 
		// max and min values:
	void SetMax(int x, int y, int z)
		{ xmax=x, ymax = y, zmax = z; }
	void SetMin(int x, int y, int z)
		{ xmin=x, ymin = y, zmin = z; }
#endif

	polyBase& polyBase::operator =(const polyBase& polygon);
};

#ifndef PALETTE_H
#include "palette.h"
#endif

class Polygon: public polyBase {
protected:
	friend class Object;  // access to vertex pointers
	vertex_type **vertex;		// List of vertices
	long double distance;
		// copy operator is protected since we want to
		// avoid possible killing of the vertex array through
		// an auto scoped variable
	Polygon& Polygon::operator =(const Polygon& polygon);
public:
	Polygon():polyBase(),vertex(0){;}
	~Polygon();
	int	backface();
	void DrawPoly(unsigned char far *screen_buffer,Palette *palette,int ambient);
	vertex_type * GetVerticePtr(int vertnum)
		{ return vertex[vertnum]; }
	long double GetDistance()
		{ return (distance); }
	void  SetDistance(long double dist)
		{ distance = dist; }
#ifdef HID_SURF_TEST5
	int should_be_swapped(const Polygon& poly2);
	int z_overlap(const Polygon& poly2);
	int xy_overlap(const Polygon& poly2);
	int surface_inside(const Polygon& poly2);
	int surface_outside(const Polygon& poly2);
#endif

};

class ClippedPolygon: public polyBase {
private:
	clip_type vertex[20];
public:
	ClippedPolygon():polyBase(){;}
	void DrawPoly(unsigned char far *screen);
	clip_type * GetClipPtr(int vertnum)
		{ return &vertex[vertnum]; }
	void Project(int distance, int xorigin, int yorigin);
	ClippedPolygon& operator =(const Polygon& polygon)
		{
			((polyBase& )*this) = (polyBase&)polygon;
			return *this;
		}

};

class	Object{
private:
	int		number_of_vertices;	// Number of vertices in object
	int		number_of_polygons;	// Number of polygons in object
	int		x,y,z;				// World coordinates of object's local origin
	int   xangle,yangle,zangle; // Orientation of object in space
	int   xscale,yscale,zscale;
	Polygon	*polygon;  // List of polygons in object
	vertex_type *vertex;		// Array of vertices in object
	int		convex;			   // Is it a convex polyhedron?
	int   update;             // Has position been updated?
public:
	Object():polygon(0),vertex(0){;}
	~Object();
	int Load();
	void Transform();
	void Atransform();
	// set Scale matrix to xscale, yscale, zscale
	void Scale();
	// set Rotate matrix to to xangle, yangle, zangle
	void Rotate();
	// set tanslation matrix to x,y,z
	void Translate();
#ifdef LIGHTING_DEMO
		// perform projection
	void Project(int distance,int xorigin,int yorigin);
		//loop through polygons using Gouraud drawpoly
	void Draw(unsigned char far *screen, Palette *pal, int ambient);
#endif
	int GetPolygonCount() const
		{ return number_of_polygons; }
	Polygon * GetPolygonPtr(int polynum) const
		{ return &polygon[polynum]; }
    int isConvex()
		{ return (convex != 0); }
	void Complete()
		{ update = 0; }
	int NeedsUpdate()
		{ return( update != 0); }
	void MeasureNormals(int numshades);
		// need these for GOURAUD calculation of colors:
	int GetVertexCount() const
		{ return number_of_vertices; }
	vertex_type *GetVertexPtr(int vnum)
		{ return &vertex[vnum]; }
};



// Function prototypes:

void inittrans();
void scale(int sf);
void scale(float xs,float ys,float zs);
void rotate(int ax,int ay,int az);
void translate(int xt,int yt,int zt);


#endif