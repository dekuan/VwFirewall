// VwFunc.h: interface for the CVwFunc class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWFUNCBASE_HEADER__
#define __VWFUNCBASE_HEADER__

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#include <vector>
using namespace std;

#include <winsvc.h>
#pragma comment( lib, "Version.lib" )

#include "md5.h"
#include "MultiString.h"

/**
 *	常量定义
 */
#define CONST_VWFUNCBASE_MAX_FILEMAP_LENGTH	( 100 * 1024 * 1024 )
#define CONST_VWFUNCBASE_VALIDIPADDRSTRING	"1234567890."	//	是否有效的 IP 地址检测里面用到的常量
#define CONST_VWFUNCBASE_VALIDDOMAINSTRING	"*.-1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"	//	域名中所能包含的字符
#define CONST_VWFUNCBASE_DOMAINEXTLIST		"|.com.cn|.com.tw|.com.mx|.net.cn|.net.tw|.org.cn|.org.tw|.org.nz|.org.uk|.gov.cn|.gov.tw|.idv.tw|.us.com|.br.com|.cn.com|.de.com|.eu.com|.hu.com|.no.com|.qc.com|.ru.com|.sa.com|.se.com|.uk.com|.uy.com|.za.com|.se.net|.uk.net|"



//////////////////////////////////////////////////////////////////////////

/**
 *	替换字符串里面的一个子串
 */
static void _vwfunc_strchg(char* dest, int len, char* rstr)
{
	int rlen = (int)strlen(rstr);
	memmove(dest+rlen, dest+len, strlen(dest+len)+1);
	memcpy(dest, rstr, rlen);
}
//	无符号搜索子串
static char * _vwfunc_stristr(char* src, char* dst)
{
	for(;(strlen(src)-strlen(dst) >= 0 && *src);src++)
	{
		if (!strnicmp(src, dst, strlen(dst)))
			return src;
	}
	return NULL;
}
//	替换目标字符串里面的子串
static void _vwfunc_replace( char* src, char* sFindStr, char* sRepWithStr )
{
	for(char* pos=src;(pos=_vwfunc_stristr(pos, sFindStr))!=NULL;pos=pos+strlen(sRepWithStr))
	{
		_vwfunc_strchg(pos, strlen(sFindStr), sRepWithStr);
	}
}

//////////////////////////////////////////////////////////////////////////

typedef struct tagParseUrl
{
	tagParseUrl()
	{
		memset( this, 0, sizeof(tagParseUrl) );
	}
	
	TCHAR szScheme[ 16 ];		//	e.g. http
	TCHAR szUser[ 32 ];
	TCHAR szPass[ 32 ];
	TCHAR szHost[ 128 ];
	UINT  uPort;
	TCHAR szPath[ 1024 ];
	TCHAR szQuery[ 1024 ];	//	after the question mark ?
	TCHAR szFragment[ MAX_PATH ];	//	after the hashmark #	
	
} STPARSEURL, *PSTPARSEURL;

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


//	返回当前是否是调试环境
BOOL _vwfunc_is_debug();

//	分配一块共享内存
BOOL _vwfunc_malloc_share_memory( DWORD dwSize, LPCTSTR lpcszMemName, VOID ** lppvBuffer, HANDLE phFileMap, LPBOOL lpbCreate, LPSECURITY_ATTRIBUTES lpFileMappingAttributes = NULL );

//	打开一块共享内存
BOOL _vwfunc_open_share_memory( LPCTSTR lpcszMemName, VOID ** lppvBuffer, HANDLE hFileMap );

//	get file map buffer
BOOL _vwfunc_get_file_mapbuffer( LPCTSTR lpszFilePath, LPTSTR lpszBuffer, DWORD dwSize );

//	Get File md5
BOOL _vwfunc_get_file_md5( LPCTSTR lpszFilePath, LPTSTR lpszMd5, DWORD dwSize, LPTSTR lpszError );

//	获取一个字符串的 MD5 值(32位字符串)
BOOL _vwfunc_get_string_md5( LPCTSTR lpszString, LPTSTR lpszMd5, DWORD dwSize, DWORD dwSpecStringLen = 0 );


//	Get Windows system type
ENUMWINDOWSSYSTYPE _vwfunc_get_shellsystype();

//	is window nt4
BOOL _vwfunc_is_window_nt4();

//	获取 Windows temp 目录
BOOL _vwfunc_get_window_tempdir( LPTSTR lpszDir, DWORD dwSize );

//	简单加密解密
VOID  _vwfunc_xorenc( CHAR * pData, BYTE xorkey = 0XFF );

