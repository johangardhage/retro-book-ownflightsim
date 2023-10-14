#ifndef POLYGON_H
#define POLYGON_H

// Variable structures to hold shape data:
struct vertex_type { // Structure for individual vertices
	long lx, ly, lz, lt;	 // Local coordinates of vertex
	long wx, wy, wz, wt;  // World coordinates of vertex
	long sx, sy, st;		 // Screen coordinates of vertex
};

// Global transformation arrays:

float matrix[4][4];         // Master transformation matrix
float smat[4][4];				    // Scaling matrix
float zmat[4][4];				    // Z rotation matrix
float xmat[4][4];				    // X rotation matrix
float ymat[4][4];				    // Y rotation matrix
float tmat[4][4];           // Translation matrix

void matmult(float result[4][4], float mat1[4][4], float mat2[4][4])
{
	// Multiply matrix MAT1 by matrix MAT2, returning the result in RESULT
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result[i][j] = 0;
			for (int k = 0; k < 4; k++) {
				result[i][j] += mat1[i][k] * mat2[k][j];
			}
		}
	}
}

void matcopy(float dest[4][4], float source[4][4])
{
	// Copy matrix SOURCE to matrix DEST
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			dest[i][j] = source[i][j];
		}
	}
}

// Transformation functions:
void inittrans()
{
	// Initialize master transformation matrix to the identity matrix
	matrix[0][0] = 1; matrix[0][1] = 0; matrix[0][2] = 0; matrix[0][3] = 0;
	matrix[1][0] = 0; matrix[1][1] = 1; matrix[1][2] = 0; matrix[1][3] = 0;
	matrix[2][0] = 0; matrix[2][1] = 0; matrix[2][2] = 1; matrix[2][3] = 0;
	matrix[3][0] = 0; matrix[3][1] = 0; matrix[3][2] = 0; matrix[3][3] = 1;
}

void scale(float sf)
{
	float mat[4][4];

	// Initialize scaling matrix:
	smat[0][0] = sf; smat[0][1] = 0;  smat[0][2] = 0;  smat[0][3] = 0;
	smat[1][0] = 0;  smat[1][1] = sf; smat[1][2] = 0;  smat[1][3] = 0;
	smat[2][0] = 0;  smat[2][1] = 0;  smat[2][2] = sf; smat[2][3] = 0;
	smat[3][0] = 0;  smat[3][1] = 0;  smat[3][2] = 0;  smat[3][3] = 1;

	// Concatenate with master matrix:
	matmult(mat, smat, matrix);
	matcopy(matrix, mat);
}

void rotate(float ax, float ay, float az)
{
	// Create three rotation matrices that will rotate an object
	// AX radians on the X axis, AY radians on the Y axis and
	// AZ radians on the Z axis

	float mat1[4][4];
	float mat2[4][4];

	// Initialize X rotation matrix:
	xmat[0][0] = 1;  xmat[0][1] = 0;        xmat[0][2] = 0;       xmat[0][3] = 0;
	xmat[1][0] = 0;  xmat[1][1] = cos(ax);  xmat[1][2] = sin(ax); xmat[1][3] = 0;
	xmat[2][0] = 0;  xmat[2][1] = -sin(ax); xmat[2][2] = cos(ax); xmat[2][3] = 0;
	xmat[3][0] = 0;  xmat[3][1] = 0;        xmat[3][2] = 0;       xmat[3][3] = 1;

	// Concatenate this matrix with master matrix:
	matmult(mat1, xmat, matrix);

	// Initialize Y rotation matrix:
	ymat[0][0] = cos(ay); ymat[0][1] = 0; ymat[0][2] = -sin(ay); ymat[0][3] = 0;
	ymat[1][0] = 0;       ymat[1][1] = 1; ymat[1][2] = 0;        ymat[1][3] = 0;
	ymat[2][0] = sin(ay); ymat[2][1] = 0; ymat[2][2] = cos(ay);  ymat[2][3] = 0;
	ymat[3][0] = 0;       ymat[3][1] = 0; ymat[3][2] = 0;        ymat[3][3] = 1;

	// Concatenate this matrix with master matrix:
	matmult(mat2, ymat, mat1);

	// Initialize Z rotation matrix:
	zmat[0][0] = cos(az);  zmat[0][1] = sin(az);  zmat[0][2] = 0;  zmat[0][3] = 0;
	zmat[1][0] = -sin(az); zmat[1][1] = cos(az);  zmat[1][2] = 0;  zmat[1][3] = 0;
	zmat[2][0] = 0;        zmat[2][1] = 0;        zmat[2][2] = 1;  zmat[2][3] = 0;
	zmat[3][0] = 0;        zmat[3][1] = 0;        zmat[3][2] = 0;  zmat[3][3] = 1;

	// Concatenate this matrix with master matrix:
	matmult(matrix, zmat, mat2);
}

