// VwNamedPipe.cpp: implementation of the CDeNamedPipe class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeNamedPipe.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeNamedPipe::CDeNamedPipe()
{
	m_hNamedPipeMutex	= NULL;
	m_bContinueWork		= TRUE;

	memset( m_szPipeName, 0, sizeof(m_szPipeName) );
	memset( m_szPassword, 0, sizeof(m_szPassword) );
}
CDeNamedPipe::~CDeNamedPipe()
{
	CloseNamedPipeMutex();
}

/**
 *	@ public
 *	Server 创建新的管道服务
 */
LONG CDeNamedPipe::ServerCreateNew( IN LPCTSTR lpcszPipeName,
				    IN PFNDENAMEDPIPEANSWER lpvAnswerFunc,
				    IN LPCTSTR lpcszPassword,
				    IN DWORD dwMaxInstances /* = PIPE_UNLIMITED_INSTANCES */,
				    IN DWORD dwTimeOut /* = NMPWAIT_USE_DEFAULT_WAIT */,
				    IN LPVOID lpvArglist /* = NULL */ )
{
	//
	//	lpszPipeName		- [in] 管道名字
	//	lpvThreadWorker		- [in] 处理线程
	//	lpcszPassword		- [in] 关闭管道服务时候需要的密码
	//	dwMaxInstances		- [in] 允许最多创建多少个管道服务实例
	//	dwTimeOut		- [in] 超时
	//	RETURN			- return DENAMEDPIPE_ERROR_SUCC if successfully. otherwise ...
	//

	if ( NULL == lpcszPipeName || 0 == lpcszPipeName[0] )
	{
		return DENAMEDPIPE_ERROR_PARAM;
	}

	LONG  lnRet			= DENAMEDPIPE_ERROR_UNKNOWN;
	BOOL  bConnected		= FALSE;
	DWORD dwThreadId		= 0;
	HANDLE hPipe			= NULL;
	HANDLE hThread			= NULL;
	TCHAR szPipeName[ MAX_PATH ]	= {0};
	STDENAMEDPIPETHREAD stThread;


	//
	//	检测管道是否存在
	//
//	if ( DENAMEDPIPE_ERROR_ALREADY_EXISTS == CreateNamedPipeMutex( lpcszPipeName ) )
//	{
//		//	pipe named <lpcszPipeName> Already exist.
//		return DENAMEDPIPE_ERROR_ALREADY_EXISTS;
//	}

	//
	//	保存到管道名称
	//
	_sntprintf( m_szPipeName, sizeof(m_szPipeName)-sizeof(TCHAR), _T("%s"), lpcszPipeName );
	if ( lpcszPassword )
	{
		_sntprintf( m_szPassword, sizeof(m_szPassword)-sizeof(TCHAR), _T("%s"), m_szPassword );
	}

	//
	//	get formated pipe name
	//
	getFormatedPipeName( lpcszPipeName, szPipeName, sizeof(szPipeName) );

	//
	//	The main loop creates an instance of the named pipe and 
	//	then waits for a client to connect to it. When the client 
	//	connects, a thread is created to handle communications 
	//	with that client, and the loop is repeated. 
	//
	for ( ;; ) 
	{
		if ( ! m_bContinueWork )
		{
			//
			//	客户端发出关闭管道服务的命令
			//	关闭服务，关闭自己的进程，结束！
			//
			break;
		}

		__try
		{
			hPipe = CreateNamedPipe
			(
				szPipeName,				//	pipe name
				PIPE_ACCESS_DUPLEX,			//	read/write access
				PIPE_TYPE_MESSAGE |			//	message type pipe
					PIPE_READMODE_MESSAGE |		//	message-read mode
					PIPE_WAIT,			//	blocking mode
				dwMaxInstances,				//	PIPE_UNLIMITED_INSTANCES, //	max. instances
				DENAMEDPIPE_BUFSIZE,			//	output buffer size
				DENAMEDPIPE_BUFSIZE,			//	input buffer size
				dwTimeOut,				//	NMPWAIT_USE_DEFAULT_WAIT, //	client time-out
				NULL					//	default security attribute
			);
			if ( INVALID_HANDLE_VALUE != hPipe )
			{
				//
				//	阻塞等待
				//	Wait for the client to connect; if it succeeds,
				//	the function returns a nonzero value. If the function
				//	returns zero, GetLastError returns ERROR_PIPE_CONNECTED.
				//
				bConnected = ConnectNamedPipe( hPipe, NULL ) ? TRUE : ( ERROR_PIPE_CONNECTED == GetLastError() );
				if ( bConnected )
				{
					//	Create a thread for this client.
					stThread.lpvClassInstance	= this;
					stThread.hPipe			= hPipe;
					stThread.lpvAnswerFunc		= lpvAnswerFunc;
					stThread.lpvArglist		= lpvArglist;
					hThread = CreateThread
					(
						NULL,				//	no security attribute
						0,				//	default stack size
						(LPTHREAD_START_ROUTINE)_threadServerWorker,
						(LPVOID)(&stThread),		//	thread parameter
						0,				//	not suspended
						&dwThreadId			//	returns thread ID
					);
					if ( hThread )
					{
						//
						//	管道工作线程创建成功，句柄由工作线程负责关闭
						//
						lnRet = DENAMEDPIPE_ERROR_SUCC;
						CloseHandle( hThread );
					}
					else
					{
						lnRet = DENAMEDPIPE_ERROR_CREATE_THREAD;
						//_tcscpy( lpszError, _T("CreateThread failed") );
					}
				}
				else
				{
					//	The client could not connect, so close the pipe.
					lnRet = DENAMEDPIPE_ERROR_CONNECT_PIPE;
					//_tcscpy( lpszError, _T("ConnectNamedPipe failed") );

					CloseHandle( hPipe );
				}
			}
			else
			{
				lnRet = DENAMEDPIPE_ERROR_CREATE_PIPE;
				//_tcscpy( lpszError, _T("CreatePipe failed") );
			}
		}
		__except( EXCEPTION_EXECUTE_HANDLER )
		{
		}
	}

	return lnRet;
}


