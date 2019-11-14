// DeLib.cpp: implementation of the CDeLib class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeLibFunc.h"

#include "crc32.h"
#include "MultiString.h"
#include "md5.h"

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )

//	for :: _delib_get_file_version
#pragma comment( lib, "Version.lib" )



/**
 *	是否是调试环境
 */
BOOL _delib_is_debug()
{
	#ifdef _DEBUG
		return TRUE;
	#endif

	return FALSE;
}

/**
 *	获取处理器个数
 */
DWORD _delib_get_processor_number()
{
	SYSTEM_INFO sysinfo;
	GetSystemInfo( &sysinfo );
	return sysinfo.dwNumberOfProcessors;
}


/**
 *	WINDOWS NT/98下安全关机或注销的调用涵数
 */
VOID _delib_shutdown_system( UINT ShutdownFlag )
{
	OSVERSIONINFO oi;
	oi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx( &oi );

	//	如果是 NT/2000 下需先设置相关权限
	if ( VER_PLATFORM_WIN32_NT == oi.dwPlatformId )
	{
		HANDLE handle;
		TOKEN_PRIVILEGES tkp;
		
		OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &handle );
		LookupPrivilegeValue( NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid );

		tkp.PrivilegeCount		= 1;  // one privilege to set    
		tkp.Privileges[ 0 ].Attributes	= SE_PRIVILEGE_ENABLED;

		AdjustTokenPrivileges( handle, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0 );
	}

	//	WIN98 下关机或注销时可直接调用下面涵数既可
	::ExitWindowsEx( ShutdownFlag, 0 );
}

/**
 *	写系统事件日志
 */
BOOL _delib_report_systemevent( LPCTSTR lpcszSrcName, DWORD dwEventID, WORD wCategory, WORD wNumStrings, LPCTSTR * lpStrings )
{
	//
	//	lpcszSrcName	- [in] event source name
	//	dwEventID	- [in] event identifier
	//	wCategory	- [in] event category
	//	wNumStrings	- [in] count of insert strings
	//	lpStrings	- [in] insert strings
	//	RETURN		VOID
	//

	HANDLE hEventLog;
	BOOL bReport	= FALSE;

	//	Get a handle to the event log.
	hEventLog = RegisterEventSource
	(
		NULL,		//	use local computer
		lpcszSrcName	//	event source name
	);
	if ( hEventLog )
	{
		//	Report the event.
		bReport = ReportEvent
		(
			hEventLog,		//	event log handle 
			EVENTLOG_ERROR_TYPE,	//	event type 
			wCategory,		//	event category  
			dwEventID,		//	event identifier 
			NULL,			//	no user security identifier 
			wNumStrings,		//	number of substitution strings 
			0,			//	no data 
			lpStrings,		//	pointer to strings 
			NULL			//	no data
		);
		if ( ! bReport )
		{
			//	Could not report the event.
		}

		DeregisterEventSource( hEventLog );
	}
	else
	{
		//	Could not register the event source.
	}

	return bReport;
}

/**
 *	运行一个阻塞的进程
 */
DWORD _delib_create_process( LPCTSTR lpszCmdLine, BOOL bBlocked )
{
	//
	//	lpszCmdLine	- [in] 创建进程的命令行
	//	bBlocked	- [in] 是否阻塞
	//	RETURN		返回新创建的进程 ID
	//

	if ( NULL == lpszCmdLine )
	{
		return FALSE;
	}

	DWORD dwProcessId	= 0;
	BOOL  bCreateProcess	= FALSE;
	DWORD dweExitCode	= 0;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	memset( &si, 0, sizeof(si) );
	si.cb = sizeof(si);
	memset( &pi, 0, sizeof(pi) );

	//
	//	Start the child process.
	//
	bCreateProcess = CreateProcess
	(
		NULL,			//	No module name (use command line). 
		(TCHAR*)lpszCmdLine,	//	Command line. 
		NULL,			//	Process handle not inheritable. 
		NULL,			//	Thread handle not inheritable. 
		FALSE,			//	Set handle inheritance to FALSE. 
		0,			//	No creation flags. 
		NULL,			//	Use parent's environment block. 
		NULL,			//	Use parent's starting directory. 
		&si,			//	Pointer to STARTUPINFO structure.
		&pi			//	Pointer to PROCESS_INFORMATION structure.
	);
	if ( bCreateProcess )
	{
		dwProcessId = pi.dwProcessId;

		if ( bBlocked )
		{
			//	Wait until child process exits.
			if ( pi.hProcess )
			{
				//	用户指定要阻塞进程
				if ( WAIT_OBJECT_0 == WaitForSingleObject( pi.hProcess, INFINITE ) )
				{
					GetExitCodeProcess( pi.hProcess, &dweExitCode );
				}

				//	Close process and thread handles.
				CloseHandle( pi.hProcess );
			}
			
			if ( pi.hThread )
			{
				CloseHandle( pi.hThread );
			}
		}
		else
		{
			//	用户未指定要等待，那么就直接返回进程的 ID
			//	用户如果需要进程句柄的话，可以传入进程 ID 通过 OpenProcess 函数来获取
		}
	}

	return dwProcessId;
}

