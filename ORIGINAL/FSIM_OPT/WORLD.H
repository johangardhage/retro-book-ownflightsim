#ifndef WORLD_H

#define WORLD_H

#ifndef POLY2_H
#include "poly2.h"
#endif
#ifndef PALETTE_H
#include "palette.h"
#endif
#ifndef LIGHT_H
#include "light.h"
#endif

class	World {
private:
	int		number_of_objects;
	Object	*obj;
  	source_type source; 
	Palette palette;
		// Load light source:
	int LoadSource();
#ifdef LIGHTING_DEMO
	  // Calculate length of normal for polygon at array[objnum]
	void CalcObjectColor(int objnum);
	  // Calculate length of normal for each shaded polygon in WORLD:
	void CalcColors();
#endif
public:
	World():obj(0){;}
	~World();
		// reinstall palette after PCX screens
	void SetWorldPalette()
		{ palette.Install(source.ambient); }
		// loadpoly return -1 on error or number of polys in world
	int LoadPoly(const char *filename);
	// adjust palette to lighting source
	void AdjustLighting();
	// for outside access:
	int GetObjectCount() const
		{ return number_of_objects; }
	Object *GetObjectPtr(int objnum) const
		{ return &obj[objnum]; }
	void CalcPolygonColor(Polygon *polyptr);
#ifdef LIGHTING_DEMO
	void Draw(int objnum, unsigned char far *screen,int xorigin, int yorigin);
	void Draw(unsigned char far *screen,int xorigin, int yorigin);
#endif
};

#endif