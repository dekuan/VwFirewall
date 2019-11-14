// VwIISAppPoolChecker.cpp: implementation of the CVwIISAppPoolChecker class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VwIISAppPoolChecker.h"
#include <tlhelp32.h>

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVwIISAppPoolChecker::CVwIISAppPoolChecker()
{
	// 提升权限
	UpPrivilege( SE_DEBUG_NAME, TRUE );
}

CVwIISAppPoolChecker::~CVwIISAppPoolChecker()
{
	// 降低权限
	UpPrivilege( SE_DEBUG_NAME, FALSE );
}

BOOL CVwIISAppPoolChecker::IsIISAppPoolRunning()
{
	BOOL bRet;
	VWIISAPPPOOLCHECKERWINDOWSSYSTYPE osType;
	vector<STVWIISAPPPOOLCHECKERPROCESSINFO> vcProcessList;
	vector<STVWIISAPPPOOLCHECKERPROCESSINFO>::iterator it;
	STVWIISAPPPOOLCHECKERPROCESSINFO * pstProcInfo;
	TCHAR szIISAppPoolProcessName[ 64 ];
	TCHAR szError[ MAX_PATH ];

	bRet	= FALSE;

	memset( szIISAppPoolProcessName, 0, sizeof(szIISAppPoolProcessName) );
	memset( szError, 0, sizeof(szError) );
	vcProcessList.clear();
	if ( EnumProcess( vcProcessList, szError ) )
	{
		osType	= GetShellSystemType();
		if ( VWIISAPPPOOLCHECKER_WINDOWS_2003 == osType ||
			VWIISAPPPOOLCHECKER_WINDOWS_2008 == osType ||
			VWIISAPPPOOLCHECKER_WINDOWS_VISTA == osType ||
			VWIISAPPPOOLCHECKER_WINDOWS_2008_R2 == osType ||
			VWIISAPPPOOLCHECKER_WINDOWS_7 == osType )
		{
			_sntprintf( szIISAppPoolProcessName, sizeof(szIISAppPoolProcessName)/sizeof(TCHAR)-1, _T("%s"), _T("w3wp.exe") );
		}
		else
		{
			_sntprintf( szIISAppPoolProcessName, sizeof(szIISAppPoolProcessName)/sizeof(TCHAR)-1, _T("%s"), _T("inetinfo.exe") );
		}
	
		for ( it = vcProcessList.begin(); it != vcProcessList.end(); it ++ )
		{
			pstProcInfo = it;
			if ( pstProcInfo )
			{
				if ( 0 == _tcsicmp( pstProcInfo->szProcName, szIISAppPoolProcessName ) )
				{
					bRet = TRUE;
					break;
				}	
			}
		}	
	}
	vcProcessList.clear();

	return bRet;
}

