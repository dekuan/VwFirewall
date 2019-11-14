// MetaOpIISHeader.h: interface for the CAdsiOpIIS class.
//
//////////////////////////////////////////////////////////////////////
#ifndef __METAOPIISHEADER_HEADER__
#define __METAOPIISHEADER_HEADER__

//	for iis
#include <iadmw.h>
#include <iiscnfg.h>
#include <iwamreg.h>

//	for stl
#include <ctime>
#include <memory>
#include <string>
#include <bitset>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;



/**
 *	常量定义
 */
#define METAOPIIS_MD_TIMEOUT		20							//对METABASE操作超时限制
#define METAOPIIS_KEYNAME_LEN		16
#define METAOPIIS_BUFFER_SIZE		1024							

//	操作FLAG标志
//#define FLAG_NORMAL			0
//#define FLAG_NEW			1
//#define FLAG_DELETE			2
//#define FLAG_MODIFY			3


///////////////////////////////////////////////////////////////////////////
//	定义出错代码
#define ERROR_CREATE_DIR			1001		//	新建一个目录失败
#define ERROR_DELETE_DIR			1002		//	删除一个目录失败

#define ERROR_ADD_FTPUSER			1003		//	增加一个FTP用户失败
#define ERROR_DELETE_FTPUSER			1004		//	删除一个FTP用户失败
#define ERROR_ENABLE_FTPUSER			1005		//	开启一个FTP用户帐户失败
#define ERROR_DISABLE_FTPUSER			1006		//	禁用一个FTP用户帐户失败
#define ERROR_MODIFY_FTPUSER			1007		//	修改一个FTP帐户信息失败

#define ERROR_CREATE_KEY			1010		//	新建一个主键失败
#define ERROR_MODIFY_SBINDING			1011		//	修改绑定信息失败
#define ERROR_MODIFY_SCOMMENT			1012		//	修改站点说明失败
#define ERROR_MODIFY_SSTATE			1013		//	修改站点状态失败
#define ERROR_MODIFY_MAXCONNECTION		1014		//	修改最大连接数失败
#define ERROR_MODIFY_MAINDIR			1015		//	修改站点主目录失败
#define ERROR_MODIFY_MDIRACCESS			1016		//	修改主目录属性失败
#define ERROR_STATE_COMMAND			1017		//	操作站点失败
#define ERROR_MODIFY_DEFAULTDOC			1018		//	修改默认文档失败		
#define ERROR_CREATE_APP			1019		//	建立应用程序失败
#define ERROR_USE_LOGS				1020		//	禁用或启用日志失败
#define ERROR_NEW_LOGFILES			1021		//	修改新日志时间间隔失败

#define ERROR_NOEXIST_KEY			1101		//	给定的主键不存在
#define ERROR_INVALID_SB			1102		//	给定的绑定数据不合格


#define FILE_VERSION_COMMENT	"IIS backup tools V4.15 Copyright (c) 1999-2008 ViDun.Com"
#define METAOPIIS_DEFAULTDOC	"default.htm,default.html,default.asp,index.htm,index.html,index.asp"


//	AppPool key define
//#define MD_APPPOOL_STATE				9027	//	2001
#define MD_APPPOOL_PERIODICRESTARTTIME			9001	//	2002
#define MD_APPPOOL_PERIODICRESTARTREQUESTS		9002	//	2003
#define MD_APPPOOL_PERIODICRESTARTSCHEDULE		9020	//	2004
#define MD_APPPOOL_PERIODICRESTARTMEMORY		9024	//	2005
#define MD_APPPOOL_PERIODICRESTARTPRIVATEMEMORY		9038	//	2006
#define MD_APPPOOL_IDLETIMEOUT				9005	//	2007
#define MD_APPPOOL_APPPOOLQUEUELENGTH			9017	//	2008
#define MD_APPPOOL_CPULIMIT				9023	//	2009
#define MD_APPPOOL_CPURESETINTERVAL			2144	//	2010
#define MD_APPPOOL_CPUACTION				9022	//	2011
#define MD_APPPOOL_MAXPROCESSES				9003	//	2012
#define MD_APPPOOL_PINGINGENABLED			9004	//	2013	
#define MD_APPPOOL_PINGINTERVAL				9013	//	2014
#define MD_APPPOOL_RAPIDFAILPROTECTION			9006	//	2015
#define MD_APPPOOL_RAPIDFAILPROTECTIONMAXCRASHES	9030	//	2016
#define MD_APPPOOL_RAPIDFAILPROTECTIONINTERVAL		9029	//	2017
#define MD_APPPOOL_STARTUPTIMELIMIT			9011	//	2018
#define MD_APPPOOL_SHUTDOWNTIMELIMIT			9012	//	2018



