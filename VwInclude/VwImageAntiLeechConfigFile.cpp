// VwImageAntiLeechConfigFile.cpp: implementation of the CVwImageAntiLeechConfigFile class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning(disable : 4291)
#pragma warning(disable : 4786)


#include "stdafx.h"
#include "VwImageAntiLeechConfigFile.h"

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )

#include "VwIniFile.h"
//#include "Ini.h"



/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVwImageAntiLeechConfigFile::CVwImageAntiLeechConfigFile()
{
	m_bInitSucc = InitModule();
}
CVwImageAntiLeechConfigFile::~CVwImageAntiLeechConfigFile()
{
}


/**
 *	@ public
 *	装载配置信息
 */
BOOL CVwImageAntiLeechConfigFile::LoadConfig( STVWIMAGEANTILEECHCONFIG * pstConfig )
{
	if ( NULL == pstConfig )
	{
		return FALSE;
	}

	pstConfig->bStart		= (BOOL)LoadConfigUInt( CVWIMAGEANTILEECHCONFIGFILE_INI_DOMAIN_MAIN, CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_START );
//	pstConfig->bStartWordBlock	= (BOOL)LoadConfigUInt( CVWIMAGEANTILEECHCONFIGFILE_INI_DOMAIN_MAIN, CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_START_WORDBLOCK );
//	pstConfig->bStartAntiSQL	= (BOOL)LoadConfigUInt( CVWIMAGEANTILEECHCONFIGFILE_INI_DOMAIN_MAIN, CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_START_ANTISQL );
//	pstConfig->bStartWordFilter	= (BOOL)LoadConfigUInt( CVWIMAGEANTILEECHCONFIGFILE_INI_DOMAIN_MAIN, CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_START_WORDFILTER );

	pstConfig->bLog = (BOOL)LoadConfigUInt( CVWIMAGEANTILEECHCONFIGFILE_INI_DOMAIN_MAIN, CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_LOG );

	LoadConfigString( CVWIMAGEANTILEECHCONFIGFILE_INI_DOMAIN_LICENSE, CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_REGIP, pstConfig->szRegIp, sizeof(pstConfig->szRegIp) );
	LoadConfigString( CVWIMAGEANTILEECHCONFIGFILE_INI_DOMAIN_LICENSE, CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_REGHOST, pstConfig->szRegHost, sizeof(pstConfig->szRegHost) );
	LoadConfigString( CVWIMAGEANTILEECHCONFIGFILE_INI_DOMAIN_LICENSE, CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_REGKEY, pstConfig->szRegKey, sizeof(pstConfig->szRegKey) );
	pstConfig->dwRegExpireType = (DWORD)LoadConfigUInt( CVWIMAGEANTILEECHCONFIGFILE_INI_DOMAIN_LICENSE, CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_REGEXPIRETYPE );
	LoadConfigString( CVWIMAGEANTILEECHCONFIGFILE_INI_DOMAIN_LICENSE, CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_REGCREATEDATE, pstConfig->szRegCreateDate, sizeof(pstConfig->szRegCreateDate) );
	LoadConfigString( CVWIMAGEANTILEECHCONFIGFILE_INI_DOMAIN_LICENSE, CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_REGEXPIREDATE, pstConfig->szRegExpireDate, sizeof(pstConfig->szRegExpireDate) );


	return TRUE;
}
UINT CVwImageAntiLeechConfigFile::LoadConfigUInt( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPCTSTR lpcszCfgFile /*= NULL*/ )
{
	if ( NULL == lpcszDomain || 0 == _tcslen(lpcszDomain) )
	{
		return 0;
	}
	if ( NULL == lpcszKey || 0 == _tcslen(lpcszKey) )
	{
		return 0;
	}

	INT nRet;
	CVwIniFile cVwIniFile;

	if ( cVwIniFile.Create( m_szCfgFile ) )
	{
		nRet= cVwIniFile.GetMyPrivateProfileInt( lpcszDomain, lpcszKey, 0 );
	}
	//nRet = GetPrivateProfileInt( lpcszDomain, lpcszKey, 0, m_szCfgFile );

	return nRet;
}
BOOL CVwImageAntiLeechConfigFile::LoadConfigString( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPTSTR lpszString, DWORD dwSize, LPCTSTR lpcszCfgFile /*= NULL*/ )
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
	CVwIniFile cVwIniFile;

	if ( cVwIniFile.Create( lpcszCfgFile ? lpcszCfgFile : m_szCfgFile ) )
	{
		bRet = cVwIniFile.GetMyPrivateProfileString( lpcszDomain, lpcszKey, "", lpszString, dwSize );
	}

	return bRet;
	//return GetPrivateProfileString( lpcszDomain, lpcszKey, _T(""), lpszString, dwSize, m_szCfgFile );
}
BOOL CVwImageAntiLeechConfigFile::SaveConfig( STVWIMAGEANTILEECHCONFIG * pstConfig )
{
	if ( NULL == pstConfig )
	{
		return FALSE;
	}

	BOOL bSaved	= TRUE;

	bSaved &= CVwImageAntiLeechConfigFile::SaveConfig
		(
			CVWIMAGEANTILEECHCONFIGFILE_INI_DOMAIN_MAIN,
			CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_START,
			( pstConfig->bStart ? "1" : "0" )
		);

	bSaved &= CVwImageAntiLeechConfigFile::SaveConfig
		(
			CVWIMAGEANTILEECHCONFIGFILE_INI_DOMAIN_MAIN,
			CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_LOG,
			( pstConfig->bLog ? "1" : "0" )
		);

	return bSaved;
}
BOOL CVwImageAntiLeechConfigFile::SaveConfig( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPCTSTR lpcszValue )
{
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

	//if ( CModuleInfo::mb_pfnWritePrivateProfileString )
	//{
	return WritePrivateProfileString( lpcszDomain, lpcszKey, lpcszValue, m_szCfgFile );
	//}

	return FALSE;
}

