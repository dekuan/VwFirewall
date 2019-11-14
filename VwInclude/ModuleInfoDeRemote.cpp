// ModuleInfoDeRemote.cpp: implementation of the CModuleInfoDeRemote class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ModuleInfoDeRemote.h"

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CModuleInfoDeRemote::CModuleInfoDeRemote()
{
	memset( m_szCfgFile, 0, sizeof(m_szCfgFile) );
	memset( m_szKeyFile, 0, sizeof(m_szKeyFile) );
	memset( m_szKeyDecFile, 0, sizeof(m_szKeyDecFile) );

	//	初始化路径信息
	InitPathInfo( mb_hDllInstance );
}

CModuleInfoDeRemote::~CModuleInfoDeRemote()
{
}





//////////////////////////////////////////////////////////////////////////
//	Private



/**
 *	初始化模块信息
 */
BOOL CModuleInfoDeRemote::InitPathInfo( HINSTANCE hInstance )
{
	BOOL bRet		= FALSE;
	TCHAR szDecStr[ 128 ]	= {0};

	if ( mb_bInitSucc )
	{
		bRet = TRUE;

		//	DeRemoteMBase.dll 的配置文件
		memset( szDecStr, 0, sizeof(szDecStr) );
		memcpy( szDecStr, g_szModuleInfoDeRemote_DeRemoteMBaseini, min( sizeof(g_szModuleInfoDeRemote_DeRemoteMBaseini), sizeof(szDecStr) ) );
		delib_xorenc( szDecStr );
		_sntprintf( m_szCfgFile, sizeof(m_szCfgFile)-sizeof(TCHAR), _T("%s%s"), CModuleInfo::mb_szModPath, szDecStr );

		//	存放加密后的 licence 信息的文件
		memset( szDecStr, 0, sizeof(szDecStr) );
		memcpy( szDecStr, g_szModuleInfoDeRemote_DeRemotekey, min( sizeof(g_szModuleInfoDeRemote_DeRemotekey), sizeof(szDecStr) ) );
		delib_xorenc( szDecStr );
		_sntprintf( m_szKeyFile, sizeof(m_szKeyFile)-sizeof(TCHAR), _T("%s%s"), CModuleInfo::mb_szModPath, szDecStr );

		//	存放解密后 licence 信息的文件
		memset( szDecStr, 0, sizeof(szDecStr) );
		memcpy( szDecStr, g_szModuleInfoDeRemote_DeRemotekeydec, min( sizeof(g_szModuleInfoDeRemote_DeRemotekeydec), sizeof(szDecStr) ) );
		delib_xorenc( szDecStr );
		_sntprintf( m_szKeyDecFile, sizeof(m_szKeyDecFile)-sizeof(TCHAR), _T("%s%s"), CModuleInfo::mb_szModPath, szDecStr );

		//	配置工具
		memset( szDecStr, 0, sizeof(szDecStr) );
		memcpy( szDecStr, g_szModuleInfoDeRemote_DeRemoteCfgexe, min( sizeof(g_szModuleInfoDeRemote_DeRemoteCfgexe), sizeof(szDecStr) ) );
		delib_xorenc( szDecStr );
		_sntprintf( m_szDeRemoteCfg, sizeof(m_szDeRemoteCfg)-sizeof(TCHAR), _T("%s%s"), CModuleInfo::mb_szModPath, szDecStr );

		//	配置工具自动启动命令行
		memset( szDecStr, 0, sizeof(szDecStr) );
		memcpy( szDecStr, g_szModuleInfoDeRemote_DeRemoteCfgexeauto, min( sizeof(g_szModuleInfoDeRemote_DeRemoteCfgexeauto), sizeof(szDecStr) ) );
		delib_xorenc( szDecStr );
		_sntprintf( m_szDeRemoteCfgAuto, sizeof(m_szDeRemoteCfgAuto)-sizeof(TCHAR), _T("%s%s"), CModuleInfo::mb_szModPath, szDecStr );	
	}

	return bRet;
}