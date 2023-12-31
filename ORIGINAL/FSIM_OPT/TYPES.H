//-------+---------+---------+---------+---------+---------+---------+---------+
// 1991-1992 Betz Associates. Released to the Public Domain. This source code
// is offered without warranty or guarantee of any kind, and the author(s)
// expressly disclaim any responsibility for consequences of it's use in
// products for commercial, shareware, or public domain release of any kind.
//
// Release of copyright applies only to this source code module, and does
// not apply to any other source code which may be included with it in a
// program
//
// File Name: TYPES.H
// Project:   Flights of Fantasy
// Creation:  August 2, 1992
// Author:    Mark Betz (MB)
//
// Machine:   IBM PC and Compatibles
// Change History
// ------ -------
//
//      Date            Rev.	Author		Purpose
//      ----            ----    ------      -------
//      8-2-1992        1.0     MB          Initial development
//      8-29-1992       1.1b    MB          first beta
//      9-26-1992       1.1     MB          publication release
//      8-23-1995       2.0     mracky
//
// Description
// -----------
//      Provides some basic redefinition of native C types
//-------+---------+---------+---------+---------+---------+---------+---------+

#ifndef TYPES_H
#define TYPES_H

typedef unsigned char byte;
typedef unsigned int word;
typedef unsigned long dword;

// note: this will most likely be troublesome in the future since
// boolean will be a built-in C++ type
typedef int boolean;

const int false = 0;
const int true = 1;

#endif