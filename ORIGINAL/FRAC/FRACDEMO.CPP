// FRACDEMO.CPP
// copyright (c) 1995 Waite Group Press
//
// 9/20/95 mracky@sirius.com  update to C++; changes to Polygon class

#include	<dos.h>
#include	<conio.h>
#include <iostream.h>
#include  <stdlib.h>
#include "world_fr.h"
#include "screen.h"
#include "pcx.h"
#include "fracdemo.h"

const XORIGIN=80;
const YORIGIN=80;
const WIND_WIDTH=7*21;
const WIND_HEIGHT=7*21;

// local function prototypes 
void InitRand();
void fractalize(Polygon *poly,int rlevel);
long lpow(int x, int y);

World world;
Pcx bgobject;

static Object *fracObject;
static int vertnum;
static int polynum;
static int polyColor;

void main(int argc,char* argv[])
{
	int xangle=128,yangle=0,zangle=0; // X,Y&Z angles for rotation
									//  object
	int xrot=0,yrot=2,zrot=0;       // X,Y&Z rotations increments
	unsigned char *screen_buffer;     // Offscreen drawing

    // Load background image:

	if (bgobject.load("fracscrn.pcx")) {
		cerr << "Cannot load PCX file."<< endl;
		exit(-1);
	}
	if( world.LoadPoly("MOLEHILL.WLD")< 1 )  {  // Load object description(s)
		cerr << "Failure loading molehill.WLD file into world." << endl;
		exit(-1);
	}
	// after loadpoly is completed, the last polygon will be 
	// for making the fractal 

	screen_buffer=new unsigned char[64000L]; // Create offscreen buffer
	int oldmode=*(int *)MK_FP(0x40,0x49);			// Save previous video mode
	setgmode(0x13);					        // Set mode 13h

	unsigned char *ptr = bgobject.Image();
	for(long i=0; i<64000; i++)             // Put background
	 screen_buffer[i]=*ptr++;           	 //  in buffer

	world.AdjustLighting();                   // adjust the palette by the source

	// initalize the pyramid to somewhat random edges
	InitRand();
		// object to draw
	int curobj =0;

	int scalefactor=1;
	int zdistance=600;
	int key = 0;

		// the last object number is used 
		// for identifying the fractal object:
	int lastobj = world.GetObjectCount()-1;
	fracObject = world.GetObjectPtr(lastobj);

		// flag for showing the fractal
	char showFractal= 0;

	 // paint the background to the video screen:
	putwindow(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,
		FP_OFF(screen_buffer), FP_SEG(screen_buffer));

		// Loop until escape key is pressed
	while (key != 27) {
		//cls(screen_buffer);
		clrwin(10,8,WIND_WIDTH,WIND_HEIGHT,screen_buffer);

		inittrans();                    // Initialize translations
		scale(scalefactor);             // Create scaling matrix
		rotate(xangle,yangle,zangle);   // Create rotation matrices

		// Increment rotation angles
		xangle+=xrot;
		yangle+=yrot;
		zangle+=zrot;
	 	// Check for 256 degree wrap around:

		if (xangle>255) xangle=0;
		if (xangle<0) xangle=255;
		if (yangle>255) yangle=0;
		if (yangle<0) yangle=255;
		if (zangle>255) zangle=0;
		if (zangle<0) zangle=255;

		translate(0,0,zdistance);          // Create translation matrix
	 // Call the Draw world object
		world.Draw((showFractal?fracObject:world.GetObjectPtr(curobj)),screen_buffer,XORIGIN,YORIGIN);
	 // Put the viewport out to the video display:
		putwindow(10,8,WIND_WIDTH,WIND_HEIGHT,FP_OFF(screen_buffer), FP_SEG(screen_buffer));
	 // Watch for user input:

		if (kbhit()) {  // If input received....
			key=getch();  // Read the key code
			switch(key) {
				case '\r':
					// ENTER: Fractalize the object
					if(showFractal)	{
						showFractal = 0;
						break;
					}
					else {
						// if not showfractal

						// make a fractal version of current object:
						Object * objptr =world.GetObjectPtr(curobj);

						// estimate the number of polygons needed
						// (based on fractalized() triangles)
						int polyNeed = objptr->GetPolygonCount()* lpow(4, MAXLEVEL);

						// init the fractal object to allocate space
						// number of vertices is times 3 / 2 because
						// every polygon has three edges while every
						// edge belongs to two polygons

						fracObject->Init( (polyNeed*3)/2,polyNeed);

						// set the global counters for the recursive func.
						vertnum = 0;
						polynum = 0;
						for( int ii=0; ii < objptr->GetPolygonCount(); ii++)	{
							Polygon *pptr = objptr->GetPolygonPtr(ii);

							// set the global color for fractal assignment
							polyColor = pptr->GetOriginalColor();

							fractalize(pptr,1);
						}
						objptr->Cloak();
						fracObject->Uncloak();
						// prepare the fractal object lighting:
						world.SetLighting(fracObject);
						// show the created fractal
						showFractal = 1;
						curobj++;
						if (curobj>=lastobj)
							curobj=0;
					}
					break;

				case '7':

		  // "7": Speed up x rotation

					xrot++;
					break;

				case '4':

		  // "4": Stop x rotation

					xrot=0;
					break;

				case '1':

		  // "1": Slow down x rotation

					--xrot;
					break;

				case '8':

		  // "8": Speed up y rotation

					yrot++;
					break;

				case '5':

		  // "5": Stop y rotation

					yrot=0;
					break;

				case '2':

		  // "2": Slow down y rotation

					--yrot;
					break;

				case '9':

		  // "9": Speed up z rotation

					zrot++;
					break;

				case '6':

		  // "6": Stop z rotation

					zrot=0;
					break;

				case '3':

		  // "3": Slow down z rotation

					--zrot;
					break;

				case '0':

		  // "0": Shut off all rotation

					zrot = xrot = yrot = 0;
					zangle = xangle = yangle = 0;
					break;

				case '+':

          // "+": Increase distance

					zdistance+=30;
					break;

				case '-':

          // "-": Decrease distance

					if (zdistance>530) zdistance-=30;
					break;

				case '/':

          // "/": Decrease ambient light
				{
					int amb =world.GetAmbient();
					world.SetAmbient(amb-10);
					break;
				}
				case '*':

          //  Increase ambient light
				{
					int amb =world.GetAmbient();
					world.SetAmbient(amb+10);
					break;
				}
			}
		}
  }
	setgmode(oldmode);                       // Reset previous video mode & end
	if( screen_buffer)
		delete [] screen_buffer;
}

