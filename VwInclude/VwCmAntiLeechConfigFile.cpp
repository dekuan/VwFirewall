// VwCmAntiLeechConfigFile.cpp: implementation of the CVwCmAntiLeechConfigFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VwCmAntiLeechConfigFile.h"
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

CVwCmAntiLeechConfigFile::CVwCmAntiLeechConfigFile()
{
	m_bInitSucc = InitModule();
}
CVwCmAntiLeechConfigFile::~CVwCmAntiLeechConfigFile()
{

}


/**
 *	@ public
 *	装载配置信息
 */
BOOL CVwCmAntiLeechConfigFile::LoadConfig( STVWCMANTILEECHCONFIG * pstConfig )
{
	if ( NULL == pstConfig )
	{
		return FALSE;
	}

	//	[Main]
	pstConfig->bRtspStart		= (BOOL)LoadConfigUInt( CVWCMANTILEECHCONFIGFILE_INI_DOMAIN_MAIN, CVWCMANTILEECHCONFIGFILE_INI_KEY_RTSP_START );
	pstConfig->bRtspBlockMmsD	= (BOOL)LoadConfigUInt( CVWCMANTILEECHCONFIGFILE_INI_DOMAIN_MAIN, CVWCMANTILEECHCONFIGFILE_INI_KEY_RTSP_BLOCK_MMSD );

	//	[License]
	LoadConfigString( CVWCMANTILEECHCONFIGFILE_INI_DOMAIN_LICENSE, CVWCMANTILEECHCONFIGFILE_INI_KEY_REGIP, pstConfig->szRegIp, sizeof(pstConfig->szRegIp) );
	LoadConfigString( CVWCMANTILEECHCONFIGFILE_INI_DOMAIN_LICENSE, CVWCMANTILEECHCONFIGFILE_INI_KEY_REGHOST, pstConfig->szRegHost, sizeof(pstConfig->szRegHost) );
	LoadConfigString( CVWCMANTILEECHCONFIGFILE_INI_DOMAIN_LICENSE, CVWCMANTILEECHCONFIGFILE_INI_KEY_REGKEY, pstConfig->szRegKey, sizeof(pstConfig->szRegKey) );
	LoadConfigString( CVWCMANTILEECHCONFIGFILE_INI_DOMAIN_LICENSE, CVWCMANTILEECHCONFIGFILE_INI_KEY_REGPRTYPE, pstConfig->szRegPrType, sizeof(pstConfig->szRegPrType) );
	LoadConfigString( CVWCMANTILEECHCONFIGFILE_INI_DOMAIN_LICENSE, CVWCMANTILEECHCONFIGFILE_INI_KEY_REGPRTYPECS, pstConfig->szRegPrTypeChecksum, sizeof(pstConfig->szRegPrTypeChecksum) );
	pstConfig->dwRegExpireType = (DWORD)LoadConfigUInt( CVWCMANTILEECHCONFIGFILE_INI_DOMAIN_LICENSE, CVWCMANTILEECHCONFIGFILE_INI_KEY_REGEXPIRETYPE );
	LoadConfigString( CVWCMANTILEECHCONFIGFILE_INI_DOMAIN_LICENSE, CVWCMANTILEECHCONFIGFILE_INI_KEY_REGCREATEDATE, pstConfig->szRegCreateDate, sizeof(pstConfig->szRegCreateDate) );
	LoadConfigString( CVWCMANTILEECHCONFIGFILE_INI_DOMAIN_LICENSE, CVWCMANTILEECHCONFIGFILE_INI_KEY_REGEXPIREDATE, pstConfig->szRegExpireDate, sizeof(pstConfig->szRegExpireDate) );

	//	[VSID]
	pstConfig->uVsidType		= (UINT)LoadConfigUInt( CVWCMANTILEECHCONFIGFILE_INI_DOMAIN_VSID, CVWCMANTILEECHCONFIGFILE_INI_KEY_VSID_TYPE, 0 );
	pstConfig->bVsidUseCookie	= (BOOL)LoadConfigUInt( CVWCMANTILEECHCONFIGFILE_INI_DOMAIN_VSID, CVWCMANTILEECHCONFIGFILE_INI_KEY_VSID_USE_COOKIE, 1 );
	LoadConfigString( CVWCMANTILEECHCONFIGFILE_INI_DOMAIN_VSID, CVWCMANTILEECHCONFIGFILE_INI_KEY_VSID_KEY, pstConfig->szVsidKey, sizeof(pstConfig->szVsidKey), "vsid" );
	pstConfig->uVsidKeyTime		= (UINT)LoadConfigUInt( CVWCMANTILEECHCONFIGFILE_INI_DOMAIN_VSID, CVWCMANTILEECHCONFIGFILE_INI_KEY_VSID_KEY_TIME, 0 );

	return TRUE;
}
UINT CVwCmAntiLeechConfigFile::LoadConfigUInt( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, INT nDefault /*= 0 */ )
{
	if ( NULL == lpcszDomain || 0 == _tcslen(lpcszDomain) )
	{
		return 0;
	}
	if ( NULL == lpcszKey || 0 == _tcslen(lpcszKey) )
	{
		return 0;
	}

	if ( CModuleInfo::mb_pfnGetPrivateProfileInt )
	{
		return CModuleInfo::mb_pfnGetPrivateProfileInt( lpcszDomain, lpcszKey, nDefault, m_szCfgFile );
	}

	return 0;
}
BOOL CVwCmAntiLeechConfigFile::LoadConfigString( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPTSTR lpszString, DWORD dwSize, LPCTSTR lpDefault /*= NULL*/ )
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
	if ( CModuleInfo::mb_pfnGetPrivateProfileString )
	{
		return CModuleInfo::mb_pfnGetPrivateProfileString
			(
				lpcszDomain,
				lpcszKey,
				( lpDefault ? lpDefault : _T("") ),
				lpszString,
				dwSize,
				m_szCfgFile
			);
	}

	return FALSE;
}
BOOL CVwCmAntiLeechConfigFile::SaveConfig( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPCTSTR lpcszValue )
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

	if ( CModuleInfo::mb_pfnWritePrivateProfileString )
	{
		uLen = _tcslen( lpcszValue ) + 2 + 1;
		pszTemp = new TCHAR[ uLen ];
		if ( pszTemp )
		{
			memset( pszTemp, 0, uLen );

			if ( NULL == strchr( lpcszValue, '\"' ) )
			{
				_sntprintf( pszTemp, uLen-sizeof(TCHAR), "\"%s\"", lpcszValue );
			}
			else
			{
				_sntprintf( pszTemp, uLen-sizeof(TCHAR), "%s", lpcszValue );
			}

			bRet = CModuleInfo::mb_pfnWritePrivateProfileString( lpcszDomain, lpcszKey, pszTemp, m_szCfgFile );

			delete [] pszTemp;
			pszTemp = NULL;
		}
	}

	return bRet;
}
BOOL CVwCmAntiLeechConfigFile::SaveConfigUInt( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, UINT uValue )
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

	if ( CModuleInfo::mb_pfnWritePrivateProfileString )
	{
		memset( szTemp, 0, sizeof(szTemp) );
		_sntprintf( szTemp, sizeof(szTemp)-sizeof(TCHAR), "%u", uValue );

		bRet = CModuleInfo::mb_pfnWritePrivateProfileString( lpcszDomain, lpcszKey, szTemp, m_szCfgFile );
	}
	
	return bRet;
}

