// POLY_G.CPP
//   A set of functions to create translation matrices for various
// polygon transformation functions, to transform a polygon-fill
// object using a matrix concatenated from those matrices, and to
// draw a polygon-fill object on the mode 13h display
//
// copyright (c) 1995 Chris Lampton and Waite Group Press
//
// 9/12/95 mracky@siruis.com  update to C++; created Polygon and Object classes

#ifndef POLYGON_H
#define POLYGON_H

#define ONE (SHIFT_MULT)

// Variable structures to hold shape data:
struct vertex_type { // Structure for individual vertices
	long lx, ly, lz, lt;	 // Local coordinates of vertex
	long wx, wy, wz, wt;  // World coordinates of vertex
	long ax, ay, az, at;  // World coordinates aligned with view
	long sx, sy, st;		 // Screen coordinates of vertex
	long nlx, nly, nlz, nlt;  // Untransformed normal vector
	long nwx, nwy, nwz, nwt;  // Transformed normal vector
	long nlength;          // Length of normal vector
	int  brightness;       // Brightness for Gouraud shading
};

struct clip_type {
	long x, y, z;
	long x1, y1, z1;
};

// Global transformation arrays:
long matrix[4][4];         // Master transformation matrix
long smat[4][4];				    // Scaling matrix
long zmat[4][4];				    // Z rotation matrix
long xmat[4][4];				    // X rotation matrix
long ymat[4][4];				    // Y rotation matrix
long tmat[4][4];           // Translation matrix
long rmat[4][4];					  // Perspective matrix

void matmult(long result[4][4], long mat1[4][4], long mat2[4][4])
{
	// Multiply matrix MAT1 by matrix MAT2, returning the result in RESULT
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result[i][j] = ((mat1[i][0] * mat2[0][j]) + (mat1[i][1] * mat2[1][j]) + (mat1[i][2] * mat2[2][j]) + (mat1[i][3] * mat2[3][j])) >> SHIFT;
		}
	}
}

void matcopy(long dest[4][4], long source[4][4])
{
	// Copy matrix SOURCE to matrix DEST
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			dest[i][j] = source[i][j];
		}
	}
}

void inittrans()
{
	// Initialize master transformation matrix to the identity matrix
	matrix[0][0] = ONE; matrix[0][1] = 0;   matrix[0][2] = 0;   matrix[0][3] = 0;
	matrix[1][0] = 0;   matrix[1][1] = ONE; matrix[1][2] = 0;   matrix[1][3] = 0;
	matrix[2][0] = 0;   matrix[2][1] = 0;   matrix[2][2] = ONE; matrix[2][3] = 0;
	matrix[3][0] = 0;   matrix[3][1] = 0;   matrix[3][2] = 0;   matrix[3][3] = ONE;
}

void scale(int sf)
{
	// shift the value for storage
	long  val = long(sf) << SHIFT;

	// Initialize scaling matrix:
	smat[0][0] = val; smat[0][1] = 0; smat[0][2] = 0; smat[0][3] = 0;
	smat[1][0] = 0; smat[1][1] = val; smat[1][2] = 0; smat[1][3] = 0;
	smat[2][0] = 0; smat[2][1] = 0; smat[2][2] = val; smat[2][3] = 0;
	smat[3][0] = 0; smat[3][1] = 0; smat[3][2] = 0;  smat[3][3] = ONE;

	// Concatenate with master matrix:
	long mat[4][4];
	matmult(mat, smat, matrix);
	matcopy(matrix, mat);
}

void scale(float xs, float ys, float zs)
{
	// Initialize scaling matrix:
	smat[0][0] = (long)(zs * SHIFT_MULT); smat[0][1] = 0; smat[0][2] = 0; smat[0][3] = 0;
	smat[1][0] = 0; smat[1][1] = (long)(ys * SHIFT_MULT); smat[1][2] = 0; smat[1][3] = 0;
	smat[2][0] = 0; smat[2][1] = 0; smat[2][2] = (long)(xs * SHIFT_MULT); smat[2][3] = 0;
	smat[3][0] = 0; smat[3][1] = 0; smat[3][2] = 0;  smat[3][3] = ONE;

	// Concatenate with master matrix:
	long mat[4][4];

	matmult(mat, smat, matrix);
	matcopy(matrix, mat);
}