BOOL CVwImageAntiLeechConfigFile::GetHostDetailIniDomainNameByHost( LPCTSTR lpcszHost, LPTSTR lpszDomainName, DWORD dwSize )
{
	if ( NULL == lpcszHost || NULL == lpszDomainName || 0 == dwSize )
	{
		return FALSE;
	}
	
	_sntprintf( lpszDomainName, dwSize-1, _T("HOST-%s"), lpcszHost );
	return TRUE;
}

BOOL CVwImageAntiLeechConfigFile::GetReplaceInfoByHostAndFiletype( LPCTSTR lpcszHost, LPCTSTR lpcszFiletype, STVWIMAGEANTILEECHREPLACE * pstReplace )
{
	BOOL  bRet;
	TCHAR szKeyName[ MAX_PATH ];
	TCHAR szSectionName[ MAX_PATH ];

	if ( NULL == lpcszHost || NULL == lpcszFiletype || NULL == pstReplace )
	{
		return FALSE;
	}

	bRet = FALSE;
	memset( szSectionName, 0, sizeof(szSectionName) );

	if ( GetHostDetailIniDomainNameByHost( lpcszHost, szSectionName, sizeof(szSectionName)/sizeof(TCHAR) ) )
	{
		delib_drv_wow64_enablewow64_fs_redirection( FALSE );

		memset( szKeyName, 0, sizeof(szKeyName) );
		_sntprintf( szKeyName, sizeof(szKeyName)/sizeof(TCHAR)-1, _T("%sIsRep"), lpcszFiletype );
		pstReplace->bIsReplace = CVwImageAntiLeechConfigFile::LoadConfigUInt( szSectionName, szKeyName );
		
		memset( szKeyName, 0, sizeof(szKeyName) );
		_sntprintf( szKeyName, sizeof(szKeyName)/sizeof(TCHAR)-1, _T("%sRepFile"), lpcszFiletype );
		memset( pstReplace->szReplaceFile, 0, sizeof(pstReplace->szReplaceFile) );
		CVwImageAntiLeechConfigFile::LoadConfigString( szSectionName, szKeyName, pstReplace->szReplaceFile, sizeof(pstReplace->szReplaceFile)/sizeof(TCHAR) );
		pstReplace->bFileExist = ( 0 != pstReplace->szReplaceFile[0] && PathFileExists( pstReplace->szReplaceFile ) );

		delib_drv_wow64_enablewow64_fs_redirection( TRUE );
		
		bRet = TRUE;
	}

	return bRet;
}
BOOL CVwImageAntiLeechConfigFile::SaveReplaceInfoByHostAndFiletype( LPCTSTR lpcszHost, LPCTSTR lpcszFiletype, STVWIMAGEANTILEECHREPLACE * pstReplace )
{
	BOOL  bRet;
	TCHAR szKeyName[ MAX_PATH ];
	TCHAR szSectionName[ MAX_PATH ];
	TCHAR szValue[ MAX_PATH ];
	
	if ( NULL == lpcszHost || NULL == lpcszFiletype || NULL == pstReplace )
	{
		return FALSE;
	}

	bRet = FALSE;
	memset( szSectionName, 0, sizeof(szSectionName) );
	
	if ( GetHostDetailIniDomainNameByHost( lpcszHost, szSectionName, sizeof(szSectionName)/sizeof(TCHAR) ) )
	{
		delib_drv_wow64_enablewow64_fs_redirection( FALSE );
		
		memset( szKeyName, 0, sizeof(szKeyName) );
		memset( szValue, 0, sizeof(szValue) );
		_sntprintf( szKeyName, sizeof(szKeyName)/sizeof(TCHAR)-1, _T("%sIsRep"), lpcszFiletype );
		_sntprintf( szValue, sizeof(szValue)/sizeof(TCHAR)-1, _T("\"%d\""), pstReplace->bIsReplace ? 1 : 0 );
		CVwImageAntiLeechConfigFile::SaveConfig( szSectionName, szKeyName, szValue );
		
		memset( szKeyName, 0, sizeof(szKeyName) );
		memset( szValue, 0, sizeof(szValue) );
		_sntprintf( szKeyName, sizeof(szKeyName)/sizeof(TCHAR)-1, _T("%sRepFile"), lpcszFiletype );
		_sntprintf( szValue, sizeof(szValue)/sizeof(TCHAR)-1, _T("\"%s\""), pstReplace->szReplaceFile );
		CVwImageAntiLeechConfigFile::SaveConfig( szSectionName, szKeyName, szValue );

		delib_drv_wow64_enablewow64_fs_redirection( TRUE );

		bRet = TRUE;
	}
	
	return bRet;
}


