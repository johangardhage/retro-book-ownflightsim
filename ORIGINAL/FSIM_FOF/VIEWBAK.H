
// #include "poly.h"

class View
{
private:
	int xorigin,yorigin;
	int xmin,ymin,xmax,ymax;
	int distance,ground,sky;
	unsigned char *screen_buffer;
	clipped_polygon_type clip_array;
//	void project(object_type *object);
	void cproject(clipped_polygon_type *clip);
//	void clip(polygon_type *polygon,clipped_polygon_type *clip);
	void zclip(polygon_type *polygon,clipped_polygon_type *clip);
	void xyclip(clipped_polygon_type *clip);
	void make_polygon_list(world_type world,polygon_list_type *polylist);
	void draw_polygon_list(polygon_list_type *polylist, char far *screen);
	int	backface(polygon_type p);
	void alignview(world_type *world,view_type view);
	void draw_horizon(int xangle,int yangle,int zangle,unsigned char *screen);
public:
	View();
	void setwindow(int xo,int yo,int xmn,int ymn,int xmx,int ymx,int dist,
					int grnd,int sk,unsigned char *screen_buf);
	void display(world_type world,view_type curview,int horizon_flag);
};

