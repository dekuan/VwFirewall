// http.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "DeHttp.h"
#include "BindStatusCallback.h"
#include <stdio.h>
#include <stdlib.h>


#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )

#include "Urlmon.h"
#pragma comment( lib, "Urlmon.lib" )

//#include "DeLib.h"
//#pragma comment( lib, "DeLib.lib" )




//////////////////////////////////////////////////////////////////////////
//
// 判断是否存在Internet连接
BOOL CDeHttp::HaveInternetConnection()
{
	BOOL  bRet		= FALSE;
	DWORD dwFlag		= 0;

	__try
	{
		//	如果函数返回FALSE，则肯定没有连接
		bRet = InternetGetConnectedState( &dwFlag, 0 );

		//	如果OFFLINE被置位，则可能是没有连接，也可能是实际上有连接存在
		//	为了尽可能不干扰用户的正常使用，也认为是没有连接
		//if ( dwFlag & INTERNET_CONNECTION_OFFLINE )
		//	return FALSE;

		//	必须有下列任意一种连接，才认为是有连接
		//	因为INTERNET_CONNECTION_CONFIGURED被置位也可能是没有连接
		//if ( ( dwFlag & INTERNET_CONNECTION_LAN ) ||
		//	( dwFlag & INTERNET_CONNECTION_MODEM ) ||
		//	( dwFlag & INTERNET_CONNECTION_PROXY ) )
		//{
		//	return TRUE;
		//}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		bRet = FALSE;
	}

	return bRet;
}


//////////////////////////////////////////////////////////////////////////
// 获取 URL 以及对方服务器信息
BOOL CDeHttp::GetHttpInfo( LPCTSTR lpcszUrl, STHTTPINFO & stHttp, LPTSTR lpszError, DWORD dwTimeout )
{
	if ( FALSE == HaveInternetConnection() )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("当前没有网络连接") );
		return FALSE;
	}

	STHTTPTHREAD stHttpThread;

	stHttpThread.pvThisCls		= this;
	stHttpThread.bRet		= FALSE;
	stHttpThread.pszUrl		= lpcszUrl;
	stHttpThread.stHttpInfo		= &stHttp;
	stHttpThread.pszError		= lpszError;

	__try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				(PBEGINTHREADEX_THREADFUNC)GetHttpInfoProc,
				&stHttpThread,
				0,
				(PBEGINTHREADEX_THREADID)&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
		{
			return FALSE;
		}

		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			TerminateThread( stHttpThread.hThread, 0 );
			while ( WAIT_TIMEOUT == WaitForSingleObject( stHttpThread.hThread, 1000 ) )
			{
				TerminateThread( stHttpThread.hThread, 0 );
			}

			if ( lpszError )
				_tcscpy( lpszError, _T("网络连接超时") );
		}
		if ( stHttpThread.hThread )
		{
			CloseHandle( stHttpThread.hThread );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( lpszError )
		{
			_tcscpy( lpszError, _T("线程异常错误退出") );
		}
	}

	return stHttpThread.bRet;
}

DWORD WINAPI CDeHttp::GetHttpInfoProc( LPVOID p )
{
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	pstThread->bRet = FALSE;

	if ( NULL == pstThread->pszUrl )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("输入参数地址有误") );
		return HTTP_THREAD_FAIL;
	}
	if ( 0 == _tcslen(pstThread->pszUrl) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("输入参数为空") );
		return HTTP_THREAD_FAIL;
	}

	
	HINTERNET hSession	= NULL;
	HINTERNET hHttpFile	= NULL;
	BOOL  bQuerySucc	= FALSE;
	TCHAR szQueryBuf[ 128 ]	= {0};
	DWORD dwQueryBufLen	= sizeof(szQueryBuf);

	__try
	{
		hSession = ::InternetOpen( DEFAULT_AGENT_NAME, PRE_CONFIG_INTERNET_ACCESS, _T(""), INTERNET_INVALID_PORT_NUMBER, 0 );
		if ( hSession )
		{
			hHttpFile = ::InternetOpenUrl( hSession, pstThread->pszUrl, NULL, 0, INTERNET_FLAG_EXISTING_CONNECT, 0 );
			if ( hHttpFile )
			{
				memset( szQueryBuf, 0, sizeof(szQueryBuf) );
				dwQueryBufLen = sizeof(szQueryBuf);
				bQuerySucc = ::HttpQueryInfo ( hHttpFile, HTTP_QUERY_CONTENT_LENGTH, szQueryBuf, &dwQueryBufLen, NULL );
				if ( bQuerySucc && _tcslen(szQueryBuf) > 0 )
				{
					pstThread->stHttpInfo->dwHttpFileLen = (DWORD)_ttol(szQueryBuf);
				}

				memset( szQueryBuf, 0, sizeof(szQueryBuf) );
				dwQueryBufLen = sizeof(szQueryBuf);
				bQuerySucc = ::HttpQueryInfo ( hHttpFile, HTTP_QUERY_STATUS_CODE, szQueryBuf, &dwQueryBufLen, NULL );
				if ( bQuerySucc && _tcslen(szQueryBuf) > 0 )
				{
					pstThread->stHttpInfo->dwHttpStatusCode = (DWORD)_ttol(szQueryBuf);
				}
				
				memset( szQueryBuf, 0, sizeof(szQueryBuf) );
				dwQueryBufLen = sizeof(szQueryBuf);
				bQuerySucc = ::HttpQueryInfo ( hHttpFile, HTTP_QUERY_LAST_MODIFIED, szQueryBuf, &dwQueryBufLen, NULL );
				if ( bQuerySucc && _tcslen(szQueryBuf) > 0 )
				{
					_sntprintf
					( 
						pstThread->stHttpInfo->szHttpLastModified, 
						sizeof(pstThread->stHttpInfo->szHttpLastModified)/sizeof(TCHAR)-1, 
						_T("%s"), szQueryBuf
					);
				}

				memset( szQueryBuf, 0, sizeof(szQueryBuf) );
				dwQueryBufLen = sizeof(szQueryBuf);
				bQuerySucc = ::HttpQueryInfo ( hHttpFile, HTTP_QUERY_SERVER, szQueryBuf, &dwQueryBufLen, NULL );
				if ( bQuerySucc && _tcslen(szQueryBuf) > 0 )
				{
					_sntprintf
					( 
						pstThread->stHttpInfo->szHttpServerInfo,
						sizeof(pstThread->stHttpInfo->szHttpServerInfo)/sizeof(TCHAR)-1,
						_T("%s"), szQueryBuf
					);
				}

				pstThread->bRet = TRUE;

				::InternetCloseHandle( hHttpFile );
			}

			::InternetCloseHandle( hSession );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("线程异常错误退出") );
		if ( hHttpFile )
			::InternetCloseHandle( hHttpFile );
		if ( hSession )
			::InternetCloseHandle( hSession );
	}

	return ( TRUE == pstThread->bRet ) ? HTTP_THREAD_SUCC : HTTP_THREAD_FAIL;
}



//////////////////////////////////////////////////////////////////////////
// 判断某个 URL 是否连接正常
BOOL CDeHttp::IsUrlConnectOk( LPCTSTR lpcszUrl, LPTSTR lpszError, DWORD dwTimeout )
{
	if ( NULL == lpcszUrl )
	{
		return FALSE;
	}
	else if ( 0 == _tcslen(lpcszUrl) )
	{
		return FALSE;
	}

	if ( FALSE == HaveInternetConnection() )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("当前没有网络连接") );
		return FALSE;
	}

	// 获取状态码
	DWORD dwStatusCode = GetStatusCode( lpcszUrl, lpszError, dwTimeout );
	if ( HTTP_STATUS_OK == dwStatusCode )
	{
		return TRUE;
	}
	else
	{
		if ( lpszError )
			_sntprintf( lpszError, MAX_PATH-1, _T("%d"), dwStatusCode );
		return FALSE;
	}

}


//////////////////////////////////////////////////////////////////////////
// 获取某个 URL 的连接状态代码
DWORD CDeHttp::GetStatusCode( LPCTSTR lpcszUrl, LPTSTR lpszError, DWORD dwTimeout )
{
	if ( FALSE == HaveInternetConnection() )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("当前没有网络连接") );
		return FALSE;
	}

	STHTTPTHREAD stHttpThread;

	stHttpThread.pvThisCls	= this;
	stHttpThread.dwRet	= GETSTATUSCODE_ERROR;
	stHttpThread.pszUrl	= lpcszUrl;
	stHttpThread.pszError	= lpszError;

	__try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				(PBEGINTHREADEX_THREADFUNC)GetStatusCodeProc,
				&stHttpThread,
				0,
				(PBEGINTHREADEX_THREADID)&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
			return FALSE;
		
		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			TerminateThread( stHttpThread.hThread, 0 );
			while ( WAIT_TIMEOUT == WaitForSingleObject( stHttpThread.hThread, 1000 ) )
			{
				TerminateThread( stHttpThread.hThread, 0 );
			}

			stHttpThread.dwRet = GETSTATUSCODE_ERROR;
			if ( lpszError )
				_tcscpy( lpszError, _T("网络连接超时") );
		}
		if ( stHttpThread.hThread )
			CloseHandle( stHttpThread.hThread );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( lpszError )
		{
			_tcscpy( lpszError, _T("线程异常错误退出") );
		}
	}

	return stHttpThread.dwRet;
}
DWORD WINAPI CDeHttp::GetStatusCodeProc( LPVOID p )
{
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;

	if ( NULL == pstThread->pszUrl )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("输入参数地址有误") );
		return GETSTATUSCODE_ERROR;
	}
	if ( 0 == _tcslen(pstThread->pszUrl) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("输入参数为空") );
		return GETSTATUSCODE_ERROR;
	}


	HINTERNET hSession	= NULL;
	HINTERNET hHttpFile	= NULL;
	TCHAR szQueryBuf[ 32 ]	= {0};
	DWORD dwQueryBufLen	= 0;
	BOOL  bQuerySucc	= FALSE;

	pstThread->dwRet = GETSTATUSCODE_ERROR;
	__try
	{
		hSession = ::InternetOpen( DEFAULT_AGENT_NAME, PRE_CONFIG_INTERNET_ACCESS, _T(""), INTERNET_INVALID_PORT_NUMBER, 0 );
		if ( hSession )
		{
			hHttpFile = ::InternetOpenUrl( hSession, pstThread->pszUrl, NULL, 0, INTERNET_FLAG_EXISTING_CONNECT, 0 );
			if ( hHttpFile )
			{
				//	得到状态码
				dwQueryBufLen	= sizeof(szQueryBuf);
				bQuerySucc	= ::HttpQueryInfo
					(
						hHttpFile,
						HTTP_QUERY_STATUS_CODE,
						szQueryBuf,
						&dwQueryBufLen,
						NULL
					);
				if ( bQuerySucc && _tcslen(szQueryBuf) > 0 )
				{
					pstThread->dwRet = (DWORD)_ttol(szQueryBuf);
				}

				::InternetCloseHandle( hHttpFile );
			}

			::InternetCloseHandle( hSession );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( pstThread->pszError )
		{
			_tcscpy( pstThread->pszError, _T("线程异常错误退出") );
		}
		if ( hHttpFile )
		{
			::InternetCloseHandle( hHttpFile );
		}
		if ( hSession )
		{
			::InternetCloseHandle( hSession );
		}
	}

	return pstThread->dwRet;
}

/**
 *	获取某个 URL 的连接状态代码
 */