/**
 *	@ public
 *	client 客户端向服务器发送命令
 */
LONG CDeNamedPipe::ClientSendCommand( IN LPCTSTR lpcszPipeName, IN LPCTSTR lpcszCommand, OUT LPTSTR lpszReply, IN DWORD dwSize )
{
	if ( NULL == lpcszPipeName || NULL == lpcszCommand )
	{
		return DENAMEDPIPE_ERROR_PARAM;
	}
	if ( 0 == lpcszPipeName[0] || 0 == lpcszCommand[0] )
	{
		return DENAMEDPIPE_ERROR_PARAM;
	}

	LONG  lnRet			= DENAMEDPIPE_ERROR_UNKNOWN;
	TCHAR szPipeName[ MAX_PATH ]	= {0};
	HANDLE hPipe	= NULL;
	BOOL  bSuccess		= FALSE;
	BOOL  bConnected	= FALSE;
	DWORD cbRead, cbWritten, dwMode;

	//	组成 pipe 的完整名字
	getFormatedPipeName( lpcszPipeName, szPipeName, sizeof(szPipeName) );

	//	Try to open a named pipe; wait for it, if necessary.	
	while ( TRUE )
	{
		bConnected = FALSE;
		hPipe = CreateFile
		(
			szPipeName,			// pipe name
			GENERIC_READ|GENERIC_WRITE,	// read and write access
			0,				// no sharing
			NULL,				// default security attributes
			OPEN_EXISTING,			// opens existing pipe
			0,				// default attributes
			NULL				// no template file
		);

		if ( INVALID_HANDLE_VALUE != hPipe )
		{
			//	the pipe handle is valid
			bConnected = TRUE;
			break;
		}
		else
		{
			//	Exit if an error other than ERROR_PIPE_BUSY occurs.
			if ( ERROR_PIPE_BUSY == GetLastError() )
			{
				//	All pipe instances are busy, so wait for 20 seconds.
				if ( WaitNamedPipe( szPipeName, 20000 ) )
				{
					//	continue loop ...
				}
				else
				{
					lnRet = DENAMEDPIPE_ERROR_OPEN_PIPE_FAILED;
					//_tcscpy( lpszError, _T("Could not open pipe, WaitNamedPipe failed") );
					break;
				}
			}
			else
			{
				//	Exit if an error other than ERROR_PIPE_BUSY occurs.
				lnRet = DENAMEDPIPE_ERROR_OPEN_PIPE_BUSY;
				//_tcscpy( lpszError, _T("Could not open pipe") );
				break;
			}
		}
	}

	if ( bConnected )
	{
		//	The pipe connected; change to message-read mode.
		dwMode = PIPE_READMODE_MESSAGE;
		bSuccess = SetNamedPipeHandleState
		(
			hPipe,		// pipe handle
			&dwMode,	// new pipe mode
			NULL,		// don't set maximum bytes
			NULL		// don't set maximum time
		);
		if ( bSuccess )
		{
			//	Send a message to the pipe server.
			bSuccess = WriteFile
			(
				hPipe,						// pipe handle
				lpcszCommand,					// message
				( lstrlen(lpcszCommand)+1 )*sizeof(TCHAR),	// message length
				&cbWritten,					// bytes written
				NULL						// not overlapped
			);
			if ( bSuccess )
			{
				do
				{
					//	Read from the pipe.
					bSuccess = ReadFile
					(
						hPipe,			//	pipe handle
						lpszReply,		//	buffer to receive reply
						dwSize,			//	DENAMEDPIPE_BUFSIZE*sizeof(TCHAR),  // size of buffer
						&cbRead,		//	number of bytes read
						NULL			//	not overlapped
					);
					if ( ! bSuccess && ERROR_MORE_DATA != GetLastError() )
					{
						break;
					}

					//	looks szBuffer
					lnRet = DENAMEDPIPE_ERROR_SUCC;
				}
				while ( ! bSuccess );	// repeat loop if ERROR_MORE_DATA
			}
			else
			{
				lnRet = DENAMEDPIPE_ERROR_WRITE_FILE;
				//_tcscpy( lpszError, _T("WriteFile failed") );
			}
		}
		else
		{
			lnRet = DENAMEDPIPE_ERROR_SET_PIPE_HANDLE_STATE;
			//_tcscpy( lpszError, _T("SetNamedPipeHandleState failed") );
		}

		//	...
		CloseHandle( hPipe );
	}
	else
	{
		lnRet = DENAMEDPIPE_ERROR_CONNECT_PIPE;
	}

	return lnRet;
}


