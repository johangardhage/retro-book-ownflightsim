// POLY.CPP
//   A set of functions to create translation matrices for various
// polygon transformation functions, to transform a polygon-fill
// object using a matrix concatenated from those matrices, and to
// draw a polygon-fill object on the mode 13h display
// 9/6/95 - mracky : updated to C++
#include  <stdio.h>
#include  <math.h>
#include  "poly.h"
#include  "fix.h"

#define   ONE    (SHIFT_MULT)

// Global transformation arrays:

long matrix[4][4];         // Master transformation matrix
long smat[4][4];				    // Scaling matrix
long zmat[4][4];				    // Z rotation matrix
long xmat[4][4];				    // X rotation matrix
long ymat[4][4];				    // Y rotation matrix
long tmat[4][4];           // Translation matrix

// Function prototype:

// Multiply two 4x4 matrices, result in RESULT:

void matmult(long result[4][4],long mat1[4][4],long mat2[4][4]);

// Copy one 4x4 matrix into another 4x4 matrix:

void matcopy(long dest[4][4],long source[4][4]);

// Transformation functions:

void inittrans()
{

// Initialize master transformation matrix to the identity matrix

  matrix[0][0]=ONE; matrix[0][1]=0;   matrix[0][2]=0;   matrix[0][3]=0;
  matrix[1][0]=0;   matrix[1][1]=ONE; matrix[1][2]=0;   matrix[1][3]=0;
  matrix[2][0]=0;   matrix[2][1]=0;   matrix[2][2]=ONE; matrix[2][3]=0;
  matrix[3][0]=0;   matrix[3][1]=0;   matrix[3][2]=0;   matrix[3][3]=ONE;
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
  matmult(mat,smat,matrix);
  matcopy(matrix,mat);
}

void rotate(int ax,int ay,int az)
{

  // Create three rotation matrices that will rotate an object
  // AX radians on the X axis, AY radians on the Y axis and
  // AZ radians on the Z axis

  long mat1[4][4];
  long mat2[4][4];

	// Initialize X rotation matrix:

	xmat[0][0]=ONE;  xmat[0][1]=0;        xmat[0][2]=0;       xmat[0][3]=0;
  xmat[1][0]=0;    xmat[1][1]=COS(ax);  xmat[1][2]=SIN(ax); xmat[1][3]=0;
	xmat[2][0]=0;    xmat[2][1]=-SIN(ax); xmat[2][2]=COS(ax); xmat[2][3]=0;
	xmat[3][0]=0;    xmat[3][1]=0;        xmat[3][2]=0;       xmat[3][3]=ONE;

  // Concatenate this matrix with master matrix:

  matmult(mat1,xmat,matrix);

  // Initialize Y rotation matrix:

	ymat[0][0]=COS(ay); ymat[0][1]=0;   ymat[0][2]=-SIN(ay); ymat[0][3]=0;
  ymat[1][0]=0;       ymat[1][1]=ONE; ymat[1][2]=0;        ymat[1][3]=0;
	ymat[2][0]=SIN(ay); ymat[2][1]=0;   ymat[2][2]=COS(ay);  ymat[2][3]=0;
	ymat[3][0]=0;       ymat[3][1]=0;   ymat[3][2]=0;        ymat[3][3]=ONE;

  // Concatenate this matrix with master matrix:

  matmult(mat2,ymat,mat1);

	// Initialize Z rotation matrix:

	zmat[0][0]=COS(az);  zmat[0][1]=SIN(az);  zmat[0][2]=0;    zmat[0][3]=0;
	zmat[1][0]=-SIN(az); zmat[1][1]=COS(az);  zmat[1][2]=0;    zmat[1][3]=0;
	zmat[2][0]=0;        zmat[2][1]=0;        zmat[2][2]=ONE;  zmat[2][3]=0;
	zmat[3][0]=0;        zmat[3][1]=0;        zmat[3][2]=0;    zmat[3][3]=ONE;

  // Concatenate this matrix with master matrix:

  matmult(matrix,zmat,mat2);
}

void translate(int xt,int yt,int zt)
{

// Create a translation matrix that will translate an object an
// X distance of XT, a Y distance of YT, and a Z distance of ZT
// from the screen origin

  long mat[4][4];

  tmat[0][0]=ONE;      tmat[0][1]=0;      tmat[0][2]=0;      tmat[0][3]=0;
  tmat[1][0]=0;        tmat[1][1]=ONE;    tmat[1][2]=0;      tmat[1][3]=0;
  tmat[2][0]=0;        tmat[2][1]=0;      tmat[2][2]=ONE;    tmat[2][3]=0;
  tmat[3][0]=(long)xt<<SHIFT; tmat[3][1]=(long)yt<<SHIFT; tmat[3][2]=(long)zt<<SHIFT; tmat[3][3]=ONE;

  // Concatenate with master matrix:

  matmult(mat,matrix,tmat);
  matcopy(matrix,mat);
}

