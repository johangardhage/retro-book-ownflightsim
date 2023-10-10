
// PALETTE.CPP
//   A set of functions to manage the palette.
//
// Copyright 1995 by Dale Sather and The Waite Group Press
//
// 9/20/95 mracky@sirius.com  updated to C++: Palette class created 

#include  <stdio.h>
#include  <math.h>
#include  <string.h>
#include  <alloc.h>
#include  "poly2.h"
#include  "palette.h"
#include  "screen.h" // set palette

Palette::~Palette()
{
	if(unshaded_color)
		delete [] unshaded_color;
	if(shaded_color)
		delete [] shaded_color;
}

void Palette::Install(float ambient)
{
  rgb_type allcolors[256];
  // Record the number of shades and convert ambient from percentage
  number_of_shades=(256-number_of_unshaded_colors)/number_of_shaded_colors;
  ambient=ambient/100;

  // Copy unshaded colors to the palette array
  memcpy(&allcolors[0],unshaded_color,number_of_unshaded_colors*sizeof(rgb_type));
	// start filling in the palette at the first available entry:
  int index=number_of_unshaded_colors;

  for (int shade=0; shade<number_of_shades; shade++) {
	 float brightness=(shade*(1-ambient))/(number_of_shades-1)+ambient;
	 for (int color=0; color<number_of_shaded_colors; color++) {
		allcolors[index].red=(unsigned char)(shaded_color[color].red*brightness);
		allcolors[index].green=(unsigned char)(shaded_color[color].green*brightness);
		allcolors[index++].blue=(unsigned char)(shaded_color[color].blue*brightness);
		}
  }
  setpalette((unsigned char far *)allcolors);
}

int Palette::ColorIndex(int color,int level,int ambient)
{
  if (level>100)  // Level should not exceed 100%
	 level=100;
  else if (level<ambient) // Level should not be lower than ambient
	 level=ambient;
  int shade=((level-ambient)*(number_of_shades-1))/(100-ambient);
  return color+shade*number_of_shaded_colors;
}

