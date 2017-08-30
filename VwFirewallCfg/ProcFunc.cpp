// ProcFunc.cpp: implementation of the CProcFunc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VwFirewallCfg.h"
#include "ProcFunc.h"

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )

#include "DeAdsiOpIIS.h"

#include "htmlhelp2.h"
#pragma comment( lib, "htmlhelp.lib" )
#pragma comment( lib, "bufferoverflowU.lib" )

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcFunc::CProcFunc()
{
	memset( m_szLangCode, 0, sizeof(m_szLangCode) );
	memset( m_szChmFile, 0, sizeof(m_szChmFile) );

	LoadString( AfxGetInstanceHandle(), IDS_LANGCODE, m_szLangCode, sizeof(m_szLangCode) );
	_sntprintf( m_szChmFile, sizeof(m_szChmFile)-sizeof(TCHAR), "%s\\%s_%s.chm", m_cVwFirewallConfigFile.mb_szModPath, PROJECT_NAMET, m_szLangCode );
}

CProcFunc::~CProcFunc()
{

}

BOOL CProcFunc::GetToDomain( LPCTSTR lpcszPanDomain, LPTSTR lpszToDomain, DWORD dwSize )
{
	if ( ! IsValidPanDomain( lpcszPanDomain ) )
	{
		return FALSE;
	}
	if ( NULL == lpszToDomain )
	{
		return FALSE;
	}

	_sntprintf( lpszToDomain, dwSize-sizeof(TCHAR), "%s", lpcszPanDomain+2 );

	return TRUE;
}

BOOL CProcFunc::IsToDomainExist( LPCTSTR lpcszToDomain )
{
	if ( NULL == lpcszToDomain )
	{
		return FALSE;
	}
	if ( 0 == _tcslen( lpcszToDomain ) )
	{
		return FALSE;
	}

	BOOL bRet	= FALSE;
	CDeAdsiOpIIS aOpiis;
	vector<STHOSTINFO> vcAllHost;
	vector<STHOSTINFO>::iterator it;
	STHOSTINFO * pstHostTemp;

	if ( aOpiis.GetAllHostInfo( vcAllHost ) && vcAllHost.size() )
	{
		for ( it = vcAllHost.begin(); it != vcAllHost.end(); it ++ )
		{
			pstHostTemp = it;
			if ( 0 == _tcsicmp( pstHostTemp->szHostName, lpcszToDomain ) )
			{
				bRet = TRUE;
				break;
			}
		}
	}

	return bRet;
}


/**
 *	@ Public
 *	是否是泛域名
 */
BOOL CProcFunc::IsValidPanDomain( LPCTSTR lpcszString )
{
	if ( NULL == lpcszString || NULL == lpcszString[0] )
	{
		return FALSE;
	}
	if ( _tcsstr(lpcszString,"..") )
	{
		return FALSE;
	}

	UINT  uLen;
	TCHAR szTestChars[ MAX_PATH ];
	UINT  i = 0;

	uLen = _tcslen( lpcszString );

	//	*.a.cn
	if ( uLen < 6 || uLen > 128 )
	{
		return FALSE;
	}

	if ( 0 != _tcsnicmp( lpcszString, _T("*"), 1 ) )
	{
		//	"*" 字符不再第一位
		return FALSE;
	}

	if ( 1 != delib_get_chr_count( lpcszString, '*' ) )
	{
		//	"*" 字符的数量超过 1
		return FALSE;
	}

	_sntprintf( szTestChars, sizeof(szTestChars)/sizeof(TCHAR)-1, _T("%s"), _T("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890.-*") );

	for ( i = 0; i < uLen; i ++ )
	{
		if ( NULL == _tcschr( szTestChars, lpcszString[i] ) )
		{
			return FALSE;
		}
	}

	return TRUE;
}

/**
 *	@ Public
 *	是否是泛域名
 */
BOOL CProcFunc::IsValidDomain( LPCTSTR lpcszString )
{
	if ( NULL == lpcszString || NULL == lpcszString[0] )
	{
		return FALSE;
	}
	if ( _tcsstr(lpcszString,"..") )
	{
		return FALSE;
	}

	UINT  uLen;
	TCHAR szTestChars[ MAX_PATH ];
	UINT  i = 0;

	uLen = _tcslen( lpcszString );

	//	a.cn
	if ( uLen < 4 || uLen > 128 )
	{
		return FALSE;
	}

	//	普通域名
	_sntprintf( szTestChars, sizeof(szTestChars)/sizeof(TCHAR)-1, _T("%s"), _T("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890.-") );

	for ( i = 0; i < uLen; i ++ )
	{
		if ( NULL == _tcschr( szTestChars, lpcszString[i] ) )
		{
			return FALSE;
		}
	}

	return TRUE;
}

