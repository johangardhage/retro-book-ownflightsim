// tmem.cpp
// Copyright 1988-1995, HXD (hard times development)
//  written by mickRacky
//  comments to mracky@sirius.com
//  This code is free to anyone who wants to use it as long as they
//  don't distribute it for money.
//
// this contains code to track memory heap usage
//	the module has never been cleaned-up so expect a lot of
//	garbage code...

// HOW IT WORKS
//   This depends on the interactive nature of turbo debugger
//   which allows you to call a function from the data examine window.
//   From that window you can turn off or on the track memory utility.
//
//	the module uses C++ operator override of new and delete to include
//   code which links these as nodes into a linked list.  This linked list
//   can then be used to determine if all memory is released.
//
// HOW TO USE
//   include this module in your project with the compiler node option
//    MEM_DEBUG as a define.
//
//   call ToggleTrackMem() from TD's DE at some point in the code
//		from where you want to start to track memory usage
//	 proceed to debug
//	 then call CheckCore() for a list of unreleased heap objects
//	    the default report goes to the DOS screen, but you can easily
//      change this to a file or printer
//      by using the static variables toPrinter or toFile(see below)

//
//
//	  you can also use CheckMem((void *)0,0) to check internal integrity
//	  or use CheckMem((void *)nn,0) to check a particular
//	  node for integrity in the internal heap. it should return a
//		value of 2 (heap ok), 3 (node free), but never -1 (heap corrupt)
//	  this will write to all free space in the heap with a "magic number".
//	   after this call is made you can explicitly check the free space in
//		the heap with walkFree()
//


#ifndef MEM_DEBUG
#include <stdlib.h>
#include <errno.h>
void  ToggleTrackMem()
{
	;
}

#else

#include <stdio.h>
#include <string.h>
#include <mem.h>
#include <dos.h>
#include <stdlib.h>
#include <alloc.h>
#include <errno.h>

const int BCOffset = 0x0004; // all Borland allocation blocks' offset

/***********************************************************************/
	// programmer debug functions:
	// call any of these from the Data Examine window.
	//

	// checkcore function first calls setwhere to setup the destination for
	// any reporting, it then calls cCout to report on the node list. and 
	// finally it performs cleanup on any open stream.
int CheckCore(void);


	// call checkmem() for internal integrity check (this error is rare
	//   when borland's rover gets cross linked.)
int CheckMem(void *mm=0, int report=0);

	// walkfree can only be called after checkmem() has been called.
	// this function goes through all the free space to see if the magic
	// number (aa) has been overwritten by any used space which precedes it
	// in contiguous memory.  
int WalkFree(void);
	

/***********************************************************************/
#define MAGIC_NUMBER 0xaa

// maximum number of nodes to track
// increase this if necessary:
#define TRACKMAX 200

struct MEMHDR {
	size_t siz;
	MEMHDR *next;
};
#define ALIGN_SIZE sizeof(double)
#define HDR_SIZE sizeof(MEMHDR)
#define RESERVE_SIZE (((HDR_SIZE+(ALIGN_SIZE-1))/ALIGN_SIZE)*ALIGN_SIZE)
#define CLIENT2HDR(x) ((MEMHDR *)(((char *)(x))- RESERVE_SIZE))
#define HDR2CLIENT(x) ((void *)(((char *)(x)) + RESERVE_SIZE))

void walkMem(int alloFree, void *p);
static void add2list(MEMHDR *p);
static void TagErr( void *p);
static int delelist( MEMHDR *p);
static void ReportItem(register MEMHDR *p, int idx = -1, const char *st=0 );
int cCout(void);


void checkList(MEMHDR *);
static MEMHDR *core = NULL;
static MEMHDR *last = NULL;
static unsigned long mem_size = 0L;
static unsigned long track_size = 0L;
static int trackObject = 0;
static int exitOnErr = 0;
static int toPrinter = 0;
static int toFile	 = 0;
static char reportFile[30] = { "memory.lst" };
static FILE *where = stderr;

