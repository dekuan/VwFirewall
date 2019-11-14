// VirtualWallConfigFile.h: interface for the CVirtualWallConfigFile class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VIRTUALWALLCONFIGFILE_HEADER__
#define __VIRTUALWALLCONFIGFILE_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ModuleInfo.h"


class CVirtualWallConfigFile :
	public CModuleInfo
{
public:
	CVirtualWallConfigFile();
	virtual ~CVirtualWallConfigFile();

	TCHAR m_szExePath[ MAX_PATH ];
	TCHAR m_szAppPath[ MAX_PATH ];
	TCHAR m_szExeMakeCfgMapFilePath[ MAX_PATH ];

	TCHAR m_szLogsPath[ MAX_PATH ];
	TCHAR m_szCfgDbFile[ MAX_PATH ];
	TCHAR m_szCfgChgFile[ MAX_PATH ];
	TCHAR m_szCfgMapFile[ MAX_PATH ];		//	配置数据内存映射文件名
	TCHAR m_szCoreWorkFile[ MAX_PATH ];		//	核心模块工作情况记录文件
	TCHAR m_szStatusBarAdIni[ MAX_PATH ];		//	StatusBar 广告信息文件

	TCHAR m_szVwCfgVersion[ 32 ];
	TCHAR m_szWindowsDir[ MAX_PATH ];

	TCHAR m_szVwDllFile[ MAX_PATH ];
	TCHAR m_szVwDllName[ 64 ];
	TCHAR m_szFilterVwName[ 64 ];
	TCHAR m_szFilterVwDesc[ 64 ];
	TCHAR m_szVwDllVersion[ 32 ];
	
	TCHAR m_szVwClientDllFile[ MAX_PATH ];
	TCHAR m_szVwClientDllName[ 64 ];
	TCHAR m_szVwClientFilterName[ 64 ];
	TCHAR m_szVwClientFilterDesc[ 64 ];
	TCHAR m_szVwClientVersion[ 32 ];
	TCHAR m_szVwClientCfgFile[ MAX_PATH ];

	TCHAR m_szMetaPath[ MAX_PATH ];

	TCHAR m_szDllFileDeLib[ MAX_PATH ];
	TCHAR m_szDllFileDeLibNPp[ MAX_PATH ];
	TCHAR m_szDllFileDeLibSks[ MAX_PATH ];


private:
	BOOL InitModule();
	
protected:
	BOOL  m_bInitSucc;

};




#endif // __VIRTUALWALLCONFIGFILE_HEADER__
