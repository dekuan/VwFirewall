// DeIpEncode.cpp: implementation of the CDeIpEncode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeIpEncode.h"
#include <time.h>

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )


//	RandNum Table
static const TCHAR g_ArrDeIpEncodeRandNumTable[] =
{
	'1', '2', '3', '4', '5', '6', '7', '8', '9'	//, 'a', 'b', 'c', 'd', 'e', 'f'
};
static const UINT g_uArrDeIpEncodeRandNumTableLength = sizeof( g_ArrDeIpEncodeRandNumTable ) / sizeof( TCHAR );



BOOL deipencode_get_encodedid_by_ipaddress( LPCTSTR lpcszIpAddress, LPTSTR lpszEncodedId, DWORD dwSize )
{
	DWORD dwIpAddrValue;
//	WORD  wValueL, wValueH;
	DWORD dwNewValue;
	TCHAR szRandNum[ 16 ];

	if ( NULL == lpcszIpAddress || 0 == lpcszIpAddress[0] )
	{
		return FALSE;
	}
	if ( NULL == lpszEncodedId || 0 == dwSize )
	{
		return FALSE;
	}

	dwIpAddrValue = delib_get_ipaddr_value_from_string( lpcszIpAddress );
	
	//	异或加密
	delib_xorenc( (LPSTR)&dwIpAddrValue, 0xF9 );

	//	将高低位置换一下，组成新的数字
	dwNewValue = ( LOWORD( dwIpAddrValue ) << 16 ) | HIWORD( dwIpAddrValue );

	memset( szRandNum, 0, sizeof(szRandNum) );
	deipencode_get_randnum( szRandNum, sizeof(szRandNum)/sizeof(TCHAR) );
	_sntprintf( lpszEncodedId, dwSize-1, _T("%s%d"), szRandNum, dwNewValue );

	return TRUE;
}

BOOL deipencode_get_ipaddress_by_encodedid( LPCTSTR lpcszEncodedId, LPTSTR lpszIpAddress, DWORD dwSize )
{
	BOOL bRet;
	INT  nLen;
	CHAR * pszStopChar;
	DWORD dwIpAddrValue;
//	WORD  wValueL, wValueH;
	DWORD dwValue;

	if ( NULL == lpcszEncodedId || 0 == lpcszEncodedId[0] )
	{
		return FALSE;
	}
	if ( NULL == lpszIpAddress || 0 == dwSize )
	{
		return FALSE;
	}

	bRet = FALSE;
	nLen = _tcslen( lpcszEncodedId );
	if ( nLen > 8 )
	{
		pszStopChar = "";

		//	前 8 位是无效的随机数字，从第 8 位开始后面才是真正的数据
		dwValue = strtoul( lpcszEncodedId + 8, &pszStopChar, 10 );

		//
		dwIpAddrValue = ( LOWORD( dwValue ) << 16 ) | HIWORD( dwValue );

		//	异或解密
		delib_xorenc( (LPSTR)&dwIpAddrValue, 0xF9 );

		if ( delib_get_ipaddr_string_from_dword( dwIpAddrValue, lpszIpAddress, dwSize ) )
		{
			bRet = TRUE;
		}
	}

	return bRet;
}

/**
 *	获取随机数字：11111111 ~ 99999999
 */
BOOL deipencode_get_randnum( LPTSTR lpszRandNum, DWORD dwSize )
{
	//
	//	lpszRandNum	- [out] 返回
	//	dwSize		- [in]  返回值大小
	//	RETURN		- TRUE / FALSE
	//
	if ( NULL == lpszRandNum || dwSize < 8 )
	{
		return FALSE;
	}

	srand( (unsigned)time(NULL) );
	_sntprintf
	(
		lpszRandNum,
		dwSize-1,
		_T("%01c%01c%01c%01c%01c%01c%01c%01c"),
		g_ArrDeIpEncodeRandNumTable[ (UINT)( rand() % g_uArrDeIpEncodeRandNumTableLength ) ],
		g_ArrDeIpEncodeRandNumTable[ (UINT)( rand() % g_uArrDeIpEncodeRandNumTableLength ) ],
		g_ArrDeIpEncodeRandNumTable[ (UINT)( rand() % g_uArrDeIpEncodeRandNumTableLength ) ],
		g_ArrDeIpEncodeRandNumTable[ (UINT)( rand() % g_uArrDeIpEncodeRandNumTableLength ) ],
		g_ArrDeIpEncodeRandNumTable[ (UINT)( rand() % g_uArrDeIpEncodeRandNumTableLength ) ],
		g_ArrDeIpEncodeRandNumTable[ (UINT)( rand() % g_uArrDeIpEncodeRandNumTableLength ) ],
		g_ArrDeIpEncodeRandNumTable[ (UINT)( rand() % g_uArrDeIpEncodeRandNumTableLength ) ],
		g_ArrDeIpEncodeRandNumTable[ (UINT)( rand() % g_uArrDeIpEncodeRandNumTableLength ) ]
	);
	lpszRandNum[ 8 ] = 0;
	return TRUE;
}


