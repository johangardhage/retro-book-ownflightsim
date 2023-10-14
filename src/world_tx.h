// WORLD_TX.CPP
//   A set of functions to perform lighting calculations for polygons.
// copyright (c) 1995 Waite Group Press
// 7/19/95 Dale Sather        lighting functions created
// 9/20/95 mracky@sirius.com  update to C++; created World class

#ifndef WORLD_H
#define WORLD_H

class World {
private:
	int number_of_objects;
	Object *obj;

public:
	World() :obj(0) { ; }
	~World()
	{
		if (obj) {
			delete[] obj;
		}
	}

	// loadpoly return -1 on error or number of polys in world
	int LoadPoly(const char *filename)
	{
		if (_pf.Open(filename)) {
			return(-1);
		}

		// Light Source info is 4 ints
		int temp = 4;
		while (temp--) {
			_pf.getnumber();
		}

		// three colors for each non-shade:
		temp = _pf.getnumber() * 3;
		while (temp--) _pf.getnumber();

		// three colors for each shade:
		temp = _pf.getnumber() * 3;
		while (temp--) _pf.getnumber();

		// Initialize polygon count:
		polycount = 0;

		// world members:
		number_of_objects = _pf.getnumber();
		obj = new Object[number_of_objects];
		if (!obj) {
			return(-1);
		}

		for (int objnum = 0; objnum < number_of_objects; objnum++) {
			if (obj[objnum].Load()) {
				return(-1);
			}
		}

		return(polycount);
	}

	// adjust palette to lighting source
	int GetObjectCount() const
	{
		return number_of_objects;
	}

	Object *GetObjectPtr(int objnum) const
	{
		return &obj[objnum];
	}

	void Draw(int objnum, unsigned char *screen, int xorigin, int yorigin, int resolution)
	{
		if (objnum < number_of_objects) {
			Object *optr = &obj[objnum];
			optr->Transform();       // Transform OBJECT using MATRIX
			optr->Project(400, xorigin, yorigin); // Perform perspective projection
			optr->Draw(screen, 400, resolution);  // Draw transformed object
		}
	}

	void Draw(unsigned char *screen, int xorigin, int yorigin, int resolution)
	{
		for (int i = 0; i < number_of_objects; i++) {
			Draw(i, screen, xorigin, yorigin, resolution);
		}
	}
};

#endif