void reflect(int xr, int yr, int zr)
{
	rmat[0][0] = xr >> SHIFT; rmat[0][1] = 0; rmat[0][2] = 0; rmat[0][3] = 0;
	rmat[1][0] = 0; rmat[1][1] = yr >> SHIFT; rmat[1][2] = 0; rmat[1][3] = 0;
	rmat[2][0] = 0; rmat[2][1] = 0; rmat[2][2] = zr >> SHIFT; rmat[2][3] = 0;
	rmat[3][0] = 0; rmat[3][1] = 0; rmat[3][2] = 0; rmat[3][3] = ONE;

	long mat[4][4];

	matmult(mat, matrix, rmat);
	matcopy(matrix, mat);
}

void rotate(int ax, int ay, int az)
{
	// Create three rotation matrices that will rotate an object
	// AX radians on the X axis, AY radians on the Y axis and
	// AZ radians on the Z axis

	long mat1[4][4];
	long mat2[4][4];

	// Initialize X rotation matrix:
	xmat[0][0] = ONE;  xmat[0][1] = 0;        xmat[0][2] = 0;       xmat[0][3] = 0;
	xmat[1][0] = 0;    xmat[1][1] = COS(ax);  xmat[1][2] = SIN(ax); xmat[1][3] = 0;
	xmat[2][0] = 0;    xmat[2][1] = -SIN(ax); xmat[2][2] = COS(ax); xmat[2][3] = 0;
	xmat[3][0] = 0;    xmat[3][1] = 0;        xmat[3][2] = 0;       xmat[3][3] = ONE;

	// Initialize Y rotation matrix:
	ymat[0][0] = COS(ay); ymat[0][1] = 0;   ymat[0][2] = -SIN(ay); ymat[0][3] = 0;
	ymat[1][0] = 0;       ymat[1][1] = ONE; ymat[1][2] = 0;        ymat[1][3] = 0;
	ymat[2][0] = SIN(ay); ymat[2][1] = 0;   ymat[2][2] = COS(ay);  ymat[2][3] = 0;
	ymat[3][0] = 0;       ymat[3][1] = 0;   ymat[3][2] = 0;        ymat[3][3] = ONE;

	// Initialize Z rotation matrix:
	zmat[0][0] = COS(az);  zmat[0][1] = SIN(az);  zmat[0][2] = 0;    zmat[0][3] = 0;
	zmat[1][0] = -SIN(az); zmat[1][1] = COS(az);  zmat[1][2] = 0;    zmat[1][3] = 0;
	zmat[2][0] = 0;        zmat[2][1] = 0;        zmat[2][2] = ONE;  zmat[2][3] = 0;
	zmat[3][0] = 0;        zmat[3][1] = 0;        zmat[3][2] = 0;    zmat[3][3] = ONE;

	// Concatenate this matrix with master matrix:
	matmult(mat1, matrix, ymat);
	matmult(mat2, mat1, xmat);
	matmult(matrix, mat2, zmat);
}

void translate(int xt, int yt, int zt)
{
	// Create a translation matrix that will translate an object an
	// X distance of XT, a Y distance of YT, and a Z distance of ZT
	// from the screen origin

	tmat[0][0] = ONE;      tmat[0][1] = 0;      tmat[0][2] = 0;      tmat[0][3] = 0;
	tmat[1][0] = 0;        tmat[1][1] = ONE;    tmat[1][2] = 0;      tmat[1][3] = 0;
	tmat[2][0] = 0;        tmat[2][1] = 0;      tmat[2][2] = ONE;    tmat[2][3] = 0;
	tmat[3][0] = (long)xt << SHIFT; tmat[3][1] = (long)yt << SHIFT; tmat[3][2] = (long)zt << SHIFT; tmat[3][3] = ONE;

	// Concatenate with master matrix:
	long mat[4][4];

	matmult(mat, matrix, tmat);
	matcopy(matrix, mat);
}

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
	int zmax, zmin;			// Maximum and minimum z coordinates of polygon
	int xmax, xmin;
	int ymax, ymin;
#endif

