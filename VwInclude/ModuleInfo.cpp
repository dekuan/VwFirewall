// ModuleInfo.cpp: implementation of the CModuleInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include <Windows.h>
//#include "Winnls.h"
#include "ModuleInfo.h"

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )






/**
 *	加密数据
 */

//	"Kernel32.dll"
static CHAR g_szModuleInfo_Kernel32_dll[] =
{
	-76, -102, -115, -111, -102, -109, -52, -51, -47, -101, -109, -109, 0
};

//	"GetPrivateProfileIntW"
static CHAR g_szModuleInfo_GetPrivateProfileIntW[] =
{
	-72, -102, -117, -81, -115, -106, -119, -98, -117, -102, -81, -115, -112, -103, -106, -109, -102, -74, -111, -117, -88, 0
};

//	"GetPrivateProfileStringW"
static CHAR g_szModuleInfo_GetPrivateProfileStringW[] =
{
	-72, -102, -117, -81, -115, -106, -119, -98, -117, -102, -81, -115, -112, -103, -106, -109, -102, -84, -117, -115, -106,
	-111, -104, -88, 0
};

//	"WritePrivateProfileStringW"
static CHAR g_szModuleInfo_WritePrivateProfileStringW[] =
{
	-88, -115, -106, -117, -102, -81, -115, -106, -119, -98, -117, -102, -81, -115, -112, -103, -106, -109, -102, -84, -117,
	-115, -106, -111, -104, -88, 0
};


//	"GetPrivateProfileIntA"
static CHAR g_szModuleInfo_GetPrivateProfileIntA[] =
{
	-72, -102, -117, -81, -115, -106, -119, -98, -117, -102, -81, -115, -112, -103, -106, -109, -102, -74, -111, -117, -66, 0
};

//	"GetPrivateProfileStringA"
static CHAR g_szModuleInfo_GetPrivateProfileStringA[] =
{
	-72, -102, -117, -81, -115, -106, -119, -98, -117, -102, -81, -115, -112, -103, -106, -109, -102, -84, -117, -115, -106,
	-111, -104, -66, 0
};

//	"WritePrivateProfileStringA"
static CHAR g_szModuleInfo_WritePrivateProfileStringA[] =
{
	-88, -115, -106, -117, -102, -81, -115, -106, -119, -98, -117, -102, -81, -115, -112, -103, -106, -109, -102, -84, -117,
	-115, -106, -111, -104, -66, 0
};

/**
 *	global var
 */
static HINSTANCE g_hModuleInfoLibrary = NULL;




/**
 *	这是一个空函数，主要用来获取自己的 hInstance 和 DLL 的全路径
 */
