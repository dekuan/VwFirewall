// VwLicence.h: interface for the CVwLicence class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWLICENCE_HEADER__
#define __VWLICENCE_HEADER__

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



static const CVWLICENCE_VERSION_MAJOR	= 1;
static const CVWLICENCE_VERSION_MINOR	= 1;


#define CVWLICENCE_PRODUCTTYPE_COMADV		"comadv"
#define CVWLICENCE_PRODUCTTYPE_COMNORMAL	"comnormal"
#define CVWLICENCE_PRODUCTTYPE_PSLUNLMT		"pslunlmt"
#define CVWLICENCE_PRODUCTTYPE_PSLEXTREME	"pslextreme"
#define CVWLICENCE_PRODUCTTYPE_PSLADV		"psladv"
#define CVWLICENCE_PRODUCTTYPE_PSLNORMAL	"pslnormal"
#define CVWLICENCE_PRODUCTTYPE_PSLBASIC		"pslbasic"
#define CVWLICENCE_PRODUCTTYPE_PSLSMART		"pslsmart"
#define CVWLICENCE_PRODUCTTYPE_PSLSGLSITE	"pslsglsite"

#define CVWLICENCE_PRODUCTTYPE_EVALUATION	"evaluation"

#define CVWLICENCE_PRODUCTTYPE_PSLRENEW		"pslrenew"
#define CVWLICENCE_PRODUCTTYPE_UPGRADE		"upgrade"
#define CVWLICENCE_PRODUCTTYPE_REMOTEHELP	"remotehelp"

#define CVWLICENCE_PRODUCTTYPE_UNKNOWN		"unknown"


typedef struct tagVwLicencePrType
{
	LPCTSTR lpcszPrType;
	LPCTSTR lpcszChs;
	LPCTSTR lpcszEn;

}STVWLICENCEPRTYPE, *LPSTVWLICENCEPRTYPE;
static STVWLICENCEPRTYPE g_ArrVwLicencePrTypeList[] =
{
	//	must placed at index 0
	{ CVWLICENCE_PRODUCTTYPE_EVALUATION,	_T("免费评测版"),	_T("Free Evaluating Edition") },

	{ CVWLICENCE_PRODUCTTYPE_COMADV,	_T("高级公司版"),	_T("Advanced Enterprise Edition") },
	{ CVWLICENCE_PRODUCTTYPE_COMNORMAL,	_T("普通公司版"),	_T("Enterprise Edition") },
	{ CVWLICENCE_PRODUCTTYPE_PSLUNLMT,	_T("终身个人版"),	_T("Unlimited Personal Edition") },

	{ CVWLICENCE_PRODUCTTYPE_PSLEXTREME,	_T("至尊个人版"),	_T("Extreme Personal Edition") },
	{ CVWLICENCE_PRODUCTTYPE_PSLADV,	_T("高级个人版"),	_T("Advanced Personal Edition") },
	{ CVWLICENCE_PRODUCTTYPE_PSLNORMAL,	_T("普通个人版"),	_T("Personal Edition") },

	{ CVWLICENCE_PRODUCTTYPE_PSLBASIC,	_T("基础个人版"),	_T("Basic Personal Edition") },
	{ CVWLICENCE_PRODUCTTYPE_PSLSMART,	_T("精简个人版"),	_T("Smart Personal Edition") },
	{ CVWLICENCE_PRODUCTTYPE_PSLSGLSITE,	_T("单站点个人版"),	_T("Single-Site Personal Edition") },

	{ CVWLICENCE_PRODUCTTYPE_PSLRENEW,	_T("续费"),		_T("Renewal") },
	{ CVWLICENCE_PRODUCTTYPE_UPGRADE,	_T("升级"),		_T("Upgrade") },
	{ CVWLICENCE_PRODUCTTYPE_REMOTEHELP,	_T("远程协助服务"),	_T("Remote Help") },

	{ CVWLICENCE_PRODUCTTYPE_UNKNOWN,	_T("未知类型"),		_T("Unknown Type") },
};
static UINT g_nVwLicencePrTypeListCount = sizeof(g_ArrVwLicencePrTypeList)/sizeof(g_ArrVwLicencePrTypeList[0]);






/**
 *	class of CVwLicence
 */
class CVwLicence  
{
public:
	CVwLicence();
	virtual ~CVwLicence();

public:
	BOOL IsValidLicense( LPCTSTR lpcszPrName, LPCTSTR lpcszRegIp, LPCTSTR lpcszRegKey );
	BOOL IsValidPrTypeChecksum( LPCTSTR lpcszPrTypeChecksum, LPCTSTR lpcszRegPrType, LPCTSTR lpcszRegIp, LPCTSTR lpcszRegKey );
	BOOL IsValidPrTypeChecksumV4( LPCTSTR lpcszPrTypeChecksum, LPCTSTR lpcszRegPrType, LPCTSTR lpcszRegIp, LPCTSTR lpcszRegKey, LPCTSTR lpcszExpireDate );

	BOOL GetPrTypeNameByCode( LPCTSTR lpcszPrType, LPTSTR lpszPrTypeName, DWORD dwSize );
	BOOL GetComputerHardwareID( LPTSTR lpszHID, DWORD dwSize, BOOL bCreateWithDiskSN = TRUE );

private:
	BOOL CheckDllRegKey( LPCTSTR lpcszPrName, LPCTSTR lpcszRegIp, LPCTSTR lpcszRegKey );
	BOOL GetScanfCode( LPCTSTR lpcszRegKey, LPTSTR lpszScanfCode, DWORD dwSize );
	BOOL IsLocalAddr( LPCTSTR lpcszIpAddr );
	BOOL GetStringMd5( LPCTSTR lpszString, LPTSTR lpszMd5, DWORD dwSize, DWORD dwSpecStringLen = 0 );
	

public:
	TCHAR m_szDecCVwLicence_PrType_comadv[ 32 ];
	TCHAR m_szDecCVwLicence_PrType_comnormal[ 32 ];
	TCHAR m_szDecCVwLicence_PrType_pslunlmt[ 32 ];
	TCHAR m_szDecCVwLicence_PrType_psladv[ 32 ];
	TCHAR m_szDecCVwLicence_PrType_pslnormal[ 32 ];

private:
	TCHAR m_szPrNameVwInfoMonitor[ 64 ];
	TCHAR m_szPrNameVwPanDomain[ 64 ];
	TCHAR m_szPrNameVwCmAntiLeech[ 64 ];
	TCHAR m_szPrNameVwFirewall[ 64 ];
	TCHAR m_szPrNameRefererGuard[ 64 ];
	TCHAR m_szDecCVwLicence_PrTypeFmt[ 32 ];
	TCHAR m_szDecCVwLicence_PrTypeFmtV4[ 32 ];
};


#endif // __VWLICENCE_HEADER__
