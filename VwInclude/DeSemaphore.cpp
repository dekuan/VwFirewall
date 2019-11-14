// DeSemaphore.cpp: implementation of the CDeSemaphore class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeSemaphore.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeSemaphore::CDeSemaphore()
{
	m_hSemaphore	= NULL;
}
CDeSemaphore::~CDeSemaphore()
{
	if ( m_hSemaphore )
	{
		CloseHandle( m_hSemaphore );
		m_hSemaphore = NULL;
	}
}


BOOL CDeSemaphore::Create( LPCTSTR lpcszName )
{
	m_hSemaphore = CreateSemaphore( NULL, 0, CDESEMAPHORE_MAXIMUM_COUNT, lpcszName );
	return ( m_hSemaphore ? TRUE : FALSE );
}

BOOL CDeSemaphore::Open( LPCTSTR lpcszName )
{
	m_hSemaphore = OpenSemaphore( SEMAPHORE_ALL_ACCESS, TRUE, lpcszName );
	return ( m_hSemaphore ? TRUE : FALSE );
}

BOOL CDeSemaphore::Lock( LONG * lplnPreviousCount /* = NULL */ )
{
	BOOL bRet	= FALSE;
	LONG lnTmpCount;

	if ( m_hSemaphore )
	{
		bRet = ReleaseSemaphore
			(
				m_hSemaphore,
				1,
				lplnPreviousCount ? lplnPreviousCount : ( & lnTmpCount )
			);
	}

	return bRet;
}

BOOL CDeSemaphore::Unlock( DWORD dwMilliseconds )
{
	BOOL  bRet	= FALSE;
	DWORD dwWait;

	if ( m_hSemaphore )
	{
		dwWait = WaitForSingleObject( m_hSemaphore, dwMilliseconds );
		if ( WAIT_OBJECT_0 == dwWait )
		{
			bRet = TRUE;
		}
		else if ( WAIT_TIMEOUT == dwWait )
		{
			bRet = FALSE;
		}
	}

	return bRet;
}

LONG CDeSemaphore::GetCount()
{
	LONG lnPreviousCount;

	Lock( &lnPreviousCount );
	Unlock( 1 );

	return lnPreviousCount;
}

BOOL CDeSemaphore::IsSucc()
{
	return ( m_hSemaphore ? TRUE : FALSE );
}