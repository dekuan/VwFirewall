#ifndef __DRVFUNC_HEADER__
#define __DRVFUNC_HEADER__

#include "stdafx.h"
#include "md5.h"


#define IO_CHECK_CREATE_PARAMETERS		0x0200
#define IO_ATTACH_DEVICE			0x0400
#define IO_IGNORE_SHARE_ACCESS_CHECK		0x0800

//	for process
#define SYSNAME					"System"	//	进程结构里的标识

//	for process offset
#define BASE_PROCESS_PEB_OFFSET			0x01B0
#define BASE_PEB_PROCESS_PARAMETER_OFFSET	0x0010
#define BASE_PROCESS_PARAMETER_FULL_IMAGE_NAME	0x003C

#define W2003_BASE_PROCESS_PEB_OFFSET		0x0190


/**
 *	driver letter infomation
 */
typedef struct tagDriverInfo
{
	UCHAR uszDosDriverLetter[ 3 ];		//	"A:"
	WCHAR wszDosFullDriverLetter[ 7 ];	//	L"\\??\\A:"
	UCHAR uszDeviceName[ 64 ];		//	"\\Device\\HarddiskVolume1"
	WCHAR wszDeviceName[ 64 ];		//	L"\\Device\\HarddiskVolume1"

}STDRIVERINFO, *LPSTDRIVERINFO;




/**
 *	dirver functions
 */
BOOLEAN  drvfunc_query_device_name( IN OUT LPSTDRIVERINFO pstDriverInfo );
BOOLEAN  drvfunc_get_windowsdir( IN PDRIVER_OBJECT DriverObject, OUT PWCHAR pwszWindowDir, IN USHORT uSize );
NTSTATUS drvfunc_get_localtime( PCHAR pszCurrentTime, ULONG uSize );

NTSTATUS drvfunc_w2a( IN PWCHAR pSourceString, OUT PCHAR pDestString, IN USHORT nBufferLen );
NTSTATUS drvfunc_w2c( IN PWCHAR pSourceString, OUT PCHAR pDestString, IN USHORT nBufferLen );
NTSTATUS drvfunc_w2c_ex( IN PWCHAR pwszSourceString, IN USHORT nSourceStringLength, OUT PCHAR pszDestString, IN USHORT nDestBufferLength );
NTSTATUS drvfunc_u2c( IN PUNICODE_STRING pusSourceString, OUT PCHAR pDestString, IN USHORT nBufferLen );
NTSTATUS drvfunc_c2w( IN PCHAR pSourceString, OUT PWCHAR pDestString, IN USHORT nDestBufferLenInBytes );
NTSTATUS drvfunc_c2w_ex( IN PCHAR pSourceString, IN USHORT nSourceStringLength, OUT PWCHAR pDestString, IN USHORT nDestBufferLenInBytes );
NTSTATUS drvfunc_a2u( IN PCHAR pSourceString, OUT PUNICODE_STRING pusDest );

//	for files
INT	 drvfunc_get_dir_length( UCHAR * lpuszFilePath, UINT uFilePathLength );
INT	 drvfunc_get_dir_length_w( WCHAR * lpwszFilePath, UINT uFilePathLength );
BOOLEAN  drvfunc_is_dir_a( UCHAR * lpuszFilePath, UINT uFilePathLength );
BOOLEAN  drvfunc_is_dir_w( WCHAR * lpwszFilePath, UINT uFilePathLength );
BOOLEAN  drvfunc_is_dir_exist_a( UCHAR * pucszFilename );
BOOLEAN	 drvfunc_is_dir_exist_w( IN LPCWSTR lpcwszFilename );
BOOLEAN	 drvfunc_is_file_exist_a( UCHAR * pucszFilename );
BOOLEAN  drvfunc_is_file_exist( LPCWSTR lpcwszFilename );
DWORD	 drvfunc_get_filesize( IN LPCWSTR lpcwszFilename );
BOOLEAN	 drvfunc_get_filecontent( IN LPCWSTR lpcwszFilename, OUT LPSTR lpszContent, IN DWORD dwSize );
BOOLEAN  drvfunc_save_filecontent( IN LPCWSTR lpcwszFilename, IN CONST PVOID lpcszContent, IN DWORD dwSize, IN BOOLEAN bAppend );
BOOLEAN	 drvfunc_get_filename_by_handle( IN HANDLE hFileHandle, OUT UNICODE_STRING * pusFilename );
UINT	 drvfunc_get_ansifilename_by_handle( IN HANDLE hFileHandle, OUT PCHAR pszFilename, IN USHORT uSize );
UINT	 drvfunc_get_widefilename_by_handle( IN HANDLE hFileHandle, OUT PWCHAR pwszFilename, IN USHORT uSize );
BOOLEAN  drvfunc_delete_file( IN UNICODE_STRING * pusFilename );

//	for process
NTSTATUS drvfunc_get_os_version( PULONG pOsMajorVersion, PULONG pOsMinorVersion );
ULONG	 drvfunc_get_processname_offset();
BOOLEAN  drvfunc_get_processname( IN ULONG uProcessNameOffset, OUT PCHAR pProcessName, IN ULONG uSize );
BOOLEAN  drvfunc_get_processfullname( OUT PCHAR pszFullProcessName, IN ULONG uSize, OUT USHORT * puFullnameLength );

//	for string
INT      drvfunc_get_ini_value_int( CHAR * pszBuffer, LPCSTR lpszKeyName );
BOOL     drvfunc_get_ini_value_bool( CHAR * pszBuffer, LPCSTR lpszKeyName );
UINT	 drvfunc_get_casecookie_value( LPCSTR lpszCookieString, LPCSTR lpszCookieName, LPCSTR lpcszEndStr, LPSTR lpszValue, DWORD dwSize, BOOL bTrimDbQuote );
BOOLEAN	 drvfunc_get_casecookie_boolvalue( LPCSTR lpszCookieString, LPCSTR lpszCookieName, LPCSTR lpcszEndStr );
VOID     drvfunc_xorenc( LPSTR pData, BYTE xorkey );



#endif	// __DRVFUNC_HEADER__