/**
 *	结构体定义
 */
typedef struct tagMetaKeyName
{
	tagMetaKeyName()
	{
		memset( this, 0, sizeof(tagMetaKeyName) );
	}
	WCHAR wszKeyName[ METADATA_MAX_NAME_LEN ];

}STMETAKEYNAME, *PSTMETAKEYNAME;

typedef struct tagMetaVirtualDir
{
	//	虚拟目录属性
	tagMetaVirtualDir()
	{
		memset( this, 0, sizeof(tagMetaVirtualDir) );
	}
	
	DWORD dwFlag;						//	操作标志	
	
	TCHAR szSiteKeyName[ METADATA_MAX_NAME_LEN ];		//	所属站点键名
	TCHAR szKeyName_VirDir[ METADATA_MAX_NAME_LEN ];	//	虚拟目录键名
	TCHAR szPath_VirDir[ MAX_PATH];				//	虚拟目录路径，包括HTTP重定位路径
	DWORD dwAccessFlag;					//	虚拟目录访问属性
	
}STMETAVIRTUALDIR, *PSTMETAVIRTUALDIR;

typedef struct tagMetaSiteInfo
{
	//	站点信息
	tagMetaSiteInfo()
	{
		memset( this, 0, sizeof(tagMetaSiteInfo) );
		lnMaxBindwidth	= -1;
	}

	DWORD dwOpFlag;					//	该站点的标志,操作的方式

	TCHAR szKeyName[ METADATA_MAX_NAME_LEN ];	//	该站点在METABASE中的键名
	TCHAR szServerComment[ MAX_PATH ];		//	站点说明(名称)
	TCHAR szAppPoolId[ MAX_PATH ];			//	AppPoolId
	DWORD dwServerState;				//	站点服务状态
	DWORD dwAllowKeepAlive;				//	保持 Http 连接
	DWORD dwConnectionTimeout;			//	连接超时
	LONG  lnMaxConnection;				//	最大连接数，即最大在线人数
	LONG  lnMaxBindwidth;				//	启用最大带宽

	TCHAR szBindings[ METAOPIIS_BUFFER_SIZE ];	//	绑定信息

	LONG  lnLogType;				//	是否启用日志以及启用日志格式 0-不启用 1-启用并使用W3C格式
	LONG  lnLogFileLocaltimeRollover;		//	日志纪录：使用本地时间
	LONG  lnLogFilePeriod;				//	日志记录：计划
	LONG  lnLogFileTruncateSize;			//	日志纪录：文件大小
	LONG  lnLogExtFileFlags;			//	日志纪录：高级选项
	TCHAR szLogFileDirectory[ MAX_PATH ];		//	日志记录：目录
	TCHAR szLogPluginClsid[ MAX_PATH ];		//	日志纪录：PluginClsid

	TCHAR szServerMDir[ MAX_PATH ];			//	站点主目录
	DWORD dwMDirAccessFlag;				//	主目录：访问属性
	LONG  lnDirBrowseFlags;				//	主目录：目录浏览属性
	DWORD dwDontLog;				//	主目录：记录访问
	DWORD dwContentIndexed;				//	主目录：索引资源
	TCHAR szDefaultDoc[ METAOPIIS_BUFFER_SIZE ];	//	默认文档
	TCHAR szHttpExpires[ METADATA_MAX_NAME_LEN ];	//	Http头 >> 启用内容过期
	TCHAR szHttpErrors[ METAOPIIS_BUFFER_SIZE ];	//	自定义错误
	LONG  lnEnableDocFooter;			//	文档 >> 启用文档页脚
	TCHAR szDefaultDocFooter[ MAX_PATH ];		//	文档 >> 文档页脚文件路径

	INT   nStatus;					//	站点状态

}STMETASITEINFO, *PSTMETASITEINFO;