DWORD CDeHttp::GetStatusCodeEx( LPCTSTR lpcszUrl, LPCTSTR lpszAgent, vector<STHTTPCOOKIE> * pvcCookie, LPTSTR lpszError, DWORD dwTimeout )
{
	//
	//	pszUrl		- [in]  URL
	//	pvcCookie	- [in]  Cookie
	//	pszError	- [out] 错误信息
	//	dwTimeout	- [in]  超时，单位：秒
	//	
	//	RETURN		- HTTP STATUS CODE
	//

	if ( FALSE == HaveInternetConnection() )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("当前没有网络连接") );
		return FALSE;
	}
	
	STHTTPTHREAD stHttpThread;
	
	stHttpThread.pvThisCls	= this;
	stHttpThread.dwRet	= GETSTATUSCODE_ERROR;
	stHttpThread.pszUrl	= lpcszUrl;
	stHttpThread.pszAgent	= lpszAgent;
	stHttpThread.pvcCookie	= pvcCookie;
	stHttpThread.pszError	= lpszError;
	stHttpThread.dwTimeout	= dwTimeout;

	__try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				(PBEGINTHREADEX_THREADFUNC)GetStatusCodeExProc,
				&stHttpThread,
				0,
				(PBEGINTHREADEX_THREADID)&stHttpThread.uThreadID
			);
		if ( stHttpThread.hThread )
		{
			if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
			{
				TerminateThread( stHttpThread.hThread, 0 );
				while ( WAIT_TIMEOUT == WaitForSingleObject( stHttpThread.hThread, 1000 ) )
				{
					TerminateThread( stHttpThread.hThread, 0 );
				}

				stHttpThread.dwRet = GETSTATUSCODE_ERROR;
				if ( lpszError )
				{
					_tcscpy( lpszError, _T("网络连接超时") );
				}
			}

			if ( stHttpThread.hThread )
			{
				CloseHandle( stHttpThread.hThread );
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( lpszError )
		{
			_tcscpy( lpszError, _T("线程异常错误退出") );
		}
	}
	
	return stHttpThread.dwRet;
}
DWORD WINAPI CDeHttp::GetStatusCodeExProc( LPVOID p )
{
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;

	if ( NULL == pstThread->pszUrl )
	{
		if ( pstThread->pszError )
		{
			_tcscpy( pstThread->pszError, _T("输入参数地址有误") );
		}
		return GETSTATUSCODE_ERROR;
	}
	if ( 0 == _tcslen(pstThread->pszUrl) )
	{
		if ( pstThread->pszError )
		{
			_tcscpy( pstThread->pszError, _T("输入参数为空") );
		}
		return GETSTATUSCODE_ERROR;
	}


	HINTERNET hSession		= NULL;
	HINTERNET hHttpFile		= NULL;
	TCHAR szQueryBuf[ 4096 ]	= {0};
	DWORD dwQueryBufLen		= 0;
	BOOL  bQuerySucc		= FALSE;
	BOOL  bFindDateTag		= FALSE;
	vector<STHTTPCOOKIE>::iterator	it;
	//STHTTPCOOKIE * pstHttpCookie;
	TCHAR szAgent[ MAX_PATH ]	= {0};
	TCHAR szTemp[ MAX_PATH ]	= {0};


	pstThread->dwRet = GETSTATUSCODE_ERROR;

	try
	{
		//
		//	设置 Cookie
		//
		if ( pstThread->pvcCookie )
		{
			for ( it = pstThread->pvcCookie->begin(); it != pstThread->pvcCookie->end(); it ++ )
			{
				//pstHttpCookie = it;
				InternetSetCookie( pstThread->pszUrl, (*it).szName, (*it).szData );
			}
		}

		if ( pstThread->pszAgent && _tcslen(pstThread->pszAgent) > 0 )
		{
			_sntprintf( szAgent, sizeof(szAgent)/sizeof(TCHAR)-1, _T("%s"), pstThread->pszAgent );
		}
		else
		{
			_sntprintf( szAgent, sizeof(szAgent)/sizeof(TCHAR)-1, _T("%s"), DEFAULT_AGENT_NAME );
		}

		hSession = ::InternetOpen( szAgent, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL );
		if ( hSession )
		{
			hHttpFile = ::InternetOpenUrl( hSession, pstThread->pszUrl, NULL, 0, INTERNET_FLAG_EXISTING_CONNECT, 0 );
			if ( hHttpFile )
			{
				//	确保 HTTP 头中有 Date: 信息
				memset( szQueryBuf, 0, sizeof(szQueryBuf) );
				dwQueryBufLen	= sizeof(szQueryBuf);
				bQuerySucc	= ::HttpQueryInfo
					(
						hHttpFile,
						HTTP_QUERY_RAW_HEADERS_CRLF,
						szQueryBuf,
						&dwQueryBufLen,
						NULL
					);

				DWORD dwLastError = GetLastError();
	
				if ( bQuerySucc && _tcslen(szQueryBuf) > 0 )
				{
					//
					//	检查是否有 Date: 标记
					//	主要是为了检测 HTTP 头是否完整
					//
					if ( _tcsstr( szQueryBuf, _T("Date:") ) )
					{
						bFindDateTag	= TRUE;
					}

					//	得到状态码
					memset( szQueryBuf, 0, sizeof(szQueryBuf) );
					dwQueryBufLen	= sizeof(szQueryBuf);
					bQuerySucc	= ::HttpQueryInfo
						(
							hHttpFile,
							HTTP_QUERY_STATUS_CODE,		//HTTP_QUERY_RAW_HEADERS_CRLF
							szQueryBuf,
							&dwQueryBufLen,
							NULL
						);
					if ( bQuerySucc && _tcslen(szQueryBuf) > 0 )
					{
						pstThread->dwRet = (DWORD)_ttol(szQueryBuf);
					}
				}

				if ( ! bFindDateTag && 200 == pstThread->dwRet )
				{
					//
					//	HTTP 头不完整，标记为错误
					//
					pstThread->dwRet = GETSTATUSCODE_ERROR;
				}

				::InternetCloseHandle( hHttpFile );
			}

			::InternetCloseHandle( hSession );
		}
	}
	catch( ... )
	{
		if ( pstThread->pszError )
		{
			_tcscpy( pstThread->pszError, _T("线程异常错误退出") );
		}
		if ( hHttpFile )
		{
			::InternetCloseHandle( hHttpFile );
		}
		if ( hSession )
		{
			::InternetCloseHandle( hSession );
		}
	}

	return pstThread->dwRet;
}






/**
 *	获取整个 Header
 */
BOOL CDeHttp::GetHttpHeader( LPCTSTR lpcszUrl, LPTSTR lpszHeader, DWORD dwSize, LPTSTR lpszError, DWORD dwTimeout )
{
	/*
		lpcszUrl	- [in]  待访问的 URL
		lpszHeader	- [out] 返回的整个 HTTP 头
		dwSize		- [in]  返回缓冲区大小
		lpszError	- [out] 返回文字的错误信息
		dwTimeout	- [in]  超时时间
		RETURN		- TRUE / FALSE
	*/

	if ( FALSE == HaveInternetConnection() )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("当前没有网络连接") );
		return FALSE;
	}

	STHTTPTHREAD stHttpThread;

	stHttpThread.pvThisCls		= this;
	stHttpThread.bRet		= FALSE;
	stHttpThread.pszUrl		= lpcszUrl;
	stHttpThread.pszHeader		= lpszHeader;
	stHttpThread.dwSize		= dwSize;
	stHttpThread.pszError		= lpszError;
	
	__try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				(PBEGINTHREADEX_THREADFUNC)GetHttpHeaderProc,
				&stHttpThread,
				0,
				(PBEGINTHREADEX_THREADID)&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
		{
			return FALSE;
		}

		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			TerminateThread( stHttpThread.hThread, 0 );
			while ( WAIT_TIMEOUT == WaitForSingleObject( stHttpThread.hThread, 1000 ) )
			{
				TerminateThread( stHttpThread.hThread, 0 );
			}

			if ( lpszError )
			{
				_tcscpy( lpszError, _T("网络连接超时") );
			}
		}
		if ( stHttpThread.hThread )
		{
			CloseHandle( stHttpThread.hThread );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( lpszError )
		{
			_tcscpy( lpszError, _T("线程异常错误退出") );
		}
	}

	return stHttpThread.bRet;
}
DWORD WINAPI CDeHttp::GetHttpHeaderProc( LPVOID p )
{
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	
	if ( NULL == pstThread->pszUrl )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("输入参数地址有误") );
		return HTTP_THREAD_FAIL;
	}
	if ( 0 == _tcslen(pstThread->pszUrl) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("输入参数为空") );
		return HTTP_THREAD_FAIL;
	}
	if ( NULL == pstThread->pszHeader || 0 == pstThread->dwSize )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("Header 接受参数不正确") );
		return HTTP_THREAD_FAIL;
	}

	HINTERNET hSession	= NULL;
	HINTERNET hHttpFile	= NULL;
	BOOL  bQuerySucc	= FALSE;

	pstThread->bRet = FALSE;
	__try
	{
		hSession = ::InternetOpen( DEFAULT_AGENT_NAME, PRE_CONFIG_INTERNET_ACCESS, _T(""), INTERNET_INVALID_PORT_NUMBER, 0 );
		if ( hSession )
		{
			hHttpFile = ::InternetOpenUrl( hSession, pstThread->pszUrl, NULL, 0, INTERNET_FLAG_EXISTING_CONNECT, 0 );
			if ( hHttpFile )
			{
				// 获取以 CRLF 为分割的 Header
				bQuerySucc = ::HttpQueryInfo
					(
						hHttpFile,
						HTTP_QUERY_RAW_HEADERS_CRLF,
						pstThread->pszHeader,
						&pstThread->dwSize,
						NULL
					);
				if ( bQuerySucc && _tcslen(pstThread->pszHeader) > 0 )
				{
					pstThread->bRet = TRUE;
				}
				
				::InternetCloseHandle( hHttpFile );
			}
			
			::InternetCloseHandle( hSession );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("线程异常错误退出") );
		if ( hHttpFile )
			::InternetCloseHandle( hHttpFile );
		if ( hSession )
			::InternetCloseHandle( hSession );
	}
	
	return ( TRUE == pstThread->bRet ) ? HTTP_THREAD_SUCC : HTTP_THREAD_FAIL;
}

/**
 *	获取整个 Header
 */
BOOL CDeHttp::GetHttpHeaderEx(
	LPCTSTR lpcszUrl, vector<STHTTPHEADERVAR> & vcHttpHeaderVar,
	LPTSTR lpszHeader, DWORD dwSize, LPTSTR lpszError, DWORD dwTimeout )
{
	/*
		lpcszUrl	- [in]  待访问的 URL
		vcHttpHeaderVar	- [in]  HTTP 头附加值
		lpszHeader	- [out] 返回的整个 HTTP 头
		dwSize		- [in]  返回缓冲区大小
		lpszError	- [out] 返回文字的错误信息
		dwTimeout	- [in]  超时时间
		RETURN		- TRUE / FALSE
	*/

	if ( FALSE == HaveInternetConnection() )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("当前没有网络连接") );
		return FALSE;
	}

	STHTTPTHREAD stHttpThread;

	stHttpThread.pvThisCls		= this;
	stHttpThread.bRet		= FALSE;
	stHttpThread.pszUrl		= lpcszUrl;
	stHttpThread.pvcHttpHeaderVar	= & vcHttpHeaderVar;
	stHttpThread.pszHeader		= lpszHeader;
	stHttpThread.dwSize		= dwSize;
	stHttpThread.pszError		= lpszError;

	__try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				(PBEGINTHREADEX_THREADFUNC)GetHttpHeaderExProc,
				&stHttpThread,
				0,
				(PBEGINTHREADEX_THREADID)&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
		{
			return FALSE;
		}

		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			TerminateThread( stHttpThread.hThread, 0 );
			while ( WAIT_TIMEOUT == WaitForSingleObject( stHttpThread.hThread, 1000 ) )
			{
				TerminateThread( stHttpThread.hThread, 0 );
			}

			if ( lpszError )
			{
				_tcscpy( lpszError, _T("网络连接超时") );
			}
		}
		if ( stHttpThread.hThread )
		{
			CloseHandle( stHttpThread.hThread );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( lpszError )
		{
			_tcscpy( lpszError, _T("线程异常错误退出") );
		}
	}

	return stHttpThread.bRet;
}
DWORD WINAPI CDeHttp::GetHttpHeaderExProc( LPVOID p )
{
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	CDeHttp * pCHttp = (CDeHttp*)pstThread->pvThisCls;

	if ( NULL == pCHttp )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("内部错误：无法找到类地址") );
		return HTTP_THREAD_FAIL;
	}
	if ( NULL == pstThread->pszUrl )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("输入参数地址有误") );
		return HTTP_THREAD_FAIL;
	}
	if ( 0 == _tcslen(pstThread->pszUrl) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("输入参数为空") );
		return HTTP_THREAD_FAIL;
	}
	if ( NULL == pstThread->pszHeader || 0 == pstThread->dwSize )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("Header 接受参数不正确") );
		return HTTP_THREAD_FAIL;
	}

	const TCHAR * szAcceptType = HTTP_ACCEPT_TYPE;
	HINTERNET hSession	= NULL;
	//HINTERNET hHttpFile	= NULL;
	HINTERNET hConnect	= NULL;
	HINTERNET hRequest	= NULL;
	DWORD dwFlags		= 0;
	BOOL bAddHeaders	= FALSE;
	BOOL bSendRequest	= FALSE;
	DWORD dwBytesRead	= 0;
	BOOL  bQuerySucc	= FALSE;
	vector<STHTTPHEADERVAR> vcHttpHeaderVar;
	vector<STHTTPHEADERVAR>::iterator it;
	TCHAR szServer[MAX_PATH]	= {0};
	TCHAR szPath[ 1024 ]		= {0};
	TCHAR szHeadLine[MAX_PATH]	= {0};
#ifdef UNICODE
	STDEHTTPDEPARSEURLW stParseUrl;
#else
	STDEHTTPDEPARSEURL stParseUrl;
