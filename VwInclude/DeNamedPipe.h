// DeNamedPipe.h: interface for the CDeNamedPipe class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DENAMEDPIPE_HEADER__
#define __DENAMEDPIPE_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>



#ifndef __DENAMEDPIPE_DEFINE__
#define __DENAMEDPIPE_DEFINE__


/**
 *	const define
 */
#define DENAMEDPIPE_BUFSIZE		4096
#define DENAMEDPIPE_MUTEX_NAMEPRE	"DeNamedPipe_"
#define DENAMEDPIPE_CLOSESERVER		"__delib_npp_close_server__"
#define DENAMEDPIPE_CLOSESERVER_CMDLEN	_tcslen( DENAMEDPIPE_CLOSESERVER )

/**
 *	return value
 */
#define DENAMEDPIPE_ERROR_SUCC				0
#define DENAMEDPIPE_ERROR_UNKNOWN			-1000
#define DENAMEDPIPE_ERROR_PARAM				-1001
#define DENAMEDPIPE_ERROR_CONNECT_PIPE			-1002
#define DENAMEDPIPE_ERROR_WRITE_FILE			-1003
#define DENAMEDPIPE_ERROR_SET_PIPE_HANDLE_STATE		-1004
#define DENAMEDPIPE_ERROR_CREATE_PIPE			-1005
#define DENAMEDPIPE_ERROR_CREATE_THREAD			-1006
#define DENAMEDPIPE_ERROR_ALREADY_EXISTS		-1007
#define DENAMEDPIPE_ERROR_OPEN_PIPE_FAILED		-1008
#define DENAMEDPIPE_ERROR_OPEN_PIPE_BUSY		-1009



/**
 *	callback Answer Processor
 *	eg: static BOOL CALLBACK GetAnswerToRequest( IN LPVOID lpvArglist, IN LPCTSTR lpcszCommand, OUT LPTSTR lpszReply, IN DWORD dwSize, OUT LPDWORD pdwBytes );
 */
typedef BOOL (CALLBACK * PFNDENAMEDPIPEANSWER)( IN LPVOID lpvArglist, IN LPCTSTR lpcszCommand, OUT LPTSTR lpszReply, IN DWORD dwSize, OUT LPDWORD pdwBytes );


#endif	// __DENAMEDPIPE_DEFINE__




/**
 *	struct
 */
typedef struct tagDeNamedPipeThread
{
	tagDeNamedPipeThread()
	{
		memset( this, 0, sizeof(tagDeNamedPipeThread) );
	}

	LPVOID lpvClassInstance;
	HANDLE hPipe;
	LPVOID lpvAnswerFunc;
	LPVOID lpvArglist;

}STDENAMEDPIPETHREAD,*PSTDENAMEDPIPETHREAD;



/**
 *	class of CDeNamedPipe
 */
class CDeNamedPipe
{
public:
	CDeNamedPipe();
	virtual ~CDeNamedPipe();

	//	创建新的管道服务
	LONG ServerCreateNew( IN LPCTSTR lpcszPipeName,
			      IN PFNDENAMEDPIPEANSWER lpvAnswerFunc,
			      IN LPCTSTR lpcszPassword,
			      IN DWORD dwMaxInstances = PIPE_UNLIMITED_INSTANCES,
			      IN DWORD dwTimeOut = NMPWAIT_USE_DEFAULT_WAIT,
			      IN LPVOID lpvArglist = NULL );

	//	客户端向服务器发送命令
	LONG ClientSendCommand( IN LPCTSTR lpcszPipeName, IN LPCTSTR lpcszCommand, OUT LPTSTR lpszReply, IN DWORD dwSize );

	//	检测该名称的管道服务是否存在
	LONG CreateNamedPipeMutex( IN LPCTSTR lpcszPipeName );
	VOID CloseNamedPipeMutex();

	//	获取关闭服务器的命令行
	BOOL GetCloseServerCommand( IN LPCTSTR lpcszPassword, OUT LPTSTR lpszCommand, IN DWORD dwSize );

private:
	//	管道服务的 worker
	static VOID _threadServerWorker( LPVOID lpvParam );
	VOID ServerWorkerProc( LPVOID lpvParam );
	BOOL ServerWorkerDefaultAnswerFunc( IN LPVOID lpvArglist, IN LPCTSTR lpcszCommand, OUT LPTSTR lpszReply, IN DWORD dwSize, OUT LPDWORD pdwBytes );

	VOID getFormatedPipeName( IN LPCTSTR lpcszPipeName, OUT LPTSTR lpszFmtPipeName, IN DWORD dwSize );
	
	BOOL IsValidPassword( IN LPCTSTR lpcszCommand );

private:
	HANDLE	m_hNamedPipeMutex;
	BOOL	m_bContinueWork;
	TCHAR	m_szPipeName[ MAX_PATH ];
	TCHAR	m_szPassword[ MAX_PATH ];

};

#endif // __DENAMEDPIPE_HEADER__
