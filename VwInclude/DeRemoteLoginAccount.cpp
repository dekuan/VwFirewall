// ProcLoginAccount.cpp: implementation of the CDeRemoteLoginAccount class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeRemoteLoginAccount.h"

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeRemoteLoginAccount::CDeRemoteLoginAccount()
{
	m_bInitSucc	= FALSE;

	//	用于迷惑破解者
	_sntprintf( m_szPasswordKey, sizeof(m_szPasswordKey)-sizeof(TCHAR), _T("%s"), _T("xingpwd") );
	memset( m_szPasswordKey, 0, sizeof(m_szPasswordKey) );

	memcpy( m_szPasswordKey, g_szDeRemoteLoginAccount_PasswordKey, min( sizeof(g_szDeRemoteLoginAccount_PasswordKey), sizeof(m_szPasswordKey) ) );
	delib_xorenc( m_szPasswordKey );

	//	...
	if ( CModuleInfoDeRemote::mb_bInitSucc )
	{
		m_bInitSucc	= TRUE;

		//	装载支持的语言
		m_vcSupportedLang.clear();
		LoadLangSupports();
	}
}
CDeRemoteLoginAccount::~CDeRemoteLoginAccount()
{

}

DWORD CDeRemoteLoginAccount::GetAccountCount()
{
	m_vcAllAccount.clear();
	if ( LoadAll( m_vcAllAccount ) )
	{
		return m_vcAllAccount.size();
	}
	return 0;
}

BOOL CDeRemoteLoginAccount::LoadAll( vector<STDELOGINACCOUNTINFO> & pvcAccount )
{
	BOOL bRet		= FALSE;
	TCHAR szBuffer[ 32 ]	= {0};
	//vector<STINISECTIONLINE>::iterator it;
	STINISECTIONLINE * pstAccList;
	INT nAccCount;
	INT i;
	STINISECTIONLINE * pstItem;
	STDELOGINACCOUNTINFO stAccount;


	nAccCount = delib_ini_parse_section_lineex( m_szCfgFile, _T("LoginAccount"), NULL, TRUE );
	if ( 0 == nAccCount )
	{
		return FALSE;
	}

	pstAccList = new STINISECTIONLINE[ nAccCount ];
	if ( NULL == pstAccList )
	{
		return FALSE;
	}

	memset( pstAccList, 0, sizeof(STINISECTIONLINE)*nAccCount );
	delib_ini_parse_section_lineex( m_szCfgFile, _T("LoginAccount"), pstAccList, TRUE );

	for ( i = 0; i < nAccCount; i ++ )
	{
		pstItem = & pstAccList[ i ];

		memset( & stAccount, 0, sizeof(stAccount) );

		_sntprintf( stAccount.szUserName, sizeof(stAccount.szUserName)-sizeof(TCHAR), _T("%s"), pstItem->szName );

		memset( szBuffer, 0, sizeof(szBuffer) );
		if ( delib_get_cookie_value( pstItem->szValue, _T("status:"), szBuffer, sizeof(szBuffer) ) )
		{
			stAccount.dwStatus = StrToInt( szBuffer );
		}

		//	password
		delib_get_cookie_value( pstItem->szValue, _T("password:"), stAccount.szPassword, sizeof(stAccount.szPassword) );
		
		//	lang
		delib_get_cookie_value( pstItem->szValue, _T("lang:"), stAccount.szLang, sizeof(stAccount.szLang) );
		if ( ! GetLangName( stAccount.szLang, stAccount.szLangName, sizeof(stAccount.szLangName) ) )
		{
			_tcscpy( stAccount.szLang, DEREMOTELOGINACCOUNT_DEFAULT_LANG );
			_tcscpy( stAccount.szLangName, DEREMOTELOGINACCOUNT_DEFAULT_LANGNAME );
		}

		if ( _tcslen( stAccount.szUserName ) && _tcslen( stAccount.szPassword ) )
		{
			//	添加到返回 vector 中
			pvcAccount.push_back( stAccount );
		}
	}

	//	free memory
	delete [] pstAccList;
	pstAccList = NULL;


	return ( pvcAccount.size() ? TRUE : FALSE );
}


