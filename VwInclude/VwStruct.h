#ifndef __VWSTRUCT_HEADER__
#define __VWSTRUCT_HEADER__


#include <set>
using namespace std;

#include "VwStructBase.h"
#include "VwStructCache.h"
#include "ErrorLog.h"
#include "ProcCfg.h"
#include "VwSpiderMatchMemV.h"
#include <windows.h>
#include <stdio.h>

//#include "VwSpiderMatch.h"

/**
 *	Http Header Info 信息存储
 */
typedef struct tagHttpHeaderInfo
{
	tagHttpHeaderInfo()
	{
		lnCfgIndex		= (-1);
		szAgent[ 0 ]		= 0;
		dwAgentLength		= 0;
		szContentType[ 0 ]	= 0;
		szContentLength[ 0 ]	= 0;
		szRange[ 0 ]		= 0;
		szRemoteAddr[ 0 ]	= 0;
		szHost[ 0 ]		= 0;
		szMethod[ 0 ]		= 0;
//		szServerPort[ 0 ]	= 0;
		szReferer[ 0 ]		= 0;
		dwRefererLength		= 0;
//		szRefererHost[ 0 ]	= 0;
		szCookie[ 0 ]		= 0;
		szFullUri[ 0 ]		= 0;
		dwFullUriLength		= 0;
		szUrlDir[ 0 ]		= 0;
		szUrlExt[ 0 ]		= 0;
		dwUrlExtLength		= 0;
		szUrlFile[ 0 ]		= 0;
	}
	LONG  lnCfgIndex;		//	STHOSTINFO 中的一个成员变量，用于 SET CACHE
	TCHAR szTemp[ 128 ];		//	用于临时获取字符串信息的缓存
	TCHAR szAgent[ MAX_PATH ];	//	UserAgent : 2009-01-13 长度从 128 到 MAX_PATH
	DWORD dwAgentLength;		//	UserAgent 长度
	TCHAR szContentType[ 32 ];	//	最少要 32 个字符，因为：Content-Type: application/octet-stream 是 24 个字节
	TCHAR szContentLength[ 32 ];
	DWORD dwContentLength;
	TCHAR szRange[ 64 ];		//	2009-5-14 长度从 16 升级到 64
	DWORD dwLocalAddrValue;
//	TCHAR szLocalAddr[ 16 ];
//	DWORD dwRemoteAddrLength;
	DWORD dwRemoteAddrValue;
	TCHAR szRemoteAddr[ 16 ];
	TCHAR szHost[ 128 ];
	TCHAR szMethod[ 16 ];			//	REQUEST_METHOD
//	TCHAR szServerPort[ 6 ];		//	SERVER_PORT
	ULONG uServerPort;			//	SERVER_PORT
	TCHAR szReferer[ 520 ];			//	2009-05-25 日从 MAX_PATH 升级到 520
	DWORD dwRefererLength;			//	szReferer 的长度
	//TCHAR szRefererHost[ 128 ];		//
	TCHAR szCookie[ 8192 ];			//	2009-3-15 V3.1.5 从 1024 升级到 2048
						//	2010-6-08 V3.2.2.1061 from 2048 -> 4096
						//	2010-7-11 V3.2.2.1062 from 4096 -> 8192
	TCHAR szVwRandMd5[ 128 ];		//	Vw-Vsid 节的内容
	TCHAR szFullUri[ 512 ];			//	URI : 2009-01-28 长度从 MAX_PATH 升级到 512
	TCHAR szFullUriLCase[ 512 ];		//	URI 小写的 : 2009-01-28 长度从 MAX_PATH 升级到 512
	DWORD dwFullUriLength;			//	the length of szFullUri
	TCHAR szUrlDir[ MAX_PATH ];		//	url dir : 2009-01-11 长度从 128 升级到 MAX_PATH
	DWORD dwUrlDirLength;			//	the length of url dir
	TCHAR szUrlExt[ 16 ];			//	file ext
	DWORD dwUrlExtLength;			//	the length of szUrlExt
	TCHAR szUrlFile[ 64 ];			//	url file : 2009-01-11 长度从 32 升级到 64
						//	### 这个长度不能随便修改，因为 SDK 文件里根据这个限制而截断
						//	### 如果要修改长度，必须同时修改 SDK 文件中的限制

}STHTTPHEADERINFO, *PSTHTTPHEADERINFO;









