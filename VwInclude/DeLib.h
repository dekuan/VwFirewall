/*
 ************************************************************
 *
 *	DeLib.h
 *
 *	CLASS NAME:
 *		Desafe Lib
 *
 *	AUTHOR:
 *		liuqixing@gmail.com
 *
 *	HISTORY:
 *		2008-09-01 liuqixing@gmail.com Created
 *		2008-10-07 LiuQiXing V1.0.2.1002 增加函数
 *			delib_get_os_type_string
 *			delib_get_local_ipaddr
 *		2008-10-22 LiuQiXing v1.0.3.1003 修正函数
 *			delib_get_ipaddr_value_from_string 返回值为 -1 修改成 0
 *		2009-05-24 LiuQiXing v1.0.5.1005 修正函数
 *			delib_get_shellsystype 增加类型
 *			_DEOS_WINDOWS_2008_R2, _DEOS_WINDOWS_2008, _DEOS_WINDOWS_VISTA
 *		2010-09-01 LiuQiXing V1.1.4.1014
 *			delib_run_block_process_ex
 *		2010-09-08 LiuQiXing V1.1.5.1015
 *			delib_is_pe_file
 *		2010-11-18 LiuQiXing V1.1.6.1016
 *			delib_get_ipaddr_string_from_dword
 *		2010-12-12 LiuQiXing V1.1.8.1018
 *			delib_is_valid_ipaddr
 *
 ************************************************************
 */

#ifndef __DELIB_HEADER__
#define __DELIB_HEADER__


//////////////////////////////////////////////////////////////////////////
//	start define

#ifndef __DELIB_DEFINES__
#define __DELIB_DEFINES__

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

typedef struct tagIniSectionLineW
{
	tagIniSectionLineW()
	{
		memset( this, 0, sizeof(tagIniSectionLineW) );
	}
	WCHAR szLine[ MAX_PATH ];
	WCHAR szName[ 64 ];
	WCHAR szValue[ MAX_PATH ];
	
}STINISECTIONLINEW, *PSTINISECTIONLINEW;



typedef struct tagTextLine
{
	tagTextLine()
	{
		memset( this, 0, sizeof(tagTextLine) );
	}
	TCHAR szLine[ MAX_PATH ];

}STTEXTLINE, *LPSTTEXTLINE;

typedef struct tagDeParseUrl
{
	tagDeParseUrl()
	{
		memset( this, 0, sizeof(tagDeParseUrl) );
	}
	
	CHAR szScheme[ 16 ];		//	e.g. http
	CHAR szUser[ 32 ];
	CHAR szPass[ 32 ];
	CHAR szHost[ 128 ];
	UINT  uPort;
	CHAR szPath[ 1024 ];
	CHAR szQuery[ 1024 ];	//	after the question mark ?
	CHAR szFragment[ MAX_PATH ];	//	after the hashmark #	

} STDEPARSEURL, *LPSTDEPARSEURL;
typedef struct tagDeParseUrlW
{
	tagDeParseUrlW()
	{
		memset( this, 0, sizeof(tagDeParseUrlW) );
	}
	
	WCHAR szScheme[ 16 ];		//	e.g. http
	WCHAR szUser[ 32 ];
	WCHAR szPass[ 32 ];
	WCHAR szHost[ 128 ];
	UINT  uPort;
	WCHAR szPath[ 1024 ];
	WCHAR szQuery[ 1024 ];		//	after the question mark ?
	WCHAR szFragment[ MAX_PATH ];	//	after the hashmark #	

} STDEPARSEURLW, *LPSTDEPARSEURLW;

typedef enum tagDeWindowsSysType
{
	_DEOS_UNKNOWN,
	_DEOS_WINDOWS_32S,
	_DEOS_WINDOWS_NT3,
	_DEOS_WINDOWS_95,
	_DEOS_WINDOWS_98,
	_DEOS_WINDOWS_ME,
	_DEOS_WINDOWS_NT4,
	_DEOS_WINDOWS_2000,
	_DEOS_WINDOWS_XP,
	_DEOS_WINDOWS_2003,
	_DEOS_WINDOWS_2008,	//	Windows Server 2008
	_DEOS_WINDOWS_VISTA,	//	Windows Vista
	_DEOS_WINDOWS_2008_R2,	//	Windows Server 2008 R2
	_DEOS_WINDOWS_7,	//	Windows Server 7

} ENUMDEWINDOWSSYSTYPE;

