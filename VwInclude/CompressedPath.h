// CompressedPath.h: interface for the CompressedPath class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __CCOMPRESSEDPATH_HEADER__
#define __CCOMPRESSEDPATH_HEADER__


#include "shlobj.h"
#pragma comment( lib, "shell32.lib" )

//////////////////////////////////////////////////////////////////////////
#define SPPATH_SYSTEMDRIVE			0x101	// C:  系统硬盘
#define SPPATH_WINDIR				0x102	// C:\WINNT	 Windows目录
#define SPPATH_SYSTEMROOT			0x103	// C:\WINNT	 系统启动目录
#define SPPATH_WINSYSTEM			0x104	// C:\WINNT\SYSTEM32
#define SPPATH_PROGRAMFILES			0x105	// C:\Program Files
#define SPPATH_USERPROFILE			0x106	// C:\Documents and Settings\Administrator 其中 Administrator 是当前用户username，可变的
#define SPPATH_ALLUSERPROFILE			0x107	// C:\Documents and Settings\All Users
#define SPPATH_COMMON_DESKTOPDIRECTORY		0x108	// C:\Documents and Settings\All Users\Desktop
#define SPPATH_COMMON_DOCUMENTS			0x109	// C:\Documents and Settings\All Users\Documents
#define SPPATH_DESKTOPDIRECTORY			0x110	// C:\Documents and Settings\username\Desktop
#define SPPATH_TEMP				0x111	// C:\WINNT\TEMP
#define SPPATH_TMP				0x112	// C:\WINNT\TEMP


class CCompressedPath
{
public:
	CCompressedPath();
	virtual ~CCompressedPath();

	BOOL GetCompressedPath( LPCTSTR lpszFilePathIn, LPTSTR lpszFilePathNew, DWORD dwPathNewSize, LPTSTR lpszWinDir, DWORD dwWDSize );

private:
	BOOL GetMySpecialFolder( INT nFolder, LPTSTR lpszPath, DWORD dwPSize );
	BOOL LeftPathReplace( LPCTSTR lpszSrcPath, LPTSTR lpszDstPath, DWORD dwDPSize, LPCTSTR lpszFind, LPCTSTR lpszRpWith );

};



#endif // __CCOMPRESSEDPATH_HEADER__