/**
 *	@ public
 *	创建管道 Mutex
 */
LONG CDeNamedPipe::CreateNamedPipeMutex( IN LPCTSTR lpcszPipeName )
{
	//
	//	lpcszPipeName	- [in] 管道名称
	//	RETURN		- 如果管道不存在则返回 DENAMEDPIPE_ERROR_SUCC
	//			  否则，返回 DENAMEDPIPE_ERROR_ALREADY_EXISTS
	//

	if ( NULL == lpcszPipeName || 0 == _tcslen(lpcszPipeName) )
	{
		return DENAMEDPIPE_ERROR_SUCC;
	}

	LONG  lnRet			= DENAMEDPIPE_ERROR_SUCC;
	TCHAR szMutexName[ MAX_PATH ]	= {0};

	_sntprintf( szMutexName, sizeof(szMutexName)-sizeof(TCHAR), _T("%s%s"), _T(DENAMEDPIPE_MUTEX_NAMEPRE), lpcszPipeName );
	m_hNamedPipeMutex = CreateMutex( NULL, TRUE, szMutexName );
	if ( GetLastError() == ERROR_ALREADY_EXISTS )
	{
		lnRet = DENAMEDPIPE_ERROR_ALREADY_EXISTS;
	}

	return lnRet;
}

/**
 *	@ public
 *	关闭管道 Mutex
 */
VOID CDeNamedPipe::CloseNamedPipeMutex()
{
	OutputDebugString( "CDeNamedPipe::CloseNamedPipeMutex() enter...;" );

	if ( m_hNamedPipeMutex )
	{
		ReleaseMutex( m_hNamedPipeMutex );
		CloseHandle( m_hNamedPipeMutex );

		m_hNamedPipeMutex = NULL;

		OutputDebugString( "CDeNamedPipe::CloseNamedPipeMutex();" );
	}
}


/**
 *	@ public
 *	获取关闭服务器的命令行
 */
