#ifndef POLY_H

// POLY.H
// 8/28/95 -mickRacky - updated to C++ 
// 9/2/95  -mickRacky - commented out XORIGIN and YORIGIN const for 
//                      optimized version and changed class functions
//                      to pass them as parameters.
#define POLY_H

// Variable structures to hold shape data:

struct vertex_type { // Structure for individual vertices
  long lx,ly,lz,lt;	 // Local coordinates of vertex
  long wx,wy,wz,wt;  // World coordinates of vertex
  long sx,sy,st;		 // Screen coordinates of vertex
};

class	polygon_type {
private:
	friend class object_type;  // access to vertex pointers
	int	number_of_vertices;	// Number of vertices in polygon
	int	color;              // Color of polygon
	vertex_type **vertex;		// List of vertices
public:
	polygon_type():vertex(0){;}
	~polygon_type();
	int	backface();
	void DrawPoly(unsigned char far *screen);
};

class	object_type{
private:
	int		number_of_vertices;	// Number of vertices in object
	int		number_of_polygons;	// Number of polygons in object
	int		x,y,z;							// World coordinates of object's local origin
	polygon_type	*polygon;		// List of polygons in object
	vertex_type *vertex;			// Array of vertices in object
	int		convex;							// Is it a convex polyhedron?
public:
	object_type():polygon(0),vertex(0){;}
	~object_type();
	int load();
	void Transform();
	void Project(int distance, int xorigin, int yorigin);
	void Draw(unsigned char far *screen);
};

class	world_type {
private:
	int		number_of_objects;
	object_type	*obj;
public:
	world_type():obj(0){;}
	~world_type();
	int loadpoly(char *filename);
	void Draw(int objnum, unsigned char far *screen, int origin, int yorigin);
	void Draw(unsigned char far *screen, int xorigin, int yorigin);
	// for outside access:
	int GetObjectCount()
		{ return number_of_objects; }
};


// Function prototypes:

void inittrans();
void scale(int sf);
void rotate(int ax,int ay,int az);
void translate(int xt,int yt,int zt);


#endif