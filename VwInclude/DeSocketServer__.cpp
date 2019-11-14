// DeSocketServer.cpp: implementation of the CDeSocketServer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeSocketServer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeSocketServer::CDeSocketServer()
{
	g_cDeSocketServer	= this;
}
CDeSocketServer::~CDeSocketServer()
{
}

/**
 *	配置 Socket
 */
LONG CDeSocketServer::SetSocketConfig( IN LPCTSTR lpcszBindAddr, IN UINT uPort, IN INT nAddressFamily, IN INT nSocketType, IN INT nProtocol )
{
	LONG lnRet	= DESOCKETSERVER_ERROR_UNKNOWN;

	//	set config data
	_sntprintf( m_stSocketCfg.szBindAddr, sizeof(m_stSocketCfg.szBindAddr)-sizeof(TCHAR), "%s", lpcszBindAddr );
	_sntprintf( m_stSocketCfg.szBindPort, sizeof(m_stSocketCfg.szBindPort)-sizeof(TCHAR), "%d", uPort );
	m_stSocketCfg.nAddressFamily	= nAddressFamily;
	m_stSocketCfg.nSocketType	= nSocketType;
	m_stSocketCfg.nProtocol		= nProtocol;

	//	for others
	m_stSocketCfg.nBufferSize	= DESOCKETSERVER_DEFAULT_BUFFER_SIZE;
	m_stSocketCfg.nOverlappedCount	= DESOCKETSERVER_DEFAULT_OVERLAPPED_COUNT;

	return DESOCKETSERVER_ERROR_SUCC;
}


/**
 *	启动服务
 */
