// DeSocketServer.h: interface for the CDeSocketServer class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DESOCKETSERVER_HEADER__
#define __DESOCKETSERVER_HEADER__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <WINSOCK2.H>

#define _WSPIAPI_COUNTOF
#include <ws2tcpip.h>

#include <mswsock.h>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#include "DeLib.h"


/**
 *	宏定义
 */
#define DESOCKETSERVER_ERROR_SUCC			0
#define DESOCKETSERVER_ERROR_UNKNOWN			-1000
#define DESOCKETSERVER_WSA_STARTUP			-1001
#define DESOCKETSERVER_CREATE_IO_COMPPORT		-1002
#define DESOCKETSERVER_CREATE_THREAD			-1003
#define DESOCKETSERVER_RESOLVE_ADDRESS			-1004
#define DESOCKETSERVER_SOCKET				-1005
#define DESOCKETSERVER_BIND				-1006
#define DESOCKETSERVER_WSA_IOCTL			-1007
#define DESOCKETSERVER_LISTEN				-1008
#define DESOCKETSERVER_HEAPALLOC			-1009
#define DESOCKETSERVER_WSA_WAIT_FOR_MULTIPLE_EVENTS	-1010
#define DESOCKETSERVER_GET_SOCK_OPT			-1011
#define DESOCKETSERVER_ALLOC_SOCKET_OBJ			-1012
#define DESOCKETSERVER_ALLOC_BUFFER_OBJ			-1013

#define DESOCKETSERVER_DEFAULT_PORT			5150	//	default socket port

#define DESOCKETSERVER_DEFAULT_BUFFER_SIZE		4096	//	default buffer size
#define DESOCKETSERVER_DEFAULT_OVERLAPPED_COUNT		5	//	Number of overlapped recv per socket
#define DESOCKETSERVER_MAX_COMPLETION_THREAD_COUNT	32	//	Maximum number of completion threads allowed

#define DESOCKETSERVER_OP_ACCEPT			0	//	AcceptEx
#define DESOCKETSERVER_OP_READ				1	//	WSARecv/WSARecvFrom
#define DESOCKETSERVER_OP_WRITE				2	//	WSASend/WSASendTo



//
//	This is our per I/O buffer. It contains a WSAOVERLAPPED structure as well
//	as other necessary information for handling an IO operation on a socket.
//
typedef struct tagDeBufferObj
{
	WSAOVERLAPPED ol;
	SOCKET sClient;		//	Used for AcceptEx client socket

	CHAR * pszBuf;		//	Buffer for recv/send/AcceptEx
	INT nBufLen;		//	Length of the buffer
	
	INT nOperation;		//	Type of operation issued

	SOCKADDR_STORAGE saAddr;
	INT nAddrLen;

	ULONG ulIoOrder;	//	Order in which this I/O was posted

	struct tagDeBufferObj * pstNext;

} STDEBUFFEROBJ, *PSTDEBUFFEROBJ;


//
//	This is our per socket buffer. It contains information about the socket handle
//	which is returned from each GetQueuedCompletionStatus call.
//
typedef struct tagDeSocketObj
{
	SOCKET s;				//	Socket handle

	INT nAf;				//	Address family of socket (AF_INET, AF_INET6)
	BOOL bClosing;				//	Is the socket closing?

	volatile LONG OutstandingOps;		//	Number of outstanding overlapped ops on socket

	STDEBUFFEROBJ ** PendingAccepts;	//	Pending AcceptEx buffers (used for listening sockets only)

	ULONG LastSendIssued;			//	Last sequence number sent
	ULONG IoCountIssued;			//	Next sequence number assigned to receives
	STDEBUFFEROBJ * OutOfOrderSends;	//	List of send buffers that completed out of order

	//	Pointers to Microsoft specific extensions. These are used by listening sockets only
	LPFN_ACCEPTEX lpfnAcceptEx;
	LPFN_GETACCEPTEXSOCKADDRS lpfnGetAcceptExSockaddrs;

	CRITICAL_SECTION critSecSocket;		//	Protect access to this structure

	struct tagDeSocketObj * pstNext;	//	next point

} STDESOCKETOBJ, *PSTDESOCKETOBJ;