#endif

	if ( pCHttp->dehttp_delib_parse_url( pstThread->pszUrl, (DWORD)_tcslen(pstThread->pszUrl), & stParseUrl ) )
	{
		_sntprintf( szServer, sizeof(szServer)/sizeof(TCHAR)-1, _T("%s"), stParseUrl.szHost );
		_sntprintf( szPath, sizeof(szPath)/sizeof(TCHAR)-1, _T("%s?%s"), stParseUrl.szPath, stParseUrl.szQuery );
	}
	else
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("无法解析 URL 地址") );
		return HTTP_THREAD_FAIL;
	}

	pstThread->bRet = FALSE;
	try
	{
		//	第一步：初始化 internet DLL 会话句柄，这是第一个被调用的函数
		hSession = ::InternetOpen( DEFAULT_AGENT_NAME, PRE_CONFIG_INTERNET_ACCESS, _T(""), INTERNET_INVALID_PORT_NUMBER, 0 );
		if ( hSession )
		{
			//	第二步：初始化 HTTP session 连接句柄
			hConnect = ::InternetConnect( hSession, szServer, DEFAULT_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0 );
			if ( hConnect )
			{
				//	第三步：打开一个HTTP请求句柄
				hRequest = ::HttpOpenRequest
					(
						hConnect, HTTP_VERB_GET, szPath,
						HTTP_VERSION, NULL, 0,
						INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_NO_AUTO_REDIRECT, 0
					);
				if ( hRequest ) 
				{
					//	添加可能需要的 HTTP 头
					bAddHeaders	= TRUE;
					if ( pstThread->pvcHttpHeaderVar )
					{
						vcHttpHeaderVar = *pstThread->pvcHttpHeaderVar;
						for ( it = vcHttpHeaderVar.begin(); it != vcHttpHeaderVar.end(); it ++ )
						{
							_sntprintf( szHeadLine, sizeof(szHeadLine)/sizeof(TCHAR)-1, _T("%s"), (*it).szHeadLine );
							bAddHeaders &= ::HttpAddRequestHeaders
							(
								hRequest,
								szHeadLine,
								(DWORD)_tcslen( szHeadLine ),
								HTTP_ADDREQ_FLAG_REPLACE|HTTP_ADDREQ_FLAG_ADD
							);
						}
					}

					if ( bAddHeaders )
					{
						//	第四步：发出请求
						bSendRequest = ::HttpSendRequest( hRequest, NULL, 0, 0, 0 );
						if ( bSendRequest )
						{
							//	获取以 CRLF 为分割的 Header
							bQuerySucc = ::HttpQueryInfo
								(
									hRequest,
									HTTP_QUERY_RAW_HEADERS_CRLF,
									pstThread->pszHeader,
									&pstThread->dwSize,
									NULL
								);
							if ( bQuerySucc && _tcslen(pstThread->pszHeader) > 0 )
							{
								pstThread->bRet = TRUE;
							}
						}
					}
					else
					{
						if ( pstThread->pszError )
							_tcscpy( pstThread->pszError, _T("添加 HTTP 头部信息失败") );
					}

					::InternetCloseHandle( hRequest );
				}
				else
				{
					if ( pstThread->pszError )
						_tcscpy( pstThread->pszError, _T("HttpOpenRequest 失败，打开一个 HTTP 请求句柄失败") );
				}

				::InternetCloseHandle( hConnect );
			}
			else
			{
				if ( pstThread->pszError )
					_tcscpy( pstThread->pszError, _T("InternetConnect 失败") );
			}

			::InternetCloseHandle(hSession);
		}
		else
		{
			if ( pstThread->pszError )
				_tcscpy( pstThread->pszError, _T("初始化 Internet DLL 失败") );
		}
	}
	catch ( ... )
	//__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("线程异常错误退出") );
		if ( hRequest )
			::InternetCloseHandle( hRequest );
		if ( hConnect )
			::InternetCloseHandle( hConnect );
		if ( hSession )
			::InternetCloseHandle( hSession );
	}
	
	return ( TRUE == pstThread->bRet ) ? HTTP_THREAD_SUCC : HTTP_THREAD_FAIL;
}


/**
 *	Get Response
 */
BOOL CDeHttp::GetResponse( LPCTSTR lpcszUrl, LPTSTR lpszResponse, DWORD dwSize, LPTSTR lpszError, DWORD dwTimeout )
{
	//
	//	lpcszUrl	- [in]
	//	lpszResponse	- [in]
	//	dwSize		- [in]
	//	lpszError	- [in]
	//	dwTimeout	- [in] 单位毫秒
	//	RETURN		- [in]
	//

	if ( FALSE == HaveInternetConnection() )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("当前没有网络连接") );
		return FALSE;
	}

	STHTTPTHREAD stHttpThread;

	stHttpThread.pvThisCls		= this;
	stHttpThread.bRet		= FALSE;
	stHttpThread.pszUrl		= lpcszUrl;
	stHttpThread.pszResponse	= lpszResponse;
	stHttpThread.dwSize		= dwSize;
	stHttpThread.pszError		= lpszError;

	try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				(PBEGINTHREADEX_THREADFUNC)GetResponseProc,
				&stHttpThread,
				0,
				(PBEGINTHREADEX_THREADID)&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
			return FALSE;

		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			TerminateThread( stHttpThread.hThread, 0 );
			while ( WAIT_TIMEOUT == WaitForSingleObject( stHttpThread.hThread, 1000 ) )
			{
				TerminateThread( stHttpThread.hThread, 0 );
			}

			if ( lpszError )
				_tcscpy( lpszError, _T("网络连接超时") );
		}
		if ( stHttpThread.hThread )
			CloseHandle( stHttpThread.hThread );
	}
	catch ( ... )
	//__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("线程异常错误退出") );
	}

	return stHttpThread.bRet;
}
DWORD WINAPI CDeHttp::GetResponseProc( LPVOID p )
{	
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	CDeHttp * pCHttp = (CDeHttp*)pstThread->pvThisCls;
	pstThread->bRet = FALSE;
	
	if ( NULL == pCHttp )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("内部错误：无法找到类地址") );
		return HTTP_THREAD_FAIL;
	}
	if ( NULL == pstThread->pszUrl || NULL == pstThread->pszResponse )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("参数地址无效") );
		return HTTP_THREAD_FAIL;
	}
	if ( 0 == _tcslen(pstThread->pszUrl) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("输入参数不能为空") );
		return HTTP_THREAD_FAIL;
	}

	TCHAR szServer[MAX_PATH]	= {0};
	TCHAR szPath[MAX_PATH]		= {0};
#ifdef UNICODE
	STDEHTTPDEPARSEURLW stParseUrl;
#else
	STDEHTTPDEPARSEURL stParseUrl;
#endif

	if ( pCHttp->dehttp_delib_parse_url( pstThread->pszUrl, (DWORD)_tcslen(pstThread->pszUrl), & stParseUrl ) )
	{
		_sntprintf( szServer, sizeof(szServer)/sizeof(TCHAR)-1, _T("%s"), stParseUrl.szHost );
		_sntprintf( szPath, sizeof(szPath)/sizeof(TCHAR)-1, _T("%s?%s"), stParseUrl.szPath, stParseUrl.szQuery );
	}
	else
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("无法解析 URL 地址") );
		return HTTP_THREAD_FAIL;
	}

	/*
	if ( FALSE == pCHttp->ParseUrl( pstThread->pszUrl, szServer, szPath ) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("无法解析 URL 地址") );
		return HTTP_THREAD_FAIL;
	}
	*/

	HINTERNET hSession	= NULL;
	HINTERNET hConnect	= NULL;
	HINTERNET hRequest	= NULL;
	BOOL bSendRequest	= FALSE;
	DWORD dwBytesRead	= 0;

	try
	{
		// 第一步:初始化 internet DLL 会话句柄
		hSession = ::InternetOpen( DEFAULT_AGENT_NAME, PRE_CONFIG_INTERNET_ACCESS, _T(""), INTERNET_INVALID_PORT_NUMBER, 0 );
		if ( hSession )
		{
			// 第二步：初始化 HTTP session
			hConnect = ::InternetConnect
				(
					hSession,		// 当前internet会话句柄
					szServer,		// server name
					INTERNET_INVALID_PORT_NUMBER,
					NULL,			// "",//user name
					NULL,			// password
					INTERNET_SERVICE_HTTP,	// Type of service to access
					0, 0
				);
			if ( hConnect )
			{
				//	第三步：打开一个HTTP请求句柄
				hRequest = ::HttpOpenRequest( hConnect, HTTP_VERB_GET, szPath, HTTP_VERSION, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0 );
				if ( hRequest )
				{
					// 第四步：发出请求
					bSendRequest = ::HttpSendRequest( hRequest, NULL, 0, 0, 0 );
					if ( bSendRequest )
					{
						/*
						while( dwBufferSize > 0 && 
							InternetReadFile( hRequest, pstThread->pszResponse+dwTotalRead, dwBufferSize, &dwBytesRead ) )
						{
							if ( dwBytesRead == 0 )
							{
								break;
							}
							dwTotalRead	+= dwBytesRead;
							dwBufferSize	-= dwBytesRead;
						}
						*/

						if ( pCHttp->ReadInternetFileBuffer( hRequest, pstThread->pszResponse, pstThread->dwSize ) )
						{
							pstThread->bRet = TRUE;
						}
						else
						{
							if ( pstThread->pszError )
								_tcscpy( pstThread->pszError, _T("获取服务器返回信息为空") );
						}

						/*
						DWORD dwBytesRead;
						BOOL  bReadFile = InternetReadFile
							(
								hRequest, pstThread->pszResponse,
								pstThread->dwSize-sizeof(TCHAR),
								&dwBytesRead
							);
						if ( bReadFile && dwBytesRead > 0 )
						{
							pstThread->bRet = TRUE;
						}
						else
						{
							if ( pstThread->pszError )
								_tcscpy( pstThread->pszError, _T("获取服务器返回信息为空") );
						}
						*/
					}
					else
					{
						if ( pstThread->pszError )
							_stprintf( pstThread->pszError, _T("发出一个 %s 请求失败, ErrCode=%d"), pstThread->pszUrl, GetLastError() );
					}

					::InternetCloseHandle(hRequest);
				}
				else
				{
					if ( pstThread->pszError )
						_tcscpy( pstThread->pszError, _T("HttpOpenRequest 失败，打开一个HTTP请求句柄失败") );
				}

				::InternetCloseHandle(hConnect);
			}
			else
			{
				if ( pstThread->pszError )
					_tcscpy( pstThread->pszError, _T("InternetConnect 失败，初始化HTTP session失败") );
			}

			::InternetCloseHandle(hSession);
		}
		else
		{
			if ( pstThread->pszError )
				_tcscpy( pstThread->pszError, _T("InternetOpen 失败，初始化Internet DLL失败") );
		}
	}
	catch ( ... )
	//__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("线程异常错误退出") );
		if ( hRequest )
			::InternetCloseHandle(hRequest);
		if ( hConnect )
			::InternetCloseHandle(hConnect);
		if ( hSession )
			::InternetCloseHandle(hSession);
	}

	return ( TRUE == pstThread->bRet ) ? HTTP_THREAD_SUCC : HTTP_THREAD_FAIL;

}

/**
 *	从 HINTERNET hRequest 句柄读取完整的服务器回应内容
 */
BOOL CDeHttp::ReadInternetFileBuffer( HINTERNET hRequest, TCHAR * pszBuffer, DWORD dwSize )
{
	TCHAR szTempBuffer[ MAX_PATH ];
	LONGLONG  lnlnBufferSize;
	DWORD dwBytesRead;
	DWORD dwTotalRead;

	if ( NULL == hRequest || NULL == pszBuffer || 0 == dwSize )
	{
		return FALSE;
	}

	__try
	{
		lnlnBufferSize	= dwSize-sizeof(TCHAR);
		dwTotalRead	= 0;
		memset( szTempBuffer, 0, sizeof(szTempBuffer) );

		while ( InternetReadFile( hRequest, szTempBuffer, sizeof(szTempBuffer), &dwBytesRead ) )
		{
			if ( dwBytesRead == 0 )
			{
				break;
			}
			if ( lnlnBufferSize <= 0 )
			{
				break;
			}

			memcpy( pszBuffer+dwTotalRead, szTempBuffer, min( dwBytesRead, lnlnBufferSize ) );

			dwTotalRead	+= dwBytesRead;
			lnlnBufferSize	-= dwBytesRead;

			memset( szTempBuffer, 0, sizeof(szTempBuffer) );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return TRUE;
}

/**
 *	Get Response
 */
BOOL CDeHttp::GetResponseByProxy( LPCTSTR lpcszUrl, LPCTSTR lpcszProxy, LPTSTR lpszResponse, DWORD dwSize, LPTSTR lpszError, DWORD dwTimeout )
{
	//
	//	lpcszUrl	- [in]
	//	lpcszProxy	- [in] 代理服务器
	//	lpszResponse	- [in]
	//	dwSize		- [in]
	//	lpszError	- [in]
	//	dwTimeout	- [in] 单位毫秒
	//	RETURN		- [in]
	//

	if ( FALSE == HaveInternetConnection() )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("当前没有网络连接") );
		return FALSE;
	}

	STHTTPTHREAD stHttpThread;

	stHttpThread.pvThisCls		= this;
	stHttpThread.bRet		= FALSE;
	stHttpThread.pszUrl		= lpcszUrl;
	stHttpThread.lpcszProxy		= lpcszProxy;
	stHttpThread.pszResponse	= lpszResponse;
	stHttpThread.dwSize		= dwSize;
	stHttpThread.pszError		= lpszError;

	try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				(PBEGINTHREADEX_THREADFUNC)GetResponseByProxyProc,
				&stHttpThread,
				0,
				(PBEGINTHREADEX_THREADID)&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
			return FALSE;

		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			TerminateThread( stHttpThread.hThread, 0 );
			while ( WAIT_TIMEOUT == WaitForSingleObject( stHttpThread.hThread, 1000 ) )
			{
				TerminateThread( stHttpThread.hThread, 0 );
			}

			if ( lpszError )
			{
				_tcscpy( lpszError, _T("网络连接超时") );
			}
		}

		if ( stHttpThread.hThread )
		{
			CloseHandle( stHttpThread.hThread );
		}
	}
	catch ( ... )
	//__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("线程异常错误退出") );
	}

	return stHttpThread.bRet;
}
DWORD WINAPI CDeHttp::GetResponseByProxyProc( LPVOID p )
{
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	CDeHttp * pCHttp = (CDeHttp*)pstThread->pvThisCls;
	pstThread->bRet = FALSE;

	if ( NULL == pCHttp )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("内部错误：无法找到类地址") );
		return HTTP_THREAD_FAIL;
	}
	if ( NULL == pstThread->pszUrl || NULL == pstThread->pszResponse )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("参数地址无效") );
		return HTTP_THREAD_FAIL;
	}
	if ( 0 == _tcslen(pstThread->pszUrl) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("输入参数不能为空") );
		return HTTP_THREAD_FAIL;
	}

	TCHAR szServer[MAX_PATH]	= {0};
	TCHAR szPath[MAX_PATH]		= {0};
