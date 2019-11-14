// DeSocketIoComp.cpp: implementation of the CDeSocketIoComp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeSocketIoComp.h"

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeSocketIoComp::CDeSocketIoComp()
{
	//	继续工作标记，设置为 TRUE
	m_bContinue		= TRUE;

	memset( &m_szPassword, 0, sizeof(m_szPassword) );
}

CDeSocketIoComp::~CDeSocketIoComp()
{
}

/**
 *	@ Public
 *	开始服务
 */
LONG CDeSocketIoComp::StartServer( LPCTSTR lpcszIpAddr, UINT uPort, INT nProtocol,
				PFNDESOCKETIOCOMPMSGPROCESSOR lpvResponseFunc /* = NULL */,
				LPCTSTR lpcszPassword /* = NULL */,
				LPVOID lpvArglist /* = NULL */ )
{
	if ( NULL == lpcszIpAddr || 0 == uPort )
	{
		return DESOCKETIOCOMP_ERROR_PARAM;
	}

	if ( IPPROTO_TCP != nProtocol && IPPROTO_UDP != nProtocol )
	{
		nProtocol = IPPROTO_TCP;
	}

	WSADATA wsaData;
	SOCKET sListen;
	SOCKET sClient;
	SOCKADDR_IN saLocal;
	SOCKADDR_IN saClient;
	DWORD i;
	UINT  uThreadId;
	DWORD dwProcessorNumber			= 0;
	int iaddrSize				= sizeof(SOCKADDR_IN);
	HANDLE hCompletionPort			= INVALID_HANDLE_VALUE;
	ULONG ArrCompThreads[ DESOCKETIOCOMP_MAX_COMPLETION_THREAD_COUNT ];
	LPPER_IO_OPERATION_DATA lpPerIOData	= NULL;


//	try
	{
		//
		//	save config
		//
		memset( & m_stConfig, 0, sizeof(m_stConfig) );

		m_stConfig.lpvResponseFunc	= lpvResponseFunc;
		m_stConfig.lpvArglist		= lpvArglist;
		_sntprintf( m_stConfig.szBindIpAddr, sizeof(m_stConfig.szBindIpAddr)-sizeof(TCHAR), "%s", lpcszIpAddr );
		m_stConfig.uPort		= uPort;
		m_stConfig.nProtocol		= nProtocol;

		//	Initialize Windows Socket library
		if ( 0 != WSAStartup( MAKEWORD(2,2), &wsaData ) )
		{
			return DESOCKETIOCOMP_WSA_STARTUP;
		}

		//	Create completion port
		hCompletionPort = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );
		if ( NULL == hCompletionPort )
		{
			return DESOCKETIOCOMP_CREATE_IO_COMPPORT;
		}

		//	Find out how many processors are on this system
		dwProcessorNumber = min( delib_get_processor_number(), DESOCKETIOCOMP_MAX_COMPLETION_THREAD_COUNT );


		//	Create worker thread
		for ( i = 0; i < dwProcessorNumber * 2 + 2; i++ )
		{
			//ArrCompThreads[ i ] = CreateThread( NULL, 0, _threadWorker, hCompletionPort, 0, &dwThreadId );
			ArrCompThreads[ i ] = _beginthreadex
				(
					NULL,
					0,
					&_threadWorker,
					(void*)( new DE_SOCKET_IO_COMP_THREAD( this, hCompletionPort ) ),
					0,
					&uThreadId
				);

			if ( -1 == ArrCompThreads[ i ] )
			{
				return DESOCKETIOCOMP_CREATE_THREAD;
			}
		}

		//	Create listening socket
		sListen = socket( AF_INET, SOCK_STREAM, nProtocol );
		if ( INVALID_SOCKET == sListen )
		{
			return DESOCKETIOCOMP_SOCKET;
		}

		//	Bind
		saLocal.sin_family		= AF_INET;
		saLocal.sin_addr.S_un.S_addr	= ( lpcszIpAddr ? inet_addr( lpcszIpAddr ) : htonl(INADDR_ANY) );
		saLocal.sin_port		= htons( uPort );
		if ( SOCKET_ERROR == bind( sListen, (PSOCKADDR)&saLocal, sizeof(saLocal) ) )
		{
			return DESOCKETIOCOMP_BIND;
		}

		//	Listen
		if ( SOCKET_ERROR == listen( sListen, SOMAXCONN ) )
		{
			return DESOCKETIOCOMP_LISTEN;
		}


		//
		//	到这里基本上算是成功了
		//
		SetPassword( lpcszPassword );


		//
		//	等待请求的到来 ...
		//
		while ( m_bContinue )
		{
			try
			{
				//	Accept a connection
				memset( &saClient, 0, sizeof(saClient) );
				sClient = accept( sListen, (struct sockaddr *)&saClient, &iaddrSize );
				if ( INVALID_SOCKET != sClient )
				{
					//	Associate the newly arrived client socket with completion port
					if ( CreateIoCompletionPort( (HANDLE)sClient, hCompletionPort, (DWORD)sClient, 0 ) )
					{
						//	Launch an asynchronous operation for new arrived connection
						lpPerIOData = (LPPER_IO_OPERATION_DATA)HeapAlloc
							(
								GetProcessHeap(),
								HEAP_ZERO_MEMORY,
								sizeof(PER_IO_OPERATION_DATA)
							);
						if ( lpPerIOData )
						{
							lpPerIOData->saClient		= saClient;
							lpPerIOData->stBuffer.len	= sizeof(lpPerIOData->szMessage);
							lpPerIOData->stBuffer.buf	= lpPerIOData->szMessage;
							lpPerIOData->OperationType	= RECV_POSTED;

							//	receive data from client
							WSARecv
							(
								sClient,
								&lpPerIOData->stBuffer,
								1,
								&lpPerIOData->dwNumberOfBytesRecvd,
								&lpPerIOData->dwFlags,
								&lpPerIOData->overlap,
								NULL
							);
						}
					}
				}
			}
			catch ( ... )
			{
			}	
		}

		PostQueuedCompletionStatus( hCompletionPort, 0xFFFFFFFF, 0, NULL );
		CloseHandle( hCompletionPort );
		closesocket( sListen );
		WSACleanup();
	}
