// RefererGuardConfigFile.cpp: implementation of the CRefererGuardConfigFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RefererGuardConfigFile.h"

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )

#include "VwIniFile.h"
//#include "Ini.h"

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )

#include "DeLibDrv.h"
#pragma comment( lib, "DeLibDrv.lib" )

#ifdef APPINSTANCE_REFERERGUARD_CFG_TOOLS
	#include "commctrl.h"
	#include "atlbase.h"
#endif


//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRefererGuardConfigFile::CRefererGuardConfigFile()
{
	m_bInitSucc = InitModule();
}
CRefererGuardConfigFile::~CRefererGuardConfigFile()
{
}


/**
 *	@ public
 *	装载配置信息
 */
BOOL CRefererGuardConfigFile::LoadConfig( STREFERERGUARDCONFIG * pstConfig, LPCTSTR lpcszFilename /*= NULL*/ )
{
#ifdef APPINSTANCE_REFERERGUARDCFG
	UINT uWordMaxLen;
#endif

	if ( NULL == pstConfig )
	{
		return FALSE;
	}

	pstConfig->bStart		= (BOOL)LoadConfigUInt( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_START, lpcszFilename );
	pstConfig->bLog			= (BOOL)LoadConfigUInt( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_LOG, lpcszFilename );
	pstConfig->bUseFirewall		= (BOOL)LoadConfigUInt( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_USEFIREWALL, lpcszFilename );
	pstConfig->bEnableEcpHosts	= (BOOL)LoadConfigUInt( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_ENABLEECPHOSTS, lpcszFilename );
	LoadConfigString( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_PUBKEY, pstConfig->szPubKey, sizeof(pstConfig->szPubKey), lpcszFilename );

	pstConfig->bDenyEmptyReferer		= (BOOL)LoadConfigUInt( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_DENY_EMPTY_REFERER, lpcszFilename );
	pstConfig->bDenyEmptyRefererIsRdTo	= (BOOL)LoadConfigUInt( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_DENY_EMPTY_REFERER_IS_RDTO, lpcszFilename );
	pstConfig->dwDenyEmptyRefererRdToWait	= (DWORD)LoadConfigUInt( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_DENY_EMPTY_REFERER_RDTOWAIT, lpcszFilename );
	LoadConfigString( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_DENY_EMPTY_REFERER_RDTOURL, pstConfig->szDenyEmptyRefererRdToUrl, sizeof(pstConfig->szDenyEmptyRefererRdToUrl), lpcszFilename );

	pstConfig->bDenyAllOtherReferer		= (BOOL)LoadConfigUInt( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_DENY_ALL_OTHER_REFERER, lpcszFilename );
	pstConfig->bDenyAllOtherRefererIsRdTo	= (BOOL)LoadConfigUInt( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_DENY_ALL_OTHER_REFERER_IS_RDTO, lpcszFilename );
	pstConfig->dwDenyAllOtherRefererRdToWait= (DWORD)LoadConfigUInt( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_DENY_ALL_OTHER_REFERER_RDTOWAIT, lpcszFilename );
	LoadConfigString( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_DENY_ALL_OTHER_REFERER_RDTOURL, pstConfig->szDenyAllOtherRefererRdToUrl, sizeof(pstConfig->szDenyAllOtherRefererRdToUrl), lpcszFilename );

	pstConfig->bDenyBlackList		= (BOOL)LoadConfigUInt( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_DENY_BLACKLIST, lpcszFilename );
	pstConfig->bDenyBlackListIsRdTo		= (BOOL)LoadConfigUInt( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_DENY_BLACKLIST_IS_RDTO, lpcszFilename );
	pstConfig->dwDenyBlackListRdToWait	= (DWORD)LoadConfigUInt( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_DENY_BLACKLIST_RDTOWAIT, lpcszFilename );
	LoadConfigString( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_DENY_BLACKLIST_RDTOURL, pstConfig->szDenyBlackListRdToUrl, sizeof(pstConfig->szDenyBlackListRdToUrl), lpcszFilename );

	//	License
	LoadConfigString( CREFERERGUARDCONFIGFILE_INI_DOMAIN_LICENSE, CREFERERGUARDCONFIGFILE_INI_KEY_REGIP, pstConfig->szRegIp, sizeof(pstConfig->szRegIp), lpcszFilename );
	LoadConfigString( CREFERERGUARDCONFIGFILE_INI_DOMAIN_LICENSE, CREFERERGUARDCONFIGFILE_INI_KEY_REGHOST, pstConfig->szRegHost, sizeof(pstConfig->szRegHost), lpcszFilename );
	LoadConfigString( CREFERERGUARDCONFIGFILE_INI_DOMAIN_LICENSE, CREFERERGUARDCONFIGFILE_INI_KEY_REGKEY, pstConfig->szRegKey, sizeof(pstConfig->szRegKey), lpcszFilename );
	pstConfig->dwRegExpireType = (DWORD)LoadConfigUInt( CREFERERGUARDCONFIGFILE_INI_DOMAIN_LICENSE, CREFERERGUARDCONFIGFILE_INI_KEY_REGEXPIRETYPE, lpcszFilename );
	LoadConfigString( CREFERERGUARDCONFIGFILE_INI_DOMAIN_LICENSE, CREFERERGUARDCONFIGFILE_INI_KEY_REGCREATEDATE, pstConfig->szRegCreateDate, sizeof(pstConfig->szRegCreateDate), lpcszFilename );
	LoadConfigString( CREFERERGUARDCONFIGFILE_INI_DOMAIN_LICENSE, CREFERERGUARDCONFIGFILE_INI_KEY_REGEXPIREDATE, pstConfig->szRegExpireDate, sizeof(pstConfig->szRegExpireDate), lpcszFilename );
	LoadConfigString( CREFERERGUARDCONFIGFILE_INI_DOMAIN_LICENSE, CREFERERGUARDCONFIGFILE_INI_KEY_REGPRTYPE, pstConfig->szRegPrType, sizeof(pstConfig->szRegPrType), lpcszFilename );
	LoadConfigString( CREFERERGUARDCONFIGFILE_INI_DOMAIN_LICENSE, CREFERERGUARDCONFIGFILE_INI_KEY_REGPRTYPECHECKSUM, pstConfig->szRegPrTypeChecksum, sizeof(pstConfig->szRegPrTypeChecksum), lpcszFilename );

	//	Load Protected Hosts
	pstConfig->vcProtectedHosts.clear();
	LoadProtectedHosts( pstConfig->vcProtectedHosts, lpcszFilename );

	//	Load Excepted Hosts
	//	暂时不需要 @ 2011-12-06
	pstConfig->vcExceptedHosts.clear();
	LoadExceptedHosts( pstConfig->vcExceptedHosts );


	//	black list
#ifdef APPINSTANCE_REFERERGUARDCFG
	pstConfig->uBlackListWordMaxLen = 0;
	uWordMaxLen = 128;
	LoadWordsFromFile( m_szBlackListFile, TRUE, pstConfig->vcBlackListWithPattern, &uWordMaxLen );
	if ( uWordMaxLen > pstConfig->uBlackListWordMaxLen )
	{
		pstConfig->uBlackListWordMaxLen = uWordMaxLen;
	}

	uWordMaxLen = 128;
	LoadWordsFromFile( m_szBlackListFile, FALSE, pstConfig->vcBlackList, &uWordMaxLen );
	if ( uWordMaxLen > pstConfig->uBlackListWordMaxLen )
	{
		pstConfig->uBlackListWordMaxLen = uWordMaxLen;
	}
#endif

	return TRUE;
}

