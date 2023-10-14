
#ifndef SCREEN_H
#define SCREEN_H

const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 200;
const int XORIGIN = (SCREEN_WIDTH / 2);
const int YORIGIN = 90; // (SCREEN_HEIGHT / 2);

static unsigned char *framebuffer = NULL;

// C++ inline functions
// calculates and returns the buffer size necessary to hold
// the image bounded by x1, y1 at top left, and x2, y2 at lower right.
inline int BufSize(int x1, int y1, int x2, int y2)
{
	return(((x2 - x1) + 1) * ((y2 - y1) + 1));
}

void SetGfxBuffer(unsigned char *buffer)
{
	if (buffer == NULL) {
		framebuffer = RETRO.framebuffer;
	} else {
		framebuffer = buffer;
	}
}

void setpalette(unsigned char *palette)
{
	RETRO_Set6bitPalette((RETRO_Palette *)palette);
}

// clears a range of palette registers to zero
void ClrPalette(int start, int number)
{
	if ((start > 256) | (start < 0) | ((start + number) > 256)) {
		return;
	}
	for (int i = start; i < (start + number); i++) {
		RETRO_SetColor(i, 0, 0, 0);
	}
}

// reads the dac data in palette into the supplied palette structure.
void ReadPalette(int start, int number, unsigned char *palette)
{
	if ((start > 256) | (start < 0) | ((start + number) > 256)) {
		return;
	}
	for (int i = start; i < (start + number); i++) {
		RETRO_Palette color = RETRO_Get6bitColor(i);
		palette[i * 3 + 0] = color.r;
		palette[i * 3 + 1] = color.g;
		palette[i * 3 + 2] = color.b;
	}
}

void fadepalin(int start, int end, unsigned char *palette)
{
	for (int i = 0; i < 50; i++) {
		RETRO_FadeIn(50, i, (RETRO_Palette *)palette);
		RETRO_Flip();
		SDL_Delay(50);
	}
}

void fadepalout(int start, int end, unsigned char *palette)
{
	for (int i = 0; i < 50; i++) {
		RETRO_FadeOut(50, i, (RETRO_Palette *)palette);
		RETRO_Flip();
		SDL_Delay(50);
	}
}

void cls(unsigned char *dest)
{
	memset(dest, 0, SCREEN_WIDTH * SCREEN_HEIGHT);
}

void blitscreen(unsigned char *buffer)
{
	memcpy(RETRO.framebuffer, buffer, SCREEN_WIDTH * SCREEN_HEIGHT);
}

// clear the graphics screen to the color provided
void ClearScr(char color)
{
	memset(framebuffer, color, SCREEN_WIDTH * SCREEN_HEIGHT);
}

void clrwin(int x1, int y1, int x2, int y2, unsigned char *dest)
{
	int ydim = (y2 - y1) + 1;
	int xdim = (x2 - x1) + 1;
	for (int i = y1; i < ydim; i++) {
		memset(&dest[(y1 + i) * SCREEN_WIDTH + x1], 0, xdim);
	}
}

void putwindow(int x1, int y1, int x2, int y2, unsigned char *src, unsigned char *dest = framebuffer)
{
	int ydim = (y2 - y1) + 1;
	int xdim = (x2 - x1) + 1;
	for (int i = 0; i < ydim; i++) {
		memcpy(&dest[(y1 + i) * SCREEN_WIDTH + x1], &src[i * xdim], xdim);
	}
}

// returns a void type pointer to video memory at pixel coordinate x,y
unsigned char *XyToPtr(int x, int y)
{
	return(framebuffer + (y * 320) + x);
}

// set the pixel at coordinate x,y to color
void WritePixel(int x, int y, char color)
{
	framebuffer[(y * 320) + x] = color;
}

// GetImage() grabs the pixel values in the rectangle marked by (x1,y1) on
// the top left, and (x2,y2) on the bottom right. The data is placed in buffer.
void GetImage(int x1, int y1, int x2, int y2, unsigned char *dest)
{
	int ydim = (y2 - y1) + 1;
	int xdim = (x2 - x1) + 1;
	for (int i = 0; i < ydim; i++) {
		memcpy(&dest[i * xdim], &framebuffer[(y1 + i) * 320 + x1], xdim);
	}
}