typedef struct tagLmtCalc
{
	tagLmtCalc()
	{
		lnLmtSleepTime		= 0;
	//	lnLmtSpeedTimeSpent	= 0;
	//	lnLmtSpeedLastSendTime	= 0;
	//	lnLmtSpeedSendCount	= 0;

		dwTickStart		= 0;
		dwUsedTime		= 0;
		dwTotalSendData		= 0;
	}

	LONG	lnLmtSleepTime;			//	Sleep 的时间，需要根据上面两个值计算出来
	
	//LONG	lnLmtSpeedTimeSpent;		//	当前网络能力：发送 8192*50 个字节所需要的时间，单位：毫秒
	//LONG	lnLmtSpeedLastSendTime;		//	上次发包的时间
	//LONG	lnLmtSpeedSendCount;		//	发包计数

	DWORD	dwTickStart;
	DWORD	dwUsedTime;
	DWORD	dwTotalSendData;

}STLMTCALC, *PSTLMTCALC;



enum
{
	NOTIFY_ACTION_PREPROC_HEADERS_START	= 0,
	NOTIFY_ACTION_PREPROC_HEADERS_END,

	NOTIFY_ACTION_URL_MAP_START,
	NOTIFY_ACTION_URL_MAP_END,

	NOTIFY_ACTION_SEND_RESPONSE_START,
	NOTIFY_ACTION_SEND_RESPONSE_END,

	NOTIFY_ACTION_SEND_RAW_DATA_START,
	NOTIFY_ACTION_SEND_RAW_DATA_END,

	NOTIFY_ACTION_LOG_START,
	NOTIFY_ACTION_LOG_END,

	NOTIFY_ACTION_END_OF_REQUEST_START,
	NOTIFY_ACTION_END_OF_REQUEST_END
};
#define NOTIFY_ACTION_COUNT	12




typedef struct tagSTVWNOTIFYACTIONTIME
{
	VOID UpdateTime( DWORD dwAction )
	{
		if ( IsValidAction( dwAction ) )
		{
			dwTick = GetTickCount();
			dwArrTick[ dwAction ] = dwTick;
			GetLocalTime( &stTime[ dwAction ] );
		}
	}
	BOOL IsValidAction( DWORD dwAction )
	{
		return ( dwAction >=0 && dwAction < NOTIFY_ACTION_COUNT );
	}	

	DWORD dwTick;
	DWORD dwArrTick[ NOTIFY_ACTION_COUNT ];
	SYSTEMTIME stTime[ NOTIFY_ACTION_COUNT ];

}STVWNOTIFYACTIONTIME, *LPSTVWNOTIFYACTIONTIME;

typedef struct tagMyHttpFilterContext
{
	//
	//	这个结构体不会有外部的 memset 之类的初始化动作
	//
	tagMyHttpFilterContext()
	{
		lnHaspPoolIdx	= -1;
		dwRequestFlag	= REQUEST_VW_OKPASS;
	}

	DWORD   lnStartTickCount;
//	LONG	lnTickCount;		//	Current TickCount
	LONG	lnPoolNodeFlag;		//	MemPool Index
	LONG	lnCyclePoolIdx;		//	循环池中的 IDX 值
//	LONG	lnCyclePoolFlag;	//	是否是因为循环池满而临时 new 出来的，如果是的话，用完要 free

	LONG	lnHaspPoolIdx;		//	线程限制池的索引 HaspPool Index
	LONG	lnCfgIdx;		//	该站点配置信息的 Config index
	DWORD	dwRequestFlag;		//	请求标记
	DWORD	dwNotificationType;	//	Notification 类型
	DWORD   dwAction;
	STVWNOTIFYACTIONTIME stActionTime;

	DWORD	dwProcessId;
	TCHAR	szAppPoolName[ 128 ];
	DWORD	dwAppPoolNameCrc32;

	BOOL	bExceptResource;	//	是否是例外的资源
	BOOL	bSentLastReport;

	//STCFGDATA stCfgData;
	STCFGDATA * pstCfgData;
	STLMTCALC stLmtCalc;
	STHTTPHEADERINFO stHhInfo;
	STLOGMGR * pstLogMgr;

}STMYHTTPFILTERCONTEXT, *PSTMYHTTPFILTERCONTEXT;