BOOL CRefererGuardConfigFile::SaveConfig( STREFERERGUARDCONFIG * pstConfig, LPCTSTR lpcszFilename /*= NULL*/ )
{
	BOOL bSaved;

	if ( NULL == pstConfig )
	{
		return FALSE;
	}

	bSaved	= TRUE;

	bSaved &= SaveConfig( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_START, ( pstConfig->bStart ? 1 : 0 ), lpcszFilename );
	bSaved &= SaveConfig( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_LOG, ( pstConfig->bLog ? 1 : 0 ), lpcszFilename );
	bSaved &= SaveConfig( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_USEFIREWALL, ( pstConfig->bUseFirewall ? 1 : 0 ), lpcszFilename );
	bSaved &= SaveConfig( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_ENABLEECPHOSTS, ( pstConfig->bEnableEcpHosts ? 1 : 0 ), lpcszFilename );
	bSaved &= SaveConfig( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_PUBKEY, pstConfig->szPubKey, lpcszFilename );

	bSaved &= SaveConfig( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_DENY_EMPTY_REFERER, ( pstConfig->bDenyEmptyReferer ? 1 : 0 ), lpcszFilename );
	bSaved &= SaveConfig( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_DENY_EMPTY_REFERER_IS_RDTO, ( pstConfig->bDenyEmptyRefererIsRdTo ? 1 : 0 ), lpcszFilename );
	bSaved &= SaveConfig( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_DENY_EMPTY_REFERER_RDTOWAIT, pstConfig->dwDenyEmptyRefererRdToWait, lpcszFilename );
	bSaved &= SaveConfig( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_DENY_EMPTY_REFERER_RDTOURL, pstConfig->szDenyEmptyRefererRdToUrl, lpcszFilename );

	bSaved &= SaveConfig( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_DENY_ALL_OTHER_REFERER, ( pstConfig->bDenyAllOtherReferer ? 1 : 0 ), lpcszFilename );
	bSaved &= SaveConfig( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_DENY_ALL_OTHER_REFERER_IS_RDTO, ( pstConfig->bDenyAllOtherRefererIsRdTo ? 1 : 0 ), lpcszFilename );
	bSaved &= SaveConfig( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_DENY_ALL_OTHER_REFERER_RDTOWAIT, pstConfig->dwDenyAllOtherRefererRdToWait, lpcszFilename );
	bSaved &= SaveConfig( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_DENY_ALL_OTHER_REFERER_RDTOURL, pstConfig->szDenyAllOtherRefererRdToUrl, lpcszFilename );
	
	bSaved &= SaveConfig( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_DENY_BLACKLIST, ( pstConfig->bDenyBlackList ? 1 : 0 ), lpcszFilename );
	bSaved &= SaveConfig( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_DENY_BLACKLIST_IS_RDTO, ( pstConfig->bDenyBlackListIsRdTo ? 1 : 0 ), lpcszFilename );
	bSaved &= SaveConfig( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_DENY_BLACKLIST_RDTOWAIT, pstConfig->dwDenyBlackListRdToWait, lpcszFilename );
	bSaved &= SaveConfig( CREFERERGUARDCONFIGFILE_INI_DOMAIN_MAIN, CREFERERGUARDCONFIGFILE_INI_KEY_DENY_BLACKLIST_RDTOURL, pstConfig->szDenyBlackListRdToUrl, lpcszFilename );

	//	...
	SaveProtectedHosts( pstConfig->vcProtectedHosts, lpcszFilename );
	SaveExceptedHosts( pstConfig->vcExceptedHosts, lpcszFilename );

	//	black list
#ifdef APPINSTANCE_REFERERGUARDCFG
	DeleteAllWordsInFile( m_szBlackListFile );
	SaveWordsToFile( m_szBlackListFile, pstConfig->vcBlackListWithPattern, 128 );
	SaveWordsToFile( m_szBlackListFile, pstConfig->vcBlackList, 128 );
#endif

	return bSaved;
}



