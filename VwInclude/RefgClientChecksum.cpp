// MfClientChecksum.cpp: implementation of the CMfClientChecksum class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RefgClientChecksum.h"

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )

/**
 *	@ Public
 *	ªÒ»° MfClientChecksum
 */
BOOL refgclientchecksum_get_checksum( LPCTSTR lpcszRemoteAddr, LPCTSTR lpcszPubKey, LPTSTR lpszChecksum, DWORD dwSize )
{
	BOOL bRet;
	TCHAR szString[ 90 ];

	if ( NULL == lpcszRemoteAddr || NULL == lpcszPubKey || NULL == lpszChecksum )
	{
		return FALSE;
	}
	if ( 0 == lpcszRemoteAddr[0] || 0 == lpcszPubKey[0] )
	{
		return FALSE;
	}
	if ( 0 == dwSize )
	{
		return FALSE;
	}

	//	...
	bRet = FALSE;

	_sntprintf( szString, sizeof(szString)/sizeof(TCHAR)-1, _T("%s-mfccs-%s"), lpcszRemoteAddr, lpcszPubKey );
	StrTrim( szString, _T("\r\n\t ") );
	_tcslwr( szString );

	if ( delib_get_string_md5( szString, lpszChecksum, dwSize ) )
	{
		bRet = TRUE;
	}

	return bRet;
}