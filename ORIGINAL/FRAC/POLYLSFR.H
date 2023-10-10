#ifndef POLYLSFR_H

#define POLYLSFR_H

#include "poly_fr.h" // Polygon
#include "world_fr.h" // World

class PolygonList {
private:
	int		number_of_polygons;
	Polygon **polylist;
	void ExtractPolygons(Object *objptr);
public:
	PolygonList():polylist(0){;}
	~PolygonList();
	void Create(unsigned int polynum);
	int GetPolygonCount() const
		{ return number_of_polygons; }
	Polygon * GetPolygonPtr(int polynum) const
		{ return polylist[polynum]; }
	void MakePolygonList(Object * object);
	void MakePolygonList(World& world);
	void z_sort();
};


#endif