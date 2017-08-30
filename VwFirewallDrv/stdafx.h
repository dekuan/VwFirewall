#ifndef __STDAFX_HEADER__
#define __STDAFX_HEADER__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ntifs.h>
#include <ntddk.h>


////////////////////////////////////////////////////////////
//
typedef BOOLEAN		BOOL;
typedef int		INT;
typedef unsigned int	UINT;
typedef unsigned short	WORD;
typedef unsigned long	ULONG;
typedef unsigned long	DWORD;
typedef UCHAR		BYTE;
typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;


#define MAKEWORD(a, b)		((WORD)(((BYTE)((DWORD_PTR)(a) & 0xff)) | ((WORD)((BYTE)((DWORD_PTR)(b) & 0xff))) << 8))
#define MAKELONG(a, b)		((LONG)(((WORD)((DWORD_PTR)(a) & 0xffff)) | ((DWORD)((WORD)((DWORD_PTR)(b) & 0xffff))) << 16))
#define LOWORD(l)		((WORD)((DWORD_PTR)(l) & 0xffff))
#define HIWORD(l)		((WORD)((DWORD_PTR)(l) >> 16))
#define LOBYTE(w)		((BYTE)((DWORD_PTR)(w) & 0xff))
#define HIBYTE(w)		((BYTE)((DWORD_PTR)(w) >> 8))


#define MAX_PATH		260
#define MAX_WPATH		MAX_PATH*2
#define MAX_BUFFER		2048

#define IS_USER_DEBUG		0





/**
 *	for OutputDebugString
 */
#if DBG
	#define dprintf DbgPrint
#else
	#define dprintf(x)
#endif





#endif	// __STDAFX_HEADER__