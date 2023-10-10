#ifndef PALETTE_H

#define PALETTE_H

// Structure for color data:

struct rgb_type {      // Structure for RGB color definition
  unsigned char red;   // Red intensity (0 to 63)
  unsigned char green; // Green intensity (0 to 63)
  unsigned char blue;  // Blue intensity (0 to 63)
};



class Palette { 
private:
  int number_of_unshaded_colors; // Number of colors with no brightness
  rgb_type *unshaded_color;      // List of shaded colors
  int number_of_shaded_colors;   // Number of colors with brightness
  rgb_type *shaded_color;        // List of unshaded colors
  int number_of_shades;          // Number of brightness levels
public:
		// constructor
	Palette():unshaded_color(0), shaded_color(0){;}
		// destructor
	~Palette();

	int Load();
		// adjust for ambient light:
	void Install(float ambient);
		// retrieve information:
	int GetUnshadedCount()
		{ return number_of_unshaded_colors; }
	rgb_type *GetUnshadedPtr(int i)
		{ return &unshaded_color[i]; }
	int GetShadedCount()
		{ return number_of_shaded_colors; }
	rgb_type *GetShadedPtr(int i)
		{ return &shaded_color[i]; }
		// compute the index into shaded colors:
	int ColorIndex(int color,int level,int ambient);
};



#endif