#ifdef UNICODE
	STDEHTTPDEPARSEURLW stParseUrl;
#else
	STDEHTTPDEPARSEURL stParseUrl;
#endif

	if ( pCHttp->dehttp_delib_parse_url( pstThread->pszUrl, (DWORD)_tcslen(pstThread->pszUrl), & stParseUrl ) )
	{
		_sntprintf( szServer, sizeof(szServer)/sizeof(TCHAR)-1, _T("%s"), stParseUrl.szHost );
		_sntprintf( szPath, sizeof(szPath)/sizeof(TCHAR)-1, _T("%s?%s"), stParseUrl.szPath, stParseUrl.szQuery );
	}
	else
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("无法解析 URL 地址") );
		return HTTP_THREAD_FAIL;
	}

	/*
	if ( FALSE == pCHttp->ParseUrl( pstThread->pszUrl, szServer, szPath ) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("无法解析 URL 地址") );
		return HTTP_THREAD_FAIL;
	}
	*/

	HINTERNET hSession	= NULL;
	HINTERNET hConnect	= NULL;
	HINTERNET hRequest	= NULL;
	BOOL bSendRequest	= FALSE;
	DWORD dwBytesRead	= 0;


	try
	{
		// 第一步:初始化 internet DLL 会话句柄
		hSession = ::InternetOpen( DEFAULT_AGENT_NAME, INTERNET_OPEN_TYPE_PROXY, pstThread->lpcszProxy, pstThread->lpcszProxy, 0 );
		if ( hSession )
		{
			// 第二步：初始化 HTTP session
			hConnect = ::InternetConnect
				(
					hSession,		// 当前internet会话句柄
					szServer,		// server name
					INTERNET_INVALID_PORT_NUMBER,
					NULL,			// "",//user name
					NULL,			// password
					INTERNET_SERVICE_HTTP,	// Type of service to access
					0, 0
				);
			if ( hConnect )
			{
				//	第三步：打开一个HTTP请求句柄
				hRequest = ::HttpOpenRequest( hConnect, HTTP_VERB_GET, szPath, HTTP_VERSION, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0 );
				if ( hRequest )
				{
					// 第四步：发出请求
					bSendRequest = ::HttpSendRequest( hRequest, NULL, 0, 0, 0 );
					if ( bSendRequest )
					{
						DWORD dwBytesRead;
						BOOL  bReadFile = InternetReadFile
							(
								hRequest, pstThread->pszResponse,
								pstThread->dwSize-sizeof(TCHAR),
								&dwBytesRead
							);
						if ( bReadFile && dwBytesRead > 0 )
						{
							pstThread->bRet = TRUE;
						}
						else
						{
							if ( pstThread->pszError )
								_tcscpy( pstThread->pszError, _T("获取服务器返回信息为空") );
						}
					}
					else
					{
						if ( pstThread->pszError )
							_stprintf( pstThread->pszError, _T("发出一个 %s 请求失败, ErrCode=%d"), pstThread->pszUrl, GetLastError() );
					}

					::InternetCloseHandle(hRequest);
				}
				else
				{
					if ( pstThread->pszError )
						_tcscpy( pstThread->pszError, _T("HttpOpenRequest 失败，打开一个HTTP请求句柄失败") );
				}

				::InternetCloseHandle(hConnect);
			}
			else
			{
				if ( pstThread->pszError )
					_tcscpy( pstThread->pszError, _T("InternetConnect 失败，初始化HTTP session失败") );
			}

			::InternetCloseHandle(hSession);
		}
		else
		{
			if ( pstThread->pszError )
				_tcscpy( pstThread->pszError, _T("InternetOpen 失败，初始化Internet DLL失败") );
		}
	}
	catch ( ... )
	//__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("线程异常错误退出") );
		if ( hRequest )
			::InternetCloseHandle(hRequest);
		if ( hConnect )
			::InternetCloseHandle(hConnect);
		if ( hSession )
			::InternetCloseHandle(hSession);
	}

	return ( TRUE == pstThread->bRet ) ? HTTP_THREAD_SUCC : HTTP_THREAD_FAIL;

}




/**
 *	Get Response
 */
BOOL CDeHttp::GetResponseEx( LPCTSTR lpcszUrl, vector<STHTTPHEADERVAR> & vcHttpHeaderVar, LPTSTR lpszResponse, DWORD dwSize, LPTSTR lpszError, DWORD dwTimeout )
{
	if ( FALSE == HaveInternetConnection() )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("当前没有网络连接") );
		return FALSE;
	}

	STHTTPTHREAD stHttpThread;

	stHttpThread.pvThisCls		= this;
	stHttpThread.bRet		= FALSE;
	stHttpThread.pszUrl		= lpcszUrl;
	stHttpThread.pvcHttpHeaderVar	= & vcHttpHeaderVar;
	stHttpThread.pszResponse	= lpszResponse;
	stHttpThread.dwSize		= dwSize;
	stHttpThread.pszError		= lpszError;

	__try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				(PBEGINTHREADEX_THREADFUNC)GetResponseExProc,
				&stHttpThread,
				0,
				(PBEGINTHREADEX_THREADID)&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
			return FALSE;

		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			TerminateThread( stHttpThread.hThread, 0 );
			while ( WAIT_TIMEOUT == WaitForSingleObject( stHttpThread.hThread, 1000 ) )
			{
				TerminateThread( stHttpThread.hThread, 0 );
			}

			if ( lpszError )
				_tcscpy( lpszError, _T("网络连接超时") );
		}
		if ( stHttpThread.hThread )
			CloseHandle( stHttpThread.hThread );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("线程异常错误退出") );
	}

	return stHttpThread.bRet;
}
DWORD WINAPI CDeHttp::GetResponseExProc( LPVOID p )
{
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	CDeHttp * pCHttp = (CDeHttp*)pstThread->pvThisCls;
	pstThread->bRet = FALSE;

	if ( NULL == pCHttp )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("内部错误：无法找到类地址") );
		return HTTP_THREAD_FAIL;
	}
	if ( NULL == pstThread->pszUrl || NULL == pstThread->pszResponse )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("参数地址无效") );
		return HTTP_THREAD_FAIL;
	}
	if ( 0 == _tcslen(pstThread->pszUrl) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("输入参数不能为空") );
		return HTTP_THREAD_FAIL;
	}

	TCHAR szServer[MAX_PATH]	= {0};
	TCHAR szPath[MAX_PATH]		= {0};
#ifdef UNICODE
	STDEHTTPDEPARSEURLW stParseUrl;
#else
	STDEHTTPDEPARSEURL stParseUrl;
#endif

	if ( pCHttp->dehttp_delib_parse_url( pstThread->pszUrl, (DWORD)_tcslen(pstThread->pszUrl), & stParseUrl ) )
	{
		_sntprintf( szServer, sizeof(szServer)/sizeof(TCHAR)-1, _T("%s"), stParseUrl.szHost );
		_sntprintf( szPath, sizeof(szPath)/sizeof(TCHAR)-1, _T("%s?%s"), stParseUrl.szPath, stParseUrl.szQuery );
	}
	else
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("无法解析 URL 地址") );
		return HTTP_THREAD_FAIL;
	}

	HINTERNET hSession	= NULL;
	HINTERNET hConnect	= NULL;
	HINTERNET hRequest	= NULL;
	BOOL bSendRequest	= FALSE;
	DWORD dwBytesRead	= 0;
	BOOL bAddHeaders	= FALSE;
	vector<STHTTPHEADERVAR> vcHttpHeaderVar;
	vector<STHTTPHEADERVAR>::iterator it;
	TCHAR szHeadLine[MAX_PATH]	= {0};


	try
	{
		// 第一步:初始化 internet DLL 会话句柄
		hSession = ::InternetOpen( DEFAULT_AGENT_NAME, PRE_CONFIG_INTERNET_ACCESS, _T(""), INTERNET_INVALID_PORT_NUMBER, 0 );
		if ( hSession )
		{
			// 第二步：初始化 HTTP session
			hConnect = ::InternetConnect
				(
					hSession,		// 当前internet会话句柄
					szServer,		// server name
					INTERNET_INVALID_PORT_NUMBER,
					NULL,			// "",//user name
					NULL,			// password
					INTERNET_SERVICE_HTTP,	// Type of service to access
					0, 0
				);
			if ( hConnect )
			{
				//	第三步：打开一个HTTP请求句柄
				hRequest = ::HttpOpenRequest( hConnect, HTTP_VERB_GET, szPath, HTTP_VERSION, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0 );
				if ( hRequest )
				{
					//	添加可能需要的 HTTP 头
					bAddHeaders	= TRUE;
					if ( pstThread->pvcHttpHeaderVar )
					{
						vcHttpHeaderVar = *pstThread->pvcHttpHeaderVar;
						for ( it = vcHttpHeaderVar.begin(); it != vcHttpHeaderVar.end(); it ++ )
						{
							_sntprintf( szHeadLine, sizeof(szHeadLine)/sizeof(TCHAR)-1, _T("%s"), (*it).szHeadLine );
							bAddHeaders &= ::HttpAddRequestHeaders
							(
								hRequest,
								szHeadLine,
								(DWORD)_tcslen( szHeadLine ),
								HTTP_ADDREQ_FLAG_REPLACE|HTTP_ADDREQ_FLAG_ADD
							);
						}
					}
					
					if ( bAddHeaders )
					{
						// 第四步：发出请求
						bSendRequest = ::HttpSendRequest( hRequest, NULL, 0, 0, 0 );
						if ( bSendRequest )
						{
							DWORD dwBytesRead;
							BOOL  bReadFile = InternetReadFile
								(
									hRequest, pstThread->pszResponse,
									pstThread->dwSize-sizeof(TCHAR),
									&dwBytesRead
								);
							if ( bReadFile && dwBytesRead > 0 )
							{
								pstThread->bRet = TRUE;
							}
							else
							{
								if ( pstThread->pszError )
									_tcscpy( pstThread->pszError, _T("获取服务器返回信息为空") );
							}
						}
						else
						{
							if ( pstThread->pszError )
								_stprintf( pstThread->pszError, _T("发出一个 %s 请求失败, ErrCode=%d"), pstThread->pszUrl, GetLastError() );
						}
					}
					else
					{
						if ( pstThread->pszError )
							_tcscpy( pstThread->pszError, _T("添加 HTTP 头部信息失败") );
					}
					::InternetCloseHandle( hRequest );
				}
				else
				{
					if ( pstThread->pszError )
						_tcscpy( pstThread->pszError, _T("HttpOpenRequest 失败，打开一个HTTP请求句柄失败") );
				}
				::InternetCloseHandle(hConnect);
			}
			else
			{
				if ( pstThread->pszError )
					_tcscpy( pstThread->pszError, _T("InternetConnect 失败，初始化HTTP session失败") );
			}
			::InternetCloseHandle( hSession );
		}
		else
		{
			if ( pstThread->pszError )
				_tcscpy( pstThread->pszError, _T("InternetOpen 失败，初始化Internet DLL失败") );
		}
	}
	catch ( ... )
	//__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("线程异常错误退出") );
		if ( hRequest )
			::InternetCloseHandle(hRequest);
		if ( hConnect )
			::InternetCloseHandle(hConnect);
		if ( hSession )
			::InternetCloseHandle(hSession);
	}

	return ( TRUE == pstThread->bRet ) ? HTTP_THREAD_SUCC : HTTP_THREAD_FAIL;
}




/**
 *	POST 一个 FORM 并且得到返回的缓冲区内容
 */
BOOL CDeHttp::PostForm( LPCTSTR lpcszUrl, LPTSTR lpszPostData, LPTSTR lpszResponse, DWORD dwSize, LPTSTR lpszError, DWORD dwTimeout )
{
	if ( FALSE == HaveInternetConnection() )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("当前没有网络连接") );
		return FALSE;
	}

	STHTTPTHREAD stHttpThread;

	stHttpThread.pvThisCls		= this;
	stHttpThread.bRet		= FALSE;
	stHttpThread.pszUrl		= lpcszUrl;
	stHttpThread.pszPostData	= lpszPostData;
	stHttpThread.pszResponse	= lpszResponse;
	stHttpThread.dwSize		= dwSize;
	stHttpThread.pszError		= lpszError;

	__try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				(PBEGINTHREADEX_THREADFUNC)PostFormProc,
				&stHttpThread,
				0,
				(PBEGINTHREADEX_THREADID)&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
			return FALSE;
		
		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			TerminateThread( stHttpThread.hThread, 0 );
			while ( WAIT_TIMEOUT == WaitForSingleObject( stHttpThread.hThread, 1000 ) )
			{
				TerminateThread( stHttpThread.hThread, 0 );
			}

			if ( lpszError )
				_tcscpy( lpszError, _T("网络连接超时") );
		}
		if ( stHttpThread.hThread )
			CloseHandle( stHttpThread.hThread );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("线程异常错误退出") );
	}

	return stHttpThread.bRet;
}

