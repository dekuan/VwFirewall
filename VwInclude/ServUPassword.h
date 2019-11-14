// ServUPass.h: interface for the ServUPass class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __SERVU_PASS_HEADER__
#define __SERVU_PASS_HEADER__



#include "md5.h"


class CServUPassword
{
public:
	CServUPassword();
	~CServUPassword();

	BOOL IsPasswordsMatch( LPCTSTR lpszPwd, LPCTSTR lpszServUPwd );
	BOOL GetServUPass( LPCTSTR lpszPwdIn, LPCTSTR lpszRand, LPTSTR lpszServUPwd, DWORD dwSPSize );
	BOOL GetRandNum( LPTSTR lpszRandNum, DWORD dwSize );

	BOOL GetStringMd5( LPCTSTR lpszString, LPTSTR lpszMd5, DWORD dwSize );
};




#endif // __SERVU_PASS_HEADER__