void translate(int xt, int yt, int zt)
{
	// Create a translation matrix that will translate an object an
	// X distance of XT, a Y distance of YT, and a Z distance of ZT
	// from the screen origin

	float mat[4][4];

	tmat[0][0] = 1;  tmat[0][1] = 0;  tmat[0][2] = 0;  tmat[0][3] = 0;
	tmat[1][0] = 0;  tmat[1][1] = 1;  tmat[1][2] = 0;  tmat[1][3] = 0;
	tmat[2][0] = 0;  tmat[2][1] = 0;  tmat[2][2] = 1;  tmat[2][3] = 0;
	tmat[3][0] = xt; tmat[3][1] = yt; tmat[3][2] = zt; tmat[3][3] = 1;

	// Concatenate with master matrix:
	matmult(mat, matrix, tmat);
	matcopy(matrix, mat);
}

class polygon_type {
private:
	friend class object_type;  // access to vertex pointers
	int	number_of_vertices;	// Number of vertices in polygon
	int	color;              // Color of polygon
	int	zmax, zmin;					// Maximum and minimum z coordinates of polygon
	int xmax, xmin;
	int ymax, ymin;
	vertex_type **vertex;		// List of vertices

public:
	polygon_type() :vertex(0) { ; }
	~polygon_type()
	{
		if (vertex) {
			delete[] vertex;
		}
	}

	int	backface()
	{
		// 	 Returns 0 if POLYGON is visible, -1 if not.
		//   POLYGON must be part of a convex polyhedron

		vertex_type *v0, *v1, *v2;  // Pointers to three vertices

		// Point to vertices:
		v0 = vertex[0];
		v1 = vertex[1];
		v2 = vertex[2];
		int z = (v1->sx - v0->sx) * (v2->sy - v0->sy) - (v1->sy - v0->sy) * (v2->sx - v0->sx);
		return(z >= 0);
	}