BOOL CDeHttp::PostFormEx( LPCTSTR lpcszUrl, LPTSTR lpszPostData, LPTSTR lpszAgent, LPTSTR lpszHeader, 
				LPTSTR lpszResponse, DWORD dwSize, LPTSTR lpszError, DWORD dwTimeout )
{
	if ( FALSE == HaveInternetConnection() )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("当前没有网络连接") );
		return FALSE;
	}

	STHTTPTHREAD stHttpThread;
	
	stHttpThread.bRet		= FALSE;
	stHttpThread.pszUrl		= lpcszUrl;
	stHttpThread.pszAgent		= lpszAgent;	// for V1.0.0.2
	stHttpThread.pszHeader		= lpszHeader;	// for V1.0.0.2
	stHttpThread.pszPostData	= lpszPostData;
	stHttpThread.pszResponse	= lpszResponse;
	stHttpThread.dwSize		= dwSize;
	stHttpThread.pszError		= lpszError;

	_try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				(PBEGINTHREADEX_THREADFUNC)PostFormProc,
				&stHttpThread,
				0,
				(PBEGINTHREADEX_THREADID)&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
			return FALSE;
		
		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			TerminateThread( stHttpThread.hThread, 0 );
			while ( WAIT_TIMEOUT == WaitForSingleObject( stHttpThread.hThread, 1000 ) )
			{
				TerminateThread( stHttpThread.hThread, 0 );
			}

			if ( lpszError )
				_tcscpy( lpszError, _T("网络连接超时") );
		}
		if ( stHttpThread.hThread )
			CloseHandle( stHttpThread.hThread );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("线程异常错误退出") );
	}
	
	return stHttpThread.bRet;
	
}

DWORD WINAPI CDeHttp::PostFormProc( LPVOID p )
{
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	CDeHttp * pCHttp = (CDeHttp*)pstThread->pvThisCls;
	pstThread->bRet = FALSE;
	
	if ( NULL == pCHttp )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("内部错误：无法找到类地址") );
		return HTTP_THREAD_FAIL;
	}
	if ( NULL == pstThread->pszUrl || 0 == _tcslen(pstThread->pszUrl) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("输入参数有错误") );
		return HTTP_THREAD_FAIL;
	}

	const TCHAR * szAcceptType = HTTP_ACCEPT_TYPE;
	const TCHAR * szContentType = _T("Content-Type: application/x-www-form-urlencoded\r\n");

	TCHAR szServer[MAX_PATH]	= {0};
	TCHAR szPath[MAX_PATH]		= {0};
#ifdef UNICODE
	STDEHTTPDEPARSEURLW stParseUrl;
#else
	STDEHTTPDEPARSEURL stParseUrl;
#endif
	
	if ( pCHttp->dehttp_delib_parse_url( pstThread->pszUrl, (DWORD)_tcslen(pstThread->pszUrl), & stParseUrl ) )
	{
		_sntprintf( szServer, sizeof(szServer)/sizeof(TCHAR)-sizeof(TCHAR), _T("%s"), stParseUrl.szHost );
		_sntprintf( szPath, sizeof(szPath)/sizeof(TCHAR)-sizeof(TCHAR), _T("%s?%s"), stParseUrl.szPath, stParseUrl.szQuery );
	}
	else
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("无法解析 URL 地址") );
		return HTTP_THREAD_FAIL;
	}

	if ( 0 == _tcslen(szServer) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("服务器名不能为空") );
		return HTTP_THREAD_FAIL;
	}
	if ( 0 == _tcslen(szPath) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("请求路径不能为空") );
		return HTTP_THREAD_FAIL;
	}
	if ( NULL == pstThread->pszPostData )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("请求内容指针无效") );
		return HTTP_THREAD_FAIL;
	}
	else if ( 0 == _tcslen(pstThread->pszPostData) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("请求内容不能为空") );
		return HTTP_THREAD_FAIL;
	}


	HINTERNET hSession		= NULL;
	HINTERNET hConnect		= NULL;
	HINTERNET hRequest		= NULL;
	DWORD dwFlags			= 0;

	BOOL bAddHeaders		= FALSE;
	BOOL bSendRequest		= FALSE;
	BOOL bReadFile			= FALSE;
	DWORD dwBytesRead		= 0;

	TCHAR szAgent[ MAX_PATH ]	= {0};
	TCHAR szHeader[ 1024 ]		= {0};
	TCHAR szSubHeader[ MAX_PATH ]	= {0};

	__try
	{
		if ( pstThread->pszAgent && _tcslen(pstThread->pszAgent) > 0 )
		{
			_sntprintf( szAgent, sizeof(szAgent)/sizeof(TCHAR)-1, _T("%s"), pstThread->pszAgent );
		}
		else
		{
			_sntprintf( szAgent, sizeof(szAgent)/sizeof(TCHAR)-1, _T("%s"), DEFAULT_AGENT_NAME );
		}

		if ( pstThread->pszHeader && _tcslen(pstThread->pszHeader) > 0 )
		{
			_sntprintf( szHeader, sizeof(szHeader)/sizeof(TCHAR)-1, _T("%s"), pstThread->pszHeader );
		}
		else
		{
			_sntprintf( szHeader, sizeof(szHeader)/sizeof(TCHAR)-1, _T("%s"), HTTP_ACCEPT );
		}

		// 第一步：初始化 internet DLL 会话句柄，这是第一个被调用的函数
		hSession = InternetOpen( szAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
		if ( hSession )
		{
			// 第二步：初始化 HTTP session 连接句柄
			hConnect = ::InternetConnect
				(
					hSession,		//	当前internet会话句柄
					szServer,		//	Server Name
					DEFAULT_PORT,		//	Server Port
					NULL,			//	User Name
					NULL,			//	Password
					INTERNET_SERVICE_HTTP,	//	Type of service to access
					0, 0
				);
			if ( hConnect )
			{
				////////////////////////////////////////////////////////////
				// 第三步：打开一个HTTP请求句柄
				dwFlags = INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_FORMS_SUBMIT;
				hRequest =:: HttpOpenRequest
					(
						hConnect,
						HTTP_VERB_POST,		//	HTTP Verb
						szPath,			//	Object Name
						HTTP_VERSION,		//	Version
						NULL,			//	Reference
						&szAcceptType,		//	Accept Type															
						dwFlags,		//	flag
						NULL			//	context call-back point
					);
				if ( hRequest ) 
				{
					// 第四步：添加 HTTP 协议头
					bAddHeaders	= TRUE;

					PTCHAR pszMove	= szHeader;
					PTCHAR pszTail	= NULL;
					pszTail = _tcsstr( pszMove, _T("\r\n") );
					while( _tcslen(pszMove) > 0 )
					{
						if ( pszTail )
						{
							* pszTail = '\0';
							pszTail += sizeof(TCHAR)*2;
						}
						_sntprintf( szSubHeader, sizeof(szSubHeader)/sizeof(TCHAR)-1, _T("%s\r\n"), pszMove );
						bAddHeaders &= ::HttpAddRequestHeaders
							(
								hRequest,
								szSubHeader,
								(DWORD)_tcslen(szSubHeader),
								HTTP_ADDREQ_FLAG_REPLACE|HTTP_ADDREQ_FLAG_ADD
							);
						if ( pszTail )
						{
							pszMove = pszTail;
							pszTail = _tcsstr( pszMove, _T("\r\n") );
						}
						else
							break;
					}
					if ( bAddHeaders )
					{
						//	第五步：发出请求 SEND REQUEST WITH POST ARGUEMENTS
						bSendRequest = ::HttpSendRequest
							(
								hRequest,				// handle by returned HttpOpenRequest
								szContentType,				// additional HTTP header
								(DWORD)_tcslen(szContentType),			// additional HTTP header length
								reinterpret_cast<LPVOID>(pstThread->pszPostData),	// additional data in HTTP Post or HTTP Put
								(DWORD)_tcslen(pstThread->pszPostData)			// additional data length
							);
						if ( FALSE == bSendRequest && 12168 == ::GetLastError() )
						{
							bSendRequest = TRUE;
						}

						//	第六步：得到返回内容
						if ( bSendRequest )
						{
							bReadFile = InternetReadFile( hRequest, pstThread->pszResponse, pstThread->dwSize-sizeof(TCHAR), &dwBytesRead );
							if ( bReadFile && dwBytesRead > 0 )
							{
								pstThread->bRet = TRUE;
							}
							else
							{
								if ( pstThread->pszError )
								{
									_tcscpy( pstThread->pszError, _T("获取服务器返回内容为空") );
								}
							}
						}
						else
						{
							if ( pstThread->pszError )
							{
								_stprintf( pstThread->pszError,
									_T("发出一个 %s 请求失败, ErrCode=%d"),
									pstThread->pszUrl, GetLastError() );
							}
						}
					}
					else
					{
						if ( pstThread->pszError )
							_tcscpy( pstThread->pszError, _T("添加 HTTP 头部信息失败") );
					}

					//////////////////////////////////////////////////
					::InternetCloseHandle(hRequest);
				}
				else
				{
					if ( pstThread->pszError )
						_tcscpy( pstThread->pszError, _T("HttpOpenRequest 失败，打开一个 HTTP 请求句柄失败") );
				}

				//////////////////////////////////////////////////
				::InternetCloseHandle(hConnect);
			}
			else
			{
				if ( pstThread->pszError )
					_tcscpy( pstThread->pszError, _T("InternetConnect 失败") );
			}

			//////////////////////////////////////////////////
			::InternetCloseHandle(hSession);
		}
		else
		{
			if ( pstThread->pszError )
				_tcscpy( pstThread->pszError, _T("初始化 Internet DLL 失败") );
		}

	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("线程异常错误退出") );
		if ( hRequest )
			::InternetCloseHandle(hRequest);
		if ( hConnect )
			::InternetCloseHandle(hConnect);
		if ( hSession )
			::InternetCloseHandle(hSession);
	}

	return ( TRUE == pstThread->bRet ) ? HTTP_THREAD_SUCC : HTTP_THREAD_FAIL;

}


//////////////////////////////////////////////////////////////////////////
//
// 从指定 URL 获取内容并保存到文件
BOOL CDeHttp::GetFile( LPCTSTR lpcszUrl, LPTSTR lpszDestFile, BOOL bTextFile, LPTSTR lpszError, DWORD dwTimeout )
{
	if ( FALSE == HaveInternetConnection() )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("当前没有网络连接") );
		return FALSE;
	}

	STHTTPTHREAD stHttpThread;

	stHttpThread.pvThisCls		= this;
	stHttpThread.bRet		= FALSE;
	stHttpThread.pszUrl		= lpcszUrl;
	stHttpThread.pszDestFile	= lpszDestFile;
	stHttpThread.bTextFile		= bTextFile;
	stHttpThread.pszError		= lpszError;

	__try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				(PBEGINTHREADEX_THREADFUNC)GetFileProc,
				&stHttpThread,
				0,
				(PBEGINTHREADEX_THREADID)&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
			return FALSE;

		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			TerminateThread( stHttpThread.hThread, 0 );
			while ( WAIT_TIMEOUT == WaitForSingleObject( stHttpThread.hThread, 1000 ) )
			{
				TerminateThread( stHttpThread.hThread, 0 );
			}

			if ( lpszError )
				_tcscpy( lpszError, _T("网络连接超时") );
		}
		if ( stHttpThread.hThread )
			CloseHandle( stHttpThread.hThread );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("线程异常错误退出") );
	}

	return stHttpThread.bRet;
}


BOOL CDeHttp::GetFileEx( LPCTSTR lpcszUrl, LPTSTR lpszAgent, LPTSTR lpszHeader, LPCTSTR lpcszDestFile, BOOL bTextFile, LPTSTR lpszError, DWORD dwTimeout )
{
	if ( FALSE == HaveInternetConnection() )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("当前没有网络连接") );
		return FALSE;
	}
	
	STHTTPTHREAD stHttpThread;

	stHttpThread.pvThisCls		= this;
	stHttpThread.bRet		= FALSE;
	stHttpThread.pszUrl		= lpcszUrl;
	stHttpThread.pszAgent		= lpszAgent;	// for V1.0.0.2
	stHttpThread.pszHeader		= lpszHeader;	// for V1.0.0.2
	stHttpThread.pszDestFile	= lpcszDestFile;
	stHttpThread.bTextFile		= bTextFile;
	stHttpThread.pszError		= lpszError;
	
	__try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				(PBEGINTHREADEX_THREADFUNC)GetFileProc,
				&stHttpThread,
				0,
				(PBEGINTHREADEX_THREADID)&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
			return FALSE;
		
		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			TerminateThread( stHttpThread.hThread, 0 );
			while ( WAIT_TIMEOUT == WaitForSingleObject( stHttpThread.hThread, 1000 ) )
			{
				TerminateThread( stHttpThread.hThread, 0 );
			}

			if ( lpszError )
				_tcscpy( lpszError, _T("网络连接超时") );
		}
		if ( stHttpThread.hThread )
			CloseHandle( stHttpThread.hThread );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("线程异常错误退出") );
	}
	
	return stHttpThread.bRet;
}

