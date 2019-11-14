//	Crc32.h
//	LiuQiXing @ 2006-08-19
//
//////////////////////////////////////////////////////////////////////


#ifndef __CRC32_HEADER__
#define __CRC32_HEADER__


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>



/**
 *	CRC32 class
 */
class CCrc32
{
public:
	CCrc32();
	virtual ~CCrc32();


public:
	//
	DWORD GetCrc32( const BYTE * pBinBuf, int nBufLen );

private:
	//	Init Crc32 Table
	VOID  InitCrc32Table();

	//	Calc crc32 ... 
	inline void CalcCrc32( const BYTE byte, DWORD &dwCrc32 );



private:
	//	..
	DWORD m_ArrdwCrc32Table[ 256 ];

};





#endif