LONG CDeSocketServer::StartServer()
{
	LONG lnRet	= DESOCKETSERVER_ERROR_UNKNOWN;

	WSADATA	wsd;
	DWORD dwProcessorNumber;
	STDESOCKETOBJ * sockobj			= NULL;
	STDESOCKETOBJ * psoListenSockets	= NULL;
	HANDLE hCompletionPort;
	HANDLE ArrCompThreads[ DESOCKETSERVER_MAX_COMPLETION_THREAD_COUNT ];
	HANDLE hrc;
	int endpointcount=0;
	int nInterval		= 0;
	int rc;
	int i;
	struct addrinfo * res	= NULL;
	struct addrinfo * ptr	= NULL;


	//	Load Winsock
	if ( WSAStartup( MAKEWORD(2,2), &wsd ) != 0 )
	{
		//	unable to load Winsock!
		return DESOCKETSERVER_WSA_STARTUP;
	}

	//	Create the completion port used by this server
	hCompletionPort = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, (ULONG_PTR)NULL, 0 );
	if ( NULL == hCompletionPort )
	{
		//	CreateIoCompletionPort failed: %d\n", GetLastError()
		return DESOCKETSERVER_CREATE_IO_COMPPORT;
	}

	//	Find out how many processors are on this system
	dwProcessorNumber = getProcessorNumber();

	//
	//	Create the worker threads to service the completion notifications
	//
	for ( i = 0; i < (int)dwProcessorNumber; i++ )
	{
		ArrCompThreads[ i ] = CreateThread
			(
				NULL,
				0,
				_threadCompletionRoutine,
				(LPVOID)hCompletionPort,
				0,
				NULL
			);
		if ( NULL == ArrCompThreads[ i ] )
		{
			//	CreatThread failed: %d\n", GetLastError()
			return DESOCKETSERVER_CREATE_THREAD;
		}
	}

	//printf( "Local address: %s; Port: %s; Family: %d\n", g_pszBindAddr, g_pszBindPort, g_nAddressFamily );

	res = getLocalAddressList
	(
		m_stSocketCfg.szBindAddr,
		m_stSocketCfg.szBindPort,
		m_stSocketCfg.nAddressFamily,
		m_stSocketCfg.nSocketType,
		m_stSocketCfg.nProtocol
	);
	if ( NULL == res )
	{
		//	getLocalAddressList failed to return any addresses!
		return DESOCKETSERVER_RESOLVE_ADDRESS;
	}

	//
	//	For each local address returned, create a listening/receiving socket
	//
	ptr = res;
	while ( ptr )
	{
		//	PrintAddress(ptr->ai_addr, ptr->ai_addrlen); printf("\n");
		sockobj = AllocSocketObj( INVALID_SOCKET, ptr->ai_family );
		if ( NULL == sockobj )
		{
			return DESOCKETSERVER_ALLOC_SOCKET_OBJ;
		}

		//
		//	create the socket
		//
		sockobj->s = socket
		(
			ptr->ai_family,
			ptr->ai_socktype,
			ptr->ai_protocol
		);
		if ( INVALID_SOCKET == sockobj->s )
		{
			//	socket failed: %d\n", WSAGetLastError()
			return DESOCKETSERVER_SOCKET;
		}

		//
		//	Associate the socket and its STDESOCKETOBJ to the completion port
		//
		hrc = CreateIoCompletionPort( (HANDLE)sockobj->s, hCompletionPort, (ULONG_PTR)sockobj, 0 );
		if ( NULL == hrc )
		{
			//	CreateIoCompletionPort failed: %d\n", GetLastError()
			return DESOCKETSERVER_CREATE_IO_COMPPORT;
		}

		//
		//	bind the socket to a local address and port
		//
		rc = bind( sockobj->s, ptr->ai_addr, ptr->ai_addrlen );
		if ( SOCKET_ERROR == rc )
		{
			//	bind failed: %d\n", WSAGetLastError()
			return DESOCKETSERVER_BIND;
		}

		if ( IPPROTO_TCP == m_stSocketCfg.nProtocol )
		{
			//
			//	for TCP Protocol
			//

			STDEBUFFEROBJ * acceptobj		= NULL;
			GUID  guidAcceptEx		= WSAID_ACCEPTEX;
			GUID  guidGetAcceptExSockaddrs	= WSAID_GETACCEPTEXSOCKADDRS;
			DWORD bytes;

			//	Need to load the Winsock extension functions from each provider
			//	-- e.g. AF_INET and AF_INET6. 
			rc = WSAIoctl
			(
				sockobj->s,
				SIO_GET_EXTENSION_FUNCTION_POINTER,
				&guidAcceptEx,
				sizeof(guidAcceptEx),
				&sockobj->lpfnAcceptEx,
				sizeof(sockobj->lpfnAcceptEx),
				&bytes,
				NULL,
				NULL
			);
			if ( SOCKET_ERROR == rc )
			{
				//	WSAIoctl: SIO_GET_EXTENSION_FUNCTION_POINTER failed: %d\n", WSAGetLastError()
				return DESOCKETSERVER_WSA_IOCTL;
			}
			rc = WSAIoctl
			(
				sockobj->s,
				SIO_GET_EXTENSION_FUNCTION_POINTER,
				&guidGetAcceptExSockaddrs,
				sizeof(guidGetAcceptExSockaddrs),
				&sockobj->lpfnGetAcceptExSockaddrs,
				sizeof(sockobj->lpfnGetAcceptExSockaddrs),
				&bytes,
				NULL,
				NULL
			);
			if ( SOCKET_ERROR == rc )
			{
				//	WSAIoctl: SIO_GET_EXTENSION_FUNCTION_POINTER faled: %d\n", WSAGetLastError()
				return DESOCKETSERVER_WSA_IOCTL;
			}

			//
			//	For TCP sockets, we need to "listen" on them
			//
			rc = listen( sockobj->s, 100 );
			if ( SOCKET_ERROR == rc )
			{
				//	listen failed: %d\n", WSAGetLastError()
				return DESOCKETSERVER_LISTEN;
			}

			//	Keep track of the pending AcceptEx operations
			sockobj->PendingAccepts = (STDEBUFFEROBJ**)HeapAlloc
				(
					GetProcessHeap(), 
					HEAP_ZERO_MEMORY, 
					(sizeof(STDEBUFFEROBJ*) * m_stSocketCfg.nOverlappedCount )
				);
			if ( NULL == sockobj->PendingAccepts )
			{
				//	HeapAlloc failed: %d\n", GetLastError())
				//	ExitProcess(-1);
				return DESOCKETSERVER_HEAPALLOC;
			}

			//
			//	Post the AcceptEx(s)
			//
			for ( i = 0; i < m_stSocketCfg.nOverlappedCount; i++ )
			{
				sockobj->PendingAccepts[ i ] = acceptobj = AllocBufferObj( sockobj, m_stSocketCfg.nBufferSize );
				PostAccept( sockobj, acceptobj );
			}

			//
			//	Maintain a list of the listening socket structures
			//	向下传递
			//
			if ( NULL == psoListenSockets )
			{
				psoListenSockets	= sockobj;
			}
			else
			{
				sockobj->pstNext	= psoListenSockets;
				psoListenSockets	= sockobj;
			}
		}
		else
		{
			//
			//	for OTHER Protocol
			//

			STDEBUFFEROBJ * recvobj	= NULL;
			DWORD bytes;
			int optval;

			//	Turn off UDP errors resulting from ICMP messages (port/host unreachable, etc)
			optval = 0;
			rc = WSAIoctl
			(
				sockobj->s,
				SIO_UDP_CONNRESET,
				&optval,
				sizeof(optval),
				NULL,
				0,
				&bytes,
				NULL,
				NULL
			);
			if ( SOCKET_ERROR == rc )
			{
				//	WSAIoctl: SIO_UDP_CONNRESET failed: %d\n", WSAGetLastError()
			}
			
			//	For UDP, simply post some receives
			for ( i = 0; i < m_stSocketCfg.nOverlappedCount; i++ )
			{
				recvobj = AllocBufferObj( sockobj, m_stSocketCfg.nBufferSize );
				PostRecv( sockobj, recvobj );
			}
		}
		
		endpointcount ++;
		ptr = ptr->ai_next;
	}

	//	free the addrinfo structure for the 'bind' address
	freeaddrinfo( res );


	//	记录开始时间
	g_lnStartTime = g_lnStartTimeLast = GetTickCount();

	nInterval = 0;
	while ( TRUE )
	{
		rc = WSAWaitForMultipleEvents
		(
			dwProcessorNumber,
			ArrCompThreads,
			TRUE,
			5000,
			FALSE
		);
		if ( WAIT_FAILED == rc )
		{
			//	WSAWaitForMultipleEvents failed: %d\n", WSAGetLastError()
			break;
		}
		else if ( WAIT_TIMEOUT == rc )
		{
			nInterval ++;

			//	打印当前流量信息
			//PrintStatistics();

			if ( 12 == nInterval )
			{
				STDESOCKETOBJ * listenptr	= NULL;
				int optval;
				int optlen;

				//
				//	For TCP, cycle through all the outstanding AcceptEx operations
				//	to see if any of the client sockets have been connected but
				//	haven't received any data. If so, close them as they could be
				//	a denial of service attack.
				//
				listenptr = psoListenSockets;
				while ( listenptr )
				{
					for ( i = 0; i < m_stSocketCfg.nOverlappedCount; i++ )
					{
						optlen = sizeof(optval);
						rc = getsockopt
						(
							listenptr->PendingAccepts[ i ]->sClient,
							SOL_SOCKET,
							SO_CONNECT_TIME,
							(char*)&optval,
							&optlen
						);
						if ( SOCKET_ERROR == rc )
						{
							//	getsockopt: SO_CONNECT_TIME failed: %d\n", WSAGetLastError()
							return DESOCKETSERVER_GET_SOCK_OPT;
						}

						//
						//	If the socket has been connected for more than 5 minutes, close it.
						//	If closed, the AcceptEx call will fail in the completion thread.
						//	关闭连接超过 5 分钟的 socket
						//
						if ( 0xFFFFFFFF != optval && optval > 300 )
						{
							closesocket( listenptr->PendingAccepts[ i ]->sClient );
						}
					}

					listenptr = listenptr->pstNext;
				}

				nInterval = 0;
			}
		}
	}

	WSACleanup();

	return DESOCKETSERVER_ERROR_SUCC;
}



