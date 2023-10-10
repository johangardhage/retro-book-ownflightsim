
// POLY.CPP - 1/20/93
//
//   A set of functions to create translation matrices for various
// polygon transformation functions and to transform a polygon-fill
// object using a matrix concatenated from those matrices.
//
// Copyright 1993 by Christopher Lampton and The Waite Group Press

#include  <stdio.h>
#include  <math.h>
#include  "poly.h"
#include  "drawpoly.h"
#include  "fix.h"

#define   ONE    (1L<<SHIFT)

// Global transformation arrays:

long matrix[4][4];          // Master transformation matrix
long smat[4][4];				    // Scaling matrix
long rmat[4][4];					  // Perspective matrix
long zmat[4][4];				    // Z rotation matrix
long xmat[4][4];				    // X rotation matrix
long ymat[4][4];				    // Y rotation matrix
long tmat[4][4];            // Translation matrix

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

void scale(float xs,float ys,float zs)
{
	long mat[4][4];

	// Initialize scaling matrix:

	smat[0][0] = (long)(zs*SHIFT_MULT); smat[0][1] = 0; smat[0][2] = 0; smat[0][3] = 0;
	smat[1][0] = 0; smat[1][1] = (long)(ys*SHIFT_MULT); smat[1][2] = 0; smat[1][3] = 0;
	smat[2][0] = 0; smat[2][1] = 0; smat[2][2] = (long)(xs*SHIFT_MULT); smat[2][3] = 0;
	smat[3][0] = 0; smat[3][1] = 0; smat[3][2] = 0;  smat[3][3] = ONE;

	// Concatenate with master matrix:

	matmult(mat,smat,matrix);
	matcopy(matrix,mat);
}

void reflect(int xr,int yr,int zr)
{
	long mat[4][4];

	rmat[0][0]=xr>>SHIFT; rmat[0][1]=0; rmat[0][2]=0; rmat[0][3]=0;
	rmat[1][0]=0; rmat[1][1]=yr>>SHIFT; rmat[1][2]=0; rmat[1][3]=0;
	rmat[2][0]=0; rmat[2][1]=0; rmat[2][2]=zr>>SHIFT; rmat[2][3]=0;
	rmat[3][0]=0; rmat[3][1]=0; rmat[3][2]=0; 				rmat[3][3]=ONE;

	matmult(mat,matrix,rmat);
	matcopy(matrix,mat);
}

void rotate(int ax,int ay,int az)
{

	// Create three rotation matrices that will rotate an object
	// AX radians on the X axis, AY radians on the Y axis and
	// AZ radians on the Z axis

	long mat1[4][4];
	long mat2[4][4];

	// Initialize Y rotation matrix:

	ymat[0][0]=COS(ay); ymat[0][1]=0;   ymat[0][2]=-SIN(ay); ymat[0][3]=0;
	ymat[1][0]=0;       ymat[1][1]=ONE; ymat[1][2]=0;        ymat[1][3]=0;
	ymat[2][0]=SIN(ay); ymat[2][1]=0;   ymat[2][2]=COS(ay);  ymat[2][3]=0;
	ymat[3][0]=0;       ymat[3][1]=0;   ymat[3][2]=0;        ymat[3][3]=ONE;

	// Concatenate this matrix with master matrix:


	// Initialize X rotation matrix:

	xmat[0][0]=ONE;  xmat[0][1]=0;        xmat[0][2]=0;       xmat[0][3]=0;
	xmat[1][0]=0;    xmat[1][1]=COS(ax);  xmat[1][2]=SIN(ax); xmat[1][3]=0;
	xmat[2][0]=0;    xmat[2][1]=-SIN(ax); xmat[2][2]=COS(ax); xmat[2][3]=0;
	xmat[3][0]=0;    xmat[3][1]=0;        xmat[3][2]=0;       xmat[3][3]=ONE;

	// Concatenate this matrix with master matrix:


	// Initialize Z rotation matrix:

	zmat[0][0]=COS(az);  zmat[0][1]=SIN(az);  zmat[0][2]=0;    zmat[0][3]=0;
	zmat[1][0]=-SIN(az); zmat[1][1]=COS(az);  zmat[1][2]=0;    zmat[1][3]=0;
	zmat[2][0]=0;        zmat[2][1]=0;        zmat[2][2]=ONE;  zmat[2][3]=0;
	zmat[3][0]=0;        zmat[3][1]=0;        zmat[3][2]=0;    zmat[3][3]=ONE;

	// Concatenate this matrix with master matrix:

	matmult(mat1,matrix,ymat);
	matmult(mat2,mat1,xmat);
	matmult(matrix,mat2,zmat);
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
	tmat[3][0]=(long)xt<<SHIFT; tmat[3][1]=(long)yt<<SHIFT; tmat[3][2]=(long)zt*SHIFT_MULT; tmat[3][3]=ONE;

	// Concatenate with master matrix:

	matmult(mat,matrix,tmat);
	matcopy(matrix,mat);
}

void transform(object_type *object)
{
  // Multiply all vertices in OBJECT with master transformation matrix:

  for (int v=0; v<(*object).number_of_vertices; v++) {
    vertex_type *vptr=&object->vertex[v];
    vptr->wx=((long)vptr->lx*matrix[0][0]+(long)vptr->ly*matrix[1][0]
	 +(long)vptr->lz*matrix[2][0]+matrix[3][0])>>SHIFT;
    vptr->wy=((long)vptr->lx*matrix[0][1]+(long)vptr->ly*matrix[1][1]
	 +(long)vptr->lz*matrix[2][1]+matrix[3][1])>>SHIFT;
    vptr->wz=((long)vptr->lx*matrix[0][2]+(long)vptr->ly*matrix[1][2]
	 +(long)vptr->lz*matrix[2][2]+matrix[3][2])>>SHIFT;
  }
}

void atransform(object_type *object)

// Multiply all vertices in OBJECT with master
//  transformation matrix:

{

  // Step through all vertices of OBJECT:

  for (int v=0; v<(*object).number_of_vertices; v++) {

  // Create pointer to vertex:

  vertex_type *vptr=&(*object).vertex[v];

  // Calculate new aligned x coordinate:

  vptr->ax=((long)vptr->wx*matrix[0][0]
    +(long)vptr->wy*matrix[1][0]
    +(long)vptr->wz*matrix[2][0]
    +matrix[3][0])>>SHIFT;

  // Calculate new aligned y coordinate:

  vptr->ay=((long)vptr->wx*matrix[0][1]
    +(long)vptr->wy*matrix[1][1]
    +(long)vptr->wz*matrix[2][1]
    +matrix[3][1])>>SHIFT;

  // Calculate new aligned z coordinate:

  vptr->az=((long)vptr->wx*matrix[0][2]
    +(long)vptr->wy*matrix[1][2]
    +(long)vptr->wz*matrix[2][2]
    +matrix[3][2])>>SHIFT;
  }
}

void matmult(long result[4][4],long mat1[4][4],long mat2[4][4])
{

  // Multiply matrix MAT1 by matrix MAT2, returning the result in RESULT

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
