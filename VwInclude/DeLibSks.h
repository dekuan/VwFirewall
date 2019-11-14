#ifndef __DELIBSKS_HEADER__
#define __DELIBSKS_HEADER__



/**
 *	return value
 *	copyed from DeSocketIoComp.h
 */
#define DELIBSKS_ERROR_SUCC				0
#define DELIBSKS_ERROR_UNKNOWN				-1000
#define DELIBSKS_ERROR_PARAM				-1001
#define DELIBSKS_ERROR_SOCKET_ALREADY_EXISTS		-1002
#define DELIBSKS_ERROR_SOCKET_APP			-1003

#define DELIBSKS_ERROR_WSA_STARTUP			-2000
#define DELIBSKS_ERROR_CREATE_IO_COMPPORT		-2001
#define DELIBSKS_ERROR_CREATE_THREAD			-2002
#define DELIBSKS_ERROR_RESOLVE_ADDRESS			-2003
#define DELIBSKS_ERROR_SOCKET				-2004
#define DELIBSKS_ERROR_BIND				-2005
#define DELIBSKS_ERROR_WSA_IOCTL			-2006
#define DELIBSKS_ERROR_LISTEN				-2007
#define DELIBSKS_ERROR_HEAPALLOC			-2008
#define DELIBSKS_ERROR_WSA_WAIT_FOR_MULTIPLE_EVENTS	-2009
#define DELIBSKS_ERROR_GET_SOCK_OPT			-2010


/**
 *	callback Answer Processor
 *	eg: static BOOL CALLBACK _callbackGetSocketIoCompResponse( IN LPVOID lpvArglist, IN LPTSTR lpszMessage, OUT LPTSTR lpszResponse, IN DWORD dwSize );
 */
typedef BOOL (CALLBACK * PFNDELIBSKSANSWER)( IN LPVOID lpvArglist, IN LPTSTR lpszMessage, IN DWORD dwMessageLength, OUT LPTSTR lpszResponse, IN DWORD dwSize );
typedef VOID (CALLBACK * PFNDELIBSKSERRORPROC)( IN LPVOID lpvArglist, IN HANDLE hAppHandle, IN LONG lnErrorCode );


/**
 *	µ¼³öº¯Êý
 */
LONG __stdcall delib_sks_server_createnew( IN LPCTSTR lpcszIpAddr, IN UINT uPort, IN PFNDELIBSKSANSWER lpvAnswerFunc, IN LPCTSTR lpcszPassword, IN LPVOID lpvArglist );
LONG __stdcall delib_sks_client_send_message( IN LPCTSTR lpcszIpAddr, IN UINT uPort, LPCTSTR lpcszMessage, LPTSTR lpszResponse, DWORD dwSize );
LONG __stdcall delib_sks_client_send_message_ex( IN LPCTSTR lpcszIpAddr, IN UINT uPort, LPCTSTR lpcszMessage, DWORD dwMsgLength, LPTSTR lpszResponse, DWORD dwSize );
LONG __stdcall delib_sks_client_send_message_only( IN LPCTSTR lpcszIpAddr, IN UINT uPort, LPCTSTR lpcszMessage, DWORD dwMsgLength );
LONG __stdcall delib_sks_client_stop_server( IN LPCTSTR lpcszIpAddr, IN UINT uPort, IN LPCTSTR lpcszPassword );





#endif	// __DELIBSKS_HEADER__



