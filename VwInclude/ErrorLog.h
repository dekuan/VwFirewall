//////////////////////////////////////////////////////////////////////////
// ErrorLog.h
// Version 2005-19-27
//
#ifndef __ERRORLOG_HEADER__
#define __ERRORLOG_HEADER__

#include "VwConstAntileechs.h"
#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )



typedef struct tagLogMgr
{
	tagLogMgr()
	{
		memset( this, 0, sizeof(tagLogMgr) );
	}
	virtual BOOL OutputLog( DWORD dwErrorCode, LPCTSTR lpcszContent ) { return TRUE; }

	TCHAR  szLogFileName[ MAX_PATH ];
	TCHAR  szSiteLogDir[ MAX_PATH ];
	TCHAR  szSiteId[ 32 ];
	DWORD  dwTimePeriod;
	DWORD  dwWriteCount;
	FILE * fp;

}STLOGMGR, *PSTLOGMGR;

typedef struct tagLogger
	: public tagLogMgr
{
	/*
		写日志，接口函数，需要继承类来实现
	*/
	virtual BOOL OutputLog( DWORD dwErrorCode, LPCTSTR lpcszContent )
	{
		if ( NULL == lpcszContent )
			return FALSE;

		LPVOID lpMsgBuf			= NULL;
		TCHAR szErrInfo[ MAX_PATH ]	= {0};
		TCHAR szErrorTime[ MAX_PATH ]	= {0};

		if ( ! GetLogErrorTime( szErrorTime, sizeof(szErrorTime) ) )
		{
			return FALSE;
		}
		
		if ( 0 != dwErrorCode )
		{
			FormatMessage
			(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
				NULL,
				dwErrorCode,
				LANG_NEUTRAL,
				(LPTSTR)&lpMsgBuf,
				0,
				NULL
			);
			if ( lpMsgBuf )
			{
				//	获取系统错误信息，并且格式化输出
				_sntprintf( szErrInfo, sizeof(szErrInfo)-sizeof(TCHAR), "%s\t\t%s: %s", szErrorTime, lpcszContent, lpMsgBuf );
				LocalFree( lpMsgBuf );
			}
		}
		else
		{
			_sntprintf( szErrInfo, sizeof(szErrInfo)-sizeof(TCHAR), "%s\t\t%s\r\n", szErrorTime, lpcszContent );
		}

		//	写错误信息入日志文件
		__try
		{
			if ( OpenLogFile() )
			{
				if ( this->fp )
				{
					fprintf( this->fp, "%s", szErrInfo );

					// ..
					if ( this->dwWriteCount >= 10 )
					{
						this->dwWriteCount = 0;
						fflush( this->fp );
					}
					this->dwWriteCount ++;
				}
			}
		}
		__except( EXCEPTION_EXECUTE_HANDLER )
		{
			return FALSE;
		}

		return TRUE;
	}

	BOOL OpenLogFile()
	{
		BOOL bRet = FALSE;
		TCHAR szCurrLogFileName[ MAX_PATH ];

		if ( GetLogFileName( szCurrLogFileName, sizeof(szCurrLogFileName) ) )
		{
			if ( 0 != _tcsicmp( szCurrLogFileName, this->szLogFileName ) )
			{
				if ( this->fp )
				{
					fclose( this->fp );
				}

				// ..
				_sntprintf( this->szLogFileName, sizeof(this->szLogFileName)-sizeof(TCHAR), "%s", szCurrLogFileName );
				this->fp = fopen( this->szLogFileName, "a" );
			}
			
			bRet = this->fp ? TRUE : FALSE;
		}

		return bRet;
	}

	BOOL GetLogErrorTime( LPTSTR lpszErrorTime, DWORD dwSize )
	{
		if ( NULL == lpszErrorTime )
			return FALSE;
		
		SYSTEMTIME sysTime;
		GetLocalTime( &sysTime );
		
		_sntprintf
		(
			lpszErrorTime, dwSize-sizeof(TCHAR), "%d-%02d-%02d %02d:%02d:%02d",
			sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond
		);

		return TRUE;
	}

	BOOL GetLogFileName( LPTSTR lpszLogFileName, DWORD dwSize )
	{
		if ( NULL == lpszLogFileName )
			return FALSE;

		if ( 0 == _tcslen( this->szSiteLogDir ) )
			return FALSE;
		else
		{
			if ( FALSE == PathIsDirectory( this->szSiteLogDir ) )
			{
				CreateDirectory( this->szSiteLogDir, NULL );
			}
		}

		SYSTEMTIME sysTime;
		GetLocalTime( &sysTime );

		if ( ILG_LOGTIMEPERIOD_HOURLY == this->dwTimePeriod )
		{
			_sntprintf( lpszLogFileName, dwSize-sizeof(TCHAR), "%s%d-%d-%d_%d.log", this->szSiteLogDir, sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour );
		}
		else if ( ILG_LOGTIMEPERIOD_DAILY == this->dwTimePeriod )
		{
			_sntprintf( lpszLogFileName, dwSize-sizeof(TCHAR), "%s%d-%d-%d.log", this->szSiteLogDir, sysTime.wYear, sysTime.wMonth, sysTime.wDay );
		}
		else if ( ILG_LOGTIMEPERIOD_MONTHLY == this->dwTimePeriod )
		{
			_sntprintf( lpszLogFileName, dwSize-sizeof(TCHAR), "%s%d-%d.log", this->szSiteLogDir, sysTime.wYear, sysTime.wMonth );
		}

		return TRUE;
	}

}STLOGGER, *PSTLOGGER;



class CErrorLog
{
public:
	CErrorLog();
	~CErrorLog();

	VOID Init( HINSTANCE hInstance );
	STLOGMGR * GetLogObject( LPCTSTR lpcszSiteId, DWORD dwTimePeriod );

private:
	STLOGGER m_Logger[ MAX_SITECOUNT ];
	TCHAR    m_szLogDir[ MAX_PATH ];
};



#endif	// __ERRORLOG_HEADER__