public:
	polyBase() { ; }

	int GetVertexCount() const
	{
		return number_of_vertices;
	}

	void SetVertexCount(int count)
	{
		number_of_vertices = count;
	}

	int GetOriginalColor() const
	{
		return original_color;
	}

	void SetColor(int val)
	{
		color = val;
	}

	int GetColor() const
	{
		return color;
	}

	long GetNormalLen()
	{
		return nlength;
	}

#ifdef HID_SURF_TEST5
	// for outside access to set the max and min values:
	void SetMax(int x, int y, int z)
	{
		xmax = x, ymax = y, zmax = z;
	}

	void SetMin(int x, int y, int z)
	{
		xmin = x, ymin = y, zmin = z;
	}
#endif

	polyBase &operator =(const polyBase &polygon)
	{
		color = polygon.color;
#ifdef HID_SURF_TEST5
		xmin = polygon.xmin;
		xmax = polygon.xmax;
		zmin = polygon.zmin;
		zmax = polygon.zmax;
		ymin = polygon.ymin;
		ymax = polygon.ymax;
#endif
		return *this;
	}
};

class Polygon : public polyBase {
protected:
	friend class Object;  // access to vertex pointers
	vertex_type **vertex;		// List of vertices
	long double distance;

	// copy operator is protected since we want to
	// avoid possible killing of the vertex array through
	// an auto scoped variable
	Polygon &operator =(const Polygon &polygon)
	{
		// copy the polybase:
		((polyBase &)*this) = (polyBase &)polygon;
		number_of_vertices = polygon.GetVertexCount();
		//vertex = polygon.vertex;
		distance = polygon.distance;

		return *this;
	}

public:
	Polygon() :polyBase(), vertex(0) { ; }

	~Polygon()
	{
		if (vertex) {
			delete[] vertex;
		}
	}

	int	backface()
	{
		vertex_type *v0, *v1, *v2;  // Pointers to three vertices
		// Point to vertices:
		v0 = vertex[0];
		v1 = vertex[1];
		v2 = vertex[2];
		long z = (v1->sx - v0->sx) * (v2->sy - v0->sy) - (v1->sy - v0->sy) * (v2->sx - v0->sx);
		return(z >= 0);
	}

