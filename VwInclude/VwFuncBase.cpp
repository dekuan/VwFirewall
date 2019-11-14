// VwFunc.cpp: implementation of the CVwFunc class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include "VwFuncBase.h"

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )

#include <exdisp.h>


/**
 *	@ 公共 全局函数
 *	当前环境是调试环境还是正式 RELEASE 环境
 **/
BOOL _vwfunc_is_debug()
{
	#ifdef _DEBUG
		return TRUE;
	#endif

	return FALSE;
}


/**
 *	分配一块共享内存
 */
BOOL _vwfunc_malloc_share_memory( DWORD dwSize, LPCTSTR lpcszMemName, VOID ** lppvBuffer, HANDLE hFileMap, LPBOOL lpbCreate, LPSECURITY_ATTRIBUTES lpFileMappingAttributes )
{
	//
	//	dwSize		- [in]  要分配的内存大小
	//	lpcszMemName	- [in]  共享内存名字
	//	lpvBuffer	- [out] 返回共享内存区地址
	//	hFileMap	- [out] 文件句柄
	//	lpbCreate	- [out] 是否是新创建的
	//	RETURN		- TRUE / FALSE
	//
	if ( 0 == dwSize )
	{
		return FALSE;
	}
	if ( NULL == lpcszMemName )
	{
		return FALSE;
	}

	BOOL bRet	= FALSE;


	//
	//	创建内存映射
	//
	*lppvBuffer	= NULL;
	*lpbCreate	= FALSE;

	hFileMap = OpenFileMapping( FILE_MAP_READ|FILE_MAP_WRITE, FALSE, lpcszMemName );
	if ( ! hFileMap )
	{
		//	FileMap 不存在，要创建之
		*lpbCreate = TRUE;
		hFileMap = CreateFileMapping( INVALID_HANDLE_VALUE, lpFileMappingAttributes, PAGE_READWRITE, 0, dwSize, lpcszMemName );
	}
	if ( hFileMap )
	{
		(*lppvBuffer) = (LPVOID)MapViewOfFile( hFileMap, FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, 0 );
		if ( (*lppvBuffer) )
		{
			bRet = TRUE;
		}
	}

	//	..
	return bRet;
}

/**
 *	打开一块共享内存
 */
BOOL _vwfunc_open_share_memory( LPCTSTR lpcszMemName, VOID ** lppvBuffer, HANDLE hFileMap )
{
	//
	//	lpcszMemName	- [in]  共享内存名字
	//	lpvBuffer	- [out] 返回共享内存区地址
	//	hFileMap	- [out] 文件句柄
	//	RETURN		- TRUE / FALSE
	//
	if ( NULL == lpcszMemName )
	{
		return FALSE;
	}

	BOOL bRet	= FALSE;

	//
	//	创建内存映射
	//
	*lppvBuffer	= NULL;

	hFileMap = OpenFileMapping( FILE_MAP_READ|FILE_MAP_WRITE, FALSE, lpcszMemName );
	if ( hFileMap )
	{
		(*lppvBuffer) = (LPVOID)MapViewOfFile( hFileMap, FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, 0 );
		if ( (*lppvBuffer) )
		{
			bRet = TRUE;
		}
	}
	DWORD aaa = GetLastError();

	//	..
	return bRet;
}

/**
 *	获取文件内容
 */
BOOL _vwfunc_get_file_mapbuffer( LPCTSTR lpszFilePath, LPTSTR lpszBuffer, DWORD dwSize )
{
	BOOL bRet = FALSE;
	UINT uFileLen = 0;
	
        HANDLE hfile = CreateFile( lpszFilePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0 );
	if ( INVALID_HANDLE_VALUE != hfile )
	{
		uFileLen = GetFileSize( hfile, 0 );
		if ( uFileLen > 0 )
		{
			if ( uFileLen > CONST_VWFUNCBASE_MAX_FILEMAP_LENGTH )
			{
				uFileLen = CONST_VWFUNCBASE_MAX_FILEMAP_LENGTH;
			}

			HANDLE hmap = CreateFileMapping( hfile, 0, PAGE_READONLY, 0, (DWORD)uFileLen, 0 );
			if ( hmap )
			{
				BYTE* pView = (BYTE*)MapViewOfFile( hmap, FILE_MAP_READ, 0, 0, (SIZE_T)uFileLen );
				if ( pView )
				{
					// ..
					bRet = TRUE;
					_sntprintf( lpszBuffer, dwSize-sizeof(TCHAR), "%s", (TCHAR*)pView );
					
					// close view
					UnmapViewOfFile(pView);
				}
				// close map
				CloseHandle( hmap );
			}
		}
		// close handle
		CloseHandle(hfile);
	}

	return bRet;
}

/**
 *	获取文件的 md5 值
 */
BOOL _vwfunc_get_file_md5( LPCTSTR lpszFilePath, LPTSTR lpszMd5, DWORD dwSize, LPTSTR lpszError )
{
	BOOL bRet = FALSE;
	UINT uFileLen = 0;

        HANDLE hfile = CreateFile( lpszFilePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0 );
	if ( INVALID_HANDLE_VALUE != hfile )
	{
		uFileLen = GetFileSize( hfile, 0 );
		if ( uFileLen > 0 )
		{
			if ( uFileLen > CONST_VWFUNCBASE_MAX_FILEMAP_LENGTH )
			{
				_tcscpy( lpszError, "文件大小已经超过 100M。" );
				uFileLen = CONST_VWFUNCBASE_MAX_FILEMAP_LENGTH;
			}
			HANDLE hmap = CreateFileMapping( hfile, 0, PAGE_READONLY, 0, (DWORD)uFileLen, 0 );
			if ( hmap )
			{
				BYTE* pView = (BYTE*)MapViewOfFile( hmap, FILE_MAP_READ, 0, 0, (SIZE_T)uFileLen );
				if ( pView )
				{
					//	Get MD5
					MD5_CTX	m_md5;
					unsigned char szEncrypt[ 16 ] = {0};
					
					m_md5.MD5Update( (unsigned char *)pView, uFileLen );
					m_md5.MD5Final( szEncrypt );
					
					TCHAR szHexTmp[ 32 ]		= {0};
					TCHAR szOutput[ MAX_PATH ]	= {0};
					
					for ( INT i = 0; i < sizeof(szEncrypt); i ++ )
					{
						memset( szHexTmp, 0, sizeof(szHexTmp) );
						_sntprintf( szHexTmp, sizeof(szHexTmp)-sizeof(TCHAR), "%02X", szEncrypt[i] );
						_tcscat( szOutput, szHexTmp );
					}
					
					// ..
					bRet = TRUE;
					_sntprintf( lpszMd5, dwSize-sizeof(TCHAR), _T("%s"), szOutput );
					
					// close view
					UnmapViewOfFile(pView);
				}
				else
				{
					_tcscpy( lpszError, "Failed to MapViewOfFile" );
				}
				
				// close map
				CloseHandle( hmap );
			}
			else
			{
				_tcscpy( lpszError, "CreateFileMapping" );
			}
		}
		else
		{
			_tcscpy( lpszError, "0 长度文件" );
		}
		
		// close handle
		CloseHandle(hfile);
	}
	else
	{
		_tcscpy( lpszError, "Failed to CreateFile" );
	}
	
	return bRet;
}

/**
 *	获取一个字符串的 MD5 值(32位字符串)
 */
BOOL _vwfunc_get_string_md5( LPCTSTR lpszString, LPTSTR lpszMd5, DWORD dwSize, DWORD dwSpecStringLen )
{
	/*
		lpszString	- [in]  待处理字符串
		lpszMd5		- [out] 返回 MD5 值字符串缓冲区
		dwSize		- [in]  返回 MD5 值字符串长度
		dwSpecStringLen	- [in]  指定处理 lpszString 从头开始的多少个字节
		RETURN		- TRUE / FALSE
	*/
	if ( NULL == lpszString || NULL == lpszMd5 )
		return FALSE;
	if ( dwSize < 32 )
		return FALSE;

	MD5_CTX	m_md5;
	unsigned char szEncrypt[16];
	unsigned char c;
	INT i			= 0;
	
	memset( szEncrypt, 0, sizeof(szEncrypt) );
	if ( dwSpecStringLen > 0 )
		m_md5.MD5Update( (unsigned char *)lpszString, dwSpecStringLen );
	else
		m_md5.MD5Update( (unsigned char *)lpszString, _tcslen(lpszString) );
	m_md5.MD5Final( szEncrypt );
	
	// ..
	for ( i = 0; i < 16; i++ )
	{
		c = szEncrypt[i] / 16;
		lpszMd5[i*2]	= ( c < 10 ) ? ( '0' + c ) : ( 'a' + c - 10 );
		c = szEncrypt[i] % 16;
		lpszMd5[i*2+1]	= ( c < 10 ) ? ( '0' + c ) : ( 'a' + c - 10 );
	}

	// ..
	lpszMd5[ min( 32, dwSize-sizeof(TCHAR) ) ] = 0;
	
	return TRUE;
}