DWORD WINAPI CDeHttp::GetFileProc( LPVOID p )
{
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	pstThread->bRet = FALSE;

	if ( NULL == pstThread->pszUrl || NULL == pstThread->pszDestFile )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("输入参数地址有问题") );
		return HTTP_THREAD_FAIL;
	}
	else if ( 0 == _tcslen(pstThread->pszUrl) || 0 == _tcslen(pstThread->pszDestFile) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("输入参数有误，有空的参数") );
		return HTTP_THREAD_FAIL;
	}

	HINTERNET hInternet	= NULL;
	HINTERNET hHttpConnect	= NULL;

	DWORD dwTotalRead	= 0;
	DWORD dwBufferSize	= 0;
	DWORD dwBytesRead	= 0;
	LPVOID pBuffer		= NULL;
	FILE * fp		= NULL;

	TCHAR szAgent[ MAX_PATH ]	= {0};
	TCHAR szHeader[ 1024 ]		= {0};
	TCHAR szSubHeader[ MAX_PATH ]	= {0};
	
	__try
	{
		if ( pstThread->pszAgent && _tcslen(pstThread->pszAgent) > 0 )
			_sntprintf( szAgent, sizeof(szAgent)/sizeof(TCHAR)-1, _T("%s"), pstThread->pszAgent );
		else
			_sntprintf( szAgent, sizeof(szAgent)/sizeof(TCHAR)-1, _T("%s"), DEFAULT_AGENT_NAME );
		
		if ( pstThread->pszHeader && _tcslen(pstThread->pszHeader) > 0 )
			_sntprintf( szHeader, sizeof(szHeader)/sizeof(TCHAR)-1, _T("%s"), pstThread->pszHeader );
		else
			_sntprintf( szHeader, sizeof(szHeader)/sizeof(TCHAR)-1, _T("%s"), HTTP_ACCEPT );


		hInternet = InternetOpen( szAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
		if ( hInternet )
		{
			DWORD dwFlag = INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_PRAGMA_NOCACHE|INTERNET_FLAG_RELOAD;
			hHttpConnect = InternetOpenUrl
				(
					hInternet,
					pstThread->pszUrl, 
					szHeader, 
					(DWORD)_tcslen(szHeader), 
					dwFlag,
					0
				);

			dwTotalRead = 0;
			if ( hHttpConnect )
			{
				dwBufferSize	= 300*1024;	// 300K Buffer
				dwBytesRead	= 0;
				pBuffer		= malloc( dwBufferSize );
				if ( NULL == pBuffer )
				{
					if ( pstThread->pszError )
						_tcscpy( pstThread->pszError, _T("内存不够") );
					return HTTP_THREAD_FAIL;
				}
				if ( pstThread->bTextFile )
					fp = _tfopen( pstThread->pszDestFile, _T("wt") );
				else
					fp = _tfopen( pstThread->pszDestFile, _T("wb") );
				if ( fp )
				{
					while( InternetReadFile( hHttpConnect, pBuffer, dwBufferSize, &dwBytesRead ) )
					{
						if ( dwBytesRead == 0 ) 
							break;
						fwrite( pBuffer, dwBytesRead, 1, fp );
						dwTotalRead += dwBytesRead;
					}
					pstThread->bRet = TRUE;
					fclose( fp );
					if ( 0 == dwTotalRead )
					{
						if ( pstThread->pszError )
							_tcscpy( pstThread->pszError, _T("获取指定网页的内容为空") );
					}
				}
				else
				{
					if ( pstThread->pszError )
						_tcscpy( pstThread->pszError, _T("无法创建本地文件，请确认磁盘是否可写") );
				}
				if ( pBuffer )
				{
					free( pBuffer );
					pBuffer = NULL;
				}

				::InternetCloseHandle( hHttpConnect );
			}
			else
			{
				if ( pstThread->pszError )
					_tcscpy( pstThread->pszError, _T("InternetOpenUrl 失败，无法连接到指定 URL") );
			}

			::InternetCloseHandle( hInternet );
		}
		else
		{
			if ( pstThread->pszError )
				_tcscpy( pstThread->pszError, _T("InternetOpen 失败，可能是网络有问题") );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("线程异常错误退出") );
		if ( hHttpConnect )
			::InternetCloseHandle( hHttpConnect );
		if ( hInternet )
			::InternetCloseHandle( hInternet );
	}

	return ( TRUE == pstThread->bRet ) ? HTTP_THREAD_SUCC : HTTP_THREAD_FAIL;

}


//////////////////////////////////////////////////////////////////////////
// 上传文件
BOOL CDeHttp::PostFile( LPCTSTR lpcszUrl, STHTTPFORMDATA & stFormData, const HWND hMainWnd,
				LPTSTR lpszResponse, DWORD dwSize, LPTSTR lpszError, DWORD dwTimeout )
{
	if ( FALSE == HaveInternetConnection() )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("当前没有网络连接") );
		return FALSE;
	}

	STHTTPTHREAD stHttpThread;

	stHttpThread.pvThisCls		= this;
	stHttpThread.bRet		= FALSE;
	stHttpThread.pszUrl		= lpcszUrl;
	stHttpThread.stFormData		= &stFormData;
	stHttpThread.hMainWnd		= hMainWnd;
	stHttpThread.pszResponse	= lpszResponse;
	stHttpThread.dwSize		= dwSize;
	stHttpThread.pszError		= lpszError;

	__try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				(PBEGINTHREADEX_THREADFUNC)PostFileProc,
				&stHttpThread,
				0,
				(PBEGINTHREADEX_THREADID)&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
			return FALSE;
		
		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			TerminateThread( stHttpThread.hThread, 0 );
			while ( WAIT_TIMEOUT == WaitForSingleObject( stHttpThread.hThread, 1000 ) )
			{
				TerminateThread( stHttpThread.hThread, 0 );
			}

			if ( lpszError )
				_tcscpy( lpszError, _T("网络连接超时") );
		}
		if ( stHttpThread.hThread )
			CloseHandle( stHttpThread.hThread );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("线程异常错误退出") );
	}

	return stHttpThread.bRet;
}

DWORD WINAPI CDeHttp::PostFileProc( LPVOID p )
{
	TCHAR szServer[MAX_PATH]	= {0};
	TCHAR szPath[MAX_PATH]		= {0};

	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	CDeHttp * pCHttp = (CDeHttp*)pstThread->pvThisCls;
	pstThread->bRet = FALSE;

	if ( NULL == pCHttp )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("内部错误：无法找到类地址") );
		return HTTP_THREAD_FAIL;
	}
	if ( NULL == pstThread->pszUrl || 0 == _tcslen(pstThread->pszUrl) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("输入参数有错误") );
		return HTTP_THREAD_FAIL;
	}

#ifdef UNICODE
	STDEHTTPDEPARSEURLW stParseUrl;
#else
	STDEHTTPDEPARSEURL stParseUrl;
#endif

	if ( pCHttp->dehttp_delib_parse_url( pstThread->pszUrl, (DWORD)_tcslen(pstThread->pszUrl), & stParseUrl ) )
	{
		_sntprintf( szServer, sizeof(szServer)/sizeof(TCHAR)-1, _T("%s"), stParseUrl.szHost );
		_sntprintf( szPath, sizeof(szPath)/sizeof(TCHAR)-1, _T("%s?%s"), stParseUrl.szPath, stParseUrl.szQuery );
	}
	else
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("无法解析 URL 地址") );
		return HTTP_THREAD_FAIL;
	}
	/*
	if ( FALSE == pCHttp->ParseUrl( pstThread->pszUrl, szServer, szPath ) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("无法解析 URL 地址") );
		return HTTP_THREAD_FAIL;
	}
	*/
	
	if ( 0 == _tcslen(szServer) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("服务器名不能为空") );
		return HTTP_THREAD_FAIL;
	}
	if ( 0 == _tcslen(szPath) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("请求路径不能为空") );
		return HTTP_THREAD_FAIL;
	}

	HINTERNET hSession		= NULL;
	HINTERNET hConnect		= NULL;
	HINTERNET hRequest		= NULL;
	BOOL bSendRequest		= FALSE;

	__try
	{
		////////////////////////////////////////////////////////////
		// 第一步：初始化 internet DLL 会话句柄
		hSession = InternetOpen( DEFAULT_AGENT_NAME, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
		if ( hSession )
		{
			////////////////////////////////////////////////////////////
			// 第二步：初始化 HTTP session 连接句柄
			hConnect = ::InternetConnect
				(
					hSession,		// 当前internet会话句柄
					szServer,		// Server Name
					DEFAULT_PORT,		// Server Port
					NULL,			// User Name
					NULL,			// Password
					INTERNET_SERVICE_HTTP,	// Type of service to access
					0,
					0
				);
			if ( hConnect )
			{
				////////////////////////////////////////////////////////////
				// 第三步：打开一个 HTTP 请求连接句柄
				DWORD dwFlags = INTERNET_FLAG_KEEP_CONNECTION|INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_FORMS_SUBMIT;
				hRequest =:: HttpOpenRequest
					(
						hConnect,
						HTTP_VERB_POST,		// HTTP Verb
						szPath,			// Uri, Object Name
						HTTP_VERSION,		// Version
						NULL,			// Reference
						NULL,			// Accept Type // &szAcceptType
						dwFlags,
						NULL			// context call-back point
					);
				if ( hRequest && INVALID_HANDLE_VALUE != hRequest )
				{
					////////////////////////////////////////////////////////////
					// 第四步：发送内容并接收返回应答内容
					if ( pCHttp->UserHttpSendReqEx( 
							hRequest, *pstThread->stFormData, pstThread->hMainWnd, 
							pstThread->pszError ) )
					{
						DWORD dwBytesRead;
						if ( InternetReadFile( hRequest, pstThread->pszResponse, pstThread->dwSize-sizeof(TCHAR), &dwBytesRead ) )
						{
							if ( dwBytesRead > 0 )
								pstThread->bRet = TRUE;
						}
					}

					::InternetCloseHandle( hRequest );
				}
				else
				{
					if ( pstThread->pszError )
						_tcscpy( pstThread->pszError, _T("打开一个 HTTP 请求句柄失败") );
				}

				::InternetCloseHandle( hConnect );
			}
			else
			{
				if ( pstThread->pszError )
					_tcscpy( pstThread->pszError, _T("初始化 HTTP Session 失败") );
			}

			::InternetCloseHandle( hSession );
		}
		else
		{
			if ( pstThread->pszError )
				_tcscpy( pstThread->pszError, _T("初始化 Internet DLL 失败") );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("线程异常错误退出") );
		if ( hRequest )
			::InternetCloseHandle( hRequest );
		if ( hConnect )
			::InternetCloseHandle( hConnect );
		if ( hSession )
			::InternetCloseHandle( hSession );
	}

	return ( TRUE == pstThread->bRet ) ? HTTP_THREAD_SUCC : HTTP_THREAD_FAIL;

}





//////////////////////////////////////////////////////////////////////////
// 下载一个比较大的文件，该函数能长时间连接下载
BOOL CDeHttp::DownloadFile( LPCTSTR lpcszUrl, LPTSTR lpszDestFile, const HWND hMainWnd, BOOL bAddNoise, LPTSTR lpszError, DWORD dwTimeout )
{
	if ( FALSE == HaveInternetConnection() )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("当前没有网络连接") );
		return FALSE;
	}

	if ( NULL == lpcszUrl || 0 == _tcslen(lpcszUrl) )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("需要从哪里下载的 URL 参数有问题") );
		return HTTP_THREAD_FAIL;
	}
	if ( NULL == lpszDestFile || 0 == _tcslen(lpszDestFile) )
	{
		if ( lpszError )
			_tcscpy( lpszError, _T("下载到本地的文件名参数有问题") );
		return HTTP_THREAD_FAIL;
	}

	STHTTPTHREAD stHttpThread;

	stHttpThread.pvThisCls		= this;
	stHttpThread.bRet		= FALSE;
	stHttpThread.pszUrl		= lpcszUrl;
	stHttpThread.pszDestFile	= lpszDestFile;
	stHttpThread.pszError		= lpszError;
	stHttpThread.hMainWnd		= hMainWnd;
	stHttpThread.bAddNoise		= bAddNoise;
	stHttpThread.dwTimeout		= dwTimeout;

	stHttpThread.hThread = (HANDLE)_beginthreadex
		(
			NULL,
			0,
			(PBEGINTHREADEX_THREADFUNC)DownloadFileProc,
			&stHttpThread,
			0,
			(PBEGINTHREADEX_THREADID)&stHttpThread.uThreadID
		);
	if ( NULL == stHttpThread.hThread )
		return FALSE;
	
	if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
	{
		TerminateThread( stHttpThread.hThread, 0 );
		while ( WAIT_TIMEOUT == WaitForSingleObject( stHttpThread.hThread, 1000 ) )
		{
			TerminateThread( stHttpThread.hThread, 0 );
		}

		if ( lpszError )
			_tcscpy( lpszError, _T("网络连接超时") );
	}
	if ( stHttpThread.hThread )
		CloseHandle( stHttpThread.hThread );
	
	return stHttpThread.bRet;
}

