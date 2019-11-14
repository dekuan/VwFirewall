// DeZip.h: interface for the CDeZip class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DEZIP_HEADER__
#define __DEZIP_HEADER__

#include <tchar.h>
#include "XUnzip.h"


#define MAX_URL				1024

#define OPERATOR_DELETE			0
#define OPERATOR_CREATEDIR		1
#define OPERATOR_UNCOMPRESS		2


typedef void (CALLBACK* PSP_UNCOMPRESS_CALLBACK)( IN BOOL bSucced, DWORD dwOperType, IN LPCSTR lpPath1, IN LPCSTR lpPath2, IN DWORD dwErr );



class CDeZip  
{
public:
	CDeZip( PSP_UNCOMPRESS_CALLBACK pfnDetailInfo = NULL );
	virtual ~CDeZip();

	BOOL UnZipFiles( LPCSTR lpZipFile, LPCSTR lpUnZipFileDir, BOOL bUnZipSubDir = FALSE, BOOL bFirstEnter = TRUE );

private:
	VOID VerifyZip( INT nCount, HZIP hz, LPCTSTR lpszFile );
	BOOL UnZip( LPCSTR lpZipFile, LPCSTR lpUnZipFileDir );
	PSP_UNCOMPRESS_CALLBACK m_pfnCallbackDetailInfo;

	BOOL MyFileExists( LPCTSTR lpszFile );
	BOOL StrReplace( char *lpSrc, char chOld, char chNew );
	BOOL MultiCreateDirectory( LPCTSTR lpcszDir );
};


#endif // __DEZIP_HEADER__