	void DrawPoly(unsigned char *screen_buffer)
	{
		// Draw polygon in POLYGON_TYPE in SCREEN_BUFFER

		// Uninitialized variables:
		int ydiff1, ydiff2,         // Difference between starting x and ending x
			xdiff1, xdiff2,         // Difference between starting y and ending y
			start,                 // Starting offset of line between edges
			length,                // Distance from edge 1 to edge 2
			errorterm1, errorterm2, // Error terms for edges 1 & 2
			offset1, offset2,       // Offset of current pixel in edges 1 & 2
			count1, count2,         // Increment count for edges 1 & 2
			xunit1, xunit2;         // Unit to advance x offset for edges 1 & 2

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
		while (edgecount > 0) {    // Continue drawing until all edges drawn
			offset1 = 320 * ystart1 + xstart1;  // Offset of edge 1
			offset2 = 320 * ystart2 + xstart2;  // Offset of edge 2
			errorterm1 = 0;           // Initialize error terms
			errorterm2 = 0;           // for edges 1 & 2
			if ((ydiff1 = yend1 - ystart1) < 0) ydiff1 = -ydiff1; // Get absolute value of
			if ((ydiff2 = yend2 - ystart2) < 0) ydiff2 = -ydiff2; // x & y lengths of edges
			if ((xdiff1 = xend1 - xstart1) < 0) {               // Get value of length
				xunit1 = -1;                                  // Calculate X increment
				xdiff1 = -xdiff1;
			} else {
				xunit1 = 1;
			}
			if ((xdiff2 = xend2 - xstart2) < 0) {               // Get value of length
				xunit2 = -1;                                  // Calculate X increment
				xdiff2 = -xdiff2;
			} else {
				xunit2 = 1;
			}

			// Choose which of four routines to use:
			if (xdiff1 > ydiff1) {    // If X length of edge 1 is greater than y length
				if (xdiff2 > ydiff2) {  // If X length of edge 2 is greater than y length
					// Increment edge 1 on X and edge 2 on X:
					count1 = xdiff1;    // Count for x increment on edge 1
					count2 = xdiff2;    // Count for x increment on edge 2
					while (count1 && count2) {  // Continue drawing until one edge is done
						// Calculate edge 1:
						while ((errorterm1 < xdiff1) && (count1 > 0)) { // Finished w/edge 1?
							if (count1--) {     // Count down on edge 1
								offset1 += xunit1;  // Increment pixel offset
								xstart1 += xunit1;
							}
							errorterm1 += ydiff1; // Increment error term
							if (errorterm1 < xdiff1) {  // If not more than XDIFF
								screen_buffer[offset1] = color; // ...plot a pixel
							}
						}
						errorterm1 -= xdiff1; // If time to increment X, restore error term

						// Calculate edge 2:
						while ((errorterm2 < xdiff2) && (count2 > 0)) {  // Finished w/edge 2?
							if (count2--) {     // Count down on edge 2
								offset2 += xunit2;  // Increment pixel offset
								xstart2 += xunit2;
							}
							errorterm2 += ydiff2; // Increment error term
							if (errorterm2 < xdiff2) {  // If not more than XDIFF
								screen_buffer[offset2] = color;  // ...plot a pixel
							}
						}
						errorterm2 -= xdiff2; // If time to increment X, restore error term

						// Draw line from edge 1 to edge 2:
						length = offset2 - offset1; // Determine length of horizontal line
						if (length < 0) {         // If negative...
							length = -length;       // Make it positive
							start = offset2;        // And set START to edge 2
						} else {
							start = offset1;     // Else set START to edge 1
						}
						for (int i = start; i < start + length + 1; i++) {  // From edge to edge...
							screen_buffer[i] = color;         // ...draw the line
						}
						offset1 += 320;           // Advance edge 1 offset to next line
						ystart1++;
						offset2 += 320;           // Advance edge 2 offset to next line
						ystart2++;
					}
				} else {
					// Increment edge 1 on X and edge 2 on Y:
					count1 = xdiff1;    // Count for X increment on edge 1
					count2 = ydiff2;    // Count for Y increment on edge 2
					while (count1 && count2) {  // Continue drawing until one edge is done
						// Calculate edge 1:
						while ((errorterm1 < xdiff1) && (count1 > 0)) { // Finished w/edge 1?
							if (count1--) {     // Count down on edge 1
								offset1 += xunit1;  // Increment pixel offset
								xstart1 += xunit1;
							}
							errorterm1 += ydiff1; // Increment error term
							if (errorterm1 < xdiff1) {  // If not more than XDIFF
								screen_buffer[offset1] = color; // ...plot a pixel
							}
						}
						errorterm1 -= xdiff1; // If time to increment X, restore error term

						// Calculate edge 2:
						errorterm2 += xdiff2; // Increment error term
						if (errorterm2 >= ydiff2) { // If time to increment Y...
							errorterm2 -= ydiff2;        // ...restore error term
							offset2 += xunit2;           // ...and advance offset to next pixel
							xstart2 += xunit2;
						}
						--count2;

						// Draw line from edge 1 to edge 2:
						length = offset2 - offset1; // Determine length of horizontal line
						if (length < 0) {         // If negative...
							length = -length;       // ...make it positive
							start = offset2;        // And set START to edge 2
						} else {
							start = offset1;     // Else set START to edge 1
						}
						for (int i = start; i < start + length + 1; i++) {  // From edge to edge
							screen_buffer[i] = color;         // ...draw the linee
						}
						offset1 += 320;           // Advance edge 1 offset to next line
						ystart1++;
						offset2 += 320;           // Advance edge 2 offset to next line
						ystart2++;
					}
				}
			} else {
				if (xdiff2 > ydiff2) {
					// Increment edge 1 on Y and edge 2 on X:
					count1 = ydiff1;  // Count for Y increment on edge 1
					count2 = xdiff2;  // Count for X increment on edge 2
					while (count1 && count2) {  // Continue drawing until one edge is done
						// Calculate edge 1:
						errorterm1 += xdiff1; // Increment error term
						if (errorterm1 >= ydiff1) {  // If time to increment Y...
							errorterm1 -= ydiff1;         // ...restore error term
							offset1 += xunit1;            // ...and advance offset to next pixel
							xstart1 += xunit1;
						}
						--count1;

						// Calculate edge 2:
						while ((errorterm2 < xdiff2) && (count2 > 0)) { // Finished w/edge 1?
							if (count2--) {     // Count down on edge 2
								offset2 += xunit2;  // Increment pixel offset
								xstart2 += xunit2;
							}
							errorterm2 += ydiff2; // Increment error term
							if (errorterm2 < xdiff2) {  // If not more than XDIFF
								screen_buffer[offset2] = color; // ...plot a pixel
							}
						}
						errorterm2 -= xdiff2;  // If time to increment X, restore error term

						// Draw line from edge 1 to edge 2:
						length = offset2 - offset1; // Determine length of horizontal line
						if (length < 0) {    // If negative...
							length = -length;  // ...make it positive
							start = offset2;   // And set START to edge 2
						} else {
							start = offset1;  // Else set START to edge 1
						}
						for (int i = start; i < start + length + 1; i++) {  // From edge to edge...
							screen_buffer[i] = color;        // ...draw the line
						}
						offset1 += 320;         // Advance edge 1 offset to next line
						ystart1++;
						offset2 += 320;         // Advance edge 2 offset to next line
						ystart2++;
					}
				} else {
					// Increment edge 1 on Y and edge 2 on Y:
					count1 = ydiff1;  // Count for Y increment on edge 1
					count2 = ydiff2;  // Count for Y increment on edge 2
					while (count1 && count2) {  // Continue drawing until one edge is done
						// Calculate edge 1:
						errorterm1 += xdiff1;  // Increment error term
						if (errorterm1 >= ydiff1) {  // If time to increment Y
							errorterm1 -= ydiff1;         // ...restore error term
							offset1 += xunit1;            // ...and advance offset to next pixel
							xstart1 += xunit1;
						}
						--count1;

						// Calculate edge 2:
						errorterm2 += xdiff2; // Increment error term
						if (errorterm2 >= ydiff2) {  // If time to increment Y
							errorterm2 -= ydiff2;         // ...restore error term
							offset2 += xunit2;            // ...and advance offset to next pixel
							xstart2 += xunit2;
						}
						--count2;

						// Draw line from edge 1 to edge 2:
						length = offset2 - offset1;  // Determine length of horizontal line
						if (length < 0) {          // If negative...
							length = -length;        // ...make it positive
							start = offset2;         // And set START to edge 2
						} else {
							start = offset1;      // Else set START to edge 1
						}
						for (int i = start; i < start + length + 1; i++) {   // From edge to edge
							screen_buffer[i] = color;          // ...draw the linee
						}
						offset1 += 320;            // Advance edge 1 offset to next line
						ystart1++;
						offset2 += 320;            // Advance edge 2 offset to next line
						ystart2++;
					}
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
				xend1 = vertex[endvert1]->sx;  // Get x & y of new end vertex
				yend1 = vertex[endvert1]->sy;
			}
			if (!count2) {          // If edge 2 is complete...
				--edgecount;          // Decrement the edge count
				startvert2 = endvert2;  // Make ending vertex into start vertex
				endvert2++;           // And get new ending vertex
				if (endvert2 == (number_of_vertices)) {
					endvert2 = 0; // Check for wrap
				}
				xend2 = vertex[endvert2]->sx;  // Get x & y of new end vertex
				yend2 = vertex[endvert2]->sy;
			}
		}
	}
};

class object_type {
private:
	int		number_of_vertices;	// Number of vertices in object
	int		number_of_polygons;	// Number of polygons in object
	int		x, y, z;							// World coordinates of object's local origin
	polygon_type *polygon;		// List of polygons in object
	vertex_type *vertex;			// Array of vertices in object
	int		convex;							// Is it a convex polyhedron?

public:
	object_type() :polygon(0), vertex(0) { ; }
	~object_type()
	{
		if (polygon) {
			delete[] polygon;
		}
		if (vertex) {
			delete[] vertex;
		}
	}