UINT CRefererGuardConfigFile::LoadConfigUInt( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPCTSTR lpcszFilename /*= NULL*/ )
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
	
	
	delib_drv_wow64_enablewow64_fs_redirection( FALSE );
	
	if ( cVwIniFile.Create
		(
		( lpcszFilename && PathFileExists( lpcszFilename ) ) ? lpcszFilename : m_szCfgFile
		)
		)
	{
		nRet= cVwIniFile.GetMyPrivateProfileInt( lpcszDomain, lpcszKey, 0 );
	}
	//nRet = GetPrivateProfileInt( lpcszDomain, lpcszKey, 0, m_szCfgFile );
	
	delib_drv_wow64_enablewow64_fs_redirection( TRUE );
	
	return nRet;
}
BOOL CRefererGuardConfigFile::LoadConfigString( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPTSTR lpszString, DWORD dwSize, LPCTSTR lpcszFilename /*= NULL*/ )
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
	
	
	delib_drv_wow64_enablewow64_fs_redirection( FALSE );
	
	if ( cVwIniFile.Create
		(
		( lpcszFilename && PathFileExists( lpcszFilename ) ) ? lpcszFilename : m_szCfgFile
		)
		)
	{
		bRet = cVwIniFile.GetMyPrivateProfileString( lpcszDomain, lpcszKey, "", lpszString, dwSize );
	}
	
	delib_drv_wow64_enablewow64_fs_redirection( TRUE );
	
	return bRet;
	//return GetPrivateProfileString( lpcszDomain, lpcszKey, _T(""), lpszString, dwSize, m_szCfgFile );
}
BOOL CRefererGuardConfigFile::SaveConfig( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPCTSTR lpcszValue, LPCTSTR lpcszFilename /*= NULL*/ )
{
	BOOL bRet;
	TCHAR * pszTemp;
	DWORD dwValLen;

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

	//	...
	bRet = FALSE;

	delib_drv_wow64_enablewow64_fs_redirection( FALSE );

	__try
	{
		//if ( CModuleInfo::mb_pfnWritePrivateProfileString )
		//{
		dwValLen = _tcslen( lpcszValue ) + 3;
		pszTemp = new TCHAR[ dwValLen ];
		if ( pszTemp )
		{
			memset( pszTemp, 0, dwValLen );

			if ( '\"' != lpcszValue[0] )
			{
				_sntprintf( pszTemp, dwValLen/sizeof(TCHAR)-1, _T("\"%s\""), lpcszValue );
			}
			else
			{
				_sntprintf( pszTemp, dwValLen/sizeof(TCHAR)-1, _T("%s"), lpcszValue );
			}
			bRet = WritePrivateProfileString
				(
					lpcszDomain,
					lpcszKey,
					pszTemp,
					lpcszFilename ? lpcszFilename : m_szCfgFile
				);

			delete [] pszTemp;
			pszTemp = NULL;
		}
		//}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	delib_drv_wow64_enablewow64_fs_redirection( TRUE );

	return bRet;
}
BOOL CRefererGuardConfigFile::SaveConfig( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, UINT uValue, LPCTSTR lpcszFilename /*= NULL*/ )
{
	BOOL  bRet;
	TCHAR szTemp[ 32 ];

	if ( NULL == lpcszDomain || 0 == _tcslen(lpcszDomain) )
	{
		return FALSE;
	}
	if ( NULL == lpcszKey || 0 == _tcslen(lpcszKey) )
	{
		return FALSE;
	}


	bRet = FALSE;

	//	...
	delib_drv_wow64_enablewow64_fs_redirection( FALSE );

	_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1, _T("%d"), uValue );
	//if ( CModuleInfo::mb_pfnWritePrivateProfileString )
	//{
	bRet = WritePrivateProfileString
		(
			lpcszDomain,
			lpcszKey,
			szTemp,
			lpcszFilename ? lpcszFilename : m_szCfgFile
		);
	//}

	delib_drv_wow64_enablewow64_fs_redirection( TRUE );

	return bRet;
}