//
// Function: _threadCompletionRoutine
// 
// Description:
//    This is the completion thread which services our completion port. One of
//    these threads is created per processor on the system. The thread sits in 
//    an infinite loop calling GetQueuedCompletionStatus and handling socket
//    IO that completed.
//
DWORD WINAPI CDeSocketServer::_threadCompletionRoutine( LPVOID lpParam )
{
	if ( g_cDeSocketServer )
	{
		return g_cDeSocketServer->CompletionRoutineProc( lpParam );
	}
	return -1;
}
DWORD CDeSocketServer::CompletionRoutineProc( LPVOID lpParam )
{
	STDESOCKETOBJ * sockobj		= NULL;		//	Per socket object for completed I/O
	STDEBUFFEROBJ * bufobj		= NULL;		//	Per I/O object for completed I/O
	OVERLAPPED * lpOverlapped	= NULL;		//	Pointer to overlapped structure for completed I/O
	HANDLE hCompletionPort;				//	Completion port handle
	DWORD BytesTransfered;				//	Number of bytes transfered
	DWORD Flags;					//	Flags for completed I/O
	INT rc; 
	INT error;

	hCompletionPort = (HANDLE)lpParam;
	while ( TRUE )
	{
		error = NO_ERROR;
		rc = GetQueuedCompletionStatus
		(
			hCompletionPort,
			&BytesTransfered,
			(PULONG_PTR)&sockobj,
			&lpOverlapped,
			INFINITE
		);

		bufobj = CONTAINING_RECORD( lpOverlapped, STDEBUFFEROBJ, ol );
		if ( FALSE == rc )
		{
			// If the call fails, call WSAGetOverlappedResult to translate the
			//    error code into a Winsock error code.
			dbgprint( "_threadCompletionRoutine: GetQueuedCompletionStatus failed: %d\n", GetLastError() );
			rc = WSAGetOverlappedResult
			(
				sockobj->s,
				& bufobj->ol,
				& BytesTransfered,
				FALSE,
				& Flags
			);
			if ( FALSE == rc )
			{
				error = WSAGetLastError();
			}
		}

		//	Handle the IO operation
		HandleIo( sockobj, bufobj, hCompletionPort, BytesTransfered, error );
	}

	ExitThread( 0 );
	return 0;
}


