// ProcVwAd.cpp: implementation of the CProcVwAd class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProcVwAd.h"
#include "DeHttp.h"

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )

//#include "VwIniFile.h"
#include "Ini.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcVwAd::CProcVwAd()
{
	//	...
	InitData();
}

CProcVwAd::~CProcVwAd()
{

}

/**
 *	Public
 *	设置下载超时
 */
VOID CProcVwAd::SetDownloadTimeout( DWORD dwTimeout )
{
	m_stCfg.dwDownloadTimeout = dwTimeout;
}

/**
 *	Public
 *	下载广告信息(用随机的办法来负载均衡)
 */
BOOL CProcVwAd::QueryAdInfo( LPCTSTR lpcszAdName, LPCTSTR lpcszLanguage, UINT uRedirectUrlId, vector<STPROCVWADITEM> * pvcAdList, STPROCVWADINFO * pstAdInfo /*= NULL*/ )
{
	TCHAR szUrl[ 128 ];

	memset( szUrl, 0, sizeof(szUrl) );
	_sntprintf
	(
		szUrl, sizeof(szUrl)/sizeof(TCHAR)-1,
		_T("http://rd%d.vidun.net/?%d"),
		( GetTickCount() % 100 + 1 ),
		uRedirectUrlId
	);
	return QueryAdInfo( lpcszAdName, lpcszLanguage, szUrl, pvcAdList, pstAdInfo );
}

/**
 *	Public
 *	下载广告信息
 */
BOOL CProcVwAd::QueryAdInfo( LPCTSTR lpcszAdName, LPCTSTR lpcszLanguage, LPCTSTR lpcszUrl, vector<STPROCVWADITEM> * pvcAdList, STPROCVWADINFO * pstAdInfo /*= NULL*/ )
{
	BOOL  bRet;
	TCHAR szError[ MAX_PATH ]	= {0};
	CDeHttp http;
	//CVwIniFile cVwIniFile;
	CIni ini;
	CString strRetVal;
	BOOL  bRun			= FALSE;
	TCHAR szTempDir[ MAX_PATH ]	= {0};
	TCHAR szStaticAdIni[ MAX_PATH ]	= {0};
	STINISECTIONLINE stLine;
	STPROCVWADITEM stItem;
	UINT i;

	STINISECTIONLINE * pstSection	= NULL;
	INT nSectionCount		= 0;

	if ( NULL == lpcszAdName || NULL == lpcszLanguage || NULL == lpcszUrl || NULL == pvcAdList )
	{
		return FALSE;
	}
	if ( 0 == _tcslen( lpcszAdName ) || 0 == _tcslen( lpcszLanguage ) || 0 == _tcslen( lpcszUrl ) )
	{
		return FALSE;
	}

	//	...
	bRet	= FALSE;
	pvcAdList->clear();

	delib_get_window_tempdir( szTempDir, sizeof(szTempDir) );
	_sntprintf( szStaticAdIni, sizeof(szStaticAdIni)/sizeof(TCHAR)-1, _T("%s\\%s.ini"), szTempDir, lpcszAdName );

	try
	{ 
		if ( http.DownloadFile( lpcszUrl, szStaticAdIni, NULL, TRUE, szError, m_stCfg.dwDownloadTimeout ) )
		{
			ini.SetPathName( szStaticAdIni );

			//	只要下载成功就算搞成了
			bRet = TRUE;

			if ( pstAdInfo )
			{
				pstAdInfo->dwSingleSpan	= ini.GetInt( _T("global"), _T("single_span"), 30, BASE_DECIMAL ) * 1000;
				pstAdInfo->dwLoopSpan	= ini.GetInt( _T("global"), _T("loop_span"), 60, BASE_DECIMAL ) * 1000;
				//pstAdInfo->dwSingleSpan	= GetPrivateProfileInt( _T("global"), _T("single_span"), 30, szStaticAdIni ) * 1000;
				//pstAdInfo->dwLoopSpan	= GetPrivateProfileInt( _T("global"), _T("loop_span"), 60, szStaticAdIni ) * 1000;
			}

			//	下载成功
			bRun = (BOOL)( ini.GetInt( _T("global"), _T("run"), 0 ) );
			//bRun = (BOOL)GetPrivateProfileInt( _T("global"), _T("run"), 0, szStaticAdIni );
			if ( bRun )
			{
				nSectionCount = delib_ini_parse_section_lineex( szStaticAdIni, lpcszLanguage, NULL );
				if ( nSectionCount )
				{
					pstSection = new STINISECTIONLINE[ nSectionCount ];
					if ( pstSection )
					{
						nSectionCount = delib_ini_parse_section_lineex( szStaticAdIni, lpcszLanguage, pstSection );
						for ( i = 0; i < nSectionCount; i ++ )
						{
							stLine = pstSection[ i ];

							memset( &stItem, 0, sizeof(stItem) );
							
							ini.GetString( stLine.szLine, _T("txt"), stItem.szTxt, sizeof(stItem.szTxt), _T("") );
							ini.GetString( stLine.szLine, _T("url"), stItem.szUrl, sizeof(stItem.szUrl), _T("") );

							//cVwIniFile.GetMyPrivateProfileString( stLine.szLine, _T("txt"), _T(""), stItem.szTxt, sizeof(stItem.szTxt) );
							//cVwIniFile.GetMyPrivateProfileString( stLine.szLine, _T("url"), _T(""), stItem.szUrl, sizeof(stItem.szUrl) );
							//GetPrivateProfileString( stLine.szLine, _T("txt"), _T(""), stItem.szTxt, sizeof(stItem.szTxt), szStaticAdIni );
							//GetPrivateProfileString( stLine.szLine, _T("url"), _T(""), stItem.szUrl, sizeof(stItem.szUrl), szStaticAdIni );
							StrTrim( stItem.szTxt, _T("\r\n\t ") );
							StrTrim( stItem.szUrl, _T("\r\n\t ") );

							if ( _tcslen( stItem.szTxt ) && _tcslen( stItem.szUrl ) )
							{
								pvcAdList->push_back( stItem );
							}
						}

						delete [] pstSection;
						pstSection = NULL;
					}
				}
			}
		}
	}
	catch ( ... )
	{
	}

	return bRet;
}




//////////////////////////////////////////////////////////////////////////
//	Private
//




/**
 *	Private
 *	初始化
 */
VOID CProcVwAd::InitData()
{
	memset( &m_stCfg, 0, sizeof(m_stCfg) );

	//	下载 INI 文件的超时
	m_stCfg.dwDownloadTimeout = 10*1000;
}