BOOL CDeRemoteLoginAccount::SaveAccount( STDELOGINACCOUNTINFO * pstAccount )
{
	//
	//	pstAccount	- [in/out]
	//	RETURN		- TRUE / FALSE
	//
	//	[LoginAccount]
	//	xing	= "status:1;password:0034ae198102bcbbf182cda877a5e060;lang:zhcn;"
	//	amei	= "status:1;password:dd4b21e9ef71e1291183a46b913ae6f2;lang:en;"
	//
	//

	if ( NULL == pstAccount )
	{
		return FALSE;
	}
	if ( ! IsValidUsername( pstAccount->szUserName ) )
	{
		return FALSE;
	}
	if ( _tcslen( pstAccount->szPasswordNew ) && ( ! IsValidPassword( pstAccount->szPasswordNew ) ) )
	{
		return FALSE;
	}
	if ( ! GetLangName( pstAccount->szLang, pstAccount->szLangName, sizeof(pstAccount->szLangName) ) )
	{
		_tcscpy( pstAccount->szLang, DEREMOTELOGINACCOUNT_DEFAULT_LANG );
		_tcscpy( pstAccount->szLangName, DEREMOTELOGINACCOUNT_DEFAULT_LANGNAME );
	}

	TCHAR szLine[ MAX_PATH ]	= {0};
	TCHAR szPwd[ 64 ]		= {0};

	if ( _tcslen( pstAccount->szPasswordNew ) )
	{
		GetEncodePassword( pstAccount->szPasswordNew, pstAccount->szPassword, sizeof(pstAccount->szPassword) );
	}

	_sntprintf
	(
		szLine, sizeof(szLine)-sizeof(TCHAR),
		_T("\"status:%d;password:%s;lang:%s;\""),
		pstAccount->dwStatus,
		pstAccount->szPassword,
		pstAccount->szLang
	);

	return WritePrivateProfileString( _T("LoginAccount"), pstAccount->szUserName, szLine, m_szCfgFile );
}

BOOL CDeRemoteLoginAccount::DeleteAccount( STDELOGINACCOUNTINFO * pstAccount )
{
	if ( NULL == pstAccount )
	{
		return FALSE;
	}
	if ( ! IsValidUsername( pstAccount->szUserName ) )
	{
		return FALSE;
	}

	return WritePrivateProfileString( _T("LoginAccount"), pstAccount->szUserName, _T("\"\""), m_szCfgFile );
}