/**
 *	@ private
 *	该函数负责装载带有通配符(*)的例外域名
 */
BOOL CRefererGuardConfigFile::LoadWordsFromFile( LPCTSTR lpcszFilename, BOOL bWithPatternWords, vector<STREFERERGUARDWORD> & vcWordList, UINT * puWordMaxLen )
{
	//
	//	lpcszFilename		- [in] 包含词语的文件地址
	//	bWithPatternWords	- [in] 是否 必须 含有通配符词语
	//	vcWordList		- [out] 输出词列表
	//
	if ( NULL == lpcszFilename || ( ! PathFileExists( lpcszFilename ) ) )
	{
		return FALSE;
	}
	if ( NULL == puWordMaxLen )
	{
		return FALSE;
	}

	BOOL  bRet	= FALSE;
	FILE * fp;
	vector<STREFERERGUARDWORD>::iterator it;
	STREFERERGUARDWORD stWord;
	UINT uSpecWordMaxLen;
	UINT nWLen;
	BOOL bExistItem;


	delib_drv_wow64_enablewow64_fs_redirection( FALSE );

	//	要求最大词长度不超过这个
	uSpecWordMaxLen	= *puWordMaxLen;
	if ( 0 == uSpecWordMaxLen )
	{
		uSpecWordMaxLen = sizeof(stWord.szWord)/sizeof(TCHAR)-1;
	}

	//	..
	vcWordList.clear();
	
	fp = fopen( lpcszFilename, _T("r") );
	if ( fp )
	{
		bRet	= TRUE;
		
		memset( & stWord, 0, sizeof(stWord) );
		while( fgets( stWord.szWord, sizeof(stWord.szWord)/sizeof(TCHAR)-1, fp ) )
		{
			StrTrim( stWord.szWord, _T("\r\n\t ") );

			nWLen = _tcslen( stWord.szWord );
			if ( nWLen > 0 )
			{
				//	如果调用者要求限制长度
				if ( 0 == uSpecWordMaxLen || nWLen < uSpecWordMaxLen )
				{
					stWord.bWildcard = _tcschr( stWord.szWord, '*' ) ? TRUE : FALSE;
					if ( stWord.bWildcard == bWithPatternWords )
					{
						//	计算单个词的最大长度
						if ( nWLen > ( *puWordMaxLen ) )
						{
							*puWordMaxLen = nWLen;
						}

						//	按照调用者对是否含有通配符的要求，装载对应的词

						//
						//	插入 ...
						//
						bExistItem = FALSE;
						for ( it = vcWordList.begin(); it != vcWordList.end(); it ++ )
						{
							if ( 0 == _tcsicmp( (*it).szWord, stWord.szWord ) )
							{
								bExistItem = TRUE;
								break;
							}
						}
						if ( ! bExistItem )
						{
							//	将 带有通配符的域名 插入词典
							stWord.uLen = nWLen;
							vcWordList.push_back( stWord );
						}
					}
				}
			}

			memset( & stWord, 0, sizeof(stWord) );
		}
		
		//	..
		fclose( fp );
		fp = NULL;
	}

	delib_drv_wow64_enablewow64_fs_redirection( TRUE );

	return bRet;
}

BOOL CRefererGuardConfigFile::DeleteAllWordsInFile( LPCTSTR lpcszFilename )
{
	//
	//	lpcszFilename	- [in] 文件名
	//	RETURN		- [in]
	//
	if ( NULL == lpcszFilename || 0 == _tcslen(lpcszFilename) )
	{
		return FALSE;
	}
	
	FILE * fp;

	delib_drv_wow64_enablewow64_fs_redirection( FALSE );

	fp = fopen( lpcszFilename, "w" );
	if ( fp )
	{
		fclose( fp );
		fp = NULL;
	}
	
	delib_drv_wow64_enablewow64_fs_redirection( TRUE );
	
	return TRUE;
}
BOOL CRefererGuardConfigFile::SaveWordsToFile( LPCTSTR lpcszFilename, vector<STREFERERGUARDWORD> & vcWordList, UINT uWordMaxLen )
{
	//
	//	lpcszFilename	- [in] 文件名
	//	vcWordList	- [in] 返回的 vector 词列表
	//	uWordMaxLen	- [in] 最大词长度(也就是超过次长度就不保存)
	//	RETURN		- [in]
	//
	if ( NULL == lpcszFilename || 0 == _tcslen(lpcszFilename) )
	{
		return FALSE;
	}

	BOOL bRet	= FALSE;

	vector<STREFERERGUARDWORD>::iterator it;
	STREFERERGUARDWORD * pstWord;
	FILE * fp;
	TCHAR szLine[ MAX_PATH ];


	delib_drv_wow64_enablewow64_fs_redirection( FALSE );

	fp = fopen( lpcszFilename, "a" );
	if ( fp )
	{
		//	...
		bRet = TRUE;

		if ( vcWordList.size() )
		{
			for ( it = vcWordList.begin(); it != vcWordList.end(); it ++ )
			{
				pstWord = it;
				StrTrim( pstWord->szWord, _T("\r\n ") );

				if ( 0 == _tcslen(pstWord->szWord) )
				{
					continue;
				}

				//	检查词是否超过 用户要求的长度限制
				if ( uWordMaxLen && _tcslen(pstWord->szWord) > uWordMaxLen )
				{
					continue;
				}

				memset( szLine, 0, sizeof(szLine) );
				_sntprintf( szLine, sizeof(szLine)-sizeof(TCHAR), _T("%s\r\n"), pstWord->szWord );

				//	写文件行
				fwrite( szLine, sizeof(TCHAR), _tcslen(szLine), fp );
			}
		}

		fclose( fp );
		fp = NULL;
	}

	delib_drv_wow64_enablewow64_fs_redirection( TRUE );

	return bRet;
}






