// DeInfoMFilterConfigFile.cpp: implementation of the CVwInfoMonitorConfigFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VwInfoMonitorConfigFile.h"

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )

#include "VwIniFile.h"
//#include "Ini.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVwInfoMonitorConfigFile::CVwInfoMonitorConfigFile()
{
	m_bInitSucc = InitModule();
}
CVwInfoMonitorConfigFile::~CVwInfoMonitorConfigFile()
{
}


/**
 *	@ public
 *	装载配置信息
 */
BOOL CVwInfoMonitorConfigFile::LoadConfig( STVWINFOMCONFIG * pstConfig )
{
	if ( NULL == pstConfig )
	{
		return FALSE;
	}

	pstConfig->bStart		= (BOOL)LoadConfigUInt( CVWINFOMONITORCONFIGFILE_INI_DOMAIN_MAIN, CVWINFOMONITORCONFIGFILE_INI_KEY_START );
	pstConfig->bStartWordBlock	= (BOOL)LoadConfigUInt( CVWINFOMONITORCONFIGFILE_INI_DOMAIN_MAIN, CVWINFOMONITORCONFIGFILE_INI_KEY_START_WORDBLOCK );
	pstConfig->bStartAntiSQL	= (BOOL)LoadConfigUInt( CVWINFOMONITORCONFIGFILE_INI_DOMAIN_MAIN, CVWINFOMONITORCONFIGFILE_INI_KEY_START_ANTISQL );
	pstConfig->bStartWordFilter	= (BOOL)LoadConfigUInt( CVWINFOMONITORCONFIGFILE_INI_DOMAIN_MAIN, CVWINFOMONITORCONFIGFILE_INI_KEY_START_WORDFILTER );

	pstConfig->bLog = (BOOL)LoadConfigUInt( CVWINFOMONITORCONFIGFILE_INI_DOMAIN_MAIN, CVWINFOMONITORCONFIGFILE_INI_KEY_LOG );

	LoadConfigString( CVWINFOMONITORCONFIGFILE_INI_DOMAIN_LICENSE, CVWINFOMONITORCONFIGFILE_INI_KEY_REGIP, pstConfig->szRegIp, sizeof(pstConfig->szRegIp) );
	LoadConfigString( CVWINFOMONITORCONFIGFILE_INI_DOMAIN_LICENSE, CVWINFOMONITORCONFIGFILE_INI_KEY_REGHOST, pstConfig->szRegHost, sizeof(pstConfig->szRegHost) );
	LoadConfigString( CVWINFOMONITORCONFIGFILE_INI_DOMAIN_LICENSE, CVWINFOMONITORCONFIGFILE_INI_KEY_REGKEY, pstConfig->szRegKey, sizeof(pstConfig->szRegKey) );
	pstConfig->dwRegExpireType = (DWORD)LoadConfigUInt( CVWINFOMONITORCONFIGFILE_INI_DOMAIN_LICENSE, CVWINFOMONITORCONFIGFILE_INI_KEY_REGEXPIRETYPE );
	LoadConfigString( CVWINFOMONITORCONFIGFILE_INI_DOMAIN_LICENSE, CVWINFOMONITORCONFIGFILE_INI_KEY_REGCREATEDATE, pstConfig->szRegCreateDate, sizeof(pstConfig->szRegCreateDate) );
	LoadConfigString( CVWINFOMONITORCONFIGFILE_INI_DOMAIN_LICENSE, CVWINFOMONITORCONFIGFILE_INI_KEY_REGEXPIREDATE, pstConfig->szRegExpireDate, sizeof(pstConfig->szRegExpireDate) );

	pstConfig->bBlockPost = (BOOL)LoadConfigUInt( CVWINFOMONITORCONFIGFILE_INI_DOMAIN_BLOCK, CVWINFOMONITORCONFIGFILE_INI_KEY_BLOCKPOST );
	pstConfig->bBlockPostSize = (BOOL)LoadConfigUInt( CVWINFOMONITORCONFIGFILE_INI_DOMAIN_BLOCK, CVWINFOMONITORCONFIGFILE_INI_KEY_BLOCKPOSTSIZE );
	pstConfig->dwBlockPostSizeCfg = (DWORD)LoadConfigUInt( CVWINFOMONITORCONFIGFILE_INI_DOMAIN_BLOCK, CVWINFOMONITORCONFIGFILE_INI_KEY_BLOCKPOSTSIZECFG );

	return TRUE;
}
UINT CVwInfoMonitorConfigFile::LoadConfigUInt( LPCTSTR lpcszDomain, LPCTSTR lpcszKey )
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
BOOL CVwInfoMonitorConfigFile::LoadConfigString( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPTSTR lpszString, DWORD dwSize )
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

	if ( cVwIniFile.Create( m_szCfgFile ) )
	{
		bRet = cVwIniFile.GetMyPrivateProfileString( lpcszDomain, lpcszKey, "", lpszString, dwSize );
	}

	return bRet;
	//return GetPrivateProfileString( lpcszDomain, lpcszKey, _T(""), lpszString, dwSize, m_szCfgFile );
}
BOOL CVwInfoMonitorConfigFile::SaveConfig( STVWINFOMCONFIG * pstConfig )
{
	if ( NULL == pstConfig )
	{
		return FALSE;
	}

	BOOL bSaved	= TRUE;

	bSaved &= CVwInfoMonitorConfigFile::SaveConfig
		(
			CVWINFOMONITORCONFIGFILE_INI_DOMAIN_MAIN,
			CVWINFOMONITORCONFIGFILE_INI_KEY_START,
			( pstConfig->bStart ? "1" : "0" )
		);

	bSaved &= CVwInfoMonitorConfigFile::SaveConfig
		(
			CVWINFOMONITORCONFIGFILE_INI_DOMAIN_MAIN,
			CVWINFOMONITORCONFIGFILE_INI_KEY_START_WORDBLOCK,
			( pstConfig->bStartWordBlock ? "1" : "0" )
		);

	CVwInfoMonitorConfigFile::SaveConfig
		(
			CVWINFOMONITORCONFIGFILE_INI_DOMAIN_BLOCK,
			CVWINFOMONITORCONFIGFILE_INI_KEY_BLOCKPOST,
			( pstConfig->bBlockPost ? "1" : "0" )
		);

	bSaved &= CVwInfoMonitorConfigFile::SaveConfig
		(
			CVWINFOMONITORCONFIGFILE_INI_DOMAIN_MAIN,
			CVWINFOMONITORCONFIGFILE_INI_KEY_START_ANTISQL,
			( pstConfig->bStartAntiSQL ? "1" : "0" )
		);

	bSaved &= CVwInfoMonitorConfigFile::SaveConfig
		(
			CVWINFOMONITORCONFIGFILE_INI_DOMAIN_MAIN,
			CVWINFOMONITORCONFIGFILE_INI_KEY_START_WORDFILTER,
			( pstConfig->bStartWordFilter ? "1" : "0" )
		);

	bSaved &= CVwInfoMonitorConfigFile::SaveConfig
		(
			CVWINFOMONITORCONFIGFILE_INI_DOMAIN_MAIN,
			CVWINFOMONITORCONFIGFILE_INI_KEY_LOG,
			( pstConfig->bLog ? "1" : "0" )
		);

	return bSaved;
}
BOOL CVwInfoMonitorConfigFile::SaveConfig( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPCTSTR lpcszValue )
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