//	catch ( ... )
//	{
//		return DESOCKETIOCOMP_ERROR_UNKNOWN;
//	}

	return DESOCKETIOCOMP_ERROR_SUCC;
}

/**
 *	@ Public
 *	停止服务
 */
LONG CDeSocketIoComp::StopServer()
{
	TCHAR szResponse[ MAX_PATH ]	= {0};

	//	设置停止标记
	m_bContinue = FALSE;

	//	发送一个消息，这样就可以解决 accept 的阻塞
	SendMessage( m_stConfig.szBindIpAddr, m_stConfig.uPort, m_stConfig.nProtocol, "hello,stop now.", szResponse, sizeof(szResponse) );

	return DESOCKETIOCOMP_ERROR_SUCC;
}
LONG CDeSocketIoComp::StopServer( LPCTSTR lpcszIpAddr, UINT uPort, INT nProtocol /*= IPPROTO_TCP*/ )
{
	if ( NULL == lpcszIpAddr || uPort > 65535 )
	{
		return DESOCKETIOCOMP_ERROR_PARAM;
	}

	TCHAR szResponse[ MAX_PATH ]	= {0};

	//	设置停止标记
	m_bContinue = FALSE;
	
	//	发送一个消息，这样就可以解决 accept 的阻塞
	SendMessage( lpcszIpAddr, uPort, nProtocol, "hello,stop now.", szResponse, sizeof(szResponse) );

	return DESOCKETIOCOMP_ERROR_SUCC;
}



/**
 *	发送消息给服务端
 */
