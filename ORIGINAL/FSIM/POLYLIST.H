#ifndef POLYLIST_H

#define POLYLIST_H

#include "poly2.h" // Polygon
#include "world.h" // World

class PolygonList {
private:
	int		number_of_polygons;
	Polygon **polylist;
public:
	PolygonList():polylist(0){;}
	~PolygonList();
	void Create(unsigned int polynum);
	int GetPolygonCount() const
		{ return number_of_polygons; }
	Polygon * GetPolygonPtr(int polynum) const
		{ return polylist[polynum]; }
	void MakePolygonList(const World& world);
	void z_sort();
};


#endif