BOOL CRefererGuardConfigFile::LoadProtectedHosts( vector<STREFERERGUARDHOST> & vcProtectedHosts, LPCTSTR lpcszFilename /*= NULL*/, DWORD dwMaxHostCount /*= 0*/, BOOL bLoadUsedOnly /*= FALSE*/ )
{
	STINISECTIONLINE stLine;
	STINISECTIONLINE * pstIniSecs;
	INT nSecsCount;
	INT i;
	
	TCHAR szTemp[ MAX_PATH ];
	STREFERERGUARDHOST stHost;

	try
	{
		//
		//	[ProtectedHosts]
		//	protectedhosts="use:1;wcd:0;host:www.chinapig.cn;ecpage:/index.html;"
		//	protectedhosts="use:1;wcd:0;host:www.chinameate.cn;ecpage:/index.php;"
		//
		delib_drv_wow64_enablewow64_fs_redirection( FALSE );

		nSecsCount = delib_ini_parse_section_lineex( ( lpcszFilename ? lpcszFilename : m_szCfgFile ), CREFERERGUARDCONFIGFILE_INI_DOMAIN_PROTECTEDHOSTS, NULL, FALSE );
		if ( nSecsCount > 0 )
		{
			pstIniSecs = new STINISECTIONLINE[ nSecsCount ];
			if ( pstIniSecs )
			{
				memset( pstIniSecs, 0, sizeof(STINISECTIONLINE)*nSecsCount );
				if ( nSecsCount ==
					delib_ini_parse_section_lineex( ( lpcszFilename ? lpcszFilename : m_szCfgFile ), CREFERERGUARDCONFIGFILE_INI_DOMAIN_PROTECTEDHOSTS, pstIniSecs, FALSE ) )
				{
					for ( i = 0; i < nSecsCount; i ++ )
					{
						memset( & stHost, 0, sizeof(stHost) );
						
						memset( szTemp, 0, sizeof(szTemp) );
						delib_get_casecookie_value( pstIniSecs[ i ].szValue, "wcd:", szTemp, sizeof(szTemp) );
						if ( 0 == strcmp( szTemp, "1" ) )
						{
							stHost.bWildcard = TRUE;
						}
						else
						{
							stHost.bWildcard = FALSE;
						}
						delib_get_casecookie_value( pstIniSecs[ i ].szValue, "host:", stHost.szHost, sizeof(stHost.szHost) );
						StrTrim( stHost.szHost, _T("\r\n\t ") );
						_tcslwr( stHost.szHost );
						stHost.uHostLen = _tcslen( stHost.szHost );

						memset( szTemp, 0, sizeof(szTemp) );
						delib_get_casecookie_value( pstIniSecs[ i ].szValue, "use:", szTemp, sizeof(szTemp) );
						if ( 0 == strcmp( szTemp, "1" ) )
						{
							stHost.bUse = TRUE;
						}
						else
						{
							stHost.bUse = FALSE;
						}

						delib_get_casecookie_value( pstIniSecs[ i ].szValue, "ecpage:", stHost.szExceptedPage, sizeof(stHost.szExceptedPage) );
						StrTrim( stHost.szExceptedPage, _T("\r\n\t ") );
						_tcslwr( stHost.szExceptedPage );
						stHost.uEcpPageLen = _tcslen( stHost.szExceptedPage );

						//
						//	...
						//
						if ( bLoadUsedOnly )
						{
							//	用户指定仅加载有效的项
							if ( FALSE == stHost.bUse )
							{
								continue;
							}
						}
						if ( dwMaxHostCount > 0 )
						{
							//	是否超出用户指定的限制
							if ( vcProtectedHosts.size() >= dwMaxHostCount )
							{
								break;
							}
						}

						vcProtectedHosts.push_back( stHost );

					} // end of for
				}

				delete [] pstIniSecs;
				pstIniSecs = NULL;
			}
		}

		delib_drv_wow64_enablewow64_fs_redirection( TRUE );
	}
	catch ( ... )
	{
	}
	
	return TRUE;
}

