#ifndef WORLD_FR_H

// WORLD CLASS file for fractal experiments

#define WORLD_FR_H

#ifndef POLY_FR_H
#include "poly_fr.h"
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
	  // Calculate length of normal for polygon in object
	void CalcObjectColor(Object *bjptr);
public:
	World():obj(0){;}
	~World();
		// loadpoly return -1 on error or number of polys in world
	int LoadPoly(const char *filename);
	// adjust palette to lighting source
	void AdjustLighting();
	// set the object's normals for lighting
	void SetLighting(Object *objptr);
	// for outside access:
	int GetObjectCount() const
		{ return number_of_objects; }
	Object *GetObjectPtr(int objnum) const
		{ return &obj[objnum]; }
	void CalcPolygonColor(Polygon *polyptr);
	void Draw(Object *objptr, unsigned char far *screen,int xorigin, int yorigin);
	void SetAmbient(int ambient);
	int GetAmbient()
		{ return source.ambient; }
};


#endif