// return power of x
long lpow(int x, int y)
{
	long result = 1L;
	while(y--)
		result *= x;
	return result;
}

// inline macro to save some writing
// Random Variation:
//
inline long RandomVari()
{ return (random(VARIATION)-VARIATION/2); }


void fractalize(Polygon *poly,int rlevel)
{

  vertex_type *vptr  = poly->GetVertexPtr(0);
  vertex_type *vptr1 = poly->GetVertexPtr(1);
  vertex_type *vptr2 = poly->GetVertexPtr(2);

  long x0=vptr->lx;
  long y0=vptr->ly;
  long z0=vptr->lz;
  long x1=vptr1->lx;
  long y1=vptr1->ly;
  long z1=vptr1->lz;
  long x2=vptr2->lx;
  long y2=vptr2->ly;
  long z2=vptr2->lz;

  unsigned int seed=x0*100+y0*10+z0;
  srand(seed);

  long r=RandomVari()/rlevel;
  x0+=r;
  r=RandomVari()/rlevel;
  y0+=r;
  r=RandomVari()/rlevel;
  z0+=r;
  seed=x1*100+y1*10+z1;
  srand(seed);
  r=RandomVari()/rlevel;
  x1+=r;
  r=RandomVari()/rlevel;
  y1+=r;
  r=RandomVari()/rlevel;
  z1+=r;
  seed=x2*100+y2*10+z2;
  srand(seed);
  r=RandomVari()/rlevel;
  x2+=r;
  r=RandomVari()/rlevel;
  y2+=r;
  r=RandomVari()/rlevel;
  z2+=r;


	 // new vertices storage:
  vertex_type  newvert[6];

  // assign the vertex pointer vptr to either
  //   local storage newvert,  or
  // if this is the depth of the recursion, 
  //   to the fractal object's vertex array
  if (rlevel<MAXLEVEL)
	vptr = newvert;
  else
	vptr = fracObject->GetVertexPtr(vertnum);

  vptr[0].lx=x0;
  vptr[0].ly=y0;
  vptr[0].lz=z0;
  vptr[1].lx=x0+(x1-x0)/2;
  vptr[1].ly=y0+(y1-y0)/2;
  vptr[1].lz=z0+(z1-z0)/2;
  vptr[2].lx=x1;
  vptr[2].ly=y1;
  vptr[2].lz=z1;
  vptr[3].lx=x1+(x2-x1)/2;
  vptr[3].ly=y1+(y2-y1)/2;
  vptr[3].lz=z1+(z2-z1)/2;
  vptr[4].lx=x2;
  vptr[4].ly=y2;
  vptr[4].lz=z2;
  vptr[5].lx=x2+(x0-x2)/2;
  vptr[5].ly=y2+(y0-y2)/2;
  vptr[5].lz=z2+(z0-z2)/2;

  if (rlevel<MAXLEVEL) {

	 // create fracPolygon with 3 vertice
	 Polygon newpoly;
	 if( newpoly.MakeVertexArray(3) )
		return;
	 newpoly.SetVertexPtr(0,&newvert[0]);
	 newpoly.SetVertexPtr(1,&newvert[1]);
	 newpoly.SetVertexPtr(2,&newvert[5]);
	 fractalize(&newpoly,rlevel+1);
	 newpoly.SetVertexPtr(0,&newvert[1]);
	 newpoly.SetVertexPtr(1,&newvert[2]);
	 newpoly.SetVertexPtr(2,&newvert[3]);
	 fractalize((Polygon *)&newpoly,rlevel+1);
	 newpoly.SetVertexPtr(0,&newvert[5]);
	 newpoly.SetVertexPtr(1,&newvert[3]);
	 newpoly.SetVertexPtr(2,&newvert[4]);
	 fractalize((Polygon *)&newpoly,rlevel+1);
	 newpoly.SetVertexPtr(0,&newvert[5]);
	 newpoly.SetVertexPtr(1,&newvert[1]);
	 newpoly.SetVertexPtr(2,&newvert[3]);
	 fractalize((Polygon *)&newpoly,rlevel+1);

	 // newPoly is destroyed
  }
  else {

	Polygon *polyptr = fracObject->GetPolygonPtr(polynum);
	if( polyptr->MakeVertexArray(3) )
		return;
	 polyptr->SetVertexPtr(0,vptr);
	 polyptr->SetVertexPtr(1,vptr+1);
	 polyptr->SetVertexPtr(2,vptr+5);
	polyptr->SetOriginalColor(polyColor);

	polyptr = fracObject->GetPolygonPtr(polynum+1);
	if( polyptr->MakeVertexArray(3) )
		return;
	 polyptr->SetVertexPtr(0,vptr+1);
	 polyptr->SetVertexPtr(1,vptr+2);
	 polyptr->SetVertexPtr(2,vptr+3);
	polyptr->SetOriginalColor(polyColor);

	polyptr = fracObject->GetPolygonPtr(polynum+2);
	if( polyptr->MakeVertexArray(3) )
		return;
	 polyptr->SetVertexPtr(0,vptr+5);
    polyptr->SetVertexPtr(1,vptr+3);
	 polyptr->SetVertexPtr(2,vptr+4);
	polyptr->SetOriginalColor(polyColor);

	polyptr = fracObject->GetPolygonPtr(polynum+3);
	if( polyptr->MakeVertexArray(3) )
		return;
	 polyptr->SetVertexPtr(0,vptr+5);
    polyptr->SetVertexPtr(1,vptr+1);
	 polyptr->SetVertexPtr(2,vptr+3);
	polyptr->SetOriginalColor(polyColor);

    vertnum+=6;
	 polynum+=4;

	 fracObject->SetNumVertices(vertnum);
	 fracObject->SetNumPolygons(polynum);
  }
}

void InitRand()
{
	vertex_type *vptr = (world.GetObjectPtr(0))->GetVertexPtr(0);

	for (int i=0; i<5; i++)
	{
		long r=RandomVari();
		 vptr[i].lx+=r;
		r=RandomVari();
		vptr[i].ly+=r;
		r=RandomVari();
		vptr[i].lz+=r;
	}
}