static void clearTrack()
{
	last = core = (void *)NULL;
}


void setWhere()
{
	if( toFile)
	{
		where = fopen(reportFile, "at");
		if( where== NULL)
			where = stderr;
	}
	else
	{
		if( toPrinter)
			where = stdprn;
		else
			where = stderr;
	}
}

void unSetWhere()
{
	if( where != stderr)
	{
		if( where != stdprn)
			fclose(where);
		where = stderr;
	}
}
/*
	ToggleTrackMem
		this will turn on or off the track mechanism.
		it also will check the DOS environement variable TRACKMEM
		for preferences on output. to set it up to go to the printer

			C:>SET TRACKMEM=p
		when in debugger the same can be done by using Data Examine
		to set
			toPrinter=1
			toFile=0

*/
void ToggleTrackMem()
{
	trackObject = (! trackObject);
	if( trackObject )
	{
		char *ptr = getenv("TRACKMEM");
		exitOnErr = (strchr(ptr,'x') != NULL);
		toPrinter = (strchr(ptr,'p') != NULL);
		toFile	= (strchr(ptr, 'f') != NULL);
		track_size = mem_size;
	}
	else
	{
		clearTrack();
	}
}

static void ReportItem(register MEMHDR *p, int idx, const char *st )
{
	if( idx == -1)
		fprintf( where, " %p %6u %s\n",p, p->siz,st);
	else
		fprintf( where, "%-5d %p %6u%s",idx, p, p->siz,
				(idx %2)?"\n":"\t\t");

}

// redefine new.
// 
// AFTER USING CHECKCORE to provide you a memory position for
// an unfreed pointer, restart the program and use DE to assign
// the target pointer to sgbrk, then put a break point below
// 1. get the address of an unfreed object. we're interested in
//     the segment.
// 2. put a breakpoint on the first inside this function ( malloc...)
// 3. when the program reaches this line, set sgbrk to the segment address
//    (use data examine)
// 4. release this breakpoint and put a breakpoint where it says "put
//     break point here"
// 5. when the program stops at that point it will be the allocation of that
//     memory space.  This might be your offending object (or possibly an
//     object which is created and then released before your offending object
//     is released). Use View Stack to see where the object is being new'd 
//     from, or as an alternative, use ALT-F8 to execute back to the original 
//     call. 
void *operator new(size_t bigness)
{
	static unsigned int sgbrk = 0;
	register MEMHDR huge *ptr;

			// Borland maps new() to malloc
	ptr = (MEMHDR huge *)malloc( bigness + RESERVE_SIZE);
	if( ptr == NULL)
	{
		errno = ENOMEM;
		return( NULL);
	}
	if( sgbrk && sgbrk == FP_SEG(ptr) )
	{
		int ii = 0;
		ii++; // put break point here
	}
	memset(ptr, 0, bigness + RESERVE_SIZE);
	ptr->siz = bigness;
	ptr->next = 0;
	mem_size += bigness;
	if( trackObject)
	{
		add2list((MEMHDR *)ptr);
		/* FOR REALLY HARD TO FIND MEMORY BLOCKS
			UNDO THE NEXT COMMENTS, THIS WILL CAUSE NOTICEABLE
			SLOWDOWNS IN THE PROGRAM
		*/
		/*
		if(	CheckMem((void *)0,0) == -1)
		{
			setWhere();
			walkMem(-1,ptr);
			unSetWhere();
		}
		*/

	}

	return( HDR2CLIENT(ptr ));

}

