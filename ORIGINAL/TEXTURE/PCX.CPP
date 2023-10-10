
// PCX.CPP - 1/10/93
//
// A set of functions to create a PCX object for loading and
// compressing PCX files.
//
// Copyright 1993 by Christopher Lampton and The Waite Group Press
//
//  8/21/95 mickRacky updated pcx class to full class including
//           constructor and destructor management of buffers

#include <stdio.h>
#include <dos.h>
#include <io.h>
#include <fcntl.h>
#include <mem.h>

#include "screen.h"
#include "pcx.h"

const long IMAGE_SIZE=64000L; // Size of PCX bitmap

// Pcx class constructor
Pcx::Pcx()
{
	image = cimage = 0; // set initial pointers to 0
}

// Pcx class destructor
Pcx::~Pcx()
{
	// if pointers have changed from initial 0
	// then release memory
	if( image)
		delete [] image;
	if( cimage)
		delete [] cimage;
}


int Pcx::Load(char far *filename)
{

// Function to load PCX data from file FILENAME     

	// Open file; return nonzero value on error

	if ((infile=open(filename,O_BINARY))==-1)
			return(-1);

  // Move file pointer to start of header:

	lseek(infile,0L,SEEK_SET);

  // Reader header from PCX file:

	read(infile,&header,	sizeof(pcx_header));

  // The PCX class is now reuseable. If there is already an image there
  // release it...
	if( image != 0)
		delete [] image;
	if( cimage != 0)
		delete [] cimage;
	image = cimage = 0;
  // Decompress bitmap and place in buffer,
  //  returning non-zero value if error occurs:

	if (load_image()) return(-1);

  // Decompress palette and store in array:

	load_palette();

	close(infile); // Close PCX file
	return(0);     // Return non-error condition
}

// Decompress bitmap and store in buffer
int Pcx::load_image()
{
  // Symbolic constants for encoding modes, with
  //  BYTEMODE representing uncompressed byte mode
  //  and RUNMODE representing run-length encoding mode:

  	const int BYTEMODE=0, RUNMODE=1;

  		// Buffer for data read from disk:

  	const int BUFLEN=5*1024;
	int mode=BYTEMODE;  // Current encoding mode being used,
							 //  initially set to BYTEMODE
  	int readlen;  // Number of characters read from file
	static unsigned char outbyte; // Next byte for buffer
  	static unsigned char bytecount; // Counter for runs
	static unsigned char buffer[BUFLEN]; // Disk read buffer

  // Allocate memory for bitmap buffer and return -1 if
  //  an error occurs:
	if ((image=new unsigned char[IMAGE_SIZE])==0)
		return(-1);
	int bufptr=0; // Point to start of buffer
	readlen=0;    // Zero characters read so far

  // position file to offset for reading image
	lseek(infile,128L,SEEK_SET);
  // Create pointer to start of image:
	unsigned char *image_ptr=image;

  // Loop for entire length of bitmap buffer:

	for (long i=0; i<IMAGE_SIZE; i++) {
		if (mode==BYTEMODE) { // If we're in individual byte
								  //  mode....
			if (bufptr>=readlen) { // If past end of buffer...
				bufptr=0;            // Point to start

        // Read more bytes from file into buffer;
        //  if no more bytes left, break out of loop

				if ((readlen=read(infile,buffer,BUFLEN))==0)
					break;
			}
			outbyte=buffer[bufptr++]; // Next byte of bitmap
			if (outbyte>0xbf) {       // If run-length flag...

		  // Calculate number of bytes in run:

				bytecount = (int)((int)outbyte & 0x3f);
				if (bufptr>=readlen) {  // If past end of buffer
					bufptr=0;             // Point to start

			 // Read more bytes from file into buffer;
			 //  if no more bytes left, break out of loop

					if ((readlen=read(infile,buffer,BUFLEN))==0)
						break;
				}
				outbyte=buffer[bufptr++]; // Next byte of bitmap

        // Switch to run-length mode:

				if (--bytecount > 0) mode = RUNMODE;
			}
		}

    // Switch to individual byte mode:

		else if (--bytecount == 0) mode=BYTEMODE;

    // Add next byte to bitmap buffer:

		*image_ptr++=outbyte;
	}
	return 0;
}

void Pcx::load_palette()

// Load color register values from file into palette array

{

  // Seek to start of palette, which is always 768 bytes
  //  from end of file:

	lseek(infile,-768L,SEEK_END);

  // Read all 768 bytes of palette into array:

	unsigned char *ptr = Palette();

	read(infile,ptr ,3*256);

  // Adjust for required bit shift:

	for (int i=0; i<3*256; i++, ptr++)
			*ptr >>= 2;
}


// Helper function for bitmap compression
// if cbuffer is 0 then just return count
unsigned int Pcx::CompressWork(unsigned char *cbuffer)
{

	long index=0;  // Pointer to position in IMAGE
	long cindex=0; // Pointer to position in CIMAGE

	unsigned char *image_ptr = Image();

	int writeFlag = (cbuffer != 0);
	int runptr;
  	// Begin compression:
	
	while (index<IMAGE_SIZE) 
	{
	    int runlength = 0; // Set length of run to zero
		if (image_ptr[index]==0)  // If next byte transparent
		{
      		// Loop while next byte is zero:

			while (image_ptr[index]==0) 
			{
				index++;       // Point to next byte of IMAGE
				runlength++; // Count off this byte
				if (runlength>=127) break; // Can't count more than
											  //  127
				if (index >= IMAGE_SIZE) break; // If end of IMAGE,
                                      //  break out of loop
			}

      		// Set next byte of CIMAGE to run length:
			if( writeFlag) {
				cbuffer[cindex] = (unsigned char) (runlength + 128);
				// Set next byte of CIMAGE to zero:
				cbuffer[cindex+1] = 0;
				}
			cindex++; // Point to next byte of CIMAGE
			cindex++; // Point to next byte of CIMAGE
		}
		else 
		{

		  // If not a run of zeroes, get next byte from IMAGE
			runptr=cindex++; // Remember this address, then point
							//  to next byte of CIMAGE

		  // Loop while next byte is not zero:

			while (image_ptr[index]!=0) 
			{

		        // Get next byte of IMAGE into CIMAGE:

				if( writeFlag)
					cbuffer[cindex]=image_ptr[index];
				cindex++; // Point to next byte of CIMAGE
				index++;  // Point to next byte of IMAGE
						// can't count  more than 127
				if (++runlength>=127) 
					break; 
					// If end of IMAGE, break out of loop
				if (index >= IMAGE_SIZE) 
					break; 
			}
			if( writeFlag)
				cbuffer[runptr]=runlength; // Put run length in buffer
		}
	}

	return(cindex); // Return length of buffer
}

// Function to compress bitmap stored in IMAGE into
//  compressed image buffer CIMAGE
int Pcx::Compress()
{

	clength = CompressWork(0); // Set length of compressed image

	// Allocate memory for CIMAGE, return error if memory
	//  not available:
	
	cimage=new unsigned char[clength];

	if (cimage)  
	{
		// perform the loop again, this time on the 
		// allocated CIMAGE buffer:
		CompressWork(cimage);
		return clength;
	}

	return(0); // Return status
}
