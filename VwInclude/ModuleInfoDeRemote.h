// ModuleInfoDeRemote.h: interface for the CModuleInfoDeRemote class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __MODULEINFODEREMOTE_HEADER__
#define __MODULEINFODEREMOTE_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ModuleInfo.h"



//	"DeRemoteMBase.ini"
static TCHAR g_szModuleInfoDeRemote_DeRemoteMBaseini[] =
{
	-69, -102, -83, -102, -110, -112, -117, -102, -78, -67, -98, -116, -102, -47, -106, -111, -106, 0
};

//	"DeRemote.key"
static TCHAR g_szModuleInfoDeRemote_DeRemotekey[] =
{
	-69, -102, -83, -102, -110, -112, -117, -102, -47, -108, -102, -122, 0
};

//	"DeRemote.key.dec"
static TCHAR g_szModuleInfoDeRemote_DeRemotekeydec[] =
{
	-69, -102, -83, -102, -110, -112, -117, -102, -47, -108, -102, -122, -47, -101, -102, -100, 0
};

//	"DeRemoteCfg.exe"
static TCHAR g_szModuleInfoDeRemote_DeRemoteCfgexe[] =
{
	-69, -102, -83, -102, -110, -112, -117, -102, -68, -103, -104, -47, -102, -121, -102, 0
};

//	"DeRemoteCfg.exe -auto"
static TCHAR g_szModuleInfoDeRemote_DeRemoteCfgexeauto[] =
{
	-69, -102, -83, -102, -110, -112, -117, -102, -68, -103, -104, -47, -102, -121, -102, -33, -46, -98, -118, -117, -112, 0
};



/**
 *	class of CModuleInfoDeRemote
 */
class CModuleInfoDeRemote :
	public CModuleInfo
{
public:
	CModuleInfoDeRemote();
	virtual ~CModuleInfoDeRemote();

private:
	BOOL  InitPathInfo( HINSTANCE hInstance );

public:
	TCHAR m_szCfgFile[ MAX_PATH ];
	TCHAR m_szKeyFile[ MAX_PATH ];
	TCHAR m_szKeyDecFile[ MAX_PATH ];

	TCHAR m_szDeRemoteCfg[ MAX_PATH ];
	TCHAR m_szDeRemoteCfgAuto[ MAX_PATH ];
};



#endif // __MODULEINFODEREMOTE_HEADER__
