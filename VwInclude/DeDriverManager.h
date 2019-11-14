//////////////////////////////////////////////////////////////////////////
// DeDriverManager.h
#ifndef __DEDRIVERMANAGER_HEADER__
#define __DEDRIVERMANAGER_HEADER__

#pragma once

#include "winsvc.h"
#include "winioctl.h"



/**
 *	Error Code
 */
#define DEDRIVERMANAGER_SUCCESS				0
#define DEDRIVERMANAGER_ERROR_UNKNOWN			-1000
#define DEDRIVERMANAGER_ERROR_PARAM			-1001
#define DEDRIVERMANAGER_ERROR_MEMORY			-1002

#define DEDRIVERMANAGER_ERROR_SCM			-2001
#define DEDRIVERMANAGER_ERROR_SERVICE			-2002
#define DEDRIVERMANAGER_ERROR_INVALID_PATH_OR_FILE	-2003
#define DEDRIVERMANAGER_ERROR_INVALID_HANDLE		-2004
#define DEDRIVERMANAGER_ERROR_STARTING			-2005
#define DEDRIVERMANAGER_ERROR_STOPPING			-2006
#define DEDRIVERMANAGER_ERROR_REMOVING			-2007
#define DEDRIVERMANAGER_ERROR_IO			-2008
#define DEDRIVERMANAGER_ERROR_NOT_INITIALIZED		-2009
#define DEDRIVERMANAGER_ERROR_ALREADY_INITIALIZED	-2010
#define DEDRIVERMANAGER_ERROR_NULL_POINTER		-2011



/**
 *	class of CDeDriverManager
 */
class CDeDriverManager
{
public:
	CDeDriverManager();
	~CDeDriverManager();


	LONG InitDriver( LPCTSTR lpcszDriverName, LPCTSTR lpcszDriverPath );

	LONG LoadDriver( LPCTSTR lpcszDriverName, LPCTSTR lpcszDriverPath, BOOL bStart = TRUE );
	LONG UnloadDriver( BOOL bForceClearData = FALSE );

	LONG StartDriver();
	LONG StopDriver();

	VOID SetRemovable( BOOL bValue );

	BOOL IsInitialized();
	BOOL IsStarted();
	BOOL IsLoaded();


	HANDLE GetDriverHandle();

	LONG  WriteIo( DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD dwInBufferSize );
	LONG  ReadIo( DWORD dwIoControlCode, LPVOID lpOutBuffer, DWORD dwOutBufferSize );
	LONG  RawIo( DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD dwInBufferSize, LPVOID lpOutBuffer, DWORD dwOutBufferSize );

private:
	LONG OpenDriverDevice();

private:
	HANDLE m_hDriver;	// driver handle

	TCHAR  m_szDriverName[ MAX_PATH ];
	TCHAR  m_szDriverPath[ MAX_PATH ];
	TCHAR  m_szDriverDosName[ MAX_PATH ];

	BOOL   m_bInitialized;
	BOOL   m_bStarted;
	BOOL   m_bLoaded;
	BOOL   m_bRemovable;

};



#endif	// __DEDRIVERMANAGER_HEADER__