// CompressedPath.cpp: implementation of the CCompressedPath class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VwCompressedPath.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCompressedPath::CCompressedPath()
{

}

CCompressedPath::~CCompressedPath()
{

}

/**
 *	获取格式化后的文件路径，主要是将 Windows 路径替换成 $WINDOWSDIR$
 */
BOOL CCompressedPath::GetCompressedPath( LPCTSTR lpszFilePathIn, LPTSTR lpszFilePathNew, DWORD dwPathNewSize, LPTSTR lpszWinDir, DWORD dwWDSize )
{
	if ( NULL == lpszFilePathIn || 0 == _tcslen(lpszFilePathIn) )
	{
		return FALSE;
	}
	
	BOOL  bRet = FALSE;
	TCHAR szSPFolder[ MAX_PATH ] = {0};
	
	// 获取 WINDOW 目录
	if ( lpszWinDir && dwWDSize > 0 )
	{
		GetWindowsDirectory( lpszWinDir, dwWDSize );
	}
	
	if ( ! bRet && GetSpecialFolderPath( SPPATH_WINSYSTEM, szSPFolder, sizeof(szSPFolder) ) )
	{
		bRet |= LeftPathReplace( lpszFilePathIn, lpszFilePathNew, dwPathNewSize, szSPFolder, _T("::WinSystem::") );
	}
	
	if ( ! bRet && GetSpecialFolderPath( SPPATH_SYSTEMROOT, szSPFolder, sizeof(szSPFolder) ) )
	{
		bRet |= LeftPathReplace( lpszFilePathIn, lpszFilePathNew, dwPathNewSize, szSPFolder, _T("::SystemRoot::") );
	}
	
	if ( ! bRet && GetSpecialFolderPath( SPPATH_WINDIR, szSPFolder, sizeof(szSPFolder) ) )
	{
		bRet |= LeftPathReplace( lpszFilePathIn, lpszFilePathNew, dwPathNewSize, szSPFolder, _T("::WinDir::") );
	}
	
	if ( ! bRet && GetSpecialFolderPath( SPPATH_PROGRAMFILES, szSPFolder, sizeof(szSPFolder) ) )
	{
		bRet |= LeftPathReplace( lpszFilePathIn, lpszFilePathNew, dwPathNewSize, szSPFolder, _T("::ProgramFiles::") );
	}
	
	if ( ! bRet && GetSpecialFolderPath( SPPATH_DESKTOPDIRECTORY, szSPFolder, sizeof(szSPFolder) ) )
	{
		bRet |= LeftPathReplace( lpszFilePathIn, lpszFilePathNew, dwPathNewSize, szSPFolder, _T("::DesktopDirectory::") );
	}
	
	if ( ! bRet && GetSpecialFolderPath( SPPATH_COMMON_DESKTOPDIRECTORY, szSPFolder, sizeof(szSPFolder) ) )
	{
		bRet |= LeftPathReplace( lpszFilePathIn, lpszFilePathNew, dwPathNewSize, szSPFolder, _T("::CommonDesktopDirectory::") );
	}
	
	if ( ! bRet && GetSpecialFolderPath( SPPATH_COMMON_DOCUMENTS, szSPFolder, sizeof(szSPFolder) ) )
	{
		bRet |= LeftPathReplace( lpszFilePathIn, lpszFilePathNew, dwPathNewSize, szSPFolder, _T("::CommonDocuments::") );
	}
	
	if ( ! bRet && GetSpecialFolderPath( SPPATH_USERPROFILE, szSPFolder, sizeof(szSPFolder) ) )
	{
		bRet |= LeftPathReplace( lpszFilePathIn, lpszFilePathNew, dwPathNewSize, szSPFolder, _T("::UserProfile::") );
	}
	
	if ( ! bRet && GetSpecialFolderPath( SPPATH_ALLUSERPROFILE, szSPFolder, sizeof(szSPFolder) ) )
	{
		bRet |= LeftPathReplace( lpszFilePathIn, lpszFilePathNew, dwPathNewSize, szSPFolder, _T("::AllUserProfile::") );
	}
	
	if ( ! bRet && GetSpecialFolderPath( SPPATH_TEMP, szSPFolder, sizeof(szSPFolder) ) )
	{
		bRet |= LeftPathReplace( lpszFilePathIn, lpszFilePathNew, dwPathNewSize, szSPFolder, _T("::Temp::") );
	}

	if ( ! bRet && GetSpecialFolderPath( SPPATH_TMP, szSPFolder, sizeof(szSPFolder) ) )
	{
		bRet |= LeftPathReplace( lpszFilePathIn, lpszFilePathNew, dwPathNewSize, szSPFolder, _T("::Tmp::") );
	}

	if ( ! bRet && GetSpecialFolderPath( SPPATH_SYSTEMDRIVE, szSPFolder, sizeof(szSPFolder) ) )
	{
		bRet |= LeftPathReplace( lpszFilePathIn, lpszFilePathNew, dwPathNewSize, szSPFolder, _T("::SystemDrive::") );
	}

	return bRet;
}


//////////////////////////////////////////////////////////////////////////
//	Private
//////////////////////////////////////////////////////////////////////////


/**
 *	获得特殊的文件目录
 */