//
//	for socket packets
//
#define VWANITILEECHS_PACKET_TYPE_COREWORKRECORD		0x792055
#define VWANITILEECHS_PACKET_TYPE_ACTIVITYQUEUEREPORTER		0x792056
#define VWANITILEECHS_PACKET_TYPE_VWCOREMEMPOOLSTATUS		0x792057	//	VwCoreMemPoolFreeNode


typedef struct tagPacketHeader
{
	DWORD	dwHeader;		//	header flag
	DWORD	dwVersion;		//	version
	DWORD	dwDataLength;		//	size of data
	TCHAR	szAppName[ 64 ];	//	Application name

}STPACKETHEADER, *LPSTPACKETHEADER;

//
//	PacketCoreWorkRecordData
//
typedef struct tagPacketCoreWorkRecordData
{
	DWORD dwProcessId;
	TCHAR szAppPoolName[ 128 ];
	DWORD dwAppPoolNameCrc32;
	DWORD dwCoreTick;
	BOOL  bContinueWork;

}STPACKETCOREWORKRECORDDATA, *LPSTPACKETCOREWORKRECORDDATA;
typedef struct tagPacketCoreWorkRecord
{
	STPACKETHEADER stHeader;	//	header 必须放首位
	STPACKETCOREWORKRECORDDATA stData;

}STPACKETCOREWORKRECORD, *LPSTPACKETCOREWORKRECORD;


//
//	PacketActivityQueueReportData
//
typedef struct tagPacketActivityQueueReportData
{
	DWORD dwProcessId;
	TCHAR szAppPoolName[ 128 ];
	DWORD dwAppPoolNameCrc32;

	DWORD lnStartTickCount;
	STVWNOTIFYACTIONTIME stActionTime;
	DWORD dwAction;
	LONG  lnCyclePoolIdx;
	DWORD dwRequestFlag;
	DWORD dwNotificationType;
	BOOL  bExceptResource;
	TCHAR szAgent[ MAX_PATH ];
	DWORD dwAgentLength;
	TCHAR szContentType[ 32 ];
	DWORD dwContentLength;
	TCHAR szRange[ 64 ];
	TCHAR szRemoteAddr[ 16 ];
	TCHAR szHost[ 128 ];
	TCHAR szMethod[ 16 ];
	ULONG uServerPort;
	TCHAR szReferer[ 520 ];
	DWORD dwRefererLength;
	TCHAR szFullUri[ 512 ];
	DWORD dwFullUriLength;
	TCHAR szUrlDir[ MAX_PATH ];
	DWORD dwUrlDirLength;
	TCHAR szUrlExt[ 16 ];
	DWORD dwUrlExtLength;
	TCHAR szUrlFile[ 64 ];

}STPACKETACTIVITYQUEUEREPORTDATA, *LPSTPACKETACTIVITYQUEUEREPORTDATA;
typedef struct tagPacketActivityQueueReport
{
	STPACKETHEADER stHeader;	//	header 必须放首位
	STPACKETACTIVITYQUEUEREPORTDATA stData;

}STPACKETACTIVITYQUEUEREPORT, *LPSTPACKETACTIVITYQUEUEREPORT;
typedef struct tagPacketActivityQueueReportListItem
{
	BOOL bUse;
	STPACKETACTIVITYQUEUEREPORT stItem;

}STPACKETACTIVITYQUEUEREPORTLISTITEM, *LPSTPACKETACTIVITYQUEUEREPORTLISTITEM;