//
//	Function: HandleIo
//
//	Description:
//		This function handles the IO on a socket. In the event of a receive, the 
//		completed receive is posted again. For completed accepts, another AcceptEx
//		is posted. For completed sends, the buffer is freed.
//
VOID CDeSocketServer::HandleIo( STDESOCKETOBJ * sock, STDEBUFFEROBJ * buf, HANDLE CompPort, DWORD BytesTransfered, DWORD error )
{
	//
	//	sock			- [in]
	//	buf			- [in]
	//	CompPort		- [in]
	//	BytesTransfered		- [in]
	//	error			- [in]
	//

	STDESOCKETOBJ * clientobj	= NULL;		//	New client object for accepted connections
	STDEBUFFEROBJ * recvobj		= NULL;		//	Used to post new receives on accepted connections
	STDEBUFFEROBJ * sendobj		= NULL;		//	Used to post new sends for data received
	BOOL bCleanupSocket;
	char * tmp;
	int i;

	if ( error != 0 )
	{
		dbgprint( "OP = %d; Error = %d\n", buf->nOperation, error );
	}

	bCleanupSocket = FALSE;

	if ( NO_ERROR != error && IPPROTO_TCP == m_stSocketCfg.nProtocol )
	{
		//
		//	An error occured on a TCP socket, free the associated per I/O buffer
		//	and see if there are any more outstanding operations. If so we must
		//	wait until they are complete as well.
		//
		FreeBufferObj( buf );

		if ( 0 == InterlockedDecrement( &sock->OutstandingOps ) )
		{
			dbgprint( "Freeing socket obj in GetOverlappedResult\n" );
			FreeSocketObj( sock );
		}
		return;
	}


	//	进入红灯区
	EnterCriticalSection( &sock->critSecSocket );

	if ( DESOCKETSERVER_OP_ACCEPT == buf->nOperation )
	{
		HANDLE hrc;
		SOCKADDR_STORAGE * LocalSockaddr	= NULL;
		SOCKADDR_STORAGE * RemoteSockaddr	= NULL;
		INT LocalSockaddrLen;
		INT RemoteSockaddrLen;

		//	Update counters
		InterlockedExchangeAdd( &g_lnBytesRead, BytesTransfered );
		InterlockedExchangeAdd( &g_lnBytesReadLast, BytesTransfered );

		//	Print the client's addresss
		sock->lpfnGetAcceptExSockaddrs
		(
			buf->pszBuf,
			buf->nBufLen - ( (sizeof(SOCKADDR_STORAGE) + 16 ) * 2 ),
			sizeof(SOCKADDR_STORAGE) + 16,
			sizeof(SOCKADDR_STORAGE) + 16,
			(SOCKADDR **)&LocalSockaddr,
			&LocalSockaddrLen,
			(SOCKADDR **)&RemoteSockaddr,
			&RemoteSockaddrLen
		);

		if ( delib_is_debug() )
		{
			printf( "Received connection from: " );
			PrintAddress((SOCKADDR *)RemoteSockaddr, RemoteSockaddrLen);
			printf("\n");
		}

		//	Get a new STDESOCKETOBJ for the client connection
		clientobj = AllocSocketObj( buf->sClient, sock->nAf );
		if ( clientobj )
		{
			// Associate the new connection to our completion port
			hrc = CreateIoCompletionPort
			(
				(HANDLE)buf->sClient,
				CompPort,
				(ULONG_PTR)clientobj,
				0
			);
			if ( hrc )
			{
				//	Get a STDEBUFFEROBJ to echo the data received with the accept back to the client
				sendobj = AllocBufferObj( clientobj, BytesTransfered );
				if ( sendobj )
				{
					//
					//	Copy the buffer to the sending object
					//
					memcpy( sendobj->pszBuf, buf->pszBuf, BytesTransfered );
					sendobj->pszBuf[ 0 ] = 'L';
					//_sntprintf( sendobj->pszBuf, BytesTransfered-sizeof(TCHAR), "I hear you:[%s]", buf->pszBuf );
					
					//	Post the send
					if ( NO_ERROR == PostSend( clientobj, sendobj ) )
					{
						//	Now post some receives on this new connection
						for ( i = 0; i < m_stSocketCfg.nOverlappedCount; i++ )
						{
							recvobj = AllocBufferObj( clientobj, m_stSocketCfg.nBufferSize );
							if ( recvobj )
							{
								if ( NO_ERROR != PostRecv( clientobj, recvobj )  )
								{
									//	If for some reason the send call fails, clean up the connection
									FreeBufferObj( recvobj );
									error = SOCKET_ERROR;
									break;
								}
							}
						}
					}
					else
					{
						// If for some reason the send call fails, clean up the connection
						FreeBufferObj(sendobj);
						error = SOCKET_ERROR;
					}
					
					//	Re-post the AcceptEx
					PostAccept( sock, buf );
					
					if ( NO_ERROR != error )
					{
						if ( 0 == clientobj->OutstandingOps )
						{
							closesocket( clientobj->s );
							clientobj->s = INVALID_SOCKET;
							FreeSocketObj( clientobj );
						}
						else
						{
							clientobj->bClosing = TRUE;
						}
						error = NO_ERROR;
					}
				}
				else
				{
					//	DESOCKETSERVER_ALLOC_BUFFER_OBJ
					//return;
					error = SOCKET_ERROR;
				}
			}
			else
			{
				//fprintf( stderr, "_threadCompletionRoutine: CreateIoCompletionPort failed: %d\n", GetLastError() );
				//return;
				error = SOCKET_ERROR;
			}
		}
		else
		{
			//DESOCKETSERVER_ALLOC_SOCKET_OBJ
			//return;
			error = SOCKET_ERROR;
		}
	}
	else if ( DESOCKETSERVER_OP_READ == buf->nOperation && NO_ERROR == error )
	{
		//
		// Receive completed successfully
		//
		if ( BytesTransfered > 0 || IPPROTO_UDP == m_stSocketCfg.nProtocol )
		{
			InterlockedExchangeAdd( &g_lnBytesRead, BytesTransfered );
			InterlockedExchangeAdd( &g_lnBytesReadLast, BytesTransfered );
			
			// Create a buffer to send
			sendobj = AllocBufferObj( sock, m_stSocketCfg.nBufferSize );
			if ( sendobj )
			{
				if ( IPPROTO_UDP == m_stSocketCfg.nProtocol )
				{
					memcpy( &sendobj->saAddr, &buf->saAddr, buf->nAddrLen );
				}
				
				// Swap the buffers (i.e. buffer we just received becomes the send buffer)
				tmp			= sendobj->pszBuf;
				sendobj->nBufLen	= BytesTransfered;
				sendobj->pszBuf		= buf->pszBuf;
				sendobj->ulIoOrder	= buf->ulIoOrder;

				buf->pszBuf	= tmp;
				buf->nBufLen	= m_stSocketCfg.nBufferSize;

				InsertPendingSend( sock, sendobj );

				if ( NO_ERROR != DoSends( sock ) )
				{
					error = SOCKET_ERROR;
				}
				else
				{
					// Post another receive
					if (PostRecv(sock, buf) != NO_ERROR)
					{
						// In the event the recv fails, clean up the connection
						FreeBufferObj(buf);
						error = SOCKET_ERROR;
					}
				}
			}
			else
			{
				//DESOCKETSERVER_ALLOC_BUFFER_OBJ
			}
		}
		else
		{
			dbgprint("Got 0 byte receive\n");
			
			// Graceful close - the receive returned 0 bytes read
			sock->bClosing = TRUE;
			
			// Free the receive buffer
			FreeBufferObj(buf);
			
			if (DoSends(sock) != NO_ERROR)
			{
				dbgprint("0: cleaning up in zero byte handler\n");
				error = SOCKET_ERROR;
			}
			
			// If this was the last outstanding operation on socket, clean it up
			if ((sock->OutstandingOps == 0) && (sock->OutOfOrderSends == NULL))
			{
				dbgprint("1: cleaning up in zero byte handler\n");
				bCleanupSocket = TRUE;
			}
		}
	}
	else if ( DESOCKETSERVER_OP_READ == buf->nOperation && NO_ERROR != error && IPPROTO_UDP == m_stSocketCfg.nProtocol )
	{
		// If for UDP, a receive completes with an error, we ignore it and re-post the recv
		if (PostRecv(sock, buf) != NO_ERROR)
		{
			error = SOCKET_ERROR;
		}
	}
	else if ( DESOCKETSERVER_OP_WRITE == buf->nOperation )
	{
		// Update the counters
		InterlockedExchangeAdd(&g_lnBytesSent, BytesTransfered);
		InterlockedExchangeAdd(&g_lnBytesSentLast, BytesTransfered);
		
		FreeBufferObj(buf);
		
		if (DoSends(sock) != NO_ERROR)
		{
			dbgprint("Cleaning up inside OP_WRITE handler\n");
			error = SOCKET_ERROR;
		}
	}

	//	
	//	...
	//
	if ( NO_ERROR != error )
	{
		sock->bClosing = TRUE;
	}
	
	//
	//	Check to see if socket is closing
	//
	if ( 0 == InterlockedDecrement( &sock->OutstandingOps ) && sock->bClosing && NULL == sock->OutOfOrderSends )
	{
		bCleanupSocket = TRUE;
	}
	else
	{
		if ( NO_ERROR != DoSends( sock ) )
		{
			bCleanupSocket = TRUE;
		}
	}

	//	离开红灯区
	LeaveCriticalSection( &sock->critSecSocket );


	//
	//	发现错误，清理掉这个 socket
	//
	if ( bCleanupSocket )
	{
		closesocket( sock->s );
		sock->s = INVALID_SOCKET;
		FreeSocketObj( sock );
	}

	return;
}