BOOL CCompressedPath::GetSpecialFolderPath( INT nFolder, LPTSTR lpszPath, DWORD dwPSize )
{
	if ( NULL == lpszPath || 0 == dwPSize )
	{
		return FALSE;
	}

	BOOL bRet = FALSE;
	TCHAR szFolder[ MAX_PATH ] = {0};

	switch( nFolder )
	{
	case SPPATH_SYSTEMDRIVE:	// C:  系统硬盘
		{
			if ( GetWindowsDirectory( szFolder, sizeof(szFolder) ) > 0 )
			{
				szFolder[2] = 0;
				_sntprintf( lpszPath, dwPSize-sizeof(TCHAR), _T("%s"), szFolder );
				bRet = TRUE;
			}
		}
		break;
	case SPPATH_WINDIR:		// C:\WINNT  Windows目录
		{
			if ( GetWindowsDirectory( szFolder, sizeof(szFolder) ) > 0 )
			{
				_sntprintf( lpszPath, dwPSize-sizeof(TCHAR), _T("%s"), szFolder );
				bRet = TRUE;
			}
		}
		break;
	case SPPATH_SYSTEMROOT:		// C:\WINNT\SYSTEM32
	case SPPATH_WINSYSTEM:
		{
			if ( GetSystemDirectory( szFolder, sizeof(szFolder) ) > 0 )
			{
				_sntprintf( lpszPath, dwPSize-sizeof(TCHAR), _T("%s"), szFolder );
				bRet = TRUE;
			}
		}
		break;
	case SPPATH_PROGRAMFILES:	// C:\Program Files
		{
			if ( SHGetSpecialFolderPath( NULL, szFolder, CSIDL_PROGRAM_FILES, FALSE ) )
			{
				_sntprintf( lpszPath, dwPSize-sizeof(TCHAR), _T("%s"), szFolder );
				bRet = TRUE;
			}
		}
		break;
	case SPPATH_USERPROFILE:	// C:\Documents and Settings\Administrator 其中 Administrator 是当前用户，可变的
		{
			if ( ExpandEnvironmentStrings( _T("%UserProfile%"), szFolder, sizeof(szFolder) ) > 0 )
			{
				_sntprintf( lpszPath, dwPSize-sizeof(TCHAR), _T("%s"), szFolder );
				bRet = TRUE;
			}
		}
		break;
	case SPPATH_ALLUSERPROFILE:	// C:\Documents and Settings\All Users
		{
			if ( ExpandEnvironmentStrings( _T("%AllUserProfile%"), szFolder, sizeof(szFolder) ) > 0 )
			{
				_sntprintf( lpszPath, dwPSize-sizeof(TCHAR), _T("%s"), szFolder );
				bRet = TRUE;
			}
		}
		break;
	case SPPATH_COMMON_DESKTOPDIRECTORY:
		{
			if ( SHGetSpecialFolderPath( NULL, szFolder, CSIDL_COMMON_DESKTOPDIRECTORY, FALSE ) )
			{
				_sntprintf( lpszPath, dwPSize-sizeof(TCHAR), _T("%s"), szFolder );
				bRet = TRUE;
			}
		}
		break;
	case SPPATH_COMMON_DOCUMENTS:
		{
			if ( SHGetSpecialFolderPath( NULL, szFolder, CSIDL_COMMON_DOCUMENTS, FALSE ) )
			{
				_sntprintf( lpszPath, dwPSize-sizeof(TCHAR), _T("%s"), szFolder );
				bRet = TRUE;
			}
		}
		break;
	case SPPATH_DESKTOPDIRECTORY:
		{
			if ( SHGetSpecialFolderPath( NULL, szFolder, CSIDL_DESKTOPDIRECTORY, FALSE ) )
			{
				_sntprintf( lpszPath, dwPSize-sizeof(TCHAR), _T("%s"), szFolder );
				bRet = TRUE;
			}
		}
		break;
	case SPPATH_TEMP:		// C:\WINNT\TEMP
	case SPPATH_TMP:		// C:\WINNT\TEMP
		{
			if ( GetTempPath( sizeof(szFolder), szFolder ) > 0 )
			{
				_sntprintf( lpszPath, dwPSize-sizeof(TCHAR), _T("%s"), szFolder );
				bRet = TRUE;
			}
		}
		break;
	}

	// ..
	return bRet;
}

BOOL CCompressedPath::LeftPathReplace( LPCTSTR lpszSrcPath, LPTSTR lpszDstPath, DWORD dwDPSize, LPCTSTR lpszFind, LPCTSTR lpszRpWith )
{
	BOOL bRet = FALSE;
	if ( lpszSrcPath && lpszDstPath && lpszFind && lpszRpWith && _tcslen(lpszFind) > 0 )
	{
		if ( NULL == _tcsstr(lpszSrcPath, _T("::") ) )
		{
			if ( _tcslen(lpszSrcPath) > _tcslen(lpszFind) &&
				0 == _tcsnicmp( lpszFind, lpszSrcPath, _tcslen(lpszFind) ) )
			{
				bRet = TRUE;
				_sntprintf( lpszDstPath, dwDPSize-sizeof(TCHAR), _T("%s%s"),
						lpszRpWith, (lpszSrcPath+_tcslen(lpszFind)) );
			}
		}
	}
	return bRet;
}