LONG CDeSocketIoComp::SendMessage( LPCTSTR lpcszIpAddr, UINT uPort, INT nProtocol, LPCTSTR lpcszMessage, LPTSTR lpszResponse, DWORD dwSize )
{
	if ( NULL == lpcszIpAddr || 0 == _tcslen(lpcszIpAddr) )
	{
		return DESOCKETIOCOMP_ERROR_PARAM;
	}
	if ( NULL == lpcszMessage || 0 == _tcslen(lpcszMessage) )
	{
		return DESOCKETIOCOMP_ERROR_PARAM;
	}
	if ( 0 == dwSize )
	{
		return DESOCKETIOCOMP_ERROR_PARAM;
	}

	LONG	    lnRet;
	WSADATA     wsaData;
	SOCKET      sClient;
	SOCKADDR_IN server;
	int         ret;


	//	...
	lnRet = DESOCKETIOCOMP_ERROR_UNKNOWN;

	__try
	{
		//	Initialize Windows socket library
		if ( 0 == WSAStartup( MAKEWORD(2,2), &wsaData ) )
		{
			// Create client socket
			sClient = socket( AF_INET, SOCK_STREAM, nProtocol );
			if ( INVALID_SOCKET != sClient )
			{
				//	Connect to server
				memset( &server, 0, sizeof(SOCKADDR_IN) );
				server.sin_family		= AF_INET;
				server.sin_addr.S_un.S_addr	= inet_addr(lpcszIpAddr);
				server.sin_port			= htons(uPort);

				if ( 0 == connect( sClient, (struct sockaddr *)&server, sizeof(SOCKADDR_IN) ) )
				{
					//	...
					lnRet = DESOCKETIOCOMP_ERROR_SUCC;

					//	Send message
					send( sClient, lpcszMessage, strlen(lpcszMessage), 0 );
					
					//	Receive message
					memset( lpszResponse, 0, dwSize );
					ret = recv( sClient, lpszResponse, dwSize, 0 );
					lpszResponse[ ret ] = '\0';
				}
				
				//	Clean up
				closesocket( sClient );
			}
			else
			{
				lnRet = DESOCKETIOCOMP_SOCKET;
			}

			WSACleanup();
		}
		else
		{
			lnRet = DESOCKETIOCOMP_WSA_STARTUP;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}	

	return lnRet;
}

LONG CDeSocketIoComp::SendMessageEx( LPCTSTR lpcszIpAddr, UINT uPort, INT nProtocol, LPCTSTR lpcszMessage, DWORD dwMsgLength, LPTSTR lpszResponse, DWORD dwSize )
{
	if ( NULL == lpcszIpAddr || 0 == _tcslen(lpcszIpAddr) )
	{
		return DESOCKETIOCOMP_ERROR_PARAM;
	}
	if ( NULL == lpcszMessage )
	{
		return DESOCKETIOCOMP_ERROR_PARAM;
	}

	LONG	    lnRet;
	WSADATA     wsaData;
	SOCKET      sClient;
	SOCKADDR_IN server;
	int         ret;

	//	...
	lnRet = DESOCKETIOCOMP_ERROR_UNKNOWN;

	__try
	{
		//	Initialize Windows socket library
		if ( 0 == WSAStartup( MAKEWORD(2,2), &wsaData ) )
		{
			// Create client socket
			sClient = socket( AF_INET, SOCK_STREAM, nProtocol );
			if ( INVALID_SOCKET != sClient )
			{
				//	Connect to server
				memset( &server, 0, sizeof(SOCKADDR_IN) );
				server.sin_family		= AF_INET;
				server.sin_addr.S_un.S_addr	= inet_addr(lpcszIpAddr);
				server.sin_port			= htons(uPort);

				if ( 0 == connect( sClient, (struct sockaddr *)&server, sizeof(SOCKADDR_IN) ) )
				{
					//	...
					lnRet = DESOCKETIOCOMP_ERROR_SUCC;

					//	Send message
					send( sClient, lpcszMessage, dwMsgLength, 0 );

					//	Receive message
					if ( lpszResponse && dwSize )
					{
						memset( lpszResponse, 0, dwSize );
						ret = recv( sClient, lpszResponse, dwSize, 0 );
						lpszResponse[ ret ] = '\0';
					}
				}
				
				//	Clean up
				closesocket( sClient );
			}
			else
			{
				lnRet = DESOCKETIOCOMP_SOCKET;
			}

			WSACleanup();
		}
		else
		{
			lnRet = DESOCKETIOCOMP_WSA_STARTUP;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return lnRet;
}

VOID CDeSocketIoComp::SetPassword( LPCTSTR lpcszPassword )
{
	if ( lpcszPassword )
	{
		_sntprintf( m_szPassword, sizeof(m_szPassword)-sizeof(TCHAR), _T("%s"), lpcszPassword );
	}
}

BOOL CDeSocketIoComp::IsValidPassword( LPCTSTR lpcszPassword )
{
	BOOL bRet	= FALSE;

	if ( _tcslen(m_szPassword) )
	{
		if ( lpcszPassword && _tcslen(lpcszPassword) )
		{
			if ( 0 == _tcsicmp( m_szPassword, lpcszPassword ) )
			{
				bRet = TRUE;
			}
		}
	}
	else
	{
		if ( NULL == lpcszPassword )
		{
			bRet = TRUE;
		}
		else if ( 0 == _tcslen(lpcszPassword) )
		{
			bRet = TRUE;
		}
	}

	return bRet;
}



//////////////////////////////////////////////////////////////////////////
//	Private


/**
 *	thread of worker
 */
unsigned __stdcall CDeSocketIoComp::_threadWorker( PVOID arglist )
{
	if ( NULL == arglist )
	{
		return 1;
	}

	__try
	{
		DE_SOCKET_IO_COMP_THREAD * pstThread	= (DE_SOCKET_IO_COMP_THREAD*)arglist;
		CDeSocketIoComp * pThis;
		if ( pstThread && pstThread->lpvThis && pstThread->hCompletionPort )
		{
			pThis	= (CDeSocketIoComp*)pstThread->lpvThis;
			if ( pThis )
			{
				pThis->WorkerProc( pstThread->hCompletionPort );
			}
			else
			{
				return -1;
			}
		}
		else
		{
			return -1;
		}

		_endthreadex( 0 );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return 0;
}
DWORD CDeSocketIoComp::WorkerProc( LPVOID lpvCompletionPortID )
{
	HANDLE hCompletionPort		= (HANDLE)lpvCompletionPortID;
	DWORD  dwBytesTransferred;
	SOCKET sClient;
	LPPER_IO_OPERATION_DATA lpPerIOData = NULL;
	BOOL bGetQue;
	CHAR szResponse[ DESOCKETIOCOMP_MSGSIZE ];

	while ( m_bContinue )
	{
		__try
		{
			//	BOOL WINAPI GetQueuedCompletionStatus
			//	(
			//		__in   HANDLE CompletionPort,
			//		__out  LPDWORD lpNumberOfBytes,
			//		__out  PULONG_PTR lpCompletionKey,
			//		__out  LPOVERLAPPED *lpOverlapped,
			//		__in   DWORD dwMilliseconds
			//	);
			bGetQue = GetQueuedCompletionStatus
			(
				hCompletionPort,
				&dwBytesTransferred,
				(PULONG_PTR)&sClient,
				(LPOVERLAPPED*)&lpPerIOData,
				INFINITE
			);
			if ( INVALID_SOCKET == sClient )
			{
				//continue;
			}
			else if ( FALSE == bGetQue && NULL == lpPerIOData )
			{
				//	Illegal call to GetQueuedCompletionStatus
				//continue;
			}
			else if ( FALSE == bGetQue && NULL != lpPerIOData )
			{
				//	This happens occasionally instead of end-of-file. Not sure why.
				//	I/o operation failed
				closesocket( sClient );
				if ( lpPerIOData )
				{
					HeapFree( GetProcessHeap(), 0, lpPerIOData );
					lpPerIOData = NULL;
				}
				//continue;
			}
			else if ( 0 == dwBytesTransferred )
			{
				//	End of file.
				//	Or Connection was closed by client
				closesocket( sClient );
				if ( lpPerIOData )
				{
					HeapFree( GetProcessHeap(), 0, lpPerIOData );
					lpPerIOData = NULL;
				}
				//continue;
			}
			else if ( 0xFFFFFFFF == dwBytesTransferred )
			{
				//return 0;
				//continue;
			}
			else if ( ! IsValidRequest( lpPerIOData ) )
			{
				closesocket( sClient );
				if ( lpPerIOData )
				{
					HeapFree( GetProcessHeap(), 0, lpPerIOData );
					lpPerIOData = NULL;
				}
				//continue;
			}
			else if ( RECV_POSTED == lpPerIOData->OperationType )
			{
				memset( szResponse, 0, sizeof(szResponse) );

				//
				//	Process Message And Make Response
				//
				if ( m_stConfig.lpvResponseFunc )
				{
					m_stConfig.lpvResponseFunc( m_stConfig.lpvArglist, lpPerIOData->szMessage, dwBytesTransferred, szResponse, sizeof(szResponse) );
				}
				else
				{
					GetResponseDefault( m_stConfig.lpvArglist, lpPerIOData->szMessage, dwBytesTransferred, szResponse, sizeof(szResponse) );
				}

				//
				//	send reponse information
				//
				if ( strlen( szResponse ) )
				{
					send( sClient, szResponse, strlen(szResponse), 0 );
				}

				//
				//	Launch another asynchronous operation for sClient
				//
				memset( lpPerIOData, 0, sizeof(PER_IO_OPERATION_DATA) );
				lpPerIOData->stBuffer.len	= sizeof(lpPerIOData->szMessage);
				lpPerIOData->stBuffer.buf	= lpPerIOData->szMessage;
				lpPerIOData->OperationType	= RECV_POSTED;

				WSARecv
				(
					sClient,
					&lpPerIOData->stBuffer,
					1,
					&lpPerIOData->dwNumberOfBytesRecvd,
					&lpPerIOData->dwFlags,
					&lpPerIOData->overlap,
					NULL
				);
			}
		}
		__except( EXCEPTION_EXECUTE_HANDLER )
		{
		}
	}

	return 0;
}



//////////////////////////////////////////////////////////////////////////
//	Private


/**
 *	@ Private
 *	默认消息回应函数
 */
BOOL CDeSocketIoComp::GetResponseDefault( LPVOID lpvArglist, IN LPTSTR lpszMessage, IN DWORD dwMessageLength, OUT LPTSTR lpszResponse, IN DWORD dwSize )
{
	if ( NULL == lpszMessage )
	{
		return FALSE;
	}

	SYSTEMTIME st;

	GetLocalTime( &st );
	_sntprintf
	(
		lpszResponse, dwSize-sizeof(TCHAR),
		"%s(%04d-%02d-%02d %02d:%02d:%02d)",
		"I RECEIVE YOU MESSAGE, THANKS!",
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond
	);

	return TRUE;
}

/**
 *	@ Private
 *	检测是否是合法的请求
 */
BOOL CDeSocketIoComp::IsValidRequest( LPPER_IO_OPERATION_DATA lpPerIOData )
{
	if ( NULL == lpPerIOData )
	{
		return FALSE;
	}

	BOOL bValid = TRUE;

	if ( _tcslen( m_stConfig.szBindIpAddr ) )
	{
		if ( 0 != _tcsicmp( "0.0.0.0", inet_ntoa(lpPerIOData->saClient.sin_addr) ) && 
			0 != _tcsicmp( m_stConfig.szBindIpAddr, inet_ntoa(lpPerIOData->saClient.sin_addr) ) )
		{
			bValid = FALSE;
		}
	}

	if ( delib_is_debug() )
	{
		printf
		(
			"A New Request From: %s:%d, %s\n",
			inet_ntoa(lpPerIOData->saClient.sin_addr), ntohs(lpPerIOData->saClient.sin_port),
			( bValid ? "Accepted" : "#Denied#" )
		);
	}

	return bValid;
}