// LOADP_FR.CPP - 1/10/93
//
// A set of functions for loading a World class    
// a database of polygon-fill objects.
//
// file reading routines 
// Copyright 1993 by Christopher Lampton and The Waite Group Press
// 9/1/95 mracky@sirius.com  updated to C++; classes created
// appended 9/20/95 mracky   integrated Dale Sather's lighting stuff
// appended 9/27/95 mracky   changed for fractal demo

#include  <io.h>
#include <fcntl.h>
#include <math.h>
#include  <ctype.h> // isdigit(), isspace()
#include  "world_fr.h" // WORLD includes polygon and lighting

class PolyFile {
	protected:
		int grip;
	public:
		PolyFile() { grip = -1; }
		int Open(const char *filename);
		~PolyFile()
			{ close(grip); }
		int getnumber();
		char nextchar();
};

static PolyFile _pf;
static int polycount;

int World::LoadPoly(const char *filename)
{
  if( _pf.Open(filename) ) // if a mistake in opening file, exit function
	return(-1);

  LoadSource();
  palette.Load();

  // Initialize polygon count:
  polycount = 0;

	 // world members:
  number_of_objects = _pf.getnumber() + 1;
	 // add one for blank fractal object

  obj= new Object[number_of_objects];
  if( ! obj )
	 return(-1);

  int lastobj = number_of_objects -1;
  for (int objnum=0; objnum< lastobj; objnum++) {
	 if( obj[objnum].Load() )
		return( -1 );
  }
	 // init the blank object
  obj[lastobj].Init(1,1);
  	// set it to invisible :
  obj[lastobj].Cloak();

  return(polycount);
}

int Object::Load()
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

	 number_of_vertices=_pf.getnumber();
	 vertex= new vertex_type[number_of_vertices];
	 if( vertex == 0)
		return(-1);
	 for (int vertnum=0; vertnum<number_of_vertices; vertnum++) {
		vertex[vertnum].lx=_pf.getnumber();
		vertex[vertnum].ly=_pf.getnumber();
		vertex[vertnum].lz=_pf.getnumber();
		vertex[vertnum].lt=1;
		vertex[vertnum].wt=1;
	 }
	 number_of_polygons=_pf.getnumber();
	 polygon= new Polygon[number_of_polygons];
	 if(polygon == 0)
		return(-1);

     // Add to global polygon count:
	 polycount+= number_of_polygons;

	 Polygon *polyptr = polygon;
	 for (int polynum=0; polynum<number_of_polygons; polynum++, polyptr++) {
		polyptr->number_of_vertices=_pf.getnumber();
		polyptr->vertex=  new vertex_type *[number_of_vertices];
		if(polyptr->vertex == 0)
			return(-1);
		for (int vertnum=0; vertnum< polyptr->number_of_vertices; vertnum++) {
		  polyptr->vertex[vertnum]= &vertex[_pf.getnumber()];
		}
		polyptr->original_color=_pf.getnumber();
	 }
	 flags = (UPDATE|VISIBLE);
	 if( (_pf.getnumber())!= 0)
	 	flags |= CONVEX;
	 return(0);
}

int World::LoadSource()
{
  source.x=_pf.getnumber();
  source.y=_pf.getnumber();
  source.z=_pf.getnumber();
  source.ambient=_pf.getnumber();
  source.length=(long)sqrt((float)source.x*source.x+(float)source.y*source.y+(float)source.z*source.z);
  return 0;
}  

int Palette::Load()
{  
  number_of_unshaded_colors=_pf.getnumber();
  unshaded_color=(rgb_type *)new rgb_type[number_of_unshaded_colors];
  if(! unshaded_color)
	return -1;
  for (int colornum=0; colornum<number_of_unshaded_colors; colornum++) {
    unshaded_color[colornum].red  =_pf.getnumber();
    unshaded_color[colornum].green=_pf.getnumber();
    unshaded_color[colornum].blue =_pf.getnumber();
  }
  number_of_shaded_colors=_pf.getnumber();
  shaded_color=(rgb_type *) new rgb_type[number_of_shaded_colors];
  if(!shaded_color)
  	return -1;
  for (colornum=0; colornum<number_of_shaded_colors; colornum++) {
    shaded_color[colornum].red  =_pf.getnumber();
    shaded_color[colornum].green=_pf.getnumber();
    shaded_color[colornum].blue =_pf.getnumber();
  }
  return 0;
}  


// ******************************************************8
// POLYFILE
//

int PolyFile::Open(const char *filename)
{
	grip = open(filename,O_RDONLY|O_TEXT);
	return (grip == -1);
}

int PolyFile::getnumber()
{
  char ch;
  int sign=1;

  int num=0;
  if ((ch=nextchar())=='-') {
	  sign=-1;
	  ch=nextchar();
  }
  while (isdigit(ch)) {
	 num=num*10+ch-'0';
	 ch=nextchar();
  }
  return(num*sign);
}


char PolyFile::nextchar()
{
  char ch;

  while(!eof(grip)) 
  {
		if( read(grip,&ch,1) != 1)
		 	break;
		if(	isspace(ch) )
			continue;
		
    	if (ch=='*')
		{
			while(1)
			{
				if( read(grip,&ch,1) != 1)
					break;
				if(ch=='\n')
					break;
			}
		}
		else
			return (ch);
  }
  return(0);
}
