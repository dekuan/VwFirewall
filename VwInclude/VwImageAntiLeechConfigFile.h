// CVwImageAntiLeechConfigFile.h: interface for the CVwImageAntiLeechConfigFile class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWIMAGEANTILEECHCONFIGFILE_HEADER__
#define __VWIMAGEANTILEECHCONFIGFILE_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "VwConst.h"
#include "ModuleInfo.h"



#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )

#include "DeLibDrv.h"
#pragma comment( lib, "DeLibDrv.lib" )

#include <vector>
#include <set>
using namespace std;




/**
 *	过滤器/扩展的名称以及描述
 */
#define VWINFOM_FILTER_NAME		"VwInfoM"
#define VWINFOM_FILTER_DESC		"VirtualWall.VwInfoMonitor"

#define VWINFOM_EXTENSION_NAME		"VwInfoMExt"
#define VWINFOM_EXTENSION_DESC		"VirtualWall.VwInfoMonitor.Extension"


/**
 *	定义配置文件
 */
#define CVWIMAGEANTILEECHCONFIGFILE_FILE_FILTER				"VwInfoMonitor.dll"
#define CVWIMAGEANTILEECHCONFIGFILE_FILE_DELIB				"DeLib.dll"

#define CVWIMAGEANTILEECHCONFIGFILE_CONFFILE_CFG			"cfg.ini"


#define CVWIMAGEANTILEECHCONFIGFILE_MAX_WORDLOADCOUNT		5000	//	从一个文件中最大装载词汇数
#define CVWIMAGEANTILEECHCONFIGFILE_MAX_SQLMPLOADCOUNT		500	//	最大SQL匹配规则个数

#define CVWIMAGEANTILEECHCONFIGFILE_MIN_WORDLEN			4	//	词的最小长度
#define CVWIMAGEANTILEECHCONFIGFILE_MAX_WORDLEN			80	//	词的最大长度
#define CVWIMAGEANTILEECHCONFIGFILE_MAX_SQLMPLEN		100	//	SQL匹配规则的最大长度

#define CVWIMAGEANTILEECHCONFIGFILE_INI_DOMAIN_MAIN		"Main"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_DOMAIN_LICENSE		"License"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_DOMAIN_TICKINFO		"TickInfo"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_DOMAIN_HOST		"Host"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_DOMAIN_SITE		"Site"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_DOMAIN_ITEMLIST		"ItemList"

#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_START		"start"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_LOG			"log"

#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_REGIP		"RegIp"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_REGHOST		"RegHost"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_REGKEY		"RegKey"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_REGEXPIRETYPE	"RegExpireType"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_REGCREATEDATE	"RegCreateDate"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_REGEXPIREDATE	"RegExpireDate"

#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_HOST		"Host"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_SITE		"Site"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_ITEMNAME		"ItemName"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_ITEMURL		"ItemUrl"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_ITEMID		"ItemID"

#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_JPG			"Jpg"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_JPEG		"Jpeg"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_GIF			"Gif"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_PNG			"Png"
#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_BMP			"Bmp"


#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_TICKINFO_CTWT	"CTWT"		//	continue to work today
#define CVWIMAGEANTILEECHCONFIGFILE_INI_KEY_TICKINFO_TICK	"TICK"		//	tickcount

#define CVWIMAGEANTILEECHCONFIGFILE_CFGVALNAME_USE		"use:"
#define CVWIMAGEANTILEECHCONFIGFILE_CFGVALNAME_HST		"hst:"
#define CVWIMAGEANTILEECHCONFIGFILE_CFGVALNAME_PRTEXT		"prtext:"
#define CVWIMAGEANTILEECHCONFIGFILE_CFGVALNAME_ITEMC		"itemc:"
#define CVWIMAGEANTILEECHCONFIGFILE_CFGVALNAME_ISREP		"isrep:"
#define CVWIMAGEANTILEECHCONFIGFILE_CFGVALNAME_REPFILE		"repfile:"


/**
 *	Protected File Types
 */