//	从资源里面提取文件到磁盘
BOOL  _vwfunc_extract_file_from_resource( HINSTANCE hInst, LPCTSTR lpcszResType, UINT uResID, LPCTSTR lpszDestFilename );
VOID *_vwfunc_get_resource_buffer( HINSTANCE hInst, LPCTSTR lpcszResType, UINT uResID, DWORD * pdwSize );


//	检查文件是否存在
BOOL  _vwfunc_my_file_exists( LPCTSTR lpcszFile );

//	获取文件大小
DWORD _vwfunc_get_my_filesize( LPCTSTR pszFilePath );

//	从文件资源里提出版本
BOOL _vwfunc_get_file_version( LPCTSTR lpszFilePath, LPTSTR lpszVersion, DWORD dwSize );

//	给与文件可写选项
BOOL _vwfunc_file_give_write_attribute( LPCTSTR lpcszFilePath );

//	获取 DLL 所在的全路径
BOOL _vwfunc_get_dll_modulepath( LPVOID lpvInnerAddr, HINSTANCE * hInstance, LPTSTR lpszModulePath, DWORD dwSize );


//////////////////////////////////////////////////////////////////////////


//	获取某个字符在一个字符串中的个数
DWORD _vwfunc_get_chr_count( LPCTSTR lpszString, TCHAR lpChr, UINT uStringLen = 0 );

//	检查字符串是否是合法的 IP 地址
BOOL  _vwfunc_is_valid_ipaddr( LPCTSTR lpszString, UINT uStringLen = 0 );

//	检查字符串是否是合法的域名
BOOL  _vwfunc_is_valid_domain( LPCTSTR lpszString );

//	检查域名是否是国内域名
static inline BOOL  _vwfunc_is_inland_domain( LPCTSTR lpcszHost, UINT uHostLen = 0 );

//	获取 Root 域名
BOOL  _vwfunc_get_root_host( LPCTSTR lpcszHost, LPTSTR lpszRootHost, DWORD dwSize );
BOOL  _vwfunc_get_root_host2( LPCTSTR lpcszHost, LPTSTR lpszRootHost, DWORD dwSize );
LPCTSTR _vwfunc_get_root_host_ptr( LPCTSTR lpcszHost );
LPCTSTR _vwfunc_get_root_host_ptr_ex( LPCTSTR lpcszHost, UINT * uRetLen );

//	从 URL 中提取 HOST
BOOL  _vwfunc_get_host_from_url( LPCTSTR lpcszUrl, LPTSTR lpszHost, DWORD dwSize );

//	获取 URL 参数里的参数的值
BOOL _vwfunc_get_request_query_string( LPCTSTR lpcszUrl, LPCTSTR lpcszName, LPTSTR lpszValue, DWORD dwSize );

//	获取指定 Cookie Value
BOOL _vwfunc_get_spec_cookie_value( LPCTSTR lpszCookieString, LPCTSTR lpszCookieName, LPTSTR lpszValue, DWORD dwSize );

//	替换不可见的控制符号为“_”
TCHAR * _vwfunc_replace_controlchars( LPSTR lpszStr, UINT uLen );

//	拷贝字符串
inline BOOL _vwfunc_memcpy( LPCTSTR lpcszSrc, DWORD dwSrcLen, LPTSTR lpszDst, DWORD dwDstSize, BOOL bRepCtlChr = FALSE );

//	parse url
BOOL _vwfunc_parse_url( LPCTSTR lpcszUrl, DWORD dwUrlLen, STPARSEURL * pstParseUrl );

//	url encode / decode
INT inline _vwfunc_char_to_int( char c );
VOID _vwfunc_get_url_decode( char *str );
INT  _vwfunc_get_url_encode( LPCTSTR lpcszInput, INT nInputLen, LPTSTR lpszOutBuf, INT nOutBufLen );
BOOL _vwfunc_swap_string( CHAR * lpszString, INT nLen, INT nLeftStrLen );

//	从 URL 中获取文件信息
BOOL _vwfunc_get_fileinfo_from_url( LPCTSTR lpcszFullUri, LPTSTR lpszExt, DWORD dwESize, LPTSTR lpszFile, DWORD dwFSize );

//	运行一个阻塞的进程
BOOL _vwfunc_run_block_process( LPCTSTR lpszCmdLine );

//	打开一个 URL
BOOL _vwfunc_lauch_iebrowser( LPCTSTR lpcszUrl );



#endif // __VWFUNCBASE_HEADER__