BOOL CDeRemoteLoginAccount::IsValidUsername( LPCTSTR lpcszUsername )
{
	if ( NULL == lpcszUsername )
	{
		return FALSE;
	}
	
	UINT  i		= 0;
	UINT  uLen	= _tcslen( lpcszUsername );
	
	if ( 0 == uLen )
	{
		return FALSE;
	}
	
	for ( i = 0; i < uLen; i ++ )
	{
		if ( NULL == _tcschr( _T("1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"), lpcszUsername[i] ) )
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CDeRemoteLoginAccount::IsValidPassword( LPCTSTR lpcszPassword )
{
	return IsValidUsername( lpcszPassword );
}

/**
 *	获取最终编码后的密码
 */
BOOL CDeRemoteLoginAccount::GetEncodePassword( LPCTSTR lpcszPassword, LPTSTR lpszPwdEncode, DWORD dwSize )
{
	if ( NULL == lpcszPassword || NULL == lpszPwdEncode )
	{
		return FALSE;
	}
	if ( 0 == _tcslen( lpcszPassword ) )
	{
		return FALSE;
	}
	if ( 0 == dwSize )
	{
		return FALSE;
	}

	BOOL  bRet			= FALSE;
	TCHAR szPwdStr[ MAX_PATH ]	= {0};

	_sntprintf( szPwdStr, sizeof(szPwdStr)-sizeof(TCHAR), _T("%s-%s"), m_szPasswordKey, lpcszPassword );
	StrTrim( szPwdStr, _T(" ") );
	_tcslwr( szPwdStr );
	if ( delib_get_string_md5( szPwdStr, lpszPwdEncode, dwSize ) )
	{
		bRet = TRUE;
	}

	return bRet;
}







//////////////////////////////////////////////////////////////////////////
//	Private





/**
 *	装载当前支持的所有语言
 */
BOOL CDeRemoteLoginAccount::LoadLangSupports()
{
	BOOL  bRet			= FALSE;
	TCHAR szTemp[ MAX_PATH ]	= {0};
	INT   nLen;

	STDELANGCONFIG stLang;
	STTEXTLINE * pstLine;
	STTEXTLINE * pstLangList;
	STTEXTLINE * pstLangNameList;
	INT nLangCount, nLangNameCount;
	INT i;

	//	Load Setting > langs
	memset( szTemp, 0, sizeof(szTemp) );
	GetPrivateProfileString( _T("Setting"), _T("langs"), _T(""), szTemp, sizeof(szTemp), m_szCfgFile );
	nLen = _tcslen( szTemp );
	if ( nLen )
	{
		nLangCount = delib_split_string( szTemp, _T("|"), NULL, FALSE );
		if ( nLangCount )
		{
			pstLangList = new STTEXTLINE[ nLangCount ];
			if ( pstLangList )
			{
				memset( pstLangList, 0, sizeof(STTEXTLINE)*nLangCount );
				delib_split_string( szTemp, _T("|"), pstLangList, FALSE );
			}
		}
	}

	//	Load Setting > langnames
	memset( szTemp, 0, sizeof(szTemp) );
	GetPrivateProfileString( _T("Setting"), _T("langnames"), _T(""), szTemp, sizeof(szTemp), m_szCfgFile );
	nLen = _tcslen( szTemp );
	if ( nLen )
	{
		nLangNameCount = delib_split_string( szTemp, _T("|"), NULL, FALSE );
		if ( nLangNameCount )
		{
			pstLangNameList = new STTEXTLINE[ nLangNameCount ];
			if ( pstLangNameList )
			{
				memset( pstLangNameList, 0, sizeof(STTEXTLINE)*nLangNameCount );
				delib_split_string( szTemp, _T("|"), pstLangNameList, FALSE );
			}
		}
	}

	//
	//	保存到全局数组
	//
	m_vcSupportedLang.clear();
	if ( nLangCount && nLangNameCount && nLangCount == nLangNameCount )
	{
		for ( i = 0; i < nLangCount; i ++ )
		{
			memset( & stLang, 0, sizeof(stLang) );

			pstLine	= & pstLangList[ i ];
			_sntprintf( stLang.szLang, sizeof(stLang.szLang)-sizeof(TCHAR), _T("%s"), pstLine->szLine );

			pstLine	= & pstLangNameList[ i ];
			_sntprintf( stLang.szLangName, sizeof(stLang.szLangName)-sizeof(TCHAR), _T("%s"), pstLine->szLine );

			if ( _tcslen( stLang.szLang ) && _tcslen( stLang.szLangName ) )
			{
				m_vcSupportedLang.push_back( stLang );
			}
		}
	}

	if ( pstLangList )
	{
		delete [] pstLangList;
		pstLangList = NULL;
	}
	if ( pstLangNameList )
	{
		delete [] pstLangNameList;
		pstLangNameList = NULL;
	}

	return bRet;
}

/**
 *	获取语言名称
 */
BOOL CDeRemoteLoginAccount::GetLangName( LPCTSTR lpcszLang, LPTSTR lpszLangName, DWORD dwSize )
{
	if ( NULL == lpcszLang || 0 == _tcslen(lpcszLang) )
	{
		return FALSE;
	}
	if ( NULL == lpszLangName || 0 == dwSize )
	{
		return FALSE;
	}

	BOOL bRet	= FALSE;
	INT  i;
	STDELANGCONFIG * pstLang;

	if ( m_vcSupportedLang.size() )
	{
		for ( i = 0; i < m_vcSupportedLang.size(); i ++ )
		{
			pstLang = & m_vcSupportedLang.at( i );
			if ( 0 == _tcsicmp( lpcszLang, pstLang->szLang ) )
			{
				bRet = TRUE;
				_sntprintf( lpszLangName, dwSize-sizeof(TCHAR), _T("%s"), pstLang->szLangName );
				break;
			}
		}
	}

	return bRet;
}