//-------+---------+---------+---------+---------+---------+---------+---------+
// PutImage() copies the data in *buff to a rectangular area of the screen
// marked by (x1,y1) on the top left, and (x2,y2) on the bottom right.
//-------+---------+---------+---------+---------+---------+---------+---------+
void PutImage(int x1, int y1, int x2, int y2, unsigned char *src)
{
	int ydim = (y2 - y1) + 1;
	int xdim = (x2 - x1) + 1;
	for (int i = 0; i < ydim; i++) {
		memcpy(&framebuffer[(y1 + i) * 320 + x1], &src[i * xdim], xdim);
	}
}

void transput(int x1, int y1, int x2, int y2, unsigned char *src, unsigned char *dest)
{
	int ydim = (y2 - y1) + 1;
	int xdim = (x2 - x1) + 1;
	for (int y = 0; y < ydim; y++) {
		for (int x = 0; x < xdim; x++) {
			if (src[y * xdim + x] != 0) {
				dest[(y1 + y) * 320 + x1 + x] = src[y * xdim + x];
			}
		}
	}
}

void ctransput(unsigned char *src, unsigned char *dest)
{
	int srcindex = 0;
	int destindex = 0;

	while (destindex < 64000) {
		unsigned char runlength = src[srcindex++];
		if ((runlength & 128) == 0) {
			for (int i = 0; i < runlength; i++) {
				dest[destindex++] = src[srcindex++];
			}
		} else {
			runlength &= 127;
			srcindex += 1;  // skip the repeat value and assume it is 0, which means it should not be drawn
			destindex += runlength;
		}
	}
}

// fill the rectangular area bounded by (tlx,tly)(brx,bry) with color.
// tlx = top left x, tly = top left y, brx = bottom right x, bry = bottom right y.
void BarFill(int x1, int y1, int x2, int y2, char color)
{
	int xdim = (x2 - x1) + 1;
	for (int row = y1; row <= y2; row++) {
		memset(&framebuffer[(row * 320) + x1], color, xdim);
	}
}

// this function calls the linedraw function in BRESNHAM.ASM.
// This function is required as a gateway because the modules
// calling Line() do not know where the graphics buffer is actually
// located. The location of the buffer is stored in the static
// locations graphSeg and graphOff in this module, and set via calls
// to SetGfxBuffer(). An alternative would be to make graphSeg and
// graphOff global to the program by removing the static storage
// specifier, but this should only be done if the overhead of the
// extra far call involved in using this gateway is proven to be
// significant.
void Line(int x1, int y1, int x2, int y2, char color)
{
	int y_unit, x_unit; // Variables for amount of change in x and y

	int offset = y1 * 320 + x1; // Calculate offset into video RAM

	int ydiff = y2 - y1;   // Calculate difference between y coordinates
	if (ydiff < 0) {     // If the line moves in the negative direction
		ydiff = -ydiff;    // ...get absolute value of difference
		y_unit = -320;     // ...and set negative unit in y dimension
	} else {
		y_unit = 320;   // Else set positive unit in y dimension
	}

	int xdiff = x2 - x1;			// Calculate difference between x coordinates
	if (xdiff < 0) {				// If the line moves in the negative direction
		xdiff = -xdiff;				// ...get absolute value of difference
		x_unit = -1;					// ...and set negative unit in x dimension
	} else {
		x_unit = 1;				// Else set positive unit in y dimension
	}

	int error_term = 0;			// Initialize error term
	if (xdiff > ydiff) {		// If difference is bigger in x dimension
		int length = xdiff + 1;	// ...prepare to count off in x direction
		for (int i = 0; i < length; i++) {  // Loop through points in x direction
			framebuffer[offset] = color;	// Set the next pixel in the line to COLOR
			offset += x_unit;				// Move offset to next pixel in x direction
			error_term += ydiff;		// Check to see if move required in y direction
			if (error_term > xdiff) {	// If so...
				error_term -= xdiff;		// ...reset error term
				offset += y_unit;				// ...and move offset to next pixel in y dir.
			}
		}
	} else {								// If difference is bigger in y dimension
		int length = ydiff + 1;	// ...prepare to count off in y direction
		for (int i = 0; i < length; i++) {	// Loop through points in y direction
			framebuffer[offset] = color;	// Set the next pixel in the line to COLOR
			offset += y_unit;				// Move offset to next pixel in y direction
			error_term += xdiff;    // Check to see if move required in x direction
			if (error_term > 0) {		// If so...
				error_term -= ydiff;	// ...reset error term
				offset += x_unit;			// ...and move offset to next pixel in x dir.
			}
		}
	}
}

void delay(int num)
{
	SDL_Delay(num);
}

#endif