/**
 *	Get Windows system type
 */
ENUMWINDOWSSYSTYPE _vwfunc_get_shellsystype()
{
	ENUMWINDOWSSYSTYPE ShellType;
	DWORD winVer;
	OSVERSIONINFO * osvi;

	winVer = GetVersion();
	if ( winVer < 0x80000000 )
	{
		// NT
		ShellType = _OS_WINDOWS_NT3;
		osvi = (OSVERSIONINFO *)malloc(sizeof(OSVERSIONINFO));
		if ( NULL != osvi )
		{
			memset( osvi, 0, sizeof(OSVERSIONINFO) );
			osvi->dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			GetVersionEx( osvi );
			if ( 4L == osvi->dwMajorVersion )
			{
				ShellType = _OS_WINDOWS_NT4;
			}
			else if ( 5L == osvi->dwMajorVersion && 0L == osvi->dwMinorVersion )
			{
				ShellType = _OS_WINDOWS_2000;
			}
			else if ( 5L == osvi->dwMajorVersion && 1L == osvi->dwMinorVersion )
			{
				ShellType = _OS_WINDOWS_XP;
			}
			else if ( 5L == osvi->dwMajorVersion && 2L == osvi->dwMinorVersion )
			{
				ShellType = _OS_WINDOWS_2003;
			}
			if ( osvi )
				free( osvi );
		}
	}
	else if ( LOBYTE(LOWORD(winVer)) < 4 )
	{
		ShellType = _OS_WINDOWS_32S;
	}
	else
	{
		ShellType = _OS_WINDOWS_95;
		osvi = (OSVERSIONINFO *)malloc(sizeof(OSVERSIONINFO));
		if ( NULL != osvi )
		{
			memset( osvi, 0, sizeof(OSVERSIONINFO) );
			osvi->dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			GetVersionEx( osvi );
			if ( 4L == osvi->dwMajorVersion && 10L == osvi->dwMinorVersion )
			{
				ShellType = _OS_WINDOWS_98;
			}
			else if ( 4L == osvi->dwMajorVersion && 90L == osvi->dwMinorVersion )
			{
				ShellType = _OS_WINDOWS_ME;
			}
			if ( osvi )
				free(osvi);
		}
	}
	
	return ShellType;
}

/**
 *	is window nt4
 */