BOOL CDeNamedPipe::GetCloseServerCommand( IN LPCTSTR lpcszPassword, OUT LPTSTR lpszCommand, IN DWORD dwSize )
{
	if ( NULL == lpcszPassword || NULL == lpszCommand || 0 == dwSize )
	{
		return FALSE;
	}

	_sntprintf( lpszCommand, dwSize-sizeof(TCHAR), _T("%s|%s"), DENAMEDPIPE_CLOSESERVER, lpcszPassword );
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//	Private


/**
 *	@ private
 *	管道服务器的 worker
 */
VOID CDeNamedPipe::_threadServerWorker( LPVOID lpvParam )
{
	if ( NULL == lpvParam )
	{
		return;
	}

	CDeNamedPipe * pThis;
	STDENAMEDPIPETHREAD * pstThread;

	__try
	{
		pstThread = (STDENAMEDPIPETHREAD*)lpvParam;
		if ( pstThread )
		{
			pThis	= (CDeNamedPipe*)pstThread->lpvClassInstance;
			if ( pThis )
			{
				//	The thread's parameter ( pstThread->hPipe ) is a handle to a pipe instance.
				pThis->ServerWorkerProc( lpvParam );
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}
VOID CDeNamedPipe::ServerWorkerProc( LPVOID lpvParam )
{
	if ( NULL == lpvParam )
	{
		return;
	}

	STDENAMEDPIPETHREAD stThread;
	TCHAR szReceive[ DENAMEDPIPE_BUFSIZE ]		= {0};
	TCHAR szReply[ DENAMEDPIPE_BUFSIZE ]		= {0}; 
	DWORD cbBytesRead, cbReplyBytes, cbWritten;
	BOOL bSuccess	= FALSE;
	HANDLE hPipe	= NULL;


	//
	//	拷贝参数
	//
	stThread = *(STDENAMEDPIPETHREAD*)lpvParam;
	hPipe = stThread.hPipe;

	//
	//	开始工作
	//
	__try
	{
		if ( hPipe )
		{
			while ( TRUE ) 
			{ 
				//	Read client requests from the pipe. 
				bSuccess = ReadFile
				( 
					hPipe,			// handle to pipe
					szReceive,		// buffer to receive data
					DENAMEDPIPE_BUFSIZE*sizeof(TCHAR), // size of buffer
					&cbBytesRead,		// number of bytes read
					NULL			// not overlapped I/O
				);

				if ( bSuccess && cbBytesRead )
				{
					if ( 0 == memcmp( szReceive, DENAMEDPIPE_CLOSESERVER, DENAMEDPIPE_CLOSESERVER_CMDLEN ) )
					{
						//	客户端发出关闭服务的命令
						m_bContinueWork = FALSE;
						break;
					}

					if ( stThread.lpvAnswerFunc )
					{
						( (PFNDENAMEDPIPEANSWER)stThread.lpvAnswerFunc )( stThread.lpvArglist, szReceive, szReply, sizeof(szReply), &cbReplyBytes );
					}
					else
					{
						ServerWorkerDefaultAnswerFunc( stThread.lpvArglist, szReceive, szReply, sizeof(szReply), &cbReplyBytes );
					}

					//	Write the reply to the pipe. 
					bSuccess = WriteFile
					(
						hPipe,			// handle to pipe
						szReply,		// buffer to write from
						cbReplyBytes,		// number of bytes to write
						&cbWritten,		// number of bytes written
						NULL			// not overlapped I/O
					);

					if ( ! bSuccess || cbReplyBytes != cbWritten )
					{
						break;
					}
				}
				else
				{
					break;
				}
			} 
			
			//
			//	Flush the pipe to allow the client to read the pipe's contents
			//	before disconnecting. Then disconnect the pipe, and close the
			//	handle to this pipe instance.
			//
			FlushFileBuffers( hPipe );
			DisconnectNamedPipe( hPipe );
			CloseHandle( hPipe );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}

/**
 *	@ private
 *
 */
BOOL CDeNamedPipe::ServerWorkerDefaultAnswerFunc( IN LPVOID lpvArglist, IN LPCTSTR lpcszCommand, OUT LPTSTR lpszReply, IN DWORD dwSize, OUT LPDWORD pdwBytes )
{
	__try
	{
		_sntprintf( lpszReply, dwSize-sizeof(TCHAR), _T("DEFAULT ANSWER FROM NAMEDPIPE SERVER[%s]"), m_szPipeName );
		*pdwBytes = ( _tcslen(lpszReply) + 1 )*sizeof(TCHAR);
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return TRUE;
}

VOID CDeNamedPipe::getFormatedPipeName( IN LPCTSTR lpcszPipeName, OUT LPTSTR lpszFmtPipeName, IN DWORD dwSize )
{
	_sntprintf( lpszFmtPipeName, dwSize-sizeof(TCHAR), _T("\\\\.\\pipe\\%s"), lpcszPipeName );
}

BOOL CDeNamedPipe::IsValidPassword( IN LPCTSTR lpcszCommand )
{
	if ( NULL == lpcszCommand )
	{
		return FALSE;
	}

	BOOL bRet	= FALSE;
	UINT uLen;
	TCHAR szPwd[ MAX_PATH ];

	__try
	{
		uLen = _tcslen(lpcszCommand);
		if ( uLen >= ( DENAMEDPIPE_CLOSESERVER_CMDLEN + sizeof(TCHAR) ) )
		{
			//	命令行大概是：
			//	DENAMEDPIPE_CLOSESERVER_CMDLEN + | + password
			memset( szPwd, 0, sizeof(szPwd) );
			_sntprintf( szPwd, sizeof(szPwd)-sizeof(TCHAR), _T("%s"), lpcszCommand + DENAMEDPIPE_CLOSESERVER_CMDLEN + sizeof(TCHAR) );
			if ( 0 == _tcsicmp( szPwd, m_szPassword ) )
			{
				bRet = TRUE;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return bRet;
}