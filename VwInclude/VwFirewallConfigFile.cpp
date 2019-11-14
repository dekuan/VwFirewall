// DeInfoMFilterConfigFile.cpp: implementation of the CVwFirewallConfigFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VwFirewallConfigFile.h"
#include "VwEnCodeString.h"

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )

#include "DeLibDrv.h"
#pragma comment( lib, "DeLibDrv.lib" )

#include "VwIniFile.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVwFirewallConfigFile::CVwFirewallConfigFile()
{
	m_bInitSucc = InitModule();
}
CVwFirewallConfigFile::~CVwFirewallConfigFile()
{

}


/**
 *	@ public
 *	装载配置信息
 */
BOOL CVwFirewallConfigFile::LoadConfig( STVWFIREWALLCONFIG * pstConfig )
{
	if ( NULL == pstConfig )
	{
		return FALSE;
	}

	pstConfig->bStart = (BOOL)LoadConfigUInt( CVWFIREWALLCONFIGFILE_INI_DOMAIN_MAIN, CVWFIREWALLCONFIGFILE_INI_KEY_START );
	pstConfig->bStartProtectFile = (BOOL)LoadConfigUInt( CVWFIREWALLCONFIGFILE_INI_DOMAIN_MAIN, CVWFIREWALLCONFIGFILE_INI_KEY_STARTFILESAFE );
	pstConfig->bStartProtectDomain = (BOOL)LoadConfigUInt( CVWFIREWALLCONFIGFILE_INI_DOMAIN_MAIN, CVWFIREWALLCONFIGFILE_INI_KEY_STARTPDOMAIN );
	pstConfig->bStartProtectRdp = (BOOL)LoadConfigUInt( CVWFIREWALLCONFIGFILE_INI_DOMAIN_MAIN, CVWFIREWALLCONFIGFILE_INI_KEY_STARTPRDP );
	
	LoadConfigString( CVWFIREWALLCONFIGFILE_INI_DOMAIN_LICENSE, CVWFIREWALLCONFIGFILE_INI_KEY_REGIP, pstConfig->szRegIp, sizeof(pstConfig->szRegIp) );
	LoadConfigString( CVWFIREWALLCONFIGFILE_INI_DOMAIN_LICENSE, CVWFIREWALLCONFIGFILE_INI_KEY_REGHOST, pstConfig->szRegHost, sizeof(pstConfig->szRegHost) );
	LoadConfigString( CVWFIREWALLCONFIGFILE_INI_DOMAIN_LICENSE, CVWFIREWALLCONFIGFILE_INI_KEY_REGKEY, pstConfig->szRegKey, sizeof(pstConfig->szRegKey) );
	LoadConfigString( CVWFIREWALLCONFIGFILE_INI_DOMAIN_LICENSE, CVWFIREWALLCONFIGFILE_INI_KEY_REGPRTYPE, pstConfig->szRegPrType, sizeof(pstConfig->szRegPrType) );
	LoadConfigString( CVWFIREWALLCONFIGFILE_INI_DOMAIN_LICENSE, CVWFIREWALLCONFIGFILE_INI_KEY_REGPRTYPECS, pstConfig->szRegPrTypeChecksum, sizeof(pstConfig->szRegPrTypeChecksum) );
	pstConfig->dwRegExpireType = (DWORD)LoadConfigUInt( CVWFIREWALLCONFIGFILE_INI_DOMAIN_LICENSE, CVWFIREWALLCONFIGFILE_INI_KEY_REGEXPIRETYPE );
	LoadConfigString( CVWFIREWALLCONFIGFILE_INI_DOMAIN_LICENSE, CVWFIREWALLCONFIGFILE_INI_KEY_REGCREATEDATE, pstConfig->szRegCreateDate, sizeof(pstConfig->szRegCreateDate) );
	LoadConfigString( CVWFIREWALLCONFIGFILE_INI_DOMAIN_LICENSE, CVWFIREWALLCONFIGFILE_INI_KEY_REGEXPIREDATE, pstConfig->szRegExpireDate, sizeof(pstConfig->szRegExpireDate) );

	return TRUE;
}
UINT CVwFirewallConfigFile::LoadConfigUInt( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPCTSTR lpcszFilename /*= NULL*/ )
{
	if ( NULL == lpcszDomain || 0 == _tcslen(lpcszDomain) )
	{
		return 0;
	}
	if ( NULL == lpcszKey || 0 == _tcslen(lpcszKey) )
	{
		return 0;
	}

	INT nRet = 0;
	CVwIniFile cVwIniFile;

	delib_drv_wow64_enablewow64_fs_redirection( FALSE );
	if ( cVwIniFile.Create( lpcszFilename ? lpcszFilename : m_szCfgFile ) )
	{
		if ( ! cVwIniFile.GetVarInt( lpcszDomain, lpcszKey, nRet ) )
		{
			nRet = 0;
		}	
	}
	//uRet = GetPrivateProfileInt( lpcszDomain, lpcszKey, 0, m_szCfgFile );
	delib_drv_wow64_enablewow64_fs_redirection( TRUE );

	return (UINT)nRet;
}
BOOL CVwFirewallConfigFile::LoadConfigString( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPTSTR lpszString, DWORD dwSize, LPCTSTR lpcszFilename /*= NULL*/ )
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

	BOOL  bRet;
	DWORD dwRet;
	CVwIniFile cVwIniFile;
	//CString strRetVal;

	//	...
	bRet	= FALSE;
	dwRet	= 0;

	delib_drv_wow64_enablewow64_fs_redirection( FALSE );

	if ( cVwIniFile.Create( lpcszFilename ? lpcszFilename : m_szCfgFile ) )
	{
		if ( cVwIniFile.GetVarStr( lpcszDomain, lpcszKey, lpszString, dwSize ) )
		{
			dwRet	= _tcslen(lpszString);
			bRet	= TRUE;
		}
	}
	//dwRet = GetPrivateProfileString( lpcszDomain, lpcszKey, _T(""), lpszString, dwSize, m_szCfgFile );
	delib_drv_wow64_enablewow64_fs_redirection( TRUE );

	return bRet;
}
BOOL CVwFirewallConfigFile::SaveConfig( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPCTSTR lpcszValue, LPCTSTR lpcszFilename /*= NULL*/ )
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

		bRet = WritePrivateProfileString( lpcszDomain, lpcszKey, pszTemp, lpcszFilename ? lpcszFilename : m_szCfgFile );

		delete [] pszTemp;
		pszTemp = NULL;
	}

	delib_drv_wow64_enablewow64_fs_redirection( TRUE );

	return bRet;
}
BOOL CVwFirewallConfigFile::SaveConfigUInt( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, UINT uValue, LPCTSTR lpcszFilename /*= NULL*/ )
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

	memset( szTemp, 0, sizeof(szTemp) );
	_sntprintf( szTemp, sizeof(szTemp)-sizeof(TCHAR), "%u", uValue );

	bRet = WritePrivateProfileString( lpcszDomain, lpcszKey, szTemp, lpcszFilename ? lpcszFilename : m_szCfgFile );

	delib_drv_wow64_enablewow64_fs_redirection( TRUE );

	return bRet;
}





