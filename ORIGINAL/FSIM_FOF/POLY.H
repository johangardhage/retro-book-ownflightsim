
// Constant definitions:

const PARALLEL=0;           // Orthographic projection flag
const PERSPECTIVE=1;        // Perspective projection flag

// Variable structures to hold shape data:

struct vertex_type {				// Structure for individual vertices
	long lx,ly,lz,lt;					// Local coordinates of vertex
	long wx,wy,wz,wt;          // World coordinates of vertex
	long ax,ay,az,at;          // World coordinates aligned with view
	long sx,sy,st;		    			// Screen coordinates of vertex
};

struct clip_type {
	long x,y,z;
	long x1,y1,z1;
};

struct clipped_polygon_type {
	int number_of_vertices;
	int color;
	clip_type vertex[20];
	int	zmax,zmin;					// Maximum and minimum z coordinates of polygon
	int xmax,xmin;
	int ymax,ymin;
};

struct	polygon_type {
	int	number_of_vertices;	// Number of vertices in polygon
	int number_of_clipped_vertices;
	int	color;              // Color of polygon
	int	zmax,zmin;					// Maximum and minimum z coordinates of polygon
	int xmax,xmin;
	int ymax,ymin;
	long double distance;
	vertex_type **vertex;		// List of vertices
	int	sortflag;						// For hidden surface sorts
};

struct	object_type{
	int		number_of_vertices;	// Number of vertices in object
	int		number_of_polygons;	// Number of polygons in object
	int		x,y,z;							// World coordinates of object's local origin
	int   xangle,yangle,zangle; // Orientation of object in space
	int   xscale,yscale,zscale;
	polygon_type	*polygon;		// List of polygons in object
	vertex_type *vertex;			// Array of vertices in object
	int		convex;							// Is it a convex polyhedron?
  int   update;             // Has position been updated?
};

struct	world_type {
	int		number_of_objects;
	object_type	*obj;
};

struct 	polygon_list_type {
	int		number_of_polygons;
	polygon_type *polygon;
};

struct view_type {
	int copx,copy,copz;
	int xangle,yangle,zangle;
};

// Function prototypes:

void inittrans();
void scale(float xf,float yf,float zf);
void reflect(int xr,int yr,int zr);
void rotate(int ax,int ay,int az);
void translate(int xt,int yt,int zt);
void transform(object_type *object);
void matmult(long result[4][4],long mat1[4][4],long mat2[4][4]);
void matcopy(long dest[4][4],long source[4][4]);
void update(object_type *object);
void atransform(object_type *object);
