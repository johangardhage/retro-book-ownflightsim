#ifndef VIEW_H

#define VIEW_H

#ifndef WORLD_H
#include "world.h" // defines World and includes ClippedPolygon
#endif
#ifndef POLYLIST_H
#include "polylist.h"
#endif

struct view_type {
	int copx,copy,copz;
	int xangle,yangle,zangle;
};

class View
{
private:
	int xorigin,yorigin;
	int xmin,ymin,xmax,ymax;
	int distance,ground,sky;
	World world;
	unsigned char *screen_buffer;
	ClippedPolygon clip_array;
	PolygonList polygon_list;
	int projection_overlap(Polygon poly1, Polygon poly2);
	int intersects(int x1_1,int y1_1,int x2_1,int y2_1,
					int x1_2,int y1_2,int x2_2,int y2_2);
	void AlignView(World *world,view_type view);
	void DrawHorizon(int xangle,int yangle,int zangle,
					 unsigned char *screen);
	void Update(Object *object);
	void DrawPolygonList(unsigned char far *screen);
	void XYClip(ClippedPolygon *clip);
	void ZClip(Polygon *polygon,ClippedPolygon *clip, int zmin=1);
public:
	void SetView(int xo,int yo,int xmn,int ymn,int xmx,
				 int ymx,int dist,int grnd,int sk,
				 unsigned char *screen_buf);
	int  SetWorld(const char *worldfilename);
	void Display(view_type curview,int horizon_flag);
	World& GetWorld()
		{ return world; }

};

#endif