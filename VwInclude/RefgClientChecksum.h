// RefgClientChecksum.h: interface for the CMfClientChecksum class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __REFGCLIENTCHECKSUM_HEADER__
#define __REFGCLIENTCHECKSUM_HEADER__

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000






//	...
BOOL refgclientchecksum_get_checksum( LPCTSTR lpcszRemoteAddr, LPCTSTR lpcszPubKey, LPTSTR lpszChecksum, DWORD dwSize );




#endif // __REFGCLIENTCHECKSUM_HEADER__


