/*****************************************************************************
File: 		 dwdsp.c
Version:	 2.20
Tab stops: every 2 columns
Project:	 DiamondWare's DSP add-on for its STK product
Copyright: 1995 DiamondWare, Ltd.  All rights reserved.
Written:	 by Keith Weiner & Erik Lorenzen
Purpose:	 Contains DSP functions which operate on DWD buffers
History:	 95/03/25 KW Started
					 95/09/29 EL Re-do for speed & clean up
					 95/10/05 KW Finalized for 2.10
					 95/10/18 EL added #if's to get rid of dword indexing warnings in
											 real mode
					 95/10/20 EL Finalized for 2.20
*****************************************************************************/



#include <time.h>

#include "dwdsp.h"



#ifndef NULL
	#define NULL ((void *)0)
#endif


#define TITLELEN	23


#define TABLESIZE 0x100 										 //number of entries in volume table

#define HALFTABLE (TABLESIZE >> 1)


/*
 . Absolute maximum volumes for 8-bit sound
 .
 . NB: 0x7f is the max positive value, and 0x80 is the maximum negative.
 .		 For further explanation, see any CS text on two's complement.
*/
#define MAXPOSVOL  0x7f
#define MAXNEGVOL -0x80


/*
 . The following two macros are quite useful, aside from their use in
 . DW's DSP.  They are lifted from DW's stddef.h include file.
*/
#define dwdsp_ABS(number) (((number) >= 0) ? (number) : -(number))

#define dwdsp_SWAP(x,y) {(x) ^= (y); (y) ^= (x); (x) ^= (y);}


/*
 . NB: In order to use this struct it must be packed, otherwise the
 .		 code that uses it will get false info and crash the machine.
*/
#ifdef _MSC_VER
	#pragma pack(1) 			/*Microsoft byte align*/
#endif

#ifdef __WATCOMC__
	#pragma pack(1) 			/*Watcom byte align*/ //NB: compatible w/Microsoft!
#endif

#ifdef __BORLANDC__
	#pragma option -a-		/*Borland byte align*/ //NB: product differentiation!
#endif



typedef struct
{
	char	title[23];			/*DiamondWare Digitized\n\0 											 */
	byte	eof;						/*1A (EOF to abort printing of file)							 */
	byte	majorver; 			/*Major Version number														 */
	byte	minorver; 			/*Minor 				"       " 												 */
	dword id; 						/*Unique sound ID 																 */
	byte	reserved1;			/*Oops! :)																				 */
	byte	compress; 			/*Compression type (0=none) 											 */
	word	rate; 					/*Sampling Rate (in Hz) 													 */
	byte	channels; 			/*(1=mono, 2=stereo)															 */
	byte	bits; 					/*Bits per sample per channel 8,16) 							 */
	word	maxsample;			/*dwdsp_ABSolute value of largest sample in file	 */
	dword length; 				/*Length of data section (in bytes) 							 */
	dword numsmp; 				/*Num samples (16bit stereo would be 4bytes/sample)*/
	dword offset; 				/*Offset of data file from start of file (in bytes)*/

} HDR;



#ifdef _MSC_VER
	#pragma pack()				/*Microsoft reset pack option to default cmdln*/
#endif

#ifdef __BORLANDC__
	#pragma option -a.		/*Borland reset pack option to default or cmdln*/
#endif

#ifdef __WATCOMC__
	#pragma pack()				/*Watcom reset pack option to default cmdln*/
#endif



typedef union
{
	HDR  hdr;
	byte data[1]; 														 //we don't really use a len of 1

} DWD;


static word errnum=dwdsp_EZERO; 						 //initialize errnum

static byte dwdtitle[TITLELEN] = "DiamondWare Digitized\n";



/****************************************************************************
* Private functions
****************************************************************************/
static word CheckDWDHdr(byte *srcdwd)
{
	word x;

	for (x=0;x<TITLELEN;x++)
	{
		if (srcdwd[x] != dwdtitle[x])
		{
			return (0);
		}
	}

	return (1);
}


static void CopyDWDHdr(byte *srcdwd, byte *desdwd)
{
	DWD *ds=(DWD *)srcdwd;
	DWD *dd=(DWD *)desdwd;
	time_t timer;
	word x;

	for (x=0;x<ds->hdr.offset;x++)						 //ds->hdr.offset is the begining
	{ 																				 //of the data section of the DWD,
		dd->data[x] = ds->data[x];							 //everything before that can be
	} 																				 //considerd header

	time(&timer); 														 //xor with time to give it a new
	dd->hdr.id ^= timer;											 //unique (enough) id
}


static dword GCF(dword a, dword b)					 //Greatest Common Factor
{
	if (b > a)
	{
		dwdsp_SWAP(a, b);
	}

	if (!b)
	{
		return (a);
	}
	else
	{
		return (GCF(b, a % b));
	}
}


static dword LCM(dword a, dword b)					 //Least Common Multiple
{
	return ((a * b) / GCF(a, b));
}


/****************************************************************************
* Public functions
****************************************************************************/
word dwdsp_ErrNo (void)
{
	return (errnum);
}


