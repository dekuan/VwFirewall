// VwNamedPipe.cpp: implementation of the CVwNamedPipe class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VwNamedPipe.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVwNamedPipe::CVwNamedPipe()
{

}

CVwNamedPipe::~CVwNamedPipe()
{

}

/**
 *	创建一个 NamedPipe 服务器
 */
LONG CVwNamedPipe::createServer( LPCTSTR lpcszPipeName, PFNVWNAMEDPIPEANSWER lpvAnswerFunc, LPTSTR lpszError )
{
	//
	//	lpszPipeName		- [in] 管道名字
	//	lpvThreadWorker		- [in] 处理线程
	//

	if ( NULL == lpcszPipeName || 0 == lpcszPipeName[0] )
	{
		return VWNAMEDPIPE_ERROR_PARAM;
	}
	if ( NULL == lpszError )
	{
		return VWNAMEDPIPE_ERROR_PARAM;
	}


	LONG  lnRet			= VWNAMEDPIPE_ERROR_UNKNOWN;
	TCHAR szMutexName[ MAX_PATH ]	= {0};
	HANDLE hMutex;

	BOOL fConnected			= FALSE;
	DWORD dwThreadId		= 0;
	HANDLE hPipe			= NULL;
	HANDLE hThread			= NULL;
	TCHAR szPipeName[ MAX_PATH ]	= {0};
	STVWNAMEDPIPETHREAD stThread;


	_sntprintf( szMutexName, sizeof(szMutexName)-sizeof(TCHAR), _T("%s%s"), _T(VWNAMEDPIPE_MUTEX_NAMEPRE), lpcszPipeName );
	hMutex = CreateMutex( NULL, TRUE, szMutexName );
	if ( GetLastError() == ERROR_ALREADY_EXISTS )
	{
		//	pipe named <lpcszPipeName> Already exist.
		_sntprintf( lpszError, MAX_PATH-sizeof(TCHAR), _T("pipe named <%s> Already exist"), lpcszPipeName );
		return VWNAMEDPIPE_ERROR_ALREADY_EXISTS;
	}

	//	get formated pipe name
	getFormatedPipeName( lpcszPipeName, szPipeName, sizeof(szPipeName) );

	// The main loop creates an instance of the named pipe and 
	// then waits for a client to connect to it. When the client 
	// connects, a thread is created to handle communications 
	// with that client, and the loop is repeated. 
	for ( ;; ) 
	{
		hPipe = CreateNamedPipe
		(
			szPipeName,			// pipe name
			PIPE_ACCESS_DUPLEX,		// read/write access
			PIPE_TYPE_MESSAGE |		// message type pipe
			PIPE_READMODE_MESSAGE |		// message-read mode
			PIPE_WAIT,			// blocking mode
			PIPE_UNLIMITED_INSTANCES,	// max. instances
			VWNAMEDPIPE_BUFSIZE,		// output buffer size
			VWNAMEDPIPE_BUFSIZE,		// input buffer size
			NMPWAIT_USE_DEFAULT_WAIT,	// client time-out
			NULL				// default security attribute
		);
		if ( INVALID_HANDLE_VALUE != hPipe )
		{
			//	Wait for the client to connect; if it succeeds,
			//	the function returns a nonzero value. If the function
			//	returns zero, GetLastError returns ERROR_PIPE_CONNECTED.
			fConnected = ConnectNamedPipe( hPipe, NULL ) ? TRUE : ( ERROR_PIPE_CONNECTED == GetLastError() );
			if ( fConnected )
			{
				//	Create a thread for this client.
				stThread.lpvClassInstance	= this;
				stThread.hPipe			= hPipe;
				stThread.lpvAnswerFunc		= lpvAnswerFunc;
				hThread = CreateThread
				(
					NULL,				// no security attribute
					0,				// default stack size
					(LPTHREAD_START_ROUTINE)_threadDefaultServerWorker,
					(LPVOID)(&stThread),		// thread parameter
					0,				// not suspended
					&dwThreadId			// returns thread ID
				);
				if ( hThread )
				{
					CloseHandle( hThread );
				}
				else
				{
					lnRet = VWNAMEDPIPE_ERROR_CREATE_THREAD;
					_tcscpy( lpszError, _T("CreateThread failed") );
				}
			}
			else
			{
				//	The client could not connect, so close the pipe.
				lnRet = VWNAMEDPIPE_ERROR_CONNECT_PIPE;
				_tcscpy( lpszError, _T("ConnectNamedPipe failed") );

				CloseHandle( hPipe );
			}
		}
		else
		{
			lnRet = VWNAMEDPIPE_ERROR_CREATE_PIPE;
			_tcscpy( lpszError, _T("CreatePipe failed") );
		}
	}
	return VWNAMEDPIPE_ERROR_SUCC;
}

VOID CVwNamedPipe::_threadDefaultServerWorker( LPVOID lpvParam )
{
	if ( NULL == lpvParam )
	{
		return;
	}
	
	//	..
	CVwNamedPipe * pThis;
	STVWNAMEDPIPETHREAD * pstThread;
	pstThread = (STVWNAMEDPIPETHREAD*)lpvParam;
	if ( pstThread )
	{
		pThis	= (CVwNamedPipe*)pstThread->lpvClassInstance;

		//	The thread's parameter ( pstThread->hPipe ) is a handle to a pipe instance.
		pThis->DefaultServerWorker( lpvParam );
	}

}