enum
{
	VWIMAGEANTILEECH_FILETYPE_COUNT	= 5,
	VWIMAGEANTILEECH_FILETYPE_NONE	= 0,
	VWIMAGEANTILEECH_FILETYPE_JPG	= 1,
	VWIMAGEANTILEECH_FILETYPE_JPEG	= 2,
	VWIMAGEANTILEECH_FILETYPE_GIF	= 4,
	VWIMAGEANTILEECH_FILETYPE_PNG	= 8,
	VWIMAGEANTILEECH_FILETYPE_BMP	= 16,
};


/**
 *	action
 *	注意: 为了节约内存，所有 action 的值加起来，总和不能超过 255
 */
#define VWINFOM_ACTION_NONE			0	//	无任何操作
#define VWINFOM_ACTION_PROTECTED		1
#define VWINFOM_ACTION_OUT_REPLACE		1	//	流出: 替换过滤
#define VWINFOM_ACTION_OUT_BLOCK		2	//	流出: 阻止提示(暂时没用)
#define VWINFOM_ACTION_IN_REPLACE		4	//	流入: 替换过滤(暂时没用)
#define VWINFOM_ACTION_IN_BLOCK			8	//	流入: 阻止提示
#define VWINFOM_ACTION_IN_BLOCK_POST		16	//	超大文件上传阻止提示
#define VWINFOM_ACTION_IN_ANTISQL		32	//	内容中含有危险 SQL 语句
#define VWINFOM_ACTION_ALERT_OUT_REPLACE	64	//	流出过滤提示
#define VWINFOM_ACTION_ALL			( VWINFOM_ACTION_OUT_REPLACE | VWINFOM_ACTION_IN_BLOCK )


typedef struct tagVwImageAntiLeechReplace
{
	tagVwImageAntiLeechReplace()
	{
		memset( this, 0, sizeof(tagVwImageAntiLeechReplace) );
	}
	DWORD dwType;
	BOOL  bIsReplace;
	BOOL  bFileExist;
	TCHAR szReplaceFile[ MAX_PATH ];

}STVWIMAGEANTILEECHREPLACE, *LPSTVWIMAGEANTILEECHREPLACE;

typedef struct tagVwImageAntiLeechHost
{
	tagVwImageAntiLeechHost()
	{
		memset( this, 0, sizeof(tagVwImageAntiLeechHost) );
	}
	
	BOOL  bUse;
	BOOL  bWildcard;
	TCHAR szHost[ 128 ];
	DWORD dwPrtExt;

}STVWIMAGEANTILEECHHOST, *LPVWIMAGEANTILEECHHOST;

typedef struct tagVwImageAntiLeechHostList
{
	tagVwImageAntiLeechHostList()
	{
		memset( this, 0, sizeof(tagVwImageAntiLeechHostList) );
	}

	STVWIMAGEANTILEECHHOST stHost;
	STVWIMAGEANTILEECHREPLACE ArrReplaceFiles[ VWIMAGEANTILEECH_FILETYPE_COUNT ];

}STVWIMAGEANTILEECHHOSTLIST, *LPSTVWIMAGEANTILEECHHOSTLIST;

typedef struct tagVwImageAntiLeechItem
{
	tagVwImageAntiLeechItem()
	{
		memset( this, 0, sizeof(tagVwImageAntiLeechItem) );
	}

	TCHAR szItemID[ 32 ];
	TCHAR szItemUrl[ MAX_PATH ];
	TCHAR szItemName[ MAX_PATH ];

}STVWIMAGEANTILEECHITEM, *LPSTVWIMAGEANTILEECHITEM;

typedef struct tagVwImageAntiLeechItemID
{
	tagVwImageAntiLeechItemID()
	{
		memset( this, 0, sizeof(tagVwImageAntiLeechItemID) );
	}

	TCHAR szItemID[ 32 ];

}STVWIMAGEANTILEECHITEMID, *LPSTVWIMAGEANTILEECHITEMID;

