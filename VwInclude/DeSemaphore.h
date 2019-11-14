// DeSemaphore.h: interface for the CDeSemaphore class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DESEMAPHORE_HEADER__
#define __DESEMAPHORE_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/**
 *	const define
 */
#define CDESEMAPHORE_MAXIMUM_COUNT	1000000000
#define CDESEMAPHORE_NAME_VIRTUALWALL	"VIRTUALWALL"


/**
 *	class of CDeSemaphore
 */
class CDeSemaphore
{
public:
	CDeSemaphore();
	virtual ~CDeSemaphore();

	BOOL Create( LPCTSTR lpcszName );
	BOOL Open( LPCTSTR lpcszName );
	BOOL Lock( LONG * lplnPreviousCount = NULL );
	BOOL Unlock( DWORD dwMilliseconds );
	LONG GetCount();
	BOOL IsSucc();


public:
	HANDLE m_hSemaphore;
};




#endif // __DESEMAPHORE_HEADER__
