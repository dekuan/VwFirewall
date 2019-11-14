// VwRemoteEvent.h: interface for the CVwRemoteEvent class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWREMOTEEVENT_HEADER__
#define __VWREMOTEEVENT_HEADER__

#include "VwEnCodeString.h"
#include "VwHttp.h"
#include "VwFunc.h"

class CVwRemoteEvent  
{
public:
	CVwRemoteEvent();
	virtual ~CVwRemoteEvent();

	BOOL Init( LPCTSTR lpcszModFilePath );
	BOOL SendEvent( LPCTSTR lpcszEvent );

public:
	BOOL  m_bInitSucc;
	TCHAR m_szVwEventHost[ MAX_PATH ];
	TCHAR m_szFilename[ 64 ];
	TCHAR m_szModVersion[ 32 ];
	TCHAR m_szMac[ 32 ];
	TCHAR m_szHDiskSN[ 32 ];
};


#endif // __VWREMOTEEVENT_HEADER__


