#ifndef POLY_FR_H

// POLY_FR.H
// 9/12/95 -mickRacky - created from POLY.H

#define POLY_FR_H

// Variable structures to hold shape data:

struct vertex_type { // Structure for individual vertices
  long lx,ly,lz,lt;	 // Local coordinates of vertex
  long wx,wy,wz,wt;  // World coordinates of vertex
  long ax,ay,az,at;  // World coordinates aligned with view
  long sx,sy,st;		 // Screen coordinates of vertex
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
public:
	polyBase(){;}
	int GetVertexCount() const
		{ return number_of_vertices; }
	void SetVertexCount(int count)
		{ number_of_vertices = count; }
	int GetOriginalColor() const
		{ return original_color; }
	void SetOriginalColor(int col)
		{ original_color = col; }
	void SetColor(int val)
		{ color = val; }
	int GetColor() const
		{ return color; }
	long GetNormalLen()
		{ return nlength; }

	polyBase& polyBase::operator =(const polyBase& polygon);
};

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
	int MakeVertexArray(int vnum);
	void SetVertexPtr(int vnum, vertex_type *vptr)
		{ vertex[vnum] = vptr; }
	int	backface();
	void DrawPoly(unsigned char far *screen);
	vertex_type * GetVertexPtr(int vertnum)
		{ return vertex[vertnum]; }
	long double GetDistance()
		{ return (distance); }
	void  SetDistance(long double dist)
		{ distance = dist; }
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

const int CONVEX = 1;
const int UPDATE = 2;
const int VISIBLE = 4;
class	Object{
private:
	int	number_of_vertices;	// Number of vertices in object
	int	number_of_polygons;	// Number of polygons in object
	int	x,y,z;				// World coordinates of object's local origin
	int xangle,yangle,zangle; // Orientation of object in space
	int xscale,yscale,zscale;
	Polygon	*polygon;  // List of polygons in object
	vertex_type *vertex;		// Array of vertices in object
	//int	convex;			   // Is it a convex polyhedron?
	//int update;             // Has position been updated?
	int flags;
public:
	Object():polygon(0),vertex(0) {;}
	~Object();
	int Init(int vnum, int polyCnt);
	int Load();
	void Transform();
	void Atransform();
	// set Scale matrix to xscale, yscale, zscale
	void Scale();
	// set Rotate matrix to to xangle, yangle, zangle
	void Rotate();
	// set tanslation matrix to x,y,z
	void Translate();

		// perform projection
	void Project(int distance,int xorigin,int yorigin);
		//loop through polygons using drawpoly
	void Draw(unsigned char far *screen);

	int GetPolygonCount() const
		{ return number_of_polygons; }
	Polygon * GetPolygonPtr(int polynum) const
		{ return &polygon[polynum]; }

    int isConvex()
		{ return ((flags & CONVEX) != 0); }
	void Complete()
		{ flags &= (~UPDATE); }
	int NeedsUpdate()
		{ return ((flags & UPDATE) != 0); }
	void Uncloak()
		{ flags |= VISIBLE; }
	void Cloak()
		{ flags &= (~VISIBLE); }
	int isVisible()
		{ return ((flags & VISIBLE) != 0); }


	int GetVertexCount() const
		{ return number_of_vertices; }
	vertex_type *GetVertexPtr(int vnum)
		{ return &vertex[vnum]; }

	void MeasureNormals(int numshades);

	void SetNumVertices(int count)
		{ number_of_vertices = count; }
	void SetNumPolygons(int count)
		{ number_of_polygons = count; }
};



// Function prototypes:

void inittrans();
void scale(int sf);
void scale(float xs,float ys,float zs);
void rotate(int ax,int ay,int az);
void translate(int xt,int yt,int zt);


#endif