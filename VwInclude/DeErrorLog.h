//////////////////////////////////////////////////////////////////////////
// ErrorLog.h
// Version 2005-19-27
//
#ifndef __DEERRORLOG_HEADER__
#define __DEERRORLOG_HEADER__


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>



//
//	定义日志类型
//
#define CDEERRORLOG_LOGTYPE_TEXT	1
#define CDEERRORLOG_LOGTYPE_HTML	2


/**
 *	日志记录类
 */
class CDeErrorLog
{
public:
	CDeErrorLog();
	~CDeErrorLog();

public:
	VOID Init( HINSTANCE hInstance, LPCTSTR lpcszModuleName, BOOL bShareWrite = TRUE );
	VOID SetLogType( DWORD dwLogType );
	BOOL WriteErrorLog( DWORD dwErrorCode, LPCTSTR lpszErrorDesc );
	BOOL WriteErrorLogEx( DWORD dwErrorCode, LPCTSTR lpszFmt, ... );

private:
	BOOL GetLogFileName( LPTSTR lpszLogFileName, DWORD dwSize );
	BOOL GetLogErrorTime( LPTSTR lpszErrorTime, DWORD dwSize );

	BOOL OpenLogFile();
	BOOL CloseLogFile();

public:
	DWORD m_dwErrorCode;
	TCHAR m_szErrorDesc[ MAX_PATH ];

private:
	FILE * m_logfp;
	BOOL  m_bShareWrite;
	DWORD m_dwLogType;
	TCHAR m_szModDir[ MAX_PATH ];
	TCHAR m_szLogDir[ MAX_PATH ];
	TCHAR m_szLogFileName[ MAX_PATH ];
	DWORD m_dwWriteCount;
	TCHAR m_szModuleName[ 32 ];
};



#endif	// __DEERRORLOG_HEADER__