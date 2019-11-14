// DeRemoteLicence.h: interface for the CDeRemoteLicence class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DEREMOTELICENCE_HEADER__
#define __DEREMOTELICENCE_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ModuleInfoDeRemote.h"
#include "DeFileEncrypt.h"
#include "DeLicence.h"



//	PrKey = DeRemoteKey_20080621
static TCHAR g_szDeRemoteLicence_PrKey[] =
{
	-69, -102, -83, -102, -110, -112, -117, -102, -76, -102, -122, -96, -51, -49, -49, -57, -49, -55, -51, -50, 0
};

//	"License"
static TCHAR g_szDeRemoteLicence_inisec_License[]	= { -77, -106, -100, -102, -111, -116, -102, 0 };

//	"gene"
static TCHAR g_szDeRemoteLicence_inikey_Gene[]		= { -104, -102, -111, -102, 0 };

//	"regcode"
static TCHAR g_szDeRemoteLicence_inikey_RegCode[]	= { -115, -102, -104, -100, -112, -101, -102, 0 };

//	"actcode"
static TCHAR g_szDeRemoteLicence_inikey_ActCode[]	= { -98, -100, -117, -100, -112, -101, -102, 0 };



/**
 *	这是一个空函数，主要用来获取自己的 hInstance 和 DLL 的全路径
 */
static VOID __cderemotelicence_global_voidfunc()
{
	return;
}




/**
 *	class of CDeRemoteLicence
 */
class CDeRemoteLicence :
	public CModuleInfoDeRemote,
	public CDeLicence
{
public:
	CDeRemoteLicence();
	virtual ~CDeRemoteLicence();

	//	验证是否是正确的注册码
	BOOL IsValidDeRemoteLicence( LPCTSTR lpcszActCode, BOOL bReLoadConfig = FALSE );

	BOOL ReLoadLicenseInfo();
	BOOL SaveLicenseInfo( STDELICENSEINFO * pstDeLicenseInfo, BOOL bForceWrite = FALSE );

private:
	BOOL BuildKey();

public:
	//	License info
	//STDELICENSEINFO m_stLicenseInfo;
	
private:
	HINSTANCE m_hDllInstance;
	TCHAR szDllFilePath[ MAX_PATH ];

	BOOL  m_bInitSucc;
	TCHAR m_szPrKey[ MAX_PATH ];

	CDeFileEncrypt m_cDeFileEnc;
	CRITICAL_SECTION m_oCriSecKeyFile;

	//	解密 INI 中的 Sec/key 等信息
	TCHAR m_szDecIniSecLicense[ 32 ];
	TCHAR m_szDecIniKeyGene[ 32 ];
	TCHAR m_szDecIniKeyRegCode[ 32 ];
	TCHAR m_szDecIniKeyActCode[ 32 ];
};


#endif // __DEREMOTELICENCE_HEADER__
