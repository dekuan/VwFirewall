// DeInfoMFilterConfigFile.cpp: implementation of the CVwDomainAssConfigFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VwDomainAssConfigFile.h"
#include "VwEnCodeString.h"

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVwDomainAssConfigFile::CVwDomainAssConfigFile()
{
	m_bInitSucc = InitModule();
}
CVwDomainAssConfigFile::~CVwDomainAssConfigFile()
{

}


/**
 *	@ public
 *	装载配置信息
 */
BOOL CVwDomainAssConfigFile::LoadConfig()
{
	GetPrivateProfileString( CVWDOMAINASSCONFIGFILE_INI_DOMAIN_CONFIG, CVWDOMAINASSCONFIGFILE_INI_KEY_SKIN, _T("default"), m_stConfig.szSkin, sizeof(m_stConfig.szSkin), m_szCfgFile );
	GetPrivateProfileString( CVWDOMAINASSCONFIGFILE_INI_DOMAIN_CONFIG, CVWDOMAINASSCONFIGFILE_INI_KEY_LANGUAGE, _T(""), m_stConfig.szLanguage, sizeof(m_stConfig.szLanguage), m_szCfgFile );

	return TRUE;
}
UINT CVwDomainAssConfigFile::LoadConfigUInt( LPCTSTR lpcszDomain, LPCTSTR lpcszKey )
{
	if ( NULL == lpcszDomain || 0 == _tcslen(lpcszDomain) )
	{
		return 0;
	}
	if ( NULL == lpcszKey || 0 == _tcslen(lpcszKey) )
	{
		return 0;
	}

	return GetPrivateProfileInt( lpcszDomain, lpcszKey, 0, m_szCfgFile );
}
BOOL CVwDomainAssConfigFile::LoadConfigString( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPTSTR lpszString, DWORD dwSize )
{
	if ( NULL == lpcszDomain || 0 == _tcslen(lpcszDomain) )
	{
		return FALSE;
	}
	if ( NULL == lpcszKey || 0 == _tcslen(lpcszKey) )
	{
		return FALSE;
	}
	if ( NULL == lpszString || 0 == dwSize )
	{
		return FALSE;
	}

	return GetPrivateProfileString( lpcszDomain, lpcszKey, _T(""), lpszString, dwSize, m_szCfgFile );
}
BOOL CVwDomainAssConfigFile::SaveConfig( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPCTSTR lpcszValue )
{
	BOOL bRet	= FALSE;
	TCHAR * pszTemp;
	UINT uLen;

	if ( NULL == lpcszDomain || 0 == _tcslen(lpcszDomain) )
	{
		return FALSE;
	}
	if ( NULL == lpcszKey || 0 == _tcslen(lpcszKey) )
	{
		return FALSE;
	}
	if ( NULL == lpcszValue )
	{
		return FALSE;
	}

	uLen = _tcslen( lpcszValue ) + 2 + 1;
	pszTemp = new TCHAR[ uLen ];
	if ( pszTemp )
	{
		memset( pszTemp, 0, uLen );

		if ( NULL == _tcschr( lpcszValue, '\"' ) )
		{
			_sntprintf( pszTemp, uLen-1, _T("\"%s\""), lpcszValue );
		}
		else
		{
			_sntprintf( pszTemp, uLen-1, _T("%s"), lpcszValue );
		}

		bRet = WritePrivateProfileString( lpcszDomain, lpcszKey, pszTemp, m_szCfgFile );

		delete [] pszTemp;
		pszTemp = NULL;
	}

	return bRet;
}
BOOL CVwDomainAssConfigFile::SaveConfigUInt( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, UINT uValue )
{
	TCHAR szTemp[ 32 ];

	if ( NULL == lpcszDomain || 0 == _tcslen(lpcszDomain) )
	{
		return FALSE;
	}
	if ( NULL == lpcszKey || 0 == _tcslen(lpcszKey) )
	{
		return FALSE;
	}

	memset( szTemp, 0, sizeof(szTemp) );
	_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1, _T("%u"), uValue );

	return WritePrivateProfileString( lpcszDomain, lpcszKey, szTemp, m_szCfgFile );
}

HBITMAP CVwDomainAssConfigFile::LoadSkinBitmap( LPCTSTR lpctszBitmapName, LPCTSTR lpctszSkinName /*=NULL*/ )
{
	if ( NULL == lpctszBitmapName || 0 == _tcslen(lpctszBitmapName) )
	{
		return NULL;
	}

	TCHAR szFilename[ MAX_PATH ];

	_sntprintf
	(
		szFilename, sizeof(szFilename)/sizeof(TCHAR)-sizeof(TCHAR),
		_T("%s\\Skin\\%s\\%s"),
		CModuleInfo::mb_szModPath,
		( lpctszSkinName ? lpctszSkinName : m_stConfig.szSkin ),
		lpctszBitmapName
	);

	if ( PathFileExists( szFilename ) )
	{
		return (HBITMAP)::LoadImage( NULL, szFilename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION|LR_DEFAULTSIZE );
	}
	else
	{
		return NULL;
	}
}








//////////////////////////////////////////////////////////////////////////
//	Private




BOOL CVwDomainAssConfigFile::InitModule()
{
	BOOL  bRet			= FALSE;
	TCHAR szSysDir[	MAX_PATH ]	= {0};


	//
	//	给类的成员变量赋初始值
	//

	memset( m_szConfDir, 0, sizeof(m_szConfDir) );
	memset( m_szLogsDir, 0, sizeof(m_szLogsDir) );

	memset( m_szCfgFile, 0, sizeof(m_szCfgFile) );
	memset( m_szSysDriverVersion, 0, sizeof(m_szSysDriverVersion) );

	memset( m_szDllFileDeLib, 0, sizeof(m_szDllFileDeLib) );

	//
	//	init ...
	//
	CModuleInfo::InitModuleInfo( NULL );

	if ( CModuleInfo::IsInitSucc() )
	{
		_sntprintf( m_szDllFileDeLib, sizeof(m_szDllFileDeLib)/sizeof(TCHAR)-1, _T("%s%s"), CModuleInfo::mb_szSysDir, CONST_DLLFILE_DELIB );

		if ( GetSystemDirectory( szSysDir, sizeof(szSysDir) ) )
		{
			bRet	= TRUE;

			_sntprintf( m_szLogsDir, sizeof(m_szLogsDir)/sizeof(TCHAR)-1, _T("%sLogs\\"), CModuleInfo::mb_szModPath );

			_sntprintf( m_szCfgFile, sizeof(m_szCfgFile)/sizeof(TCHAR)-1, _T("%sConfig.ini"), CModuleInfo::mb_szModPath );

			//
			//	获取驱动文件版本号
			//
			delib_get_file_version( CModuleInfo::mb_szModFile, m_szSysDriverVersion, sizeof(m_szSysDriverVersion) );
		}
		
	}

	return bRet;
}
