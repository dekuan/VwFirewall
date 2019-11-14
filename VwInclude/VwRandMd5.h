// VwRandMd5.h: interface for the VwRandMd5 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWRANDMD5_HEADER__
#define __VWRANDMD5_HEADER__


#include <time.h>
#include "VwFunc.h"





/**
 *	Class Of VwRandMd5
 */
class CVwRandMd5
{
public:
	CVwRandMd5();
	~CVwRandMd5();

	BOOL IsMatch( LPCTSTR lpcszString, LPCTSTR lpcszRandMd5 );
	BOOL GetRandMd5( LPCTSTR lpcszString, LPCTSTR lpcszRand, LPTSTR lpszRandMd5, DWORD dwSize );
	BOOL GetRandNum( LPTSTR lpszRandNum, DWORD dwSize );

public:
	TCHAR m_szSeed[ 32 ];

};




#endif // __VWRANDMD5_HEADER__
