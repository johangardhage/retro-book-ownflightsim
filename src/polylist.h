// POLYLIST.CPP
// created 9/8/95 mickRacky for Waite Group Press Flights of Fantasy , 2nd Edition

#ifndef POLYLIST_H
#define POLYLIST_H

class PolygonList {
private:
	int		number_of_polygons;
	Polygon **polylist;

public:
	PolygonList() :polylist(0) { ; }
	~PolygonList()
	{
		if (polylist) {
			delete[] polylist;
		}
	}

	void Create(unsigned int numPoly)
	{
		polylist = new Polygon * [numPoly];
	}

	int GetPolygonCount() const
	{
		return number_of_polygons;
	}

	Polygon *GetPolygonPtr(int polynum) const
	{
		return polylist[polynum];
	}

	void MakePolygonList(const World &world)
	{
		// Create a list of all polygons potentially visible in
		//  the viewport, removing backfaces and polygons outside
		//  of the viewing pyramid in the process

		int count = 0;  // Determine number of polygons in list

		// Loop through all objects in world:
		for (int objnum = 0; objnum < world.GetObjectCount(); objnum++) {

			// Create pointer to current object:
			Object *objptr = world.GetObjectPtr(objnum);

			// Loop through all polygons in current object:
			for (int polynum = 0; polynum < objptr->GetPolygonCount(); polynum++) {

				// Create pointer to current polygon:
				Polygon *polyptr = objptr->GetPolygonPtr(polynum);

				// If polygon isn't a backface, consider it for list:
				if (!polyptr->Backface()) {
					// Find maximum & minimum coordinates for polygon:
					int pxmax = -32768; // Initialize all mins & maxes
					int pxmin = 32767;   //  to highest and lowest
					int pymax = -32768;  //  possible values
					int pymin = 32767;
					int pzmax = -32768;
					int pzmin = 32767;

					// Loop through all vertices in polygon, to find
					//  ones with higher and lower coordinates than
					//  current min & max:
					for (int v = 0; v < polyptr->GetVertexCount(); v++) {
						vertex_type *vptr = polyptr->GetVerticePtr(v);
						if (vptr->ax > pxmax) {
							pxmax = (int)vptr->ax;
						}
						if (vptr->ax < pxmin) {
							pxmin = (int)vptr->ax;
						}
						if (vptr->ay > pymax) {
							pymax = (int)vptr->ay;
						}
						if (vptr->ay < pymin) {
							pymin = (int)vptr->ay;
						}
						if (vptr->az > pzmax) {
							pzmax = (int)vptr->az;
						}
						if (vptr->az < pzmin) {
							pzmin = (int)vptr->az;
						}
					}

#ifdef HID_SURF_TEST5
					// If we plan on using the 5 additional tests for
					// hidden surface removal, then
					// put mins & maxes in polygon descriptor:
					polyptr->SetMin(pxmin, pymin, pzmin);
					polyptr->SetMax(pxmax, pymax, pzmax);
#endif
					// Calculate center of polygon z extent:
					float xcen = (pxmin + pxmax) * 0.5;
					float ycen = (pymin + pymax) * 0.5;
					float zcen = (pzmin + pzmax) * 0.5;
					polyptr->SetDistance(xcen * xcen + ycen * ycen + zcen * zcen);

					// If polygon is in front of the view plane, add it to the polygon list:
					if (pzmax > 1) {
						polylist[count++] = polyptr;
					}
				}
			}
		}

		// Put number of polygons in polylist structure:

		number_of_polygons = count;
	}

	// if elem 1 < elem2 return -
	// if elem1 == elem2 return 0
	// if elem1 > elem2 return +
	static int _PolyCMP(const void *elem1, const void *elem2)
	{
		Polygon *p1 = *((Polygon **)elem1);
		Polygon *p2 = *((Polygon **)elem2);
		double result = p1->GetDistance() - p2->GetDistance();
		if (result > 0.0) {
			return 1;
		} else if (result < 0.0) {
			return -1;
		}
		return 0;
	}

	void z_sort()
	{
		qsort(polylist, number_of_polygons, sizeof(Polygon *), _PolyCMP);
	}
};
#endif