//////////////////////////////////////////////////////////////////////////
//	Private







BOOL CVwFirewallConfigFile::InitModule()
{
	BOOL  bRet			= FALSE;


	//
	//	给类的成员变量赋初始值
	//

	memset( m_szSysDriverDir, 0, sizeof(m_szSysDriverDir) );
	memset( m_szConfDir, 0, sizeof(m_szConfDir) );
	memset( m_szLogsDir, 0, sizeof(m_szLogsDir) );
	memset( m_szScriptDir, 0, sizeof(m_szScriptDir) );
	memset( m_szBackupDir, 0, sizeof(m_szBackupDir) );
	memset( m_szBackupObjectDir, 0, sizeof(m_szBackupObjectDir) );
	memset( m_szBackupServiceDir, 0, sizeof(m_szBackupServiceDir) );

	memset( m_szCfgFile, 0, sizeof(m_szCfgFile) );
	memset( m_szKeyFile, 0, sizeof(m_szKeyFile) );
	memset( m_szSysDriverFile, 0, sizeof(m_szSysDriverFile) );
	memset( m_szSysDriverVersion, 0, sizeof(m_szSysDriverVersion) );

	memset( m_szSys_Wshom_ocx, 0, sizeof(m_szSys_Wshom_ocx) );
	
	memset( m_szLocDriverFile_wnet_x86, 0, sizeof(m_szLocDriverFile_wnet_x86) );
	memset( m_szLocDriverFile_wnet_amd64, 0, sizeof(m_szLocDriverFile_wnet_amd64) );

	memset( m_szISAPIFilterName, 0, sizeof(m_szISAPIFilterName) );
	memset( m_szISAPIFilterDesc, 0, sizeof(m_szISAPIFilterDesc) );
	memset( m_szISAPIFilterDllName, 0, sizeof(m_szISAPIFilterDllName) );
	memset( m_szISAPIFilterFile, 0, sizeof(m_szISAPIFilterFile) );

	memset( m_szDllFileDeLib, 0, sizeof(m_szDllFileDeLib) );

	memset( m_szCfgAclsFileData, 0, sizeof(m_szCfgAclsFileData) );
	memset( m_szCfgAclsFolderData, 0, sizeof(m_szCfgAclsFolderData) );
	memset( m_szCfgAclsAntiVirusData, 0, sizeof(m_szCfgAclsAntiVirusData) );
	memset( m_szCfgServiceData, 0, sizeof(m_szCfgServiceData) );
	memset( m_szCfgObjectData, 0, sizeof(m_szCfgObjectData) );


	//
	//	for VwDmFirewallFilter.dll
	//
	memset( m_szISAPIFilterName, 0, sizeof(m_szISAPIFilterName) );
	memcpy( m_szISAPIFilterName, g_szFilterVwDmFirewallFilterName, sizeof(g_szFilterVwDmFirewallFilterName) );
	delib_xorenc( m_szISAPIFilterName );

	memset( m_szISAPIFilterDesc, 0, sizeof(m_szISAPIFilterDesc) );
	memcpy( m_szISAPIFilterDesc, g_szFilterVwDmFirewallFilterDesc, sizeof(g_szFilterVwDmFirewallFilterDesc) );
	delib_xorenc( m_szISAPIFilterDesc );

	memset( m_szISAPIFilterDllName, 0, sizeof(m_szISAPIFilterDllName) );
	memcpy( m_szISAPIFilterDllName, g_szFilterVwDmFirewallFilterDllName, sizeof(g_szFilterVwDmFirewallFilterDllName) );
	delib_xorenc( m_szISAPIFilterDllName );

	//
	//	init ...
	//
	CModuleInfo::InitModuleInfo( NULL );

	if ( CModuleInfo::IsInitSucc() )
	{
		bRet	= TRUE;

		//	for <windows server 2003 x86>
		_sntprintf( m_szLocDriverFile_wnet_x86, sizeof(m_szLocDriverFile_wnet_x86)/sizeof(TCHAR)-1, _T("%s%s_wnet_x86.sys"), CModuleInfo::mb_szModPath, DEVIE_NAMEA );
		//	for <windows server 2003 amd64>
		_sntprintf( m_szLocDriverFile_wnet_amd64, sizeof(m_szLocDriverFile_wnet_amd64)/sizeof(TCHAR)-1, _T("%s%s_wnet_amd64.sys"), CModuleInfo::mb_szModPath, DEVIE_NAMEA );
		_sntprintf( m_szISAPIFilterFile, sizeof(m_szISAPIFilterFile)/sizeof(TCHAR)-1, _T("%s%s"), CModuleInfo::mb_szModPath, m_szISAPIFilterDllName );

		_sntprintf( m_szDllFileDeLib, sizeof(m_szDllFileDeLib)/sizeof(TCHAR)-1, _T("%s%s"), CModuleInfo::mb_szSysDir, CONST_DLLFILE_DELIB );

		//	...
		_sntprintf( m_szCfgAclsFileData, sizeof(m_szCfgAclsFileData)/sizeof(TCHAR)-1, _T("%s%s"), CModuleInfo::mb_szModPath, _T("cfg.acls.file.data") );
		_sntprintf( m_szCfgAclsFolderData, sizeof(m_szCfgAclsFolderData)/sizeof(TCHAR)-1, _T("%s%s"), CModuleInfo::mb_szModPath, _T("cfg.acls.folder.data") );
		_sntprintf( m_szCfgAclsAntiVirusData, sizeof(m_szCfgAclsAntiVirusData)/sizeof(TCHAR)-1, _T("%s%s"), CModuleInfo::mb_szModPath, _T("cfg.acls.antivirus.data") );
		_sntprintf( m_szCfgServiceData, sizeof(m_szCfgServiceData)/sizeof(TCHAR)-1, _T("%s%s"), CModuleInfo::mb_szModPath, _T("cfg.service.data") );
		_sntprintf( m_szCfgObjectData, sizeof(m_szCfgObjectData)/sizeof(TCHAR)-1, _T("%s%s"), CModuleInfo::mb_szModPath, _T("cfg.object.data") );

		//	...
		_sntprintf( m_szSysDriverDir, sizeof(m_szSysDriverDir)/sizeof(TCHAR)-1, _T("%s\\drivers\\"), CModuleInfo::mb_szSysDir );	
		_sntprintf( m_szConfDir, sizeof(m_szConfDir)/sizeof(TCHAR)-1, _T("%s"), m_szSysDriverDir );
		_sntprintf( m_szCfgFile, sizeof(m_szCfgFile)/sizeof(TCHAR)-1, _T("%s%sConfig.ini"), m_szSysDriverDir, DEVIE_NAMEA );
		_sntprintf( m_szKeyFile, sizeof(m_szKeyFile)/sizeof(TCHAR)-1, _T("%s%s.dll"), m_szSysDriverDir, DEVIE_NAMEA );
		_sntprintf( m_szSysDriverFile, sizeof(m_szSysDriverFile)/sizeof(TCHAR)-1, _T("%s%s"), m_szSysDriverDir, DRIVER_FILEA );

		_sntprintf( m_szSys_Wshom_ocx, sizeof(m_szSys_Wshom_ocx)/sizeof(TCHAR)-1, _T("%s%s"), CModuleInfo::mb_szSysDir, _T("wshom.ocx") );

		_sntprintf( m_szLogsDir, sizeof(m_szLogsDir)/sizeof(TCHAR)-1, _T("%sLogs\\"), CModuleInfo::mb_szModPath );
		_sntprintf( m_szScriptDir, sizeof(m_szScriptDir)/sizeof(TCHAR)-1, _T("%sScript\\"), CModuleInfo::mb_szModPath );
		_sntprintf( m_szBackupDir, sizeof(m_szBackupDir)/sizeof(TCHAR)-1, _T("%sbackup\\"), CModuleInfo::mb_szModPath );
		_sntprintf( m_szBackupObjectDir, sizeof(m_szBackupObjectDir)/sizeof(TCHAR)-1, _T("%sbackup\\object\\"), CModuleInfo::mb_szModPath );
		_sntprintf( m_szBackupServiceDir, sizeof(m_szBackupServiceDir)/sizeof(TCHAR)-1, _T("%sbackup\\service\\"), CModuleInfo::mb_szModPath );
		


		//
		//	获取驱动文件版本号
		//
		delib_get_file_version( m_szSysDriverFile, m_szSysDriverVersion, sizeof(m_szSysDriverVersion) );
		
	}

	return bRet;
}