	// Draw polygon in structure POLYGON in SCREEN_BUFFER
	void DrawPoly(unsigned char *screen_buffer, Palette *palette, int ambient)
	{
		// Draw polygon in structure POLYGON in SCREEN_BUFFER

		// Uninitialized variables:
		unsigned int start,        // Starting offset of line between edges
			offset1,      // Offset of current pixel in edges 1 & 2
			offset2;

		int ydiff1, ydiff2,         // Difference between starting x and ending x
			xdiff1, xdiff2,         // Difference between starting y and ending y
			cdiff1, cdiff2,         // Difference between starting color and ending color
			length,                // Distance from edge 1 to edge 2
			errorterm1, errorterm2, // Error terms for edges 1 & 2
			count1, count2,         // Increment count for edges 1 & 2
			xunit1, xunit2,         // Unit to advance x offset for edges 1 & 2
			intincr1, intincr2,     // Standard integer increments for x on edges 1 & 2
			errorincr1, errorincr2, // Error increments for x on edges 1 & 2
			cstart1, cstart2,       // Starting edge colors
			cend1, cend2,           // Ending edge colors
			cerrorterm1, cerrorterm2,// Error terms for color for edges 1 & 2
			cunit1, cunit2,         // Unit to advance color for edges 1 & 2
			cintincr1, cintincr2,   // Standard integer color increments on edges 1 & 2
			cerrorincr1, cerrorincr2, // Error color increments on edges 1 & 2
			cdiff,
			cstart,                // Starting edge colors
			cend,                  // Ending edge colors
			cerrorterm,            // Error terms for color for edges 1 & 2
			cunit,                 // Unit to advance color for edges 1 & 2
			cintincr,              // Standard integer color increments on edges 1 & 2
			cerrorincr;            // Error color increments on edges 1 & 2

		// Initialize count of number of edges drawn:
		int edgecount = number_of_vertices - 1;

		// Determine which vertex is at top of polygon:
		int firstvert = 0;           // Start by assuming vertex 0 is at top
		int min_amt = vertex[0]->sy; // Find y coordinate of vertex 0
		for (int i = 1; i < number_of_vertices; i++) {  // Search thru vertices
			if ((vertex[i]->sy) < min_amt) {  // Is another vertex higher?
				firstvert = i;                   // If so, replace previous top vertex
				min_amt = vertex[i]->sy;
			}
		}

		// Finding starting and ending vertices of first two edges:
		int startvert1 = firstvert;      // Get starting vertex of edge 1
		int startvert2 = firstvert;      // Get starting vertex of edge 2
		int xstart1 = vertex[startvert1]->sx;
		int ystart1 = vertex[startvert1]->sy;
		int xstart2 = vertex[startvert2]->sx;
		int ystart2 = vertex[startvert2]->sy;
		int endvert1 = startvert1 - 1;                   // Get ending vertex of edge 1
		if (endvert1 < 0) {
			endvert1 = number_of_vertices - 1;  // Check for wrap
		}
		int xend1 = vertex[endvert1]->sx;      // Get x & y coordinates
		int yend1 = vertex[endvert1]->sy;      // of ending vertices
		int endvert2 = startvert2 + 1;                   // Get ending vertex of edge 2
		if (endvert2 == (number_of_vertices)) {
			endvert2 = 0;  // Check for wrap
		}
		int xend2 = vertex[endvert2]->sx;      // Get x & y coordinates
		int yend2 = vertex[endvert2]->sy;      // of ending vertices

		// Draw the polygon:
		char calc1 = 1;
		char calc2 = 1;

		while (edgecount > 0) {    // Continue drawing until all edges drawn
			if (calc1) {
				calc1 = 0;
				offset1 = 320 * ystart1 + xstart1;  // Offset of edge 1
				errorterm1 = 0;                 // Initialize error term
				if ((ydiff1 = yend1 - ystart1) < 0) {
					ydiff1 = -ydiff1; // Get abs value of y length
				}
				count1 = ydiff1;
				if (count1) {
					xdiff1 = xend1 - xstart1;
					intincr1 = xdiff1 / ydiff1;
					if (xdiff1 < 0) {                               // Get value of length
						xunit1 = -1;                                  // Calculate X increment
						errorincr1 = -xdiff1 % ydiff1;
					} else {
						xunit1 = 1;
						errorincr1 = xdiff1 % ydiff1;
					}

					cstart1 = vertex[startvert1]->brightness;
					cend1 = vertex[endvert1]->brightness;
					cerrorterm1 = 0;
					cdiff1 = cend1 - cstart1;
					cintincr1 = cdiff1 / ydiff1;
					if (cdiff1 < 0) {
						cunit1 = -1;
						cerrorincr1 = -cdiff1 % ydiff1;
					} else {
						cunit1 = 1;
						cerrorincr1 = cdiff1 % ydiff1;
					}
				}
			}

			if (calc2) {
				calc2 = 0;
				offset2 = 320 * ystart2 + xstart2;  // Offset of edge 2
				errorterm2 = 0;
				if ((ydiff2 = yend2 - ystart2) < 0) {
					ydiff2 = -ydiff2; // x & y lengths of edges
				}
				count2 = ydiff2;
				if (count2) {
					xdiff2 = xend2 - xstart2;
					intincr2 = xdiff2 / ydiff2;
					if (xdiff2 < 0) {                               // Get value of length
						xunit2 = -1;                                  // Calculate X increment
						errorincr2 = -xdiff2 % ydiff2;
					} else {
						xunit2 = 1;
						errorincr2 = xdiff2 % ydiff2;
					}

					cstart2 = vertex[startvert2]->brightness;
					cend2 = vertex[endvert2]->brightness;
					cerrorterm2 = 0;
					cdiff2 = cend2 - cstart2;
					cintincr2 = cdiff2 / ydiff2;
					if (cdiff2 < 0) {
						cunit2 = -1;
						cerrorincr2 = -cdiff2 % ydiff2;
					} else {
						cunit2 = 1;
						cerrorincr2 = cdiff2 % ydiff2;
					}
				}
			}

			for (; count1 && count2; count1--, count2--) {
				length = offset2 - offset1; // Determine length of horizontal line
				if (length < 0) {         // If negative...
					length = -length;       // Make it positive
					start = offset2;        // And set START to edge 2
					cstart = cstart2;
					cend = cstart1;
				} else {
					start = offset1;     // Else set START to edge 1
					cstart = cstart1;
					cend = cstart2;
				}
				length++;

				cerrorterm = 0;
				cdiff = cend - cstart;
				cintincr = cdiff / length;
				if (cdiff < 0) {
					cunit = -1;
					cerrorincr = -cdiff % length;
				} else {
					cunit = 1;
					cerrorincr = cdiff % length;
				}

				int l = length;
				for (unsigned char *p = &screen_buffer[start];l--;) {
					*p++ = palette->ColorIndex(original_color, cstart, ambient);
					cstart += cintincr;
					cerrorterm += cerrorincr;
					if (cerrorterm >= length) {
						cerrorterm -= length;
						cstart += cunit;
					}
				}

				offset1 += intincr1 + 320;
				errorterm1 += errorincr1;
				if (errorterm1 >= ydiff1) {
					errorterm1 -= ydiff1;
					offset1 += xunit1;
				}
				cstart1 += cintincr1;
				cerrorterm1 += cerrorincr1;
				if (cerrorterm1 >= ydiff1) {
					cerrorterm1 -= ydiff1;
					cstart1 += cunit1;
				}

				offset2 += intincr2 + 320;
				errorterm2 += errorincr2;
				if (errorterm2 >= ydiff2) {
					errorterm2 -= ydiff2;
					offset2 += xunit2;
				}
				cstart2 += cintincr2;
				cerrorterm2 += cerrorincr2;
				if (cerrorterm2 >= ydiff2) {
					cerrorterm2 -= ydiff2;
					cstart2 += cunit2;
				}
			}

			// Another edge (at least) is complete. Start next edge, if any.
			if (!count1) {           // If edge 1 is complete...
				--edgecount;           // Decrement the edge count
				startvert1 = endvert1;   // Make ending vertex into start vertex
				--endvert1;            // And get new ending vertex
				if (endvert1 < 0) {
					endvert1 = number_of_vertices - 1; // Check for wrap
				}
				xstart1 = xend1;
				ystart1 = yend1;
				xend1 = vertex[endvert1]->sx;  // Get x & y of new end vertex
				yend1 = vertex[endvert1]->sy;
				calc1 = 1;
			}
			if (!count2) {          // If edge 2 is complete...
				--edgecount;          // Decrement the edge count
				startvert2 = endvert2;  // Make ending vertex into start vertex
				endvert2++;           // And get new ending vertex
				if (endvert2 == (number_of_vertices)) {
					endvert2 = 0; // Check for wrap
				}
				xstart2 = xend2;
				ystart2 = yend2;
				xend2 = vertex[endvert2]->sx;  // Get x & y of new end vertex
				yend2 = vertex[endvert2]->sy;
				calc2 = 1;
			}
		}
	}

