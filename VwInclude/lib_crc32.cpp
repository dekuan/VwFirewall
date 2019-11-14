#include "StdAfx.h"
#include "lib_crc32.h"


/**
 *	全局变量定义
 */
static DWORD g_LibCrc32_ArrdwCrc32Table[ 256 ];


/**
 *	Get Crc32 Code
 */
DWORD LibCrc32_GetCrc32( const BYTE * pBinBuf, int nBufLen )
{
	DWORD dwCrc32 = 0xFFFFFFFF;

	try
	{
		const BYTE * pPoint = pBinBuf;

		for ( int i = 0; i < nBufLen; i++ )
		{
			dwCrc32 = ( dwCrc32 >> 8 ) ^ g_LibCrc32_ArrdwCrc32Table[ (BYTE)*pPoint ^ ( dwCrc32 & 0x000000FF ) ];
			//LibCrc32_CalcCrc32( (BYTE)*pPoint, dwCrc32 );
			pPoint ++;
		}
	}
	catch(...)
	{
	}

	dwCrc32 = ~dwCrc32;
	return dwCrc32;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



/**
 *	@ Private
 *	Init Crc32 Table
 */
VOID LibCrc32_InitCrc32Table()
{
	DWORD dwPolynomial	= 0xEDB88320;
	int   i, j		= 0;
	DWORD dwCrc		= 0;
	
	for ( i = 0; i < 256; i++ )
	{
		dwCrc = i;
		for ( j = 8; j > 0; j-- )
		{
			if ( dwCrc & 1 )
				dwCrc = ( dwCrc >> 1 ) ^ dwPolynomial;
			else
				dwCrc >>= 1;
		}
		g_LibCrc32_ArrdwCrc32Table[ i ] = dwCrc;
	}
}



/**
 *	@ Private
 *	Calc crc32 ...
 */
VOID LibCrc32_CalcCrc32( const BYTE byte, DWORD & dwCrc32 )
{
	dwCrc32 = ( ( dwCrc32 ) >> 8 ) ^ g_LibCrc32_ArrdwCrc32Table[ (byte) ^ ( (dwCrc32) & 0x000000FF) ];
}