BOOL _vwfunc_is_window_nt4()
{
	OSVERSIONINFO osif;
	osif.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx( &osif );
	if ( VER_PLATFORM_WIN32_NT == osif.dwPlatformId && osif.dwMajorVersion <= 4 )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/**
 *	获取 Windows Temp 目录
 */
BOOL _vwfunc_get_window_tempdir( LPTSTR lpszDir, DWORD dwSize )
{
	DWORD dwRet = GetTempPath( dwSize, lpszDir );
	if ( dwRet && dwRet < dwSize )
	{
		if ( '\\' != lpszDir[ _tcslen(lpszDir) - 1 ] )
		{
			lpszDir[ _tcslen(lpszDir) ] = '\\';
		}
		return TRUE;
	}
	//if ( ERROR_PATH_NOT_FOUND == GetLastError() )
	for ( CHAR c = 'c'; c < 'z'; c ++ )
	{
		_sntprintf( lpszDir, dwSize-sizeof(TCHAR), "%c:\\", c );
		if ( PathIsDirectory( lpszDir ) )
		{
			return TRUE;
		}
	}
	return FALSE;
}


/**
 *	简单加密解密
 *		xorkey	- 加密的 key，默认等于 0x79
 *		pData	- 内容，可以是明文也可以是密文
 *		不管是明文还是密文，只要送进来就可以解密或者加密
 */
VOID _vwfunc_xorenc( CHAR * pData, BYTE xorkey /*0XFF*/ )
{
	/*
		pData		- [in/out]	Pointer to a null-terminated string containing the Data
		xorkey		- [in]		Specifies the Encode/Decode key
	*/

	while( pData && *pData )
	{
		*pData = *pData ^ xorkey;
		pData++;
	}
}


/**
 *	@ public
 *	从资源里面提取文件到磁盘
 *	必须保证DestFilename是可以创建的
 */
BOOL _vwfunc_extract_file_from_resource( HINSTANCE hInst, LPCTSTR lpcszResType, UINT uResID, LPCTSTR lpszDestFilename )
{
	/*
		hInst			- [in] hInstance
		lpcszResType		- [in] 资源类型，例如："TXT"
		uResID			- [in] 资源 ID
		lpszDestFilename	- [in] 提取资源后，保存到的文件名，要求全路径

		RETURN			- TRUE / FALSE
	*/

	if ( NULL == hInst )
		return FALSE;
	if ( NULL == lpcszResType || 0 == strlen( lpcszResType ) )
		return FALSE;
	if ( NULL == lpszDestFilename || 0 == strlen( lpszDestFilename ) )
		return FALSE;


	BOOL  bRet		= FALSE;
	DWORD dwSize		= 0;
	PVOID pResBuffer	= NULL;
	HANDLE hFile		= NULL;
	DWORD dwWriten		= 0;

	pResBuffer = _vwfunc_get_resource_buffer( hInst, lpcszResType, uResID, &dwSize );
	if ( pResBuffer && dwSize > 0 )
	{
		hFile = CreateFile
			(
				lpszDestFilename, GENERIC_WRITE, 0,
				NULL,
				CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL, NULL
			);                        // no attr. template
		if ( hFile )
		{
			::WriteFile( hFile, pResBuffer, dwSize, &dwWriten, NULL );
			::CloseHandle( hFile);
			bRet = TRUE;
		}
	}

	return bRet;
}
/**
 *	@ Private
 */
VOID * _vwfunc_get_resource_buffer( HINSTANCE hInst, LPCTSTR lpcszResType, UINT uResID, DWORD * pdwSize )
{
	/*
		hInst			- [in] hInstance
		lpcszResType		- [in] 资源类型，例如："TXT"
		uResID			- [in] 资源 ID
		pdwSize			- [out] 提取资源大小

		RETURN			- TRUE / FALSE
	*/

	if ( NULL == hInst )
		return FALSE;
	if ( NULL == lpcszResType || NULL == lpcszResType[0] )
		return FALSE;

	HRSRC hRes;
	HGLOBAL hResLoad;

	hRes = FindResource( hInst, MAKEINTRESOURCE(uResID), lpcszResType ); 
	if ( hRes )
	{
		hResLoad = LoadResource( hInst, hRes );
		if ( hResLoad )
		{
			if ( pdwSize )
			{
				*pdwSize = SizeofResource( hInst, hRes );
			}
			return LockResource( hResLoad );
		}
	}
	return NULL;
}



/**
 *	检查文件是否存在
 */
BOOL _vwfunc_my_file_exists( LPCTSTR lpcszFile )
{
	/*
		lpcszFile	- [in] 文件路径
	*/

	if ( NULL == lpcszFile )
		return FALSE;
	if ( NULL == lpcszFile[0] )	//strlen
		return FALSE;


	FILE * fp = fopen( lpcszFile, "r" );
	if( fp )
	{
		fclose( fp );
		return TRUE;
	}

	return FALSE;
}

/**
 *	获取文件大小
 */
DWORD _vwfunc_get_my_filesize( LPCTSTR pszFilePath )
{
	/*
		lpcszFile	- [in] 文件路径
		RETURN		- 返回文件的字节数
	*/

	if ( NULL == pszFilePath )
		return 0;

	HANDLE hFile		= NULL;
	DWORD dwFileSize	= 0;

	hFile = CreateFile( pszFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0 );
	if ( INVALID_HANDLE_VALUE != hFile )
	{
		dwFileSize = ::GetFileSize( hFile, NULL );
		CloseHandle( hFile );
	}
	
	return dwFileSize;
}

/**
 *	从文件资源里提出版本
 */
/*
BOOL _vwfunc_get_file_version( LPCTSTR lpszFilePath, LPTSTR lpszVersion, DWORD dwSize )
{
	//
	//	lpszFilePath	- [in]  文件路径
	//	lpszVersion	- [out] 返回值，文件版本号
	//	dwSize		- [in]  返回值缓冲区大小
	//	RETURN		- TRUE / FALSE
	//

	if ( NULL == lpszFilePath )
	{
		return FALSE;
	}
	
	TCHAR szVersionBuffer[ 8192 ]	= {0};
	DWORD dwVerSize			= 0;
	DWORD dwHandle			= 0;

	dwVerSize = GetFileVersionInfoSize( lpszFilePath, &dwHandle );
	if ( 0 == dwVerSize || dwVerSize > (sizeof(szVersionBuffer)-1) )
	{
		return FALSE;
	}
	
	if ( GetFileVersionInfo( lpszFilePath, 0, dwVerSize, szVersionBuffer ) )
	{
		VS_FIXEDFILEINFO * pInfo;
		unsigned int nInfoLen;
		
		if( VerQueryValue( szVersionBuffer, "\\", (void**)&pInfo, &nInfoLen ) )
		{	
			memset( lpszVersion, 0, dwSize );
			_snprintf( lpszVersion, dwSize-1, "%d.%d.%d.%d", 
				HIWORD( pInfo->dwFileVersionMS ), LOWORD( pInfo->dwFileVersionMS ), 
				HIWORD( pInfo->dwFileVersionLS ), LOWORD( pInfo->dwFileVersionLS ) );
			return TRUE;
		}
	}

	return FALSE;
}
*/

/**
 *	给与文件可写选项
 */
BOOL _vwfunc_file_give_write_attribute( LPCTSTR lpcszFilePath )
{
	/*
		lpcszFilePath	- [in] 文件路径
	*/

	if ( NULL == lpcszFilePath )
		return FALSE;
	else if ( 0 == _tcslen(lpcszFilePath) )
		return FALSE;
	if ( FALSE == _vwfunc_my_file_exists( lpcszFilePath ) )
		return FALSE;

	DWORD dwFileAttrs	= 0;
	dwFileAttrs = GetFileAttributes( lpcszFilePath ); 
	if ( dwFileAttrs & FILE_ATTRIBUTE_READONLY )
	{
		SetFileAttributes( lpcszFilePath, (dwFileAttrs & ~FILE_ATTRIBUTE_READONLY) ); 
	}

	return TRUE;
}

/**
 *	获取 DLL 所在的全路径
 */
BOOL _vwfunc_get_dll_modulepath( LPVOID lpvInnerAddr, HINSTANCE * hInstance, LPTSTR lpszModulePath, DWORD dwSize )
{
	/*
		lpvInnerAddr		- [in]  模块(DLL)中任意一个全局函数的地址
		hInstance		- [in]  instance
		lpszModulePath		- [out] 返回值，如果成功将返回获取到的当前 DLL 所在目录的全路径
		dwSize			- [in]  返回值缓冲区大小
		RETURN			- TRUE / FALSE
	*/

	BOOL  bRet = FALSE;
	MEMORY_BASIC_INFORMATION mbi;

	__try
	{
		if ( lpvInnerAddr )
		{
			if ( VirtualQuery( lpvInnerAddr, &mbi, sizeof(mbi) ) )
			{
				if ( hInstance )
					*hInstance = (HMODULE)mbi.AllocationBase;
				// ..
				if ( GetModuleFileName( (HMODULE)mbi.AllocationBase, lpszModulePath, dwSize ) )
				{
					bRet = TRUE;
				}
			}
		}

/*
		// 唐赓的方法，容易崩溃
		pwAddrFunc = (WORD*)((DWORD)pwAddrFunc & ( ~1023 ));
		while( pwAddrFunc > (WORD*)0x400000 )
		{
			if ( *pwAddrFunc && *pwAddrFunc == 0x5a4d )
			{
				bRet = TRUE;
				GetModuleFileName( (HMODULE)pwAddrFunc, lpszModulePath, dwSize );
				break;
			}
			pwAddrFunc --;
		}
*/
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return bRet;
}


/**
 *	获取某个字符在一个字符串中的个数
 */
DWORD _vwfunc_get_chr_count( LPCTSTR lpszString, TCHAR lpChr, UINT uStringLen /* = 0 */ )
{
	/*
		lpszString	- [in] 带查询字符串
		lpChr		- [in] 要计算的字符
		RETURN		- lpszString 中含有 lpChr 的个数
	*/

	if ( NULL == lpszString || NULL == lpChr || NULL == lpszString[0] )
		return 0;

	DWORD dwCount	= 0;
	UINT  i		= 0;
	UINT  uLen	= ( 0 == uStringLen ? (UINT)_tcslen(lpszString) : uStringLen );
	
	for ( i = 0; i < uLen; i ++ )
	{
		if ( lpszString[i] && lpChr == lpszString[i] )
		{
			dwCount ++;
		}
	}
	return dwCount;
}

/**
 *	检查字符串是否是合法的 IP 地址，例如：211.99.66.14
 */
BOOL _vwfunc_is_valid_ipaddr( LPCTSTR lpszString, UINT uStringLen /* = 0 */ )
{
	/*
		lpszString	- [in] 待检测字符串
		RETURN		- TRUE/FALSE
	*/

	if ( NULL == lpszString )
		return FALSE;
	if ( NULL == lpszString[0] )	// strlen
		return FALSE;

	UINT  i		= 0;
	UINT  uLen	= ( 0 == uStringLen ? (UINT)_tcslen(lpszString) : uStringLen );

	//
	//	...
	//
	if ( '.' == lpszString[0] || '.' == lpszString[ uLen - sizeof(TCHAR) ] )
	{
		return FALSE;
	}
	if ( 3 != _vwfunc_get_chr_count( lpszString, '.', uLen ) )
	{
		return FALSE;
	}

	for ( i = 0; i < uLen; i ++ )
	{
		if ( NULL == _tcschr( CONST_VWFUNCBASE_VALIDIPADDRSTRING, lpszString[i] ) )
		{
			return FALSE;
		}
	}
	return TRUE;
}

/**
 *	检查字符串是否是合法的域名
 */
BOOL  _vwfunc_is_valid_domain( LPCTSTR lpszString )
{
	/*
		lpszString	- [in] 待检测字符串
		RETURN		- TRUE/FALSE
	*/

	if ( NULL == lpszString )
	{
		return FALSE;
	}
	if ( NULL == lpszString[0] )
	{
		return FALSE;
	}

	UINT  i		= 0;
	UINT  uLen	= (UINT)_tcslen( lpszString );

	//
	//	...
	//
	if ( '.' == lpszString[ uLen - sizeof(TCHAR) ] )
	{
		return FALSE;
	}

	for ( i = 0; i < uLen; i ++ )
	{
		if ( NULL == _tcschr( CONST_VWFUNCBASE_VALIDDOMAINSTRING, lpszString[i] ) )
		{
			return FALSE;
		}
	}
	return TRUE;
}


/**
 *	检查域名是否是国内域名
 */
static inline BOOL _vwfunc_is_inland_domain( LPCTSTR lpcszHost, UINT uHostLen /* = 0 */ )
{
	//
	//	lpcszHost	- [in] HOST
	//	uHostLen	- [in] Length of HOST
	//

	if ( NULL == lpcszHost )
	{
		return FALSE;
	}
	if ( NULL == lpcszHost[0] )	// strlen
	{
		return FALSE;
	}

	BOOL    bRet		= FALSE;
	UINT	uLen		= ( 0 == uHostLen ? (UINT)_tcslen(lpcszHost) : uHostLen );	//	计算长度
	LPCTSTR lpszMove	= NULL;
	LPCTSTR lpszTag		= NULL;
	TCHAR   szDmExt[ 16 ];


	//	"|.com.cn|.com.tw|.net.cn|.net.tw|.org.cn|.org.tw|.gov.cn|.gov.tw|"
	//	7 = _tcslen(".com.cn")
	if ( uLen > 7 )
	{
		//	取 HOST 后 7 位字符计算
		//	7 = _tcslen(".com.cn")
		lpszMove = lpcszHost + uLen - 7;
		if ( lpszMove )
		{
			//	将 HOST 后 7 位字符拷贝到 szDmExt
			//	7 = _tcslen(".com.cn")
			//	8 = _tcslen("|.com.cn")
			_sntprintf( szDmExt, sizeof(szDmExt)/sizeof(TCHAR)-1, _T("|%s"), lpszMove );
			szDmExt[ 8 ]	= '|';
			szDmExt[ 9 ]	= 0;
			_tcslwr( szDmExt );

			if ( _tcsstr( CONST_VWFUNCBASE_DOMAINEXTLIST, szDmExt ) )
			{
				bRet = TRUE;
			}

			//
			//lpszTag = _tcsstr( CONST_VWFUNCBASE_DOMAINEXTLIST, szDmExt );
			//if ( lpszTag )
			//{
			//	lpszTag --;
			//	if ( lpszTag && '|' == *lpszTag )
			//	{
			//		lpszTag += ( 1 + _tcslen(lpszMove) );
			//		if ( lpszTag && '|' == *lpszTag )
			//		{
			//			bRet = TRUE;
			//		}
			//	}
			//}
		}
	}

	return bRet;
}

/**
 *	获取 Root 域名
 */
BOOL _vwfunc_get_root_host( LPCTSTR lpcszHost, LPTSTR lpszRootHost, DWORD dwSize )
{
	/*
		lpcszHost	- [in]  待处理 HOST
		lpszRootHost	- [out] 返回 ROOT HOST
		dwSize		- [in]  返回缓冲区的大小
		RETURN		- TRUE/FALSE
	*/

	if ( NULL == lpcszHost || NULL == lpszRootHost )
		return FALSE;
	if ( NULL == lpcszHost[0] )	// strlen
		return FALSE;
	
	BOOL   bRet		= FALSE;
	UINT   uHostLen		= (UINT)_tcslen(lpcszHost);
	LPTSTR lpszMove		= NULL;
	DWORD  dwDotNum		= 0;
	INT    i		= 0;
	INT    nDot		= 0;

	if ( _vwfunc_is_valid_ipaddr( lpcszHost ) )
	{
		//	如果是一个 IP 地址，则直接返回 IP 地址，无须再计算
		//	_sntprintf( lpszRootHost, dwSize-sizeof(TCHAR), "%s", lpcszHost );
		memcpy( lpszRootHost, lpcszHost, min(uHostLen,dwSize-sizeof(TCHAR)) );
		lpszRootHost[ min(uHostLen,dwSize-sizeof(TCHAR)) ] = 0;
		return TRUE;
	}
	

	//
	//	开始计算 ROOT HOST
	//
	dwDotNum = _vwfunc_get_chr_count( lpcszHost, '.' );
	if ( 0 == dwDotNum || 1 == dwDotNum )
	{
		//
		//	类似 xxxxx.com
		//
		bRet = TRUE;
		//	_sntprintf( lpszRootHost, dwSize-sizeof(TCHAR), "%s", lpcszHost );
		memcpy( lpszRootHost, lpcszHost, min(dwSize-sizeof(TCHAR),uHostLen) );
		lpszRootHost[ min(dwSize-sizeof(TCHAR),uHostLen) ] = 0;
	}
	else if ( 2 == dwDotNum && _vwfunc_is_inland_domain(lpcszHost) )
	{
		//
		//	类似 xxxxx.com.cn
		//
		bRet = TRUE;
		//	_sntprintf( lpszRootHost, dwSize-sizeof(TCHAR), "%s", lpcszHost );
		memcpy( lpszRootHost, lpcszHost, min(dwSize-sizeof(TCHAR),uHostLen) );
		lpszRootHost[ min(dwSize-sizeof(TCHAR),uHostLen) ] = 0;
	}
	else
	{
		//
		//	3 点以上，类似 abc.ccc.xxxxx.com 或者 abc.xxxxx.com.cn
		//
		if ( _vwfunc_is_inland_domain(lpcszHost) )
		{
			//
			//	国内域名，类似 abc.xxxxx.com.cn，需要取 xxxxx.com.cn
			//
			for ( i = uHostLen-sizeof(TCHAR); i >= 0; i -- )
			{
				if ( '.' == lpcszHost[i] )
				{
					nDot ++;
					if ( 3 == nDot )
						break;
				}
			}
			bRet = TRUE;
			//	_sntprintf( lpszRootHost, dwSize-sizeof(TCHAR), "%s", lpcszHost+i+sizeof(TCHAR) );
			memcpy( lpszRootHost, lpcszHost+i+sizeof(TCHAR), min( dwSize-sizeof(TCHAR), _tcslen(lpcszHost+i+sizeof(TCHAR)) ) );
			lpszRootHost[ min( dwSize-sizeof(TCHAR), _tcslen(lpcszHost+i+sizeof(TCHAR)) ) ] = 0;
		}
		else
		{
			//
			//	类似 abc.ccc.xxxxx.com / www.xxxxx.com，需要取 xxxxx.com
			//
			for ( i = uHostLen-sizeof(TCHAR); i >= 0; i -- )
			{
				if ( '.' == lpcszHost[i] )
				{
					nDot ++;
					if ( 2 == nDot )
						break;
				}
			}
			bRet = TRUE;
			//	_sntprintf( lpszRootHost, dwSize-sizeof(TCHAR), "%s", lpcszHost+i+sizeof(TCHAR) );
			memcpy( lpszRootHost, lpcszHost+i+sizeof(TCHAR), min( dwSize-sizeof(TCHAR), _tcslen(lpcszHost+i+sizeof(TCHAR)) ) );
			lpszRootHost[ min( dwSize-sizeof(TCHAR), _tcslen(lpcszHost+i+sizeof(TCHAR)) ) ] = 0;
		}
	}
	
	return bRet;
}


/**
 *	获取 Root 域名2 跟上面函数不一样的是这个函数可能会返回 .abc.com.cn 这样的结果
 */
BOOL _vwfunc_get_root_host2( LPCTSTR lpcszHost, LPTSTR lpszRootHost, DWORD dwSize )
{
	//
	//	lpcszHost	- [in]  待处理 HOST
	//	lpszRootHost	- [out] 返回 ROOT HOST
	//	dwSize		- [in]  返回缓冲区的大小
	//	RETURN		- TRUE/FALSE
	//

	if ( NULL == lpcszHost || NULL == lpszRootHost )
		return FALSE;
	if ( NULL == lpcszHost[0] )	// strlen
		return FALSE;
	
	BOOL   bRet		= FALSE;
	UINT   uHostLen		= (UINT)_tcslen(lpcszHost);
	LPTSTR lpszMove		= NULL;
	DWORD  dwDotNum		= 0;
	INT    i		= 0;
	INT    nDot		= 0;

	if ( _vwfunc_is_valid_ipaddr( lpcszHost ) )
	{
		//	如果是一个 IP 地址，则直接返回 IP 地址，无须再计算
		//	_sntprintf( lpszRootHost, dwSize-sizeof(TCHAR), "%s", lpcszHost );
		memcpy( lpszRootHost, lpcszHost, min(uHostLen,dwSize-sizeof(TCHAR)) );
		lpszRootHost[ min(uHostLen,dwSize-sizeof(TCHAR)) ] = 0;
		return TRUE;
	}
	

	//
	//	开始计算 ROOT HOST
	//
	dwDotNum = _vwfunc_get_chr_count( lpcszHost, '.' );
	if ( 0 == dwDotNum || 1 == dwDotNum )
	{
		//
		//	类似 xxxxx.com
		//
		bRet = TRUE;
		//	_sntprintf( lpszRootHost, dwSize-sizeof(TCHAR), "%s", lpcszHost );
		memcpy( lpszRootHost, lpcszHost, min(dwSize-sizeof(TCHAR),uHostLen) );
		lpszRootHost[ min(dwSize-sizeof(TCHAR),uHostLen) ] = 0;
	}
	else if ( 2 == dwDotNum && _vwfunc_is_inland_domain(lpcszHost) )
	{
		//
		//	类似 xxxxx.com.cn
		//
		bRet = TRUE;
		//	_sntprintf( lpszRootHost, dwSize-sizeof(TCHAR), "%s", lpcszHost );
		memcpy( lpszRootHost, lpcszHost, min(dwSize-sizeof(TCHAR),uHostLen) );
		lpszRootHost[ min(dwSize-sizeof(TCHAR),uHostLen) ] = 0;
	}
	else
	{
		//
		//	3 点以上，类似 abc.ccc.xxxxx.com 或者 abc.xxxxx.com.cn
		//
		if ( _vwfunc_is_inland_domain(lpcszHost) )
		{
			//
			//	国内域名，类似 abc.xxxxx.com.cn，需要取 xxxxx.com.cn
			//
			for ( i = uHostLen-sizeof(TCHAR); i >= 0; i -- )
			{
				if ( '.' == lpcszHost[i] )
				{
					nDot ++;
					if ( 3 == nDot )
						break;
				}
			}
			bRet = TRUE;
			//	_sntprintf( lpszRootHost, dwSize-sizeof(TCHAR), "%s", lpcszHost+i+sizeof(TCHAR) );
			memcpy( lpszRootHost, lpcszHost+i, min( dwSize-sizeof(TCHAR), _tcslen(lpcszHost+i) ) );
			lpszRootHost[ min( dwSize-sizeof(TCHAR), _tcslen(lpcszHost+i) ) ] = 0;
		}
		else
		{
			//
			//	类似 abc.ccc.xxxxx.com / www.xxxxx.com，需要取 xxxxx.com
			//
			for ( i = uHostLen-sizeof(TCHAR); i >= 0; i -- )
			{
				if ( '.' == lpcszHost[i] )
				{
					nDot ++;
					if ( 2 == nDot )
						break;
				}
			}
			bRet = TRUE;
			//	_sntprintf( lpszRootHost, dwSize-sizeof(TCHAR), "%s", lpcszHost+i+sizeof(TCHAR) );
			memcpy( lpszRootHost, lpcszHost+i, min( dwSize-sizeof(TCHAR), _tcslen(lpcszHost+i) ) );
			lpszRootHost[ min( dwSize-sizeof(TCHAR), _tcslen(lpcszHost+i) ) ] = 0;
		}
	}
	
	return bRet;
}

LPCTSTR _vwfunc_get_root_host_ptr( LPCTSTR lpcszHost )
{
	//
	//	lpcszHost	- [in]  待处理 HOST
	//	RETURN		- TRUE/FALSE
	//
	
	if ( NULL == lpcszHost )
	{
		return NULL;
	}
	if ( NULL == lpcszHost[0] )
	{
		return NULL;
	}

	BOOL   bRet		= FALSE;
	UINT   uHostLen;
	LPTSTR lpszMove		= NULL;
	DWORD  dwDotNum		= 0;
	INT    i		= 0;
	INT    nDot		= 0;


	uHostLen = (UINT)_tcslen(lpcszHost);

	if ( _vwfunc_is_valid_ipaddr( lpcszHost ) )
	{
		//	如果是一个 IP 地址，则直接返回 IP 地址，无须再计算
		//	_sntprintf( lpszRootHost, dwSize-sizeof(TCHAR), "%s", lpcszHost );
		return lpcszHost;
	}
	
	
	//
	//	开始计算 ROOT HOST
	//
	dwDotNum = _vwfunc_get_chr_count( lpcszHost, '.' );
	if ( 0 == dwDotNum || 1 == dwDotNum )
	{
		//
		//	类似 xxxxx.com
		//
		return lpcszHost;
	}
	else if ( 2 == dwDotNum && _vwfunc_is_inland_domain(lpcszHost) )
	{
		//
		//	类似 xxxxx.com.cn
		//
		return lpcszHost;
	}
	else
	{
		//
		//	3 点以上，类似 abc.ccc.xxxxx.com 或者 abc.xxxxx.com.cn
		//
		if ( _vwfunc_is_inland_domain(lpcszHost) )
		{
			//
			//	国内域名，类似 abc.xxxxx.com.cn，需要取 xxxxx.com.cn
			//
			for ( i = uHostLen-sizeof(TCHAR); i >= 0; i -- )
			{
				if ( '.' == lpcszHost[i] )
				{
					nDot ++;
					if ( 3 == nDot )
						break;
				}
			}

			return lpcszHost+i;
		}
		else
		{
			//
			//	类似 abc.ccc.xxxxx.com / www.xxxxx.com，需要取 xxxxx.com
			//
			for ( i = uHostLen-sizeof(TCHAR); i >= 0; i -- )
			{
				if ( '.' == lpcszHost[i] )
				{
					nDot ++;
					if ( 2 == nDot )
						break;
				}
			}

			return lpcszHost+i;
		}
	}
	
	return NULL;
}
LPCTSTR _vwfunc_get_root_host_ptr_ex( LPCTSTR lpcszHost, UINT * uRetLen )
{
	//
	//	lpcszHost	- [in]  待处理 HOST，可能是 "http://" 或者 "https://" 开头的
	//	uRetLen		- [out] 返回 HOST 的长度，如果函数返回 NULL，则该值是 0
	//	RETURN		- TRUE/FALSE
	//

	if ( NULL == lpcszHost )
	{
		return NULL;
	}
	if ( NULL == lpcszHost[0] )
	{
		return NULL;
	}
	if ( NULL == uRetLen )
	{
		return NULL;
	}

	LPCTSTR lpcszRet	= NULL;
	UINT   uHostLen;
	LPCTSTR lpcszHostStart	= NULL;
	LPTSTR lpszHostEnd	= NULL;
	LPTSTR lpszPortMark	= NULL;
	LPTSTR lpszMove		= NULL;
	DWORD  dwDotNum		= 0;
	INT    i		= 0;
	INT    nDot		= 0;


	lpcszRet	= NULL;
	*uRetLen	= 0;
	lpcszHostStart	= lpcszHost;

	if ( 0 == _tcsnicmp( lpcszHostStart, _T("http://"), 7 ) )
	{
		lpcszHostStart += 7;
	}
	if ( 0 == _tcsnicmp( lpcszHostStart, _T("https://"), 8 ) )
	{
		lpcszHostStart += 8;
	}

	lpszPortMark	= _tcschr( lpcszHostStart, ':' );
	lpszHostEnd	= _tcschr( lpcszHostStart, '/' );
	if ( lpszHostEnd )
	{
		//
		//	TODO:
		//	未处理 "http://username:password@www.url.com:800/abc.html" 的情况
		//
		//	处理可能带端口的情况
		//	"http://www.url.com:800/abc.html"
		//
		if ( lpszPortMark && lpszPortMark < lpszHostEnd )
		{
			//	确定存在端口符号，并且位置正确
			lpszHostEnd = lpszPortMark;
		}
		//	123/
		uHostLen = (UINT)( lpszHostEnd - lpcszHostStart );
	}
	else
	{
		//
		//	TODO:
		//	未处理 "http://username:password@www.url.com:800" 的情况
		//
		//	处理可能带端口的情况
		//	"http://www.url.com:800"
		//
		if ( lpszPortMark )
		{
			uHostLen = (UINT)( lpszPortMark - lpcszHostStart );
		}
		else
		{
			uHostLen = (UINT)_tcslen(lpcszHostStart);
		}
	}


	if ( _vwfunc_is_valid_ipaddr( lpcszHostStart, uHostLen ) )
	{
		//	如果是一个 IP 地址，则直接返回 IP 地址，无须再计算
		//	_sntprintf( lpszRootHost, dwSize-sizeof(TCHAR), "%s", lpszHostStart );
		lpcszRet	= lpcszHostStart;
		*uRetLen	= uHostLen;
	}
	else
	{
		//
		//	开始计算 ROOT HOST
		//
		dwDotNum = _vwfunc_get_chr_count( lpcszHostStart, '.', uHostLen );
		if ( 0 == dwDotNum || 1 == dwDotNum )
		{
			//
			//	类似 xxxxx.com
			//
			lpcszRet	= lpcszHostStart;
			*uRetLen	= uHostLen;
		}
		else if ( 2 == dwDotNum && _vwfunc_is_inland_domain( lpcszHostStart, uHostLen ) )
		{
			//
			//	类似 xxxxx.com.cn
			//
			lpcszRet	= lpcszHostStart;
			*uRetLen	= uHostLen;
		}
		else
		{
			//
			//	3 点以上，类似 abc.ccc.xxxxx.com 或者 abc.xxxxx.com.cn
			//
			if ( _vwfunc_is_inland_domain( lpcszHostStart, uHostLen ) )
			{
				//
				//	国内域名，类似 abc.xxxxx.com.cn，需要取 xxxxx.com.cn
				//
				for ( i = uHostLen-sizeof(TCHAR); i >= 0; i -- )
				{
					if ( '.' == lpcszHostStart[i] )
					{
						nDot ++;
						if ( 3 == nDot )
						{
							break;
						}
					}
				}

				lpcszRet	= lpcszHostStart + i;
				*uRetLen	= uHostLen - i;
			}
			else
			{
				//
				//	类似 abc.ccc.xxxxx.com / www.xxxxx.com，需要取 xxxxx.com
				//
				for ( i = uHostLen-sizeof(TCHAR); i >= 0; i -- )
				{
					if ( '.' == lpcszHostStart[i] )
					{
						nDot ++;
						if ( 2 == nDot )
						{
							break;
						}
					}
				}

				lpcszRet	= lpcszHostStart + i;
				*uRetLen	= uHostLen - i;
			}
		}
	}	
	
	return lpcszRet;
}



/**
 *	从 URL 中提取 HOST
 */
BOOL _vwfunc_get_host_from_url( LPCTSTR lpcszUrl, LPTSTR lpszHost, DWORD dwSize )
{
	//
	//	lpcszUrl	- [in]  URL : must start with "http://..."
	//	lpszHost	- [out] 返回 HOST
	//	dwSize		- [in]  返回 HOST 缓冲区大小
	//

	if ( NULL == lpcszUrl || NULL == lpszHost || NULL == lpcszUrl[0] )
	{
		return FALSE;
	}

	BOOL bRet		= FALSE;
	LPTSTR lpszHead		= NULL;
	LPTSTR lpszTail		= NULL;

	__try
	{
		// find "://"
		lpszHead = (LPTSTR)strchr( lpcszUrl, ':' );
		if ( lpszHead )
		{
			lpszHead = CharNext( lpszHead );
			if ( lpszHead && '/' == *lpszHead )
			{
				lpszHead = CharNext( lpszHead );
				if ( lpszHead && '/' == *lpszHead )
				{
					lpszHead = CharNext(lpszHead);
					if ( lpszHead )
					{
						bRet		= TRUE;
						lpszTail	= strchr( lpszHead, '/' );
						if ( lpszTail )
						{
							memcpy( lpszHost, lpszHead, (size_t)( min( lpszTail-lpszHead, dwSize-1 ) ) );
							lpszHost[ min( lpszTail-lpszHead, dwSize-1 ) ] = 0;
						}
						else
						{
							memcpy( lpszHost, lpszHead, (size_t)( min( _tcslen(lpszHead), dwSize-1 ) ) );
							lpszHost[ min( _tcslen(lpszHead), dwSize-1 ) ] = 0;
						}
					}
				}
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return bRet;
}

/**
 *	获取 URL 参数里的参数的值
 */
BOOL _vwfunc_get_request_query_string( LPCTSTR lpcszUrl, LPCTSTR lpcszName, LPTSTR lpszValue, DWORD dwSize )
{
	/*
		lpcszUrl	- [in]  URL
		lpcszName	- [in]  参数名称
		lpszValue	- [out] 返回值
		dwSize		- [in]  返回值的缓冲区大小
	*/

	if ( NULL == lpcszUrl || NULL == lpcszName || NULL == lpszValue )
		return FALSE;

	BOOL	bRet		= FALSE;
	LPTSTR	lpszHead	= NULL;
	LPTSTR	lpszTail	= NULL;
	INT	nLen		= 0;

	lpszHead = (LPTSTR)_tcsstr( lpcszUrl, lpcszName );
	if ( lpszHead )
	{
		lpszHead += _tcslen(lpcszName);
		if ( lpszHead )
		{
			if ( '=' == *lpszHead )
			{
				lpszHead = CharNext(lpszHead);
				if ( lpszHead )
				{
					//lpszTail = _tcschr( lpszHead, '&' );
					lpszTail = strpbrk( lpszHead, " &\r\n" );
					if ( lpszTail )
						nLen = (INT)min( lpszTail-lpszHead, dwSize-1 );
					else
						nLen = (INT)min( _tcslen(lpszHead), dwSize-1 );

					bRet = TRUE;
					memcpy( lpszValue, lpszHead, nLen );
					lpszValue[ nLen ] = 0;
				}
			}
		}
	}

	return bRet;
}

/**
 *	获取指定 Cookie Value
 */
BOOL _vwfunc_get_spec_cookie_value( LPCTSTR lpszCookieString, LPCTSTR lpszCookieName, LPTSTR lpszValue, DWORD dwSize )
{
	/*
		lpszCookieString	- [in]  整个 COOKIE 的字符串
		lpszCookieName		- [in]  COOKIE 名称
		lpszValue		- [out] 返回值
		dwSize			- [in]  返回值缓冲区大小
		RETURN			- TRUE / FALSE
	*/

	if ( NULL == lpszCookieString || NULL == lpszCookieName || NULL == lpszValue )
	{
		return FALSE;
	}
	if ( NULL == lpszCookieString[0] || NULL == lpszCookieName[0] )
	{
		return FALSE;
	}
	
	BOOL	bRet		= FALSE;
	LPTSTR	lpszHead	= NULL;
	LPTSTR	lpszMove	= NULL;
	LONG	uValueLen	= 0;
	
	lpszMove = (LPTSTR)_tcsstr( lpszCookieString, lpszCookieName );
	if ( lpszMove )
	{
		lpszHead = lpszMove + _tcslen(lpszCookieName);
		if ( lpszHead )
		{
			//
			//	拷贝内容到返回值缓冲区
			//
			memcpy( lpszValue, lpszHead, dwSize-sizeof(TCHAR) );

			//
			//	处理尾部
			//
			lpszMove = strpbrk( lpszValue, " ;\r\n" );
			if ( lpszMove )
			{
				*lpszMove = 0;
			}

			bRet = TRUE;
		}
	}
	return bRet;
}

/**
 *	替换不可见的控制符号为“_”
 */
TCHAR * _vwfunc_replace_controlchars( LPSTR lpszStr, UINT uLen )
{
	/*
		lpszStr		- [in]
		uLen		- [in]
	*/

	if ( NULL == lpszStr )
	{
		return NULL;
	}

	unsigned char *s = (unsigned char *)lpszStr;
	unsigned char *e = (unsigned char *)lpszStr + uLen;
	
	while ( s < e )
	{
		if ( iscntrl( *s ) )
		{
			*s = '_';
		}
		s++;
	}
	
	return lpszStr;
}

/**
 *	拷贝字符串
 */
inline BOOL _vwfunc_memcpy( LPCTSTR lpcszSrc, DWORD dwSrcLen, LPTSTR lpszDst, DWORD dwDstSize, BOOL bRepCtlChr /*= FALSE*/ )
{
	if ( NULL == lpcszSrc || NULL == lpszDst )
		return FALSE;
	if ( 0 == dwSrcLen || 0 == dwDstSize )
		return FALSE;

	UINT uLen = min( dwSrcLen, dwDstSize-sizeof(TCHAR) );

	memcpy( lpszDst, lpcszSrc, uLen );
	lpszDst[ uLen ] = 0;
	
	if ( bRepCtlChr )
	{
		_vwfunc_replace_controlchars( lpszDst, uLen );
	}

	return TRUE;
}

/**
 *	parse url
 */
BOOL _vwfunc_parse_url( LPCTSTR lpcszUrl, DWORD dwUrlLen, STPARSEURL * pstParseUrl )
{
	/*
		lpcszUrl		- [in]  URL
		dwUrlLen		- [in]  URL 长度
		pstParseUrl		- [out] 返回值
		RETURN			- TRUE / FALSE
	*/

	if ( NULL == lpcszUrl || 0 == lpcszUrl[0] )
		return FALSE;
	if ( NULL == pstParseUrl )
		return FALSE;


	BOOL bRet	= FALSE;
	UINT uBufLen	= 0;

	char port_buf[6];
//	php_url * ret = (php_url*)calloc( 1, sizeof(php_url) );
	char const *s, *e, *p, *pp, *ue;

	s = lpcszUrl;
	ue = s + dwUrlLen;

	//
	//	parse scheme
	//
	if ( ( e = (char*)memchr( s, ':', dwUrlLen ) ) && ( e - s ) )
	{
		//	validate scheme
		p = s;
		while ( p < e )
		{
			//	scheme = 1*[ lowalpha | digit | "+" | "-" | "." ]
			if ( ! isalpha(*p) && !isdigit(*p) && *p != '+' && *p != '.' && *p != '-')
			{
				if ( e + 1 < ue )
				{
					goto parse_port;
				}
				else
				{
					goto just_path;
				}
			}
			p++;
		}

		if ( *( e + 1 ) == '\0' )
		{
			//	only scheme is available
			_vwfunc_memcpy( s, ( e - s ), pstParseUrl->szScheme, sizeof( pstParseUrl->szScheme ), TRUE );
			goto end;
		}

		/**
		 *	certain schemas like mailto: and zlib: may not have any / after them
		 *	this check ensures we support those.
		 */
		if ( *(e+1) != '/' )
		{
			/**
			 *	check if the data we get is a port this allows us to 
			 *	correctly parse things like a.com:80
			 */
			p = e + 1;
			while ( isdigit(*p) )
			{
				p++;
			}

			if ( (*p) == '\0' || *p == '/' )
			{
				goto parse_port;
			}

			_vwfunc_memcpy( s, (e-s), pstParseUrl->szScheme, sizeof( pstParseUrl->szScheme ), TRUE );

			dwUrlLen -= ++e - s;
			s = e;
			goto just_path;
		}
		else
		{
			_vwfunc_memcpy( s, (e-s), pstParseUrl->szScheme, sizeof( pstParseUrl->szScheme ), TRUE );

			if ( *(e+2) == '/' )
			{
				s = e + 3;
				if ( 0 == strnicmp( "file", pstParseUrl->szScheme, sizeof("file") ) )
				{
					if ( *(e + 3) == '/' )
					{
						/**
						 *	support windows drive letters as in:
						 *	file:///c:/somedir/file.txt
						 */
						if ( *(e + 5) == ':' )
						{
							s = e + 4;
						}
						goto nohost;
					}
				}
			}
			else
			{
				if ( 0 == strnicmp( "file", pstParseUrl->szScheme, sizeof("file") ) )
				{
					s = e + 1;
					goto nohost;
				}
				else
				{
					dwUrlLen -= ++e - s;
					s = e;
					goto just_path;
				}	
			}
		}
	}
	else if (e)
	{
		//
		//	no scheme, look for port
		//
		parse_port:
		p = e + 1;
		pp = p;

		while ( pp-p < 6 && isdigit(*pp) )
		{
			pp++;
		}

		if ( pp - p < 6 && (*pp == '/' || *pp == '\0') )
		{
			memcpy( port_buf, p, (pp-p) );
			port_buf[pp-p] = '\0';
			pstParseUrl->uPort = atoi(port_buf);
		}
		else
		{
			goto just_path;
		}
	}
	else
	{
		just_path:
		ue = s + dwUrlLen;
		goto nohost;
	}
	
	e = ue;
	
	if ( ! ( p = (char*)memchr(s, '/', (ue - s)) ) )
	{
		if ( ( p = (char*)memchr(s, '?', (ue - s)) ) )
		{
			e = p;
		}
		else if ( ( p = (char*)memchr(s, '#', (ue - s) ) ) )
		{
			e = p;
		}
	}
	else
	{
		e = p;
	}	

	//
	//	check for login and password
	//
	if ( ( p = (char*)memchr(s, '@', (e-s)) ) )
	{
		if ( ( pp = (char*)memchr(s, ':', (p-s)) ) )
		{
			if ( ( pp - s ) > 0 )
			{
				//ret->user = zend_strndup( s, (pp-s) );
				_vwfunc_memcpy( s, (pp-s), pstParseUrl->szUser, sizeof( pstParseUrl->szUser ), TRUE );
			}

			pp++;
			if ( p - pp > 0 )
			{
				//ret->pass = zend_strndup( pp, (p-pp) );
				_vwfunc_memcpy( pp, (p-pp), pstParseUrl->szPass, sizeof( pstParseUrl->szPass ), TRUE );
			}
		}
		else
		{
			//ret->user = zend_strndup(s, (p-s));
			_vwfunc_memcpy( s, (p-s), pstParseUrl->szUser, sizeof( pstParseUrl->szUser ), TRUE );
		}

		s = p + 1;
	}

	//
	//	check for port
	//
	if ( *s == '[' && *(e-1) == ']' )
	{
		/*
			Short circuit portscan, 
			we're dealing with an 
			IPv6 embedded address
		*/
		p = s;
	}
	else
	{
		/*
			memrchr is a GNU specific extension
			Emulate for wide compatability
		*/
		for ( p = e; *p != ':' && p >= s; p-- );
		{
		}
	}

	if ( p >= s && *p == ':' )
	{
		if ( ! pstParseUrl->uPort )
		{
			p ++;
			if ( e - p > 5 )
			{
				//	port cannot be longer then 5 characters
				//STR_FREE( ret->scheme );
				//STR_FREE( ret->user );
				//STR_FREE( ret->pass );
				//free( ret );
				return FALSE;
			}
			else if ( e - p > 0 )
			{
				memcpy(port_buf, p, (e-p));
				port_buf[e-p] = '\0';
				pstParseUrl->uPort = atoi(port_buf);
			}
			p--;
		}	
	}
	else
	{
		p = e;
	}
	
	//
	//	check if we have a valid host, if we don't reject the string as url
	//
	if ( ( p - s ) < 1 )
	{
		//STR_FREE(ret->scheme);
		//STR_FREE(ret->user);
		//STR_FREE(ret->pass);
		//free(ret);
		return FALSE;
	}

	//ret->host = estrndup(s, (p-s));
	_vwfunc_memcpy( s, (p-s), pstParseUrl->szHost, sizeof( pstParseUrl->szHost ), TRUE );

	if ( e == ue )
	{
		return TRUE;
	}

	s = e;
	
	nohost:
	
	if ( ( p = (char*)memchr( s, '?', (ue - s) ) ) )
	{
		pp = strchr( s, '#' );
		
		if ( pp && pp < p )
		{
			p = pp;
			pp = strchr( pp + 2, '#' );
		}

		if ( p - s )
		{
			//ret->path = estrndup(s, (p-s));
			_vwfunc_memcpy( s, (p-s), pstParseUrl->szPath, sizeof( pstParseUrl->szPath ), TRUE );
		}	

		if ( pp )
		{
			if ( pp - ++p )
			{
				//ret->query = estrndup(p, (pp-p));
				_vwfunc_memcpy( p, (pp-p), pstParseUrl->szQuery, sizeof( pstParseUrl->szQuery ), TRUE );
			}
			p = pp;
			goto label_parse;
		}
		else if ( ++p - ue )
		{
			//ret->query = estrndup(p, (ue-p));
			_vwfunc_memcpy( p, (ue-p), pstParseUrl->szQuery, sizeof( pstParseUrl->szQuery ), TRUE );
		}
	}
	else if ( ( p = (char*)memchr(s, '#', (ue - s)) ) )
	{
		if ( p - s )
		{
			//ret->path = estrndup(s, (p-s));
			_vwfunc_memcpy( s, (p-s), pstParseUrl->szPath, sizeof( pstParseUrl->szPath ), TRUE );
		}

		label_parse:
		p++;

		if ( ue - p )
		{
			//ret->fragment = estrndup(p, (ue-p));
			_vwfunc_memcpy( p, (ue-p), pstParseUrl->szFragment, sizeof( pstParseUrl->szFragment ), TRUE );
		}
	}
	else
	{
		//ret->path = estrndup(s, (ue-s));
		_vwfunc_memcpy( s, (ue-s), pstParseUrl->szPath, sizeof( pstParseUrl->szPath ), TRUE );
	}
end:
	return TRUE;
}

/**
 *	url decode
 */
INT inline _vwfunc_char_to_int( char c )
{
	if ( (c >= '0') && (c <= '9') )
		return (c - '0');
	else if ( (c >= 'A') && (c <= 'F') )
		return (c - 'A' + 10);
	else if ( (c >= 'a') && (c <= 'f') )
		return (c - 'a' + 10);
	else
		return 0;
}
VOID _vwfunc_get_url_decode( char *str )
{
	char *pDest = str;
	char *pSrc = str;
	int n;
	
	while ( pSrc && *pSrc )
	{
		if ( '+' == *pSrc )
		{
			*pDest ++ = ' ';
			pSrc ++;
		}
		else if ( '%' == *pSrc )
		{
			if ( ( '\0' != *(pSrc+1) ) && ( '\0' != *(pSrc+2) ) )
			{
				n = _vwfunc_char_to_int(*(pSrc+1)) << 4;
				n += _vwfunc_char_to_int(*(pSrc+2));
				*pDest++ = (char)n;
				pSrc += 3;
			}
			else
			{
				*pDest++ = *pSrc++;
			}
		}
		else
		{
			*pDest++ = *pSrc++;
		}
	}
	if ( pDest )
		*pDest = '\0';
}

/*
	url encode - 对输入字符串做FORM编码

	函数参数：
		strInput	为输入串，不要包括url的非form部分
		nInputLen	为输入串的长度，-1表示串以 '\0' 结尾
		strOutBuf	为输出串缓冲区
		nOutBufLen	为输出缓冲区大小，包括结尾的 '\0'

	返回值：
		输出串的长度，不包括结尾的0

	如果 nOutBufLen 的长度为 0，则返回需要的输出缓冲区的大小，包括结尾的0
*/
INT _vwfunc_get_url_encode( LPCTSTR lpcszInput, INT nInputLen, LPTSTR lpszOutBuf, INT nOutBufLen )
{
	unsigned char cCur;
	TCHAR * pDst		= NULL;
	INT   i			= 0;
	INT   nDstLen		= 0;
	TCHAR szBuf[4]		= {0};
	INT   nBufLen		= 0;

	__try
	{
		if ( -1 == nInputLen )
		{
			nInputLen = (INT)_tcslen(lpcszInput);
		}

		if ( 0 == nOutBufLen )
		{
			nDstLen = 0;
			for ( i = 0; i < nInputLen; i++ )
			{
				cCur = lpcszInput[i];
				if ( ' ' == cCur )
					nDstLen++;
				else if ( !isalnum(cCur) )
					nDstLen += 3;
				else
					nDstLen ++;
			}
			return nDstLen + 1;
		}

		pDst = lpszOutBuf;
		nDstLen = 0;
		for ( i = 0; i < nInputLen; i++ )
		{
			cCur = lpcszInput[i];
			if ( ' ' == cCur )
			{
				*szBuf = '+';
				*(szBuf+1) = 0;
			}
			else if ( !isalnum(cCur) )
				sprintf( szBuf, _T("%%%2.2X"), cCur );
			else
			{
				*szBuf = cCur;
				*(szBuf+1) = 0;
			}
			nBufLen = (INT)_tcslen( szBuf );
			if ( nBufLen + nDstLen >= nOutBufLen )
				break;
			_tcscpy( pDst, szBuf );
			nDstLen += nBufLen;
			pDst += nBufLen;
		}
		return (INT)_tcslen(lpszOutBuf);
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		return 0;
	}
}


/**
 *	交换字符串处理
 */
BOOL _vwfunc_swap_string( CHAR * lpszString, INT nLen, INT nLeftStrLen )
{
	//
	//	lpszString	- [in] 输入源字符串
	//	uLen		- [in] 输入源字符串的长度
	//	uLeftStrLen	- [in] 左边需要移动的字符串的长度
	//
	INT i, j, k;
	CHAR cTemp;

	if ( NULL == lpszString || 0 == nLen || nLeftStrLen >= nLen )
	{
		return FALSE;
	}	

	__try
	{
		for ( i = nLeftStrLen, k = 1;
			i < nLen;
			i++, k++ )
		{
			for ( j = i; j >= k ; j -- )
			{
				cTemp			= lpszString[ j ];
				lpszString[ j ]		= lpszString[ j - 1 ];
				lpszString[ j - 1 ]	= cTemp;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return TRUE;
}


/**
*	获取 URL 中的文件信息
*/
BOOL _vwfunc_get_fileinfo_from_url( LPCTSTR lpcszFullUri, LPTSTR lpszExt, DWORD dwESize, LPTSTR lpszFile, DWORD dwFSize )
{
	/*
		lpcszFullUri	- [in]  URI
		lpszExt		- [out] 文件扩展名
		dwESize		- [in]  文件扩展名缓冲区大小
		lpszFile	- [out] 文件名
		dwFSize		- [in]  文件名缓冲区大小
	*/
	
	INT	i			= 0;
	INT	nDotStartPos		= 0;
	INT	nFileStartPos		= 0;
	INT	nEndPos			= 0;
	BOOL	bFindDot		= FALSE;
	BOOL	bFindFile		= FALSE;

	if ( NULL == lpcszFullUri || NULL == lpszExt )
		return FALSE;
	
	// in:  "/test/go/aaa.exe?fp=xxx"
	// out: ".exe"
	nEndPos = (INT)_tcslen(lpcszFullUri)-sizeof(TCHAR);
	for ( i = nEndPos; i >= 0; i -- )
	{
		if ( '?' == lpcszFullUri[i] || '#' == lpcszFullUri[i] )
		{
			nEndPos = i - 1;	// 得到扩展名最后一个字符得位置
			break;
		}
	}

	for ( i = nEndPos; i >= 0; i -- )
	{
		if ( ! bFindDot && '.' == lpcszFullUri[i] )
		{
			bFindDot = TRUE;
			nDotStartPos = i;		//	得到第一个字符得位置
		}
		if ( ! bFindFile && '/' == lpcszFullUri[i] )
		{
			bFindFile = TRUE;
			nFileStartPos = i;		//	得到第一个字符得位置
			break;
		}
	}
	
	//
	//	扩展名
	//
	if ( bFindDot )
	{
		memcpy( lpszExt, lpcszFullUri+nDotStartPos, min(nEndPos-nDotStartPos+1,dwESize-sizeof(TCHAR)) );
		lpszExt[ min(nEndPos-nDotStartPos+1,dwESize-sizeof(TCHAR)) ] = 0;
	}
	
	//
	//	文件名
	//
	if ( bFindFile )
	{
		memcpy( lpszFile, lpcszFullUri+nFileStartPos+sizeof(TCHAR), min( nEndPos-nFileStartPos, dwFSize-sizeof(TCHAR) ) );
		lpszFile[ min( nEndPos-nFileStartPos, dwFSize-sizeof(TCHAR) ) ] = 0;
	}
	
	return lpszExt[0] ? TRUE : FALSE;
}





/**
*	运行一个阻塞的进程
*/
BOOL _vwfunc_run_block_process( LPCTSTR lpszCmdLine )
{
	if ( NULL == lpszCmdLine )
		return FALSE;
	
	BOOL bRet = FALSE;
	BOOL bCreateProcess = FALSE;
	DWORD dweExitCode = 0;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	
	memset( &si, 0, sizeof(si) );
	si.cb = sizeof(si);
	memset( &pi, 0, sizeof(pi) );
	
	// Start the child process. 
	bCreateProcess = CreateProcess
	(
		NULL,			// No module name (use command line). 
		(TCHAR*)lpszCmdLine,		// Command line. 
		NULL,			// Process handle not inheritable. 
		NULL,			// Thread handle not inheritable. 
		FALSE,			// Set handle inheritance to FALSE. 
		0,			// No creation flags. 
		NULL,			// Use parent's environment block. 
		NULL,			// Use parent's starting directory. 
		&si,			// Pointer to STARTUPINFO structure.
		&pi			// Pointer to PROCESS_INFORMATION structure.
	);
	if ( bCreateProcess )
	{
		bRet = TRUE;
		// Wait until child process exits.
		if ( pi.hProcess )
		{
			if ( WAIT_OBJECT_0 == 
				WaitForSingleObject( pi.hProcess, INFINITE ) )
			{
				GetExitCodeProcess( pi.hProcess, &dweExitCode );
			}
		}
		// Close process and thread handles. 
		if ( pi.hProcess )
			CloseHandle( pi.hProcess );
		if ( pi.hThread )
			CloseHandle( pi.hThread );
	}
	
	return bRet;
}


/**
 *	打开一个 URL
 */
BOOL _vwfunc_lauch_iebrowser( LPCTSTR lpcszUrl )
{
	__try
	{
		IWebBrowser2 * pBrowserApp = NULL;
		WCHAR wszUrlPage[1024];
		VARIANT vtEmpty;

		CoInitialize( NULL );

		if( FAILED( CoCreateInstance(CLSID_InternetExplorer, NULL, CLSCTX_SERVER, IID_IWebBrowser2, (void**)&pBrowserApp) ) )
		{
			CoUninitialize();
			return FALSE;
		}
		
		if (pBrowserApp != NULL)
		{
			// show the browser.
			HWND hwnd = NULL;
#ifdef _AMD64__
			pBrowserApp->get_HWND( (SHANDLE_PTR *)&hwnd );
#else
			pBrowserApp->get_HWND( (long *)&hwnd );
#endif
			if ( hwnd != NULL )
			{
				::SetForegroundWindow(hwnd); 
				//::ShowWindow(hwnd, SW_SHOWMAXIMIZED);
				::ShowWindow(hwnd, SW_SHOW);
			}
			
			// visit the url.
			if ( strlen( lpcszUrl ) > 0 )
			{
				memset( wszUrlPage,0,sizeof(wszUrlPage) );
				MultiByteToWideChar(CP_ACP, 0, lpcszUrl, -1, wszUrlPage, (sizeof(wszUrlPage)/sizeof(WCHAR))-1);
				
				VARIANT varUrl;
				vtEmpty.vt = VT_EMPTY;
				varUrl.vt = VT_BSTR;
				varUrl.bstrVal = SysAllocString(wszUrlPage);
				HRESULT hr = pBrowserApp->Navigate2(&varUrl, &vtEmpty, &vtEmpty, &vtEmpty, &vtEmpty);
				SysFreeString( varUrl.bstrVal ); 
			}
			pBrowserApp->Release();
		}
		CoUninitialize();
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		CoUninitialize();
		return FALSE;
	}
	return TRUE;
}