//
// Function: PostSend
// 
// Description:
//    Post an overlapped send operation on the socket.
//
int CDeSocketServer::PostSend( STDESOCKETOBJ * sock, STDEBUFFEROBJ * sendobj )
{
	WSABUF  wbuf;
	DWORD   bytes;
	int     rc;
	
	sendobj->nOperation = DESOCKETSERVER_OP_WRITE;
	
	wbuf.buf = sendobj->pszBuf;
	wbuf.len = sendobj->nBufLen;
	
	EnterCriticalSection( &sock->critSecSocket );
	
	// Incrmenting the last send issued and issuing the send should not be
	//    interuptable.
	sock->LastSendIssued++;
	
	if ( IPPROTO_TCP == m_stSocketCfg.nProtocol )
	{
		rc = WSASend
		(
			sock->s,
			&wbuf,
			1,
			&bytes,
			0,
			&sendobj->ol,
			NULL
		);
	}
	else
	{
		rc = WSASendTo
		(
			sock->s,
			&wbuf,
			1,
			&bytes,
			0,
			(SOCKADDR *)&sendobj->saAddr,
			sendobj->nAddrLen,
			&sendobj->ol,
			NULL
		);
	}
	
	LeaveCriticalSection( &sock->critSecSocket );
	
	if (rc == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			dbgprint("PostSend: WSASend* failed: %d\n", WSAGetLastError());
			return SOCKET_ERROR;
		}
	}
	
	// Increment the outstanding operation count
	InterlockedIncrement(&sock->OutstandingOps);
	
	return NO_ERROR;
}