//
//	for VwCoreMemPoolFreeNode
//
typedef struct tagPacketVwCoreMemPoolStatusData
{
	DWORD	dwProcessId;		//	GetCurrentProcessId();
	LONG	lnCyclePoolIdx;		//	线程限制池的索引 HaspPool Index
	DWORD   dwAppPoolNameCrc32;
	DWORD   dwAction;

}STPACKETVWCOREMEMPOOLSTATUSDATA, *LPSTPACKETVWCOREMEMPOOLSTATUSDATA;
typedef struct tagPacketVwCoreMemPoolStatus
{
	STPACKETHEADER stHeader;	//	header 必须放首位
	DWORD dwValidItemCount;		//	有效项个数
	STPACKETVWCOREMEMPOOLSTATUSDATA stData[ 128 ];

}STPACKETVWCOREMEMPOOLSTATUS, *LPSTPACKETVWCOREMEMPOOLSTATUS;


//
//	for share memory
//
#define VWANITILEECHS_MAX_TOPURL		100

typedef struct tagVirtualWallSrvTopUrl
{
	DWORD dwCountPassed;	//	正常的计数
	DWORD dwCountDenied;	//	拒绝的计数
	DWORD dwCountBlocked;	//	被拦截的计数
	DWORD dwCrc32;
	DWORD dwLength;
	TCHAR szUrl[ 512 ];

}STVIRTUALWALLSRVTOPURL, *LPSTVIRTUALWALLSRVTOPURL;

typedef struct tagVirtualWallSrvShareMemory
{
	DWORD dwVersion;
	CRITICAL_SECTION oCrSecActivityListLock;
	CRITICAL_SECTION oCrSecActivitySetStatus;
	CRITICAL_SECTION oCrSecCoreWorkRecordLock;

	BOOL  bSendActivityReportToService;				//	Client 是否发送报告给服务
	DWORD dwVwCfgNewActivitySetStatusTick;				//	Tick of set activity by VwCfgNew.exe
	STPACKETACTIVITYQUEUEREPORTLISTITEM ArrActivityList[ 1000 ];	//	...
	DWORD dwActivityListCount;					//	...

	STPACKETCOREWORKRECORD stCoreWorkRecord;

	//	for top url
	STVIRTUALWALLSRVTOPURL ArrTopRequestPassed[ VWANITILEECHS_MAX_TOPURL ];		//	Top 请求
	DWORD dwTopRequestPassedCount;

	STVIRTUALWALLSRVTOPURL ArrTopRequestDenied[ VWANITILEECHS_MAX_TOPURL ];
	DWORD dwTopRequestDeniedCount;

	STVIRTUALWALLSRVTOPURL ArrTopRequestBlocked[ VWANITILEECHS_MAX_TOPURL ];
	DWORD dwTopRequestBlockedCount;

	STVIRTUALWALLSRVTOPURL ArrTopReferer[ VWANITILEECHS_MAX_TOPURL ];		//	Top 来源
	DWORD dwTopRefererCount;

}STVIRTUALWALLSRVSHAREMEMORY, *LPSTVIRTUALWALLSRVSHAREMEMORY;











