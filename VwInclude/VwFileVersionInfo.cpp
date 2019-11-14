// RcReader.cpp: implementation of the CVwFileVersionInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VwFileVersionInfo.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVwFileVersionInfo::CVwFileVersionInfo()
{
	m_pszFilePath = NULL;
}

CVwFileVersionInfo::~CVwFileVersionInfo()
{
//	if ( m_lpBuffer )
//		free(m_lpBuffer);
}


//////////////////////////////////////////////////////////////////////////
// 初始化内存
BOOL CVwFileVersionInfo::InitInfo( TCHAR * pszError )
{
	if ( NULL == m_pszFilePath )
	{
		_tcscpy( pszError, _T("未指定文件名") );
		return FALSE;
	}

	m_dwHandle = 0;
	m_uiDataSize = MAX_PATH;

//	m_lpData = malloc( m_uiDataSize );

	// Get the version information block size,
	// then use it to allocate a storage buffer.
	m_dwSize = ::GetFileVersionInfoSize( m_pszFilePath, &m_dwHandle );
	m_lpBuffer = malloc( m_dwSize );

	// Get the versioninformation block
	m_bIsSucc = ::GetFileVersionInfo( m_pszFilePath, 0, m_dwSize, m_lpBuffer );

	return m_bIsSucc;
}


VOID CVwFileVersionInfo::FreeInfo()
{
	if ( m_lpBuffer )
	{
		free( m_lpBuffer );
	}
}



BOOL CVwFileVersionInfo::GetProductName( TCHAR * pszValue, DWORD dwSize, TCHAR * pszError )
{
	m_bIsSucc = ::VerQueryValue(
			m_lpBuffer,
			_T("\\StringFileInfo\\040904B0\\ProductName"),
			&m_lpData,
			&m_uiDataSize );

	if ( m_bIsSucc )
		_sntprintf( pszValue, dwSize-sizeof(TCHAR), _T("%s"), m_lpData );

	return m_bIsSucc;
}



BOOL CVwFileVersionInfo::GetProductVersion( TCHAR * pszValue, DWORD dwSize, TCHAR * pszError )
{
	m_bIsSucc = ::VerQueryValue(
			m_lpBuffer,
			_T("\\StringFileInfo\\040904B0\\ProductVersion"),
			&m_lpData,
			&m_uiDataSize );

	if ( m_bIsSucc )
		_sntprintf( pszValue, dwSize-sizeof(TCHAR), _T("%s"), m_lpData );

	return m_bIsSucc;
}



BOOL CVwFileVersionInfo::GetCompanyName( TCHAR * pszValue, DWORD dwSize, TCHAR * pszError )
{
	TCHAR szSubBlock[ MAX_PATH ]	= {0};

	// Read the list of languages and code pages.
	m_cbTranslate = MAX_PATH;
	VerQueryValue(
		m_lpBuffer, 
		_T("\\VarFileInfo\\Translation"),
		(LPVOID*)&m_lpTranslate,
		&m_cbTranslate );

	// Read the file description for each language and code page.
	for ( UINT i = 0; i < (m_cbTranslate/sizeof(struct LANGANDCODEPAGE)); i++ )
	{
		__try
		{
			_sntprintf( szSubBlock, sizeof(szSubBlock),
				_T("\\StringFileInfo\\%04x%04x\\CompanyName"),
				m_lpTranslate[i].wLanguage,
				m_lpTranslate[i].wCodePage);

			// Retrieve file description for language and code page "i".
			m_bIsSucc = ::VerQueryValue(
				m_lpBuffer,
				_T(szSubBlock),
				&m_lpData,
				&m_uiDataSize );

			if ( m_bIsSucc )
			{
				_sntprintf( pszValue, dwSize-sizeof(TCHAR), _T("%s"), m_lpData );
			}
			if ( _tcslen(pszValue) > 0 )
			{
				return TRUE;
			}
		}
		__finally
		{
		}
	}

	return FALSE;
}



BOOL CVwFileVersionInfo::GetCopyright( TCHAR * pszValue, DWORD dwSize, TCHAR * pszError )
{
	m_bIsSucc = ::VerQueryValue(
			m_lpBuffer,
			_T("\\StringFileInfo\\040904B0\\LegalCopyright"),
			&m_lpData,
			&m_uiDataSize );

	if ( m_bIsSucc )
		_sntprintf( pszValue, dwSize-sizeof(TCHAR), _T("%s"), m_lpData );
	
	return m_bIsSucc;
}



