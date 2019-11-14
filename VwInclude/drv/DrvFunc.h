#ifndef __DRVFUNC_HEADER__
#define __DRVFUNC_HEADER__




/**
 *	dirver functions
 */
NTSTATUS drvfunc_pass_through( IN PDEVICE_OBJECT pDevObj, IN PIRP Irp );
BOOL     drvfunc_ispass_systemaction( INT nOPType );
BOOL     drvfunc_get_windowsdir( IN PDRIVER_OBJECT DriverObject, OUT PWCHAR pwszWindowDir, IN USHORT uSize );
NTSTATUS drvfunc_get_localtime( PCHAR pszCurrentTime, ULONG uSize );
NTSTATUS drvfunc_write_log( const PCHAR pszData );

NTSTATUS drvfunc_get_osversion( PULONG pOsMajorVersion, PULONG pOsMinorVersion );

//	for process
ULONG	 drvfunc_get_processname_offset();
VOID	 drvfunc_get_processname( IN ULONG uProcessNameOffset, OUT PCHAR pProcessName, IN ULONG uSize );
VOID     drvfunc_get_processfullname( OUT PCHAR pszFullProcessName, IN ULONG uSize );

PVOID	 drvfunc_malloc( ULONG MaxBuffer );
VOID	 drvfunc_free( PVOID pBuffer );
VOID	 drvfunc_zero_memory( PVOID pBuffer, int nLen );
VOID	 drvfunc_copy_memory( PVOID pDest, PVOID pSrc, int nLen );

NTSTATUS drvfunc_w2a( IN PWCHAR pSourceString, OUT PCHAR pDestString, IN USHORT nBufferLen );
NTSTATUS drvfunc_w2c( IN PWCHAR pSourceString, OUT PCHAR pDestString, IN USHORT nBufferLen );
NTSTATUS drvfunc_w2c_ex( IN PWCHAR pwszSourceString, IN USHORT nSourceStringLength, OUT PCHAR pszDestString, IN USHORT nDestBufferLength );
NTSTATUS drvfunc_u2c( IN PUNICODE_STRING pusSourceString, OUT PCHAR pDestString, IN USHORT nBufferLen );
NTSTATUS drvfunc_c2w( IN PCHAR pSourceString, OUT PWCHAR pDestString, IN USHORT nBufferLen );
NTSTATUS drvfunc_a2u( IN PCHAR pSourceString, OUT PUNICODE_STRING pusDest );

ULONG	 drvfunc_get_fullkeyname( IN HANDLE hKey, IN PUNICODE_STRING pusSubKeyVal, IN PCHAR pFullKeyName );
PCHAR	 drvfunc_get_win32_regpath( IN PCHAR pKeyName, IN OUT PCHAR* ppRoot );
PVOID	 drvfunc_get_pointer( IN HANDLE handle );
VOID	 drvfunc_release_pointer( IN PVOID object );

INT	 drvfunc_get_dir_length( LPCSTR lpcszFilePath, UINT uFilePathLength );
BOOL	 drvfunc_is_dir_exist( IN LPCWSTR lpcwszFilename );
DWORD	 drvfunc_get_filesize( IN LPCWSTR lpcwszFilename );
BOOL	 drvfunc_get_filecontent( IN LPCWSTR lpcwszFilename, OUT LPSTR lpszContent, IN DWORD dwSize );
BOOL	 drvfunc_get_filename_by_handle( IN HANDLE hFileHandle, OUT UNICODE_STRING * pusFilename );
UINT	 drvfunc_get_ansifilename_by_handle( IN HANDLE hFileHandle, OUT PCHAR pszFilename, IN USHORT uSize );
BOOL     drvfunc_delete_file( IN UNICODE_STRING * pusFilename );

UINT	 drvfunc_get_casecookie_value( LPCSTR lpszCookieString, LPCSTR lpszCookieName, LPCSTR lpcszEndStr, LPSTR lpszValue, DWORD dwSize );


#endif	// __DRVFUNC_HEADER__