typedef struct tagSiteConfig
{
	tagSiteConfig()
	{
		memset( this, 0, sizeof(tagSiteConfig) );
	}
	
	//	HTTP_FILTER_CONTEXT Pool
	//STMYHTTPFILTERCONTEXTPOOL stMyHfcPool;
	
	STIISTABLE stIis;

	//	拦截的 扩展名
	DWORD dwCountBlockExt;
	STIISBLOCKEXTTABLE stIisBlockExt[32];
	STCFGDATA stCfgDataBlockExt[32];
	
	//	拦截的 目录
	DWORD dwCountBlockDir;
	STIISBLOCKDIRSTABLE stIisBlockDir[32];
	STCFGDATA stCfgDataBlockDir[32];

	//	例外的 目录
	DWORD dwCountAllowDir;
	STIISALLOWDIRSTABLE stIisAllowDir[ MAX_ALLOWDIRSCOUNT ];
	//STCFGDATA stCfgDataAllowDir[32];

	//	例外的搜索引擎蜘蛛
	//STVWSPIDERMATCHTREE stIisFriendSpider;
	DWORD dwExceptedSpiderCount;
	STVWSPIDERMATCHMEMVDATA stIisFriendSpiderMemV[ MAX_FRIENDHOSTSCOUNT ];

	//	例外的 友情站点
	DWORD dwCountFriendHosts;
	STIISFRIENDHOSTSTABLE stIisFriendHosts[ MAX_FRIENDHOSTSCOUNT ];
	//STCFGDATA stCfgDataFriendHosts[32];

	//	限制站点
	DWORD dwCountBlockHosts;
	STIISBLOCKHOSTSTABLE stIisBlockHosts[ MAX_BLOCKHOSTSCOUNT ];

	//	限制 IP 地址
	DWORD dwCountBlockIp;
	STIISBLOCKIPTABLE stIisBlockIp[ MAX_BLOCKIPCOUNT ];

	STIISSECURITYTABLE stIisSecurity;
	STIISLOGABLE stIisLog;
	STIISADVANCEDTABLE stIisAdvanced;

}STSITECONFIG, *PSTSITECONFIG;


/**
 *	核心配置数据结构
 */
typedef struct tagConfigCore
{
	tagConfigCore()
	{
		memset( this, 0, sizeof(tagConfigCore) );
		bContinueWork	= TRUE;
	}

	BOOL  bIsInit;				//	标记数据结构体是否已经初始化
	BOOL  bContinueWork;			//	是否继续工作
	BOOL  bEnterpriseVer;			//	是否是企业版本
	BOOL  bRegUseHostSucc;			//	标记是用 Host 注册成功的
	//	..
	DWORD dwDay;
	DWORD dwStartTicket;			//	开始滴答
	DWORD dwProcTimes;			//	处理请求的次数
	DWORD dwBlockTimes;
	__int64 _n64LimitedDataTransfer;	//	个人版本限制大小
	__int64 _n64DataTransfer;		//	9223372036854775807 字节 = 8589934591 G字节
	
	TCHAR szLocalAddr[ 32 ];		//	当前计算机 IP 地址
	TCHAR szComputerName[ 64 ];
	TCHAR szModFile[ MAX_PATH ];		//	DLL 模块自己的全路径
	TCHAR szModPath[ MAX_PATH ];		//	DLL 模块自己所在文件夹
	TCHAR szDbPath[ MAX_PATH ];		//	配置文件的路径
	TCHAR szCfgMapFile[ MAX_PATH ];		//	配置数据内存映射文件名
	TCHAR szCoreWorkFile[ MAX_PATH ];	//	核心模块工作状况记录文件

	//	..
	STMAINTABLE stMain;			//	注册码等信息

} STCONFIGCORE, *PSTCONFIGCORE;


typedef struct tagConfigExProcessInfo
{
	DWORD dwProcessId;
	TCHAR szAppPoolName[ 128 ];
	DWORD dwAppPoolNameCrc32;

}STCONFIGEXPROCESSINFO, *LPSTCONFIGEXPROCESSINFO;


/**
 *	普通配置数据结构
 */
