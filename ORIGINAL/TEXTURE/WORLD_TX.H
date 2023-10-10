#ifndef WORLD_H

#define WORLD_H

#ifndef POLY_TX_H
#include "poly_tx.h"
#endif
#ifdef NOTUSED
#ifndef PALETTE_H
#include "palette.h"
#endif
#ifndef LIGHT_H
#include "light.h"
#endif
#endif
class	World {
private:
	int		number_of_objects;
	Object	*obj;
	//source_type source;
	//Palette palette;
		// Load light source:
	int LoadSource();
#ifdef LIGHTING_DEMO
	  // Calculate length of normal for polygon at array[objnum]
	void CalcObjectColor(Object *objptr);
	  // Calculate length of normal for each shaded polygon in WORLD:
	void CalcColors();
#endif
public:
	World():obj(0){;}
	~World();
		// loadpoly return -1 on error or number of polys in world
	int LoadPoly(const char *filename);
	// adjust palette to lighting source
	//void AdjustLighting();
	// for outside access:
	int GetObjectCount() const
		{ return number_of_objects; }
	Object *GetObjectPtr(int objnum) const
		{ return &obj[objnum]; }
	//void CalcPolygonColor(Polygon *polyptr);
#ifdef LIGHTING_DEMO
	// TEXTURE DEMO adds resolution to parameter list:
	void Draw(int objnum, unsigned char far *screen,int xorigin, int yorigin, int resolution);
	void Draw(unsigned char far *screen,int xorigin, int yorigin, int resolution);
#endif

};

#endif