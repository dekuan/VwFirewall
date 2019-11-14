// DeIpEncode.h: interface for the CDeIpEncode class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DEIPENCODE_HEADER__
#define __DEIPENCODE_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>




BOOL deipencode_get_encodedid_by_ipaddress( LPCTSTR lpcszIpAddress, LPTSTR lpszEncodedId, DWORD dwSize );
BOOL deipencode_get_ipaddress_by_encodedid( LPCTSTR lpcszEncodedId, LPTSTR lpszIpAddress, DWORD dwSize );
BOOL deipencode_get_randnum( LPTSTR lpszRandNum, DWORD dwSize );






#endif // __DEIPENCODE_HEADER__
