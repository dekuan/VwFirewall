#ifndef __DESERVICE_HEADER__
#define __DESERVICE_HEADER__


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#include "shlwapi.h"
#pragma comment( lib, "shlwapi.lib" )



/**
 *	return value
 */
#define DESERVICE_ERROR_SUCC			0
#define DESERVICE_ERROR_UNKNOWN			-1000
#define DESERVICE_ERROR_PARAM			-1001
#define DESERVICE_ERROR_OPEN_SC_MANAGER		-1002
#define DESERVICE_ERROR_CREATE_SERVICE		-1003
#define DESERVICE_ERROR_DELETE_SERVICE		-1004
#define DESERVICE_ERROR_OPEN_SERVICE		-1005
#define DESERVICE_ERROR_START_SERVICE		-1006
#define DESERVICE_ERROR_STOP_SERVICE		-1007
#define DESERVICE_ERROR_QUERY_SERVICE_STATUS	-1008
#define DESERVICE_ERROR_SERVICE_ALREADY_STOPED	-1009



/**
 *	class of CDeService
 */
class CDeService
{
public:
	CDeService();
	virtual ~CDeService();

public:
	LONG Install( IN LPCTSTR lpcszSrvName, IN LPCTSTR lpcszSrvExeFilename, IN LPCTSTR lpcszSrvDesc, IN BOOL bInteractive = TRUE );
	LONG Uninstall( IN LPCTSTR lpcszSrvName );

	LONG Start( IN LPCTSTR lpcszSrvName );
	LONG Stop( IN LPCTSTR lpcszSrvName );
	LONG GetStatus( IN LPCTSTR lpcszSrvName );
	LONG GetFilepath( IN LPCTSTR lpcszSrvName, OUT LPTSTR lpszSrvExeFilename, IN DWORD dwSize );
	BOOL IsExist( IN LPCTSTR lpcszSrvName );

	LONG SetDescription( IN LPCTSTR lpcszSrvName, IN LPCTSTR lpcszSrvDesc );
};


#endif	// __DESERVICE_HEADER__