//	网络部分
typedef struct tagDeIpAddr
{
	TCHAR szIpAddr[ 32 ];

}STDEIPADDR, *LPSTDEIPADDR;


#endif	// __DELIB_DEFINES__


//	end define
//////////////////////////////////////////////////////////////////////////


/**
*	替换字符串里面的一个子串
*/
static void delib_strchg(char* dest, int len, char* rstr)
{
	size_t rlen = strlen(rstr);
	memmove(dest+rlen, dest+len, strlen(dest+len)+1);
	memcpy(dest, rstr, rlen);
}
//	无符号搜索子串
static char * delib_stristr(char* src, char* dst)
{
	for(;(strlen(src)-strlen(dst) >= 0 && *src);src++)
	{
		if (!strnicmp(src, dst, strlen(dst)))
			return src;
	}
	return NULL;
}
//	替换目标字符串里面的子串
static void delib_replace( char* src, char* sFindStr, char* sRepWithStr )
{
	for(char* pos=src;(pos=delib_stristr(pos, sFindStr))!=NULL;pos=pos+strlen(sRepWithStr))
	{
		delib_strchg(pos, strlen(sFindStr), sRepWithStr);
	}
}
/**
 *	简单加密解密
 *		xorkey	- 加密的 key，默认等于 0x79
 *		pData	- 内容，可以是明文也可以是密文
 *		不管是明文还是密文，只要送进来就可以解密或者加密
 */
static VOID delib_xorenc( LPSTR pData, BYTE xorkey = 0XFF )
{
	//
	//	pData		- [in/out]	Pointer to a null-terminated string containing the Data
	//	xorkey		- [in]		Specifies the Encode/Decode key
	//
	while ( pData && *pData )
	{
		*pData = *pData ^ xorkey;
		pData += sizeof(CHAR);
	}
}


/**
 *	函数定义
 */
HWND  __stdcall delib_get_top_window( HWND hWnd );

BOOL  __stdcall delib_is_debug();
DWORD __stdcall delib_get_processor_number();
BOOL  __stdcall delib_get_hdisk_serialnumber( LPTSTR lpszSerialNumber, DWORD dwSize );
BOOL  __stdcall delib_get_cpuid( LPTSTR lpszCPUID, DWORD dwSize );
VOID  __stdcall delib_shutdown_system( UINT ShutdownFlag );
BOOL  __stdcall delib_report_systemevent( LPCTSTR lpcszSrcName, DWORD dwEventID, WORD wCategory, WORD wNumStrings, LPCTSTR * lpStrings );
DWORD __stdcall delib_create_process( LPCTSTR lpszCmdLine, BOOL bBlocked );
BOOL  __stdcall delib_run_block_process( LPCTSTR lpszCmdLine );
BOOL  __stdcall delib_run_block_process_ex( LPCTSTR lpszCmdLine, BOOL bShowWindow, DWORD dwTimeout = INFINITE );

BOOL  __stdcall delib_get_modulefile_pathA( LPVOID lpvInnerAddr, HINSTANCE * hInstance, LPSTR lpszModuleFilePath, DWORD dwSize );
BOOL  __stdcall delib_get_modulefile_pathW( LPVOID lpvInnerAddr, HINSTANCE * hInstance, LPWSTR lpwszModuleFilePath, DWORD dwSize );
#ifdef UNICODE
	#define delib_get_modulefile_path  delib_get_modulefile_pathW
#else
	BOOL  __stdcall delib_get_modulefile_path( LPVOID lpvInnerAddr, HINSTANCE * hInstance, LPSTR lpszModuleFilePath, DWORD dwSize );
#endif

BOOL  __stdcall delib_alloc_share_memory( IN DWORD dwSize, IN LPCTSTR lpcszMemName, OUT VOID ** lppvBuffer, OUT HANDLE * phFileMap, OUT BOOL * lpbCreate );
BOOL  __stdcall delib_open_share_memory( IN LPCTSTR lpcszMemName, OUT VOID ** lppvBuffer, OUT HANDLE * phFileMap );
VOID  __stdcall delib_close_share_memory( IN OUT VOID ** lppvBuffer, IN OUT HANDLE * phFileMap );


