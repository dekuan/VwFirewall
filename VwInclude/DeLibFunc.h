// DeLib.h: interface for the CDeLib class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DELIB_HEADER__
#define __DELIB_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef WIN32
	#pragma warning (disable: 4514 4786)
#endif

#include <vector>
#include <string>
using namespace std;


#include <nb30.h>
#pragma comment( lib, "netapi32.lib" )

#ifndef FD_SET
#include "Winsock2.h"
#endif
#pragma comment( lib, "Ws2_32.lib" )


static const VERSION_MAJOR	= 1;
static const VERSION_MINOR	= 1;


//////////////////////////////////////////////////////////////////////////
//	start define

#ifndef _DELIB_DEFINES_
#define _DELIB_DEFINES_


#define DELIB_MAX_FILEMAP_LENGTH	( 100 * 1024 * 1024 )
#define DELIB_VALIDIPADDRSTRING		"1234567890."	//	是否有效的 IP 地址检测里面用到的常量


/**
 *	结构体定义
 */
typedef struct tagIniSectionLine
{
	tagIniSectionLine()
	{
		memset( this, 0, sizeof(tagIniSectionLine) );
	}
	TCHAR szLine[ MAX_PATH ];
	TCHAR szName[ 64 ];
	TCHAR szValue[ MAX_PATH ];
	
}STINISECTIONLINE, *PSTINISECTIONLINE;

typedef enum tagWindowsSysType
{
	_OS_WINDOWS_32S,
	_OS_WINDOWS_NT3,
	_OS_WINDOWS_95,
	_OS_WINDOWS_98,
	_OS_WINDOWS_ME,
	_OS_WINDOWS_NT4,
	_OS_WINDOWS_2000,
	_OS_WINDOWS_XP,
	_OS_WINDOWS_2003
		
} ENUMWINDOWSSYSTYPE;

//	网络部分
typedef struct tagAstat
{
	ADAPTER_STATUS adapt;
	NAME_BUFFER NameBuff[ 30 ];
}STASTAT, * PSTASTAT;

typedef struct tagDeIpAddr
{
	TCHAR szIpAddr[ 32 ];

}STDEIPADDR, *LPSTDEIPADDR;

#endif	// _DELIB_DEFINES_

//	end define
//////////////////////////////////////////////////////////////////////////


/**
 *	函数定义
 */
BOOL  _delib_is_debug();
DWORD _delib_get_processor_number();
VOID  _delib_shutdown_system( UINT ShutdownFlag );
BOOL  _delib_report_systemevent( LPCTSTR lpcszSrcName, DWORD dwEventID, WORD wCategory, WORD wNumStrings, LPCTSTR * lpStrings );
DWORD _delib_create_process( LPCTSTR lpszCmdLine, BOOL bBlocked );
BOOL  _delib_run_block_process( LPCTSTR lpszCmdLine );

BOOL  _delib_alloc_share_memory( DWORD dwSize, LPCTSTR lpcszMemName, VOID ** lppvBuffer, HANDLE hFileMap, LPBOOL lpbCreate, LPSECURITY_ATTRIBUTES lpFileMappingAttributes );
BOOL  _delib_open_share_memory( LPCTSTR lpcszMemName, VOID ** lppvBuffer, HANDLE hFileMap );

//	从资源里面提取文件到磁盘
BOOL  _delib_extract_file_from_resource( HINSTANCE hInst, LPCTSTR lpcszResType, UINT uResID, LPCTSTR lpszDestFilename );
PVOID _delib_get_resource_buffer( HINSTANCE hInst, LPCTSTR lpcszResType, UINT uResID, DWORD * pdwSize );

BOOL  _delib_file_exists( LPCTSTR lpcszFile );
DWORD _delib_get_file_size( LPCTSTR lpszFilePath );
BOOL  _delib_set_file_writable( LPCTSTR lpcszFilePath );
BOOL  _delib_get_file_version( LPCTSTR lpszFilePath, LPTSTR lpszVersion, DWORD dwSize );
BOOL  _delib_get_file_mapbuffer( LPCTSTR lpszFilePath, LPTSTR lpszBuffer, DWORD dwSize );
BOOL  _delib_get_file_md5( LPCTSTR lpszFilePath, LPTSTR lpszMd5, DWORD dwSize, LPTSTR lpszError );
BOOL  _delib_get_string_md5( LPCTSTR lpszString, LPTSTR lpszMd5, DWORD dwSize, DWORD dwSpecStringLen = 0 );

ENUMWINDOWSSYSTYPE _delib_get_shellsystype();
BOOL  _delib_copy_text_to_clipboard( LPCTSTR lpszString, HWND hWndNewOwner = NULL );

DWORD _delib_get_crc32( IN CONST BYTE * pBinBuf, IN INT nBufLen );
BOOL  _delib_ini_parse_section_line( LPCTSTR lpcszIniFile, LPCTSTR lpcszSection, vector<STINISECTIONLINE> & vcSection, BOOL bParseValue = FALSE );

VOID  _delib_xorenc( CHAR * pData, BYTE xorkey = 0XFF );
DWORD _delib_get_chr_count( LPCTSTR lpszString, TCHAR lpChr );
BOOL  _delib_get_cookie_value( LPCTSTR lpszCookieString, LPCTSTR lpszCookieName, LPTSTR lpszValue, DWORD dwSize );
BOOL  _delib_is_valid_systemtime( SYSTEMTIME * pst );
BOOL  _delib_get_string_systemtime( SYSTEMTIME * pst, LPTSTR lpszBuffer, DWORD dwSize );
INT   _delib_split_string( const string & sInput, const string & sDelimiter, vector<string> & vcResult, BOOL bIncludeEmpties );

//
//	network
//
BOOL  _delib_is_valid_ipaddr( LPCTSTR lpszString );
DWORD _delib_get_ipaddr_value_from_string( LPCSTR lpcszIpAddr );
BOOL  _delib_get_all_locipaddr( vector<STDEIPADDR> & vcIpAddr );
BOOL  _delib_get_mac_address( LPCTSTR lpcszNetBiosName, LPTSTR lpszMacAddress, DWORD dwSize );
BOOL  _delib_get_hdisk_serialnumber( LPTSTR lpszSerialNumber, DWORD dwSize );





#endif // __DELIB_HEADER__