typedef struct tagMetaAppPoolInfo
{
	//	AppPool 结构
	tagMetaAppPoolInfo()
	{
		memset( this, 0, sizeof(tagMetaAppPoolInfo) );
	}

	TCHAR szKeyName[ METADATA_MAX_NAME_LEN ];	//	该站点在METABASE中的键名
	//TCHAR szPoolName[ MAX_PATH ];
	DWORD dwAppPoolState;			//	2001 

	DWORD dwPeriodicRestartTime;					//	2002 回收 >> 回收工作进程(分钟)
	DWORD dwPeriodicRestartRequests;				//	2003 回收 >> 回收工作进程(请求数目)
	TCHAR szPeriodicRestartSchedule[ METAOPIIS_BUFFER_SIZE ];	//	2004 回收 >> 在下列时间回收工作进程
	DWORD dwPeriodicRestartMemory;		//	2005 回收 >> 内存回收 >> 最大虚拟内存(兆)
	DWORD dwPeriodicRestartPrivateMemory;	//	2006 回收 >> 内存回收 >> 最大使用的内存(兆)

	DWORD dwIdleTimeout;			//	2007 性能 >> 空闲超时 >> 在空闲此段工作实践后关闭工作进程(分钟)
	DWORD dwAppPoolQueueLength;		//	2008 性能 >> 请求队列限制 >> 核心请求队列限制为(请求次数)
	DWORD dwCPULimit;			//	2009 性能 >> 启用 CPU 监视 >> 最大 CPU 使用率(百分比)
	DWORD dwCPUResetInterval;		//	2010 性能 >> 启用 CPU 监视 >> 刷新 CPU 使用率(分钟)
	DWORD dwCPUAction;			//	2011 性能 >> 启用 CPU 监视 >> CPU 使用率超过最大使用率时执行的操作
	DWORD dwMaxProcesses;			//	2012 性能 >> Web园 >> 最大工作进程数

	DWORD dwPingingEnabled;			//	2013 运行状况 >> 启用 Ping
	DWORD dwPingInterval;			//	2014 运行状况 >> 启用 Ping >> 每隔下列时间 Ping 工作线程(秒)
	DWORD dwRapidFailProtection;		//	2015 运行状况 >> 启用快速失败保护
	DWORD dwRapidFailProtectionMaxCrashes;	//	2016 运行状况 >> 启用快速失败保护 >> 失败数
	DWORD dwRapidFailProtectionInterval;	//	2017 运行状况 >> 启用快速失败保护 >> 时间段(分钟)
	
	DWORD dwStartupTimeLimit;		//	2018 运行状况 >> 启动时间限制 >> 工作进程必须在下列时间内开始(秒)
	DWORD dwShutdownTimeLimit;		//	2019 运行状况 >> 关闭时间限制 >> 工作进程必须在下列时间内关闭(秒)

}STMETAAPPPOOLINFO, *PSTMETAAPPPOOLINFO;


typedef struct tagMetaAllData
{
	//	整个 IIS 的数据
	tagMetaAllData()
	{
		memset( this, 0, sizeof(tagMetaAllData) );
	}

	vector<STMETASITEINFO> vcSite;
	vector<STMETAAPPPOOLINFO> vcAppPool;

}STMETAALLDATA, *PSTMETAALLDATA;


typedef struct tagMetaOperRecord
{
	//	操作记录结构
	WCHAR wszKeyPath[ MAX_PATH ];		//	操作路径
	union					//	操作数据
	{
		WCHAR wszKeyName[ METAOPIIS_KEYNAME_LEN ];
		METADATA_RECORD metaRecord;
	};

}STMETAOPERRECORD, *PSTMETAOPERRECORD;






#endif	//	__METAOPIISHEADER_HEADER__