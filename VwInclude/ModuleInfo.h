// ModuleInfo.h: interface for the CModuleInfo class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __MODULEINFO_HEADER__
#define __MODULEINFO_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>


//	Kernel32.dll
typedef UINT  (WINAPI * PFNGETPRIVATEPROFILEINT)( LPCTSTR lpAppName, LPCTSTR lpKeyName, INT nDefault, LPCTSTR lpFileName );
typedef DWORD (WINAPI * PFNGETPRIVATEPROFILESTRING)( LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault, LPSTR lpReturnedString, DWORD nSize, LPCSTR lpFileName );
typedef BOOL  (WINAPI * PFNWRITEPRIVATEPROFILESTRING)( LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpString, LPCTSTR lpFileName );



/**
 *	base class
 *	provide Module information
 */
class CModuleInfo
{
public:
	CModuleInfo();
	virtual ~CModuleInfo();

	BOOL InitModuleInfo( HINSTANCE hInstance );
	BOOL IsInitSucc();

	BOOL GetRedirectUrl( DWORD dwID, LPTSTR lpszUrl, DWORD dwSize );
	BOOL GetUILanguage( LPTSTR lpszLanguage, DWORD dwSize );

private:
	BOOL  InitPathInfo( HINSTANCE hInstance );

public:
	BOOL  mb_bInstanceLive;		//	证明这个类的实例是否还活着
	BOOL  mb_bInitSucc;
	HINSTANCE mb_hDllInstance;
	TCHAR mb_szModName[ 64 ];
	TCHAR mb_szModFile[ MAX_PATH ];
	TCHAR mb_szModFileAuto[ MAX_PATH ];
	TCHAR mb_szModPath[ MAX_PATH ];

	TCHAR mb_szSysDrive[ 4 ];	//	"C:\\"
	TCHAR mb_szWinDir[ MAX_PATH ];
	TCHAR mb_szSysDir[ MAX_PATH ];
	TCHAR mb_szWinTmpDir[ MAX_PATH ];

	TCHAR mb_szSysRegeditFile[ MAX_PATH ];
	TCHAR mb_szSysCmdFile[ MAX_PATH ];
	TCHAR mb_szSysRegsvr32File[ MAX_PATH ];
	TCHAR mb_szSysSecEditFile[ MAX_PATH ];
	TCHAR mb_szSysServicesMscFile[ MAX_PATH ];

	PFNGETPRIVATEPROFILEINT mb_pfnGetPrivateProfileInt;
	PFNGETPRIVATEPROFILESTRING mb_pfnGetPrivateProfileString;
	PFNWRITEPRIVATEPROFILESTRING mb_pfnWritePrivateProfileString;


};



#endif // __MODULEINFO_HEADER__



