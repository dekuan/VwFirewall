// CVwDomainAssConfigFile.h: interface for the CVwDomainAssConfigFile class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWINFOMONITORCONFIGFILE_HEADER__
#define __VWINFOMONITORCONFIGFILE_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ModuleInfo.h"
#include "VwConstBase.h"

#include <vector>
using namespace std;

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )


/**
 *	定义配置文件
 */
#define CVWDOMAINASSCONFIGFILE_INI_DOMAIN_MAIN		_T("Main")
#define CVWDOMAINASSCONFIGFILE_INI_DOMAIN_CONFIG	_T("Config")

#define CVWDOMAINASSCONFIGFILE_INI_KEY_SKIN		_T("skin")
#define CVWDOMAINASSCONFIGFILE_INI_KEY_LANGUAGE		_T("language")


/**
 *	struct
 */
typedef struct tagVwDomainAssConfig
{
	tagVwDomainAssConfig()
	{
		memset( this, 0, sizeof(tagVwDomainAssConfig) );
	}

	TCHAR szSkin[ 64 ];		//	皮肤
	TCHAR szLanguage[ 64 ];		//	语言

} STVWDOMAINASSCONFIG, *LPSTVWDOMAINASSCONFIG;


/**
 *	class of CVwDomainAssConfigFile
 */
class CVwDomainAssConfigFile :
	public CModuleInfo
{
public:
	CVwDomainAssConfigFile();
	virtual ~CVwDomainAssConfigFile();

public:
	BOOL LoadConfig();
	UINT LoadConfigUInt( LPCTSTR lpcszDomain, LPCTSTR lpcszKey );
	BOOL LoadConfigString( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPTSTR lpszString, DWORD dwSize );
	BOOL SaveConfig( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPCTSTR lpcszValue );
	BOOL SaveConfigUInt( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, UINT uValue );

	HBITMAP LoadSkinBitmap( LPCTSTR lpctszBitmapName, LPCTSTR lpctszSkinName = NULL );

private:
	BOOL InitModule();

protected:
	BOOL  m_bInitSucc;
	STVWDOMAINASSCONFIG m_stConfig;

	TCHAR m_szConfDir[ MAX_PATH ];
	TCHAR m_szLogsDir[ MAX_PATH ];

	TCHAR m_szCfgFile[ MAX_PATH ];
	TCHAR m_szSysDriverVersion[ MAX_PATH ];

	TCHAR m_szDllFileDeLib[ MAX_PATH ];
};



#endif	//	__VWINFOMONITORCONFIGFILE_HEADER__


