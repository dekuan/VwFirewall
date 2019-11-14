// http.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "http.h"




//////////////////////////////////////////////////////////////////////////
// 判断是否存在Internet连接
BOOL HttpHaveInternetConnection()
{
	BOOL  bRet		= FALSE;
	DWORD dwFlag		= 0;

	__try
	{
		//	如果函数返回FALSE，则肯定没有连接
		bRet = InternetGetConnectedState( &dwFlag, 0 );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		bRet = FALSE;
	}

	return bRet;
}


/**
 *	获取某个 URL 的连接状态代码
 */
DWORD HttpGetStatusCode( LPCTSTR lpcszUrl, LPCTSTR pszAgent, vector<STHTTPCOOKIE> * pvcCookie, LPSTR pszError, DWORD dwTimeout )
{
	/*
		pszUrl		- [in]  URL
		pvcCookie	- [in]  Cookie
		pszError	- [out] 错误信息
		dwTimeout	- [in]  超时，单位：秒
		
		RETURN		- HTTP STATUS CODE
	*/

	if ( FALSE == HttpHaveInternetConnection() )
	{
		if ( pszError )
			_tcscpy( pszError, _T("当前没有网络连接") );
		return FALSE;
	}

	STHTTPTHREAD stHttpThread;

	stHttpThread.dwRet		= GETSTATUSCODE_ERROR;
	stHttpThread.pvcCookie		= pvcCookie;
	stHttpThread.pszUrl		= lpcszUrl;
	stHttpThread.pszAgent		= pszAgent;
	stHttpThread.pszError		= pszError;

	__try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				HttpGetStatusCodeProc,
				&stHttpThread,
				0,
				&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
			return FALSE;

		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			stHttpThread.dwRet = GETSTATUSCODE_ERROR;
			if ( pszError )
				_tcscpy( pszError, _T("网络连接超时") );
		}
		if ( stHttpThread.hThread )
			CloseHandle( stHttpThread.hThread );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( pszError )
			_tcscpy( pszError, _T("线程异常错误退出") );
	}

	return stHttpThread.dwRet;
}
static unsigned int __stdcall HttpGetStatusCodeProc( VOID * p )
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


	HINTERNET hSession		= NULL;
	HINTERNET hHttpFile		= NULL;
	TCHAR szQueryBuf[ MAX_PATH ]	= {0};
	DWORD dwQueryBufLen		= 0;
	BOOL  bQuerySucc		= FALSE;
	BOOL  bFindDateTag		= FALSE;
	vector<STHTTPCOOKIE>::iterator	it;
	STHTTPCOOKIE * pstHttpCookie;
	TCHAR szAgent[ MAX_PATH ]	= {0};
	TCHAR szTemp[ MAX_PATH ]	= {0};


	pstThread->dwRet = GETSTATUSCODE_ERROR;

	__try
	{
		//
		//	设置 Cookie
		//
		if ( pstThread->pvcCookie )
		{
			for ( it = pstThread->pvcCookie->begin(); it != pstThread->pvcCookie->end(); it ++ )
			{
				pstHttpCookie = it;
				InternetSetCookie( pstThread->pszUrl, pstHttpCookie->szName, pstHttpCookie->szData );
			}
		}

		if ( pstThread->pszAgent && _tcslen(pstThread->pszAgent) > 0 )
			_sntprintf( szAgent, sizeof(szAgent)-sizeof(TCHAR), _T("%s"), pstThread->pszAgent );
		else
			_sntprintf( szAgent, sizeof(szAgent)-sizeof(TCHAR), _T("%s"), _T(DEFAULT_AGENT_NAME) );

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

				if ( bQuerySucc && _tcslen(szQueryBuf) > 0 )
				{
					//
					//	检查是否有 Date: 标记
					//	主要是为了检测 HTTP 头是否完整
					//
					if ( strstr( szQueryBuf, "Date:" ) )
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
						pstThread->dwRet = (DWORD)atol(szQueryBuf);
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
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("线程异常错误退出") );
		if ( hHttpFile )
			::InternetCloseHandle( hHttpFile );
		if ( hSession )
			::InternetCloseHandle( hSession );
	}

	return pstThread->dwRet;
}



