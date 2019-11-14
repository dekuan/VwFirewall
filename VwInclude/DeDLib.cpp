// DeDLib.cpp: implementation of the CDeDLib class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeDLib.h"

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeDLib::CDeDLib( HINSTANCE hCallerInstance )
{
	m_bInitSucc		= FALSE;
	m_hCallerInstance	= hCallerInstance;
	memset( m_szDeDLibBaseFile, 0, sizeof(m_szDeDLibBaseFile) );

	m_hLibrary				= NULL;

	m_pfn_dedlib_get_file_md5		= NULL;
	m_pfn_dedlib_get_string_md5		= NULL;

	m_pfn_dedlib_get_crc32			= NULL;
	m_pfn_dedlib_ini_parse_section_line	= NULL;

	if ( InitApp( hCallerInstance ) )
	{
		m_hLibrary = LoadLibrary( m_szDeDLibBaseFile );
		if ( m_hLibrary )
		{
			m_bInitSucc = TRUE;

			//	GetProcAddress ...
			m_pfn_dedlib_get_file_md5 = (PFN_DEDLIB_GET_FILE_MD5)GetProcAddress( m_hLibrary, "dedlib_get_file_md5" );
			m_pfn_dedlib_get_string_md5 = (PFN_DEDLIB_GET_STRING_MD5)GetProcAddress( m_hLibrary, "dedlib_get_string_md5" );

			m_pfn_dedlib_get_crc32 = (PFN_DEDLIB_GET_CRC32)GetProcAddress( m_hLibrary, "dedlib_get_crc32" );
			m_pfn_dedlib_ini_parse_section_line = (PFN_DEDLIB_INI_PARSE_SECTION_LINE)GetProcAddress( m_hLibrary, "dedlib_ini_parse_section_line" );
		}
	}

	if ( ! m_bInitSucc )
	{
		//	整个进程仅仅一次错误提示: 无法装载
		if ( ! g_bDeDLibAlertError )
		{
			g_bDeDLibAlertError = TRUE;
			MessageBox( NULL, _T("Can not load DeDLibBase.dll"), _T("ERROR"), MB_ICONERROR );
		}
	}
}

CDeDLib::~CDeDLib()
{
	/*
	if ( m_hLibrary )
	{
		FreeLibrary( m_hLibrary );
	}
	*/
}


CDeDLib * CDeDLib::GetInstance( HINSTANCE hCallerInstance )
{
	if ( NULL == g_cStcClsDeDLib )
	{
		g_cStcClsDeDLib = new CDeDLib( hCallerInstance );
	}
	return g_cStcClsDeDLib;
}


BOOL CDeDLib::IsInitSucc()
{
	BOOL bRet = m_bInitSucc &&
		m_pfn_dedlib_get_file_md5 &&
		m_pfn_dedlib_get_string_md5 &&
		m_pfn_dedlib_get_crc32 &&
		m_pfn_dedlib_ini_parse_section_line;

	return bRet;
}

/**
 *	初始化应用程序
 */
BOOL CDeDLib::InitApp( HINSTANCE hCallerInstance )
{
	BOOL  bRet			= FALSE;
	TCHAR szDriver[ _MAX_DRIVE ]	= {0};
	TCHAR szPath[ _MAX_DIR ]	= {0};
	TCHAR szError[ MAX_PATH ]	= {0};
	
	if ( GetModuleFileName( hCallerInstance, m_szExeFile, sizeof(m_szExeFile) ) )
	{
		_splitpath( m_szExeFile, szDriver, szPath, 0, 0 );

		//	应用程序所在目录
		_sntprintf( m_szAppDir, sizeof(m_szAppDir)-sizeof(TCHAR), _T("%s%s"), _T(szDriver), _T(szPath) );
		_sntprintf( m_szDeDLibBaseFile, sizeof(m_szDeDLibBaseFile)-sizeof(TCHAR), _T("%s%s"), m_szAppDir, _T("DeDLibBase.dll") );

		bRet = TRUE;
	}

	return bRet;
}