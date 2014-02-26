/*
 *  stdtypes.h
 *  AutoStruct
 *
 *  Created by Robert Pasquini on 4/3/09.
 *  Copyright 2009 HomeTime. All rights reserved.
 *
 */

#ifndef _STDTYPES_H_
#define _STDTYPES_H_

#include <memory.h>

typedef int	S32;
typedef unsigned int U32;
typedef float F32;
typedef char S8;
typedef unsigned short U16;
typedef short S16;
typedef unsigned char U8;

#define ARRAY_SIZE(x)		(sizeof((x))/sizeof(*(x)))

#define CLAMP(val,min,max)	(((val)>(max))?(max):(((val)<(min))?(min):(val)))

#define ZeroStruct(s)		(memset(&s, 0, sizeof(s)))

#if defined (ABS)
#undef ABS 
#endif // MINE!

#if defined (MIN)
#undef MIN
#endif

#if defined (MAX)
#undef MAX
#endif

#define ABS(x)				(((x)<0)?-(x):(x))
#define SQR(x)				((x)*(x))
#define MIN(x,y)			(((x)<(y))?(x):(y))
#define MAX(x,y)			(((x)>(y))?(x):(y))

#define IS_POW2(x)			(((x)==1) || !((x)&((x)-1)))

#ifndef NULL
#define NULL	0
#endif

#define SET_FLAG(x,f)				((x)|=(f))
#define CLEAR_FLAG(x,f)				((x)&=~(f))
#define IS_FLAG_SET(x,f)			((x)&(f))
#define IS_FLAG_SET_STRICT(x,f)		(IS_FLAG_SET(x,f)==(f))


#define xcase break; case
#define xdefault break; default

#if PLATFORM_LINUX
#include <stdint.h>
#else
#include <stddef.h>
#endif

#define UINTPTR intptr_t	

inline U32 promoteToNearest(U32 x, U32 y)
{
	U32 remainder = x % y;
	return (remainder) ? x + (y - remainder) : x;
}


#define MM_EXPOSE /*just incase, means same as 'ED*/
#define MM_EXPOSED /**/
#define MM_IGNORE_START /**/
#define MM_IGNORE_END /**/

#ifndef FLT_MAX
#define FLT_MAX         3.402823466e+38F        /* max value */
#endif
#endif
