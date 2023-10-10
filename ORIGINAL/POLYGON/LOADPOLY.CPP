// LOADPOLY.CPP
// file reading routines 
// file reading routines 
// updated 9/1/95 mracky@sirius.com
#include  <io.h>
#include <fcntl.h>
#include  <ctype.h>
#include  "poly.h"

class PolyFile {
	protected:
		int grip;
	public:
		PolyFile() { grip = -1; }
		~PolyFile() {Close();}
		int Open(char *filename);
		void Close();
		int getnumber();
		char nextchar();
};

static PolyFile _pf;

int object_type::load()
{
	 number_of_vertices=_pf.getnumber();
	 //world->obj[objnum].vertex=(vertex_type far *)farmalloc(world->obj[objnum].number_of_vertices*sizeof(vertex_type));
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
	 //polygon= (polygon_type far *)farmalloc(world->obj[objnum].number_of_polygons*sizeof(polygon_type));
	 polygon= new polygon_type[number_of_polygons];
	 if(polygon == 0)
		return(-1);
	 polygon_type *polyptr = polygon;
	 for (int polynum=0; polynum<number_of_polygons; polynum++, polyptr++) {
		polyptr->number_of_vertices=_pf.getnumber();
		polyptr->vertex=  new vertex_type *[number_of_vertices];
		if(polyptr->vertex == 0)
			return(-1);
//		  (vertex_type far **)farmalloc(world->obj[objnum].polygon[polynum].number_of_vertices
//			 *sizeof(vertex_type far *));
		for (int vertnum=0; vertnum< polyptr->number_of_vertices; vertnum++) {
		  polyptr->vertex[vertnum]= &vertex[_pf.getnumber()];
		}
		polyptr->color=_pf.getnumber();
	 }
	 convex=_pf.getnumber();
	 return(0);
}

int world_type::loadpoly(char *filename)
{
  if( _pf.Open(filename) ) // if a mistake in opening file, exit function
	return(-1);
	 // world members:
  number_of_objects = _pf.getnumber();
  int retval = 0; // return value
  //obj= (object_type far *)farmalloc(number_of_objects*sizeof(object_type));
  obj= new object_type[number_of_objects];
  if( ! obj )
	 retval = -1;
  else {
		for (int objnum=0; objnum< number_of_objects; objnum++) {
			if( obj[objnum].load() )
			{
				retval = -1 ;
				break;
			}
		}
  }
  return(retval);
}

int PolyFile::Open(char *filename)
{
	grip = open(filename,O_RDONLY|O_TEXT);
	return (grip == -1);
}

void PolyFile::Close()
{ 
  if(grip>-1)  close(grip);
  grip = -1;
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
