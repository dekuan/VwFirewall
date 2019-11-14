// ProcSecurityDesc.h: interface for the CProcSecurityDesc class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __PROCSECURITYDESC_HEADER__
#define __PROCSECURITYDESC_HEADER__

#ifndef _WIN32_WINNT
	#define _WIN32_WINNT			0x0500
#endif
#define EXPLICIT_ENTRIES_NUM_FILTER	4	// 创建安全对象的个数
#define EXPLICIT_ENTRIES_NUM_EVERYONE	2	// Everyone 创建安全对象的个数

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )

#include <shlobj.h>	// for Security
#include <aclapi.h>



class CProcSecurityDesc
{
public:
	CProcSecurityDesc();
	virtual ~CProcSecurityDesc();

	BOOL SetSecurityDescForFilter( LPCTSTR lpcszPath, LPTSTR pszError, BOOL bPermissionsEveryoneWritable = FALSE );
	BOOL SetSecurityDescForEveryone( LPCTSTR lpcszPath, LPTSTR pszError );

	BOOL GetEveryoneSecurityAttributesData( LPSECURITY_ATTRIBUTES lpFileMappingAttributes, LPTSTR pszError );
	BOOL GetLocalSystemSecurityAttributesData( LPSECURITY_ATTRIBUTES lpFileMappingAttributes, LPTSTR pszError );

private:
	ENUMDEWINDOWSSYSTYPE GetShellSysType();
	BOOL CreateEveryoneSecurityDesc( LPCTSTR lpszPath, DWORD dwEveryoneAccessPms, LPTSTR pszError );
	BOOL CreateIISSecurityDesc( LPCTSTR lpcszPath, LPTSTR pszError, BOOL bPermissionsEveryoneWritable = FALSE );

	BOOL BuildEaFor_Iis50_2000( LPCTSTR lpcszPath, EXPLICIT_ACCESS * pea, DWORD dwEaCount, BOOL bPermissionsEveryoneWritable = FALSE );
	BOOL BuildEaFor_Iis60_2003( LPCTSTR lpcszPath, EXPLICIT_ACCESS * pea, DWORD dwEaCount, BOOL bPermissionsEveryoneWritable = FALSE );
	BOOL BuildEaFor_Iis70_2008( LPCTSTR lpcszPath, EXPLICIT_ACCESS * pea, DWORD dwEaCount, BOOL bPermissionsEveryoneWritable = FALSE );

	HRESULT GetSIDFromName( LPCSTR pwszUserName, PSID * ppSid );


};





#endif	// __PROCSECURITYDESC_HEADER__