DWORD WINAPI CDeHttp::DownloadFileProc( LPVOID p )
{
	CCallback callback;
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	pstThread->bRet = FALSE;

	TCHAR szUrlTmp[ DEFAULT_BUF_SIZE ]	= {0};
	TCHAR szUrl[ DEFAULT_BUF_SIZE ]		= {0};
	TCHAR * pszJin				= NULL;

	__try
	{
		_sntprintf( szUrlTmp, sizeof(szUrlTmp)/sizeof(TCHAR)-1, _T("%s"), pstThread->pszUrl );
		pszJin = _tcsstr( szUrlTmp, _T("#") );
		if ( pszJin )
			szUrlTmp[ (pszJin - szUrlTmp) ] = 0;

		if ( FALSE == pstThread->bAddNoise )
		{
			_sntprintf( szUrl, sizeof(szUrl)/sizeof(TCHAR)-1, _T("%s"), szUrlTmp );
		}
		else
		{
			if ( _tcsstr( szUrlTmp, CHECK_NOISE_FLAG ) )
			{
				_sntprintf( szUrl, sizeof(szUrl)/sizeof(TCHAR)-1,
					_T("%s%s%d"), szUrlTmp, URL_NOISE_2, GetTickCount() );
			}
			else
			{
				_sntprintf( szUrl, sizeof(szUrl)/sizeof(TCHAR)-1,
					_T("%s%s%d"), szUrlTmp, URL_NOISE_1, GetTickCount() );
			}
		}

		callback.m_hWnd		= pstThread->hMainWnd;
		callback.m_dwTimeout	= pstThread->dwTimeout;
		callback.m_dwStart	= GetTickCount();

		HRESULT hRet = URLDownloadToFile( NULL, szUrl, pstThread->pszDestFile, 0, &callback );
		if ( S_OK ==  hRet)
		{
			// 下载成功
			pstThread->bRet = TRUE;
		}
		else
		{
			// 下载失败
			DWORD dwErr = GetLastError();
			pstThread->bRet = FALSE;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		// 下载异常失败
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("线程异常错误退出") );
	}

	// 这个返回值其实没有什么意义
	return ( TRUE == pstThread->bRet ) ? HTTP_THREAD_SUCC : HTTP_THREAD_FAIL;
}



//////////////////////////////////////////////////////////////////////////
/*
	对输入字符串做FORM编码

	函数参数：
		strInput为输入串，不要包括url的非form部分
		nInputLen为输入串的长度，-1表示串以 '\0' 结尾
		strOutBuf为输出串缓冲区
		nOutBufLen为输出缓冲区大小，包括结尾的 '\0'

	返回值：
		输出串的长度，不包括结尾的0
	
	如果nOutBufLen的长度为0，则返回需要的输出缓冲区的大小，包括结尾的0
*/
INT CDeHttp::UrlEncode( LPCTSTR lpcszInput, INT nInputLen, LPTSTR lpszOutBuf, INT nOutBufLen )
{
	unsigned char cCur;
	TCHAR * pDst		= NULL;
	INT   i			= 0;
	INT   nDstLen		= 0;
	TCHAR szBuf[4]		= {0};
	INT   nBufLen		= 0;

	__try
	{
		if ( -1 == nInputLen )
			nInputLen = (INT)_tcslen(lpcszInput);

		if ( 0 == nOutBufLen )
		{
			nDstLen = 0;
			for ( i = 0; i < nInputLen; i++ )
			{
				cCur = lpcszInput[i];
				if ( ' ' == cCur )
					nDstLen++;
				else if ( !isalnum(cCur) )
					nDstLen += 3;
				else
					nDstLen ++;
			}
			return nDstLen + 1;
		}

		pDst = lpszOutBuf;
		nDstLen = 0;
		for ( i = 0; i < nInputLen; i++ )
		{
			cCur = lpcszInput[i];
			if ( ' ' == cCur )
			{
				*szBuf = '+';
				*(szBuf+1) = 0;
			}
			else if ( ! isalnum(cCur) )
			{
				_sntprintf( szBuf, sizeof(szBuf)/sizeof(TCHAR)-1, _T("%%%2.2X"), cCur );
			}	
			else
			{
				*szBuf = cCur;
				*(szBuf+1) = 0;
			}
			nBufLen = (INT)_tcslen( szBuf );
			if ( nBufLen + nDstLen >= nOutBufLen )
				break;
			_tcscpy( pDst, szBuf );
			nDstLen += nBufLen;
			pDst += nBufLen;
		}
		return (INT)_tcslen(lpszOutBuf);
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		return 0;
	}
}





//////////////////////////////////////////////////////////////////////////
// 私有函数
//////////////////////////////////////////////////////////////////////////