//
// Function: PostAccept
// 
// Description:
//    Post an overlapped accept on a listening socket.
//
int CDeSocketServer::PostAccept( STDESOCKETOBJ *sock, STDEBUFFEROBJ * acceptobj )
{
	DWORD   bytes;
	int     rc;
	
	acceptobj->nOperation = DESOCKETSERVER_OP_ACCEPT;
	
	//
	//	Create the client socket for an incoming connection
	//
	acceptobj->sClient = socket
	(
		sock->nAf,
		SOCK_STREAM,
		IPPROTO_TCP
	);
	if ( acceptobj->sClient == INVALID_SOCKET)
	{
		fprintf(stderr, "PostAccept: socket failed: %d\n", WSAGetLastError());
		return -1;
	}
	
	rc = sock->lpfnAcceptEx(
		sock->s,
		acceptobj->sClient,
		acceptobj->pszBuf,
		acceptobj->nBufLen - ( (sizeof(SOCKADDR_STORAGE) + 16) * 2 ),
		sizeof(SOCKADDR_STORAGE) + 16,
		sizeof(SOCKADDR_STORAGE) + 16,
		&bytes,
		&acceptobj->ol
		);
	if (rc == FALSE)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			dbgprint("PostAccept: AcceptEx failed: %d\n",
				WSAGetLastError());
			return SOCKET_ERROR;
		}
	}
	
	// Increment the outstanding overlapped count for this socket
	InterlockedIncrement(&sock->OutstandingOps);
	
	return NO_ERROR;
}

//
// Function: PostRecv
// 
// Description: 
//    Post an overlapped receive operation on the socket.
//
int CDeSocketServer::PostRecv( STDESOCKETOBJ * sock, STDEBUFFEROBJ * recvobj )
{
	WSABUF  wbuf;
	DWORD   bytes,
		flags;
	int     rc;
	
	
	recvobj->nOperation = DESOCKETSERVER_OP_READ;
	
	wbuf.buf = recvobj->pszBuf;
	wbuf.len = recvobj->nBufLen;
	
	flags = 0;
	
	EnterCriticalSection( &sock->critSecSocket );
	
	// Assign the IO order to this receive. This must be performned within
	//    the critical section. The operation of assigning the IO count and posting
	//    the receive cannot be interupted.
	recvobj->ulIoOrder = sock->IoCountIssued;
	sock->IoCountIssued ++;
	
	if ( IPPROTO_TCP == m_stSocketCfg.nProtocol )
	{
		rc = WSARecv(
			sock->s,
			&wbuf,
			1,
			&bytes,
			&flags,
			&recvobj->ol,
			NULL
			);
	}
	else
	{
		rc = WSARecvFrom
		(
			sock->s,
			&wbuf,
			1,
			&bytes,
			&flags,
			(SOCKADDR *)&recvobj->saAddr,
			&recvobj->nAddrLen,
			&recvobj->ol,
			NULL
		);
	}
	
	LeaveCriticalSection( &sock->critSecSocket );
	
	if (rc == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			dbgprint("PostRecv: WSARecv* failed: %d\n", WSAGetLastError());
			return SOCKET_ERROR;
		}
	}
	
	// Increment outstanding overlapped operations
	InterlockedIncrement(&sock->OutstandingOps);
	
	return NO_ERROR;
}