//////////////////////////////////////////////////////////////////////////
// 根据 URL 得到返回的缓冲区内容
BOOL HttpGetResponse( LPCTSTR pszUrl, LPCTSTR pszAgent, LPTSTR pszResponse, DWORD dwSize, LPTSTR pszError, DWORD dwTimeout )
{
	if ( FALSE == HttpHaveInternetConnection() )
	{
		if ( pszError )
			_tcscpy( pszError, _T("当前没有网络连接") );
		return FALSE;
	}

	STHTTPTHREAD stHttpThread;
	
	stHttpThread.bRet		= FALSE;
	stHttpThread.pszUrl		= pszUrl;
	stHttpThread.pszResponse	= pszResponse;
	stHttpThread.dwSize		= dwSize;
	stHttpThread.pszError		= pszError;

	__try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				HttpGetResponseProc,
				&stHttpThread,
				0,
				&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
			return FALSE;

		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			if ( pszError )
				_tcscpy( pszError, _T("网络连接超时") );
		}
		if ( stHttpThread.hThread )
			CloseHandle( stHttpThread.hThread );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( pszError )
			_tcscpy( pszError, _T("线程异常错误退出") );
	}

	return stHttpThread.bRet;
}

static unsigned int __stdcall HttpGetResponseProc( VOID * p )
{
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	pstThread->bRet = FALSE;

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
	ULONG uPort			= 80;

	if ( FALSE == ParseUrl( pstThread->pszUrl, szServer, szPath, & uPort ) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("无法解析 URL 地址") );
		return HTTP_THREAD_FAIL;
	}

	HINTERNET hSession		= NULL;
	HINTERNET hConnect		= NULL;
	HINTERNET hRequest		= NULL;
	BOOL bSendRequest		= FALSE;
	DWORD dwBytesRead		= 0;
	TCHAR szAgent[ MAX_PATH ]	= {0};


	__try
	{
		if ( pstThread->pszAgent && _tcslen(pstThread->pszAgent) > 0 )
			_sntprintf( szAgent, sizeof(szAgent)-sizeof(TCHAR), _T("%s"), pstThread->pszAgent );
		else
			_sntprintf( szAgent, sizeof(szAgent)-sizeof(TCHAR), _T("%s"), _T(DEFAULT_AGENT_NAME) );
		
		// 第一步:初始化 internet DLL 会话句柄
		hSession = ::InternetOpen( szAgent, PRE_CONFIG_INTERNET_ACCESS, _T(""), INTERNET_INVALID_PORT_NUMBER, 0 );
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
				// 第三步：打开一个HTTP请求句柄
				hRequest = ::HttpOpenRequest(
					hConnect, _T(HTTP_VERB_GET), szPath, HTTP_VERSION, NULL, 0, INTERNET_FLAG_DONT_CACHE, 0 );
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
					////////////////////////////////////////
					::InternetCloseHandle(hRequest);
				}
				else
				{
					if ( pstThread->pszError )
						_tcscpy( pstThread->pszError, _T("HttpOpenRequest 失败，打开一个HTTP请求句柄失败") );
				}
				////////////////////////////////////////
				::InternetCloseHandle(hConnect);
			}
			else
			{
				if ( pstThread->pszError )
					_tcscpy( pstThread->pszError, _T("InternetConnect 失败，初始化HTTP session失败") );
			}
			////////////////////////////////////////
			::InternetCloseHandle(hSession);
		}
		else
		{
			if ( pstThread->pszError )
				_tcscpy( pstThread->pszError, _T("InternetOpen 失败，初始化Internet DLL失败") );
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
// POST 一个 FORM 并且得到返回的缓冲区内容
BOOL HttpPostForm( LPCTSTR pszUrl, LPTSTR pszPostData, LPCTSTR pszAgent, 
		LPTSTR pszResponse, DWORD dwSize, LPTSTR pszError, DWORD dwTimeout )
{
	if ( FALSE == HttpHaveInternetConnection() )
	{
		if ( pszError )
			_tcscpy( pszError, _T("当前没有网络连接") );
		return FALSE;
	}

	STHTTPTHREAD stHttpThread;

	stHttpThread.bRet		= FALSE;
	stHttpThread.pszUrl		= pszUrl;
	stHttpThread.pszAgent		= pszAgent;
	stHttpThread.pszPostData	= pszPostData;
	stHttpThread.pszResponse	= pszResponse;
	stHttpThread.dwSize		= dwSize;
	stHttpThread.pszError		= pszError;

	_try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				HttpPostFormProc,
				&stHttpThread,
				0,
				&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
			return FALSE;
		
		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			if ( pszError )
				_tcscpy( pszError, _T("网络连接超时") );
		}
		if ( stHttpThread.hThread )
			CloseHandle( stHttpThread.hThread );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( pszError )
			_tcscpy( pszError, _T("线程异常错误退出") );
	}

	return stHttpThread.bRet;
}