	vertex_type *GetVerticePtr(int vertnum)
	{
		return vertex[vertnum];
	}

	long double GetDistance()
	{
		return (distance);
	}

	void  SetDistance(long double dist)
	{
		distance = dist;
	}
};

class ClippedPolygon : public polyBase {
private:
	clip_type vertex[20];

public:
	ClippedPolygon() :polyBase() { ; }

	void DrawPoly(unsigned char *screen);

	clip_type *GetClipPtr(int vertnum)
	{
		return &vertex[vertnum];
	}

	void Project(int distance, int xorigin, int yorigin)
	{
		// Project clipped polygon onto screen using perspective projection
		clip_type *vptr = vertex;
		float dd = distance;       // convert to float for precision

		for (int v = 0; v < number_of_vertices; v++, vptr++) {
			// Loop through vertices
			float z = (float)(ABS(vptr->z));
			vptr->x = dd * ((float)vptr->x / z) + xorigin;  // ...divide world x&y coords
			vptr->y = dd * ((float)vptr->y / z) + yorigin;  // ...by z coordinates
		}
	}

	ClippedPolygon &operator =(const Polygon &polygon)
	{
		((polyBase &)*this) = (polyBase &)polygon;
		return *this;
	}
};

class Object {
private:
	int number_of_vertices;	// Number of vertices in object
	int number_of_polygons;	// Number of polygons in object
	int x, y, z;				// World coordinates of object's local origin
	int xangle, yangle, zangle; // Orientation of object in space
	int xscale, yscale, zscale;
	Polygon *polygon;  // List of polygons in object
	vertex_type *vertex;		// Array of vertices in object
	int convex;			   // Is it a convex polyhedron?
	int update;             // Has position been updated?

public:
	Object() :polygon(0), vertex(0) { ; }
	~Object()
	{
		if (polygon) {
			delete[] polygon;
		}
		if (vertex) {
			delete[] vertex;
		}
	}