//	从资源里面提取文件到磁盘
BOOL  __stdcall delib_extract_file_from_resource( HINSTANCE hInst, LPCTSTR lpcszResType, UINT uResID, LPCTSTR lpszDestFilename );
PVOID __stdcall delib_get_resource_buffer( HINSTANCE hInst, LPCTSTR lpcszResType, UINT uResID, DWORD * pdwSize );

BOOL  __stdcall delib_file_exists( LPCTSTR lpcszFile );
BOOL  __stdcall delib_extract_filename( LPCTSTR lpcszFullName, LPTSTR lpszFileName, DWORD dwSize );
BOOL  __stdcall delib_extract_filepath( LPCTSTR lpcszFullName, LPTSTR lpszPathName, DWORD dwSize );
BOOL  __stdcall delib_extract_fileext( LPCTSTR lpszFullName, LPTSTR lpszExtName, DWORD dwSize );
BOOL  __stdcall delib_change_fileext( LPCTSTR lpcszSrcFileName, LPCTSTR lpcszNewExtName, LPTSTR lpszNewFilename, DWORD dwSize );
DWORD __stdcall delib_get_file_size( LPCTSTR lpcszFilePath );
BOOL  __stdcall delib_set_file_writable( LPCTSTR lpcszFilePath );

BOOL  __stdcall delib_get_file_versionA( LPCSTR lpszFilePath, LPSTR lpszVersion, DWORD dwSize );
BOOL  __stdcall delib_get_file_versionW( LPCWSTR lpszFilePath, LPWSTR lpszVersion, DWORD dwSize );
#ifdef UNICODE
	#define delib_get_file_version  delib_get_file_versionW
#else
	BOOL  __stdcall delib_get_file_version( LPCSTR lpszFilePath, LPSTR lpszVersion, DWORD dwSize );
#endif

LONG  __stdcall delib_get_file_mapbuffer( LPCTSTR lpszFilePath, LPTSTR lpszBuffer, DWORD dwSize );
BOOL  __stdcall delib_get_file_md5( LPCTSTR lpszFilePath, LPTSTR lpszMd5, DWORD dwSize, LPTSTR lpszError );
BOOL  __stdcall delib_get_string_md5( LPCTSTR lpszString, LPTSTR lpszMd5, DWORD dwSize, DWORD dwSpecStringLen = 0 );
BOOL  __stdcall delib_is_pe_file( LPCTSTR lpszFilePath );

ENUMDEWINDOWSSYSTYPE __stdcall delib_get_shellsystype();
BOOL  __stdcall delib_is_window_nt4();
BOOL  __stdcall delib_is_wow64();
BOOL  __stdcall delib_get_os_type_string( LPTSTR pszOsType, DWORD dwOtSize );

BOOL  __stdcall delib_get_window_tempdirA( LPSTR lpszDir, DWORD dwSize );
BOOL  __stdcall delib_get_window_tempdirW( LPWSTR lpszDir, DWORD dwSize );
#ifdef UNICODE
	#define delib_get_window_tempdir  delib_get_window_tempdirW
#else
	BOOL  __stdcall delib_get_window_tempdir( LPSTR lpszDir, DWORD dwSize );
#endif


BOOL  __stdcall delib_copy_text_to_clipboard( LPCTSTR lpszString, HWND hWndNewOwner = NULL );

DWORD __stdcall delib_get_crc32( IN CONST BYTE * pBinBuf, IN INT nBufLen );

INT   __stdcall delib_ini_parse_section_lineexA( LPCSTR lpcszIniFile, LPCSTR lpcszSection, STINISECTIONLINE * pstSectionList, BOOL bParseValue = FALSE );
INT   __stdcall delib_ini_parse_section_lineexW( LPCWSTR lpcszIniFile, LPCWSTR lpcszSection, STINISECTIONLINEW * pstSectionList, BOOL bParseValue = FALSE );
#ifdef UNICODE
	#define delib_ini_parse_section_lineex  delib_ini_parse_section_lineexW