/*
	debug similarly with delete.
*/
void operator delete(void *ptr)
{
	register MEMHDR *any;
	any = CLIENT2HDR(ptr);
	if( ! ptr )
	{
		// when debuggin put breakpoint here
		int iii = 0;
		iii++;
	}
		// this checks that the offset of the pointer
		// is the same as a normal offset
		// ( THUS if a pointer is allocated from an RTL library
		//   routine, it will not match the tmem allocation)

	if( FP_OFF( any) == BCOffset)
	{
		mem_size -= any->siz;
		if( trackObject )
		{
			if( delelist(any) )
			{
				setWhere();
				ReportItem(any,-1,"Not part of list ");
				unSetWhere();
			}
				// Borland maps delete to free
			free(any);
/*
			if(CheckMem((void *)0,0) == -1)
			{
				setWhere();
				walkMem(0,ptr);
				unSetWhere();
			}
*/

		}
		else
			free(any);
	}
	else
		free(ptr);



}

unsigned long mem_used()
{
	return (trackObject)?(track_size - mem_size):mem_size;
}


static void add2list(MEMHDR *p)
{
	if( ! core )
		core = p;
	if( last )
		last->next = p;
	last = p;
}

static int delelist( MEMHDR *p)
{
	if( p == core )
	{
		core = p->next;
		if( p == last )
			last = core;
	}
	else
	{
		MEMHDR *n = core;

		while(n && n->next != p )
			n = n->next;
		if(n )
		{
			if( p == last )
				last = n;
			n->next = p->next;
		}
		else
		  return -1; // not part of list
	}
	return 0;
}


void TagErr( void *p)
{
	fprintf( stderr, "\n\nInternal Memory error: %p \n", p);
	fprintf( stderr, "Unable to continue. HXD blowUp\n");
	CheckCore();
	if( exitOnErr)
		exit(-1);
}



int CheckCore()
{
	setWhere();
	int retval = cCout();
	unSetWhere();
	return retval;
}



int cCout()
{
	register MEMHDR *p;
	int idx;

	fprintf( where, "\n\n\n");
	idx = 0;
	p = core;
	while ( p != NULL)
	{
		ReportItem(p,idx);
		idx++;
		p = p->next;
	}
	return idx;
}


int CheckMem(void *mm, int report)
{
	int retval;
	if( ! mm )
	{
		retval = heapcheck();
		if( retval == _HEAPOK )
		{
			if( heapfillfree(MAGIC_NUMBER) < 0 )
				retval = -1;
		}
		if( ! report )
			return retval;
		switch( retval)
		{
			case 1:
				fprintf(where,"no heap\n");
				break;
			case 2:
				fprintf(where,"heap okay\n");
				break;
			case -1:
				fprintf(where,"heap corrupt\n");
				break;
			default:
				fprintf(where,"unknown value returned by heapcheck\n");
				break;
		}
	}
	else
	{
		retval = heapchecknode(mm);
		if(  ! report )
			return retval;
		switch( retval )
		{
			case 1:
				fprintf(where,"no heap\n");
				break;
			case -1:
				fprintf(where,"heap corrupt\n");
				break;
			case -2:
				fprintf(where,"node not found\n");
				break;
			case 3:
				fprintf(where,"node is free\n");
				break;
			case 4:
				fprintf(where,"node is used\n");
				break;
			default:
				fprintf(where,"unknown value returned by heapchecknode\n");
				break;
		}
	}
	return retval;
}


int WalkFree()
{
   return( heapcheckfree(MAGIC_NUMBER) );

}

/* an alternative to WalkFree which reports on each node*/
void walkMem(int alloFree, void *ptr)
{
	if( alloFree )
		fprintf(where," %Fp corrupt heap after allocation\n", ptr);
	else
		fprintf(where," %Fp corrupt heap after freeing\n", ptr);

	struct heapinfo hi;
	hi.ptr =(void *)0;
	while( heapwalk(&hi) == _HEAPOK )
	{
			if( hi.in_use )
				fprintf(where," %Fp == size %7u used\n",
						hi.ptr, hi.size);
			else
				fprintf(where, "%Fp == size %7u free\n",
						hi.ptr, hi.size);

	}

}
#endif

