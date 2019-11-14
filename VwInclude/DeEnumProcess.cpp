// DeEnumProcess.cpp: implementation of the CDeEnumProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeEnumProcess.h"

#include "psapi.h"
#include <tlhelp32.h>
#include <vdmdbg.h>

#include "RcReader.h"

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CDeEnumProcess::CDeEnumProcess( HINSTANCE hCallerInstance )
{
	//	提升权限
	UpPrivilege( SE_DEBUG_NAME, TRUE );
	memset( m_szSpecProcessName, 0, sizeof(m_szSpecProcessName) );
}
CDeEnumProcess::~CDeEnumProcess()
{
	//	降低权限
	UpPrivilege( SE_DEBUG_NAME, FALSE );
	FreeProcessArray();
}

/**
 *	@ Public
 *	玫举当前系统所有进程，并将信息保存到数组
 */
BOOL CDeEnumProcess::EnumProcess( CSimpleArray<STPROCESSINFO> * pArrProcess )
{
	BOOL  bRet			= FALSE;
	TCHAR szError[ MAX_PATH ]	= {0};
	INT   i;

	if ( DoProcessEnum( szError ) )
	{
		bRet = TRUE;
		if ( pArrProcess )
		{
			//	如果用户设置了需要返回的数组，則将结果拷贝给用户
			//*pArrProcess = m_aProcessInfo;

			pArrProcess->RemoveAll();
			for ( i = 0; i < m_aProcessInfo.GetSize(); i ++ )
			{
				pArrProcess->Add( m_aProcessInfo[ i ] );
			}

			DWORD dwA = pArrProcess->GetSize();
			DWORD dwB = m_aProcessInfo.GetSize();
		}
	}

	return bRet;
}

/**
 *	@ Public
 *	设置需要收集其所有有用模块的进程名称
 */
BOOL CDeEnumProcess::SetCollectProcessName( LPCTSTR lpcszProcessName )
{
	//
	//	pszProcessName	- [in] 指定收集某个进程的所有模块，例如：“explorer.exe”
	//	RETURN		- TRUE / FALSE
	//
	BOOL bRet = FALSE;
	if ( lpcszProcessName )
	{
		bRet = TRUE;
		_sntprintf( m_szSpecProcessName, sizeof(m_szSpecProcessName)-sizeof(TCHAR), "%s", lpcszProcessName );
	}
	return bRet;
}

/**
 *	@ Public
 *	释放内存
 */
BOOL CDeEnumProcess::FreeProcessArray()
{
	m_aProcessInfo.RemoveAll();
	//m_aSpecModule.RemoveAll();

	return TRUE;
}

/**
 *	@ Public
 *	根据索引获取元素
 */
BOOL CDeEnumProcess::GetItemByIndex( INT nIndex, LPTSTR lpszJsonString, DWORD dwSize, DWORD * pdwDataLen )
{
	//	nIndex		- [in] 如果 -1 则返回总数，否则返回具体的某个进程的信息
	//	lpszJsonString	- [in] 返回缓冲区
	//	dwSize		- [in] 返回缓冲区大小
	//	pdwDataLen	- [in] 数据长度
	//	RETURN		- TRUE / FLASE
	//

	if ( nIndex >= m_aProcessInfo.GetSize() )
	{
		return FALSE;
	}
	
	TCHAR szBuffer[ MAX_PATH ];
	string strTemp;
	STPROCESSINFO * pstItem;

	//
	//	开始输出 JSON 结果
	//
	
	//	total item count
	_sntprintf( szBuffer, sizeof(szBuffer)-sizeof(TCHAR), "%u", m_aProcessInfo.GetSize() );

	strTemp  = "";
	strTemp += "{\"result\":1";
	strTemp += ",\"error\":\"\",\"total\":";
	strTemp += szBuffer;
	strTemp += ",\"vdata\":[";	

	if ( nIndex >= 0 )
	{
		pstItem = & m_aProcessInfo[ nIndex ];
		if ( pstItem )
		{
			_sntprintf( szBuffer, sizeof(szBuffer)-sizeof(TCHAR), "%u", pstItem->dwProcID );
			strTemp += "\"dwProcID\":";
			strTemp += szBuffer;
			strTemp += ",";

			strTemp += "\"szProcName\":\"";
			strTemp += pstItem->szProcName;
			strTemp += "\",";

			strTemp += "\"szProcPath\":\"";
			strTemp += pstItem->szProcPath;
			strTemp += "\",";

			_sntprintf( szBuffer, sizeof(szBuffer)-sizeof(TCHAR), "%u", pstItem->dwFileSize );
			strTemp += "\"dwFileSize\":";
			strTemp += szBuffer;
			strTemp += ",";

			strTemp += "\"szVersion\":\"";
			strTemp += pstItem->szVersion;
			strTemp += "\",";

			_sntprintf( szBuffer, sizeof(szBuffer)-sizeof(TCHAR), "%u", pstItem->dwMemoryUsed );
			strTemp += "\"dwMemoryUsed\":";
			strTemp += szBuffer;
			strTemp += ",";

			_sntprintf( szBuffer, sizeof(szBuffer)-sizeof(TCHAR), "%s", pstItem->szCompany );
			delib_replace( szBuffer, "\"", "\\\"" );
			strTemp += "\"szCompany\":\"";
			strTemp += szBuffer;
			strTemp += "\",";

			strTemp += "\"szFileMd5\":\"";
			strTemp += pstItem->szFileMd5;
			strTemp += "\",";

			strTemp += "\"Reserved\":0}";
		}
	}
	else
	{
		//	通过 total 字段返回给调用者：当前进程总数
	}

	strTemp += "]}";

	_sntprintf( lpszJsonString, dwSize-sizeof(TCHAR), "%s", strTemp.c_str() );
	*pdwDataLen = strTemp.length();

	return TRUE;
}


