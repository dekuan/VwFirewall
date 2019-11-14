// ModuleInfoCommonLicence.h: interface for the CModuleInfoCommonLicence class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __MODULEINFOCOMMONLICENCE_HEADER__
#define __MODULEINFOCOMMONLICENCE_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#include "ModuleInfo.h"


//	"DeRemote.key"
static TCHAR g_szModuleInfoCommonLicence_DeRemotekey[] =
{
	-69, -102, -83, -102, -110, -112, -117, -102, -47, -108, -102, -122, 0
};



/**
 *	class of CModuleInfoCommonLicence
 */
class CModuleInfoCommonLicence :
	public CModuleInfo
{
public:
	CModuleInfoCommonLicence();
	virtual ~CModuleInfoCommonLicence();

private:
	BOOL  InitPathInfo( HINSTANCE hInstance );
	
public:
//	TCHAR m_szCfgFile[ MAX_PATH ];
	TCHAR m_szKeyFile[ MAX_PATH ];
//	TCHAR m_szKeyDecFile[ MAX_PATH ];

};

#endif // __MODULEINFOCOMMONLICENCE_HEADER__
