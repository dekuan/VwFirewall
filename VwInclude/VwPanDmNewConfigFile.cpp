// DeInfoMFilterConfigFile.cpp: implementation of the CVwPanDmNewConfigFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VwPanDmNewConfigFile.h"
#include "VwEnCodeString.h"

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )

#include "DeLibDrv.h"
#pragma comment( lib, "DeLibDrv.lib" )




#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVwPanDmNewConfigFile::CVwPanDmNewConfigFile()
{
	m_bInitSucc = InitModule();
}
CVwPanDmNewConfigFile::~CVwPanDmNewConfigFile()
{

}


/**
 *	@ public
 *	装载配置信息
 */
BOOL CVwPanDmNewConfigFile::LoadConfig( STVWPANDMNEWCONFIG * pstConfig )
{
	if ( NULL == pstConfig )
	{
		return FALSE;
	}
	
	pstConfig->bStart = (BOOL)LoadConfigUInt( CVWPANDMNEWCONFIGFILE_INI_DOMAIN_MAIN, CVWPANDMNEWCONFIGFILE_INI_KEY_START );

	LoadConfigString( CVWPANDMNEWCONFIGFILE_INI_DOMAIN_LICENSE, CVWPANDMNEWCONFIGFILE_INI_KEY_REGIP, pstConfig->szRegIp, sizeof(pstConfig->szRegIp) );
	LoadConfigString( CVWPANDMNEWCONFIGFILE_INI_DOMAIN_LICENSE, CVWPANDMNEWCONFIGFILE_INI_KEY_REGHOST, pstConfig->szRegHost, sizeof(pstConfig->szRegHost) );
	LoadConfigString( CVWPANDMNEWCONFIGFILE_INI_DOMAIN_LICENSE, CVWPANDMNEWCONFIGFILE_INI_KEY_REGKEY, pstConfig->szRegKey, sizeof(pstConfig->szRegKey) );
	LoadConfigString( CVWPANDMNEWCONFIGFILE_INI_DOMAIN_LICENSE, CVWPANDMNEWCONFIGFILE_INI_KEY_REGPRTYPE, pstConfig->szRegPrType, sizeof(pstConfig->szRegPrType) );
	LoadConfigString( CVWPANDMNEWCONFIGFILE_INI_DOMAIN_LICENSE, CVWPANDMNEWCONFIGFILE_INI_KEY_REGPRTYPECS, pstConfig->szRegPrTypeChecksum, sizeof(pstConfig->szRegPrTypeChecksum) );
	pstConfig->dwRegExpireType = (DWORD)LoadConfigUInt( CVWPANDMNEWCONFIGFILE_INI_DOMAIN_LICENSE, CVWPANDMNEWCONFIGFILE_INI_KEY_REGEXPIRETYPE );
	LoadConfigString( CVWPANDMNEWCONFIGFILE_INI_DOMAIN_LICENSE, CVWPANDMNEWCONFIGFILE_INI_KEY_REGCREATEDATE, pstConfig->szRegCreateDate, sizeof(pstConfig->szRegCreateDate) );
	LoadConfigString( CVWPANDMNEWCONFIGFILE_INI_DOMAIN_LICENSE, CVWPANDMNEWCONFIGFILE_INI_KEY_REGEXPIREDATE, pstConfig->szRegExpireDate, sizeof(pstConfig->szRegExpireDate) );

	return TRUE;
}
UINT CVwPanDmNewConfigFile::LoadConfigUInt( LPCTSTR lpcszDomain, LPCTSTR lpcszKey )
{
	if ( NULL == lpcszDomain || 0 == _tcslen(lpcszDomain) )
	{
		return 0;
	}
	if ( NULL == lpcszKey || 0 == _tcslen(lpcszKey) )
	{
		return 0;
	}

	UINT uRet;

	//	...
	uRet = 0;

	delib_drv_wow64_enablewow64_fs_redirection( FALSE );

	//if ( CModuleInfo::mb_pfnGetPrivateProfileInt )
	//{
	uRet = GetPrivateProfileInt( lpcszDomain, lpcszKey, 0, m_szCfgFile );
	//}

	delib_drv_wow64_enablewow64_fs_redirection( TRUE );

	return uRet;
}
BOOL CVwPanDmNewConfigFile::LoadConfigString( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPTSTR lpszString, DWORD dwSize )
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

	BOOL bRet;

	//	...
	bRet = FALSE;

	delib_drv_wow64_enablewow64_fs_redirection( FALSE );

	//if ( CModuleInfo::mb_pfnGetPrivateProfileString )
	//{
	bRet = GetPrivateProfileString( lpcszDomain, lpcszKey, _T(""), lpszString, dwSize, m_szCfgFile );
	//}

	delib_drv_wow64_enablewow64_fs_redirection( TRUE );

	return bRet;
}
BOOL CVwPanDmNewConfigFile::SaveConfig( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPCTSTR lpcszValue )
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

	delib_drv_wow64_enablewow64_fs_redirection( FALSE );

	//if ( CModuleInfo::mb_pfnWritePrivateProfileString )
	//{
	uLen = _tcslen( lpcszValue ) + 2 + 1;
	pszTemp = new TCHAR[ uLen ];
	if ( pszTemp )
	{
		memset( pszTemp, 0, uLen );

		if ( NULL == strchr( lpcszValue, '\"' ) )
		{
			_sntprintf( pszTemp, uLen/sizeof(TCHAR)-1, _T("\"%s\""), lpcszValue );
		}
		else
		{
			_sntprintf( pszTemp, uLen/sizeof(TCHAR)-1, _T("%s"), lpcszValue );
		}

		bRet = WritePrivateProfileString( lpcszDomain, lpcszKey, pszTemp, m_szCfgFile );

		delete [] pszTemp;
		pszTemp = NULL;
	}
	//}

	delib_drv_wow64_enablewow64_fs_redirection( TRUE );


	return bRet;
}
BOOL CVwPanDmNewConfigFile::SaveConfigUInt( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, UINT uValue )
{
	BOOL bRet		= FALSE;
	TCHAR szTemp[ 32 ];

	if ( NULL == lpcszDomain || 0 == _tcslen(lpcszDomain) )
	{
		return FALSE;
	}
	if ( NULL == lpcszKey || 0 == _tcslen(lpcszKey) )
	{
		return FALSE;
	}

	delib_drv_wow64_enablewow64_fs_redirection( FALSE );

	//if ( CModuleInfo::mb_pfnWritePrivateProfileString )
	//{
	memset( szTemp, 0, sizeof(szTemp) );
	_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1, _T("%u"), uValue );

	bRet = WritePrivateProfileString( lpcszDomain, lpcszKey, szTemp, m_szCfgFile );
	//}

	delib_drv_wow64_enablewow64_fs_redirection( TRUE );
	
	return bRet;
}