BOOL CRefererGuardConfigFile::SaveProtectedHosts( vector<STREFERERGUARDHOST> & vcProtectedHosts, LPCTSTR lpcszFilename /*= NULL*/ )
{
#ifdef APPINSTANCE_REFERERGUARD_CFG_TOOLS

	BOOL bRet	= FALSE;
	UINT i;
	CString strLine;
	TCHAR szBuffer[ 65535 ];
	CSimpleArray<STREFERERGUARDHOST> aDomainList;
	STREFERERGUARDHOST stDmItem;
	vector<STREFERERGUARDHOST>::iterator it;
	STREFERERGUARDHOST * pstTemp;


	//	清理列表先
	aDomainList.RemoveAll();
	memset( szBuffer, 0, sizeof(szBuffer) );


	//
	//	循环获取所有域名
	//
	for ( it = vcProtectedHosts.begin(); vcProtectedHosts.end() != it; it ++ )
	{
		pstTemp = it;
		aDomainList.Add( *pstTemp );
	}

	//
	//	排序，以便驱动部分更快速查找
	//
	qsort
	(
		aDomainList.GetData(), 
		aDomainList.GetSize(), 
		sizeof(STREFERERGUARDHOST),
		_qsortCmpFunc_tagRefererGuardHost
	);

	for ( i = 0; i < aDomainList.GetSize(); i ++ )
	{
		//
		//	[ProtectedHosts]
		//	protectedhosts="use:1;wcd:0;host:www.chinapig.cn;ecpage:/index.html;"
		//	protectedhosts="use:1;wcd:0;host:www.chinameate.cn;ecpage:/index.php;"
		//

		strLine	= "";
		strLine += "protectedhosts";
		strLine += "=\"";
		strLine += "use:";
		strLine += ( aDomainList[i].bUse ? "1" : "0" );
		strLine += ";";

		strLine += "wcd:";
		strLine += ( aDomainList[i].bWildcard ? "1" : "0" );
		strLine += ";";

		strLine += "host:";
		strLine += aDomainList[i].szHost;
		strLine += ";";

		strLine += "ecpage:";
		strLine += ( aDomainList[i].szExceptedPage );
		strLine += ";";

		strLine += "\"\r\n";

		if ( _tcslen(szBuffer) + strLine.GetLength() + 2 < sizeof(szBuffer) )
		{
			_tcscat( szBuffer, strLine.GetBuffer(0) );
			strLine.ReleaseBuffer();
		}
		else
		{
			break;
		}
	}

	if ( _tcslen(szBuffer) + 2 < sizeof(szBuffer) )
	{
		//	AAA\r\nN
		//	0123 4 5 
		//	BBB\r\nN
		//	CCC\r\nNN
		if ( _tcslen(szBuffer) > 2 )
		{
			//	去掉最后的 \r\n
			szBuffer[ _tcslen(szBuffer) - 1 ] = NULL;
			szBuffer[ _tcslen(szBuffer) - 1 ] = NULL;
		}

		delib_drv_wow64_enablewow64_fs_redirection( FALSE );
		bRet = WritePrivateProfileSection( CREFERERGUARDCONFIGFILE_INI_DOMAIN_PROTECTEDHOSTS, szBuffer, ( lpcszFilename ? lpcszFilename : m_szCfgFile ) );
		delib_drv_wow64_enablewow64_fs_redirection( TRUE );
	}

	return bRet;
#else
	return TRUE;
#endif

}