	int load()
	{
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

		polygon = new polygon_type[number_of_polygons];
		if (polygon == 0) {
			return(-1);
		}

		// Add to global polygon count:
		polycount += number_of_polygons;

		polygon_type *polyptr = polygon;
		for (int polynum = 0; polynum < number_of_polygons; polynum++, polyptr++) {
			polyptr->number_of_vertices = _pf.getnumber();
			polyptr->vertex = new vertex_type * [number_of_vertices];
			if (polyptr->vertex == 0)
				return(-1);
			for (int vertnum = 0; vertnum < polyptr->number_of_vertices; vertnum++) {
				polyptr->vertex[vertnum] = &vertex[_pf.getnumber()];
			}
			polyptr->color = _pf.getnumber();
		}
		convex = _pf.getnumber();
		return(0);
	}

	void Transform()
	{
		// Multiply all vertices in OBJECT with master transformation matrix:
		vertex_type *vptr = &vertex[0];
		for (int v = 0; v < number_of_vertices; v++, vptr++) {
			vptr->wx = vptr->lx * matrix[0][0] + vptr->ly * matrix[1][0] + vptr->lz * matrix[2][0] + matrix[3][0];
			vptr->wy = vptr->lx * matrix[0][1] + vptr->ly * matrix[1][1] + vptr->lz * matrix[2][1] + matrix[3][1];
			vptr->wz = vptr->lx * matrix[0][2] + vptr->ly * matrix[1][2] + vptr->lz * matrix[2][2] + matrix[3][2];
		}
	}