VOID CVwNamedPipe::DefaultServerWorker( LPVOID lpvParam )
{
	if ( NULL == lpvParam )
	{
		return;
	}

	STVWNAMEDPIPETHREAD stThread;
	TCHAR szRequest[ VWNAMEDPIPE_BUFSIZE ]		= {0};
	TCHAR szReply[ VWNAMEDPIPE_BUFSIZE ]		= {0}; 
	DWORD cbBytesRead, cbReplyBytes, cbWritten;
	BOOL bSuccess	= FALSE;
	HANDLE hPipe	= NULL;

	//	..
	stThread = *(STVWNAMEDPIPETHREAD*)lpvParam;
	hPipe = stThread.hPipe;

	//	..
	if ( hPipe )
	{
		while ( TRUE ) 
		{ 
			//	Read client requests from the pipe. 
			bSuccess = ReadFile
			( 
				hPipe,			// handle to pipe
				szRequest,		// buffer to receive data
				VWNAMEDPIPE_BUFSIZE*sizeof(TCHAR), // size of buffer
				&cbBytesRead,		// number of bytes read
				NULL			// not overlapped I/O
			);

			if ( bSuccess && cbBytesRead )
			{
				if ( stThread.lpvAnswerFunc )
				{
					( (PFNVWNAMEDPIPEANSWER)stThread.lpvAnswerFunc )( szRequest, szReply, sizeof(szReply), &cbReplyBytes );
				}
				else
				{
					getAnswerToRequest( szRequest, szReply, sizeof(szReply), &cbReplyBytes );
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
		
		//	Flush the pipe to allow the client to read the pipe's contents
		//	before disconnecting. Then disconnect the pipe, and close the
		//	handle to this pipe instance.
		FlushFileBuffers( hPipe );
		DisconnectNamedPipe( hPipe );
		CloseHandle( hPipe );
	}
}

BOOL CVwNamedPipe::getAnswerToRequest( LPTSTR lpszRequest, LPTSTR lpszReply, DWORD dwSize, LPDWORD pdwBytes )
{
	_sntprintf( lpszReply, dwSize-sizeof(TCHAR), _T("%s"), _T("Default answer from server") );
	*pdwBytes = ( _tcslen(lpszReply) + 1 )*sizeof(TCHAR);
	return TRUE;
}




/**
 *	client 获取返回值
 */
LONG CVwNamedPipe::getResponse( IN LPCTSTR lpcszPipeName, IN LPCTSTR lpszRequest, OUT LPTSTR lpszResponse, IN DWORD dwSize, OUT LPTSTR lpszError )
{
	if ( NULL == lpcszPipeName || NULL == lpszRequest || NULL == lpszError )
	{
		return VWNAMEDPIPE_ERROR_PARAM;
	}
	if ( 0 == lpcszPipeName[0] || 0 == lpszRequest[0] )
	{
		return VWNAMEDPIPE_ERROR_PARAM;
	}

	LONG  lnRet			= VWNAMEDPIPE_ERROR_UNKNOWN;
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
					_tcscpy( lpszError, _T("Could not open pipe, WaitNamedPipe failed") );
					break;
				}
			}
			else
			{
				//	Exit if an error other than ERROR_PIPE_BUSY occurs.
				_tcscpy( lpszError, _T("Could not open pipe") );
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
				lpszRequest,					// message
				( lstrlen(lpszRequest)+1 )*sizeof(TCHAR),	// message length
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
						lpszResponse,		//	buffer to receive reply
						dwSize,			//	VWNAMEDPIPE_BUFSIZE*sizeof(TCHAR),  // size of buffer
						&cbRead,		//	number of bytes read
						NULL			//	not overlapped
					);
					if ( ! bSuccess && ERROR_MORE_DATA != GetLastError() )
					{
						break;
					}

					//	looks szBuffer
					lnRet = VWNAMEDPIPE_ERROR_SUCC;
				}
				while ( ! bSuccess );	// repeat loop if ERROR_MORE_DATA
			}
			else
			{
				lnRet = VWNAMEDPIPE_ERROR_WRITE_FILE;
				_tcscpy( lpszError, _T("WriteFile failed") );
			}
		}
		else
		{
			lnRet = VWNAMEDPIPE_ERROR_SET_PIPE_HANDLE_STATE;
			_tcscpy( lpszError, _T("SetNamedPipeHandleState failed") );
		}

		//	...
		CloseHandle( hPipe );
	}
	else
	{
		lnRet = VWNAMEDPIPE_ERROR_CONNECT_PIPE;
	}

	return lnRet;
}


//////////////////////////////////////////////////////////////////////////
//	Private

VOID CVwNamedPipe::getFormatedPipeName( IN LPCTSTR lpcszPipeName, OUT LPTSTR lpszFmtPipeName, IN DWORD dwSize )
{
	_sntprintf( lpszFmtPipeName, dwSize-sizeof(TCHAR), _T("\\\\.\\pipe\\%s"), lpcszPipeName );
}