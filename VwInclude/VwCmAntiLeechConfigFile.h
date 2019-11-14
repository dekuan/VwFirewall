// VwCmAntiLeechConfigFile.h: interface for the CVwCmAntiLeechConfigFile class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWCMANTILEECHCONFIGFILE_HEADER__
#define __VWCMANTILEECHCONFIGFILE_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "VwConst.h"
#include "ModuleInfo.h"
#include "VwCmAntiLeechDrvIOCtl.h"

#include <vector>
using namespace std;

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )


/**
 *	定义配置文件
 */
#define CVWCMANTILEECHCONFIGFILE_INI_DOMAIN_MAIN		"Main"
#define CVWCMANTILEECHCONFIGFILE_INI_DOMAIN_LICENSE		"License"
#define CVWCMANTILEECHCONFIGFILE_INI_DOMAIN_VSID		"VSID"
#define CVWCMANTILEECHCONFIGFILE_INI_DOMAIN_RTSP_DOMAIN		"RtspDomain"

#define CVWCMANTILEECHCONFIGFILE_INI_KEY_START			"start"
#define CVWCMANTILEECHCONFIGFILE_INI_KEY_RTSP_START		"rtsp_start"
#define CVWCMANTILEECHCONFIGFILE_INI_KEY_RTSP_DOMAIN		"rtsp_domain"
#define CVWCMANTILEECHCONFIGFILE_INI_KEY_RTSP_BLOCK_MMSD	"rtsp_block_mmsd"

#define CVWCMANTILEECHCONFIGFILE_CFGVALNAME_USE			"use:"
#define CVWCMANTILEECHCONFIGFILE_CFGVALNAME_DM			"dm:"
#define CVWCMANTILEECHCONFIGFILE_CFGVALNAME_ECPEXT		"ecpext:"

#define CVWCMANTILEECHCONFIGFILE_INI_KEY_REGIP			"reg_ip"
#define CVWCMANTILEECHCONFIGFILE_INI_KEY_REGHOST		"reg_host"
#define CVWCMANTILEECHCONFIGFILE_INI_KEY_REGKEY			"reg_key"
#define CVWCMANTILEECHCONFIGFILE_INI_KEY_REGPRTYPE		"reg_prtype"
#define CVWCMANTILEECHCONFIGFILE_INI_KEY_REGPRTYPECS		"reg_prtypecs"
#define CVWCMANTILEECHCONFIGFILE_INI_KEY_REGEXPIRETYPE		"reg_expire_type"
#define CVWCMANTILEECHCONFIGFILE_INI_KEY_REGCREATEDATE		"reg_create_date"
#define CVWCMANTILEECHCONFIGFILE_INI_KEY_REGEXPIREDATE		"reg_expire_date"

#define CVWCMANTILEECHCONFIGFILE_INI_KEY_VSID_TYPE		"vsid_type"
#define CVWCMANTILEECHCONFIGFILE_INI_KEY_VSID_USE_COOKIE	"vsid_use_cookie"
#define CVWCMANTILEECHCONFIGFILE_INI_KEY_VSID_KEY		"vsid_key"
#define CVWCMANTILEECHCONFIGFILE_INI_KEY_VSID_KEY_TIME		"vsid_key_time"

#define CVWCMANTILEECHCONFIGFILE_PUBVSIDKEY_TYPE_NORMAL		0	//	普通的默认方式
#define CVWCMANTILEECHCONFIGFILE_PUBVSIDKEY_TYPE_RANDOM		1	//	随机变化
#define CVWCMANTILEECHCONFIGFILE_PUBVSIDKEY_TYPE_FILENAME	2	//	用文件名加钥匙加密

#define CVWCMANTILEECHCONFIGFILE_DEF_PUBVSIDUSECOOKIE		1
#define CVWCMANTILEECHCONFIGFILE_DEF_PUBVSIDUSEBROWSERIN	0
#define CVWCMANTILEECHCONFIGFILE_DEF_PUBVSIDKEY			"vsid"
#define CVWCMANTILEECHCONFIGFILE_DEF_PUBVSIDKEYTIME		0	//	2.1.6.1037 以后版本就不再鼓励用户使用时间限制了，所以默认值 6000 改为 0
#define CVWCMANTILEECHCONFIGFILE_DEF_PUBKEYTYPE			CVWCMANTILEECHCONFIGFILE_PUBVSIDKEY_TYPE_NORMAL


/**
 *	struct
 */
