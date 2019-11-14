
////////////////////////////////////////////////////////////
//
// env.h        environment type define between Win & Unix
//
// history:
//
//      2006-08009      - Create by LiuQiXing
//
//


#ifndef __ENV_HEADER__
#define __ENV_HEADER__

#include <string.h>


#ifndef WINAPI

	/**
	 *	env is freeBsd
	 *      type define for windows
	 */

        typedef unsigned char   BYTE;
        typedef unsigned int    UINT;
        typedef unsigned long   DWORD;
        typedef int             INT;
        typedef long            BOOL;
        typedef long            LONG;
        typedef char            CHAR;
        typedef void            VOID;

#define FALSE           0
#define TRUE            1
#define stricmp         strcasecmp
#define _snprintf       snprintf

#else

	/**
	 *	env is windows
	 *      type define for freebsd
	 */
	typedef	unsigned short		__uint16_t;
	typedef __uint16_t		uint16_t;
	typedef unsigned char		__charwt;	//	char word type
	//typedef unsigned short	__charwt;

	/*
	 *	Basic types upon which most other types are built.
	 */
	typedef char			__int8_t;
	typedef unsigned char		__uint8_t;
	typedef short			__int16_t;
	typedef unsigned short		__uint16_t;
	typedef int			__int32_t;
	typedef unsigned int		__uint32_t;

//	typedef long long		__int64_t;
//	typedef unsigned long long	__uint64_t;

	/*
	 *	Standard type definitions.
	 */
	typedef unsigned long		__clock_t;		//	clock()...
	typedef unsigned int		__cpumask_t;
	typedef __int32_t		__critical_t;
	typedef double			__double_t;
	typedef double			__float_t;

#endif





#endif  // __ENV_HEADER__