BOOL CVwImageAntiLeechConfigFile::GetItemInfoFilenameByHost( LPCTSTR lpcszHost, LPTSTR lpszFilename, DWORD dwSize )
{
	TCHAR szHostTemp[ MAX_PATH ];
	TCHAR szMd5[ 64 ];
	
	if ( NULL == lpcszHost || 0 == _tcslen(lpcszHost) )
	{
		return FALSE;
	}
	if ( NULL == lpszFilename || 0 == dwSize )
	{
		return FALSE;
	}
	
	memset( szHostTemp, 0, sizeof(szHostTemp) );
	_sntprintf( szHostTemp, sizeof(szHostTemp)/sizeof(TCHAR)-1, _T("%s"), lpcszHost );
	
	StrTrim( szHostTemp, _T("\r\n\t ") );
	_tcslwr( szHostTemp );
	delib_get_string_md5( szHostTemp, szMd5, sizeof(szMd5)/sizeof(TCHAR) );
	_sntprintf( lpszFilename, dwSize-1, _T("%s\\%s.ini"), m_szConfDir, szMd5 );
	
	return TRUE;
}

BOOL CVwImageAntiLeechConfigFile::ReadAllExceptedSitesFromConfig( vector<STVWIMAGEANTILEECHSITE> * pvcSiteList )
{
	BOOL bRet;
	STINISECTIONLINE stLine;
	STINISECTIONLINE * pstIniSecs;
	INT nSecsCount;
	INT i;

	TCHAR szTemp[ MAX_PATH ];
	STVWIMAGEANTILEECHSITE stSite;

	if ( NULL == pvcSiteList )
	{
		return FALSE;
	}

	//
	//	...
	//
	bRet = FALSE;
	
	//
	//	[Host]
	//	host="use:1;hst:www.chinapig.cn;prtext:8;"
	//	host="use:1;hst:www.chinameate.cn;prtext:2;"
	//
	
	delib_drv_wow64_enablewow64_fs_redirection( FALSE );

	nSecsCount = delib_ini_parse_section_lineex( m_szCfgFile, CVWIMAGEANTILEECHCONFIGFILE_INI_DOMAIN_SITE, NULL, FALSE );
	if ( nSecsCount > 0 )
	{
		pstIniSecs = new STINISECTIONLINE[ nSecsCount ];
		if ( pstIniSecs )
		{
			memset( pstIniSecs, 0, sizeof(STINISECTIONLINE)*nSecsCount );
			if ( nSecsCount ==
				delib_ini_parse_section_lineex( m_szCfgFile, CVWIMAGEANTILEECHCONFIGFILE_INI_DOMAIN_SITE, pstIniSecs, FALSE ) )
			{
				for ( i = 0; i < nSecsCount; i ++ )
				{
					//	...
					stSite.Initalize();

					delib_get_casecookie_value( pstIniSecs[ i ].szValue, CVWIMAGEANTILEECHCONFIGFILE_CFGVALNAME_HST, stSite.szHost, sizeof(stSite.szHost) );
					
					memset( szTemp, 0, sizeof(szTemp) );
					delib_get_casecookie_value( pstIniSecs[ i ].szValue, CVWIMAGEANTILEECHCONFIGFILE_CFGVALNAME_ITEMC, szTemp, sizeof(szTemp) );
					stSite.dwItemCount = (DWORD)StrToInt( szTemp );
					
					memset( szTemp, 0, sizeof(szTemp) );
					delib_get_casecookie_value( pstIniSecs[ i ].szValue, CVWIMAGEANTILEECHCONFIGFILE_CFGVALNAME_USE, szTemp, sizeof(szTemp) );
					if ( 0 == strcmp( szTemp, "1" ) )
					{
						stSite.bUse = TRUE;
					}
					else
					{
						stSite.bUse = FALSE;
					}

					//	...
					pvcSiteList->push_back( stSite );
				}
			}

			delete [] pstIniSecs;
			pstIniSecs = NULL;
		}
	}

	delib_drv_wow64_enablewow64_fs_redirection( TRUE );

	//	...
	bRet = TRUE;
	
	return bRet;
}




