#include "StdAfx.h"
#include "ErrorLog.h"



//////////////////////////////////////////////////////////////////////////
//
CErrorLog::CErrorLog()
{
	memset( m_szLogDir, 0, sizeof(m_szLogDir) );
}

CErrorLog::~CErrorLog()
{
}

VOID CErrorLog::Init( HINSTANCE hInstance )
{
	TCHAR szExePath[ MAX_PATH ]	= {0};
	TCHAR szDriver[ _MAX_DRIVE ]	= {0};
	TCHAR szPath[ _MAX_DIR ]	= {0};

	// ..
	GetModuleFileName( hInstance, szExePath, sizeof(szExePath) );
	_splitpath( szExePath, szDriver, szPath, 0, 0 );
	_sntprintf( m_szLogDir, sizeof(m_szLogDir)-sizeof(TCHAR), _T("%s%s%s\\"), _T(szDriver), _T(szPath), CONST_DIR_LOGS );
	// ..
	if ( ! PathIsDirectory(m_szLogDir) )
	{
		CreateDirectory( m_szLogDir, NULL );
	}
}

STLOGMGR * CErrorLog::GetLogObject( LPCTSTR lpcszSiteId, DWORD dwTimePeriod )
{
	if ( 0 == m_szLogDir[0] )
	{
		return NULL;
	}
	if ( NULL == lpcszSiteId || 0 == lpcszSiteId[0] )
	{
		return NULL;
	}
	
	INT i		= 0;
	
	//	..
	if ( FALSE == PathIsDirectory( m_szLogDir ) )
	{
		CreateDirectory( m_szLogDir, NULL );
	}
	
	//	..
	for ( i = 0; i < MAX_SITECOUNT; i ++ )
	{
		if ( strlen( m_Logger[i].szSiteId ) > 0 )
		{
			if ( 0 == memcmp( lpcszSiteId, m_Logger[i].szSiteId, strlen(lpcszSiteId) ) )
			{
				return & m_Logger[i];
			}
		}
		else
		{
			break;
		}
	}
	
	//	..
	if ( i < MAX_SITECOUNT )
	{
		_sntprintf( m_Logger[i].szSiteLogDir, sizeof(m_Logger[i].szSiteLogDir)-sizeof(TCHAR), "%s\\%s\\", m_szLogDir, lpcszSiteId );
		_sntprintf( m_Logger[i].szSiteId, sizeof(m_Logger[i].szSiteId)-sizeof(TCHAR), "%s", lpcszSiteId );
		m_Logger[i].dwTimePeriod = dwTimePeriod;
		m_Logger[i].fp = NULL;
		return & m_Logger[i];
	}
	
	return NULL;
}
