#ifndef PCX_H

#define PCX_H

#define	NO_ERROR 0
#define FILE_NOT_OPENED 1
#define INSUFFICIENT_MEMORY 2
#define TOO_MANY_ARGUMENTS 3

struct pcx_header {
  char manufacturer;    // Always set to 0
  char version;         // Always 5 for 256-color files
  char encoding;        // Always set to 1
  char bits_per_pixel;  // Should be 8 for 256-color files
  int  xmin,ymin;       // Coordinates for top left corner
  int  xmax,ymax;       // Width and height of image
  int  hres;            // Horizontal resolution of image
  int  vres;            // Vertical resolution of image
  char palette16[48];   // EGA palette; not used for
                        //  256-color files
  char reserved;        // Reserved for future use
  char color_planes;    // Color planes
  int  bytes_per_line;  // Number of bytes in 1 line of
                        //  pixels
  int  palette_type;    // Should be 2 for color palette
  char filler[58];      // Nothing but junk
};



// Class for loading 256-color VGA PCX files
class Pcx
{
  private:
  	pcx_header header;    // Structure for holding the PCX
                          //  header
	  // Private functions and variables for class Pcx
    int infile;  // File handle for PCX file to be loaded
    // Bitmap loading function:
    int load_image();
    // Palette loading function:
    void load_palette();
  protected:
	unsigned char far *image; // Pointer to a buffer holding
                            //  the 64000-byte bitmap
  	unsigned char far *cimage; // Point to a buffer holding
                             //  a compressed version of
									  //  the PCX bitmap
  	unsigned char palette[3*256]; // Array holding the 768-
                                //  byte palette
	int clength;  // Length of the compressed bitmap
  public:
  	// Public functions for class Pcx
	Pcx();
	~Pcx();
	 // Function to load PCX data:
    int load(char far *filename);
    // Function to compress PCX bitmap
	 int compress();
	// External access to image and palette:
	unsigned char *Image()
		{ return image; }
	unsigned char *Palette()
		{ return palette; }
};

#endif