// DeSocketIoComp.h: interface for the CDeSocketIoComp class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DESOCKETIOCOMP_HEADER__
#define __DESOCKETIOCOMP_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#include <mswsock.h>


/**
 *	∫Í∂®“Â
 */
#define DESOCKETIOCOMP_ERROR_SUCC			0
#define DESOCKETIOCOMP_ERROR_UNKNOWN			-1000
#define DESOCKETIOCOMP_ERROR_PARAM			-1001
#define DESOCKETIOCOMP_SOCKETSRV_ALREADY_EXISTS		-1002

#define DESOCKETIOCOMP_WSA_STARTUP			-2000
#define DESOCKETIOCOMP_CREATE_IO_COMPPORT		-2001
#define DESOCKETIOCOMP_CREATE_THREAD			-2002
#define DESOCKETIOCOMP_RESOLVE_ADDRESS			-2003
#define DESOCKETIOCOMP_SOCKET				-2004
#define DESOCKETIOCOMP_BIND				-2005
#define DESOCKETIOCOMP_WSA_IOCTL			-2006
#define DESOCKETIOCOMP_LISTEN				-2007
#define DESOCKETIOCOMP_HEAPALLOC			-2008
#define DESOCKETIOCOMP_WSA_WAIT_FOR_MULTIPLE_EVENTS	-2009
#define DESOCKETIOCOMP_GET_SOCK_OPT			-2010

#define DESOCKETIOCOMP_MSGSIZE				4096

#define DESOCKETIOCOMP_MAX_COMPLETION_THREAD_COUNT	32	//	Maximum number of completion threads allowed


typedef enum
{
	RECV_POSTED
		
}DESOCKETIOCOMP_OPERATION_TYPE;

/**
 *	callback Message Processor
 *	eg: static BOOL CALLBACK GetResponseDefault( IN LPVOID lpvArglist, IN LPTSTR lpszMessage, OUT LPTSTR lpszResponse, IN DWORD dwSize );
 */
typedef BOOL (CALLBACK * PFNDESOCKETIOCOMPMSGPROCESSOR)( IN LPVOID lpvArglist, IN LPTSTR lpszMessage, IN DWORD dwMessageLength, OUT LPTSTR lpszResponse, IN DWORD dwSize );



//	per io operation data
typedef struct _PER_IO_OPERATION_DATA
{
	WSAOVERLAPPED overlap;
	SOCKADDR_IN saClient;
	WSABUF stBuffer;
	CHAR szMessage[ DESOCKETIOCOMP_MSGSIZE ];
	DWORD dwNumberOfBytesRecvd;
	DWORD dwFlags;
	DESOCKETIOCOMP_OPERATION_TYPE OperationType;

}PER_IO_OPERATION_DATA, * LPPER_IO_OPERATION_DATA;

//	config
typedef struct _DE_SOCKET_IO_COMP_CONFIG
{
	LPVOID lpvArglist;
	TCHAR  szBindIpAddr[ 32 ];
	UINT   uPort;
	INT    nProtocol;
	PFNDESOCKETIOCOMPMSGPROCESSOR lpvResponseFunc;

}DE_SOCKET_IO_COMP_CONFIG, * LPDE_SOCKET_IO_COMP_CONFIG;

//	for thread
typedef struct tagDeSocketIOCompThread
{
	tagDeSocketIOCompThread( LPVOID lpvThisParam, HANDLE hCompletionPortParam )
	{
		lpvThis		= lpvThisParam;
		hCompletionPort	= hCompletionPortParam;
	}
	LPVOID lpvThis;
	HANDLE hCompletionPort;

}DE_SOCKET_IO_COMP_THREAD, *LPDE_SOCKET_IO_COMP_THREAD;


/**
 *	class of CDeSocketIoComp
 */
class CDeSocketIoComp
{
public:
	CDeSocketIoComp();
	virtual ~CDeSocketIoComp();

public:
	LONG StartServer( LPCTSTR lpcszIpAddr, UINT uPort, INT nProtocol = IPPROTO_TCP, 
			PFNDESOCKETIOCOMPMSGPROCESSOR lpvResponseFunc = NULL,
			LPCTSTR lpcszPassword = NULL,
			LPVOID lpvArglist = NULL );
	LONG StopServer();
	LONG StopServer( LPCTSTR lpcszIpAddr, UINT uPort, INT nProtocol = IPPROTO_TCP );
	LONG SendMessage( LPCTSTR lpcszIpAddr, UINT uPort, INT nProtocol, LPCTSTR lpcszMessage, LPTSTR lpszResponse, DWORD dwSize );
	LONG SendMessageEx( LPCTSTR lpcszIpAddr, UINT uPort, INT nProtocol, LPCTSTR lpcszMessage, DWORD dwMsgLength, LPTSTR lpszResponse, DWORD dwSize );

	VOID SetPassword( LPCTSTR lpcszPassword );
	BOOL IsValidPassword( LPCTSTR lpcszPassword );

private:
	static unsigned __stdcall _threadWorker( LPVOID lpvCompletionPortID );
	DWORD WorkerProc( LPVOID lpvCompletionPortID );

	BOOL  GetResponseDefault( IN LPVOID lpvArglist, IN LPTSTR lpszMessage, IN DWORD dwMessageLength, OUT LPTSTR lpszResponse, IN DWORD dwSize );
	BOOL  IsValidRequest( LPPER_IO_OPERATION_DATA lpPerIOData );

private:
	BOOL m_bContinue;
	DE_SOCKET_IO_COMP_CONFIG m_stConfig;
	TCHAR m_szPassword[ MAX_PATH ];
};


#endif // __DESOCKETIOCOMP_HEADER__
