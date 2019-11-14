// VirtualWallConfigFile.cpp: implementation of the CVirtualWallConfigFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VirtualWallConfigFile.h"
#include "VwEnCodeString.h"
#include "VwConstBase.h"
#include "VwConstAntileechs.h"

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVirtualWallConfigFile::CVirtualWallConfigFile()
{
	m_bInitSucc = InitModule();
}

CVirtualWallConfigFile::~CVirtualWallConfigFile()
{
}







//////////////////////////////////////////////////////////////////////////
//	Private




BOOL CVirtualWallConfigFile::InitModule()
{
	BOOL  bRet			= FALSE;
	BOOL  bLoadCfg			= FALSE;
	

	memset( m_szMetaPath, 0, sizeof(m_szMetaPath) );

	memset( m_szVwClientDllFile, 0, sizeof(m_szVwClientDllFile) );
	memset( m_szVwClientFilterName, 0, sizeof(m_szVwClientFilterName) );
	memset( m_szVwClientFilterDesc, 0, sizeof(m_szVwClientFilterDesc) );

	memset( m_szDllFileDeLib, 0, sizeof(m_szDllFileDeLib) );
	memset( m_szDllFileDeLibNPp, 0, sizeof(m_szDllFileDeLibNPp) );
	memset( m_szDllFileDeLibSks, 0, sizeof(m_szDllFileDeLibSks) );

	//
	//	for VirtualWall.dll
	//
	memset( m_szVwDllName, 0, sizeof(m_szVwDllName) );
	memcpy( m_szVwDllName, g_szFilterVwDllName, min( sizeof(g_szFilterVwDllName),sizeof(m_szVwDllName) ) );
	delib_xorenc( m_szVwDllName );

	memset( m_szFilterVwName, 0, sizeof(m_szFilterVwName) );
	memcpy( m_szFilterVwName, g_szFilterVwName, min( sizeof(g_szFilterVwName), sizeof(m_szFilterVwName) ) );
	delib_xorenc( m_szFilterVwName );

	memset( m_szFilterVwDesc, 0, sizeof(m_szFilterVwDesc) );
	memcpy( m_szFilterVwDesc, g_szFilterVwDesc, min( sizeof(g_szFilterVwDesc), sizeof(m_szFilterVwDesc) ) );
	delib_xorenc( m_szFilterVwDesc );

	//
	//	for VwClient.dll
	//
	memset( m_szVwClientDllName, 0, sizeof(m_szVwClientDllName) );
	memcpy( m_szVwClientDllName, g_szFilterVwClientDllName, sizeof(g_szFilterVwClientDllName) );
	delib_xorenc( m_szVwClientDllName );

	memset( m_szVwClientFilterName, 0, sizeof(m_szVwClientFilterName) );
	memcpy( m_szVwClientFilterName, g_szFilterVwClientFilterName, sizeof(g_szFilterVwClientFilterName) );
	delib_xorenc( m_szVwClientFilterName );

	memset( m_szVwClientFilterDesc, 0, sizeof(m_szVwClientFilterDesc) );
	memcpy( m_szVwClientFilterDesc, g_szFilterVwClientFilterDesc, sizeof(g_szFilterVwClientFilterDesc) );
	delib_xorenc( m_szVwClientFilterDesc );

	memcpy( m_szMetaPath, g_szFilterMetaPath, min( sizeof(g_szFilterMetaPath), sizeof(m_szMetaPath) ) );
	delib_xorenc( m_szMetaPath );
	
	
	


	if ( CModuleInfo::IsInitSucc() )
	{
		bRet	= TRUE;

		//	文件路径和可执行路径
		_sntprintf( m_szExePath, sizeof(m_szExePath)-sizeof(TCHAR), _T("%s"), CModuleInfo::mb_szModFile );
		_sntprintf( m_szAppPath, sizeof(m_szAppPath)-sizeof(TCHAR), _T("%s"), CModuleInfo::mb_szModPath );
		_sntprintf( m_szExeMakeCfgMapFilePath, sizeof(m_szExeMakeCfgMapFilePath)-sizeof(TCHAR), _T("%s -make_cfgmap_file"), CModuleInfo::mb_szModFile );

		//	日志目录
		_sntprintf( m_szLogsPath, sizeof(m_szLogsPath)-sizeof(TCHAR), _T("%s%s"), CModuleInfo::mb_szModPath, CONST_DIR_LOGS );

		//	数据库文件路径
		_sntprintf( m_szCfgDbFile, sizeof(m_szCfgDbFile)-sizeof(TCHAR), _T("%s%s"), CModuleInfo::mb_szModPath, CONST_CONFIG_DBFILE );
		
		//	chg 文件路径
		_sntprintf( m_szCfgChgFile, sizeof(m_szCfgChgFile)-sizeof(TCHAR), _T("%s%s"), CModuleInfo::mb_szModPath, CONST_CONFIG_CHGFILE );
		
		//	内存映射文件路径
		_sntprintf( m_szCfgMapFile, sizeof(m_szCfgMapFile)-sizeof(TCHAR), "%s%s", CModuleInfo::mb_szModPath, CONST_CONFIG_MAPFILE );
		
		//	核心模块工作情况记录文件
		_sntprintf( m_szCoreWorkFile, sizeof(m_szCoreWorkFile)-sizeof(TCHAR), "%s%s\\%s", CModuleInfo::mb_szModPath, CONST_DIR_LOGS, CONST_CORE_WORKFILE );
		
		//	StatusBar Ad Ini 文件
		_sntprintf( m_szStatusBarAdIni, sizeof(m_szStatusBarAdIni)-sizeof(TCHAR), "%s%s", mb_szWinTmpDir, CONST_STATUSBARAD_INIFILE );
		
		//	VirtualWall.dll
		_sntprintf( m_szVwDllFile, sizeof(m_szVwDllFile)-sizeof(TCHAR), "%s%s", CModuleInfo::mb_szModPath, m_szVwDllName );

		//	VwClient.dll
		_sntprintf( m_szVwClientDllFile, sizeof(m_szVwClientDllFile)-sizeof(TCHAR), "%s%s", CModuleInfo::mb_szModPath, m_szVwClientDllName );

		//	VwClient cfg.ini
		_sntprintf( m_szVwClientCfgFile, sizeof(m_szVwClientCfgFile)-sizeof(TCHAR), "%s%s", CModuleInfo::mb_szModPath, "cfg.ini" );


		//	DeLib dll files
		_sntprintf( m_szDllFileDeLib, sizeof(m_szDllFileDeLib)-sizeof(TCHAR), "%s%s", CModuleInfo::mb_szSysDir, CONST_DLLFILE_DELIB );
		_sntprintf( m_szDllFileDeLibNPp, sizeof(m_szDllFileDeLibNPp)-sizeof(TCHAR), "%s%s", CModuleInfo::mb_szSysDir, CONST_DLLFILE_DELIBNPP );
		_sntprintf( m_szDllFileDeLibSks, sizeof(m_szDllFileDeLibSks)-sizeof(TCHAR), "%s%s", CModuleInfo::mb_szSysDir, CONST_DLLFILE_DELIBSKS );	
	}
	
	return bRet;
}