//////////////////////////////////////////////////////////////////////////
//	Private




BOOL CVwCmAntiLeechConfigFile::InitModule()
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
	memset( m_szLocDriverFile, 0, sizeof(m_szLocDriverFile) );
	memset( m_szSysDriverVersion, 0, sizeof(m_szSysDriverVersion) );

	memset( m_szISAPIFilterName, 0, sizeof(m_szISAPIFilterName) );
	memset( m_szISAPIFilterDesc, 0, sizeof(m_szISAPIFilterDesc) );
	memset( m_szISAPIFilterDllName, 0, sizeof(m_szISAPIFilterDllName) );
	memset( m_szISAPIFilterFile, 0, sizeof(m_szISAPIFilterFile) );


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
		_sntprintf( m_szLocDriverFile, sizeof(m_szLocDriverFile)-sizeof(TCHAR), "%s%s", CModuleInfo::mb_szModPath, DRIVER_FILEA );
		_sntprintf( m_szISAPIFilterFile, sizeof(m_szISAPIFilterFile)-sizeof(TCHAR), "%s%s", CModuleInfo::mb_szModPath, m_szISAPIFilterDllName );

		if ( GetSystemDirectory( szSysDir, sizeof(szSysDir) ) )
		{
			bRet	= TRUE;

			_sntprintf( m_szSysDriverDir, sizeof(m_szSysDriverDir)-sizeof(TCHAR), "%s\\drivers\\", szSysDir );
			_sntprintf( m_szConfDir, sizeof(m_szConfDir)-sizeof(TCHAR), "%s", m_szSysDriverDir );
			_sntprintf( m_szLogsDir, sizeof(m_szLogsDir)-sizeof(TCHAR), "%sLogs\\", CModuleInfo::mb_szModPath );

			_sntprintf( m_szCfgFile, sizeof(m_szCfgFile)-sizeof(TCHAR), "%s%sConfig.ini", m_szSysDriverDir, DEVIE_NAMEA );
			_sntprintf( m_szKeyFile, sizeof(m_szKeyFile)-sizeof(TCHAR), "%s%s.dll", m_szSysDriverDir, DEVIE_NAMEA );
			_sntprintf( m_szSysDriverFile, sizeof(m_szSysDriverFile)-sizeof(TCHAR), "%s%s", m_szSysDriverDir, DRIVER_FILEA );

			//
			//	获取驱动文件版本号
			//
			delib_get_file_version( m_szSysDriverFile, m_szSysDriverVersion, sizeof(m_szSysDriverVersion) );		
		}
		
	}

	return bRet;
}
