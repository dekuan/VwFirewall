#ifndef __DELIBNPP_HEADER__
#define __DELIBNPP_HEADER__




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
 *	导出函数定义
 */
LONG __stdcall delib_npp_server_createnew( IN LPCTSTR lpcszPipeName,
					   IN PFNDENAMEDPIPEANSWER lpvAnswerFunc,
					   IN LPCTSTR lpcszPassword,
					   IN DWORD dwMaxInstances = PIPE_UNLIMITED_INSTANCES,
					   IN DWORD dwTimeOut = NMPWAIT_USE_DEFAULT_WAIT,
					   IN LPVOID lpvArglist = NULL );
LONG __stdcall delib_npp_client_send_command( IN LPCTSTR lpcszPipeName, IN LPCTSTR lpszCommand, OUT LPTSTR lpszReply, IN DWORD dwSize );
LONG __stdcall delib_npp_client_stop_server( IN LPCTSTR lpcszPipeName, IN LPCTSTR lpcszPassword );




#endif	// __DELIBNPP_HEADER__