// PCX.CPP - 1/10/93
//
// A set of functions to create a PCX object for loading and
// compressing PCX files.
//
// Copyright 1993 by Christopher Lampton and The Waite Group Press
//
//  8/21/95 mickRacky updated pcx class to full class including
//           constructor and destructor management of buffers

#ifndef PCX_H
#define PCX_H

#define	NO_ERROR 0
#define FILE_NOT_OPENED 1
#define INSUFFICIENT_MEMORY 2
#define TOO_MANY_ARGUMENTS 3

const int IMAGE_SIZE = 64000; // Size of PCX bitmap

struct pcx_header {
	char manufacturer;          // Always set to 0
	char version;               // Always 5 for 256-color files
	char encoding;              // Always set to 1
	char bits_per_pixel;        // Should be 8 for 256-color files
	short int  xmin, ymin;      // Coordinates for top left corner
	short int  xmax, ymax;      // Width and height of image
	short int  hres;            // Horizontal resolution of image
	short int  vres;            // Vertical resolution of image
	char palette16[48];         // EGA palette; not used for 256-color files
	char reserved;              // Reserved for future use
	char color_planes;          // Color planes
	short int  bytes_per_line;  // Number of bytes in 1 line of  pixels
	short int  palette_type;    // Should be 2 for color palette
	char filler[58];            // Nothing but junk
};

// Class for loading 256-color VGA PCX files
class Pcx
{
private:
	pcx_header header;    // Structure for holding the PCX header
	// Private functions and variables for class Pcx
	FILE *infile;  // File handle for PCX file to be loaded

	// Decompress bitmap and store in buffer
	int load_image()
	{
		// Symbolic constants for encoding modes, with
		//  BYTEMODE representing uncompressed byte mode
		//  and RUNMODE representing run-length encoding mode:
		const int BYTEMODE = 0, RUNMODE = 1;

		// Buffer for data read from disk:
		const int BUFLEN = 5 * 1024;
		int mode = BYTEMODE;  // Current encoding mode being used, initially set to BYTEMODE
		int readlen;  // Number of characters read from file
		static unsigned char outbyte; // Next byte for buffer
		static unsigned char bytecount; // Counter for runs
		static unsigned char buffer[BUFLEN]; // Disk read buffer

		// Allocate memory for bitmap buffer and return -1 if
		//  an error occurs:
		if ((image = new unsigned char[IMAGE_SIZE]) == 0) {
			return(-1);
		}

		int bufptr = 0; // Point to start of buffer
		readlen = 0;    // Zero characters read so far

		// position file to offset for reading image
		fseek(infile, 128L, SEEK_SET);

		// Create pointer to start of image:
		unsigned char *image_ptr = image;

		// Loop for entire length of bitmap buffer:
		for (long i = 0; i < IMAGE_SIZE; i++) {
			if (mode == BYTEMODE) { // If we're in individual byte mode....
				if (bufptr >= readlen) { // If past end of buffer...
					bufptr = 0;            // Point to start

					// Read more bytes from file into buffer;
					//  if no more bytes left, break out of loop
					if ((readlen = fread(buffer, 1, BUFLEN, infile)) == 0) {
						break;
					}
				}

				outbyte = buffer[bufptr++]; // Next byte of bitmap
				if (outbyte > 0xbf) {       // If run-length flag...
					// Calculate number of bytes in run:
					bytecount = (int)((int)outbyte & 0x3f);
					if (bufptr >= readlen) {  // If past end of buffer
						bufptr = 0;             // Point to start

						// Read more bytes from file into buffer;
						//  if no more bytes left, break out of loop
						if ((readlen = fread(buffer, 1, BUFLEN, infile)) == 0) {
							break;
						}
					}
					outbyte = buffer[bufptr++]; // Next byte of bitmap

					// Switch to run-length mode:
					if (--bytecount > 0) {
						mode = RUNMODE;
					}
				}
			}

			// Switch to individual byte mode:
			else if (--bytecount == 0) {
				mode = BYTEMODE;
			}

			// Add next byte to bitmap buffer:
			*image_ptr++ = outbyte;
		}
		return 0;
	}