/**
 *	@ Public
 *	是否是正确的计算机名
 */
BOOL CProcFunc::IsValidComputerName( LPCTSTR lpcszString )
{
	if ( NULL == lpcszString || NULL == lpcszString[0] )
	{
		return FALSE;
	}

	UINT  uLen;
	TCHAR szTestChars[ MAX_PATH ];
	UINT  i = 0;

	uLen = _tcslen( lpcszString );

	//	4 ~ 15 个字符
	if ( uLen < 4 || uLen > 15 )
	{
		return FALSE;
	}
	if ( 0 == _tcsnicmp( lpcszString, _T("-"), 1 ) )
	{
		return FALSE;
	}

	//	组成计算机名的字符集
	_sntprintf( szTestChars, sizeof(szTestChars)/sizeof(TCHAR)-1, _T("%s"), _T("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890-_") );

	for ( i = 0; i < uLen; i ++ )
	{
		if ( NULL == _tcschr( szTestChars, lpcszString[i] ) )
		{
			return FALSE;
		}
	}

	return TRUE;
}

/**
 *	@ Public
 *	是否是正确的端口
 */
BOOL CProcFunc::IsValidPortNumber( LPCTSTR lpcszString )
{
	if ( NULL == lpcszString || NULL == lpcszString[0] )
	{
		return FALSE;
	}

	UINT  uLen;
	TCHAR szTestChars[ MAX_PATH ];
	UINT  i = 0;
	LONG  lnPortNumber;

	//	1024 ~ 65535
	uLen = _tcslen( lpcszString );

	//	1 ~ 5 个字符
	if ( uLen < 1 || uLen > 5 )
	{
		return FALSE;
	}

	//	组成计算机名的字符集
	_sntprintf( szTestChars, sizeof(szTestChars)/sizeof(TCHAR)-1, _T("%s"), _T("1234567890") );

	for ( i = 0; i < uLen; i ++ )
	{
		if ( NULL == _tcschr( szTestChars, lpcszString[i] ) )
		{
			return FALSE;
		}
	}

	lnPortNumber = atol( lpcszString );
	if ( lnPortNumber <= 0 || lnPortNumber >= 65535 )
	{
		return FALSE;
	}	

	return TRUE;
}


/**
 *	@ Public
 *	是否是正确的子域名
 */
