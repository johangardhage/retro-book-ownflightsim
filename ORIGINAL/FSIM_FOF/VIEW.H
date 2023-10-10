
class View
{
private:
	int xorigin,yorigin;
	int xmin,ymin,xmax,ymax;
	int distance,ground,sky;
  world_type world;
	unsigned char *screen_buffer;
	clipped_polygon_type clip_array;
	void cproject(clipped_polygon_type *clip);
  void cproject(polygon_type *poly);
	void cproject(object_type *obj);
	void zclip(polygon_type *polygon,
              clipped_polygon_type *clip);
	void xyclip(clipped_polygon_type *clip);
	void make_polygon_list(world_type world,
           polygon_list_type *polylist);
	void z_sort(polygon_list_type *polylist);
	int should_be_swapped(polygon_type poly1,
                polygon_type poly2);
	int z_overlap(polygon_type poly1,polygon_type poly2);
  int swap_poly(polygon_list_type *polylist);
	int xy_overlap(polygon_type poly1,polygon_type poly2);
	int surface_outside(polygon_type poly1,
                      polygon_type poly2);
	int surface_inside(polygon_type poly1,
                      polygon_type poly2);
	int projection_overlap(polygon_type poly1,
                         polygon_type poly2);
	int intersects(int x1_1,int y1_1,int x2_1,int y2_1,
									int x1_2,int y1_2,int x2_2,int y2_2);
	void draw_polygon_list(polygon_list_type *polylist,
                         char far *screen);
	int	backface(polygon_type p);
	void alignview(world_type *world,view_type view);
	void draw_horizon(int xangle,int yangle,int zangle,
                     unsigned char *screen);
  void update(object_type *object);
public:
	void setview(int xo,int yo,int xmn,int ymn,int xmx,
                 int ymx,int dist,int grnd,int sk,
                 unsigned char *screen_buf);
  void setworld(world_type wrld,int polycnt);
	void display(view_type curview,int horizon_flag);
};