BOOL CRefererGuardConfigFile::LoadExceptedHosts( vector<STREFERERGUARDHOST> & vcExceptedHosts, LPCTSTR lpcszFilename /*= NULL*/ )
{
	STINISECTIONLINE stLine;
	STINISECTIONLINE * pstIniSecs;
	INT nSecsCount;
	INT i;

	TCHAR szTemp[ MAX_PATH ];
	STREFERERGUARDHOST stHost;


	//
	//	[ExceptedHosts]
	//	exceptedhosts="use:1;wcd:0;host:www.chinapig.cn;"
	//	exceptedhosts="use:1;wcd:0;host:www.chinameate.cn;"
	//
	delib_drv_wow64_enablewow64_fs_redirection( FALSE );

	try
	{
		nSecsCount = delib_ini_parse_section_lineex( ( lpcszFilename ? lpcszFilename : m_szCfgFile ), CREFERERGUARDCONFIGFILE_INI_DOMAIN_EXCEPTEDHOSTS, NULL, FALSE );
		if ( nSecsCount > 0 )
		{
			pstIniSecs = new STINISECTIONLINE[ nSecsCount ];
			if ( pstIniSecs )
			{
				memset( pstIniSecs, 0, sizeof(STINISECTIONLINE)*nSecsCount );
				if ( nSecsCount ==
					delib_ini_parse_section_lineex( ( lpcszFilename ? lpcszFilename : m_szCfgFile ), CREFERERGUARDCONFIGFILE_INI_DOMAIN_EXCEPTEDHOSTS, pstIniSecs, FALSE ) )
				{
					for ( i = 0; i < nSecsCount; i ++ )
					{
						memset( & stHost, 0, sizeof(stHost) );
						
						memset( szTemp, 0, sizeof(szTemp) );
						delib_get_casecookie_value( pstIniSecs[ i ].szValue, "wcd:", szTemp, sizeof(szTemp) );
						if ( 0 == strcmp( szTemp, "1" ) )
						{
							stHost.bWildcard = TRUE;
						}
						else
						{
							stHost.bWildcard = FALSE;
						}
						delib_get_casecookie_value( pstIniSecs[ i ].szValue, "host:", stHost.szHost, sizeof(stHost.szHost) );
						StrTrim( stHost.szHost, _T("\r\n\t ") );
						_tcslwr( stHost.szHost );
						
						memset( szTemp, 0, sizeof(szTemp) );
						delib_get_casecookie_value( pstIniSecs[ i ].szValue, "use:", szTemp, sizeof(szTemp) );
						if ( 0 == strcmp( szTemp, "1" ) )
						{
							stHost.bUse = TRUE;
						}
						else
						{
							stHost.bUse = FALSE;
						}

						//	...
						vcExceptedHosts.push_back( stHost );
					}
				}

				delete [] pstIniSecs;
				pstIniSecs = NULL;
			}
		}
	}
	catch ( ... )
	{
	}

	delib_drv_wow64_enablewow64_fs_redirection( TRUE );
	
	return TRUE;
}
BOOL CRefererGuardConfigFile::SaveExceptedHosts( vector<STREFERERGUARDHOST> & vcExceptedHosts, LPCTSTR lpcszFilename /*= NULL*/ )
{
#ifdef APPINSTANCE_REFERERGUARD_CFG_TOOLS
	
	BOOL bRet	= FALSE;
	UINT i;
	CString strLine;
	TCHAR szBuffer[ 65535 ];
	CSimpleArray<STREFERERGUARDHOST> aDomainList;
	STREFERERGUARDHOST stDmItem;
	vector<STREFERERGUARDHOST>::iterator it;
	STREFERERGUARDHOST * pstTemp;


	//	清理列表先
	aDomainList.RemoveAll();
	memset( szBuffer, 0, sizeof(szBuffer) );


	//
	//	循环获取所有域名
	//
	for ( it = vcExceptedHosts.begin(); vcExceptedHosts.end() != it; it ++ )
	{
		pstTemp = it;
		aDomainList.Add( *pstTemp );
	}
	
	//
	//	排序，以便驱动部分更快速查找
	//
	qsort
	(
		aDomainList.GetData(), 
		aDomainList.GetSize(), 
		sizeof(STREFERERGUARDHOST),
		_qsortCmpFunc_tagRefererGuardHost
	);
	
	for ( i = 0; i < aDomainList.GetSize(); i ++ )
	{
		//
		//	[ExceptedHosts]
		//	exceptedhosts="use:1;wcd:0;host:www.chinapig.cn;"
		//	exceptedhosts="use:1;wcd:0;host:www.chinameate.cn;"
		//

		strLine	= "";
		strLine += "exceptedhosts";
		strLine += "=\"";
		strLine += "use:";
		strLine += ( aDomainList[i].bUse ? "1" : "0" );
		strLine += ";";
		
		strLine += "wcd:";
		strLine += ( aDomainList[i].bWildcard ? "1" : "0" );
		strLine += ";";

		strLine += "host:";
		strLine += aDomainList[i].szHost;
		strLine += ";";
		
		strLine += "\"\r\n";
		
		if ( _tcslen(szBuffer) + strLine.GetLength() + 2 < sizeof(szBuffer) )
		{
			_tcscat( szBuffer, strLine.GetBuffer(0) );
			strLine.ReleaseBuffer();
		}
		else
		{
			break;
		}
	}
	
	if ( _tcslen(szBuffer) + 2 < sizeof(szBuffer) )
	{
		//	AAA\r\nN
		//	0123 4 5 
		//	BBB\r\nN
		//	CCC\r\nNN
		if ( _tcslen(szBuffer) > 2 )
		{
			//	去掉最后的 \r\n
			szBuffer[ _tcslen(szBuffer) - 1 ] = NULL;
			szBuffer[ _tcslen(szBuffer) - 1 ] = NULL;
		}
		
		delib_drv_wow64_enablewow64_fs_redirection( FALSE );
		bRet = WritePrivateProfileSection( CREFERERGUARDCONFIGFILE_INI_DOMAIN_EXCEPTEDHOSTS, szBuffer, ( lpcszFilename ? lpcszFilename : m_szCfgFile ) );
		delib_drv_wow64_enablewow64_fs_redirection( TRUE );
	}
	
	return bRet;
#else
	return TRUE;
#endif
}







//////////////////////////////////////////////////////////////////////////
//	Private




