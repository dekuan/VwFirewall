// VwNamedPipe.h: interface for the CVwNamedPipe class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWNAMEDPIPE_HEADER__
#define __VWNAMEDPIPE_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>



#define VWNAMEDPIPE_BUFSIZE		4096
#define VWNAMEDPIPE_MUTEX_NAMEPRE	"VwNamedPipe_"

/**
*	return value
*/
#define VWNAMEDPIPE_ERROR_SUCC				0
#define VWNAMEDPIPE_ERROR_UNKNOWN			-1000
#define VWNAMEDPIPE_ERROR_PARAM				-1001
#define VWNAMEDPIPE_ERROR_CONNECT_PIPE			-1002
#define VWNAMEDPIPE_ERROR_WRITE_FILE			-1003
#define VWNAMEDPIPE_ERROR_SET_PIPE_HANDLE_STATE		-1004
#define VWNAMEDPIPE_ERROR_CREATE_PIPE			-1005
#define VWNAMEDPIPE_ERROR_CREATE_THREAD			-1006
#define VWNAMEDPIPE_ERROR_ALREADY_EXISTS		-1007


/**
 *	callback Answer Processor
 *	eg: static BOOL CALLBACK GetAnswerToRequest( LPTSTR lpszRequest, LPTSTR lpszReply, DWORD dwSize, LPDWORD pdwBytes );
 */
typedef BOOL (CALLBACK * PFNVWNAMEDPIPEANSWER)( IN LPTSTR lpszRequest, OUT LPTSTR lpszReply, IN DWORD dwSize, OUT LPDWORD pdwBytes );

typedef struct tagVwNamedPipeThread
{
	tagVwNamedPipeThread()
	{
		memset( this, 0, sizeof(tagVwNamedPipeThread) );
	}

	LPVOID lpvClassInstance;
	HANDLE hPipe;
	LPVOID lpvAnswerFunc;

}STVWNAMEDPIPETHREAD,*PSTVWNAMEDPIPETHREAD;



/**
 *	class of CVwNamedPipe
 */
class CVwNamedPipe
{
public:
	CVwNamedPipe();
	virtual ~CVwNamedPipe();

	LONG createServer( IN LPCTSTR lpcszPipeName, IN PFNVWNAMEDPIPEANSWER lpvAnswerFunc, OUT LPTSTR lpszError );
	static VOID _threadDefaultServerWorker( LPVOID lpvParam );
	VOID DefaultServerWorker( HANDLE hPipe );

	//	默认回应函数
	BOOL getAnswerToRequest( IN LPTSTR lpszRequest, OUT LPTSTR lpszReply, IN DWORD dwSize, OUT LPDWORD pdwBytes );

	LONG getResponse( IN LPCTSTR lpcszPipeName, IN LPCTSTR lpszRequest, OUT LPTSTR lpszResponse, IN DWORD dwSize, OUT LPTSTR lpszError );

private:
	VOID getFormatedPipeName( IN LPCTSTR lpcszPipeName, OUT LPTSTR lpszFmtPipeName, IN DWORD dwSize );
};

#endif // __VWNAMEDPIPE_HEADER__
