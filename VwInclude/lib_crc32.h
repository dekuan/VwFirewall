//	Crc32.h
//	LiuQiXing @ 2006-08-19
//
//////////////////////////////////////////////////////////////////////


#ifndef __LIB_CRC32_HEADER__
#define __LIB_CRC32_HEADER__


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>


DWORD LibCrc32_GetCrc32( const BYTE * pBinBuf, int nBufLen );

//	Init Crc32 Table
VOID LibCrc32_InitCrc32Table();

//	Calc crc32 ... 
VOID LibCrc32_CalcCrc32( const BYTE byte, DWORD &dwCrc32 );






#endif	// __LIB_CRC32_HEADER__