	void Project(int distance)
	{
		// Project object onto screen with perspective projection

		vertex_type *vptr = vertex;  // Point to current vertex
		for (int v = 0; v < number_of_vertices; v++, vptr++) { // Loop through vertices
			vptr->sx = distance * vptr->wx / (distance - vptr->wz) + XORIGIN;  // ...divide world x&y coords
			vptr->sy = distance * vptr->wy / (distance - vptr->wz) + YORIGIN;  // ... by z coordinates
		}
	}

	void Draw(unsigned char *screen)
	{
		// Draw  the OBJECT

		// Loop through all polygons in object:
		polygon_type *polyptr = polygon;
		for (int p = 0; p < number_of_polygons; p++, polyptr++) {
			// Draw current polygon:
			if (convex) {
				if (!polyptr->backface()) {
					polyptr->DrawPoly(screen);
				}
			} else {
				polyptr->DrawPoly(screen);
			}
		}
	}
};

class world_type {
private:
	int number_of_objects;
	object_type *obj;

public:
	world_type() :obj(0) { ; }
	~world_type()
	{
		if (obj) {
			delete[] obj;
		}
	}

	int loadpoly(const char *filename)
	{
		if (_pf.Open(filename)) { // if a mistake in opening file, exit function
			return(-1);
		}

		// world members:
		number_of_objects = _pf.getnumber();
		int retval = 0; // return value

		obj = new object_type[number_of_objects];
		if (!obj) {
			retval = -1;
		} else {
			for (int objnum = 0; objnum < number_of_objects; objnum++) {
				if (obj[objnum].load()) {
					retval = -1;
					break;
				}
			}
		}
		return(retval);
	}

	void Draw(unsigned char *screen)
	{
		for (int i = 0; i < number_of_objects; i++) {
			obj[i].Transform();               // Transform OBJECT using MATRIX
			obj[i].Project(400);             // Perform perspective projection
			obj[i].Draw(screen);  // Draw transformed object
		}
	}
};

#endif