void matmult(long result[4][4],long mat1[4][4],
              long mat2[4][4])
{

// Multiply matrix MAT1 by matrix MAT2,
//  returning the result in RESULT

  for (int i=0; i<4; i++)
    for (int j=0; j<4; j++) {
      result[i][j]=((mat1[i][0]*mat2[0][j])
        +(mat1[i][1]*mat2[1][j])
        +(mat1[i][2]*mat2[2][j])
        +(mat1[i][3]*mat2[3][j]))>>SHIFT;
    }
}

void matcopy(long dest[4][4],long source[4][4])
{

// Copy matrix SOURCE to matrix DEST

  for (int i=0; i<4; i++)
    for (int j=0; j<4; j++)
      dest[i][j]=source[i][j];
}


// ***********************************************
// WORLD_TYPE
world_type::~world_type()
{
	if(obj)
		delete [] obj;
}

void world_type::Draw(int objnum, unsigned char far *screen,int xorigin, int yorigin)
{
	if( objnum < number_of_objects) {
	 object_type *optr = &obj[objnum];
	 optr->Transform();       // Transform OBJECT using MATRIX
	 optr->Project(400, xorigin, yorigin); // Perform perspective projection
	 optr->Draw(screen);  // Draw transformed object
	 }
}
void world_type::Draw(unsigned char far *screen,int xorigin, int yorigin)
{
	for(int i=0; i < number_of_objects; i++) {
		Draw(i, screen,xorigin,yorigin);
	 }
}

// ***********************************************
// OBJECT_TYPE

object_type::~object_type()
{
	if(polygon)
		delete [] polygon;
	if(vertex)
		delete [] vertex;
}

void object_type ::Transform()
{
	// Multiply all vertices in OBJECT with master transformation matrix:
    vertex_type *vptr=vertex;
	for (int v=0; v<number_of_vertices; v++,vptr++) {
		vptr->wx=((long)vptr->lx*matrix[0][0]+(long)vptr->ly*matrix[1][0]
       +(long)vptr->lz*matrix[2][0]+matrix[3][0])>>SHIFT;
		vptr->wy=((long)vptr->lx*matrix[0][1]+(long)vptr->ly*matrix[1][1]
       +(long)vptr->lz*matrix[2][1]+matrix[3][1])>>SHIFT;
		vptr->wz=((long)vptr->lx*matrix[0][2]+(long)vptr->ly*matrix[1][2]
       +(long)vptr->lz*matrix[2][2]+matrix[3][2])>>SHIFT;
	}
}

void object_type::Project(int distance,int xorigin,int yorigin)
{

// Project object onto screen with perspective projection

  vertex_type *vptr= vertex;  // Point to current vertex
  for (int v=0; v<number_of_vertices; v++,vptr++) { // Loop through vertices
		vptr->sx=distance*vptr->wx/(distance-vptr->wz)+xorigin;  // ...divide world x&y coords
		vptr->sy=distance*vptr->wy/(distance-vptr->wz)+yorigin;  // ... by z coordinates
  }
}

void object_type::Draw(unsigned char far *screen)
// Draw  the OBJECT
{
	// Loop through all polygons in object:
	polygon_type * polyptr = polygon;
	for (int p=0; p<number_of_polygons; p++, polyptr++) 	{
		// Draw current polygon:
	 if (convex) {
		if(!polyptr->backface()) {
		  polyptr->DrawPoly(screen);
		}
	 }
	 else polyptr->DrawPoly(screen);
	}
}

// ***********************************************
// POLYGON_TYPE
polygon_type::~polygon_type()
{
	if(vertex)
		delete [] vertex;
}

int	polygon_type::backface()
{

// 	 Returns 0 if POLYGON is visible, -1 if not.
//   POLYGON must be part of a convex polyhedron

    vertex_type *v0,*v1,*v2;  // Pointers to three vertices
    // Point to vertices:
	 v0=vertex[0];
	 v1=vertex[1];
    v2=vertex[2];
    int z=(v1->sx-v0->sx)*(v2->sy-v0->sy)-(v1->sy-v0->sy)*(v2->sx-v0->sx);
    return(z>=0);
}
