// DeDLib.h: interface for the CDeDLib class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DEDLIB_HEADER__
#define __DEDLIB_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
using namespace std;



//////////////////////////////////////////////////////////////////////////
//	start define

#ifndef _DELIB_DEFINES_
#define _DELIB_DEFINES_


#define DELIB_MAX_FILEMAP_LENGTH	( 100 * 1024 * 1024 )


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

#endif	// _DELIB_DEFINES_

//	end define
//////////////////////////////////////////////////////////////////////////

typedef BOOL (WINAPI * PFN_DEDLIB_GET_FILE_MD5)( LPCTSTR lpszFilePath, LPTSTR lpszMd5, DWORD dwSize, LPTSTR lpszError );
typedef BOOL (WINAPI * PFN_DEDLIB_GET_STRING_MD5)( LPCTSTR lpszString, LPTSTR lpszMd5, DWORD dwSize, DWORD dwSpecStringLen = 0 );

typedef DWORD(WINAPI * PFN_DEDLIB_GET_CRC32)( IN CONST BYTE * pBinBuf, IN INT nBufLen );
typedef BOOL (WINAPI * PFN_DEDLIB_INI_PARSE_SECTION_LINE)( LPCTSTR lpcszIniFile, LPCTSTR lpcszSection, vector<STINISECTIONLINE> & vcSection, BOOL bParseValue = FALSE );


/**
 *	替换字符串里面的一个子串
 */
static void _dedlib_strchg(char* dest, int len, char* rstr)
{
	int rlen = strlen(rstr);
	memmove(dest+rlen, dest+len, strlen(dest+len)+1);
	memcpy(dest, rstr, rlen);
}
//	无符号搜索子串
static char * _dedlib_stristr(char* src, char* dst)
{
	for(;(strlen(src)-strlen(dst) >= 0 && *src);src++)
	{
		if (!strnicmp(src, dst, strlen(dst)))
			return src;
	}
	return NULL;
}
//	替换目标字符串里面的子串
static void _dedlib_replace( char* src, char* sFindStr, char* sRepWithStr )
{
	for(char* pos=src;(pos=_dedlib_stristr(pos, sFindStr))!=NULL;pos=pos+strlen(sRepWithStr))
	{
		_dedlib_strchg(pos, strlen(sFindStr), sRepWithStr);
	}
}

/**
 *	class of CDeDLib
 */
class CDeDLib
{
public:
	CDeDLib( HINSTANCE hCallerInstance );
	virtual ~CDeDLib();

	static CDeDLib * GetInstance( HINSTANCE hCallerInstance );
	BOOL IsInitSucc();

private:
	BOOL InitApp( HINSTANCE hCallerInstance );

public:
	PFN_DEDLIB_GET_FILE_MD5 m_pfn_dedlib_get_file_md5;
	PFN_DEDLIB_GET_STRING_MD5 m_pfn_dedlib_get_string_md5;

	PFN_DEDLIB_GET_CRC32 m_pfn_dedlib_get_crc32;
	PFN_DEDLIB_INI_PARSE_SECTION_LINE m_pfn_dedlib_ini_parse_section_line;

private:
	BOOL m_bInitSucc;
	HINSTANCE m_hCallerInstance;
	HINSTANCE m_hLibrary;

	TCHAR m_szExeFile[ MAX_PATH ];
	TCHAR m_szAppDir[ MAX_PATH ];
	TCHAR m_szDeDLibBaseFile[ MAX_PATH ];
};

static CDeDLib * g_cStcClsDeDLib	= NULL;
static BOOL g_bDeDLibAlertError		= FALSE;



#endif // __DEDLIB_HEADER__
