// KillProcess.cpp: implementation of the CKillProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KillProcess.h"
#include <vdmdbg.h>




//////////////////////////////////////////////////////////////////////////
//
BOOL WINAPI KillProcess32ByPID( DWORD dwPID, DWORD dwTimeout, TCHAR * pszError )
{
	DWORD dwResult = KillProcess32( dwPID, dwTimeout );

	if ( TA_SUCCESS_CLEAN == dwResult || TA_SUCCESS_KILL == dwResult )
	{
		return TRUE;
	}
	else
	{
		_tcscpy( pszError, _T("无法结束进程") );
		return FALSE;
	}

}





/*
 * ----------------------------------------------------------------
 * Purpose:
 * Shut down a 32-Bit Process (or 16-bit process under Windows 95)
  
 * Parameters:
 *	dwPID
 *		Process ID of the process to shut down.
 *	dwTimeout
 *		Wait time in milliseconds before shutting down the process.
    
 * Return Value:
 *	TA_FAILED	 - If the shutdown failed.
 *	TA_SUCCESS_CLEAN - If the process was shutdown using WM_CLOSE.
 *	TA_SUCCESS_KILL	 - if the process was shut down with TerminateProcess()
 *	NOTE:  See header for these defines
 * ----------------------------------------------------------------
 */
DWORD WINAPI KillProcess32( DWORD dwPID, DWORD dwTimeout )
{
	HANDLE hProc	= NULL;
	DWORD dwRet	= 0;

	// If we can't open the process with PROCESS_TERMINATE rights,
	// then we give up immediately
	hProc = OpenProcess( SYNCHRONIZE|PROCESS_TERMINATE, FALSE, dwPID );
	if ( NULL == hProc )
	{
		return TA_FAILED;
	}

	// TerminateAppEnum() posts WM_CLOSE to all windows whose PID
	// matches your process's.
	EnumWindows((WNDENUMPROC)TerminateAppEnum, (LPARAM) dwPID);

	// Wait on the handle. If it signals, great. If it times out,
	// then you kill it.
	if ( WaitForSingleObject(hProc, dwTimeout) != WAIT_OBJECT_0 )
		dwRet = (TerminateProcess(hProc,0) ? TA_SUCCESS_KILL : TA_FAILED );
	else
		dwRet = TA_SUCCESS_CLEAN;

	CloseHandle(hProc);

	return dwRet;
}



/*
 * ----------------------------------------------------------------
 * Purpose:
 *	Shut down a Win16 APP.

 * Parameters:
 *	dwPID
 *		Process ID of the NTVDM in which the 16-bit application is running.

 *	dwThread
 *		Thread ID of the thread of execution for the 16-bit application.
 * 
 *	w16Task
 *		16-bit task handle for the application.

 *	dwTimeout
 *		Wait time in milliseconds before shutting down the task.
	  
 *	Return Value:
 *		If successful, returns TA_SUCCESS_16
 *		If unsuccessful, returns TA_FAILED.
 *		NOTE:  These values are defined in the header for this function.
	    
 *		NOTE:
 *			You can get the Win16 task and thread ID through the
 *			VDMEnumTaskWOW() or the VDMEnumTaskWOWEx() functions.
 *
 * ----------------------------------------------------------------
 */ 
DWORD WINAPI KillProcess16( DWORD dwPID, DWORD dwThread, WORD w16Task, DWORD dwTimeout )
{
	HINSTANCE hInstLib	= NULL;
	TERMINFO info;

	// You will be calling the functions through explicit linking
	// so that this code will be binary compatible across
	// Win32 platforms.
	BOOL (WINAPI *lpfVDMTerminateTaskWOW)( DWORD dwProcessId, WORD HTASK );

	hInstLib = LoadLibraryA( "VDMDBG.DLL" ) ;
	if ( NULL == hInstLib )
		return TA_FAILED;

	// Get procedure addresses.
	lpfVDMTerminateTaskWOW = (BOOL (WINAPI *)(DWORD, WORD ))
		GetProcAddress( hInstLib, "VDMTerminateTaskWOW" ) ;
	
	if ( NULL == lpfVDMTerminateTaskWOW )
	{
		if ( hInstLib )
			FreeLibrary( hInstLib ) ;
		return TA_FAILED ;
	}

	// Post a WM_CLOSE to all windows that match the ID and the thread.
	info.dwID	= dwPID;
	info.dwThread	= dwThread ;
	EnumWindows( (WNDENUMPROC)Terminate16AppEnum, (LPARAM)&info );

	// Wait.
	Sleep( dwTimeout ) ;
	
	// Then terminate
	lpfVDMTerminateTaskWOW( dwPID, w16Task );

	if ( hInstLib )
		FreeLibrary( hInstLib );

	return TA_SUCCESS_16;
}


BOOL CALLBACK TerminateAppEnum( HWND hwnd, LPARAM lParam )
{
	DWORD dwID		= 0;

	GetWindowThreadProcessId( hwnd, &dwID );

	if ( dwID == (DWORD)lParam )
	{
		PostMessage( hwnd, WM_CLOSE, 0, 0 );
	}

	return TRUE;
}


BOOL CALLBACK Terminate16AppEnum( HWND hwnd, LPARAM lParam )
{
	DWORD dwID		= 0;
	DWORD dwThread		= 0;
	TERMINFO * termInfo;

	termInfo = (TERMINFO *)lParam;

	dwThread = GetWindowThreadProcessId( hwnd, &dwID );

	if ( dwID == termInfo->dwID && dwThread == termInfo->dwThread )
	{
		PostMessage( hwnd, WM_CLOSE, 0, 0 );
	}

	return TRUE ;
}