static unsigned int __stdcall HttpPostFormProc( VOID * p )
{
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	pstThread->bRet = FALSE;

	if ( NULL == pstThread->pszUrl || 0 == _tcslen(pstThread->pszUrl) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("输入参数有错误") );
		return HTTP_THREAD_FAIL;
	}

	const TCHAR * szAcceptType = _T(HTTP_ACCEPT_TYPE);
	const TCHAR * szContentType = _T("Content-Type: application/x-www-form-urlencoded\r\n");

	TCHAR szServer[MAX_PATH]	= {0};
	TCHAR szPath[MAX_PATH]		= {0};
	ULONG uPort			= 0;

	if ( FALSE == ParseUrl( pstThread->pszUrl, szServer, szPath, & uPort ) )
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
			_sntprintf( szAgent, sizeof(szAgent)-sizeof(TCHAR), _T("%s"), pstThread->pszAgent );
		else
			_sntprintf( szAgent, sizeof(szAgent)-sizeof(TCHAR), _T("%s"), _T(DEFAULT_AGENT_NAME) );

		if ( pstThread->pszHeader && _tcslen(pstThread->pszHeader) > 0 )
			_sntprintf( szHeader, sizeof(szHeader)-sizeof(TCHAR), _T("%s"), pstThread->pszHeader );
		else
			_sntprintf( szHeader, sizeof(szHeader)-sizeof(TCHAR), _T("%s"), _T(HTTP_ACCEPT) );

		// 第一步：初始化 internet DLL 会话句柄，这是第一个被调用的函数
		hSession = InternetOpen( szAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
		if ( hSession )
		{
			// 第二步：初始化 HTTP session 连接句柄
			hConnect = ::InternetConnect
				(
					hSession,		// 当前internet会话句柄
					szServer,		// Server Name
					uPort,			// Server Port
					NULL,			// User Name
					NULL,			// Password
					INTERNET_SERVICE_HTTP,	// Type of service to access
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
						HTTP_VERB_POST,		// HTTP Verb
						szPath,			// Object Name
						HTTP_VERSION,		// Version
						NULL,			// Reference
						&szAcceptType,		// Accept Type															
						dwFlags,		// flag
						NULL			// context call-back point
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
						_sntprintf( szSubHeader, sizeof(szSubHeader)-sizeof(TCHAR), _T("%s\r\n"), pszMove );
						bAddHeaders &= ::HttpAddRequestHeaders
							(
								hRequest,
								szSubHeader,
								_tcslen(szSubHeader),
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
						// 第五步：发出请求 SEND REQUEST WITH POST ARGUEMENTS
						bSendRequest = ::HttpSendRequest
							(
								hRequest,				// handle by returned HttpOpenRequest
								szContentType,				// additional HTTP header
								_tcslen(szContentType),			// additional HTTP header length
								reinterpret_cast<LPVOID>(pstThread->pszPostData),	// additional data in HTTP Post or HTTP Put
								_tcslen(pstThread->pszPostData)			// additional data length
							);
						if ( FALSE == bSendRequest && 12168 == ::GetLastError() )
							bSendRequest = TRUE;

						// 第六步：得到返回内容
						if ( bSendRequest )
						{
							bReadFile = InternetReadFile( hRequest, pstThread->pszResponse, pstThread->dwSize-sizeof(TCHAR), &dwBytesRead );
							if ( bReadFile && dwBytesRead > 0 )
								pstThread->bRet = TRUE;
							else
							{
								if ( pstThread->pszError )
									_tcscpy( pstThread->pszError, _T("获取服务器返回内容为空") );
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
// 上传文件
BOOL HttpPostFile( LPCTSTR pszUrl, STHTTPFORMDATA & stFormData, LPCTSTR pszAgent, const HWND hMainWnd, UINT uFupMessage, 
		  LPTSTR pszResponse, DWORD dwSize, LPTSTR pszError, DWORD dwTimeout )
{
	if ( FALSE == HttpHaveInternetConnection() )
	{
		if ( pszError )
			_tcscpy( pszError, _T("当前没有网络连接") );
		return FALSE;
	}

	STHTTPTHREAD stHttpThread;
	stHttpThread.bRet		= FALSE;
	stHttpThread.pszUrl		= pszUrl;
	stHttpThread.stFormData		= &stFormData;
	stHttpThread.pszAgent		= pszAgent;
	stHttpThread.hMainWnd		= hMainWnd;
	stHttpThread.uFupMessage	= uFupMessage;
	stHttpThread.pszResponse	= pszResponse;
	stHttpThread.dwSize		= dwSize;
	stHttpThread.pszError		= pszError;

	__try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				HttpPostFileProc,
				&stHttpThread,
				0,
				&stHttpThread.uThreadID
			);
		if ( NULL == stHttpThread.hThread )
			return FALSE;
		
		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			if ( pszError )
				_tcscpy( pszError, _T("网络连接超时") );
		}
		if ( stHttpThread.hThread )
			CloseHandle( stHttpThread.hThread );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( pszError )
			_tcscpy( pszError, _T("线程异常错误退出") );
	}

	return stHttpThread.bRet;
}

static unsigned int __stdcall HttpPostFileProc( VOID * p )
{
	STHTTPTHREAD * pstThread = (STHTTPTHREAD*)p;
	pstThread->bRet = FALSE;

	TCHAR szServer[MAX_PATH]	= {0};
	TCHAR szPath[MAX_PATH]		= {0};
	ULONG uPort			= 0;

	if ( NULL == pstThread->pszUrl || 0 == _tcslen(pstThread->pszUrl) )
	{
		if ( pstThread->pszError )
			_tcscpy( pstThread->pszError, _T("输入参数有错误") );
		return HTTP_THREAD_FAIL;
	}

	if ( FALSE == ParseUrl( pstThread->pszUrl, szServer, szPath, & uPort ) )
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

	HINTERNET hSession		= NULL;
	HINTERNET hConnect		= NULL;
	HINTERNET hRequest		= NULL;
	BOOL bSendRequest		= FALSE;
	TCHAR szAgent[ MAX_PATH ]	= {0};

	__try
	{
		if ( pstThread->pszAgent && _tcslen(pstThread->pszAgent) > 0 )
			_sntprintf( szAgent, sizeof(szAgent)-sizeof(TCHAR), _T("%s"), pstThread->pszAgent );
		else
			_sntprintf( szAgent, sizeof(szAgent)-sizeof(TCHAR), _T("%s"), _T(DEFAULT_AGENT_NAME) );

		////////////////////////////////////////////////////////////
		// 第一步：初始化 internet DLL 会话句柄
		hSession = InternetOpen( szAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
		if ( hSession )
		{
			////////////////////////////////////////////////////////////
			// 第二步：初始化 HTTP session 连接句柄
			hConnect = ::InternetConnect
				(
					hSession,		// 当前internet会话句柄
					szServer,		// Server Name
					uPort,			// Server Port
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
					if ( TRUE == 
						UserHttpSendReqEx( 
							hRequest, *pstThread->stFormData,
							pstThread->hMainWnd, pstThread->uFupMessage,
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
INT UrlEncode( const TCHAR * pszInput, INT nInputLen, TCHAR * pszOutBuf, INT nOutBufLen )
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
			nInputLen = _tcslen(pszInput);

		if ( 0 == nOutBufLen )
		{
			nDstLen = 0;
			for ( i = 0; i < nInputLen; i++ )
			{
				cCur = pszInput[i];
				if ( ' ' == cCur )
					nDstLen++;
				else if ( !isalnum(cCur) )
					nDstLen += 3;
				else
					nDstLen ++;
			}
			return nDstLen + 1;
		}

		pDst = pszOutBuf;
		nDstLen = 0;
		for ( i = 0; i < nInputLen; i++ )
		{
			cCur = pszInput[i];
			if ( ' ' == cCur )
			{
				*szBuf = '+';
				*(szBuf+1) = 0;
			}
			else if ( !isalnum(cCur) )
				sprintf( szBuf, _T("%%%2.2X"), cCur );
			else
			{
				*szBuf = cCur;
				*(szBuf+1) = 0;
			}
			nBufLen = _tcslen( szBuf );
			if ( nBufLen + nDstLen >= nOutBufLen )
				break;
			_tcscpy( pDst, szBuf );
			nDstLen += nBufLen;
			pDst += nBufLen;
		}
		return _tcslen(pszOutBuf);
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		return 0;
	}
}



//////////////////////////////////////////////////////////////////////////
// 非导出函数
//////////////////////////////////////////////////////////////////////////

BOOL UserHttpSendReqEx( HINTERNET hRequest, STHTTPFORMDATA & stFormData, HWND hMainWnd, UINT uFupMessage, TCHAR * pszError )
{
	////////////////////////////////////////////////////////////
	HANDLE hFile				= NULL;
	DWORD dwFileSize			= 0;
	hFile = CreateFile( stFormData.szUserFileValue, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0 );
	if ( INVALID_HANDLE_VALUE == hFile )
	{
		if ( pszError )
			_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("无法打开文件：%s"), stFormData.szUserFileValue );
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
		_tcscpy( stFormData.szFilesInput, _T(DEFAULT_FORM_INPUT_FILES) );
	_sntprintf( szFormFiles, sizeof(szFormFiles)-sizeof(TCHAR),
		_T("--%s\r\nContent-Disposition: form-data; name=\"%s\"\r\n\r\n%s\r\n\r\n"),
		_T(DEFAULT_BOUNDRY_TAG), 
		_T(stFormData.szFilesInput), _T(stFormData.szFilesValue) );
	dwTotalPostSize += _tcslen( szFormFiles );

	// 2 form -> Extra
	_sntprintf( szFormExtra, sizeof(szFormExtra)-sizeof(TCHAR),
		_T("--%s\r\nContent-Disposition: form-data; name=\"%s\"\r\n\r\n%s\r\n\r\n"),
		_T(DEFAULT_BOUNDRY_TAG), 
		_T(stFormData.szExtraInput), _T(stFormData.szExtraValue) );
	dwTotalPostSize += _tcslen( szFormExtra );

	// 3 form -> userfile
	if ( 0 == _tcslen(stFormData.szUserFileInput) )
		_tcscpy( stFormData.szUserFileInput, _T(DEFAULT_FORM_INPUT_USERFILE) );
	_sntprintf( szFormUserFile, sizeof(szFormUserFile)-sizeof(TCHAR),
		_T("--%s\r\nContent-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\nContent-Type: */*\r\n%s\r\n\r\n"),
		_T(DEFAULT_BOUNDRY_TAG), 
		_T(stFormData.szUserFileInput), _T(stFormData.szUserFileValue),
		_T("Content-Type: application/octet-stream"));
	dwTotalPostSize += _tcslen( szFormUserFile );
	dwTotalPostSize += dwFileSize;

	// 4 form -> submit
	_sntprintf( szFormSubmit, sizeof(szFormSubmit)-sizeof(TCHAR),
		_T("\r\n\r\n--%s\r\nContent-Disposition: form-data; name=\"submit\"\r\n\r\nsubmit\r\n--%s--"),
		_T(DEFAULT_BOUNDRY_TAG), _T(DEFAULT_BOUNDRY_TAG) );
	dwTotalPostSize += _tcslen( szFormSubmit );


	TCHAR szHttpHeader[ DEFAULT_BUF_SIZE ]	= {0};

	_tcscpy( szHttpHeader, _T("") );
	if ( TRUE == stFormData.bGZipOpen )
	{
		_tcscpy( szHttpHeader, _T("Accept-Encoding: gzip, deflate\r\n") );
	}
	_tcscat( szHttpHeader, _T("Accept-Language: zh-cn\r\n") );
	_tcscat( szHttpHeader, _T("Content-Type: multipart/form-data; boundary=") );
	_tcscat( szHttpHeader, _T(DEFAULT_BOUNDRY_TAG) );
	_tcscat( szHttpHeader, _T("\r\n") );


	////////////////////////////////////////////////////////////
	INTERNET_BUFFERS BufferIn;

	BufferIn.dwStructSize		= sizeof( INTERNET_BUFFERS ); // Must be set or error will occur
	BufferIn.Next			= NULL; 
	BufferIn.lpcszHeader		= _T(szHttpHeader);
	BufferIn.dwHeadersLength	= _tcslen(szHttpHeader);
	BufferIn.dwHeadersTotal		= _tcslen(szHttpHeader);
	BufferIn.lpvBuffer		= NULL;
	BufferIn.dwBufferLength		= 0;
	BufferIn.dwBufferTotal		= dwTotalPostSize; // This is the only member used other than dwStructSize
	BufferIn.dwOffsetLow		= 0;
	BufferIn.dwOffsetHigh		= 0;

	if ( FALSE == ::HttpSendRequestEx( hRequest, &BufferIn, NULL, 0, 0) )
	{
		_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("Error on HttpSendRequestEx %d"), GetLastError() );
		return FALSE;
	}


	BOOL bWriteFile				= FALSE;
	DWORD dwWritten				= 0;
	DWORD dwRead				= 0;
	DWORD dwFileByteSent			= 0;
	TCHAR szFileBuffer[ DEFAULT_BUF_SIZE ] = {0};
	

	// --> 消息：开始上传文件
	if ( hMainWnd && uFupMessage > 0 )
		::PostMessage( hMainWnd, uFupMessage, WP_HTTP_START, (INT)(dwFileSize/1024) );


	// 1 sending .......... <files> 文件名列表
	bWriteFile = ::InternetWriteFile( hRequest, 
		szFormFiles, _tcslen(szFormFiles), &dwWritten );

	// 2 sending .......... <Extra> 一些额外数据
	bWriteFile = ::InternetWriteFile( hRequest, 
		szFormExtra, _tcslen(szFormExtra), &dwWritten );

	// 3 sending .......... <userfile> 文件
	bWriteFile = ::InternetWriteFile( hRequest, 
		szFormUserFile, _tcslen(szFormUserFile), &dwWritten );

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

			// --> 消息：文件上传中
			if ( hMainWnd && uFupMessage > 0 )
				::PostMessage( hMainWnd, uFupMessage, WP_HTTP_PREGRESS, (INT)(dwFileByteSent/1024) );
		}
		if ( hFile )
			::CloseHandle( hFile );
	}

	// 4 sending .......... <submit>
	bWriteFile = ::InternetWriteFile( hRequest, szFormSubmit, _tcslen(szFormSubmit), &dwWritten );


	if ( FALSE == ::HttpEndRequest( hRequest, NULL, HSR_ASYNC | HSR_INITIATE, 0 ) )
	{
		_sntprintf( pszError, MAX_PATH-sizeof(TCHAR), _T("Error on HttpEndRequest %d"), GetLastError() );
		return FALSE;
	}

	// --> 消息：文件上传结束
	if ( hMainWnd && uFupMessage > 0 )
		::PostMessage( hMainWnd, uFupMessage, WP_HTTP_COMPLETE, 0 );


	return TRUE;
}

BOOL ParseUrl( const TCHAR * pszUrl, TCHAR * pszServer, TCHAR * pszPath, ULONG * puPort )
{
	if ( NULL == pszUrl || NULL == pszServer || NULL == pszPath )
		return FALSE;

	DWORD i			= 0;
	DWORD j			= 0;
	DWORD dwFlags		= 0;		//"/"的个数
	LPCSTR lpcszPort	= NULL;
	
	if ( pszUrl == _tcsstr(pszUrl,_T("http://")) )
		j = 7;		// 带HTTP则从第7个开始搜索
	else
		j = 0;		// 没有发现
	
	for ( i = j; i < _tcslen(pszUrl); i++ )
	{
		if ( ':' == pszUrl[i] )
		{
			lpcszPort = pszUrl + i + 1;
			break;
		}

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

	if ( 0 == strlen(pszPath) )
	{
		pszPath[ 0 ] = '/';
	}

	if ( puPort )
	{
		if ( lpcszPort && *lpcszPort )
			*puPort = atol( lpcszPort );
		else
			*puPort = 80;
	}

	return TRUE;
}