typedef struct tagVwCmAntiLeechDomain
{
	BOOL	bUse;					//	是否启用
	CHAR    szDomain[ MAX_PATH ];			//	需要处理的域名，例如：.abc.com
	USHORT  uDomainLength;				//	需要处理的域名的长度

}STVWCMANTILEECHDOMAIN, *LPSTVWCMANTILEECHDOMAIN;

typedef struct tagVwCmAntiLeechRegDate
{
	tagVwCmAntiLeechRegDate()
	{
		memset( this, 0, sizeof(tagVwCmAntiLeechRegDate) );
	}

	INT nRegYear;
	INT nRegMonth;
	INT nRegDay;

	INT nExpireYear;
	INT nExpireMonth;
	INT nExpireDay;

}STVWCMANTILEECHREGDATE, *LPSTVWCMANTILEECHREGDATE;

typedef struct tagVwCmAntiLeechConfig
{
	tagVwCmAntiLeechConfig()
	{
		memset( this, 0, sizeof(tagVwCmAntiLeechConfig) );
	}

	BOOL  bReged;				//	是否注册版本
	//BOOL  bStart;				//	驱动是否启动
	BOOL  bRtspStart;			//	是否启动 Rtsp 协议防盗链
	BOOL  bRtspBlockMmsD;			//	Rtsp 协议，拦截下载工具

	TCHAR szRegIp[ 64 ];			//	注册 IP 地址
	TCHAR szRegHost[ MAX_PATH ];		//	注册 HOST
	TCHAR szRegKey[ MAX_PATH ];		//	注册码
	TCHAR szRegPrType[ 32 ];		//	产品类型
	TCHAR szRegPrTypeChecksum[ 64 ];	//	产品类型的校验值
	DWORD dwRegExpireType;			//	过期类型
	TCHAR szRegCreateDate[ MAX_PATH ];	//	注册时间
	TCHAR szRegExpireDate[ MAX_PATH ];	//	过期时间

	//	[VSID]
	UINT	uVsidType;			//	Vsid 计算类型：0为普通，1为随机变化，2为文件名附加模式
	BOOL	bVsidUseCookie;			//	Vsid 是否接受来自 Cookie 的 VSID 信息
	CHAR	szVsidKey[ 64 ];		//	Vsid 的公共钥匙
	UINT	uVsidKeyTime;			//	Vsid 的有效时间

	DWORD dwDomainCount;			//	有通配符号的例外域名的个数
	STVWCMANTILEECHDOMAIN * pstDomain;		//	指向一个例外域名数组

} STVWCMANTILEECHCONFIG, *LPSTVWCMANTILEECHCONFIG;


//
//	qsort/bsearch compare function for the list of tagVwPanDmNewDomain
//
static int _qsortCmpFunc_tagVwCmAntiLeechDomain( const void * l, const void * r )
{
	return strlen( ((STVWCMANTILEECHDOMAIN*)l)->szDomain ) - strlen( ((STVWCMANTILEECHDOMAIN*)r)->szDomain );
}


/**
 *	class of CVwCmAntiLeechConfigFile
 */
class CVwCmAntiLeechConfigFile :
	public CModuleInfo
{
public:
	CVwCmAntiLeechConfigFile();
	virtual ~CVwCmAntiLeechConfigFile();

public:
	BOOL LoadConfig( STVWCMANTILEECHCONFIG * pstConfig );
	UINT LoadConfigUInt( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, INT nDefault = 0 );
	BOOL LoadConfigString( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPTSTR lpszString, DWORD dwSize, LPCTSTR lpDefault = NULL );
	BOOL SaveConfig( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPCTSTR lpcszValue );
	BOOL SaveConfigUInt( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, UINT uValue );

private:
	BOOL InitModule();

protected:
	BOOL  m_bInitSucc;

	TCHAR m_szSysDriverDir[ MAX_PATH ];
	TCHAR m_szConfDir[ MAX_PATH ];
	TCHAR m_szLogsDir[ MAX_PATH ];

	TCHAR m_szCfgFile[ MAX_PATH ];
	TCHAR m_szKeyFile[ MAX_PATH ];
	TCHAR m_szSysDriverFile[ MAX_PATH ];
	TCHAR m_szLocDriverFile[ MAX_PATH ];
	TCHAR m_szSysDriverVersion[ MAX_PATH ];

	TCHAR m_szISAPIFilterName[ 64 ];
	TCHAR m_szISAPIFilterDesc[ 64 ];
	TCHAR m_szISAPIFilterDllName[ 64 ];
	TCHAR m_szISAPIFilterFile[ MAX_PATH ];

};



#endif	//	__VWCMANTILEECHCONFIGFILE_HEADER__