struct __lessCmpFunc_tagVwImageAntiLeechItemID
{
	bool operator()( const STVWIMAGEANTILEECHITEMID & l, const STVWIMAGEANTILEECHITEMID & r ) const
	{
		return ( _tcsicmp( l.szItemID, r.szItemID ) < 0 ? TRUE : FALSE );
	}
};
typedef set<STVWIMAGEANTILEECHITEMID,__lessCmpFunc_tagVwImageAntiLeechItemID> _SET_ITEMID;
typedef set<STVWIMAGEANTILEECHITEMID,__lessCmpFunc_tagVwImageAntiLeechItemID>::iterator _SET_ITEMID_IT;

typedef struct tagVwImageAntiLeechSite
{
	tagVwImageAntiLeechSite()
	{
		Initalize();
	}
	VOID Initalize()
	{
		bUse		= FALSE;
		dwItemCount	= 0;
		memset( szHost, 0, sizeof(szHost) );
		setItemIDList.clear();
	}

	BOOL  bUse;
	TCHAR szHost[ MAX_PATH ];
	DWORD dwItemCount;

	set<STVWIMAGEANTILEECHITEMID,__lessCmpFunc_tagVwImageAntiLeechItemID> setItemIDList;
	set<STVWIMAGEANTILEECHITEMID,__lessCmpFunc_tagVwImageAntiLeechItemID>::iterator itItemIDList;

}STVWIMAGEANTILEECHSITE, *LPSTVWIMAGEANTILEECHSITE;


typedef struct tagVwImageAntiLeechConfigTicketInfo
{
	BOOL bContinueWork;
	DWORD dwStartTicket;
	DWORD dwDay;
	__int64 _n64LimitedDataTransfer;	//	个人版本限制大小
	__int64 _n64DataTransfer;		//	9223372036854775807 字节 = 8589934591 G字节

}STVWIMAGEANTILEECHCONFIGTICKETINFO, *LPSTVWIMAGEANTILEECHCONFIGTICKETINFO;

typedef struct tagVwImageAntiLeechConfig
{
	tagVwImageAntiLeechConfig()
	{
		memset( this, 0, sizeof(tagVwImageAntiLeechConfig) );
	}

	STVWIMAGEANTILEECHCONFIGTICKETINFO stTickInfo;

	BOOL  bReged;						//	是否注册版本
	BOOL  bStart;						//	是否启动监控
	BOOL  bLog;						//	是否记录日志

	DWORD dwFilterFlags;					//	Filter 部分的 Flag 重要参数

	TCHAR szRegIp[ 64 ];					//	注册 IP 地址
	TCHAR szRegHost[ MAX_PATH ];				//	注册 HOST
	TCHAR szRegKey[ MAX_PATH ];				//	注册码
	DWORD dwRegExpireType;					//	过期类型
	TCHAR szRegCreateDate[ MAX_PATH ];			//	注册时间
	TCHAR szRegExpireDate[ MAX_PATH ];			//	过期时间



} STVWIMAGEANTILEECHCONFIG, *LPSTVWIMAGEANTILEECHCONFIG;