word dwdsp_ChngLen(byte *desdwd, byte *srcdwd, dword newlen)
{
	DWD *ds=(DWD *)srcdwd;
	DWD *dd=(DWD *)desdwd;
	int8 *srcdata;
	int8 *desdata;
	double tmp;
	dword  srclen;
	dword  deslen;
	dword  hdrsize;
	dword  interpidx;
	dword  interpsmp;
	dword  lcm;
	dword  x;

	if ((srcdwd == NULL) || (desdwd == NULL))
	{
		errnum = dwdsp_NULLPTR; 								 //sorry can't use a NULL ptr
		return (0);
	}

	if (!CheckDWDHdr(srcdwd))
	{
		errnum = dwdsp_NOTADWD; 								 //sorry can't use srcdwd that
		return (0); 														 //doesn't pt to a DWD
	}

	if (srcdwd == desdwd)
	{
		errnum = dwdsp_SAMEPTR; 								 //sorry can't do this operation
		return (0); 														 //on the source buffer
	}

	hdrsize = ds->hdr.offset;

	if (newlen <= hdrsize)
	{
		errnum = dwdsp_BADLEN;									 //the result DWD is too short
		return (0); 														 //to have any data!
	}

	CopyDWDHdr(srcdwd, desdwd); 							 //make sure hdr is up to date

	#ifdef __FLAT__ 													 //make sure not to index by a
																						 //dword unless flat model is used
		srcdata = (int8 *)(ds->data + hdrsize);  //point to the begining of data
		desdata = (int8 *)(dd->data + hdrsize);  //in the DWDs
	#else
		srcdata = (int8 *)(ds->data + (word)hdrsize); //point to the begining of data
		desdata = (int8 *)(dd->data + (word)hdrsize); //in the DWDs
	#endif

	srclen = ds->hdr.length;
	deslen = newlen - hdrsize;

	lcm = LCM(srclen, deslen);

	dd->hdr.length = deslen;									 //set destination DWD data length

	/*
	 . Go through source DWD, and either duplicate or remove samples
	 . in order to "pitch shift" a sound.  It's not as mathematically
	 . correct as the real technique, but it's much faster.
	 .
	 . NB: The use of floating-point operations has more to do with
	 .		 readability than performance, except on Pentium(R) processors! :)
	 .		 (serious) A technique akin to Bresenham's line would quickly
	 .		 convert this to integer-math.
	*/
	for (x=0;x<deslen;x++)
	{
		tmp = ((double)x * (double)lcm) / (double)deslen;
		interpidx = (dword)tmp;

		tmp = ((double)interpidx * (double)srclen) / (double)lcm;
		interpsmp = (dword)tmp;

		#ifdef __FLAT__ 												 //make sure not to index by a
																						 //dword unless flat model is used
			desdata[x] = srcdata[interpsmp];
		#else
			desdata[(word)x] = srcdata[(word)interpsmp];
		#endif
	}

	return (1);
}


word dwdsp_ChngVol(byte *desdwd, byte *srcdwd, word vollev)
{
	DWD *ds=(DWD *)srcdwd;
	DWD *dd=(DWD *)desdwd;
	dword min=ds->hdr.offset;
	dword max=ds->hdr.length+ds->hdr.offset;
	static byte table[TABLESIZE]; 						 //don't put it on the stack
	word	loop;
	word	tblidx; 														 //used to index into mixing table
	int32 tmp;
	int32 volume;
	dword buffidx;														 //used to index into DWD buffers

	if ((srcdwd == NULL) || (desdwd == NULL))
	{
		errnum = dwdsp_NULLPTR;
		return(0);
	}

	if (!CheckDWDHdr(srcdwd))
	{
		errnum = dwdsp_NOTADWD;
		return(0);
	}

	if (srcdwd != desdwd) 										 //if the source is different from
	{ 																				 //the destination copy the header
		CopyDWDHdr(srcdwd, desdwd);
	}

	for (loop=0;loop<HALFTABLE;loop++)
	{
		/* Postive numbers */
		volume = (int32)loop;
		tblidx = loop;

		tmp = ((volume * (int32)vollev) / dwdsp_IDENTITY);

		if (tmp > MAXPOSVOL)
		{
			tmp = MAXPOSVOL;											 //clip if it exceeds the
		} 																			 //dynamic range

		table[tblidx] = (byte)tmp;

		/* Negative numbers */
		volume = MAXNEGVOL + (int32)loop;
		tblidx = (word)(HALFTABLE + loop);

		tmp = ((volume * (int32)vollev) / dwdsp_IDENTITY);

		if (tmp < MAXNEGVOL)
		{
			tmp = MAXNEGVOL;											 //clip if it exceeds the
		} 																			 //dynamic range

		table[tblidx] = (byte)tmp;
	}

	for (buffidx=min;buffidx<max;buffidx++) 	 //convert sound
	{
		#ifdef __FLAT__ 												 //make sure not to index by a
																						 //dword unless flat model is used
			dd->data[(word)buffidx] = table[ds->data[(word)buffidx]];
		#else
			dd->data[(word)buffidx] = table[ds->data[(word)buffidx]];
		#endif
	}

	tmp = ((ds->hdr.maxsample * vollev) / dwdsp_IDENTITY); //Set max sample

	if (tmp > MAXPOSVOL)
	{
		dd->hdr.maxsample = MAXPOSVOL;
	}

	return (1);
}