//////////////////////////////////////////////////////////////////////////
//	Private




BOOL CVwImageAntiLeechConfigFile::InitModule()
{
	BOOL  bRet			= FALSE;
	BOOL  bGetModFile		= FALSE;
	BOOL  bLoadCfg			= FALSE;
	TCHAR szDriver[ _MAX_DRIVE ]	= {0};
	TCHAR szDir[ MAX_PATH ]		= {0};


	//
	//	给类的成员变量赋初始值
	//

	memset( m_szConfDir, 0, sizeof(m_szConfDir) );
	memset( m_szLogsDir, 0, sizeof(m_szLogsDir) );

	memset( m_szFilterDllFile, 0, sizeof(m_szFilterDllFile) );
	memset( m_szDeLibDllFile, 0, sizeof(m_szDeLibDllFile) );

	memset( m_szFilterVersion, 0, sizeof(m_szFilterVersion) );

	memset( m_szCfgFile, 0, sizeof(m_szCfgFile) );

	memset( &m_stDictFile, 0, sizeof(m_stDictFile) );


	//	流出
	//memset( m_szOutRepWordsFile, 0, sizeof(m_szOutRepWordsFile) );
	//memset( m_szOutRepWordsFile_utf8, 0, sizeof(m_szOutRepWordsFile_utf8) );
	//memset( m_szOutBlockWordsFile, 0, sizeof(m_szOutBlockWordsFile) );

	//	流入
	//memset( m_szInRepWordsFile, 0, sizeof(m_szInRepWordsFile) );
	//memset( m_szInBlockWordsFile, 0, sizeof(m_szInBlockWordsFile) );
	//memset( m_szInBlockWordsFile_escape, 0, sizeof(m_szInBlockWordsFile_escape) );
	//memset( m_szInBlockSqlWordsFile, 0, sizeof(m_szInBlockSqlWordsFile) );

	//memset( m_szInBlockAlertFile, 0, sizeof(m_szInBlockAlertFile) );
	//memset( m_szInBlockSqlAlertFile, 0, sizeof(m_szInBlockSqlAlertFile) );

	//	例外域名
	//memset( m_szExceptionHostsFile, 0, sizeof(m_szExceptionHostsFile) );

	//
	//memset( m_szOutRepWordsDict, 0, sizeof(m_szOutRepWordsDict) );
	//memset( m_szOutRepWordsDict_utf8, 0, sizeof(m_szOutRepWordsDict_utf8) );
	//memset( m_szInBlockWordsDict, 0, sizeof(m_szInBlockWordsDict) );
	//memset( m_szInBlockWordsDict_escape, 0, sizeof(m_szInBlockWordsDict_escape) );

	//
	//	init ...
	//
	CModuleInfo::InitModuleInfo( NULL );

	if ( CModuleInfo::IsInitSucc() )
	{
		bRet	= TRUE;

		_sntprintf( m_szConfDir, sizeof(m_szConfDir)/sizeof(TCHAR)-1, "%sconf\\", CModuleInfo::mb_szModPath );
		_sntprintf( m_szLogsDir, sizeof(m_szLogsDir)/sizeof(TCHAR)-1, "%sLogs\\", CModuleInfo::mb_szModPath );

		_sntprintf( m_szFilterDllFile, sizeof(m_szFilterDllFile)/sizeof(TCHAR)-1, "%s%s", CModuleInfo::mb_szModPath, CVWIMAGEANTILEECHCONFIGFILE_FILE_FILTER );
		_sntprintf( m_szDeLibDllFile, sizeof(m_szDeLibDllFile)/sizeof(TCHAR)-1, "%s%s", CModuleInfo::mb_szSysDir, CVWIMAGEANTILEECHCONFIGFILE_FILE_DELIB );

		_sntprintf( m_szCfgFile, sizeof(m_szCfgFile)/sizeof(TCHAR)-1, "%s%s", m_szConfDir, CVWIMAGEANTILEECHCONFIGFILE_CONFFILE_CFG );

		//
		//	获取过滤器的版本
		//
		delib_get_file_version( CVwImageAntiLeechConfigFile::m_szFilterDllFile, m_szFilterVersion, sizeof(m_szFilterVersion) );		
	}

	return bRet;
}
