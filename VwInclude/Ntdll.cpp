#include "stdafx.h"
#include "ntdll.h"

__int64 FileTimeToQuadWord(PFILETIME pft)
{
	return (Int64ShllMod32(pft->dwHighDateTime, 32) | pft->dwLowDateTime );
}

__int64 CalcCpuUsingPercent( HANDLE hSpecProcess )
{
	FILETIME ftKernelTimeStart, ftKernelTimeEnd;
	FILETIME ftUserTimeStart, ftUserTimeEnd;
	FILETIME ftDummy;
	__int64  qwKernelTimeElapsed, qwUserTimeElapsed, qwTotalTimeElapsed;

	if ( NULL == hSpecProcess )
	{
		return 0;
	}

	//
	//	第一次 获取进程的内核时间和用户时间
	//
	if ( GetProcessTimes( hSpecProcess, &ftDummy, &ftDummy, &ftKernelTimeStart, &ftUserTimeStart ) )
	{
		Sleep( 1000 );	//	100ms

		//
		//	第二次 获取进程的内核时间和用户时间
		//
		GetProcessTimes( hSpecProcess, &ftDummy, &ftDummy, &ftKernelTimeEnd, &ftUserTimeEnd );
		
		qwKernelTimeElapsed	= FileTimeToQuadWord( &ftKernelTimeEnd ) - FileTimeToQuadWord( &ftKernelTimeStart );
		qwUserTimeElapsed	= FileTimeToQuadWord( &ftUserTimeEnd ) - FileTimeToQuadWord( &ftUserTimeStart );
		qwTotalTimeElapsed	= qwKernelTimeElapsed + qwUserTimeElapsed;
		
		return qwTotalTimeElapsed;
	}
	else
	{
		return 0;
	}
}


HANDLE GetSpecProcessHandle( LPCTSTR lpctszAppName )
{
	HANDLE hRet		= NULL;
	HANDLE hSnapAll		= NULL;
	PROCESSENTRY32 pe	= {0};

	if ( NULL == lpctszAppName || 0 == _tcslen(lpctszAppName) )
	{
		return NULL;
	}

	//
	//	Take the snapshot of all processes in system
	//
	hSnapAll = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if ( INVALID_HANDLE_VALUE != hSnapAll )
	{
		//
		//	Fill in the size of the structure before using it
		//
		pe.dwSize = sizeof(PROCESSENTRY32);

		//
		//	Work the snap of the processes, and for each process get
		//	the module file name
		//
		if ( Process32First( hSnapAll, &pe ) )
		{
			do
			{
				//	比较进程的名字		
				if ( 0 == _tcsicmp( pe.szExeFile, lpctszAppName ) )
				{
					hRet = OpenProcess( PROCESS_QUERY_INFORMATION, false, pe.th32ProcessID );
					break;
				}
			}
			while( Process32Next( hSnapAll, &pe ) );
		}

		//
		//	Clean up the snapshot
		//
		CloseHandle( hSnapAll );
	}

	return hRet;
}