	int Load()
	{
		// Get x,y and z coordinates of object's local origin:
		x = _pf.getnumber();
		y = _pf.getnumber();
		z = _pf.getnumber();

		// Get orientation of object:
		xangle = _pf.getnumber();
		yangle = _pf.getnumber();
		zangle = _pf.getnumber();

		// Get x,y and z scale factors for object:
		xscale = _pf.getnumber();
		yscale = zscale = xscale;

		number_of_vertices = _pf.getnumber();
		vertex = new vertex_type[number_of_vertices];
		if (vertex == 0) {
			return(-1);
		}

		for (int vertnum = 0; vertnum < number_of_vertices; vertnum++) {
			vertex[vertnum].lx = _pf.getnumber();
			vertex[vertnum].ly = _pf.getnumber();
			vertex[vertnum].lz = _pf.getnumber();
			vertex[vertnum].lt = 1;
			vertex[vertnum].wt = 1;
		}

		number_of_polygons = _pf.getnumber();

		polygon = new Polygon[number_of_polygons];
		if (polygon == 0) {
			return(-1);
		}

		// Add to global polygon count:
		polycount += number_of_polygons;

		Polygon *polyptr = polygon;
		for (int polynum = 0; polynum < number_of_polygons; polynum++, polyptr++) {
			polyptr->number_of_vertices = _pf.getnumber();
			polyptr->vertex = new vertex_type * [number_of_vertices];
			if (polyptr->vertex == 0) {
				return(-1);
			}
			for (int vertnum = 0; vertnum < polyptr->number_of_vertices; vertnum++) {
				polyptr->vertex[vertnum] = &vertex[_pf.getnumber()];
			}
			polyptr->original_color = _pf.getnumber();
		}
		convex = _pf.getnumber();
		update = 1;
		return(0);
	}

	void Transform()
	{
		// Multiply all vertices in OBJECT with master transformation matrix:
		vertex_type *vptr = vertex;
		for (int v = 0; v < number_of_vertices; v++, vptr++) {
			vptr->wx = ((long)vptr->lx * matrix[0][0] + (long)vptr->ly * matrix[1][0] + (long)vptr->lz * matrix[2][0] + matrix[3][0]) >> SHIFT;
			vptr->wy = ((long)vptr->lx * matrix[0][1] + (long)vptr->ly * matrix[1][1] + (long)vptr->lz * matrix[2][1] + matrix[3][1]) >> SHIFT;
			vptr->wz = ((long)vptr->lx * matrix[0][2] + (long)vptr->ly * matrix[1][2] + (long)vptr->lz * matrix[2][2] + matrix[3][2]) >> SHIFT;

			// Transform Normal Vector for GOURAUD shading (no translation)
			vptr->nwx = ((long)vptr->nlx * matrix[0][0] + (long)vptr->nly * matrix[1][0] + (long)vptr->nlz * matrix[2][0]) >> SHIFT;
			vptr->nwy = ((long)vptr->nlx * matrix[0][1] + (long)vptr->nly * matrix[1][1] + (long)vptr->nlz * matrix[2][1]) >> SHIFT;
			vptr->nwz = ((long)vptr->nlx * matrix[0][2] + (long)vptr->nly * matrix[1][2] + (long)vptr->nlz * matrix[2][2]) >> SHIFT;
		}
	}

	void Atransform()
	{
		// Multiply all vertices in OBJECT with master
		//  transformation matrix

		// Create pointer to vertex:
		vertex_type *vptr = vertex;

		// Step through all vertices of OBJECT:
		for (int v = 0; v < number_of_vertices; v++, vptr++) {
			// Calculate new aligned x coordinate:
			vptr->ax = ((long)vptr->wx * matrix[0][0] + (long)vptr->wy * matrix[1][0] + (long)vptr->wz * matrix[2][0] + matrix[3][0]) >> SHIFT;

			// Calculate new aligned y coordinate:
			vptr->ay = ((long)vptr->wx * matrix[0][1] + (long)vptr->wy * matrix[1][1] + (long)vptr->wz * matrix[2][1] + matrix[3][1]) >> SHIFT;

			// Calculate new aligned z coordinate:
			vptr->az = ((long)vptr->wx * matrix[0][2] + (long)vptr->wy * matrix[1][2] + (long)vptr->wz * matrix[2][2] + matrix[3][2]) >> SHIFT;
		}
	}