BOOL CProcFunc::IsValidSubDomain( LPCTSTR lpcszString )
{
	if ( NULL == lpcszString || 0 == _tcslen(lpcszString) )
	{
		return FALSE;
	}
	if ( _tcslen(lpcszString) > 128 )
	{
		return FALSE;
	}
	if ( _tcsstr(lpcszString,",,") )
	{
		return FALSE;
	}

	TCHAR szString[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890,-";
	UINT  i = 0;
	for ( i = 0; i < _tcslen(lpcszString); i ++ )
	{
		if ( NULL == _tcschr( szString, lpcszString[i] ) )
		{
			return FALSE;
		}
	}
	return TRUE;
}

/**
 *	@ Public
 *	是否是正确的保护目录
 */
BOOL CProcFunc::IsValidProtectedDir( LPCTSTR lpcszString )
{
	if ( NULL == lpcszString || 0 == _tcslen(lpcszString) )
	{
		return FALSE;
	}
	if ( _tcslen(lpcszString) > MAX_PATH )
	{
		return FALSE;
	}
	if ( ! PathIsDirectory( lpcszString ) )
	{
		return FALSE;
	}

	//	"c:\\windows\s-d_1"
	TCHAR szString[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890:\\_-.";
	UINT  i = 0;
	for ( i = 0; i < _tcslen(lpcszString); i ++ )
	{
		if ( NULL == _tcschr( szString, lpcszString[i] ) )
		{
			return FALSE;
		}
	}
	return TRUE;
}

/**
 *	@ Public
 *	是否是正确的保护目录
 */
BOOL CProcFunc::IsValidExceptedProcessFile( LPCTSTR lpcszString )
{
	if ( NULL == lpcszString || 0 == _tcslen(lpcszString) )
	{
		return FALSE;
	}
	if ( _tcslen(lpcszString) > MAX_PATH )
	{
		return FALSE;
	}
	if ( ! PathFileExists( lpcszString ) )
	{
		return FALSE;
	}

	//	"c:\\windows\\explorer.exe"
	TCHAR szString[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890:\\_-. ";
	UINT  i = 0;
	for ( i = 0; i < _tcslen(lpcszString); i ++ )
	{
		if ( NULL == _tcschr( szString, lpcszString[i] ) )
		{
			return FALSE;
		}
	}
	return TRUE;
}

/**
 *	@ Public
 *	是否是正确的文件扩展名（不带前面的“.”）
 */
BOOL CProcFunc::IsValidFileExtName( LPCTSTR lpcszString )
{
	if ( NULL == lpcszString || 0 == _tcslen(lpcszString) )
	{
		return FALSE;
	}
	if ( _tcslen(lpcszString) > 6 )
	{
		return FALSE;
	}
	if ( _tcsstr(lpcszString,".") )
	{
		return FALSE;
	}

	TCHAR szString[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890";
	UINT  i = 0;
	for ( i = 0; i < _tcslen(lpcszString); i ++ )
	{
		if ( NULL == _tcschr( szString, lpcszString[i] ) )
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CProcFunc::IsValidExceptedExt( LPCTSTR lpcszExceptedExt )
{
	if ( NULL == lpcszExceptedExt || 0 == _tcslen(lpcszExceptedExt) )
	{
		return FALSE;
	}

	BOOL bRet;
	STTEXTLINE * pstExtList	= NULL;
	INT nCount;
	INT i;

	//	...
	bRet = FALSE;

	//	split name & value to vecor
	nCount = delib_split_string( lpcszExceptedExt, _T("."), NULL, FALSE );
	if ( nCount > 0 && nCount < 128 )
	{
		pstExtList = new STTEXTLINE[ nCount ];
		if ( pstExtList )
		{
			bRet = TRUE;

			memset( pstExtList, 0, sizeof(STTEXTLINE)*nCount );
			delib_split_string( lpcszExceptedExt, _T("."), pstExtList, FALSE );
			for ( i = 0; i < nCount; i ++ )
			{
				if ( ! IsValidFileExtName( pstExtList[ i ].szLine ) )
				{
					bRet = FALSE;
					break;
				}
			}

			delete [] pstExtList;
			pstExtList = NULL;
		}
	}

	return bRet;
}


/**
 *	@ Public	
 */
INT CProcFunc::GetListIndexByString( CListCtrl * pListObject, INT nSubItem, LPCTSTR lpcszString, INT nExceptedIndex /* = -1 */ )
{
	if ( NULL == pListObject )
	{
		return -1;
	}
	if ( NULL == lpcszString || 0 == _tcslen(lpcszString) )
	{
		return -1;
	}

	INT  nRet	= -1;
	UINT uItemCount;
	UINT i;
	TCHAR szTemp[ MAX_PATH ];

	uItemCount	= pListObject->GetItemCount();
	for ( i = 0; i < uItemCount; i ++ )
	{
		if ( i == nExceptedIndex )
		{
			//	跳过例外的 List Index
			continue;
		}

		memset( szTemp, 0, sizeof(szTemp) );
		if ( pListObject->GetItemText( i, nSubItem, szTemp, sizeof(szTemp) ) > 0 )
		{
			if ( 0 == _tcsicmp( szTemp, lpcszString ) )
			{
				nRet = i;
				break;
			}
		}
	}

	return nRet;
}

/**
 *	@ Public
 *	
 */
INT CProcFunc::GetListIndexByPanDomain( CListCtrl * pListDomain, LPCTSTR lpcszPanDomain, INT nExceptedIndex /* = -1 */ )
{
	if ( NULL == pListDomain )
	{
		return -1;
	}
	if ( NULL == lpcszPanDomain || 0 == _tcslen(lpcszPanDomain) )
	{
		return -1;
	}

	INT  nRet	= -1;
	UINT uItemCount;
	UINT i;
	TCHAR szTemp[ MAX_PATH ];

	uItemCount	= pListDomain->GetItemCount();
	for ( i = 0; i < uItemCount; i ++ )
	{
		if ( i == nExceptedIndex )
		{
			//	跳过例外的 List Index
			continue;
		}

		memset( szTemp, 0, sizeof(szTemp) );
		if ( pListDomain->GetItemText( i, 0, szTemp, sizeof(szTemp) ) > 0 )
		{
			if ( 0 == _tcsnicmp( szTemp, _T("*"), 1 ) )
			{
				//	*.abc.com
				if ( delib_is_match_with_pattern( szTemp, lpcszPanDomain ) )
				{
					nRet = i;
					break;
				}
			}
			else if ( 0 == _tcsicmp( szTemp, lpcszPanDomain ) )
			{
				//	
				nRet = i;
				break;
			}
		}
	}

	return nRet;
}



/**
 *	@ Public
 *	打开 CHM 文件的某个页面
 */
BOOL CProcFunc::OpenChmHelpPage( WPARAM wParam )
{
	BOOL  bRet			= FALSE;
	TCHAR szCmdLine[ MAX_PATH ]	= {0};

	if ( PathFileExists( m_szChmFile ) )
	{
		bRet	= TRUE;

		//	显示帮助文件
		if ( WPARAM_HEMLHELP_VWFIREWALL_OVERVIEW == wParam )
		{
			_sntprintf( szCmdLine, sizeof(szCmdLine)-sizeof(TCHAR), "%s::/overview.htm", m_szChmFile );
			HtmlHelp( NULL, szCmdLine, HH_DISPLAY_TOPIC, 0 );
		}	
	}
	
	return bRet;
}


/**
 *	@ public
 *	设置窗口透明度
 */
BOOL CProcFunc::MakeAlphaDlg( HWND hWnd, INT nAlphaPercent )
{
	if ( NULL == hWnd )
	{
		return FALSE;
	}

	typedef BOOL (FAR PASCAL * FUNC1)
		(
			HWND hwnd,           // handle to the layered window
			COLORREF crKey,      // specifies the color key
			BYTE bAlpha,         // value for the blend function
			DWORD dwFlags        // action
		);

	//	第二步 实现代码
	HMODULE hModule = GetModuleHandle( _T("user32.dll") );
	
	if ( hModule )
	{
		FUNC1 SetLayeredWindowAttributes = (FUNC1)GetProcAddress( hModule, ("SetLayeredWindowAttributes") );
		
		if ( SetLayeredWindowAttributes )
		{
			// 设置分层扩展标记
			SetWindowLong( hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | 0x80000L );
			// 70% alpha
			SetLayeredWindowAttributes( hWnd, 0, ( 255 * nAlphaPercent ) / 100, 0x2 );
			
			// 第三步：如何除去透明选项？
			// 除去分层扩展标记
			//	SetWindowLong(hWnd,, GWL_EXSTYLE,
			//	GetWindowLong(hWnd, GWL_EXSTYLE) & ~ 0x80000L);
			// 重画窗口
			//	RedrawWindow();
		}
	}
	
	return TRUE;
}

/**
 *	@ public
 *	弹出目录选择窗口，让用户选择目录
 */
BOOL CProcFunc::BrowseForFolder( LPCTSTR lpcszTitle, LPTSTR lpszPath, DWORD dwSize )
{
	if ( NULL == lpszPath || 0 == dwSize )
	{
		return FALSE;
	}
	
	BOOL bRet	= FALSE;
	CString sFolder;
	LPMALLOC pMalloc;
	
	//	Gets the Shell's default allocator
	if ( ::SHGetMalloc(&pMalloc) == NOERROR )
	{
		BROWSEINFO bi;
		char pszBuffer[MAX_PATH];
		LPITEMIDLIST pidl;
		
		bi.hwndOwner		= AfxGetMainWnd()->GetSafeHwnd();
		bi.pidlRoot		= NULL;
		bi.pszDisplayName	= pszBuffer;
		bi.lpszTitle		= lpcszTitle;
		bi.ulFlags		= BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
		bi.lpfn			= NULL;
		bi.lParam		= 0;
		
		//	This next call issues the dialog box.
		if ( ( pidl = ::SHBrowseForFolder(&bi)) != NULL )
		{
			if ( ::SHGetPathFromIDList( pidl, pszBuffer ) )
			{ 	
				//	At this point pszBuffer contains the selected path
				sFolder = pszBuffer;
			}
			//	Free the PIDL allocated by SHBrowseForFolder.
			pMalloc->Free( pidl );
			
			//	read the file list again
			_sntprintf( lpszPath, dwSize-sizeof(TCHAR), "%s", sFolder.GetBuffer(0) );
			
			bRet = TRUE;
		}
		//	Release the shell's allocator.
		pMalloc->Release();
	}
	return bRet;
}