/**
 *	@ public
 *	装载/保存 in block 词
 */
BOOL CVwInfoMonitorConfigFile::LoadInBlockWords( vector<STVWINFOMWORD> & vcWordList, UINT * puWordMaxLen )
{
	return LoadWordsFromFile( m_stDictFile.szInBlockWordsFile, 0, vcWordList, puWordMaxLen );
}
BOOL CVwInfoMonitorConfigFile::SaveInBlockWords( vector<STVWINFOMWORD> & vcWordList, UINT uWordMaxLen )
{
	return SaveWordsFromFile( m_stDictFile.szInBlockWordsFile, vcWordList, uWordMaxLen );
}

BOOL CVwInfoMonitorConfigFile::LoadInBlockWords_escape( vector<STVWINFOMWORD> & vcWordList, UINT * puWordMaxLen )
{
	return LoadWordsFromFile( m_stDictFile.szInBlockWordsFile_escape, 0, vcWordList, puWordMaxLen );
}
BOOL CVwInfoMonitorConfigFile::SaveInBlockWords_escape( vector<STVWINFOMWORD> & vcWordList, UINT uWordMaxLen )
{
	return SaveWordsFromFile( m_stDictFile.szInBlockWordsFile_escape, vcWordList, uWordMaxLen );
}

BOOL CVwInfoMonitorConfigFile::LoadInBlockWords_utf8( vector<STVWINFOMWORD> & vcWordList, UINT * puWordMaxLen )
{
	return LoadWordsFromFile( m_stDictFile.szInBlockWordsFile_utf8, 0, vcWordList, puWordMaxLen );
}
BOOL CVwInfoMonitorConfigFile::SaveInBlockWords_utf8( vector<STVWINFOMWORD> & vcWordList, UINT uWordMaxLen )
{
	return SaveWordsFromFile( m_stDictFile.szInBlockWordsFile_utf8, vcWordList, uWordMaxLen );
}