BOOL CDeHttp::UserHttpSendReqEx( HINTERNET hRequest, STHTTPFORMDATA & stFormData, HWND hMainWnd, TCHAR * pszError )
{
	////////////////////////////////////////////////////////////
	HANDLE hFile				= NULL;
	DWORD dwFileSize			= 0;
	hFile = CreateFile( stFormData.szUserFileValue, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0 );
	if ( INVALID_HANDLE_VALUE == hFile )
	{
		if ( pszError )
			_sntprintf( pszError, MAX_PATH-1, _T("无法打开文件：%s"), stFormData.szUserFileValue );
		return FALSE;
	}
	else
	{
		dwFileSize = ::GetFileSize( hFile, NULL );
		CloseHandle( hFile );
	}


	////////////////////////////////////////////////////////////
	DWORD dwTotalPostSize			= 0;
	TCHAR szFormExtra[ MAX_PATH ]		= {0};
	TCHAR szFormUserFile[ MAX_PATH ]	= {0};
	TCHAR szFormFiles[ MAX_PATH ]		= {0};
	TCHAR szFormSubmit[ MAX_PATH ]		= {0};

	dwTotalPostSize = 0;

	// 1 form -> files
	if ( 0 == _tcslen(stFormData.szFilesInput) )
	{	
		_tcscpy( stFormData.szFilesInput, DEFAULT_FORM_INPUT_FILES );
	}

	_sntprintf( szFormFiles, sizeof(szFormFiles)/sizeof(TCHAR)-1,
		_T("--%s\r\nContent-Disposition: form-data; name=\"%s\"\r\n\r\n%s\r\n\r\n"),
		DEFAULT_BOUNDRY_TAG,
		stFormData.szFilesInput, stFormData.szFilesValue );
	dwTotalPostSize += (DWORD)_tcslen( szFormFiles );

	// 2 form -> Extra
	_sntprintf( szFormExtra, sizeof(szFormExtra)/sizeof(TCHAR)-1,
		_T("--%s\r\nContent-Disposition: form-data; name=\"%s\"\r\n\r\n%s\r\n\r\n"),
		DEFAULT_BOUNDRY_TAG,
		stFormData.szExtraInput, stFormData.szExtraValue );
	dwTotalPostSize += _tcslen( szFormExtra );

	// 3 form -> userfile
	if ( 0 == _tcslen(stFormData.szUserFileInput) )
		_tcscpy( stFormData.szUserFileInput, DEFAULT_FORM_INPUT_USERFILE );
	_sntprintf( szFormUserFile, sizeof(szFormUserFile)/sizeof(TCHAR)-1,
		_T("--%s\r\nContent-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\nContent-Type: */*\r\n%s\r\n\r\n"),
		DEFAULT_BOUNDRY_TAG,
		stFormData.szUserFileInput, stFormData.szUserFileValue,
		_T("Content-Type: application/octet-stream"));
	dwTotalPostSize += _tcslen( szFormUserFile );
	dwTotalPostSize += dwFileSize;

	// 4 form -> submit
	_sntprintf( szFormSubmit, sizeof(szFormSubmit)/sizeof(TCHAR)-1,
		_T("\r\n\r\n--%s\r\nContent-Disposition: form-data; name=\"submit\"\r\n\r\nsubmit\r\n--%s--"),
		DEFAULT_BOUNDRY_TAG, DEFAULT_BOUNDRY_TAG );
	dwTotalPostSize += _tcslen( szFormSubmit );


	TCHAR szHttpHeader[ DEFAULT_BUF_SIZE ]	= {0};

	_tcscpy( szHttpHeader, _T("") );
	if ( TRUE == stFormData.bGZipOpen )
	{
		_tcscpy( szHttpHeader, _T("Accept-Encoding: gzip, deflate\r\n") );
	}
	_tcscat( szHttpHeader, _T("Accept-Language: zh-cn\r\n") );
	_tcscat( szHttpHeader, _T("Content-Type: multipart/form-data; boundary=") );
	_tcscat( szHttpHeader, DEFAULT_BOUNDRY_TAG );
	_tcscat( szHttpHeader, _T("\r\n") );


	////////////////////////////////////////////////////////////
	INTERNET_BUFFERS BufferIn;

	BufferIn.dwStructSize		= sizeof( INTERNET_BUFFERS ); // Must be set or error will occur
	BufferIn.Next			= NULL; 
	BufferIn.lpcszHeader		= szHttpHeader;
	BufferIn.dwHeadersLength	= (DWORD)_tcslen(szHttpHeader);
	BufferIn.dwHeadersTotal		= (DWORD)_tcslen(szHttpHeader);
	BufferIn.lpvBuffer		= NULL;
	BufferIn.dwBufferLength		= 0;
	BufferIn.dwBufferTotal		= dwTotalPostSize; // This is the only member used other than dwStructSize
	BufferIn.dwOffsetLow		= 0;
	BufferIn.dwOffsetHigh		= 0;

	if ( FALSE == ::HttpSendRequestEx( hRequest, &BufferIn, NULL, 0, 0) )
	{
		_sntprintf( pszError, MAX_PATH-1, _T("Error on HttpSendRequestEx %d"), GetLastError() );
		return FALSE;
	}


	BOOL bWriteFile				= FALSE;
	DWORD dwWritten				= 0;
	DWORD dwRead				= 0;
	DWORD dwFileByteSent			= 0;
	TCHAR szFileBuffer[ DEFAULT_BUF_SIZE ] = {0};
	


	// 1 sending .......... <files> 文件名列表
	bWriteFile = ::InternetWriteFile( hRequest, 
		szFormFiles, (DWORD)_tcslen(szFormFiles), &dwWritten );

	// 2 sending .......... <Extra> 一些额外数据
	bWriteFile = ::InternetWriteFile( hRequest, 
		szFormExtra, (DWORD)_tcslen(szFormExtra), &dwWritten );

	// 3 sending .......... <userfile> 文件
	bWriteFile = ::InternetWriteFile( hRequest, 
		szFormUserFile, (DWORD)_tcslen(szFormUserFile), &dwWritten );

	hFile = ::CreateFile( stFormData.szUserFileValue, 
				GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
	if ( hFile && INVALID_HANDLE_VALUE != hFile )
	{
		while ( ::ReadFile( hFile, szFileBuffer, sizeof(szFileBuffer), &dwRead, 0 ) && dwRead > 0 )
		{
			dwWritten = 0;
			bWriteFile = ::InternetWriteFile( hRequest, szFileBuffer, dwRead, &dwWritten );
			dwFileByteSent += dwRead;
			memset( szFileBuffer, 0, sizeof(szFileBuffer) );
			
			if ( hMainWnd )
				::PostMessage( hMainWnd, UM_HTTP_PREGRESS, (INT)(dwFileByteSent/1024), (INT)(dwFileSize/1024) );
		}
		if ( hFile )
			::CloseHandle( hFile );
	}

	// 4 sending .......... <submit>
	bWriteFile = ::InternetWriteFile( hRequest, szFormSubmit, (DWORD)_tcslen(szFormSubmit), &dwWritten );


	if ( FALSE == ::HttpEndRequest( hRequest, NULL, HSR_ASYNC | HSR_INITIATE, 0 ) )
	{
		_sntprintf( pszError, MAX_PATH-1, _T("Error on HttpEndRequest %d"), GetLastError() );
		return FALSE;
	}

	if ( hMainWnd )
		::PostMessage( hMainWnd, UM_HTTP_COMPLETE, 0, 0 );


	return TRUE;
}




BOOL CDeHttp::ParseUrl( const TCHAR * pszUrl, TCHAR * pszServer, TCHAR * pszPath )
{
	if ( NULL == pszUrl || NULL == pszServer || NULL == pszPath )
		return FALSE;

	DWORD i					= 0;
	DWORD j					= 0;
	DWORD dwFlags				= 0;		//"/"的个数
	
	if ( pszUrl == _tcsstr(pszUrl,_T("http://")) )
		j = 7;		// 带HTTP则从第7个开始搜索
	else
		j = 0;		// 没有发现
	
	for ( i = j; i < _tcslen(pszUrl); i++ )
	{
		if ( 1 == dwFlags || '/' == pszUrl[i] )
		{
			if ( 1 == dwFlags )
			{
				pszPath[ i-j-1 ] = pszUrl[i];
			}
			else
			{
				j = i;
				dwFlags = 1;
			}
		}
		else
		{
			pszServer[i-j] = pszUrl[i];
		}
	}

	return TRUE;
}






/**
 *	parse url
 */
BOOL CDeHttp::dehttp_delib_parse_url(LPCSTR lpcszUrl, DWORD dwUrlLen, STDEHTTPDEPARSEURL * pstParseUrl )
{
	return dehttp_delib_parse_urlA( lpcszUrl, dwUrlLen, pstParseUrl );
}
BOOL CDeHttp::dehttp_delib_parse_urlW( LPCWSTR lpcwszUrl, DWORD dwUrlLen, STDEHTTPDEPARSEURLW * pstParseUrlW )
{
	if ( NULL == lpcwszUrl || 0 == lpcwszUrl[0] || 0 == dwUrlLen )
	{
		return FALSE;
	}
	if ( NULL == pstParseUrlW )
	{
		return FALSE;
	}

	BOOL bRet;
	CHAR * pszTmpUrl;
	STDEHTTPDEPARSEURL stParseUrl;

	bRet		= FALSE;

	__try
	{
		pszTmpUrl	= new CHAR[ dwUrlLen + 1 ];
		if ( pszTmpUrl )
		{
			memset( pszTmpUrl, 0, dwUrlLen + 1 );
			memset( &stParseUrl, 0, sizeof(stParseUrl) );
			
			WideCharToMultiByte( CP_ACP, 0, lpcwszUrl, dwUrlLen, pszTmpUrl, dwUrlLen, NULL, NULL );
			if ( dehttp_delib_parse_urlA( pszTmpUrl, strlen(pszTmpUrl), &stParseUrl ) )
			{
				memset( pstParseUrlW, 0, sizeof(STDEHTTPDEPARSEURLW) );
				MultiByteToWideChar( CP_ACP, 0, stParseUrl.szFragment, strlen(stParseUrl.szFragment), pstParseUrlW->szFragment, sizeof(pstParseUrlW->szFragment)/2 );
				MultiByteToWideChar( CP_ACP, 0, stParseUrl.szHost, strlen(stParseUrl.szHost), pstParseUrlW->szHost, sizeof(pstParseUrlW->szHost)/2 );
				MultiByteToWideChar( CP_ACP, 0, stParseUrl.szPass, strlen(stParseUrl.szPass), pstParseUrlW->szPass, sizeof(pstParseUrlW->szPass)/2 );
				MultiByteToWideChar( CP_ACP, 0, stParseUrl.szPath, strlen(stParseUrl.szPath), pstParseUrlW->szPath, sizeof(pstParseUrlW->szPath)/2 );
				MultiByteToWideChar( CP_ACP, 0, stParseUrl.szQuery, strlen(stParseUrl.szQuery), pstParseUrlW->szQuery, sizeof(pstParseUrlW->szQuery)/2 );
				MultiByteToWideChar( CP_ACP, 0, stParseUrl.szScheme, strlen(stParseUrl.szScheme), pstParseUrlW->szScheme, sizeof(pstParseUrlW->szScheme)/2 );
				MultiByteToWideChar( CP_ACP, 0, stParseUrl.szUser, strlen(stParseUrl.szUser), pstParseUrlW->szUser, sizeof(pstParseUrlW->szUser)/2 );

				bRet = TRUE;
			}

			delete [] pszTmpUrl;
			pszTmpUrl = NULL;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return bRet;
}
BOOL CDeHttp::dehttp_delib_parse_urlA( LPCSTR lpcszUrl, DWORD dwUrlLen, STDEHTTPDEPARSEURL * pstParseUrl )
{
	//
	//	lpcszUrl		- [in]  URL
	//	dwUrlLen		- [in]  URL 长度
	//	pstParseUrl		- [out] 返回值
	//	RETURN			- TRUE / FALSE
	//

	if ( NULL == lpcszUrl || 0 == lpcszUrl[0] )
		return FALSE;
	if ( NULL == pstParseUrl )
		return FALSE;


	BOOL bRet	= FALSE;
	UINT uBufLen	= 0;

	char port_buf[6];
//	php_url * ret = (php_url*)calloc( 1, sizeof(php_url) );
	char const *s, *e, *p, *pp, *ue;

	s = lpcszUrl;
	ue = s + dwUrlLen;

	//
	//	parse scheme
	//
	if ( ( e = (char*)memchr( s, ':', dwUrlLen ) ) && ( e - s ) )
	{
		//	validate scheme
		p = s;
		while ( p < e )
		{
			//	scheme = 1*[ lowalpha | digit | "+" | "-" | "." ]
			if ( ! isalpha(*p) && !isdigit(*p) && *p != '+' && *p != '.' && *p != '-')
			{
				if ( e + 1 < ue )
				{
					goto parse_port;
				}
				else
				{
					goto just_path;
				}
			}
			p++;
		}

		if ( *( e + 1 ) == '\0' )
		{
			//	only scheme is available
			dehttp_delibfunc_memcpy( s, ( e - s ), pstParseUrl->szScheme, sizeof( pstParseUrl->szScheme ), TRUE );
			goto end;
		}

		/**
		 *	certain schemas like mailto: and zlib: may not have any / after them
		 *	this check ensures we support those.
		 */
		if ( *(e+1) != '/' )
		{
			/**
			 *	check if the data we get is a port this allows us to 
			 *	correctly parse things like a.com:80
			 */
			p = e + 1;
			while ( isdigit(*p) )
			{
				p++;
			}

			if ( (*p) == '\0' || *p == '/' )
			{
				goto parse_port;
			}

			dehttp_delibfunc_memcpy( s, (e-s), pstParseUrl->szScheme, sizeof( pstParseUrl->szScheme ), TRUE );

			dwUrlLen -= ++e - s;
			s = e;
			goto just_path;
		}
		else
		{
			dehttp_delibfunc_memcpy( s, (e-s), pstParseUrl->szScheme, sizeof( pstParseUrl->szScheme ), TRUE );

			if ( *(e+2) == '/' )
			{
				s = e + 3;
				if ( 0 == strnicmp( "file", pstParseUrl->szScheme, sizeof("file") ) )
				{
					if ( *(e + 3) == '/' )
					{
						/**
						 *	support windows drive letters as in:
						 *	file:///c:/somedir/file.txt
						 */
						if ( *(e + 5) == ':' )
						{
							s = e + 4;
						}
						goto nohost;
					}
				}
			}
			else
			{
				if ( 0 == strnicmp( "file", pstParseUrl->szScheme, sizeof("file") ) )
				{
					s = e + 1;
					goto nohost;
				}
				else
				{
					dwUrlLen -= ++e - s;
					s = e;
					goto just_path;
				}	
			}
		}
	}
	else if (e)
	{
		//
		//	no scheme, look for port
		//
		parse_port:
		p = e + 1;
		pp = p;

		while ( pp-p < 6 && isdigit(*pp) )
		{
			pp++;
		}

		if ( pp - p < 6 && (*pp == '/' || *pp == '\0') )
		{
			memcpy( port_buf, p, (pp-p) );
			port_buf[pp-p] = '\0';
			pstParseUrl->uPort = atoi(port_buf);
		}
		else
		{
			goto just_path;
		}
	}
	else
	{
		just_path:
		ue = s + dwUrlLen;
		goto nohost;
	}
	
	e = ue;
	
	if ( ! ( p = (char*)memchr(s, '/', (ue - s)) ) )
	{
		if ( ( p = (char*)memchr(s, '?', (ue - s)) ) )
		{
			e = p;
		}
		else if ( ( p = (char*)memchr(s, '#', (ue - s) ) ) )
		{
			e = p;
		}
	}
	else
	{
		e = p;
	}	

	//
	//	check for login and password
	//
	if ( ( p = (char*)memchr(s, '@', (e-s)) ) )
	{
		if ( ( pp = (char*)memchr(s, ':', (p-s)) ) )
		{
			if ( ( pp - s ) > 0 )
			{
				//ret->user = zend_strndup( s, (pp-s) );
				dehttp_delibfunc_memcpy( s, (pp-s), pstParseUrl->szUser, sizeof( pstParseUrl->szUser ), TRUE );
			}

			pp++;
			if ( p - pp > 0 )
			{
				//ret->pass = zend_strndup( pp, (p-pp) );
				dehttp_delibfunc_memcpy( pp, (p-pp), pstParseUrl->szPass, sizeof( pstParseUrl->szPass ), TRUE );
			}
		}
		else
		{
			//ret->user = zend_strndup(s, (p-s));
			dehttp_delibfunc_memcpy( s, (p-s), pstParseUrl->szUser, sizeof( pstParseUrl->szUser ), TRUE );
		}

		s = p + 1;
	}

	//
	//	check for port
	//
	if ( *s == '[' && *(e-1) == ']' )
	{
		/*
			Short circuit portscan, 
			we're dealing with an 
			IPv6 embedded address
		*/
		p = s;
	}
	else
	{
		/*
			memrchr is a GNU specific extension
			Emulate for wide compatability
		*/
		for ( p = e; *p != ':' && p >= s; p-- );
		{
		}
	}

	if ( p >= s && *p == ':' )
	{
		if ( ! pstParseUrl->uPort )
		{
			p ++;
			if ( e - p > 5 )
			{
				//	port cannot be longer then 5 characters
				//STR_FREE( ret->scheme );
				//STR_FREE( ret->user );
				//STR_FREE( ret->pass );
				//free( ret );
				return FALSE;
			}
			else if ( e - p > 0 )
			{
				memcpy(port_buf, p, (e-p));
				port_buf[e-p] = '\0';
				pstParseUrl->uPort = atoi(port_buf);
			}
			p--;
		}	
	}
	else
	{
		p = e;
	}
	
	//
	//	check if we have a valid host, if we don't reject the string as url
	//
	if ( ( p - s ) < 1 )
	{
		//STR_FREE(ret->scheme);
		//STR_FREE(ret->user);
		//STR_FREE(ret->pass);
		//free(ret);
		return FALSE;
	}

	//ret->host = estrndup(s, (p-s));
	dehttp_delibfunc_memcpy( s, (p-s), pstParseUrl->szHost, sizeof( pstParseUrl->szHost ), TRUE );

	if ( e == ue )
	{
		return TRUE;
	}

	s = e;
	
	nohost:
	
	if ( ( p = (char*)memchr( s, '?', (ue - s) ) ) )
	{
		pp = strchr( s, '#' );
		
		if ( pp && pp < p )
		{
			p = pp;
			pp = strchr( pp + 2, '#' );
		}

		if ( p - s )
		{
			//ret->path = estrndup(s, (p-s));
			dehttp_delibfunc_memcpy( s, (p-s), pstParseUrl->szPath, sizeof( pstParseUrl->szPath ), TRUE );
		}	

		if ( pp )
		{
			if ( pp - ++p )
			{
				//ret->query = estrndup(p, (pp-p));
				dehttp_delibfunc_memcpy( p, (pp-p), pstParseUrl->szQuery, sizeof( pstParseUrl->szQuery ), TRUE );
			}
			p = pp;
			goto label_parse;
		}
		else if ( ++p - ue )
		{
			//ret->query = estrndup(p, (ue-p));
			dehttp_delibfunc_memcpy( p, (ue-p), pstParseUrl->szQuery, sizeof( pstParseUrl->szQuery ), TRUE );
		}
	}
	else if ( ( p = (char*)memchr(s, '#', (ue - s)) ) )
	{
		if ( p - s )
		{
			//ret->path = estrndup(s, (p-s));
			dehttp_delibfunc_memcpy( s, (p-s), pstParseUrl->szPath, sizeof( pstParseUrl->szPath ), TRUE );
		}

		label_parse:
		p++;

		if ( ue - p )
		{
			//ret->fragment = estrndup(p, (ue-p));
			dehttp_delibfunc_memcpy( p, (ue-p), pstParseUrl->szFragment, sizeof( pstParseUrl->szFragment ), TRUE );
		}
	}
	else
	{
		//ret->path = estrndup(s, (ue-s));
		dehttp_delibfunc_memcpy( s, (ue-s), pstParseUrl->szPath, sizeof( pstParseUrl->szPath ), TRUE );
	}
end:
	return TRUE;
}


/**
 *	替换不可见的控制符号为“_”
 */
TCHAR * CDeHttp::dehttp_delibfunc_replace_controlchars( LPSTR lpszStr, UINT uLen )
{
	//
	//	lpszStr		- [in]
	//	uLen		- [in]
	//
	
	if ( NULL == lpszStr )
	{
		return NULL;
	}
	
	unsigned char *s = (unsigned char *)lpszStr;
	unsigned char *e = (unsigned char *)lpszStr + uLen;
	
	while ( s < e )
	{
		if ( iscntrl( *s ) )
		{
			*s = '_';
		}
		s++;
	}
	
	return lpszStr;
}

/**
*	拷贝字符串
*/
BOOL CDeHttp::dehttp_delibfunc_memcpy( LPCTSTR lpcszSrc, DWORD dwSrcLen, LPTSTR lpszDst, DWORD dwDstSize, BOOL bRepCtlChr /*= FALSE*/ )
{
	if ( NULL == lpcszSrc || NULL == lpszDst )
		return FALSE;
	if ( 0 == dwSrcLen || 0 == dwDstSize )
		return FALSE;
	
	UINT uLen = min( dwSrcLen, dwDstSize-sizeof(TCHAR) );
	
	memcpy( lpszDst, lpcszSrc, uLen );
	lpszDst[ uLen ] = 0;
	
	if ( bRepCtlChr )
	{
		dehttp_delibfunc_replace_controlchars( lpszDst, uLen );
	}
	
	return TRUE;
}