// CVwFirewallConfigFile.h: interface for the CVwFirewallConfigFile class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWDMFIREWALLCONFIGFILE_HEADER__
#define __VWDMFIREWALLCONFIGFILE_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "VwConst.h"
#include "ModuleInfo.h"
#include "VwFirewallDrvIOCtl.h"
#include "VwConstBase.h"

#include <vector>
using namespace std;

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )


/**
 *	定义配置文件
 */
#define CVWFIREWALLCONFIGFILE_INI_DOMAIN_MAIN			"Main"
#define CVWFIREWALLCONFIGFILE_INI_DOMAIN_LICENSE		"License"
#define CVWFIREWALLCONFIGFILE_INI_DOMAIN_DOMAIN			"Domain"
#define CVWFIREWALLCONFIGFILE_INI_DOMAIN_RDPCLIENTNAME		"RdpClientName"
#define CVWFIREWALLCONFIGFILE_INI_DOMAIN_PROTECTEDDIR		"ProtectedDir"
#define CVWFIREWALLCONFIGFILE_INI_DOMAIN_EXCEPTEDPROCESS	"ExceptedProcess"
#define CVWFIREWALLCONFIGFILE_INI_DOMAIN_SYSHOST		"SysHost"
#define CVWFIREWALLCONFIGFILE_INI_DOMAIN_APPINFO		"AppInfo"

#define CVWFIREWALLCONFIGFILE_INI_KEY_START			"start"
#define CVWFIREWALLCONFIGFILE_INI_KEY_STARTFILESAFE		"startfilesafe"
#define CVWFIREWALLCONFIGFILE_INI_KEY_STARTPRDP			"startprdp"
#define CVWFIREWALLCONFIGFILE_INI_KEY_STARTPDOMAIN		"startpdomain"
#define CVWFIREWALLCONFIGFILE_INI_KEY_DOMAIN			"domain"
#define CVWFIREWALLCONFIGFILE_INI_KEY_SYSHOST			"syshost"
#define CVWFIREWALLCONFIGFILE_INI_KEY_RDP_CLIENTNAME		"rdpclientname"
#define CVWFIREWALLCONFIGFILE_INI_KEY_PROTECTEDDIR		"protecteddir"
#define CVWFIREWALLCONFIGFILE_INI_KEY_EXCEPTEDPROCESS		"exceptedprocess"
#define CVWFIREWALLCONFIGFILE_INI_KEY_APP_INSDIR		"app_insdir"
#define CVWFIREWALLCONFIGFILE_INI_KEY_APP_LOGDIR		"app_logdir"

#define CVWFIREWALLCONFIGFILE_CFGVALNAME_USE			"use:"
#define CVWFIREWALLCONFIGFILE_CFGVALNAME_ITEM			"item:"
#define CVWFIREWALLCONFIGFILE_CFGVALNAME_DM			"dm:"
#define CVWFIREWALLCONFIGFILE_CFGVALNAME_WCD			"wcd:"		//	Wildcard

#define CVWFIREWALLCONFIGFILE_CFGVALNAME_DIR			"dir:"		//	for [ProtectedDir]
#define CVWFIREWALLCONFIGFILE_CFGVALNAME_TREE			"tree:"		//	for [ProtectedDir]
#define CVWFIREWALLCONFIGFILE_CFGVALNAME_ALLEXT			"allext:"	//	for [ProtectedDir]
#define CVWFIREWALLCONFIGFILE_CFGVALNAME_PROTEXT		"protext:"	//	for [ProtectedDir]
#define CVWFIREWALLCONFIGFILE_CFGVALNAME_ECPEXT			"ecpext:"	//	for [ProtectedDir]

#define CVWFIREWALLCONFIGFILE_INI_KEY_REGIP			"reg_ip"
#define CVWFIREWALLCONFIGFILE_INI_KEY_REGHOST			"reg_host"
#define CVWFIREWALLCONFIGFILE_INI_KEY_REGKEY			"reg_key"
#define CVWFIREWALLCONFIGFILE_INI_KEY_REGPRTYPE			"reg_prtype"
#define CVWFIREWALLCONFIGFILE_INI_KEY_REGPRTYPECS		"reg_prtypecs"
#define CVWFIREWALLCONFIGFILE_INI_KEY_REGEXPIRETYPE		"reg_expire_type"
#define CVWFIREWALLCONFIGFILE_INI_KEY_REGCREATEDATE		"reg_create_date"
#define CVWFIREWALLCONFIGFILE_INI_KEY_REGEXPIREDATE		"reg_expire_date"


/**
 *	struct
 */
typedef struct tagVwFirewallDomain
{
	BOOL	bUse;					//	是否启用
	CHAR    szDomain[ MAX_PATH ];			//	需要处理的域名，例如：.abc.com
	USHORT  uDomainLength;				//	需要处理的域名的长度
	BOOL	bWildcard;				//	是否是泛域名

}STVWFIREWALLDOMAIN, *LPVWFIREWALLDOMAIN;

typedef struct tagRdpClientName
{
	BOOLEAN bUse;
	TCHAR   szClientName[ 16 ];
	USHORT  uLength;

}STRDPCLIENTNAME, *LPSTRDPCLIENTNAME;

