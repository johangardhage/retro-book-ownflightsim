#ifndef TEXTURE_H

#define TEXTURE_H
// Structures to hold texture map data:

struct texture_type { // Structure containing texture mapping vectors
  float ox,oy,oz;  // World coordinates of texture map origin
  float ix,iy,iz;  // Texture map X direction vector
  float jx,jy,jz;  // Texture map Y direction vector
};

#ifndef PCX_H
#include "pcx.h"
#endif
#ifndef POLY_TX_H
#include "poly_tx.h"
#endif

class Texture: public Pcx, texture_type {
	public:
		unsigned char MapPixel(int x,int y,int color,int distance);
		unsigned char MapPoint(int x, int y, int color);
		void CalcMapVectors(Polygon *polygon,int resolution);
};

#endif