/**
 *	运行一个阻塞的进程
 */
BOOL _delib_run_block_process( LPCTSTR lpszCmdLine )
{
	if ( NULL == lpszCmdLine )
	{
		return FALSE;
	}

	BOOL bRet = FALSE;
	BOOL bCreateProcess = FALSE;
	DWORD dweExitCode = 0;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	
	memset( &si, 0, sizeof(si) );
	si.cb = sizeof(si);
	memset( &pi, 0, sizeof(pi) );

	//	Start the child process. 
	bCreateProcess = CreateProcess
	(
		NULL,			// No module name (use command line). 
		(TCHAR*)lpszCmdLine,	// Command line. 
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

		//	Wait until child process exits.
		if ( pi.hProcess )
		{
			if ( WAIT_OBJECT_0 == 
				WaitForSingleObject( pi.hProcess, INFINITE ) )
			{
				GetExitCodeProcess( pi.hProcess, &dweExitCode );
			}
		}
		//	Close process and thread handles. 
		if ( pi.hProcess )
		{
			CloseHandle( pi.hProcess );
		}
		if ( pi.hThread )
		{
			CloseHandle( pi.hThread );
		}
	}

	return bRet;
}


/**
 *	分配一块共享内存
 */
BOOL _delib_alloc_share_memory( DWORD dwSize, LPCTSTR lpcszMemName, VOID ** lppvBuffer, HANDLE hFileMap, LPBOOL lpbCreate, LPSECURITY_ATTRIBUTES lpFileMappingAttributes )
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
BOOL _delib_open_share_memory( LPCTSTR lpcszMemName, VOID ** lppvBuffer, HANDLE hFileMap )
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
 *	@ public
 *	从资源里面提取文件到磁盘
 *	必须保证DestFilename是可以创建的
 */
