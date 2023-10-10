
// Constant definitions:

const XORIGIN=160;    // Origin of coordinate system X axis
const YORIGIN=100;    // Origin of coordinate system Y axis

// Variable structures to hold shape data:

struct vertex_type {  // Structure for individual vertices
	int lx,ly,lz,lt;		// Local coordinates of vertex
	int wx,wy,wz,wt;    // World coordinates of vertex
	int sx,sy,sz,st;		// Screen coordinates of vertex
};

struct line_type  {   // Structure for wireframe lines
  int start,end;      // Pointers to vertices for start
							 //  and end
};

class Shape {		     // Class for complete shape
private:
	int color;					     // Color of shape
	int	number_of_vertices;  // Number of vertices in shape
  int number_of_lines;     // Number of lines in shape
	vertex_type *vertex;		 // Array of vertex descriptors
  line_type *line;         // Array of line descriptors
public:
  Shape(int numVertex, vertex_type *vert,int numLine, line_type *lin,int col=15):
	vertex(vert),number_of_vertices(numVertex),line(lin),
	number_of_lines(numLine),color(col) {;}
	void Draw(char far *screen);
	// The Translate,Scale and Rotate functions are
	// all combined into one transformation function:
	void Transform(float scale_factor,int xtrans, int ytrans, float angle);
	void Project(int distance);
	void Transform();
};

// Function prototypes:

void inittrans();
void scale(float sf);
void rotate(float ax,float ay,float az);
void translate(int xt,int yt,int zt);


