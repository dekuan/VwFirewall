// CVwPanDmNewConfigFile.h: interface for the CVwPanDmNewConfigFile class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWINFOMONITORCONFIGFILE_HEADER__
#define __VWINFOMONITORCONFIGFILE_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "VwConst.h"
#include "ModuleInfo.h"
#include "VwPanDmNewDrvIOCtl.h"
#include "VwConstBase.h"

#include <vector>
using namespace std;

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )


/**
 *	定义配置文件
 */
#define CVWPANDMNEWCONFIGFILE_INI_DOMAIN_MAIN		"Main"
#define CVWPANDMNEWCONFIGFILE_INI_DOMAIN_LICENSE	"License"
#define CVWPANDMNEWCONFIGFILE_INI_DOMAIN_DOMAIN		"Domain"
#define CVWPANDMNEWCONFIGFILE_INI_DOMAIN_SYSHOST	"SysHost"

#define CVWPANDMNEWCONFIGFILE_INI_KEY_START		"start"
#define CVWPANDMNEWCONFIGFILE_INI_KEY_DOMAIN		"domain"
#define CVWPANDMNEWCONFIGFILE_INI_KEY_SYSHOST		"syshost"

#define CVWPANDMNEWCONFIGFILE_CFGVALNAME_USE		"use:"
#define CVWPANDMNEWCONFIGFILE_CFGVALNAME_DM		"dm:"
#define CVWPANDMNEWCONFIGFILE_CFGVALNAME_ECPEXT		"ecpext:"
#define CVWPANDMNEWCONFIGFILE_CFGVALNAME_MAPSDD		"mapsdd:"

#define CVWPANDMNEWCONFIGFILE_INI_KEY_REGIP		"reg_ip"
#define CVWPANDMNEWCONFIGFILE_INI_KEY_REGHOST		"reg_host"
#define CVWPANDMNEWCONFIGFILE_INI_KEY_REGKEY		"reg_key"
#define CVWPANDMNEWCONFIGFILE_INI_KEY_REGPRTYPE		"reg_prtype"
#define CVWPANDMNEWCONFIGFILE_INI_KEY_REGPRTYPECS	"reg_prtypecs"
#define CVWPANDMNEWCONFIGFILE_INI_KEY_REGEXPIRETYPE	"reg_expire_type"
#define CVWPANDMNEWCONFIGFILE_INI_KEY_REGCREATEDATE	"reg_create_date"
#define CVWPANDMNEWCONFIGFILE_INI_KEY_REGEXPIREDATE	"reg_expire_date"


/**
 *	struct
 */
typedef struct tagVwPanDmNewDomain
{
	BOOL	bUse;					//	是否启用
	CHAR    szDomain[ MAX_PATH ];			//	需要处理的域名，例如：.abc.com
	USHORT  uDomainLength;				//	需要处理的域名的长度
	CHAR    szExceptedSubDomain[ 32 ][ 32 ];	//	例外子域名
	CHAR    szExceptedSubDomainString[ MAX_PATH ];	//	例外子域名
	USHORT  uExceptedSubDomainCount;		//	例外子域名的个数
	BOOL	bMapSubDomainDir;			//	是否映射子域名目录，例如 xing.abc.com -> abc.com/xing/

}STVWPANDMNEWDOMAIN, *LPSTVWPANDMNEWDOMAIN;

typedef struct tagVwPanDmNewRegDate
{
	tagVwPanDmNewRegDate()
	{
		memset( this, 0, sizeof(tagVwPanDmNewRegDate) );
	}

	INT nRegYear;
	INT nRegMonth;
	INT nRegDay;

	INT nExpireYear;
	INT nExpireMonth;
	INT nExpireDay;

}STVWPANDMNEWREGDATE, *LPSTVWPANDMNEWREGDATE;

typedef struct tagVwPanDmNewConfig
{
	tagVwPanDmNewConfig()
	{
		memset( this, 0, sizeof(tagVwPanDmNewConfig) );
	}

	BOOL  bReged;				//	是否注册版本
	BOOL  bStart;				//	是否启动监控

	TCHAR szRegIp[ 64 ];			//	注册 IP 地址
	TCHAR szRegHost[ MAX_PATH ];		//	注册 HOST
	TCHAR szRegKey[ MAX_PATH ];		//	注册码
	TCHAR szRegPrType[ 32 ];		//	产品类型
	TCHAR szRegPrTypeChecksum[ 64 ];	//	产品类型的校验值
	DWORD dwRegExpireType;			//	过期类型
	TCHAR szRegCreateDate[ MAX_PATH ];	//	注册时间
	TCHAR szRegExpireDate[ MAX_PATH ];	//	过期时间

	DWORD dwDomainCount;			//	有通配符号的例外域名的个数
	STVWPANDMNEWDOMAIN * pstDomain;		//	指向一个例外域名数组

} STVWPANDMNEWCONFIG, *LPSTVWPANDMNEWCONFIG;


//
//	qsort/bsearch compare function for the list of tagVwPanDmNewDomain
//
static int _qsortCmpFunc_tagVwPanDmNewDomain( const void * l, const void * r )
{
	return strlen( ((STVWPANDMNEWDOMAIN*)l)->szDomain ) - strlen( ((STVWPANDMNEWDOMAIN*)r)->szDomain );
}


/**
 *	class of CVwPanDmNewConfigFile
 */
class CVwPanDmNewConfigFile :
	public CModuleInfo
{
public:
	CVwPanDmNewConfigFile();
	virtual ~CVwPanDmNewConfigFile();

public:
	BOOL LoadConfig( STVWPANDMNEWCONFIG * pstConfig );
	UINT LoadConfigUInt( LPCTSTR lpcszDomain, LPCTSTR lpcszKey );
	BOOL LoadConfigString( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPTSTR lpszString, DWORD dwSize );
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
	TCHAR m_szSysDriverVersion[ MAX_PATH ];

	TCHAR m_szLocDriverFile_wnet_x86[ MAX_PATH ];
	TCHAR m_szLocDriverFile_wnet_amd64[ MAX_PATH ];

	TCHAR m_szISAPIFilterName[ 64 ];
	TCHAR m_szISAPIFilterDesc[ 64 ];
	TCHAR m_szISAPIFilterDllName[ 64 ];
	TCHAR m_szISAPIFilterFile[ MAX_PATH ];

	TCHAR m_szDllFileDeLib[ MAX_PATH ];
};



#endif	//	__VWINFOMONITORCONFIGFILE_HEADER__


