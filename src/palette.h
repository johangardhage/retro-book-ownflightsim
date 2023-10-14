// PALETTE.CPP
//   A set of functions to manage the palette.
//
// Copyright 1995 by Dale Sather and The Waite Group Press
//
// 9/20/95 mracky@sirius.com  updated to C++: Palette class created 

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
	Palette() :unshaded_color(0), shaded_color(0) { ; }
	~Palette()
	{
		if (unshaded_color) {
			delete[] unshaded_color;
		}
		if (shaded_color) {
			delete[] shaded_color;
		}
	}

	int Load()
	{
		number_of_unshaded_colors = _pf.getnumber();
		unshaded_color = (rgb_type *)new rgb_type[number_of_unshaded_colors];
		if (!unshaded_color) {
			return -1;
		}

		for (int colornum = 0; colornum < number_of_unshaded_colors; colornum++) {
			unshaded_color[colornum].red = _pf.getnumber();
			unshaded_color[colornum].green = _pf.getnumber();
			unshaded_color[colornum].blue = _pf.getnumber();
		}
		number_of_shaded_colors = _pf.getnumber();
		shaded_color = (rgb_type *) new rgb_type[number_of_shaded_colors];

		if (!shaded_color) {
			return -1;
		}

		for (int colornum = 0; colornum < number_of_shaded_colors; colornum++) {
			shaded_color[colornum].red = _pf.getnumber();
			shaded_color[colornum].green = _pf.getnumber();
			shaded_color[colornum].blue = _pf.getnumber();
		}
		return 0;
	}

	// adjust for ambient light:
	void Install(float ambient)
	{
		rgb_type allcolors[256];
		// Record the number of shades and convert ambient from percentage
		number_of_shades = (256 - number_of_unshaded_colors) / number_of_shaded_colors;
		ambient = ambient / 100;

		// Copy unshaded colors to the palette array
		memcpy(&allcolors[0], unshaded_color, number_of_unshaded_colors * sizeof(rgb_type));
		// start filling in the palette at the first available entry:
		int index = number_of_unshaded_colors;

		for (int shade = 0; shade < number_of_shades; shade++) {
			float brightness = (shade * (1 - ambient)) / (number_of_shades - 1) + ambient;
			for (int color = 0; color < number_of_shaded_colors; color++) {
				allcolors[index].red = (unsigned char)(shaded_color[color].red * brightness);
				allcolors[index].green = (unsigned char)(shaded_color[color].green * brightness);
				allcolors[index++].blue = (unsigned char)(shaded_color[color].blue * brightness);
			}
		}
		setpalette((unsigned char *)allcolors);
	}

	// compute the index into shaded colors:
	int ColorIndex(int color, int level, int ambient)
	{
		if (level > 100) { // Level should not exceed 100%
			level = 100;
		} else if (level < ambient) { // Level should not be lower than ambient
			level = ambient;
		}
		int shade = ((level - ambient) * (number_of_shades - 1)) / (100 - ambient);
		return color + shade * number_of_shaded_colors;
	}

	// retrieve information:
	int GetUnshadedCount()
	{
		return number_of_unshaded_colors;
	}

	rgb_type *GetUnshadedPtr(int i)
	{
		return &unshaded_color[i];
	}

	int GetShadedCount()
	{
		return number_of_shaded_colors;
	}

	rgb_type *GetShadedPtr(int i)
	{
		return &shaded_color[i];
	}
};

#endif