BOOL CVwFileVersionInfo::GetComments( TCHAR * pszValue, DWORD dwSize, TCHAR * pszError )
{
	m_bIsSucc = ::VerQueryValue(
			m_lpBuffer,
			_T("\\StringFileInfo\\040904B0\\Comments"),
			&m_lpData,
			&m_uiDataSize );

	if ( m_bIsSucc )
		_sntprintf( pszValue, dwSize-sizeof(TCHAR), _T("%s"), m_lpData );
	
	return m_bIsSucc;
}



BOOL CVwFileVersionInfo::GetFileDescription( TCHAR * pszValue, DWORD dwSize, TCHAR * pszError )
{
	m_bIsSucc = ::VerQueryValue(
			m_lpBuffer,
			_T("\\StringFileInfo\\040904B0\\FileDescription"),
			&m_lpData,
			&m_uiDataSize );

	if ( m_bIsSucc )
		_sntprintf( pszValue, dwSize-sizeof(TCHAR), _T("%s"), m_lpData );
	
	return m_bIsSucc;
}



BOOL CVwFileVersionInfo::GetFileVersion( TCHAR * pszValue, DWORD dwSize, TCHAR * pszError )
{
	TCHAR szSubBlock[ MAX_PATH ]	= {0};
	
	// Read the list of languages and code pages.
	m_cbTranslate = MAX_PATH;
	VerQueryValue(
		m_lpBuffer, 
		_T("\\VarFileInfo\\Translation"),
		(LPVOID*)&m_lpTranslate,
		&m_cbTranslate );
	
	// Read the file description for each language and code page.
	for ( UINT i = 0; i < (m_cbTranslate/sizeof(struct LANGANDCODEPAGE)); i++ )
	{
		__try
		{
			_sntprintf( szSubBlock, sizeof(szSubBlock),
				_T("\\StringFileInfo\\%04x%04x\\FileVersion"),
				m_lpTranslate[i].wLanguage,
				m_lpTranslate[i].wCodePage);
			
			// Retrieve file description for language and code page "i".
			m_bIsSucc = ::VerQueryValue(
				m_lpBuffer,
				_T(szSubBlock),
				&m_lpData,
				&m_uiDataSize );
			
			if ( m_bIsSucc )
			{
				_sntprintf( pszValue, dwSize-sizeof(TCHAR), _T("%s"), m_lpData );
			}
			if ( _tcslen(pszValue) > 0 )
			{
				return TRUE;
			}
		}
		__finally
		{
		}
	}
	
	return FALSE;
}



BOOL CVwFileVersionInfo::GetInternalName( TCHAR * pszValue, DWORD dwSize, TCHAR * pszError )
{
	m_bIsSucc = ::VerQueryValue(
			m_lpBuffer,
			_T("\\StringFileInfo\\040904B0\\InternalName"),
			&m_lpData,
			&m_uiDataSize );

	if ( m_bIsSucc )
		_sntprintf( pszValue, dwSize-sizeof(TCHAR), _T("%s"), m_lpData );
	
	return m_bIsSucc;
}



BOOL CVwFileVersionInfo::GetLegalTrademarks( TCHAR * pszValue, DWORD dwSize, TCHAR * pszError )
{
	m_bIsSucc = ::VerQueryValue(
			m_lpBuffer,
			_T("\\StringFileInfo\\040904B0\\LegalTrademarks"),
			&m_lpData,
			&m_uiDataSize );

	if ( m_bIsSucc )
		_sntprintf( pszValue, dwSize-sizeof(TCHAR), _T("%s"), m_lpData );
	
	return m_bIsSucc;
}



BOOL CVwFileVersionInfo::GetPrivateBuild( TCHAR * pszValue, DWORD dwSize, TCHAR * pszError )
{
	m_bIsSucc = ::VerQueryValue(
			m_lpBuffer,
			_T("\\StringFileInfo\\040904B0\\PrivateBuild"),
			&m_lpData,
			&m_uiDataSize );

	if ( m_bIsSucc )
		_sntprintf( pszValue, dwSize-sizeof(TCHAR), _T("%s"), m_lpData );
	
	return m_bIsSucc;
}



BOOL CVwFileVersionInfo::GetSpecialBuild( TCHAR * pszValue, DWORD dwSize, TCHAR * pszError )
{
	m_bIsSucc = ::VerQueryValue(
			m_lpBuffer,
			_T("\\StringFileInfo\\040904B0\\SpecialBuild"),
			&m_lpData,
			&m_uiDataSize );

	if ( m_bIsSucc )
		_sntprintf( pszValue, dwSize-sizeof(TCHAR), _T("%s"), m_lpData );
	
	return m_bIsSucc;
}


