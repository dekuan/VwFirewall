// DeCommonLicence.h: interface for the CDeCommonLicence class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DECOMMONLICENCE_HEADER__
#define __DECOMMONLICENCE_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ModuleInfo.h"
#include "DeFileEncrypt.h"
#include "DeLicence.h"



/**
 *	在线获取激活码的返回结果(result=)
 */
#define MCH_GET_ACTCODE_SUCC		0
#define MCH_GET_ACTCODE_ERR_UNKNOWN	-1
#define MCH_GET_ACTCODE_ERR_NETWORK	-2
#define MCH_GET_ACTCODE_ERR_MD5STR	-1001
#define MCH_GET_ACTCODE_ERR_PARAM	-1002
#define MCH_GET_ACTCODE_ERR_QUERYDB	-1003
#define MCH_GET_ACTCODE_ERR_REGCODE	-1004




/**
 *	struct
 */
typedef struct tagDeCommonLicenceCfg
{
	tagDeCommonLicenceCfg()
	{
		memset( this, 0, sizeof(tagDeCommonLicenceCfg) );
	}

	BOOL  bCreateGeneWithDiskSN;	//	是否使用硬盘序列号作为生成 gene 的参数
	TCHAR szSoftName[ MAX_PATH ];	//	产品名称，例如："deremote"
	TCHAR szPrKey[ MAX_PATH ];	//	产品注册码钥匙
	TCHAR szHttpKey[ MAX_PATH ];	//	http 通讯钥匙
	
	TCHAR szWorkDir[ MAX_PATH ];	//	工作目录

}STDECOMMONLICENCECFG, *LPSTDECOMMONLICENCECFG;




/**
 *	class of CDeCommonLicence
 */
class CDeCommonLicence :
	public CModuleInfo,
	public CDeLicence
{
public:
	CDeCommonLicence();
	virtual ~CDeCommonLicence();

	//	初始化配置信息
	BOOL InitCfg( STDECOMMONLICENCECFG * pstCfg );

	//	检查是否是正确的注册码格式
	BOOL IsValidLicenseString( LPCTSTR lpcszRegCode );

	//	验证是否是正确的注册码
	BOOL IsValidLicence( LPCTSTR lpcszActCode, BOOL bReLoadConfig = FALSE );
	
	//	在线获取激活码(ActCode)
	LONG QueryActCodeFromServer( LPCTSTR lpcszRegCode, LPTSTR lpszActCode, DWORD dwSize );
	BOOL GetQueryActCodeMd5info( LPCTSTR lpcszStr, LPTSTR lpszMd5, DWORD dwSize );
	BOOL IsValidQueryActCodeData( LPCTSTR lpcszMd5, LPCTSTR lpcszStr );
	
	BOOL ReLoadLicenseInfo();
	BOOL SaveLicenseInfo( STDELICENSEINFO * pstDeLicenseInfo, BOOL bForceWrite = FALSE );


public:
	//
	//	各各软件各个版本的 PrKey
	//
	TCHAR m_szDecDeInfoMonitorSoftName[ 32 ];
	TCHAR m_szDecDeInfoMonitorPrKey_V1_0_0_1000[ 64 ];


private:
	BOOL GetKeyFilePath( LPCTSTR lpcszSoftName, LPTSTR lpszKeyFile, DWORD dwSize );

private:
	HINSTANCE m_hDllInstance;

	BOOL  m_bInitSucc;
	STDECOMMONLICENCECFG m_stCfg;

	//	保存注册信息的文件路径
	TCHAR m_szKeyFile[ MAX_PATH ];
	CDeFileEncrypt m_cDeFileEnc;
	CRITICAL_SECTION m_oCriSecKeyFile;

	//	解密 INI 中的 Sec/key 等信息
	TCHAR m_szDecIniSecLicense[ 32 ];
	TCHAR m_szDecIniKeyGene[ 32 ];
	TCHAR m_szDecIniKeyRegCode[ 32 ];
	TCHAR m_szDecIniKeyActCode[ 32 ];

	//	解密后的通讯密码
	TCHAR m_szDecDefaultHttpKey[ 32 ];

};




#endif // __DECOMMONLICENCE_HEADER__