//
// Function: InsertPendingSend
// 
// Description:
//    This routine inserts a send buffer object into the socket's list
//    of out of order sends. The routine DoSends will go through this
//    list to issue those sends that are in the correct order.
//
void CDeSocketServer::InsertPendingSend( STDESOCKETOBJ * sock, STDEBUFFEROBJ *send)
{
	STDEBUFFEROBJ *ptr=NULL,
		*prev=NULL;
	
	
	EnterCriticalSection( &sock->critSecSocket );
	
	send->pstNext = NULL;
	
	// This loop finds the place to put the send within the list.
	//    The send list is in the same order as the receives were
	//    posted.
	ptr = sock->OutOfOrderSends;
	while (ptr)
	{
		if ( send->ulIoOrder < ptr->ulIoOrder )
		{
			break;
		}
		
		prev = ptr;
		ptr = ptr->pstNext;
	}
	if (prev == NULL)
	{
		// Inserting at head
		sock->OutOfOrderSends = send;
		send->pstNext = ptr;
	}
	else
	{
		// Insertion somewhere in the middle
		prev->pstNext = send;
		send->pstNext = ptr;
	}
	
	LeaveCriticalSection( &sock->critSecSocket );
}

//
// Function: DoSends
//
// Description:
//    This routine goes through a socket object's list of out of order send
//    buffers and sends as many of them up to the current send count. For each
//    send posted, the LastSendIssued is incremented. This means that the next
//    buffer sent must have an IO sequence nubmer equal to the LastSendIssued.
//    This is to preserve the order of data echoed back.
//
int CDeSocketServer::DoSends( STDESOCKETOBJ * sock )
{
	STDEBUFFEROBJ *sendobj=NULL;
	int         ret;
	
	ret = NO_ERROR;
	
	EnterCriticalSection( &sock->critSecSocket );
	
	sendobj = sock->OutOfOrderSends;
	while ( sendobj && sendobj->ulIoOrder == sock->LastSendIssued )
	{
		if (PostSend(sock, sendobj) != NO_ERROR)
		{
			FreeBufferObj(sendobj);
			
			ret = SOCKET_ERROR;
			break;
		}
		sock->OutOfOrderSends = sendobj = sendobj->pstNext;
	}
	
	LeaveCriticalSection( &sock->critSecSocket );
	
	return ret;
}

//
// Function: PrintStatistics
//
// Description:
//    Print the send/recv statistics for the server
//
void CDeSocketServer::PrintStatistics()
{
	ULONG       bps, tick, elapsed;
	
	tick = GetTickCount();
	
	elapsed = ( tick - g_lnStartTime ) / 1000;
	
	if (elapsed == 0)
		return;
	
	printf("\n");
	
	// Calculate average bytes per second
	bps = g_lnBytesSent / elapsed;
	printf("Average BPS sent: %lu [%lu]\n", bps, g_lnBytesSent);
	
	bps = g_lnBytesRead / elapsed;
	printf("Average BPS read: %lu [%lu]\n", bps, g_lnBytesRead);
	
	elapsed = (tick - g_lnStartTimeLast) / 1000;
	
	if (elapsed == 0)
		return;
	
	// Calculate bytes per second over the last X seconds
	bps = g_lnBytesSentLast / elapsed;
	printf("Current BPS sent: %lu\n", bps);
	
	bps = g_lnBytesReadLast / elapsed;
	printf("Current BPS read: %lu\n", bps);
	
	InterlockedExchange(&g_lnBytesSentLast, 0);
	InterlockedExchange(&g_lnBytesReadLast, 0);
	
	g_lnStartTimeLast = tick;
}


//////////////////////////////////////////////////////////////////////////
//	Private
//



/**
 *	@ Private
 *	获取处理器个数
 */
DWORD CDeSocketServer::getProcessorNumber()
{
	SYSTEM_INFO sysinfo;
	GetSystemInfo( &sysinfo );
	return min( sysinfo.dwNumberOfProcessors, DESOCKETSERVER_MAX_COMPLETION_THREAD_COUNT );
}

//
//	Function: getLocalAddressList
//
//	Description:
//		This routine resolves the specified address and returns a list of addrinfo
//		structure containing SOCKADDR structures representing the resolved addresses.
//		Note that if 'addr' is non-NULL, then getaddrinfo will resolve it whether
//		it is a string listeral address or a hostname.
//
struct addrinfo * CDeSocketServer::getLocalAddressList( char * addr, char * port, int af, int type, int proto )
{
	struct addrinfo * pstNewAddr;
	struct addrinfo hints;
	int nGet;
	
	memset( & hints, 0, sizeof(hints) );
	hints.ai_flags		= ( addr ? 0 : AI_PASSIVE );
	hints.ai_family		= af;
	hints.ai_socktype	= type;
	hints.ai_protocol	= proto;
	
	//	printf( "Invalid address %s, getaddrinfo failed: %d\n", addr, rc );
	nGet = getaddrinfo( addr, port, &hints, &pstNewAddr );
	return ( 0 == nGet ? pstNewAddr : NULL );
}