typedef struct tagVwImageAntiLeechConfigData
{
	tagVwImageAntiLeechConfigData()
	{
		Initialize();
	}
	~tagVwImageAntiLeechConfigData()
	{
		DeleteCriticalSection( & sm_oCrSecTree );
	}
	VOID Initialize()
	{
		InitializeCriticalSection( & sm_oCrSecTree );
		
		sm_uCurrTreeIndex = 0;
		
		vcProtectedHostList[ 0 ].clear();
		vcProtectedHostList[ 1 ].clear();
		pvcCurrentProtectedHostList = & vcProtectedHostList[ 0 ];
		pvcOfflineProtectedHostList = & vcProtectedHostList[ 1 ];
		
		vcExceptedSite[ 0 ].clear();
		vcExceptedSite[ 1 ].clear();
		pvcCurrentExceptedSite = & vcExceptedSite[ 0 ];
		pvcOfflineExceptedSite = & vcExceptedSite[ 1 ];
	}
	UINT GetOnlineTreeIndex()
	{
		return sm_uCurrTreeIndex;
	}
	UINT GetOfflineTreeIndex()
	{
		return ( ( sm_uCurrTreeIndex + 1 ) % 2 );
	}
	vector<STVWIMAGEANTILEECHHOSTLIST> * GetProtectedHostList( BOOL bOnline )
	{
		vector<STVWIMAGEANTILEECHHOSTLIST> * pvcRet;

		//	...
		EnterCriticalSection( & sm_oCrSecTree );

		pvcRet = bOnline ? pvcCurrentProtectedHostList : pvcOfflineProtectedHostList;

		//	...
		LeaveCriticalSection( & sm_oCrSecTree );

		return pvcRet;
	}
	vector<STVWIMAGEANTILEECHSITE> * GetExceptedSite( BOOL bOnline )
	{
		vector<STVWIMAGEANTILEECHSITE> * pvcRet;

		//	...
		EnterCriticalSection( & sm_oCrSecTree );
		
		pvcRet = bOnline ? pvcCurrentExceptedSite : pvcOfflineExceptedSite;
		
		//	...
		LeaveCriticalSection( & sm_oCrSecTree );

		return pvcRet;
	}
	VOID SwitchTrees()
	{
		//	...
		EnterCriticalSection( & sm_oCrSecTree );

		//
		//	获取当前树的索引值
		//	离线树索引值 -> 赋给 -> 当前在线树的索引值
		//
		sm_uCurrTreeIndex = GetOfflineTreeIndex();

		//
		//	将 离线树的指针 直接指向 当前树指针
		//
		pvcCurrentProtectedHostList = & vcProtectedHostList[ sm_uCurrTreeIndex ];
		pvcOfflineProtectedHostList = & vcProtectedHostList[ GetOfflineTreeIndex() ];

		pvcCurrentExceptedSite = & vcExceptedSite[ sm_uCurrTreeIndex ];
		pvcOfflineExceptedSite = & vcExceptedSite[ GetOfflineTreeIndex() ];

		//	...
		LeaveCriticalSection( & sm_oCrSecTree );
	}

	//	Index of tree
	UINT sm_uCurrTreeIndex;
	CRITICAL_SECTION sm_oCrSecTree;


	//	受保护的 HOST 列表
	vector<STVWIMAGEANTILEECHHOSTLIST> vcProtectedHostList[ 2 ];
	vector<STVWIMAGEANTILEECHHOSTLIST> * pvcCurrentProtectedHostList;
	vector<STVWIMAGEANTILEECHHOSTLIST> * pvcOfflineProtectedHostList;

	//	例外的网店
	vector<STVWIMAGEANTILEECHSITE> vcExceptedSite[ 2 ];
	vector<STVWIMAGEANTILEECHSITE> * pvcCurrentExceptedSite;
	vector<STVWIMAGEANTILEECHSITE> * pvcOfflineExceptedSite;

}STVWIMAGEANTILEECHCONFIGDATA, *LPSTVWIMAGEANTILEECHCONFIGDATA;


typedef struct tagVwInfoMDictFile
{
	//	流出
	TCHAR szOutRepWordsFile[ MAX_PATH ];
	TCHAR szOutRepWordsFile_utf8[ MAX_PATH ];
	TCHAR szOutBlockWordsFile[ MAX_PATH ];		//	暂不使用

	//	流入
	TCHAR szInRepWordsFile[ MAX_PATH ];		//	暂不使用
	TCHAR szInBlockWordsFile[ MAX_PATH ];		//	流入过滤词
	TCHAR szInBlockWordsFile_escape[ MAX_PATH ];	//	流入过滤词 escape 编码后版本
	TCHAR szInBlockWordsFile_utf8[ MAX_PATH ];	//	流入过滤词 utf8 版本
	TCHAR szInBlockSqlWordsFile[ MAX_PATH ];	//	防 SQL 注入

	//	例外域名
	TCHAR szExceptionHostsFile[ MAX_PATH ];

	TCHAR szInBlockAlertFile[ MAX_PATH ];		//	流入警告文件
	TCHAR szInBlockSqlAlertFile[ MAX_PATH ];	//	防SQL注入警告文件


	//
	//	词典文件
	//

	//	流出
	TCHAR szOutRepWordsDict[ MAX_PATH ];
	TCHAR szOutRepWordsDict_utf8[ MAX_PATH ];
	TCHAR szOutBlockWordsDict[ MAX_PATH ];	//	暂不使用
	
	//	流入
	TCHAR szInRepWordsDict[ MAX_PATH ];		//	暂不使用
	TCHAR szInBlockWordsDict[ MAX_PATH ];		//	流入过滤词
	TCHAR szInBlockWordsDict_escape[ MAX_PATH ];	//	流入过滤词 escape 编码后版本
	TCHAR szInBlockWordsDict_utf8[ MAX_PATH ];	//	流入过滤词 utf-8 版本

	//	例外域名
	TCHAR szExceptionHostsDict[ MAX_PATH ];

}STVWINFOMDICTFILE, *LPSTVWINFOMDICTFILE;



