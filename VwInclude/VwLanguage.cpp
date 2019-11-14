// VwLanguage.cpp: implementation of the CVwLanguage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VwLanguage.h"


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

CVwLanguage::CVwLanguage()
{
	memset( &m_stCurrentLang, 0, sizeof(m_stCurrentLang) );

	m_vcLangList.clear();

	//	获取本地语言信息
	GetLocalInfo();
}

CVwLanguage::~CVwLanguage()
{
	m_vcLangList.clear();
}

//
//	language list
//
BOOL CVwLanguage::LoadLangList( LPCTSTR lpctszDir )
{
	return ScanFiles( lpctszDir );
}

DWORD CVwLanguage::GetLangListCount()
{
	return m_vcLangList.size();
}

//
//	设置当前使用的语言
//
VOID CVwLanguage::SetCurrentLang( STVWLANGUAGELIST * pstLang )
{
	if ( NULL == pstLang )
	{
		return;
	}

	m_stCurrentLang = ( * pstLang );
}
VOID CVwLanguage::SetCurrentLang( LPCTSTR lptszLangEngName )
{
	if ( NULL == lptszLangEngName )
	{
		return;
	}

	STVWLANGUAGELIST * pstLangItem;

	for ( m_itLangList = m_vcLangList.begin(); m_itLangList != m_vcLangList.end(); m_itLangList ++ )
	{
		pstLangItem = m_itLangList;
		if ( 0 == _tcsicmp( lptszLangEngName, pstLangItem->szLangEngName ) )
		{
			m_stCurrentLang = ( * pstLangItem );
			break;
		}
	}
}

//
//	装载语言
//
BOOL CVwLanguage::GetLangText( LPCTSTR lpctszID, LPTSTR lptszText, DWORD dwSize )
{
	BOOL bRet	= FALSE;

	if ( _tcslen( m_stCurrentLang.szFilepath ) && PathFileExists( m_stCurrentLang.szFilepath ) )
	{
		bRet = TRUE;

		GetPrivateProfileString
		(
			lpctszID, _T("Loc"), _T(""),
			lptszText,
			dwSize/sizeof(TCHAR)-sizeof(TCHAR),
			m_stCurrentLang.szFilepath
		);
	}	

	return bRet;
}

BOOL CVwLanguage::SetLangForDlgItem( LPCTSTR lpctszID, HWND hWnd )
{
	BOOL bRet	= FALSE;
	TCHAR szText[ MAX_PATH ]	= {0};

	if ( GetLangText( lpctszID, szText, sizeof(szText) ) )
	{
		bRet = TRUE;

		::SetWindowText( hWnd, szText );
	}

	return bRet;
}



//////////////////////////////////////////////////////////////////////////
//	Private

/**
 *	获取本地语言信息
 */
VOID CVwLanguage::GetLocalInfo()
{
	LANGID lnLangID;
	UINT uLangPriID;
	INT i;

	m_pszLocLangEnglishName	= NULL;

	lnLangID	= GetSystemDefaultLangID();
	uLangPriID	= PRIMARYLANGID( lnLangID );

	for ( i = 0; i < g_nLanguageLocaleIDCount; i ++ )
	{
		if ( lnLangID == g_ArrLanguageLocaleID[ i ].dwLocaleID )
		{
			m_pszLocLangEnglishName = g_ArrLanguageLocaleID[ i ].lpszEnglishName;
			return;
		}
	}

	for ( i = 0; i < g_nLanguageLocaleIDCount; i ++ )
	{
		if ( uLangPriID == PRIMARYLANGID( g_ArrLanguageLocaleID[ i ].dwLocaleID ) )
		{
			m_pszLocLangEnglishName = g_ArrLanguageLocaleID[ i ].lpszEnglishName;
			return;
		}
	}
}

BOOL CVwLanguage::ScanFiles( LPCTSTR lpctszDir )
{
	CFileFind finder;
	BOOL  bWorking;
	TCHAR szPathFind[ MAX_PATH ]	= {0};
	STVWLANGUAGELIST stLangListItem;

	if ( NULL == lpctszDir || 0 == _tcslen( lpctszDir ) )
	{
		return FALSE;
	}
	if ( ! PathIsDirectory( lpctszDir ) )
	{
		return FALSE;
	}


	// ..
	_sntprintf( szPathFind, sizeof(szPathFind)/sizeof(TCHAR)-sizeof(TCHAR), _T("%s\\*.ini"), lpctszDir );

	//
	//	start working for files
	//
	bWorking = finder.FindFile( szPathFind );

	m_vcLangList.clear();

	while ( bWorking )
	{
		bWorking = finder.FindNextFile();
		if ( ! finder.IsDots() && ! finder.IsDirectory() )
		{
			memset( &stLangListItem, 0, sizeof(stLangListItem) );

			//
			//	保存语言文件全路径
			//
			_sntprintf( stLangListItem.szFilepath, sizeof(stLangListItem.szFilepath)/sizeof(TCHAR)-sizeof(TCHAR), _T("%s"), finder.GetFilePath() );
			_sntprintf( stLangListItem.szFilename, sizeof(stLangListItem.szFilename)/sizeof(TCHAR)-sizeof(TCHAR), _T("%s"), PathFindFileName( stLangListItem.szFilepath ) );
			_sntprintf( stLangListItem.szLangEngName, sizeof(stLangListItem.szLangEngName)/sizeof(TCHAR)-sizeof(TCHAR), _T("%s"), stLangListItem.szFilename );

			stLangListItem.szLangEngName[ ( _tcslen( stLangListItem.szLangEngName ) - 4 ) ] = 0;

			//
			//	读取语言名称
			//
			GetPrivateProfileString
			(
				_T("Main"), _T("LangName"), _T(""),
				stLangListItem.szLangLocName,
				sizeof(stLangListItem.szLangLocName)/sizeof(TCHAR),
				stLangListItem.szFilepath
			);
			GetPrivateProfileString
			(
				_T("Main"), _T("LangCode"), _T(""),
				stLangListItem.szLangLocCode,
				sizeof(stLangListItem.szLangLocCode)/sizeof(TCHAR),
				stLangListItem.szFilepath
			);

			if ( _tcslen( stLangListItem.szLangLocName ) && _tcslen( stLangListItem.szLangLocCode ) )
			{
				m_vcLangList.push_back( stLangListItem );
			}
		}
	}

	finder.Close();

	return TRUE;
}