	// Load color register values from file into palette array
	void load_palette()
	{
		// Seek to start of palette, which is always 768 bytes from end of file:
		fseek(infile, -768L, SEEK_END);

		// Read all 768 bytes of palette into array:
		unsigned char *ptr = Palette();

		fread(ptr, 3, 256, infile);

		// Adjust for required bit shift:
		for (int i = 0; i < 3 * 256; i++, ptr++) {
			*ptr >>= 2;
		}
	}

protected:
	unsigned char *image; // Pointer to a buffer holding the 64000-byte bitmap
	unsigned char *cimage; // Point to a buffer holding a compressed version of the PCX bitmap
	unsigned char palette[3 * 256]; // Array holding the 768-byte palette
	unsigned int clength;  // Length of the compressed bitmap

	// workhorse function for compression returns compressed buffer length
	// Helper function for bitmap compression if cbuffer is 0 then just return count
	unsigned int CompressWork(unsigned char *cbuffer)
	{
		long index = 0;  // Pointer to position in IMAGE
		long cindex = 0; // Pointer to position in CIMAGE

		unsigned char *image_ptr = Image();

		int writeFlag = (cbuffer != 0);
		int runptr;

		// Begin compression:
		while (index < IMAGE_SIZE) {
			int runlength = 0; // Set length of run to zero
			if (image_ptr[index] == 0) {   // If next byte transparent
				// Loop while next byte is zero:
				while (image_ptr[index] == 0) {
					index++;       // Point to next byte of IMAGE
					runlength++; // Count off this byte
					if (runlength >= 127) break; // Can't count more than 127
					if (index >= IMAGE_SIZE) break; // If end of IMAGE, break out of loop
				}

				// Set next byte of CIMAGE to run length:
				if (writeFlag) {
					cbuffer[cindex] = (unsigned char)(runlength + 128);
					// Set next byte of CIMAGE to zero:
					cbuffer[cindex + 1] = 0;
				}
				cindex++; // Point to next byte of CIMAGE
				cindex++; // Point to next byte of CIMAGE
			} else {
				// If not a run of zeroes, get next byte from IMAGE
				runptr = cindex++; // Remember this address, then point to next byte of CIMAGE

				// Loop while next byte is not zero:
				while (image_ptr[index] != 0) {
					// Get next byte of IMAGE into CIMAGE:
					if (writeFlag) {
						cbuffer[cindex] = image_ptr[index];
					}
					cindex++; // Point to next byte of CIMAGE
					index++;  // Point to next byte of IMAGE
					// can't count  more than 127
					if (++runlength >= 127) {
						break;
					}
					// If end of IMAGE, break out of loop
					if (index >= IMAGE_SIZE) {
						break;
					}
				}
				if (writeFlag) {
					cbuffer[runptr] = runlength; // Put run length in buffer
				}
			}
		}

		return(cindex); // Return length of buffer
	}

public:
	// Public functions for class Pcx
	// Pcx class constructor
	Pcx()
	{
		image = cimage = 0; // set initial pointers to 0
	}

	// Pcx class destructor
	~Pcx()
	{
		// if pointers have changed from initial 0
		// then release memory
		if (image) {
			delete[] image;
		}
		if (cimage) {
			delete[] cimage;
		}
	}

	// Function to load PCX data:
	int Load(const char *filename)
	{
		// Function to load PCX data from file FILENAME     
		// Open file; return nonzero value on error
		if ((infile = fopen(filename, "rb")) == NULL) {
			return(-1);
		}

		// Move file pointer to start of header:
		fseek(infile, 0L, SEEK_SET);

		// Reader header from PCX file:
		fread(&header, sizeof(pcx_header), 1, infile);

		// The PCX class is now reuseable. If there is already an image there release it...
		if (image != 0) {
			delete[] image;
		}
		if (cimage != 0) {
			delete[] cimage;
		}
		image = cimage = 0;

		// Decompress bitmap and place in buffer,
		//  returning non-zero value if error occurs:
		if (load_image()) {
			return(-1);
		}

		// Decompress palette and store in array:
		load_palette();

		fclose(infile); // Close PCX file
		return(0);     // Return non-error condition
	}

	// Function to compress bitmap stored in IMAGE into compressed image buffer CIMAGE
	int Compress()
	{
		clength = CompressWork(0); // Set length of compressed image

		// Allocate memory for CIMAGE, return error if memory not available:
		cimage = new unsigned char[clength];
		if (cimage) {
			// perform the loop again, this time on the allocated CIMAGE buffer:
			CompressWork(cimage);
			return clength;
		}

		return(0); // Return status
	}

	// External access to image and palette:
	pcx_header *Header()
	{
		return &header;
	}

	unsigned char *Image()
	{
		return image;
	}

	unsigned char *Palette()
	{
		return palette;
	}

	unsigned char *Cimage()
	{
		return cimage;
	}
};

#endif