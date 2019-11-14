//////////////////////////////////////////////////////////////////////////
//	BrowseDirectory.h
//
#ifndef __BROWSEDIRECTORY_HEADER__
#define __BROWSEDIRECTORY_HEADER__

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )




static TCHAR g_szBrowseDirectoryRootDir[MAX_PATH];

static INT CALLBACK callbackBrowseDirectory_BrowseCallbackProc( HWND hwnd, UINT msg, LPARAM lParam, LPARAM pData )
{
	if ( BFFM_INITIALIZED == msg )
	{
		::SendMessage( hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)g_szBrowseDirectoryRootDir );
	}
	return 0;
}   

static BOOL BrowseDirectory_Phy( HWND hWnd, LPCTSTR lpcszTitle, LPSTR lpszDir, DWORD dwLen )
{
	if ( NULL == hWnd || NULL == lpcszTitle || NULL == lpszDir || 0 == dwLen )
	{
		return FALSE;
	}

	_sntprintf( g_szBrowseDirectoryRootDir, sizeof(g_szBrowseDirectoryRootDir)-sizeof(TCHAR), "%s", lpszDir );

	BROWSEINFO bi;
	LPITEMIDLIST lpstItemList;

	SHGetSpecialFolderLocation( AfxGetMainWnd()->GetSafeHwnd(), CSIDL_DRIVES, &lpstItemList );

	bi.hwndOwner		= hWnd;
	bi.pidlRoot		= lpstItemList;
	bi.pszDisplayName	= 0;
	bi.lpszTitle		= lpcszTitle;
	bi.lpfn			= callbackBrowseDirectory_BrowseCallbackProc;
	bi.lParam		= 0;
	bi.ulFlags		= BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;

	LPITEMIDLIST pidl;
	if ( pidl = SHBrowseForFolder(&bi) )
	{
		SHGetPathFromIDList( pidl, lpszDir );
		return TRUE;
	}

	return FALSE;
}

static BOOL BrowseDirectory_VDir( HWND hWnd, LPCTSTR lpcszTitle, LPCTSTR lpcszRoot, LPSTR lpszVDir, DWORD dwSize )
{
	if ( NULL == hWnd || NULL == lpcszTitle || NULL == lpcszRoot || NULL == lpszVDir || 0 == dwSize )
	{
		return FALSE;
	}
	if ( 0 == _tcslen( lpcszRoot ) || ( ! PathIsDirectory( lpcszRoot ) ) )
	{
		return FALSE;
	}

	BOOL bRet			= FALSE;
	TCHAR szRootDir[ MAX_PATH ]	= {0};
	TCHAR szDir[ MAX_PATH ]		= {0};
	DWORD dwRootDirLen;
	DWORD dwVDirLen;
	UINT  i;

	_sntprintf( szRootDir, sizeof(szRootDir)-sizeof(TCHAR), "%s", lpcszRoot );
	_sntprintf( szDir, sizeof(szDir)-sizeof(TCHAR), "%s", lpcszRoot );
	if ( '\\' == szRootDir[ _tcslen( szRootDir ) - 1 ] || '/' == szRootDir[ _tcslen( szRootDir ) - 1 ] )
	{
		//	去掉最后的 "/" 或者 "\"
		szRootDir[ _tcslen( szRootDir ) - 1 ]	= 0;
	}
	dwRootDirLen	= _tcslen( szRootDir );

	if ( BrowseDirectory_Phy( hWnd, lpcszTitle, szDir, sizeof(szDir) ) &&
		_tcslen( szDir ) >= dwRootDirLen )
	{
		if ( 0 == _tcsnicmp( szRootDir, szDir, dwRootDirLen ) )
		{
			bRet = TRUE;

			_sntprintf( lpszVDir, dwSize-sizeof(TCHAR), "%s/", szDir + dwRootDirLen );
			dwVDirLen = _tcslen(lpszVDir);
			if ( dwVDirLen )
			{
				for ( i = 0; i < _tcslen(lpszVDir); i ++ )
				{
					if ( '\\' == lpszVDir[ i ] )
					{
						lpszVDir[ i ] = '/';
					}
				}
			}
			else
			{
				_sntprintf( lpszVDir, dwSize-sizeof(TCHAR), "%s", "/" );
			}
		}
	}

	return bRet;
}



#endif	//	__BROWSEDIRECTORY_HEADER__