/**
 *	对进程数组排序
 */
static int __cdecl _cmpfunc_EnumProcess_ProcName( const void * l, const void *r )
{
	STPROCESSINFO * p1=(STPROCESSINFO * )l;
	STPROCESSINFO * p2=(STPROCESSINFO * )r;
	return _tcsicmp( p1->szProcName, p2->szProcName );
}
static int __cdecl _cmpfunc_EnumProcess_FileSize( const void * l, const void *r )
{
	STPROCESSINFO * p1=(STPROCESSINFO * )l;
	STPROCESSINFO * p2=(STPROCESSINFO * )r;
	return ( p1->dwFileSize - p2->dwFileSize );
}
static int __cdecl _cmpfunc_EnumProcess_Company( const void * l, const void *r )
{
	STPROCESSINFO * p1=(STPROCESSINFO * )l;
	STPROCESSINFO * p2=(STPROCESSINFO * )r;
	return _tcsicmp( p1->szCompany, p2->szCompany );
}
BOOL CDeEnumProcess::SortProcessArray( DWORD dwSortType )
{
	switch( dwSortType )
	{
	case DEENUMPROCESS_SORT_BY_PROCESS_NAME:
		{
			qsort
			(
				m_aProcessInfo.GetData(), 
				m_aProcessInfo.GetSize(), 
				sizeof(STPROCESSINFO), _cmpfunc_EnumProcess_ProcName
			);
		}
		break;
		
	case DEENUMPROCESS_SORT_BY_PROCESS_SIZE:
		{
			qsort
			(
				m_aProcessInfo.GetData(), 
				m_aProcessInfo.GetSize(), 
				sizeof(STPROCESSINFO), _cmpfunc_EnumProcess_FileSize
			);
		}
		break;
		
	case DEENUMPROCESS_SORT_BY_PROCESS_COMPANY :
		{
			qsort
			(
				m_aProcessInfo.GetData(), 
				m_aProcessInfo.GetSize(), 
				sizeof(STPROCESSINFO), _cmpfunc_EnumProcess_Company
			);
		}
		break;
		
	default :
		{
			qsort
			(
				m_aProcessInfo.GetData(), 
				m_aProcessInfo.GetSize(), 
				sizeof(STPROCESSINFO), _cmpfunc_EnumProcess_ProcName
			);
		}
		break;
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// 收集特殊模块信息
BOOL CDeEnumProcess::CollectSpecModule( LPTSTR pszFilePath )
{
	if ( pszFilePath && _tcslen(pszFilePath) > 0 && PathFileExists(pszFilePath) )
	{
		// Microsoft Corporation
		BOOL bMicrosoft = FALSE;
		CRcReader rcread;
		TCHAR szError[ MAX_PATH ] = {0};
		TCHAR szCompany[ MAX_PATH ] = {0};
		STSPECMODULE stSpecModule;
		BOOL bExist = FALSE;
		INT  i = 0;

		rcread.m_pszFilePath = pszFilePath;
		if ( rcread.InitInfo( szError ) )
		{
			if ( rcread.GetCompanyName( szCompany, sizeof(szCompany), szError ) )
			{
				if ( 0 == _tcsicmp( szCompany, "Microsoft Corporation" ) )
				{
					bMicrosoft = TRUE;
				}
			}
			rcread.FreeInfo();
		}

		if ( ! bMicrosoft )
		{
			_sntprintf( stSpecModule.szFilePath, sizeof(stSpecModule.szFilePath)-sizeof(TCHAR), "%s", pszFilePath );
			for ( i = 0; i < m_aSpecModule.GetSize(); i ++ )
			{
				if ( 0 == _tcsicmp(m_aSpecModule[i].szFilePath,pszFilePath) )
				{
					bExist = TRUE;
					break;
				}
			}
			if ( ! bExist )
				m_aSpecModule.Add( stSpecModule );
		}
	}
	return TRUE;
}








//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//	Private


BOOL CDeEnumProcess::UpPrivilege( TCHAR * pszPrivilege, BOOL bEnable )
{
	if ( NULL == pszPrivilege )
	{
		return FALSE;
	}
	else if ( 0 == _tcslen(pszPrivilege) )
	{
		return FALSE;
	}
	
	HANDLE		hToken;
	TOKEN_PRIVILEGES tp;
	
	//
	// obtain the token, first check the thread and then the process
	//
	if ( ! OpenThreadToken( GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, TRUE, &hToken ) )
	{
		if ( ERROR_NO_TOKEN == GetLastError() )
		{
			if ( ! OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken ) )
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
	}
	
	//
	// get the luid for the privilege
	//
	if ( ! LookupPrivilegeValue(NULL, pszPrivilege, &tp.Privileges[0].Luid) )
	{
		return FALSE;
	}
	
	tp.PrivilegeCount = 1;
	
	if ( bEnable )
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;
	
	//
	// enable or disable the privilege
	//
	if ( ! AdjustTokenPrivileges(hToken, FALSE, &tp, 0, (PTOKEN_PRIVILEGES)NULL, 0) )
		return FALSE;
	
	if ( !CloseHandle(hToken) )
		return FALSE;
	
	return TRUE;
}


/**
 *	@ private
 *	开始玫举当前系统所有进程，并将信息保存到数组
 */
BOOL CDeEnumProcess::DoProcessEnum( LPTSTR lpszError )
{
	////////////////////////////////////////
	BOOL bIsAdded			= FALSE;
	STPROCESSINFO	stInfo;
	ENUMWINDOWSSYSTYPE	osType;

	////////////////////////////////////////
	HINSTANCE	hInstLib	= NULL;
	HANDLE		hSnapShot	= NULL;
	HANDLE		hModuleSnap	= NULL;

	////////////////////////////////////////
	PROCESSENTRY32 procentry;
	MODULEENTRY32 me32;

	////////////////////////////////////////
	BOOL bFlag			= FALSE;
	DWORD dwSize			= 0;
	HMODULE hMods[ 1024 ]		= {0};
	HANDLE hProcess			= NULL;

	TCHAR szTemp[ MAX_PATH ]	= {0};
	TCHAR szModProPath[ MAX_PATH ]	= {0};


	////////////////////////////////////////
	// ToolHelp Function Pointers
	HANDLE (WINAPI *lpfCreateToolhelp32Snapshot)( DWORD, DWORD );
	BOOL (WINAPI *lpfProcess32First)( HANDLE, LPPROCESSENTRY32 );
	BOOL (WINAPI *lpfProcess32Next)( HANDLE, LPPROCESSENTRY32 );
	BOOL (WINAPI *lpfModule32First)( HANDLE, LPMODULEENTRY32 );
	BOOL (WINAPI *lpfModule32Next)( HANDLE, LPMODULEENTRY32 );


	////////////////////////////////////////
	// Get OS type
	osType = delib_get_shellsystype();
	
	
	//////////////////////////////////////////////////////////////////////////
	// 判断操作系统是否是被支持的
	//////////////////////////////////////////////////////////////////////////
	if ( _OS_WINDOWS_98 != osType && _OS_WINDOWS_ME != osType && _OS_WINDOWS_2000 != osType && _OS_WINDOWS_XP != osType )
	{
		_tcscpy( lpszError, _T("NOT SUPPORT OS.") );
		return FALSE;
	}
	
	
	__try
	{
		hInstLib = LoadLibrary( "Kernel32.DLL" );
		if ( NULL == hInstLib )
		{
			_tcscpy( lpszError, _T("Unable to load Kernel32.DLL") );
			return FALSE;
		}

		lpfCreateToolhelp32Snapshot =
			(HANDLE (WINAPI *)(DWORD,DWORD))GetProcAddress(hInstLib, "CreateToolhelp32Snapshot");
		lpfProcess32First = 
			(BOOL (WINAPI *)(HANDLE,LPPROCESSENTRY32))GetProcAddress(hInstLib, "Process32First");
		lpfProcess32Next =
			(BOOL (WINAPI *)(HANDLE,LPPROCESSENTRY32))GetProcAddress(hInstLib, "Process32Next");
		lpfModule32First =
			(BOOL (WINAPI *)(HANDLE,LPMODULEENTRY32))GetProcAddress(hInstLib, "Module32First");
		lpfModule32Next =
			(BOOL (WINAPI *)(HANDLE,LPMODULEENTRY32))GetProcAddress(hInstLib, "Module32Next");
		
		
		if ( NULL == lpfProcess32Next || NULL == lpfProcess32First || 
			NULL == lpfModule32First || NULL == lpfModule32Next ||
			NULL == lpfCreateToolhelp32Snapshot )
		{
			_tcscpy( lpszError, _T("Unable to get function address in Kernel32.DLL") );
			return FALSE;
		}
		
		////////////////////////////////////////
		// Get a handle to a Toolhelp snapshot of all processes
		hSnapShot = lpfCreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
		
		if ( INVALID_HANDLE_VALUE == hSnapShot )
		{
			_tcscpy( lpszError, _T("Unable to snap process by CreateToolhelp32Snapshot") );
			return FALSE;
		}

		// Get the first process' information
		procentry.dwSize = sizeof( PROCESSENTRY32 );
		bFlag = lpfProcess32First( hSnapShot, &procentry );
		

		////////////////////////////////////////////////////////////
		// --- While there are processes, keep looping
		while ( bFlag )
		{
			bIsAdded = FALSE;

			// 处理系统一些特殊的进程比如：[System] ProcessID=0
			if ( 0 == procentry.th32ProcessID )
			{
				stInfo.dwProcID = procentry.th32ProcessID;
				stInfo.dwMemoryUsed = procentry.dwSize;
				_tcscpy( stInfo.szProcName, _T("SysIdleProcess") );
				_tcscpy( stInfo.szProcPath, _T("") );
				SaveProcessInfo( &stInfo, lpszError );
				// ...
				goto Flag_Snap_Next_Process;
			}
			// Take a snapshot of all modules in the specified process
			hModuleSnap = lpfCreateToolhelp32Snapshot( TH32CS_SNAPMODULE, procentry.th32ProcessID );
			if ( INVALID_HANDLE_VALUE == hModuleSnap )
			{
				// 可能是 C:\pagefile.sys
				stInfo.dwProcID = procentry.th32ProcessID;
				stInfo.dwMemoryUsed = procentry.dwSize;
				_sntprintf( stInfo.szProcName, sizeof(stInfo.szProcName)-sizeof(TCHAR), _T("%s"), _T(procentry.szExeFile) );
				_tcscpy( stInfo.szProcPath, _T("") );
				SaveProcessInfo( &stInfo, lpszError );
				// ...
				goto Flag_Snap_Next_Process;
			}

			if ( ! bIsAdded && ( _OS_WINDOWS_98 == osType || _OS_WINDOWS_ME == osType ) )
			{
				// For Windows 98, me
				bIsAdded = TRUE;

				stInfo.dwProcID = procentry.th32ProcessID;
				stInfo.dwMemoryUsed = 0;
				_sntprintf( stInfo.szProcName, sizeof(stInfo.szProcName)-sizeof(TCHAR),
					_T("%s"), _T( PathFindFileName(procentry.szExeFile)) );
				_sntprintf( stInfo.szProcPath, sizeof(stInfo.szProcPath)-sizeof(TCHAR),
					_T("%s"), _T(procentry.szExeFile) );
				// ..
				SaveProcessInfo( &stInfo, lpszError );
				//goto Flag_Snap_Next_Process;
			}

			//////////////////////////////////////////////////
			// loop all mudules for special process
			me32.dwSize = sizeof( MODULEENTRY32 );
			if ( lpfModule32First( hModuleSnap, &me32 ) )
			{
				do
				{
					memset( szTemp, 0, sizeof(szTemp) );
					memset( szModProPath, 0, sizeof(szModProPath) );
					if ( ! CorrectSpecSysPath( me32.szExePath, szModProPath ) )
					{
						// 如果没有特殊路径的话
						_tcscpy( szModProPath, me32.szExePath );
					}

					if ( GetShortPathName( _T(szModProPath), szTemp, sizeof(szTemp) ) > 0 )
					{
						if ( ! bIsAdded && ( _OS_WINDOWS_2000 == osType || _OS_WINDOWS_XP == osType ) )
						{
							// For Windows 2000, XP
							bIsAdded = TRUE;

							stInfo.dwProcID = me32.th32ProcessID;
							stInfo.dwMemoryUsed = me32.modBaseSize;
							_sntprintf( stInfo.szProcName, sizeof(stInfo.szProcName)-sizeof(TCHAR),
								_T("%s"), _T(me32.szModule) );
							_sntprintf( stInfo.szProcPath, sizeof(stInfo.szProcPath)-sizeof(TCHAR),
								_T("%s"), _T(szModProPath) );
							// ..
							SaveProcessInfo( &stInfo, lpszError );
							//goto Flag_Snap_Next_Process;
						}

						//	到这里，已经确定了这个进程的进程名称
						//	我们需要将 iexplore.exe, lsass.exe 两个进程中的所有可疑模块送会服务器
						if ( _tcslen( m_szSpecProcessName ) > 0 )
						{
							if ( 0 == _tcsicmp( stInfo.szProcName, m_szSpecProcessName ) )
							{
								CollectSpecModule( szModProPath );
							}
						}
					}
				}
				while ( Module32Next( hModuleSnap, &me32 ) );
			}

			////////////////////
Flag_Snap_Next_Process:

			if ( hModuleSnap )
			{
				CloseHandle( hModuleSnap );
				hModuleSnap = NULL;
			}

			procentry.dwSize = sizeof( PROCESSENTRY32 );
			bFlag = Process32Next( hSnapShot, &procentry );

		} // loop enom process
		////////////////////////////////////////////////////////////

	}
	__finally
	{
		if ( hProcess )
			CloseHandle( hProcess );
		if ( hInstLib )
			FreeLibrary( hInstLib );
	}


	////////////////////////////////////////
	//
	if ( hProcess )
		CloseHandle( hProcess );
	
	if ( hInstLib )
		FreeLibrary( hInstLib );


	return TRUE;

}


BOOL CDeEnumProcess::CorrectSpecSysPath( TCHAR * pszPathSpec, TCHAR * pszPathOut )
{
	if ( NULL == pszPathSpec || NULL == pszPathOut )
	{
		return FALSE;
	}
	else if ( 0 == _tcslen(pszPathSpec) )
	{
		return FALSE;
	}
	
	TCHAR szTemp[ MAX_PATH ]	= {0};
	TCHAR szWindowsDir[ MAX_PATH ]	= {0};
	
	memset( szTemp, 0, sizeof(szTemp) );
	memcpy( szTemp, pszPathSpec, _tcslen("\\??\\") );
	if ( 0 == _tcsicmp( _T("\\??\\"), szTemp ) )
	{
		memcpy( pszPathOut, pszPathSpec + _tcslen("\\??\\"), _tcslen(pszPathSpec) - _tcslen("\\??\\") );
		return TRUE;
	}
	
	memset( szTemp, 0, sizeof(szTemp) );
	memcpy( szTemp, pszPathSpec, _tcslen("\\SystemRoot\\") );
	if ( 0 == _tcsicmp( _T("\\SystemRoot\\"), szTemp ) )
	{
		_tcscpy( pszPathOut, _T("") );
		DWORD dwWDLen = GetWindowsDirectory( szWindowsDir, sizeof(szWindowsDir) );
		if ( dwWDLen > 3 )
		{
			_tcscpy( pszPathOut, szWindowsDir );
			_tcscat( pszPathOut, _T("\\") );
		}
		else if ( 3 == dwWDLen )
		{
			_tcscpy( pszPathOut, szWindowsDir );
		}
		
		memcpy( szTemp, 
			pszPathSpec + _tcslen("\\SystemRoot\\"), 
			_tcslen(pszPathSpec) - _tcslen("\\SystemRoot\\") );
		_tcscat( pszPathOut, szTemp );
		return TRUE;
	}
	
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
// 获取 CollectSpecModule() 收集的文件列表
BOOL CDeEnumProcess::GetColSpecModules( LPTSTR pszFileList, DWORD dwSize )
{
	INT i = 0;
	TCHAR szTemp[ MAX_PATH ] = {0};
	
	if ( pszFileList )
	{
		_sntprintf( pszFileList, dwSize-sizeof(TCHAR), "c=%d|||", m_aSpecModule.GetSize() );
		for ( i = 0; i < m_aSpecModule.GetSize(); i ++ )
		{
			if ( _tcslen(pszFileList) + _tcslen(m_aSpecModule[i].szFilePath) < ( dwSize - 6 - sizeof(TCHAR) ) )
			{
				// f1=C:\Program Files\Winamp3\winampa.exe|||
				_sntprintf( szTemp, sizeof(szTemp)-sizeof(TCHAR), "f%d=%s|||", (i+1), m_aSpecModule[i].szFilePath );
				_tcscat( pszFileList, szTemp );
			}
		}
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//
BOOL CDeEnumProcess::SaveProcessInfo( STPROCESSINFO * pstInfo, LPTSTR lpszError )
{
	if ( NULL == pstInfo )
	{
		return FALSE;
	}

	STPROCESSINFO * pstHead			= NULL;
	STPROCESSINFO * pstTemp			= NULL;
	STPROCESSINFO * pstNewInfo		= NULL;
	TCHAR szWindowTitle[ MAX_PATH ]		= {0};
	TCHAR szVersion[ MAX_PATH ]		= {0};
	TCHAR szCompany[ MAX_PATH ]		= {0};
	TCHAR szError[ MAX_PATH ]		= {0};
	INT   i					= 0;

	for ( i = 0; i < m_aProcessInfo.GetSize(); i ++ )
	{
		if ( pstInfo->dwProcID == m_aProcessInfo[i].dwProcID )
		{
			return FALSE;
		}
	}
	
	//
	// 将数据拷贝给新成员
	//
	STPROCESSINFO stNewInfo;

	stNewInfo.dwProcID = pstInfo->dwProcID;

	memset( stNewInfo.szWindowTitle, 0, sizeof(stNewInfo.szWindowTitle) );
	/*
	if ( TRUE == GetWindowTitle( pstInfo->dwProcID, szWindowTitle, sizeof(szWindowTitle), 5 ) )
	{
		_sntprintf( stNewInfo.szWindowTitle, sizeof(stNewInfo.szWindowTitle)-sizeof(TCHAR), _T("%s"), _T(szWindowTitle) );
	}
	*/

	// File Name
	_sntprintf( stNewInfo.szProcName, sizeof(stNewInfo.szProcName)-sizeof(TCHAR), _T("%s"), pstInfo->szProcName );
	
	// File Path
	_sntprintf( stNewInfo.szProcPath, sizeof(stNewInfo.szProcPath)-sizeof(TCHAR), _T("%s"), pstInfo->szProcPath );
	
	// File md5
	delib_get_file_md5( stNewInfo.szProcPath, stNewInfo.szFileMd5, sizeof(stNewInfo.szFileMd5), szError );
	
	// Get Compressed Proc Path
	CCompressedPath::GetCompressedPath( stNewInfo.szProcPath, stNewInfo.szCompressPath, sizeof(stNewInfo.szCompressPath), NULL, 0 );

	// Get File Icon
	//SHFILEINFO stSfi;
	//SHGetFileInfo( stNewInfo.szProcPath, 0, &stSfi, sizeof(stSfi), SHGFI_SYSICONINDEX|SHGFI_SMALLICON );
	//stSfi.hIcon

	memset( stNewInfo.szVersion, 0, sizeof(stNewInfo.szVersion) );
	memset( stNewInfo.szCompany, 0, sizeof(stNewInfo.szCompany) );
	
	CRcReader rcread;
	rcread.m_pszFilePath = pstInfo->szProcPath;
	if ( rcread.InitInfo( szError ) )
	{
		if ( rcread.GetFileVersion( szVersion, sizeof(szVersion), szError ) )
		{
			_sntprintf( stNewInfo.szVersion, sizeof(stNewInfo.szVersion), _T("%s"), szVersion );
		}
		if ( rcread.GetCompanyName( szCompany, sizeof(szCompany), szError ) )
		{
			_sntprintf( stNewInfo.szCompany, sizeof(stNewInfo.szCompany), _T("%s"), szCompany );
		}
		rcread.FreeInfo();
	}

	stNewInfo.dwMemoryUsed = pstInfo->dwMemoryUsed;
	
	// Get File Size
	stNewInfo.dwFileSize = delib_get_file_size( pstInfo->szProcPath );
	
	//
	// 将新成员
	//
	m_aProcessInfo.Add( stNewInfo );

	return TRUE;
}