//////////////////////////////////////////////////////////////////////////
//	Private




BOOL CVwPanDmNewConfigFile::InitModule()
{
	BOOL  bRet			= FALSE;
	TCHAR szSysDir[	MAX_PATH ]	= {0};


	//
	//	给类的成员变量赋初始值
	//

	memset( m_szSysDriverDir, 0, sizeof(m_szSysDriverDir) );
	memset( m_szConfDir, 0, sizeof(m_szConfDir) );
	memset( m_szLogsDir, 0, sizeof(m_szLogsDir) );

	memset( m_szCfgFile, 0, sizeof(m_szCfgFile) );
	memset( m_szKeyFile, 0, sizeof(m_szKeyFile) );
	memset( m_szSysDriverFile, 0, sizeof(m_szSysDriverFile) );
	memset( m_szSysDriverVersion, 0, sizeof(m_szSysDriverVersion) );
	
	memset( m_szLocDriverFile_wnet_x86, 0, sizeof(m_szLocDriverFile_wnet_x86) );
	memset( m_szLocDriverFile_wnet_amd64, 0, sizeof(m_szLocDriverFile_wnet_amd64) );

	memset( m_szISAPIFilterName, 0, sizeof(m_szISAPIFilterName) );
	memset( m_szISAPIFilterDesc, 0, sizeof(m_szISAPIFilterDesc) );
	memset( m_szISAPIFilterDllName, 0, sizeof(m_szISAPIFilterDllName) );
	memset( m_szISAPIFilterFile, 0, sizeof(m_szISAPIFilterFile) );

	memset( m_szDllFileDeLib, 0, sizeof(m_szDllFileDeLib) );


	//
	//	for VwPanDmNewFilter.dll
	//
	memset( m_szISAPIFilterName, 0, sizeof(m_szISAPIFilterName) );
	memcpy( m_szISAPIFilterName, g_szFilterVwPanDmNewFilterName, sizeof(g_szFilterVwPanDmNewFilterName) );
	delib_xorenc( m_szISAPIFilterName );

	memset( m_szISAPIFilterDesc, 0, sizeof(m_szISAPIFilterDesc) );
	memcpy( m_szISAPIFilterDesc, g_szFilterVwPanDmNewFilterDesc, sizeof(g_szFilterVwPanDmNewFilterDesc) );
	delib_xorenc( m_szISAPIFilterDesc );

	memset( m_szISAPIFilterDllName, 0, sizeof(m_szISAPIFilterDllName) );
	memcpy( m_szISAPIFilterDllName, g_szFilterVwPanDmNewFilterDllName, sizeof(g_szFilterVwPanDmNewFilterDllName) );
	delib_xorenc( m_szISAPIFilterDllName );

	//
	//	init ...
	//
	CModuleInfo::InitModuleInfo( NULL );

	if ( CModuleInfo::IsInitSucc() )
	{
		//	for <windows server 2003 x86>
		_sntprintf( m_szLocDriverFile_wnet_x86, sizeof(m_szLocDriverFile_wnet_x86)/sizeof(TCHAR)-1, _T("%s%s_wnet_x86.sys"), CModuleInfo::mb_szModPath, DEVIE_NAMEA );
		//	for <windows server 2003 amd64>
		_sntprintf( m_szLocDriverFile_wnet_amd64, sizeof(m_szLocDriverFile_wnet_amd64)/sizeof(TCHAR)-1, _T("%s%s_wnet_amd64.sys"), CModuleInfo::mb_szModPath, DEVIE_NAMEA );

		_sntprintf( m_szISAPIFilterFile, sizeof(m_szISAPIFilterFile)/sizeof(TCHAR)-1, _T("%s%s"), CModuleInfo::mb_szModPath, m_szISAPIFilterDllName );

		_sntprintf( m_szDllFileDeLib, sizeof(m_szDllFileDeLib)/sizeof(TCHAR)-1, _T("%s%s"), CModuleInfo::mb_szSysDir, CONST_DLLFILE_DELIB );

		if ( GetSystemDirectory( szSysDir, sizeof(szSysDir) ) )
		{
			bRet	= TRUE;

			_sntprintf( m_szSysDriverDir, sizeof(m_szSysDriverDir)/sizeof(TCHAR)-1, _T("%s\\drivers\\"), szSysDir );
			_sntprintf( m_szConfDir, sizeof(m_szConfDir)/sizeof(TCHAR)-1, _T("%s"), m_szSysDriverDir );
			_sntprintf( m_szLogsDir, sizeof(m_szLogsDir)/sizeof(TCHAR)-1, _T("%sLogs\\"), CModuleInfo::mb_szModPath );

			_sntprintf( m_szCfgFile, sizeof(m_szCfgFile)/sizeof(TCHAR)-1, _T("%s%sConfig.ini"), m_szSysDriverDir, DEVIE_NAMEA );
			_sntprintf( m_szKeyFile, sizeof(m_szKeyFile)/sizeof(TCHAR)-1, _T("%s%s.dll"), m_szSysDriverDir, DEVIE_NAMEA );
			_sntprintf( m_szSysDriverFile, sizeof(m_szSysDriverFile)/sizeof(TCHAR)-1, _T("%s%s"), m_szSysDriverDir, DRIVER_FILEA );

			//
			//	获取驱动文件版本号
			//
			delib_get_file_version( m_szSysDriverFile, m_szSysDriverVersion, sizeof(m_szSysDriverVersion) );		
		}
		
	}

	return bRet;
}
