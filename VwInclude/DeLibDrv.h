#ifndef __DELIBDRV_HEADER__
#define __DELIBDRV_HEADER__


/**
 *	Error Code
 */
#define DELIBDRV_ERROR_SUCC			0
#define DELIBDRV_ERROR_UNKNOWN			-1000
#define DELIBDRV_ERROR_PARAM			-1001

#define DELIBDRV_ERROR_COPYFILE_TODRVDIR	-2001
#define DELIBDRV_ERROR_OPEN_SC_MANAGER		-2002
#define DELIBDRV_ERROR_CREATE_SERVICE		-2003
#define DELIBDRV_ERROR_DELETE_SERVICE		-2004
#define DELIBDRV_ERROR_OPEN_SERVICE		-2005
#define DELIBDRV_ERROR_START_SERVICE		-2006
#define DELIBDRV_ERROR_STOP_SERVICE		-2007
#define DELIBDRV_ERROR_QUERY_SERVICE_STATUS	-2008
#define DELIBDRV_ERROR_SERVICE_ALREADY_STOPED	-2009
#define DELIBDRV_ERROR_READ_IO			-2010
#define DELIBDRV_ERROR_WRITE_IO			-2011
#define DELIBDRV_ERROR_RAW_IO			-2012




/**
 *	º¯Êý¶¨Òå
 */
LONG __stdcall delib_drv_install( IN LPCTSTR lpcszDriverName, IN LPCTSTR lpcszDriverFilename, IN LPCTSTR lpcszSrvDesc, IN DWORD dwStartType );
LONG __stdcall delib_drv_uninstall( IN LPCTSTR lpcszDriverName );

LONG __stdcall delib_drv_start( IN LPCTSTR lpcszDriverName );
LONG __stdcall delib_drv_stop( IN LPCTSTR lpcszDriverName );
LONG __stdcall delib_drv_get_start( IN LPCTSTR lpcszDriverName );
LONG __stdcall delib_drv_set_start( IN LPCTSTR lpcszDriverName, IN DWORD dwStart );
LONG __stdcall delib_drv_get_status( IN LPCTSTR lpcszDriverName );
BOOL __stdcall delib_drv_is_exist( IN LPCTSTR lpcszDriverName );

LONG __stdcall delib_drv_write_io( LPCTSTR lpcszDriverName, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD dwInBufferSize );
LONG __stdcall delib_drv_read_io( LPCTSTR lpcszDriverName, DWORD dwIoControlCode, LPVOID lpOutBuffer, DWORD dwOutBufferSize, DWORD * pBytesReturned );
LONG __stdcall delib_drv_randw_io( LPCTSTR lpcszDriverName, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD dwInBufferSize, LPVOID lpOutBuffer, DWORD dwOutBufferSize, DWORD * pBytesReturned );

LONG __stdcall delib_drv_get_description( IN LPCTSTR lpcszDriverName, OUT LPTSTR lpszSrvDesc, IN DWORD dwSize );
LONG __stdcall delib_drv_set_description( IN LPCTSTR lpcszDriverName, IN LPCTSTR lpcszSrvDesc );

BOOL __stdcall delib_drv_is_wow64();
BOOL __stdcall delib_drv_wow64_enablewow64_fs_redirection( BOOL bWow64FsEnableRedirection );



#endif	// __DELIBDRV_HEADER__