/**
 *	@ public
 *	装载/保存 in anti-sql 词
 */
BOOL CVwInfoMonitorConfigFile::LoadInAntiSqlWords( vector<STVWINFOMWORD> & vcWordList, UINT * puWordMaxLen )
{
	return LoadWordsFromFile( m_stDictFile.szInBlockSqlWordsFile, 0, vcWordList, puWordMaxLen );
}
BOOL CVwInfoMonitorConfigFile::SaveInAntiSqlWords( vector<STVWINFOMWORD> & vcWordList, UINT uWordMaxLen )
{
	return SaveWordsFromFile( m_stDictFile.szInBlockSqlWordsFile, vcWordList, uWordMaxLen );
}



/**
 *	@ public
 *	装载/保存 out rep 词
 */
BOOL CVwInfoMonitorConfigFile::LoadOutRepWords( vector<STVWINFOMWORD> & vcWordList, UINT * puWordMaxLen )
{
	return LoadWordsFromFile( m_stDictFile.szOutRepWordsFile, 0, vcWordList, puWordMaxLen );
}
BOOL CVwInfoMonitorConfigFile::SaveOutRepWords( vector<STVWINFOMWORD> & vcWordList, UINT uWordMaxLen )
{
	return SaveWordsFromFile( m_stDictFile.szOutRepWordsFile, vcWordList, uWordMaxLen );
}

BOOL CVwInfoMonitorConfigFile::LoadOutRepWords_utf8( vector<STVWINFOMWORD> & vcWordList, UINT * puWordMaxLen )
{
	return LoadWordsFromFile( m_stDictFile.szOutRepWordsFile_utf8, 0, vcWordList, puWordMaxLen );
}
BOOL CVwInfoMonitorConfigFile::SaveOutRepWords_utf8( vector<STVWINFOMWORD> & vcWordList, UINT uWordMaxLen )
{
	return SaveWordsFromFile( m_stDictFile.szOutRepWordsFile_utf8, vcWordList, uWordMaxLen );
}



/**
 *	@ public
 *	装载/保存 例外的域名
 */
BOOL CVwInfoMonitorConfigFile::LoadExceptionHosts( vector<STVWINFOMWORD> & vcWordList, UINT * puWordMaxLen )
{
	return LoadWordsFromFile( m_stDictFile.szExceptionHostsFile, 0, vcWordList, puWordMaxLen );
}
BOOL CVwInfoMonitorConfigFile::SaveExceptionHosts( vector<STVWINFOMWORD> & vcWordList, UINT uWordMaxLen )
{
	return SaveWordsFromFile( m_stDictFile.szExceptionHostsFile, vcWordList, uWordMaxLen );
}


/**
 *	@ public
 *	装载/保存 指定文件中的词
 */
