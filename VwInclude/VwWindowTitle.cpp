// GetWindowTitle.cpp: implementation of the CGetWindowTitle class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VwWindowTitle.h"



/**
 *	根据进程 ID 获取窗口标题
 */
BOOL WINAPI GetWindowTitle( DWORD dwPID, TCHAR * pszWinTitle, DWORD dwSize, DWORD dwTimeout )
{
	////////////////////////////////////////
	HANDLE hProc		= NULL;
	HANDLE hEvent		= NULL;
	BOOL bIsSucc		= FALSE;
	
	////////////////////////////////////////
	STPARAMS stParams;

	hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

	__try
	{
		hProc = OpenProcess( SYNCHRONIZE|PROCESS_TERMINATE, FALSE, dwPID );
		if ( NULL == hProc )
		{
			return FALSE;
		}

		stParams.hEvent		= hProc;
		stParams.dwPID		= dwPID;
		stParams.pszWinTitle	= pszWinTitle;
		stParams.dwSize		= dwSize;
		stParams.dwTimeout	= dwTimeout;

		ResetEvent( hEvent );
		EnumWindows( (WNDENUMPROC)CallBackGetWindowTitle, (LPARAM)&stParams );

		if ( WaitForSingleObject(hEvent, dwTimeout) != WAIT_OBJECT_0 )
			bIsSucc = FALSE;
		else
			bIsSucc = TRUE;
		if ( _tcslen(stParams.pszWinTitle) > 0 )
			bIsSucc = TRUE;
	}
	__finally
	{
		if ( hProc )
		{
			CloseHandle( hProc );
		}
	}

	return bIsSucc;
	
}


BOOL CALLBACK CallBackGetWindowTitle( HWND hwnd, LPARAM lParam )
{
	DWORD dwID				= 0;
	STPARAMS * pstParams			= NULL;
	TCHAR szWindowTitle[ MAX_PATH ]		= {0};
	HWND hwndParent				= NULL;
	HWND hwndTmp				= NULL;

	pstParams = (STPARAMS *)lParam;

	GetWindowThreadProcessId( hwnd, &dwID );

	if ( dwID == (DWORD)pstParams->dwPID )
	{
		hwndTmp = hwnd;
		hwndParent = GetParent(hwndTmp);
		while ( hwndParent )
		{
			hwndTmp = hwndParent;
			hwndParent = GetParent( hwndTmp );
		}
		if ( NULL == hwndParent )
		{
			GetWindowText( hwndTmp, szWindowTitle, sizeof(szWindowTitle)-sizeof(TCHAR) );
			if ( _tcslen(szWindowTitle) > 0 &&
				0 != _tcscmp( _T("SysFader"), _T(szWindowTitle) ) )
			{
				_sntprintf (
					pstParams->pszWinTitle, 
					pstParams->dwSize-sizeof(TCHAR),
					_T("%s"), szWindowTitle );
				SetEvent( pstParams->hEvent );
				// 返回 FALSE 就停止 EnumWindows
				return FALSE;
			}
		}
	}

	return TRUE;
}