typedef struct tagConfigEx
{
	tagConfigEx()
	{
		memset( this, 0, sizeof(tagConfigEx) );
		memset( & stCfgDataInit, 0, sizeof(stCfgDataInit) );

		//	默认过滤器 flags
		dwFilterFlags = SF_NOTIFY_ORDER_DEFAULT | 
				SF_NOTIFY_PREPROC_HEADERS | 
				SF_NOTIFY_URL_MAP | 
				SF_NOTIFY_SEND_RESPONSE | 
				SF_NOTIFY_SEND_RAW_DATA | 
				SF_NOTIFY_END_OF_REQUEST;
	}
	~tagConfigEx()
	{
		//
		//if ( pstHostAll )
		//{
		//	delete [] pstHostAll;
		//	pstHostAll = NULL;
		//}
		//if ( pstAllSiteCfg )
		//{
		//	delete [] pstAllSiteCfg;
		//	pstAllSiteCfg = NULL;
		//}
	}

	//	..
	BOOL  bCfgLoading;			//	是否正在装载配置数据

	//	filter 配置
	DWORD dwFilterFlags;			//	过滤器的配置 flags
	TCHAR szUrlMatchHeader[ 8 ];		//	URL 匹配标记头，用来判断例外友情站点的 * 匹配计算，一般 6 个字符，例如 "VWXCY:"

	STHOSTINFO * pstHostAll;		//	[数组] 所有的 HOST 列表
	DWORD dwHostSize;			//	占用内存大小
	DWORD dwHostCount;			//	HOST 个数

	//	..
	STSITECONFIG * pstAllSiteCfg;		//	[数组] 装载的时候将内存分配在堆里
	DWORD dwSiteCfgSize;			//	占用内存大小
	DWORD dwSiteCfgCount;			//	站点数量

	//	常量初始化
	STCFGDATA stCfgDataInit;		//	常量配置信息，用来初始化 STMYHTTPFILTERCONTEXT->pstCfgData，该指针将指向这里
	STCONFIGEXPROCESSINFO stProcessInfo;

} STCONFIGEX, *PSTCONFIGEX;









/**
 *	内存映射文件头信息结构体
 */
typedef struct tagCfgMapHeader
{
	//	for mb_stCfgCore.stMain
	DWORD dwMainTableSize;			//	main_table 占用的内存大小 sizeof( mb_stCfgCore.stMain )

	//	for tagConfigEx.pstHostAll
	DWORD dwHostSize;			//	占用内存大小
	DWORD dwHostCount;			//	HOST 个数

	//	for tagConfigEx.pstAllSiteCfg
	DWORD dwSiteCfgSize;			//	占用内存大小
	DWORD dwSiteCfgCount;			//	站点数量
	
} STCFGMAPHEADER, * PSTCFGMAPHEADER;


/**
 *	STRING TAB 数据
 *	使用在 CProcBase
 */
typedef struct tagStringTab
{
	tagStringTab()
	{
		memset( this, 0, sizeof(tagStringTab) );
	}

	TCHAR szLangCode[ 16 ];

	TCHAR szResponseHeader[ MAX_URL ];
	DWORD dwResponseHeaderLength;

	TCHAR szResponseTopInfo[ MAX_URL ];
	DWORD dwResponseTopInfoLength;

	TCHAR szResponseTopInfoLogo[ MAX_URL ];
	DWORD dwResponseTopInfoLogoLength;

	TCHAR szResponseAddInfo[ MAX_URL ];
	DWORD dwResponseAddInfoLength;

	TCHAR szStringTrialVersion[ MAX_PATH ];

	TCHAR szWrnAntiLeechs[ 64 ];
	TCHAR szVrnAntiAttack[ 64 ];			//	Anti-Attack
	TCHAR szWrnAccessDenied[ 64 ];			//	Access Denied!
	TCHAR szWrnAccessDeniedBIp[ 64 ];		//	Access denied from your location.
	TCHAR szWrnAccessDeniedBHost[ 64 ];		//	Access denied from this website.

	TCHAR szWrnTooMuchConnection[ 64 ];		//	Too much connection!
	TCHAR szWrnDownloadForbidden[ 64 ];		//	The download is forbidden!
	TCHAR szWrnRequestUrlTooLong[ 64 ];		//	The request url is too long!
	TCHAR szWrnRequestUrlUnsafe[ 64 ];		//	The request url is unsafe!
	TCHAR szWrnPostContentTooLong[ 64 ];		//	The post content is too long!
	TCHAR szWrnContentSentByServerTooLong[ 64 ];	//	The content sent by server is too long!
	TCHAR szWrnTooMuchVisitToday[ 64 ];		//	Too much visiting today!

}STSTRINGTAB, *PSTSTRINGTAB;








#endif	// __VWSTRUCT_HEADER__