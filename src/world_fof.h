// WORLD.CPP
//   A set of functions to perform lighting calculations for polygons.
// copyright (c) 1995 Waite Group Press
// 7/19/95 Dale Sather        lighting functions created
// 9/20/95 mracky@sirius.com  update to C++; created World class

#ifndef WORLD_H
#define WORLD_H

// Structure for light source data:
struct source_type { // Structure for light source
	long x, y, z;        // Incident vector
	long length;       // Length of incident vector
	int  ambient;      // Amount of ambient light
};

class World {
private:
	int number_of_objects;
	Object *obj;
	source_type source;
	Palette palette;

	// Load light source:
	int LoadSource()
	{
		source.x = _pf.getnumber();
		source.y = _pf.getnumber();
		source.z = _pf.getnumber();
		source.ambient = _pf.getnumber();
		source.length = (long)sqrt((float)source.x * source.x + (float)source.y * source.y + (float)source.z * source.z);
		return 0;
	}

	// Calculate length of normal for polygon at array[objnum]
	void CalcObjectColor(Object *object)
	{
		for (int p = 0; p < object->GetPolygonCount(); p++) {
			CalcPolygonColor(object->GetPolygonPtr(p));
		}
	}

	// Calculate length of normal for each shaded polygon in WORLD:
	void CalcColors()
	{
		// Calculate the palette index for each polygon in WORLD:
		for (int o = 0; o < number_of_objects; o++) {
			CalcObjectColor(&obj[o]);
		}
	}

public:
	World() :obj(0) { ; }
	~World()
	{
		if (obj) {
			delete[] obj;
		}
	}

	// reinstall palette after PCX screens
	void SetWorldPalette()
	{
		palette.Install(source.ambient);
	}

	// loadpoly return -1 on error or number of polys in world
	int LoadPoly(const char *filename)
	{
		if (_pf.Open(filename)) {
			return(-1);
		}

		LoadSource();
		palette.Load();

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
	void AdjustLighting()
	{

		SetWorldPalette();
		// Calc lengths of polygon normals
		int unshaded = palette.GetUnshadedCount();

		// Calculate normal vectors for each vertex in WORLD:
		for (int o = 0; o < number_of_objects; o++) {
			obj[o].MeasureNormals(unshaded);
		}
	}

	// for outside access:
	int GetObjectCount() const
	{
		return number_of_objects;
	}

	Object *GetObjectPtr(int objnum) const
	{
		return &obj[objnum];
	}

	void CalcPolygonColor(Polygon *polyptr)
	{
		int ambient = source.ambient;
		int temp = polyptr->GetOriginalColor();

		if (temp < palette.GetUnshadedCount()) {
			polyptr->SetColor(temp);
		} else {
			vertex_type *v0 = polyptr->GetVerticePtr(0);
			vertex_type *v1 = polyptr->GetVerticePtr(1);
			vertex_type *v2 = polyptr->GetVerticePtr(2);
			long ax = v0->wx - v1->wx;  // Vector for edge from vertex 1 to vertex 0
			long ay = v0->wy - v1->wy;
			long az = v0->wz - v1->wz;
			long bx = v2->wx - v1->wx;  // Vector for edge from vertex 1 to vertex 2
			long by = v2->wy - v1->wy;
			long bz = v2->wz - v1->wz;
			long nx = ay * bz - az * by;   // Normal is cross product of the two vectors
			long ny = az * bx - ax * bz;
			long nz = ax * by - ay * bx;
			int brightness = (int)(((nx * source.x + ny * source.y + nz * source.z) * (100 - ambient)) / (source.length * polyptr->GetNormalLen()));
			if (brightness < 0) {
				brightness = ambient;  // Ambient applies to polygons not facing the source
			} else {
				brightness += ambient; // Add ambient for polygons facing the source
			}
			polyptr->SetColor(palette.ColorIndex(temp, brightness, ambient));
		}
	}

	void Draw(int objnum, unsigned char *screen, int xorigin, int yorigin)
	{
		if (objnum < number_of_objects) {
			Object *optr = &obj[objnum];
			optr->Transform();       // Transform OBJECT using MATRIX
			CalcObjectColor(optr);
			optr->Project(400, xorigin, yorigin); // Perform perspective projection
			optr->Draw(screen);  // Draw transformed object
		}
	}

	void Draw(unsigned char *screen, int xorigin, int yorigin)
	{
		for (int i = 0; i < number_of_objects; i++) {
			Draw(i, screen, xorigin, yorigin);
		}
	}
};

#endif