BOOL CRefererGuardConfigFile::InitModule()
{
	BOOL  bRet			= FALSE;
	BOOL  bGetModFile		= FALSE;
	BOOL  bLoadCfg			= FALSE;
	TCHAR szDriver[ _MAX_DRIVE ]	= {0};
	TCHAR szDir[ MAX_PATH ]		= {0};
	
	
	//
	//	给类的成员变量赋初始值
	//
	memset( m_szLogsDir, 0, sizeof(m_szLogsDir) );
	
	memset( m_szFilterMainDllFile, 0, sizeof(m_szFilterMainDllFile) );
	memset( m_szFilterClientDllFile, 0, sizeof(m_szFilterClientDllFile) );
	memset( m_szDeLibDllFile, 0, sizeof(m_szDeLibDllFile) );
	memset( m_szDeLibDrvDllFile, 0, sizeof(m_szDeLibDrvDllFile) );
	
	memset( m_szFilterMainVersion, 0, sizeof(m_szFilterMainVersion) );
	memset( m_szFilterClientVersion, 0, sizeof(m_szFilterClientVersion) );
	
	memset( m_szCfgFile, 0, sizeof(m_szCfgFile) );
	memset( m_szCfgFileNameOnly, 0, sizeof(m_szCfgFileNameOnly) );
	memset( m_szTickFile, 0, sizeof(m_szTickFile) );
	memset( m_szExceptionHostsFile, 0, sizeof(m_szExceptionHostsFile) );
	memset( m_szBlackListFile, 0, sizeof(m_szBlackListFile) );
	
	//
	//	init ...
	//
	CModuleInfo::InitModuleInfo( NULL );
	
	if ( CModuleInfo::IsInitSucc() )
	{
		bRet	= TRUE;
		
		_sntprintf( m_szLogsDir, sizeof(m_szLogsDir)/sizeof(TCHAR)-1, _T("%sLogs\\"), CModuleInfo::mb_szModPath );
		
		_sntprintf( m_szFilterMainDllFile, sizeof(m_szFilterMainDllFile)/sizeof(TCHAR)-1, _T("%s%s"), CModuleInfo::mb_szModPath, CREFERERGUARDCONFIGFILE_FILE_FILTER_MAIN );
		_sntprintf( m_szFilterClientDllFile, sizeof(m_szFilterClientDllFile)/sizeof(TCHAR)-1, _T("%s%s"), CModuleInfo::mb_szModPath, CREFERERGUARDCONFIGFILE_FILE_FILTER_CLIENT );
		_sntprintf( m_szDeLibDllFile, sizeof(m_szDeLibDllFile)/sizeof(TCHAR)-1, _T("%s%s"), CModuleInfo::mb_szSysDir, CREFERERGUARDCONFIGFILE_FILE_DELIB );
		_sntprintf( m_szDeLibDrvDllFile, sizeof(m_szDeLibDrvDllFile)/sizeof(TCHAR)-1, _T("%s%s"), CModuleInfo::mb_szSysDir, CREFERERGUARDCONFIGFILE_FILE_DELIBDRV );
		_sntprintf( m_szDeLibFiInsDllFile, sizeof(m_szDeLibFiInsDllFile)/sizeof(TCHAR)-1, _T("%s%s"), CModuleInfo::mb_szSysDir, CREFERERGUARDCONFIGFILE_FILE_DELIBFIINS );

	#ifdef APPINSTANCE_REFERERGUARDCFG
		//	RefererGuardCfg
		_sntprintf( m_szCfgFile, sizeof(m_szCfgFile)/sizeof(TCHAR)-1, _T("%s%s.ini"), CModuleInfo::mb_szModPath, REFERERGUARD_FILTER_NAME );
		_sntprintf( m_szTickFile, sizeof(m_szTickFile)/sizeof(TCHAR)-1, _T("%s%s.%s"), CModuleInfo::mb_szModPath, REFERERGUARD_FILTER_NAME, CREFERERGUARDCONFIGFILE_CONFFILE_TICK );
	#else
		#ifdef APPINSTANCE_REFGCLIENTCFG
		//	RefgClientCfg
		_sntprintf( m_szCfgFile, sizeof(m_szCfgFile)/sizeof(TCHAR)-1, _T("%s%s.ini"), CModuleInfo::mb_szModPath, REFGCLIENT_FILTER_NAME );
		_sntprintf( m_szTickFile, sizeof(m_szTickFile)/sizeof(TCHAR)-1, _T("%s%s.%s"), CModuleInfo::mb_szModPath, REFGCLIENT_FILTER_NAME, CREFERERGUARDCONFIGFILE_CONFFILE_TICK );		
		#else
		//	for RefererGuard.dll and RefgClient.dll
		_sntprintf( m_szCfgFileNameOnly, sizeof(m_szCfgFileNameOnly)/sizeof(TCHAR)-1, _T("%s.ini"), mb_szModName );
		_sntprintf( m_szCfgFile, sizeof(m_szCfgFile)/sizeof(TCHAR)-1, _T("%s%s"), CModuleInfo::mb_szModPath, m_szCfgFileNameOnly );
		_sntprintf( m_szTickFile, sizeof(m_szTickFile)/sizeof(TCHAR)-1, _T("%s%s.tick.ini"), CModuleInfo::mb_szModPath, mb_szModName );
		#endif
	#endif
		
		_sntprintf( m_szExceptionHostsFile, sizeof(m_szExceptionHostsFile)/sizeof(TCHAR)-1, _T("%s%s"), CModuleInfo::mb_szModPath, CREFERERGUARDCONFIGFILE_CONFFILE_ECPHOST );
		_sntprintf( m_szBlackListFile, sizeof(m_szBlackListFile)/sizeof(TCHAR)-1, _T("%s%s"), CModuleInfo::mb_szModPath, CREFERERGUARDCONFIGFILE_CONFFILE_BLACKLIST );
		
		//
		//	获取过滤器的版本
		//
		delib_get_file_version( CRefererGuardConfigFile::m_szFilterMainDllFile, m_szFilterMainVersion, sizeof(m_szFilterMainVersion) );
		delib_get_file_version( CRefererGuardConfigFile::m_szFilterClientDllFile, m_szFilterClientVersion, sizeof(m_szFilterClientVersion) );
	}
	
	return bRet;
}