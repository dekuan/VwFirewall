// ModuleInfoCommonLicence.cpp: implementation of the CModuleInfoCommonLicence class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ModuleInfoCommonLicence.h"

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CModuleInfoCommonLicence::CModuleInfoCommonLicence()
{
//	memset( m_szCfgFile, 0, sizeof(m_szCfgFile) );
	memset( m_szKeyFile, 0, sizeof(m_szKeyFile) );
//	memset( m_szKeyDecFile, 0, sizeof(m_szKeyDecFile) );
	
	//	初始化路径信息
	InitPathInfo( mb_hDllInstance );
}

CModuleInfoCommonLicence::~CModuleInfoCommonLicence()
{
}



//////////////////////////////////////////////////////////////////////////
//	Private


/**
 *	初始化模块信息
 */
BOOL CModuleInfoCommonLicence::InitPathInfo( HINSTANCE hInstance )
{
	BOOL bRet		= FALSE;
	TCHAR szDecStr[ 128 ]	= {0};
	
	if ( mb_bInitSucc )
	{
		bRet = TRUE;
		
		//	存放加密后的 licence 信息的文件
		memset( szDecStr, 0, sizeof(szDecStr) );
		memcpy( szDecStr, g_szModuleInfoCommonLicence_DeRemotekey, min( sizeof(g_szModuleInfoCommonLicence_DeRemotekey), sizeof(szDecStr) ) );
		delib_xorenc( szDecStr );
		_sntprintf( m_szKeyFile, sizeof(m_szKeyFile)-sizeof(TCHAR), _T("%s%s"), CModuleInfo::mb_szModPath, szDecStr );
	}

	return bRet;
}