static VOID __cmoduleinfo_global_voidfunc()
{
	return;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CModuleInfo::CModuleInfo()
{
	mb_bInstanceLive	= TRUE;
	mb_bInitSucc		= FALSE;
	mb_hDllInstance		= NULL;

	memset( mb_szModName, 0, sizeof(mb_szModName) );
	memset( mb_szModFile, 0, sizeof(mb_szModFile) );
	memset( mb_szModFileAuto, 0, sizeof(mb_szModFileAuto) );
	memset( mb_szModPath, 0, sizeof(mb_szModPath) );

	memset( mb_szSysDrive, 0, sizeof(mb_szSysDrive) );
	memset( mb_szWinDir, 0, sizeof(mb_szWinDir) );
	memset( mb_szSysDir, 0, sizeof(mb_szSysDir) );
	memset( mb_szWinTmpDir, 0, sizeof(mb_szWinTmpDir) );

	memset( mb_szSysRegeditFile, 0, sizeof(mb_szSysRegeditFile) );
	memset( mb_szSysCmdFile, 0, sizeof(mb_szSysCmdFile) );
	memset( mb_szSysRegsvr32File, 0, sizeof(mb_szSysRegsvr32File) );
	memset( mb_szSysSecEditFile, 0, sizeof(mb_szSysSecEditFile) );
	memset( mb_szSysServicesMscFile, 0, sizeof(mb_szSysServicesMscFile) );

	//
	//	装载一些函数
	//
	CHAR szLibName[ 128 ]		= {0};
	CHAR szFuncName[ 128 ]		= {0};
	mb_pfnGetPrivateProfileInt	= NULL;
	mb_pfnGetPrivateProfileString	= NULL;
	mb_pfnWritePrivateProfileString	= NULL;


	memset( szLibName, 0, sizeof(szLibName) );
	memcpy( szLibName, g_szModuleInfo_Kernel32_dll, min( sizeof(g_szModuleInfo_Kernel32_dll), sizeof(szLibName) ) );
	delib_xorenc( szLibName );

	if ( NULL == g_hModuleInfoLibrary )
	{
		g_hModuleInfoLibrary = LoadLibraryA( szLibName );
	}
	if ( g_hModuleInfoLibrary )
	{
#ifdef _UNICODE
		//	GetPrivateProfileIntW
		memset( szFuncName, 0, sizeof(szFuncName) );
		memcpy( szFuncName, g_szModuleInfo_GetPrivateProfileIntW, min( sizeof(g_szModuleInfo_GetPrivateProfileIntW), sizeof(szFuncName) ) );
		delib_xorenc( szFuncName );
		mb_pfnGetPrivateProfileInt	= (PFNGETPRIVATEPROFILEINT)GetProcAddress( g_hModuleInfoLibrary, szFuncName );

		//	GetPrivateProfileStringW
		memset( szFuncName, 0, sizeof(szFuncName) );
		memcpy( szFuncName, g_szModuleInfo_GetPrivateProfileStringW, min( sizeof(g_szModuleInfo_GetPrivateProfileStringW), sizeof(szFuncName) ) );
		delib_xorenc( szFuncName );
		mb_pfnGetPrivateProfileString	= (PFNGETPRIVATEPROFILESTRING)GetProcAddress( g_hModuleInfoLibrary, szFuncName );

		//	WritePrivateProfileStringW
		memset( szFuncName, 0, sizeof(szFuncName) );
		memcpy( szFuncName, g_szModuleInfo_WritePrivateProfileStringW, min( sizeof(g_szModuleInfo_WritePrivateProfileStringW), sizeof(szFuncName) ) );
		delib_xorenc( szFuncName );
		mb_pfnWritePrivateProfileString	= (PFNWRITEPRIVATEPROFILESTRING)GetProcAddress( g_hModuleInfoLibrary, szFuncName );
#else
		//	GetPrivateProfileIntA
		memset( szFuncName, 0, sizeof(szFuncName) );
		memcpy( szFuncName, g_szModuleInfo_GetPrivateProfileIntA, min( sizeof(g_szModuleInfo_GetPrivateProfileIntA), sizeof(szFuncName) ) );
		delib_xorenc( szFuncName );
		mb_pfnGetPrivateProfileInt	= (PFNGETPRIVATEPROFILEINT)GetProcAddress( g_hModuleInfoLibrary, szFuncName );

		//	GetPrivateProfileStringA
		memset( szFuncName, 0, sizeof(szFuncName) );
		memcpy( szFuncName, g_szModuleInfo_GetPrivateProfileStringA, min( sizeof(g_szModuleInfo_GetPrivateProfileStringA), sizeof(szFuncName) ) );
		delib_xorenc( szFuncName );
		mb_pfnGetPrivateProfileString	= (PFNGETPRIVATEPROFILESTRING)GetProcAddress( g_hModuleInfoLibrary, szFuncName );

		//	WritePrivateProfileStringA
		memset( szFuncName, 0, sizeof(szFuncName) );
		memcpy( szFuncName, g_szModuleInfo_WritePrivateProfileStringA, min( sizeof(g_szModuleInfo_WritePrivateProfileStringA), sizeof(szFuncName) ) );
		delib_xorenc( szFuncName );
		mb_pfnWritePrivateProfileString	= (PFNWRITEPRIVATEPROFILESTRING)GetProcAddress( g_hModuleInfoLibrary, szFuncName );
#endif
	}

	//
	//	初始化
	//
	mb_bInitSucc = InitPathInfo( NULL );
}

CModuleInfo::~CModuleInfo()
{
	mb_bInstanceLive = FALSE;
}


/**
 *	@ public
 *	初始化
 */
BOOL CModuleInfo::InitModuleInfo( HINSTANCE hInstance )
{
	//
	//	初始化
	//
	mb_bInitSucc = InitPathInfo( hInstance );

	return mb_bInitSucc;
}


/**
 *	@ public
 *	是否初始化成功
 */
BOOL CModuleInfo::IsInitSucc()
{
	return mb_bInitSucc;
}

/**
 *	@ public
 *	获取 RD Url
 */
BOOL CModuleInfo::GetRedirectUrl( DWORD dwID, LPTSTR lpszUrl, DWORD dwSize )
{
	TCHAR szLang[ 32 ];

	if ( NULL == lpszUrl || 0 == dwSize )
	{
		return FALSE;
	}

	if ( GetUILanguage( szLang, sizeof(szLang) ) )
	{
		_sntprintf( lpszUrl, dwSize/sizeof(TCHAR)-1, _T("http://rd.vidun.net/?id=%d&clang=%s"), dwID, szLang );
	}

	return TRUE;
}

/**
 *	@ public
 *	获取 UI 语言
 */
BOOL CModuleInfo::GetUILanguage( LPTSTR lpszLanguage, DWORD dwSize )
{
	BOOL bRet;
	LANGID lnLangID;

	if ( NULL == lpszLanguage || 0 == dwSize )
	{
		return FALSE;
	}

	//	...
	bRet = FALSE;
	lnLangID = GetUserDefaultUILanguage();
	if ( LANG_CHINESE == PRIMARYLANGID(lnLangID) && SUBLANG_CHINESE_SIMPLIFIED == SUBLANGID(lnLangID) )
	{
		//	chs
		_sntprintf( lpszLanguage, dwSize/sizeof(TCHAR)-1, _T("%s"), _T("chs") );
	}
	else if ( LANG_CHINESE == PRIMARYLANGID(lnLangID) && SUBLANG_CHINESE_TRADITIONAL == SUBLANGID(lnLangID) )
	{ 
		//	cht
		_sntprintf( lpszLanguage, dwSize/sizeof(TCHAR)-1, _T("%s"), _T("cht") );
	}
	else if ( LANG_ENGLISH == PRIMARYLANGID(lnLangID) )
	{
		//	eng
		_sntprintf( lpszLanguage, dwSize/sizeof(TCHAR)-1, _T("%s"), _T("en") );
	}
	else
	{
		//	other
		_sntprintf( lpszLanguage, dwSize/sizeof(TCHAR)-1, _T("%s"), _T("other"));
	}

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
//	Private


BOOL CModuleInfo::InitPathInfo( HINSTANCE hInstance )
{
	BOOL  bRet			= FALSE;
	BOOL  bGetModFile		= FALSE;
	BOOL  bLoadCfg			= FALSE;
	TCHAR szDriver[ _MAX_DRIVE ]	= {0};
	TCHAR szDir[ MAX_PATH ]		= {0};
	TCHAR * pszTemp;

	if ( hInstance )
	{
		mb_hDllInstance = hInstance;
		bGetModFile = GetModuleFileName( mb_hDllInstance, mb_szModFile, sizeof(mb_szModFile) );
	}
	else
	{
		//	获取本 DLL 的 Instance 和 DLL 的全路径
		bGetModFile = delib_get_modulefile_path( &__cmoduleinfo_global_voidfunc, &mb_hDllInstance, mb_szModFile, sizeof(mb_szModFile) );
	}

	if ( bGetModFile && PathFileExists( mb_szModFile ) )
	{
		//	初始化成功
		bRet	= TRUE;

		_tsplitpath( mb_szModFile, szDriver, szDir, 0, 0 );
		_sntprintf( mb_szModFileAuto, sizeof(mb_szModFileAuto)/sizeof(TCHAR)-1, _T("%s -auto"), mb_szModFile );
		_sntprintf( mb_szModPath, sizeof(mb_szModPath)/sizeof(TCHAR)-1, _T("%s%s"), szDriver, szDir );

		_sntprintf( mb_szModName, sizeof(mb_szModName)/sizeof(TCHAR)-1, _T("%s"), PathFindFileName( mb_szModFile ) );
		pszTemp = _tcschr( mb_szModName, '.' );
		if ( pszTemp )
		{
			*pszTemp = 0;
		}

		//	保证最后一个字符是 '\'
		if ( '\\' != mb_szModPath[ _tcslen(mb_szModPath) - sizeof(TCHAR) ] )
		{
			if ( _tcslen( mb_szModPath ) < ( sizeof(mb_szModPath)-sizeof(TCHAR) ) )
			{
				_tcscat( mb_szModPath, _T("\\") );
			}
		}

		//
		//	获取 Windows 目录，并且保证以 "\" 结尾
		//
		if ( GetWindowsDirectory( mb_szWinDir, sizeof(mb_szWinDir) ) )
		{
			mb_szSysDrive[ 0 ] = mb_szWinDir[ 0 ];
			mb_szSysDrive[ 1 ] = mb_szWinDir[ 1 ];
			mb_szSysDrive[ 2 ] = mb_szWinDir[ 2 ];

			//	保证最后一个字符是 '\'
			if ( '\\' != mb_szWinDir[ _tcslen(mb_szWinDir) - sizeof(TCHAR) ] )
			{
				if ( _tcslen( mb_szWinDir ) < ( sizeof(mb_szWinDir)-sizeof(TCHAR) ) )
				{
					_tcscat( mb_szWinDir, _T("\\") );
				}
			}

			//	...
			_sntprintf( mb_szSysRegeditFile, sizeof(mb_szSysRegeditFile)/sizeof(TCHAR)-1, _T("%s%s"), mb_szWinDir, _T("regedit.exe") );
		}

		//
		//	获取 system32 目录，并且保证以 "\" 结尾
		//
		if ( GetSystemDirectory( mb_szSysDir, sizeof(mb_szSysDir) ) )
		{
			//	保证最后一个字符是 '\'
			if ( '\\' != mb_szSysDir[ _tcslen(mb_szSysDir) - sizeof(TCHAR) ] )
			{
				if ( _tcslen( mb_szSysDir ) < ( sizeof(mb_szSysDir)-sizeof(TCHAR) ) )
				{
					_tcscat( mb_szSysDir, _T("\\") );
				}
			}

			_sntprintf( mb_szSysCmdFile, sizeof(mb_szSysCmdFile)/sizeof(TCHAR)-1, _T("%s%s"), mb_szSysDir, _T("cmd.exe") );
			_sntprintf( mb_szSysRegsvr32File, sizeof(mb_szSysRegsvr32File)/sizeof(TCHAR)-1, _T("%s%s"), mb_szSysDir, _T("regsvr32.exe") );
			_sntprintf( mb_szSysSecEditFile, sizeof(mb_szSysSecEditFile)/sizeof(TCHAR)-1, _T("%s%s"), mb_szSysDir, _T("secedit.exe") );
			_sntprintf( mb_szSysServicesMscFile, sizeof(mb_szSysServicesMscFile)/sizeof(TCHAR)-1, _T("%s%s"), mb_szSysDir, _T("services.msc") );
		}

		//
		//	获取 windows temp 目录，并且保证以 "\" 结尾
		//
		if ( delib_get_window_tempdir( mb_szWinTmpDir, sizeof(mb_szWinTmpDir) ) )
		{
			//	保证最后一个字符是 '\'
			if ( '\\' != mb_szWinTmpDir[ _tcslen(mb_szWinTmpDir) - sizeof(TCHAR) ] )
			{
				if ( _tcslen( mb_szWinTmpDir ) < ( sizeof(mb_szWinTmpDir)-sizeof(TCHAR) ) )
				{
					_tcscat( mb_szWinTmpDir, _T("\\") );
				}
			}
		}
	}
	
	return bRet;
}