BOOL CVwInfoMonitorConfigFile::LoadWordsFromFile( LPCTSTR lpcszFilename, UINT uMaxWordLoadCount, vector<STVWINFOMWORD> & vcWordList, UINT * puWordMaxLen )
{
	//
	//	lpcszFilename		- [in] 文件名
	//	uMaxWordLoadCount	- [in] 最多装载多少个词，0表示不限制
	//	vcWordList		- [in] 返回的 vector 词列表
	//	puWordMaxLen		- [in/out] 输入: 最大词长度(也就是超过次长度就不装载)
	//					   输出: 真实的最大词长度
	//	RETURN			- [in]
	//
	if ( NULL == lpcszFilename || 0 == _tcslen(lpcszFilename) )
	{
		return FALSE;
	}
	if ( NULL == puWordMaxLen )
	{
		return FALSE;
	}
	if ( ! PathFileExists( lpcszFilename ) )
	{
		return FALSE;
	}

	BOOL bRet		= FALSE;
	FILE * fp;
	UINT nWLen;
	UINT uSpecWordMaxLen;
	STVWINFOMWORD stWord;
	
	//	要求最大词长度不超过这个
	uSpecWordMaxLen	= *puWordMaxLen;
	if ( 0 == uSpecWordMaxLen )
	{
		uSpecWordMaxLen = CVWINFOMONITORCONFIGFILE_MAX_WORDLEN;
	}
	
	vcWordList.clear();
	*puWordMaxLen	= 0;
	
	fp = fopen( lpcszFilename, _T("r") );
	if ( fp )
	{
		//	...
		bRet = TRUE;
		
		memset( & stWord, 0, sizeof(stWord) );
		while( fgets( stWord.szWord, sizeof(stWord.szWord)-sizeof(TCHAR), fp ) )
		{
			StrTrim( stWord.szWord, _T("\r\n ") );
			nWLen = strlen( stWord.szWord );
			if ( nWLen && nWLen <= uSpecWordMaxLen )
			{
				//	计算单个词的最大长度
				if ( nWLen > ( *puWordMaxLen ) )
				{
					*puWordMaxLen = nWLen;
				}
				
				//	将新单词插入词典
				stWord.uLen = nWLen;
				vcWordList.push_back( stWord );

				//
				//	限制每次从一个文件中装载词的总个数
				//
				if ( uMaxWordLoadCount )
				{
					if ( vcWordList.size() >= uMaxWordLoadCount )
					{
						break;
					}
				}
			}
			
			memset( & stWord, 0, sizeof(stWord) );
		}
		
		//	..
		fclose( fp );
		fp = NULL;
	}
	
	return bRet;
}

/**
 *	@ private
 *	该函数负责装载带有通配符(*)的例外域名
 */
BOOL CVwInfoMonitorConfigFile::LoadWildcardWordsFromFile( LPCTSTR lpcszFilename, DWORD dwMaxWordLen, BOOL bOnlyPatternWords, vector<STVWINFOMWORD> & vcWordList )
{
	//
	//	lpcszFilename		- [in] 包含词语的文件地址
	//	dwMaxWordLen		- [in] 最大词长度，0 表示不限制
	//	bOnlyPatternWords	- [in] 是否 必须 含有通配符词语
	//	vcWordList		- [out] 输出词列表
	//
	if ( NULL == lpcszFilename || ( ! PathFileExists( lpcszFilename ) ) )
	{
		return FALSE;
	}

	BOOL  bRet	= FALSE;
	FILE * fp;
	vector<STVWINFOMWORD>::iterator it;
	STVWINFOMWORD stWord;
	UINT nWLen;
	BOOL bExistItem;


	//	..
	vcWordList.clear();
	
	fp = fopen( lpcszFilename, _T("r") );
	if ( fp )
	{
		bRet	= TRUE;
		
		memset( & stWord, 0, sizeof(stWord) );
		while( fgets( stWord.szWord, sizeof(stWord.szWord)-sizeof(TCHAR), fp ) )
		{
			StrTrim( stWord.szWord, _T("\r\n ") );

			nWLen = _tcslen( stWord.szWord );
			if ( nWLen )
			{
				//	如果调用者要求限制长度
				if ( dwMaxWordLen )
				{
					if ( nWLen >= dwMaxWordLen )
					{
						continue;
					}
				}
			}
			else
			{
				continue;
			}

			//	如果调用者要求词语中必须含有通配符
			stWord.bWildcard = _tcschr( stWord.szWord, '*' ) ? TRUE : FALSE;
			if ( bOnlyPatternWords )
			{
				if ( ! stWord.bWildcard )
				{
					continue;
				}
			}

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
			
			memset( & stWord, 0, sizeof(stWord) );
		}
		
		//	..
		fclose( fp );
	}

	return bRet;
}