	// set Scale matrix to xscale, yscale, zscale
	void Scale()
	{
		scale(xscale, yscale, zscale);
	}

	// set Rotate matrix to to xangle, yangle, zangle
	void Rotate()
	{
		rotate(xangle, yangle, zangle);
	}

	// set tanslation matrix to x,y,z
	void Translate()
	{
		translate(x, y, z);
	}

	// perform projection
	void Project(int distance, int xorigin, int yorigin)
	{
		// Project object onto screen with perspective projection

		vertex_type *vptr = vertex;  // Point to current vertex
		for (int v = 0; v < number_of_vertices; v++, vptr++) { // Loop through vertices
			vptr->sx = distance * vptr->wx / (distance - vptr->wz) + xorigin;  // ...divide world x&y coords
			vptr->sy = distance * vptr->wy / (distance - vptr->wz) + yorigin;  // ... by z coordinates
		}
	}

	//loop through polygons using Gouraud drawpoly
	void Draw(unsigned char *screen, Palette *pal, int ambient)
	{
		// Loop through all polygons in object:
		Polygon *polyptr = polygon;
		for (int p = 0; p < number_of_polygons; p++, polyptr++) {
			// Draw current polygon:
			if (convex) {
				if (!polyptr->backface()) {
					polyptr->DrawPoly(screen, pal, ambient);
				}
			} else {
				polyptr->DrawPoly(screen, pal, ambient);
			}
		}
	}

	int GetPolygonCount() const
	{
		return number_of_polygons;
	}

	Polygon *GetPolygonPtr(int polynum) const
	{
		return &polygon[polynum];
	}

	int isConvex()
	{
		return (convex != 0);
	}

	void Complete()
	{
		update = 0;
	}

	int NeedsUpdate()
	{
		return(update != 0);
	}

	// In the gouraud demo, the meausre normals function is used
	// to measure the vector_type normals.
	void MeasureNormals(int numshades)
	{
		// Calculate length of normal for each vertex in this object:
		for (int v = 0; v < number_of_vertices; v++) {
			vertex[v].nlx = 0;
			vertex[v].nly = 0;
			vertex[v].nlz = 0;
		}

		for (int p = 0; p < GetPolygonCount(); p++) {
			Polygon *pptr = &polygon[p];
			if (pptr->original_color >= numshades) {
				vertex_type *v0 = pptr->vertex[0];
				vertex_type *v1 = pptr->vertex[1];
				vertex_type *v2 = pptr->vertex[2];
				long ax = v0->lx - v1->lx;  // Vector for edge from vertex 1 to vertex 0
				long ay = v0->ly - v1->ly;
				long az = v0->lz - v1->lz;
				long bx = v2->lx - v1->lx;  // Vector for edge from vertex 1 to vertex 2
				long by = v2->ly - v1->ly;
				long bz = v2->lz - v1->lz;
				long nx = ay * bz - az * by;    // Normal is cross product of the two vectors
				long ny = az * bx - ax * bz;
				long nz = ax * by - ay * bx;
				// OBJECT can access POLYGON since its a friend:
				for (int v = 0; v < pptr->number_of_vertices; v++) {
					vertex_type *vptr = pptr->vertex[v];
					vptr->nlx += nx;
					vptr->nly += ny;
					vptr->nlz += nz;
				}
			}
		}

		for (int v = 0; v < number_of_vertices; v++) {
			vertex_type *vptr = &vertex[v];
			vptr->nlength = (long)sqrt((float)vptr->nlx * vptr->nlx + (float)vptr->nly * vptr->nly + (float)vptr->nlz * vptr->nlz);
		}
	}

	// need these for GOURAUD calculation of colors:
	int GetVertexCount() const
	{
		return number_of_vertices;
	}

	vertex_type *GetVertexPtr(int vnum)
	{
		return &vertex[vnum];
	}
};

#endif