//
//	Socket 启动配置信息
//
typedef struct tagDeSocketConfig
{
	tagDeSocketConfig()
	{
		memset( this, 0, sizeof(tagDeSocketConfig) );

		//	for socket
		_tcscpy( szBindAddr, _T("") );
		_sntprintf( szBindPort, sizeof(szBindPort)-sizeof(TCHAR), "%u", DESOCKETSERVER_DEFAULT_PORT );
		nAddressFamily	= AF_UNSPEC;
		nSocketType	= SOCK_STREAM;
		nProtocol	= IPPROTO_TCP;

		//	for others
		nBufferSize		= DESOCKETSERVER_DEFAULT_BUFFER_SIZE;
		nOverlappedCount	= DESOCKETSERVER_DEFAULT_OVERLAPPED_COUNT;
	}

	CHAR szBindAddr[ MAX_PATH ];	//	local interface to bind to
	CHAR szBindPort[ MAX_PATH ];	//	local port to bind to
	INT  nAddressFamily;		//	default to unspecified
	INT  nSocketType;		//	default to TCP socket type
	INT  nProtocol;			//	default to TCP protocol

	INT  nBufferSize;
	INT  nOverlappedCount;

}STDESOCKETCONFIG, *PSTDESOCKETCONFIG;

//
//	Statistics counters
//
static volatile LONG g_lnBytesRead	= 0;
static volatile LONG g_lnBytesSent	= 0;
static volatile LONG g_lnStartTime	= 0;
static volatile LONG g_lnBytesReadLast	= 0;
static volatile LONG g_lnBytesSentLast	= 0;
static volatile LONG g_lnStartTimeLast	= 0;


/**
 *	class of CDeSocketServer
 */
class CDeSocketServer
{
public:
	CDeSocketServer();
	virtual ~CDeSocketServer();

public:
	LONG SetSocketConfig( IN LPCTSTR lpcszBindAddr = NULL, IN UINT uPort = DESOCKETSERVER_DEFAULT_PORT, IN INT nAddressFamily = AF_UNSPEC, IN INT nSocketType = SOCK_STREAM, IN INT nProtocol = IPPROTO_TCP );
	LONG StartServer();

	static DWORD WINAPI _threadCompletionRoutine( LPVOID lpParam );
	DWORD CompletionRoutineProc( LPVOID lpParam );
	VOID  HandleIo( STDESOCKETOBJ * sock, STDEBUFFEROBJ * buf, HANDLE CompPort, DWORD BytesTransfered, DWORD error );

public:
	int  PostSend( STDESOCKETOBJ * sock, STDEBUFFEROBJ * sendobj );
	int  PostAccept( STDESOCKETOBJ *sock, STDEBUFFEROBJ * acceptobj );
	int  PostRecv( STDESOCKETOBJ * sock, STDEBUFFEROBJ * recvobj );
	void InsertPendingSend( STDESOCKETOBJ *sock, STDEBUFFEROBJ *send );
	int  DoSends( STDESOCKETOBJ * sock );
	void PrintStatistics();

private:
	DWORD getProcessorNumber();
	struct addrinfo * getLocalAddressList( char *addr, char *port, int af, int type, int proto );
	int  PrintAddress( SOCKADDR *sa, int salen );
	void dbgprint( CHAR * format, ... );

	STDESOCKETOBJ * AllocSocketObj( SOCKET s, int af );
	void FreeSocketObj( STDESOCKETOBJ * obj );
	
	STDEBUFFEROBJ * AllocBufferObj( STDESOCKETOBJ * sock, int buflen );
	void FreeBufferObj( STDEBUFFEROBJ * obj );
	

private:

	//	存储配置信息
	STDESOCKETCONFIG m_stSocketCfg;

};

static CDeSocketServer * g_cDeSocketServer = NULL;







#endif // __DESOCKETSERVER_HEADER__
