
// LOADPOLY.CPP - 1/10/93
//
// A set of functions for loading a world_type structure
// a database of polygon-fill objects.
//
// Copyright 1993 by Christopher Lampton and The Waite Group Press

#include  <stdio.h>
#include  <ctype.h>
#include  <alloc.h>
#include  "poly.h"
#include  "loadpoly.h"

FILE *f;
int num,objnum,vertnum,polynum;

int loadpoly(world_type *world,char *filename)

// Load polygon-fill objects into a data structure of
//  type WORLD_TYPE from disk file FILENAME

{
  f=fopen(filename,"rt");  // Open file

  // Initialize polygon count:

  int polycount = 0;

  // Get number of objects from file:

  world->number_of_objects=getnumber(f);

  // Allocate memory for objects:

  world->obj = new object_type[world->number_of_objects];

  // Load objects into OBJECT_TYPE array:

  for (objnum=0; objnum<world->number_of_objects;
        objnum++) {

    // Assign pointer CUROBJ to current object:

    object_type *curobj=&world->obj[objnum];

    // Get x,y and z coordinates of object's local origin:

		curobj->x=getnumber(f);
		curobj->y=getnumber(f);
		curobj->z=getnumber(f);

    // Get orientation of object:

		curobj->xangle=getnumber(f);
		curobj->yangle=getnumber(f);
		curobj->zangle=getnumber(f);

    // Get x,y and z scale factors for object:

		curobj->xscale=getnumber(f);
//		curobj->yscale=getnumber(f);
//		curobj->zscale=getnumber(f);
		curobj->yscale=curobj->xscale;
		curobj->zscale=curobj->xscale;

    // Get number of vertices in current object:

    curobj->number_of_vertices=getnumber(f);

    // Allocate memory for vertex array:

    curobj->vertex =
      new vertex_type[curobj->number_of_vertices];

    // Load vertices into VERTEX_TYPE array:

    for (vertnum=0; vertnum<curobj->number_of_vertices;
          vertnum++) {

      // Assign pointer CURVERT to current vertex:

      vertex_type *curvert=&curobj->vertex[vertnum];

      // Get local coordinates of vertex:

      curvert->lx=getnumber(f); // Get local x coordinate
      curvert->ly=getnumber(f); // Get local y coordinate
      curvert->lz=getnumber(f); // Get local z coordinate
      curvert->lt=1; // Initialize dummy coordinates...
      curvert->wt=1; // ...to 1
    }

    // Get number of polygons in object:

    curobj->number_of_polygons=getnumber(f);

    // Add to polygon count:

    polycount+=curobj->number_of_polygons;

    // Assign memory for polygon array:

    curobj->polygon =
      new polygon_type[curobj->number_of_polygons];

    // Load polygons into POLYGON_TYPE array:

    for (polynum=0; polynum<curobj->number_of_polygons;
          polynum++) {

      // Assign pointer CURPOLY to current polygon:

      polygon_type *curpoly=&curobj->polygon[polynum];

      // Get number of vertices in current polygon:

      curpoly->number_of_vertices=getnumber(f);

      // Calculate number of clipped vertices in current polygon:

	    curpoly->number_of_clipped_vertices =
        curpoly->number_of_vertices + 4;

      // Assign memory for vertex array:

      curpoly->vertex =
        new vertex_type*[curpoly->number_of_vertices];

      // Create array of pointers to vertices:

      for (vertnum=0; vertnum<curpoly->number_of_vertices;
            vertnum++) {

        // Allocate memory for pointer and point it at
        //  vertex in vertex array:

        curpoly->vertex[vertnum] =
          &curobj->vertex[getnumber(f)];
      }

      // Get color of current polygon:

      curpoly->color=getnumber(f);

      // Initialize sort flag to zero:

			curpoly->sortflag=0;
    }

    // Is backface removal needed?

    curobj->convex=getnumber(f);

    // Set update flag:

    curobj->update=1;
  }

  // Close up shop and return:

  fclose(f);  // Close file
  return(polycount);  // Job done!
}

int getnumber(FILE *f)

// Return next number in file f

{
  char ch;
  int sign=1;

  num=0;
  if ((ch=nextchar(f))=='-') {
     sign=-1;
     ch=nextchar(f);
  }
  while (isdigit(ch)) {
    num=num*10+ch-'0';
    ch=nextchar(f);
  }
  return(num*sign);
}

char nextchar(FILE *f)

// Return next character in file f
// Ignore spaces and comments

{
  char ch;

  while(!feof(f)) {
    while(isspace(ch=fgetc(f)));
    if (ch=='*')
       while((ch=fgetc(f))!='\n');
         else return(ch);
  }
  return(0);
}