#else
	INT   __stdcall delib_ini_parse_section_lineex( LPCTSTR lpcszIniFile, LPCTSTR lpcszSection, STINISECTIONLINE * pstSectionList, BOOL bParseValue = FALSE );
#endif

DWORD __stdcall delib_get_chr_countA( LPCSTR lpszString, CHAR lpChr );
DWORD __stdcall delib_get_chr_countW( LPCWSTR lpwszString, WCHAR lpwChr );
#ifdef UNICODE
	#define delib_get_chr_count  delib_get_chr_countW
#else
	DWORD __stdcall delib_get_chr_count( LPCSTR lpszString, CHAR lpChr );
#endif

BOOL  __stdcall delib_get_cookie_value( LPCTSTR lpszCookieString, LPCTSTR lpszCookieName, LPTSTR lpszValue, DWORD dwSize );
UINT  __stdcall delib_get_casecookie_value( LPCTSTR lpszCookieString, LPCTSTR lpszCookieName, LPTSTR lpszValue, DWORD dwSize );
UINT  __stdcall delib_get_casecookie_value_ex( LPCSTR lpszCookieString, LPCSTR lpszCookieName, LPCSTR lpcszEndStr, LPSTR lpszValue, DWORD dwSize, BOOL bTrimDbQuote );
BOOL  __stdcall delib_is_valid_systemtime( SYSTEMTIME * pst );
BOOL  __stdcall delib_get_string_systemtime( SYSTEMTIME * pst, LPTSTR lpszBuffer, DWORD dwSize );
BOOL  __stdcall delib_get_systemtime_from_string( LPCTSTR lpcszTime, SYSTEMTIME * pst );
INT   __stdcall delib_split_string( LPCTSTR lpcszInput, LPCTSTR lpcszDelimiter, STTEXTLINE * pstResultList, BOOL bIncludeEmpties );
BOOL  __stdcall delib_is_match_with_pattern( LPCTSTR lpcszPattern, LPCTSTR lpcszTargetString );
//INT   __stdcall delib_split_stringA( const string & sInput, const string & sDelimiter, STTEXTLINE * pstResultList, BOOL bIncludeEmpties );

//
//	network
//
BOOL  __stdcall delib_is_valid_ipaddr( LPCTSTR lpszString );
DWORD __stdcall delib_get_ipaddr_value_from_string( LPCSTR lpcszIpAddr );
BOOL  __stdcall delib_get_ipaddr_string_from_dword( DWORD dwIpAddrValue, LPTSTR lpszIpAddr, DWORD dwSize );
BOOL  __stdcall delib_get_local_ipaddr( LPTSTR lpszLocalAddr, DWORD dwSize );
INT   __stdcall delib_get_all_locipaddr( STDEIPADDR * pstIpAddrList );
BOOL  __stdcall delib_get_mac_address( LPCTSTR lpcszNetBiosName, LPTSTR lpszMacAddress, DWORD dwSize );

BOOL  __stdcall delib_parse_urlA( LPCSTR lpcszUrl, DWORD dwUrlLen, STDEPARSEURL * pstParseUrl );
BOOL  __stdcall delib_parse_urlW( LPCWSTR lpcwszUrl, DWORD dwUrlLen, STDEPARSEURLW * pstParseUrlW );
#ifdef UNICODE
	#define delib_parse_url  delib_parse_urlW
#else
	BOOL  __stdcall delib_parse_url( LPCSTR lpcszUrl, DWORD dwUrlLen, STDEPARSEURL * pstParseUrl );
#endif

VOID  __stdcall delib_get_url_decode( char *str );
INT   __stdcall delib_get_url_encode( LPCTSTR lpcszInput, INT nInputLen, LPTSTR lpszOutBuf, INT nOutBufLen );
BOOL  __stdcall delib_get_fileinfo_from_url( LPCTSTR lpcszFullUri, LPTSTR lpszExt, DWORD dwESize, LPTSTR lpszFile, DWORD dwFSize );

BOOL  __stdcall delib_lauch_iebrowser( LPCTSTR lpcszUrl );

BOOL  __stdcall delib_isexist_w3svc( BOOL bMustRunning );
BOOL  __stdcall delib_stop_w3svc();
BOOL  __stdcall delib_start_w3svc();



#endif	// __DELIB_HEADER__