//
//	Function: PrintAddress
//
//	Description:
//		This routine takes a SOCKADDR structure and its length and prints
//		converts it to a string representation. This string is printed
//		to the console via stdout.
//
int CDeSocketServer::PrintAddress(SOCKADDR *sa, int salen)
{
	char    host[NI_MAXHOST],
		serv[NI_MAXSERV];
	int     hostlen = NI_MAXHOST,
		servlen = NI_MAXSERV,
		rc;
	
	rc = getnameinfo
	(
		sa,
		salen,
		host,
		hostlen,
		serv,
		servlen,
		NI_NUMERICHOST | NI_NUMERICSERV
	);
	if (rc != 0)
	{
		fprintf(stderr, "%s: getnameinfo failed: %d\n", __FILE__, rc);
		return rc;
	}
	
	// If the port is zero then don't print it
	if (strcmp(serv, "0") != 0)
	{
		if (sa->sa_family == AF_INET)
			printf("[%s]:%s", host, serv);
		else
			printf("%s:%s", host, serv);
	}
	else
		printf("%s", host);
	
	return NO_ERROR;
}

VOID CDeSocketServer::dbgprint( CHAR * format, ... )
{
	if ( ! delib_is_debug() )
	{
		return;
	}

	va_list vl;
	CHAR dbgbuf[2048];
	DWORD pid;

	pid = GetCurrentProcessId();

	va_start( vl, format );
	wvsprintf( dbgbuf, format, vl );
	va_end( vl );
	
	OutputDebugString( dbgbuf );
}

//
//	Function: AllocSocketObj
//
//	Description:
//		Allocate a socket object and initialize its members. A socket object is
//		allocated for each socket created (either by socket or accept).
//		Again, a lookaside list can be implemented to cache freed STDESOCKETOBJ to
//		improve performance.
//
STDESOCKETOBJ * CDeSocketServer::AllocSocketObj( SOCKET s, int af )
{
	STDESOCKETOBJ * pstNew	= NULL;

	pstNew = (STDESOCKETOBJ*)HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(STDESOCKETOBJ) );
	if ( pstNew )
	{
		//	Initialize the members
		pstNew->s = s;
		pstNew->nAf = af;

		//	For TCP we initialize the IO count to one since the AcceptEx is posted
		//	to receive data
		pstNew->IoCountIssued = ( IPPROTO_TCP == m_stSocketCfg.nProtocol ? 1 : 0 );

		InitializeCriticalSection( &pstNew->critSecSocket );
	}
	else
	{
		//fprintf(stderr, "AllocSocketObj: HeapAlloc failed: %d\n", GetLastError());
		//ExitProcess(-1);
	}

	return pstNew;
}

//
//	Function: FreeSocketObj
//
//	Description:
//		Frees a socket object. If there are outstanding operations, the object is not freed. 
//
void CDeSocketServer::FreeSocketObj( STDESOCKETOBJ * obj )
{
	STDEBUFFEROBJ  *ptr=NULL,
                *tmp=NULL;
	
	if (obj->OutstandingOps != 0)
	{
		// Still outstanding operations so just return
		return;
	}
	// Close the socket if it hasn't already been closed
	if (obj->s != INVALID_SOCKET)
	{
		closesocket(obj->s);
		obj->s = INVALID_SOCKET;
	}
	
	DeleteCriticalSection( &obj->critSecSocket );
	
	HeapFree(GetProcessHeap(), 0, obj);
}

//
//	Function: AllocBufferObj
// 
//	Description:
//		Allocate a STDEBUFFEROBJ. Each receive posted allocates one of these. 
//		After the recv is successful, the STDEBUFFEROBJ is queued for
//		sending by the send thread. To increase performance, a lookaside lists 
//		should be used to cache free STDEBUFFEROBJ.
//
STDEBUFFEROBJ * CDeSocketServer::AllocBufferObj( STDESOCKETOBJ * sock, int buflen )
{
	STDEBUFFEROBJ * pstNew	= NULL;

	//	Allocate the object
	pstNew = (STDEBUFFEROBJ*)HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(STDEBUFFEROBJ) );
	if ( pstNew )
	{
		//	Allocate the buffer
		pstNew->pszBuf = (char*)HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(BYTE)*buflen );
		if ( pstNew->pszBuf )
		{
			pstNew->nBufLen		= buflen;	
			pstNew->nAddrLen	= sizeof(pstNew->saAddr);
		}
		else
		{
			//	AllocBufferObj: HeapAlloc failed: %d\n", GetLastError()
			//ExitProcess(-1);
		}
	}
	else
	{
		//	AllocBufferObj: HeapAlloc failed: %d\n", GetLastError()
		//ExitProcess(-1);
	}

	return pstNew;
}

//
//	Function: FreeBufferObj
// 
//	Description:
//		Free the buffer object. To increase performance, a lookaside list should be
//		implemented to cache STDEBUFFEROBJ when freed.
//
void CDeSocketServer::FreeBufferObj( STDEBUFFEROBJ * obj )
{
	HeapFree( GetProcessHeap(), 0, obj->pszBuf );
	HeapFree( GetProcessHeap(), 0, obj );
}