//	玫举当前系统所有进程，并将信息保存到链表
BOOL CVwIISAppPoolChecker::EnumProcess( vector<STVWIISAPPPOOLCHECKERPROCESSINFO> & vcProcessList, TCHAR * pszError )
{
	BOOL bRet;
	STVWIISAPPPOOLCHECKERPROCESSINFO stInfo;
	HINSTANCE hInstLib;
	HANDLE hSnapShot;
	PROCESSENTRY32 procentry;
	BOOL bFlag;


	bRet = FALSE;

	//	ToolHelp Function Pointers
	HANDLE (WINAPI *lpfCreateToolhelp32Snapshot)( DWORD, DWORD );
	BOOL   (WINAPI *lpfProcess32First)( HANDLE, LPPROCESSENTRY32 );
	BOOL   (WINAPI *lpfProcess32Next)( HANDLE, LPPROCESSENTRY32 );
	BOOL   (WINAPI *lpfModule32First)( HANDLE, LPMODULEENTRY32 );
	BOOL   (WINAPI *lpfModule32Next)( HANDLE, LPMODULEENTRY32 );

	__try
	{
		hInstLib = LoadLibrary( "Kernel32.DLL" );
		if ( hInstLib )
		{
			lpfCreateToolhelp32Snapshot	= (HANDLE (WINAPI *)(DWORD,DWORD))GetProcAddress( hInstLib, "CreateToolhelp32Snapshot" );
			lpfProcess32First		= (BOOL (WINAPI *)(HANDLE,LPPROCESSENTRY32))GetProcAddress( hInstLib, "Process32First" );
			lpfProcess32Next		= (BOOL (WINAPI *)(HANDLE,LPPROCESSENTRY32))GetProcAddress( hInstLib, "Process32Next" );
			lpfModule32First		= (BOOL (WINAPI *)(HANDLE,LPMODULEENTRY32))GetProcAddress( hInstLib, "Module32First" );
			lpfModule32Next			= (BOOL (WINAPI *)(HANDLE,LPMODULEENTRY32))GetProcAddress( hInstLib, "Module32Next" );

			if ( lpfProcess32Next && lpfProcess32First && lpfModule32First && lpfModule32Next && lpfCreateToolhelp32Snapshot )
			{
				//	Get a handle to a Toolhelp snapshot of all processes
				hSnapShot = lpfCreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
				if ( INVALID_HANDLE_VALUE != hSnapShot )
				{
					//	...
					bRet = TRUE;

					//	Get the first process' information
					procentry.dwSize = sizeof( PROCESSENTRY32 );
					bFlag = lpfProcess32First( hSnapShot, &procentry );

					////////////////////////////////////////////////////////////
					// --- While there are processes, keep looping
					while ( bFlag )
					{
						stInfo.dwProcID		= procentry.th32ProcessID;
						stInfo.dwMemoryUsed	= procentry.dwSize;

						if ( 0 == procentry.th32ProcessID )
						{
							//	处理系统一些特殊的进程比如：[System] ProcessID=0
							_tcscpy( stInfo.szProcName, _T("SysIdleProcess") );
						}
						else
						{
							//	可能是 C:\pagefile.sys
							_sntprintf( stInfo.szProcName, sizeof(stInfo.szProcName)/sizeof(TCHAR)-1, _T("%s"), _T(procentry.szExeFile) );
						}
						vcProcessList.push_back( stInfo );

						procentry.dwSize = sizeof( PROCESSENTRY32 );
						bFlag = Process32Next( hSnapShot, &procentry );
					}

					CloseHandle( hSnapShot );
					hSnapShot = NULL;
				}
				else
				{
					_tcscpy( pszError, _T("无法使用 CreateToolhelp32Snapshot 拍照进程") );
				}
			}
			else
			{
				_tcscpy( pszError, _T("无法定位 Kernel32.DLL 中的函数地址") );
			}

			//	...
			FreeLibrary( hInstLib );
			hInstLib = NULL;
		}
		else
		{
			_tcscpy( pszError, _T("无法装载 Kernel32.DLL") );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return bRet;
}








//////////////////////////////////////////////////////////////////////////
//	Private


BOOL CVwIISAppPoolChecker::UpPrivilege( TCHAR * pszPrivilege, BOOL bEnable )
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
 *	Get Windows system type
 */
VWIISAPPPOOLCHECKERWINDOWSSYSTYPE CVwIISAppPoolChecker::GetShellSystemType()
{
	VWIISAPPPOOLCHECKERWINDOWSSYSTYPE ShellType;
	DWORD dwWinVer;
	//OSVERSIONINFO * osvi;
	OSVERSIONINFOEX stOsVerEx;

	ZeroMemory( & stOsVerEx, sizeof(stOsVerEx) );

	dwWinVer = GetVersion();
	if ( dwWinVer < 0x80000000 )
	{
		// NT
		ShellType = VWIISAPPPOOLCHECKER_WINDOWS_NT3;
		stOsVerEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		if ( GetVersionEx( (OSVERSIONINFO*)&stOsVerEx ) )
		{
			if ( 4L == stOsVerEx.dwMajorVersion )
			{
				ShellType = VWIISAPPPOOLCHECKER_WINDOWS_NT4;
			}
			else if ( 5L == stOsVerEx.dwMajorVersion && 0L == stOsVerEx.dwMinorVersion )
			{
				ShellType = VWIISAPPPOOLCHECKER_WINDOWS_2000;
			}
			else if ( 5L == stOsVerEx.dwMajorVersion && 1L == stOsVerEx.dwMinorVersion )
			{
				ShellType = VWIISAPPPOOLCHECKER_WINDOWS_XP;
			}
			else if ( 5L == stOsVerEx.dwMajorVersion && 2L == stOsVerEx.dwMinorVersion )
			{
				ShellType = VWIISAPPPOOLCHECKER_WINDOWS_2003;
			}
			else if ( 6L == stOsVerEx.dwMajorVersion && 1L == stOsVerEx.dwMinorVersion )
			{
				if ( VER_NT_WORKSTATION == stOsVerEx.wProductType )
				{
					//	Windows 7
					//	OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
					ShellType = VWIISAPPPOOLCHECKER_WINDOWS_7;
				}
				else
				{
					//	Windows Server 2008 R2
					//	OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
					ShellType = VWIISAPPPOOLCHECKER_WINDOWS_2008_R2;
				}
			}
			else if ( 6L == stOsVerEx.dwMajorVersion && 0L == stOsVerEx.dwMinorVersion )
			{
				if ( VER_NT_WORKSTATION == stOsVerEx.wProductType )
				{
					//	Windows Vista
					//	OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
					ShellType = VWIISAPPPOOLCHECKER_WINDOWS_VISTA;
				}
				else
				{
					//	Windows Server 2008
					//	OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
					ShellType = VWIISAPPPOOLCHECKER_WINDOWS_2008;
				}
			}
		}
	}
	else if ( LOBYTE(LOWORD(dwWinVer)) < 4 )
	{
		ShellType = VWIISAPPPOOLCHECKER_WINDOWS_32S;
	}
	else
	{
		ShellType = VWIISAPPPOOLCHECKER_WINDOWS_95;
		stOsVerEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if ( GetVersionEx( (OSVERSIONINFO*) &stOsVerEx ) )
		{
			if ( 4L == stOsVerEx.dwMajorVersion && 10L == stOsVerEx.dwMinorVersion )
			{
				ShellType = VWIISAPPPOOLCHECKER_WINDOWS_98;
			}
			else if ( 4L == stOsVerEx.dwMajorVersion && 90L == stOsVerEx.dwMinorVersion )
			{
				ShellType = VWIISAPPPOOLCHECKER_WINDOWS_ME;
			}
		}
	}
	
	return ShellType;
}