BOOL CVwInfoMonitorConfigFile::SaveWordsFromFile( LPCTSTR lpcszFilename, vector<STVWINFOMWORD> & vcWordList, UINT uWordMaxLen )
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

	vector<STVWINFOMWORD>::iterator it;
	STVWINFOMWORD * pstWord;
	FILE * fp;
	TCHAR szLine[ MAX_PATH ];

	fp = fopen( lpcszFilename, "w" );
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

	return bRet;
}




//////////////////////////////////////////////////////////////////////////
//	Private




BOOL CVwInfoMonitorConfigFile::InitModule()
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

		_sntprintf( m_szFilterDllFile, sizeof(m_szFilterDllFile)/sizeof(TCHAR)-1, "%s%s", CModuleInfo::mb_szModPath, CVWINFOMONITORCONFIGFILE_FILE_FILTER );
		_sntprintf( m_szDeLibDllFile, sizeof(m_szDeLibDllFile)/sizeof(TCHAR)-1, "%s%s", CModuleInfo::mb_szSysDir, CVWINFOMONITORCONFIGFILE_FILE_DELIB );

		_sntprintf( m_szCfgFile, sizeof(m_szCfgFile)/sizeof(TCHAR)-1, "%s%s", m_szConfDir, CVWINFOMONITORCONFIGFILE_CONFFILE_CFG );
		
		_sntprintf( m_stDictFile.szOutRepWordsFile, sizeof(m_stDictFile.szOutRepWordsFile)/sizeof(TCHAR)-1, "%s%s", m_szConfDir, CVWINFOMONITORCONFIGFILE_CONFFILE_OUT_REP_WORDS );
		_sntprintf( m_stDictFile.szOutRepWordsFile_utf8, sizeof(m_stDictFile.szOutRepWordsFile_utf8)/sizeof(TCHAR)-1, "%s%s", m_szConfDir, CVWINFOMONITORCONFIGFILE_CONFFILE_OUT_REP_WORDS_UTF8 );
		_sntprintf( m_stDictFile.szOutBlockWordsFile, sizeof(m_stDictFile.szOutBlockWordsFile)/sizeof(TCHAR)-1, "%s%s", m_szConfDir, CVWINFOMONITORCONFIGFILE_CONFFILE_OUT_BLOCK_WORDS );

		_sntprintf( m_stDictFile.szInRepWordsFile, sizeof(m_stDictFile.szInRepWordsFile)/sizeof(TCHAR)-1, "%s%s", m_szConfDir, CVWINFOMONITORCONFIGFILE_CONFFILE_IN_REP_WORDS );
		_sntprintf( m_stDictFile.szInBlockWordsFile, sizeof(m_stDictFile.szInBlockWordsFile)/sizeof(TCHAR)-1, "%s%s", m_szConfDir, CVWINFOMONITORCONFIGFILE_CONFFILE_IN_BLOCK_WORDS );
		_sntprintf( m_stDictFile.szInBlockWordsFile_escape, sizeof(m_stDictFile.szInBlockWordsFile_escape)/sizeof(TCHAR)-1, "%s%s", m_szConfDir, CVWINFOMONITORCONFIGFILE_CONFFILE_IN_BLOCK_WORDS_ESCAPE );
		_sntprintf( m_stDictFile.szInBlockWordsFile_utf8, sizeof(m_stDictFile.szInBlockWordsFile_utf8)/sizeof(TCHAR)-1, "%s%s", m_szConfDir, CVWINFOMONITORCONFIGFILE_CONFFILE_IN_BLOCK_WORDS_UTF8 );

		_sntprintf( m_stDictFile.szInBlockSqlWordsFile, sizeof(m_stDictFile.szInBlockSqlWordsFile)/sizeof(TCHAR)-1, "%s%s", m_szConfDir, CVWINFOMONITORCONFIGFILE_CONFFILE_IN_ANTISQL_WORDS );

		_sntprintf( m_stDictFile.szInBlockAlertFile, sizeof(m_stDictFile.szInBlockAlertFile)/sizeof(TCHAR)-1, "%s%s", m_szConfDir, CVWINFOMONITORCONFIGFILE_CONFFILE_IN_BLOCK_ALERT );
		_sntprintf( m_stDictFile.szInBlockSqlAlertFile, sizeof(m_stDictFile.szInBlockSqlAlertFile)/sizeof(TCHAR)-1, "%s%s", m_szConfDir, CVWINFOMONITORCONFIGFILE_CONFFILE_IN_ANTISQL_ALERT );
		//_sntprintf( m_szInBlockAlertFileRes, sizeof(m_szInBlockAlertFileRes)/sizeof(TCHAR)-1, "res://%s/%s", m_szFilterDllFile, CVWINFOMONITORCONFIGFILE_CONFFILE_IN_BLOCK_ALERT );
		
		_sntprintf( m_stDictFile.szExceptionHostsFile, sizeof(m_stDictFile.szExceptionHostsFile)/sizeof(TCHAR)-1, "%s%s", m_szConfDir, CVWINFOMONITORCONFIGFILE_CONFFILE_EXCEPTION_HOSTS );

		//	词典文件名
		_sntprintf( m_stDictFile.szOutRepWordsDict, sizeof(m_stDictFile.szOutRepWordsDict)/sizeof(TCHAR)-1, "%s%s", m_szConfDir, CVWINFOMONITORCONFIGFILE_DICT_OUT_REP_WORDS );
		_sntprintf( m_stDictFile.szOutRepWordsDict_utf8, sizeof(m_stDictFile.szOutRepWordsDict_utf8)/sizeof(TCHAR)-1, "%s%s", m_szConfDir, CVWINFOMONITORCONFIGFILE_DICT_OUT_REP_WORDS_UTF8 );

		_sntprintf( m_stDictFile.szOutBlockWordsDict, sizeof(m_stDictFile.szOutBlockWordsDict)/sizeof(TCHAR)-1, "%s%s", m_szConfDir, CVWINFOMONITORCONFIGFILE_DICT_OUT_BLOCK_WORDS );
		_sntprintf( m_stDictFile.szInRepWordsDict, sizeof(m_stDictFile.szInRepWordsDict)/sizeof(TCHAR)-1, "%s%s", m_szConfDir, CVWINFOMONITORCONFIGFILE_DICT_IN_REP_WORDS );
		_sntprintf( m_stDictFile.szInBlockWordsDict, sizeof(m_stDictFile.szInBlockWordsDict)/sizeof(TCHAR)-1, "%s%s", m_szConfDir, CVWINFOMONITORCONFIGFILE_DICT_IN_BLOCK_WORDS );
		_sntprintf( m_stDictFile.szInBlockWordsDict_escape, sizeof(m_stDictFile.szInBlockWordsDict_escape)/sizeof(TCHAR)-1, "%s%s", m_szConfDir, CVWINFOMONITORCONFIGFILE_DICT_IN_BLOCK_WORDS_ESCAPE );
		_sntprintf( m_stDictFile.szInBlockWordsDict_utf8, sizeof(m_stDictFile.szInBlockWordsDict_utf8)/sizeof(TCHAR)-1, "%s%s", m_szConfDir, CVWINFOMONITORCONFIGFILE_DICT_IN_BLOCK_WORDS_UTF8 );

		_sntprintf( m_stDictFile.szExceptionHostsDict, sizeof(m_stDictFile.szExceptionHostsDict)/sizeof(TCHAR)-1, "%s%s", m_szConfDir, CVWINFOMONITORCONFIGFILE_DICT_EXCEPTION_HOSTS );

		//
		//	获取过滤器的版本
		//
		delib_get_file_version( CVwInfoMonitorConfigFile::m_szFilterDllFile, m_szFilterVersion, sizeof(m_szFilterVersion) );		
	}

	return bRet;
}