typedef struct tagProtectedDir
{
	BOOL   bUse;					//	是否启用
	TCHAR  szDir[ MAX_PATH ];			//	被保护目录
	BOOL   bProtSubDir;				//	是否保护其子目录	
	BOOL   bProtAllExt;				//	是否要保护所有的扩展名，TRUE 是，FALSE 不是，仅仅保护 uszProtectedExt 里指定的
	TCHAR  szExceptedExt[ 1024 ];			//	

}STPROTECTEDDIR, *LPSTPROTECTEDDIR;

typedef struct tagExceptedProcess
{
	BOOL   bUse;					//	是否启用
	TCHAR  szPath[ MAX_PATH ];			//	例外进程的路径

}STEXCEPTEDPROCESS, *LPSTEXCEPTEDPROCESS;

typedef struct tagVwFirewallRegDate
{
	tagVwFirewallRegDate()
	{
		memset( this, 0, sizeof(tagVwFirewallRegDate) );
	}

	INT nRegYear;
	INT nRegMonth;
	INT nRegDay;

	INT nExpireYear;
	INT nExpireMonth;
	INT nExpireDay;

}STVWFIREWALLREGDATE, *LPSTVWFIREWALLREGDATE;

typedef struct tagVwFirewallConfig
{
	tagVwFirewallConfig()
	{
		memset( this, 0, sizeof(tagVwFirewallConfig) );
	}

	BOOL  bReged;				//	是否注册版本
	BOOL  bStart;				//	是否启动防火墙
	BOOL  bStartProtectFile;		//	开启域名锁
	BOOL  bStartProtectDomain;		//	开启域名白名单
	BOOL  bStartProtectRdp;			//	开启保护 RDP

	TCHAR szRegIp[ 64 ];			//	注册 IP 地址
	TCHAR szRegHost[ MAX_PATH ];		//	注册 HOST
	TCHAR szRegKey[ MAX_PATH ];		//	注册码
	TCHAR szRegPrType[ 32 ];		//	产品类型
	TCHAR szRegPrTypeChecksum[ 64 ];	//	产品类型的校验值
	DWORD dwRegExpireType;			//	过期类型
	TCHAR szRegCreateDate[ MAX_PATH ];	//	注册时间
	TCHAR szRegExpireDate[ MAX_PATH ];	//	过期时间

	DWORD dwDomainCount;			//	有通配符号的例外域名的个数
	STVWFIREWALLDOMAIN * pstDomain;		//	指向一个例外域名数组

} STVWFIREWALLCONFIG, *LPSTVWFIREWALLCONFIG;


//
//	qsort/bsearch compare function for the list of tagVwDmFirewallDomain
//
static int _qsortCmpFunc_tagVwDmFirewallDomain( const void * l, const void * r )
{
	return strlen( ((STVWFIREWALLDOMAIN*)l)->szDomain ) - strlen( ((STVWFIREWALLDOMAIN*)r)->szDomain );
}


/**
 *	class of CVwFirewallConfigFile
 */
class CVwFirewallConfigFile :
	public CModuleInfo
{
public:
	CVwFirewallConfigFile();
	virtual ~CVwFirewallConfigFile();

public:
	BOOL LoadConfig( STVWFIREWALLCONFIG * pstConfig );

	UINT LoadConfigUInt( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPCTSTR lpcszFilename = NULL );
	BOOL LoadConfigString( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPTSTR lpszString, DWORD dwSize, LPCTSTR lpcszFilename = NULL );
	BOOL SaveConfig( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPCTSTR lpcszValue, LPCTSTR lpcszFilename = NULL );
	BOOL SaveConfigUInt( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, UINT uValue, LPCTSTR lpcszFilename = NULL );

private:
	BOOL InitModule();

public:
	BOOL  m_bInitSucc;

	TCHAR m_szSysDriverDir[ MAX_PATH ];
	TCHAR m_szConfDir[ MAX_PATH ];
	TCHAR m_szLogsDir[ MAX_PATH ];
	TCHAR m_szScriptDir[ MAX_PATH ];
	TCHAR m_szBackupDir[ MAX_PATH ];
	TCHAR m_szBackupObjectDir[ MAX_PATH ];
	TCHAR m_szBackupServiceDir[ MAX_PATH ];

	TCHAR m_szCfgFile[ MAX_PATH ];
	TCHAR m_szKeyFile[ MAX_PATH ];
	TCHAR m_szSysDriverFile[ MAX_PATH ];
	TCHAR m_szSysDriverVersion[ MAX_PATH ];

	TCHAR m_szSys_Wshom_ocx[ MAX_PATH ];


	TCHAR m_szLocDriverFile_wnet_x86[ MAX_PATH ];
	TCHAR m_szLocDriverFile_wnet_amd64[ MAX_PATH ];

	TCHAR m_szISAPIFilterName[ 64 ];
	TCHAR m_szISAPIFilterDesc[ 64 ];
	TCHAR m_szISAPIFilterDllName[ 64 ];
	TCHAR m_szISAPIFilterFile[ MAX_PATH ];

	TCHAR m_szDllFileDeLib[ MAX_PATH ];

	TCHAR m_szCfgAclsFileData[ MAX_PATH ];
	TCHAR m_szCfgAclsFolderData[ MAX_PATH ];
	TCHAR m_szCfgAclsAntiVirusData[ MAX_PATH ];
	TCHAR m_szCfgServiceData[ MAX_PATH ];
	TCHAR m_szCfgObjectData[ MAX_PATH ];
};



#endif	//	__VWDMFIREWALLCONFIGFILE_HEADER__


