#ifndef __DeLibPpl_HEADER__
#define __DeLibPpl_HEADER__



/**
 *	const var define
 */
#define DELIBPPL_NAME_DEINFOMFILTER		"DeInfoMFilter"

//	return value
#define DELIBPPL_ERROR_SUCC			0
#define DELIBPPL_ERROR_UNKNOWN			-1000
#define DELIBPPL_ERROR_PARAM			-1001
#define DELIBPPL_ERROR_CONNECT_PIPE		-1002
#define DELIBPPL_ERROR_WRITE_FILE		-1003
#define DELIBPPL_ERROR_SET_PIPE_HANDLE_STATE	-1004
#define DELIBPPL_ERROR_CREATE_PIPE		-1005
#define DELIBPPL_ERROR_CREATE_THREAD		-1006
#define DELIBPPL_ERROR_ALREADY_EXISTS		-1007

//	mutex
#define DELIBPPL_BUFSIZE			4096
#define DELIBPPL_MUTEX_NAMEPRE			"DeLibPpl_"
#define DELIBPPL_CMD_CLOSESERVER		"__delib_ppl_close_server__"

//	const define
#define DELIBPPL_LOGTYPE_TEXT			1
#define DELIBPPL_LOGTYPE_HTML			2

#define DELIBPPL_LOGPERIOD_HOURLY		1
#define DELIBPPL_LOGPERIOD_DAILY		2
#define DELIBPPL_LOGPERIOD_WEEKLY		3
#define DELIBPPL_LOGPERIOD_MONTHLY		4




/**
 *	struct define
 */
typedef struct tagDeLibPplConfig
{
	tagDeLibPplConfig()
	{
		memset( this, 0, sizeof(tagDeLibPplConfig) );
	}
	DWORD dwLogType;
	TCHAR szPipeName[ MAX_PATH ];
	TCHAR szLogDir[ MAX_PATH ];
	DWORD dwLogPeriod;

}STDELIBPPLCONFIG, *LPSTDELIBPPLCONFIG;


/**
 *	º¯Êý¶¨Òå
 */
LONG __stdcall delib_ppl_run_server_process( IN LPCTSTR lpcszPipeName, IN DWORD dwLogType );
LONG __stdcall delib_ppl_server_create( IN HINSTANCE hInstance, IN STDELIBPPLCONFIG * pstPipeLogConfig, OUT LPTSTR lpszError );
LONG __stdcall delib_ppl_client_send_command( IN LPCTSTR lpcszPipeName, IN LPCTSTR lpszRequest, OUT LPTSTR lpszResponse, IN DWORD dwSize, OUT LPTSTR lpszError );
LONG __stdcall delib_ppl_client_stop_server( IN LPCTSTR lpcszPipeName, OUT LPTSTR lpszError );





#endif