BOOL _delib_extract_file_from_resource( HINSTANCE hInst, LPCTSTR lpcszResType, UINT uResID, LPCTSTR lpszDestFilename )
{
	//
	//	hInst			- [in] hInstance
	//	lpcszResType		- [in] 资源类型，例如："TXT"
	//	uResID			- [in] 资源 ID
	//	lpszDestFilename	- [in] 提取资源后，保存到的文件名，要求全路径
	//	RETURN			- TRUE / FALSE
	//

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
	
	pResBuffer = _delib_get_resource_buffer( hInst, lpcszResType, uResID, &dwSize );
	if ( pResBuffer && dwSize > 0 )
	{
		hFile = CreateFile
		(
			lpszDestFilename, GENERIC_WRITE, 0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, NULL
		);		// no attr. template
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
 *	获取资源缓冲区
 */
PVOID _delib_get_resource_buffer( HINSTANCE hInst, LPCTSTR lpcszResType, UINT uResID, DWORD * pdwSize )
{
	//
	//	hInst			- [in] hInstance
	//	lpcszResType		- [in] 资源类型，例如："TXT"
	//	uResID			- [in] 资源 ID
	//	pdwSize			- [out] 提取资源大小
	//
	//	RETURN			- TRUE / FALSE
	//

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
BOOL _delib_file_exists( LPCTSTR lpcszFile )
{
	//
	//	lpcszFile	- [in] 文件路径
	//

	if ( NULL == lpcszFile )
	{
		return FALSE;
	}
	if ( NULL == lpcszFile[0] )	//strlen
	{
		return FALSE;
	}

	FILE * fp = fopen( lpcszFile, "r" );
	if( fp )
	{
		fclose( fp );
		return TRUE;
	}

	return FALSE;
}

/**
 *	获取文件大小，单位为：字节
 */
DWORD _delib_get_file_size( LPCTSTR lpszFilePath )
{
	if ( NULL == lpszFilePath || 0 == _tcslen(lpszFilePath) )
	{
		return 0;
	}

	HANDLE hFile		= NULL;
	DWORD dwFileSize	= 0;
	hFile = CreateFile( lpszFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0 );
	if ( INVALID_HANDLE_VALUE != hFile )
	{
		dwFileSize = ::GetFileSize( hFile, NULL );
		CloseHandle( hFile );
	}

	return dwFileSize;
}

/**
 *	设置文件为可写
 */
BOOL _delib_set_file_writable( LPCTSTR lpcszFilePath )
{
	//
	//	lpcszFilePath	- [in] 文件路径
	//

	if ( NULL == lpcszFilePath )
	{
		return FALSE;
	}
	else if ( NULL == lpcszFilePath[0] )
	{
		return FALSE;
	}
	if ( FALSE == _delib_file_exists( lpcszFilePath ) )
	{
		return FALSE;
	}

	DWORD dwFileAttrs	= 0;
	dwFileAttrs = GetFileAttributes( lpcszFilePath ); 
	if ( dwFileAttrs & FILE_ATTRIBUTE_READONLY )
	{
		SetFileAttributes( lpcszFilePath, (dwFileAttrs & ~FILE_ATTRIBUTE_READONLY) ); 
	}

	return TRUE;
}

/**
 *	从文件资源里提出版本
 */
BOOL _delib_get_file_version( LPCTSTR lpszFilePath, LPTSTR lpszVersion, DWORD dwSize )
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

/**
 *	获取文件内容
 */
BOOL _delib_get_file_mapbuffer( LPCTSTR lpszFilePath, LPTSTR lpszBuffer, DWORD dwSize )
{
	BOOL bRet = FALSE;
	UINT uFileLen = 0;
	
        HANDLE hfile = CreateFile( lpszFilePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0 );
	if ( INVALID_HANDLE_VALUE != hfile )
	{
		uFileLen = GetFileSize( hfile, 0 );
		if ( uFileLen > 0 )
		{
			if ( uFileLen > DELIB_MAX_FILEMAP_LENGTH )
			{
				uFileLen = DELIB_MAX_FILEMAP_LENGTH;
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
BOOL _delib_get_file_md5( LPCTSTR lpszFilePath, LPTSTR lpszMd5, DWORD dwSize, LPTSTR lpszError )
{
	BOOL bRet = FALSE;
	UINT uFileLen = 0;

        HANDLE hfile = CreateFile( lpszFilePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0 );
	if ( INVALID_HANDLE_VALUE != hfile )
	{
		uFileLen = GetFileSize( hfile, 0 );
		if ( uFileLen > 0 )
		{
			if ( uFileLen > DELIB_MAX_FILEMAP_LENGTH )
			{
				_tcscpy( lpszError, _T("FileSize too long. must less then 100M.") );
				uFileLen = DELIB_MAX_FILEMAP_LENGTH;
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
						_sntprintf( szHexTmp, sizeof(szHexTmp)-sizeof(TCHAR), _T("%02X"), szEncrypt[i] );
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
					_tcscpy( lpszError, _T("Failed to MapViewOfFile") );
				}
				
				// close map
				CloseHandle( hmap );
			}
			else
			{
				_tcscpy( lpszError, _T("Failed to CreateFileMapping") );
			}
		}
		else
		{
			_tcscpy( lpszError, _T("Empty file") );
		}
		
		// close handle
		CloseHandle(hfile);
	}
	else
	{
		_tcscpy( lpszError, _T("Failed to CreateFile") );
	}
	
	return bRet;
}

/**
 *	获取一个字符串的 MD5 值(32位字符串)
 */
BOOL _delib_get_string_md5( LPCTSTR lpszString, LPTSTR lpszMd5, DWORD dwSize, DWORD dwSpecStringLen )
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
ENUMWINDOWSSYSTYPE _delib_get_shellsystype()
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
 *	拷贝字符串到剪切板
 */
BOOL  _delib_copy_text_to_clipboard( LPCTSTR lpszString, HWND hWndNewOwner /* = NULL */ )
{
	if ( NULL == lpszString || 0 == _tcslen(lpszString) )
	{
		return FALSE;
	}

	HGLOBAL hClipBuffer;
	TCHAR * buffer;

	if ( ::OpenClipboard( hWndNewOwner ) )
	{
		::EmptyClipboard();

		hClipBuffer	= ::GlobalAlloc( GMEM_DDESHARE, _tcslen(lpszString) + sizeof(TCHAR) );
		buffer		= (TCHAR*)::GlobalLock( hClipBuffer );
		_tcscpy( buffer, lpszString );
		::GlobalUnlock( hClipBuffer );
		::SetClipboardData( CF_TEXT, hClipBuffer );
		::CloseClipboard();
	}

	return TRUE;
}


/**
 *	获取 CRC32 值
 */
DWORD _delib_get_crc32( IN CONST BYTE * pBinBuf, IN INT nBufLen )
{
	CCrc32 cCrc32;
	return cCrc32.GetCrc32( pBinBuf, nBufLen );
}


/**
 *	解析某个 INI 文件的 section 到 vector
 */
BOOL _delib_ini_parse_section_line( LPCTSTR lpcszIniFile, LPCTSTR lpcszSection, vector<STINISECTIONLINE> & vcSection, BOOL bParseValue /* = FALSE */ )
{
	//
	//	lpcszIniFile	- [in] INI 文件路径
	//	lpcszSection	- [in] Section Name
	//	vcSection	- [out] 返回值
	//	bParseValue	- [in/opt] 是否要求解析 name=value 对成功
	//	RETURN		- TRUE / FALSE
	//
	
	if ( NULL == lpcszIniFile || ! PathFileExists( lpcszIniFile ) )
	{
		return FALSE;
	}
	if ( NULL == lpcszSection || 0 == _tcslen( lpcszSection ) )
	{
		return FALSE;
	}

	BOOL  bRet = FALSE;
	CMultiString mstr;
	TCHAR szSection[ 32767 ]		= {0};		// Win95 的大小限制
	INT   nSectionLen			= 0;
	INT   nNum				= 0;
	TCHAR ** ppList				= NULL;
	TCHAR ** p				= NULL;
	STINISECTIONLINE stLine;
	vector <STINISECTIONLINE>::iterator it;
	BOOL bExist				= FALSE;
	BOOL bParseValueSucc			= FALSE;
	TCHAR * pMov				= NULL;
	
	//	清理所有配置数据
	if ( vcSection.size() )
	{
		vcSection.clear();
	}
	
	nSectionLen = GetPrivateProfileSection( lpcszSection, szSection, sizeof(szSection), lpcszIniFile );
	if ( 0 == nSectionLen )
	{
		return FALSE;
	}
	
	ppList = mstr.MultiStringToStringList( szSection, nSectionLen+sizeof(TCHAR), &nNum );
	if ( ppList )
	{
		p = ppList;
		while( p && *p )
		{
			if ( 0 == _tcslen(*p) )
			{
				p ++;
				continue;
			}
			
			memset( & stLine, 0, sizeof( stLine ) );
			_sntprintf( stLine.szLine, sizeof(stLine.szLine)-sizeof(TCHAR), "%s", *p );
			
			//	解析 name=value 对
			bParseValueSucc = ( 2 == _stscanf( *p, "%[^=]=%s", stLine.szName, stLine.szValue ) ? TRUE : FALSE );
			
			bExist = FALSE;
			for ( it = vcSection.begin(); it != vcSection.end(); it ++ )
			{
				if ( 0 == _tcsicmp( stLine.szLine, (*it).szLine ) )
				{
					bExist = TRUE;
					break;
				}
			}
			
			if ( ! bExist )
			{
				if ( bParseValue )
				{
					//	调用者要求解析 name=value 对
					if ( bParseValueSucc )
					{
						//	将这个任务添加到队列
						vcSection.push_back( stLine );
					}
				}
				else
				{
					//	将这个任务添加到队列
					vcSection.push_back( stLine );
				}
			}
			
			//	..
			p ++;
		}
		free( ppList );
	}
	
	return TRUE;
}


/**
 *	提取文件名
 */
BOOL _delib_extract_filepath( LPCTSTR lpcszFileName, LPTSTR lpszFullPath, DWORD dwSize )
{
	if ( NULL == lpcszFileName )
	{
		return FALSE;
	}
	
	INT nFLen	= _tcslen( lpcszFileName );
	INT nCopyLen	= min( dwSize, ( nFLen - _tcslen( PathFindFileName(lpcszFileName) ) ) );

	memset( lpszFullPath, 0, dwSize );
	memcpy( lpszFullPath, lpcszFileName, nCopyLen );

	return TRUE;
}



/**
 *	简单加密解密
 *		xorkey	- 加密的 key，默认等于 0x79
 *		pData	- 内容，可以是明文也可以是密文
 *		不管是明文还是密文，只要送进来就可以解密或者加密
 */
VOID _delib_xorenc( CHAR * pData, BYTE xorkey /* = 0XFF */ )
{
	//
	//	pData		- [in/out]	Pointer to a null-terminated string containing the Data
	//	xorkey		- [in]		Specifies the Encode/Decode key
	//
	
	while ( pData && *pData )
	{
		*pData = *pData ^ xorkey;
		pData++;
	}
}


/**
 *	获取某个字符在一个字符串中的个数
 */
DWORD _delib_get_chr_count( LPCTSTR lpszString, TCHAR lpChr )
{
	//
	//	lpszString	- [in] 带查询字符串
	//	lpChr		- [in] 要计算的字符
	//	RETURN		- lpszString 中含有 lpChr 的个数
	//

	if ( NULL == lpszString || NULL == lpChr || NULL == lpszString[0] )
		return 0;
	
	DWORD dwCount	= 0;
	UINT  i		= 0;
	UINT  uLen	= _tcslen(lpszString);
	
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
 *	获取指定名称的 Cookie Value
 */
BOOL _delib_get_cookie_value( LPCTSTR lpszCookieString, LPCTSTR lpszCookieName, LPTSTR lpszValue, DWORD dwSize )
{
	//
	//	lpszCookieString	- [in]  整个 COOKIE 的字符串
	//	lpszCookieName		- [in]  COOKIE 名称
	//	lpszValue		- [out] 返回值
	//	dwSize			- [in]  返回值缓冲区大小
	//	RETURN			- TRUE / FALSE
	//

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
	
	lpszMove = _tcsstr( lpszCookieString, lpszCookieName );
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
 *	检测是否是有效的 SYSTEMTIME 值
 */
BOOL _delib_is_valid_systemtime( SYSTEMTIME * pst )
{
	if ( pst && 
		pst->wMonth >= 1 && pst->wMonth <= 12 &&
		pst->wDay >= 1 && pst->wDay <= 31 &&
		pst->wHour >= 0 && pst->wHour <= 23 &&
		pst->wMinute >= 0 && pst->wMinute <= 59 &&
		pst->wSecond >= 0 && pst->wSecond <= 59 &&
		pst->wDayOfWeek >= 0 && pst->wDayOfWeek <= 6 )
	{
		return TRUE;
	}
	return FALSE;
}

/**
 *	获取 SYSTEMTIME 的字符串值
 */
BOOL _delib_get_string_systemtime( SYSTEMTIME * pst, LPTSTR lpszBuffer, DWORD dwSize )
{
	if ( pst )
	{
		_sntprintf
		(
			lpszBuffer,
			dwSize-sizeof(TCHAR),
			"%04d-%02d-%02d %02d:%02d:%02d",
			pst->wYear, pst->wMonth, pst->wDay, pst->wHour, pst->wMinute, pst->wSecond
		);
		return TRUE;
	}
	return FALSE;
}


/**
 *	将 sInput 用 sDelimiter 分割，然后保存到 vector 中
 */
INT _delib_split_string( const string & sInput, const string & sDelimiter, vector<string> & vcResult, BOOL bIncludeEmpties )
{
	//
	//	sInput			- [in]  源字符串
	//	sDelimiter		- [in]  分隔符号
	//	vcResult		- [out] 输出
	//	bIncludeEmpties		- [in]  是否输出空结果
	//	RETURN			- Number of delimiters found
	//
	// ////////////////////////////////////////////////////////////////////////
	//	eg.
	//
	//	string sInput;
	//	string sDelimiter;
	//	vector<string> vcCName;
	//	string sItem;
	//
	//	//	split name & value to vecor
	//	sInput		= "xing|amei|jack|";
	//	sDelimiter	= "|";
	//	vcCName.clear();
	//	m_pcDeDLib->m_pfn_dedlib_split_string( sInput, sDelimiter, vcCName, FALSE );
	//

	int iPos = 0;
	int newPos = -1;
	int sizeS2 = (int)sDelimiter.size();
	int isize = (int)sInput.size();
	
	if( 
		( isize == 0 )
		||
		( sizeS2 == 0 )
		)
	{
		return 0;
	}
	
	vector<int> positions;
	
	newPos = sInput.find( sDelimiter, 0 );
	
	if( newPos < 0 )
	{ 
		return 0; 
	}
	
	int numFound = 0;
	
	while( newPos >= iPos )
	{
		numFound++;
		positions.push_back(newPos);
		iPos = newPos;
		newPos = sInput.find( sDelimiter, iPos + sizeS2 );
	}
	
	if( numFound == 0 )
	{
		return 0;
	}
	
	for( int i=0; i <= (int)positions.size(); ++i )
	{
		string s("");
		if( i == 0 ) 
		{ 
			s = sInput.substr( i, positions[i] ); 
		}
		int offset = positions[i-1] + sizeS2;
		if( offset < isize )
		{
			if( i == positions.size() )
			{
				s = sInput.substr(offset);
			}
			else if( i > 0 )
			{
				s = sInput.substr( positions[i-1] + sizeS2, positions[i] - positions[i-1] - sizeS2 );
			}
		}
		if( bIncludeEmpties || ( s.size() > 0 ) )
		{
			vcResult.push_back( s );
		}
	}
	return numFound;
}








/**
 *	检查字符串是否是合法的 IP 地址，例如：211.99.66.14
 */
BOOL _delib_is_valid_ipaddr( LPCTSTR lpszString )
{
	//
	//	lpszString	- [in] 待检测字符串
	//	RETURN		- TRUE/FALSE
	//

	if ( NULL == lpszString )
		return FALSE;
	if ( NULL == lpszString[0] )	// strlen
		return FALSE;
	
	UINT  i		= 0;
	UINT  uLen	= _tcslen(lpszString);
	
	//
	//	...
	//
	if ( '.' == lpszString[0] || '.' == lpszString[ uLen - sizeof(TCHAR) ] )
		return FALSE;
	if ( 3 != _delib_get_chr_count( lpszString, '.' ) )
		return FALSE;
	
	for ( i = 0; i < uLen; i ++ )
	{
		if ( NULL == _tcschr( DELIB_VALIDIPADDRSTRING, lpszString[i] ) )
		{
			return FALSE;
		}
	}
	return TRUE;
}

/**
 *	将字符串的 IP 地址转换成数字
 */
DWORD _delib_get_ipaddr_value_from_string( LPCSTR lpcszIpAddr )
{
	if ( NULL == lpcszIpAddr || 0 == lpcszIpAddr[ 0 ] )
	{
		return 0;
	}

	//	INADDR_NONE 如果不是一个合法的 IP 地址
	return inet_addr( lpcszIpAddr );
}


/**
 *	获取所有本地的 IP 地址
 */
BOOL _delib_get_all_locipaddr( vector<STDEIPADDR> & vcIpAddr )
{
	BOOL  bRet		= FALSE;
	WSADATA WSData;
	HOSTENT * pst		= NULL;
	TCHAR ** pplist		= NULL;
	TCHAR * pTemp		= NULL;
	INT i			= 0;
	DWORD dwIndex		= 0;
	TCHAR szBuffer[128]	= {0};
	STDEIPADDR stIpAddr;

	try
	{
		if ( 0 == WSAStartup( MAKEWORD(VERSION_MAJOR,VERSION_MINOR), &WSData ) )
		{
			if ( 0 == gethostname( szBuffer, sizeof(szBuffer) ) )
			{
				pst = gethostbyname(szBuffer);
				if ( pst )
				{
					pplist = pst->h_addr_list;
					while( pplist[i] )
					{
						//	..
						pTemp = inet_ntoa( *((LPIN_ADDR)pplist[i]) );
						//	..
						if ( pTemp && _delib_is_valid_ipaddr( pTemp ) )
						{
							bRet = TRUE;

							memset( &stIpAddr, 0, sizeof(stIpAddr) );
							_sntprintf( stIpAddr.szIpAddr, sizeof(stIpAddr.szIpAddr)-sizeof(TCHAR), "%s", pTemp );
							vcIpAddr.push_back( stIpAddr );

							dwIndex ++;
						}
						i++;
					}
				}
			}
			// ..
			WSACleanup();
		}
	}
	catch (...)
	{
	}
	
	// ..
	return bRet;
}

/**
 *	获取 MAC 地址
 */
BOOL _delib_get_mac_address( LPCTSTR lpcszNetBiosName, LPTSTR lpszMacAddress, DWORD dwSize )
{
	/*
		lpcszNetBiosName	- [in]
		lpszMacAddress		- [out] 
		dwSize			- [in]
		RETURN			- TRUE / FALSE
	*/
	if ( NULL == lpcszNetBiosName || 0 == strlen( lpcszNetBiosName ) )
	{
		return FALSE;
	}
	if ( NULL == lpszMacAddress || 0 == dwSize )
	{
		return FALSE;
	}

	BOOL bRet		= FALSE;
	TCHAR szBiosName[ 64 ]	= {0};
	STASTAT Adapter;
	NCB ncb;
	UCHAR uRetCode;

	//	copy bios name
	_sntprintf( szBiosName, sizeof(szBiosName)-sizeof(TCHAR), "%s", lpcszNetBiosName );

	memset( & ncb, 0, sizeof(ncb) );
	ncb.ncb_command		= NCBRESET;
	ncb.ncb_lana_num	= 0;

	uRetCode = Netbios( &ncb );
	if ( 0 == uRetCode )
	{
		memset( & ncb, 0, sizeof(ncb) );
		ncb.ncb_command		= NCBASTAT;
		ncb.ncb_lana_num	= 0;

		//	大写
		_tcsupr( szBiosName );

		FillMemory( ncb.ncb_callname, NCBNAMSZ - 1, 0x20 );
		_sntprintf( (LPTSTR)ncb.ncb_callname, sizeof(ncb.ncb_callname)-sizeof(TCHAR), "%s", szBiosName );

		ncb.ncb_callname[ strlen( szBiosName ) ] = 0x20;
		ncb.ncb_callname[ NCBNAMSZ ] = 0x0;

		ncb.ncb_buffer = (unsigned char *) &Adapter;
		ncb.ncb_length = sizeof(Adapter);

		uRetCode = Netbios( &ncb );
		if ( 0 == uRetCode )
		{
			bRet = TRUE;
			_sntprintf
			(
				lpszMacAddress,
				dwSize-sizeof(TCHAR),
				"%02x-%02x-%02x-%02x-%02x-%02x", 
				Adapter.adapt.adapter_address[0],
				Adapter.adapt.adapter_address[1],
				Adapter.adapt.adapter_address[2],
				Adapter.adapt.adapter_address[3],
				Adapter.adapt.adapter_address[4],
				Adapter.adapt.adapter_address[5]
			);
		}
	}

	return bRet;
}


//	获取硬盘序列号
BOOL _delib_get_hdisk_serialnumber( LPTSTR lpszSerialNumber, DWORD dwSize )
{
	BOOL  bRet	= FALSE;
	TCHAR szNameBuf[ 12 ];
	DWORD dwSerialNumber;
	DWORD dwMaxLen; 
	DWORD dwFileFlag;
	TCHAR szSysNameBuf[ 10 ];

	bRet = ::GetVolumeInformation
	(
		"c:\\",
		szNameBuf,
		sizeof(szNameBuf), 
		& dwSerialNumber, 
		& dwMaxLen,
		& dwFileFlag,
		szSysNameBuf,
		sizeof(szSysNameBuf)
	);
	if ( bRet )
	{
		_sntprintf( lpszSerialNumber, dwSize-sizeof(TCHAR), "%x", dwSerialNumber );
	}

	return bRet;
}