//
//	qsort/bsearch compare function for the list of tagVwImageAntiLeechHost
//
static int _qsortCmpFunc_tagVwImageAntiLeechHost( const void * l, const void * r )
{
	if ( ((STVWIMAGEANTILEECHHOST*)l)->bUse > ((STVWIMAGEANTILEECHHOST*)r)->bUse )
	{
		return 1;
	}
	else if ( ((STVWIMAGEANTILEECHHOST*)l)->bUse == ((STVWIMAGEANTILEECHHOST*)r)->bUse )
	{
		return _tcsicmp( ((STVWIMAGEANTILEECHHOST*)r)->szHost, ((STVWIMAGEANTILEECHHOST*)l)->szHost );
	}
	else
	{
		return -2;
	}
	//return strlen( ((STVWIMAGEANTILEECHHOST*)l)->szDomain ) - strlen( ((STVWIMAGEANTILEECHHOST*)r)->szDomain );
}




/**
 *	class of CVwImageAntiLeechConfigFile
 */
class CVwImageAntiLeechConfigFile :
	public CModuleInfo
{
public:
	CVwImageAntiLeechConfigFile();
	virtual ~CVwImageAntiLeechConfigFile();

public:
	BOOL LoadConfig( STVWIMAGEANTILEECHCONFIG * pstConfig );
	UINT LoadConfigUInt( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPCTSTR lpcszCfgFile = NULL );
	BOOL LoadConfigString( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPTSTR lpszString, DWORD dwSize, LPCTSTR lpcszCfgFile = NULL );
	BOOL SaveConfig( STVWIMAGEANTILEECHCONFIG * pstConfig );
	BOOL SaveConfig( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPCTSTR lpcszValue );

	BOOL GetHostDetailIniDomainNameByHost( LPCTSTR lpcszHost, LPTSTR lpszDomainName, DWORD dwSize );
	BOOL GetReplaceInfoByHostAndFiletype( LPCTSTR lpcszHost, LPCTSTR lpcszFiletype, STVWIMAGEANTILEECHREPLACE * pstReplace );
	BOOL SaveReplaceInfoByHostAndFiletype( LPCTSTR lpcszHost, LPCTSTR lpcszFiletype, STVWIMAGEANTILEECHREPLACE * pstReplace );
	BOOL GetItemInfoFilenameByHost( LPCTSTR lpcszHost, LPTSTR lpszFilename, DWORD dwSize );
	BOOL ReadAllExceptedSitesFromConfig( vector<STVWIMAGEANTILEECHSITE> * pvcSiteList );

private:
	BOOL InitModule();

protected:
	BOOL  m_bInitSucc;

	TCHAR m_szConfDir[ MAX_PATH ];
	TCHAR m_szLogsDir[ MAX_PATH ];


	TCHAR m_szFilterDllFile[ MAX_PATH ];
	TCHAR m_szDeLibDllFile[ MAX_PATH ];
	TCHAR m_szFilterVersion[ MAX_PATH ];


	TCHAR m_szCfgFile[ MAX_PATH ];
	
	STVWINFOMDICTFILE m_stDictFile;
	
	//
	//	配置文件
	//

};



#endif	//	__VWIMAGEANTILEECHCONFIGFILE_HEADER__


