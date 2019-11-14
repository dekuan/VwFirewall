// ProcCfg.h: interface for the CProcCfg class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __PROCCFG_HEADER__
#define __PROCCFG_HEADER__


#include "VwConstAntileechs.h"
#include "procdb.h"
#include "DeAdsiOpIIS.h"


//////////////////////////////////////////////////////////////////////////
//	const
#define CPROCCFG_IFH_TYPE_WEBSITE	0	//	普通域名
#define CPROCCFG_IFH_TYPE_SPIDER	1	//	搜索引擎的蜘蛛
#define CPROCCFG_IFH_TYPE_URL		2	//	URL
#define CPROCCFG_IFH_TYPE_CLIENTIP	3	//	ClientIP

#define CPROCCFG_IBH_TYPE_URL		0	//	URL
#define CPROCCFG_IBIP_TYPE_IP		0	//	IP




//////////////////////////////////////////////////////////////////////////

//	[main_table]
typedef struct tagMainTable
{
	tagMainTable()
	{
		memset( this, 0, sizeof(tagMainTable) );
	}
	LONG	m_id;			//	自增字段
	TCHAR	m_key[64];		//	注册码
	TCHAR	m_vw_ver[32];		//	软件版本
	TCHAR	m_vw_system[32];
	TCHAR	m_reg_ipaddr[16];
	TCHAR	m_reg_host[64];
	INT	m_is_reg_user;			//	是否是正式版本
	TCHAR	m_reg_prtype[ 32 ];		//	软件类型
	TCHAR	m_reg_prtypecs[ 64 ];		//	软件类型校验码

	TCHAR	m_reg_check_date[ 64 ];		//	最后检查时间
	INT	m_reg_expire_type;		//	过期类型: 0 无限，1有限制
	TCHAR	m_reg_create_date[ 64 ];	//	注册日期
	TCHAR	m_reg_expire_date[ 64 ];	//	到期时间
	INT	m_max_allow_dirs;		//	最大例外目录数量
	INT	m_max_friend_hosts;		//	最大友情站点数量

	LONG	m_use_proxy;			//	服务器被防火墙保护，需要从 HTTP_X_FORWARDED_FOR 节或者用户指定的节中获取远程用户的 IP 地址
	TCHAR	m_proxy_variable[ 64 ];		//	在 HTTP 头中，用户指定的获取远程 IP 地址的节的变量名字

	LONG	m_login_algorithm;		//	login algorithm
	TCHAR	m_login_password[ 64 ];		//	login password

}STMAINTABLE, *PSTMAINTABLE;

//	[iis_table]
typedef struct tagIisTable
{
	tagIisTable()
	{
		memset( this, 0, sizeof(tagIisTable) );
	}

	LONG	iis_id;
	TCHAR	iis_siteid[32];
	LONG	iis_use;				//	是否被添加到保护队列，只有被添加到保护队列下面的配置才有效。iis_use 辖管着 iis_start
	LONG	iis_start;				//	是否开启: 0为关闭，1为开启
	LONG	iis_start_time_switch;			//	定时开关
	LONG	iis_enable_protects;			//	是否启用:保护的配置
	LONG	iis_enable_exceptions;			//	是否启用:例外的配置
	LONG	iis_enable_limits;			//	是否启用:限制的配置
	LONG	iis_enable_security;			//	是否启用:安全的配置
	LONG	iis_enable_logs;			//	是否启用:日志的配置
	LONG	iis_enable_vh;				//	是否开启 VirtualHtml 优化
	LONG	iis_type;				//	防盗类型：0为普通防盗，1为超强防盗
	TCHAR	iis_pubvsid_key[MAX_PATH];		//	VSID 信息
	LONG	iis_pubvsid_keytime;			//	VSID 的有效时间
	LONG	iis_pubvsid_usebrowserin;		//	要求用户必须安装客户端插件
	LONG	iis_pubvsid_usecookie;			//	是否使用 COOKIE 来传递 VSID 信息
	LONG	iis_pubvsid_type;			//	计算方法：0普通，1随机变化
	LONG	iis_pubvsid_auto_expired;		//	VSID 使用一次后自动过期: 目前版本仅对 iis_pubvsid_type=1 时有效
	TCHAR	iis_pubvsid_cookiepage[MAX_PATH];	//	种植 COOKIE 的页面
	LONG	iis_pubvsid_cookiepage_len;		//	种植 COOKIE 的页面长度，读取的时候动态计算。
	TCHAR	iis_apppoolid[ 128 ];			//	AppPoolId

}STIISTABLE, *PSTIISTABLE;

//	[iis_blockext_table]
typedef struct tagIisBlockExtTable
{
	tagIisBlockExtTable()
	{
		memset( this, 0, sizeof(tagIisBlockExtTable) );
	}
	LONG	ibe_id;
	TCHAR	iis_siteid[32];			//	关联 iis_table 的字段
	LONG	ibe_use;			//	是否使用：0为不使用，1为使用
	TCHAR	ibe_name[ 32 ];			//	扩展名
	LONG	ibe_config;			//	是否开启：1-Deny all HTTP requests, 2-anti-leechs, 4-limit playonly, 8-limit thread, 16-limit speed
	LONG	ibe_lmt_thread;			//	限制线程
	LONG	ibe_lmt_speed;			//	限制速度
	LONG	ibe_action;			//	处理动作：1-Send warning messages表示发送警告信息, 2-表示使用文件替换
	TCHAR	ibe_lmt_player[8];		//	该扩展名限制使用哪个播放器播放。MP 表示 Window Media Player；RP 表示 Real Player。
	TCHAR	ibe_replacefile[ MAX_PATH ];	//	该扩展名的警告替换文件路径

} STIISBLOCKEXTTABLE, *PSTIISBLOCKEXTTABLE;

//	[iis_blockdirs_table]
typedef struct tagIisBlockDirsTable
{
	tagIisBlockDirsTable()
	{
		memset( this, 0, sizeof(tagIisBlockDirsTable) );
	}
	LONG	ibd_id;
	TCHAR	iis_siteid[32];			//	关联 iis_table 的字段
	LONG	ibd_use;			//	是否使用：0为不使用，1为使用
	TCHAR	ibd_name[ 64 ];			//	目录名称
	LONG	ibd_config;			//	是否开启：1-limit thread, 2-limit speed
	LONG	ibd_lmt_thread;			//	限制线程
	LONG	ibd_lmt_speed;			//	限制速度
	UINT	uBdNameLen;			//	目录名称长度
	BOOL	bHasWildcard;			//	是否含有通配符

}STIISBLOCKDIRSTABLE, *PSTIISBLOCKDIRSTABLE;

//	[iis_allowdirs_table]
typedef struct tagIisAllowDirsTable
{
	tagIisAllowDirsTable()
	{
		memset( this, 0, sizeof(tagIisAllowDirsTable) );
	}
	LONG	iad_id;
	TCHAR	iis_siteid[32];			//	关联 iis_table 的字段
	LONG	iad_use;			//	是否使用：0为不使用，1为使用
	LONG	iad_except_config;		//	例外的 ACTION 配置
	TCHAR	iad_name[ 64 ];			//	目录名称
	UINT	uAdNameLen;			//	目录名称长度
	BOOL	bHasWildcard;			//	是否含有通配符

}STIISALLOWDIRSTABLE, *PSTIISALLOWDIRSTABLE;

//	[iis_friendhosts_table]
typedef struct tagIisFriendHostsTable
{
	tagIisFriendHostsTable()
	{
		memset( this, 0, sizeof(tagIisFriendHostsTable) );
	}
	LONG	ifh_id;
	TCHAR	iis_siteid[32];			//	关联 iis_table 的字段
	LONG	ifh_use;			//	是否使用：0为不使用，1为使用
	LONG	ifh_type;			//	类型：0表示普通站点，1表示搜索引擎的蜘蛛
	LONG	ifh_except_config;		//	例外的 ACTION 配置
	TCHAR	ifh_name[ 260 ];		//	URL、域名或者搜索引擎的蜘蛛的索引
	DWORD	ifh_name_dwval;			//	ClientIP 需要将 name 转换成 DWORD 值存储

}STIISFRIENDHOSTSTABLE, *PSTIISFRIENDHOSTSTABLE;

//	[iis_blockhosts_table]
typedef struct tagIisBlockHostsTable
{
	tagIisBlockHostsTable()
	{
		memset( this, 0, sizeof(tagIisBlockHostsTable) );
	}
	LONG	ibh_id;
	TCHAR	iis_siteid[32];			//	关联 iis_table 的字段
	LONG	ibh_use;			//	是否使用：0为不使用，1为使用
	LONG	ibh_type;			//	类型：0表示普通站点，1表示搜索引擎的蜘蛛
	TCHAR	ibh_name[ 260 ];		//	URL
	
}STIISBLOCKHOSTSTABLE, *PSTIISBLOCKHOSTSTABLE;

//	[iis_blockip_table]
typedef struct tagIisBlockIPTable
{
	tagIisBlockIPTable()
	{
		memset( this, 0, sizeof(tagIisBlockIPTable) );
	}
	LONG	ibip_id;
	TCHAR	iis_siteid[ 32 ];		//	关联 iis_table 的字段
	LONG	ibip_use;			//	是否使用：0为不使用，1为使用
	LONG	ibip_type;			//	类型：0表示单个IP地址，1表示通配符IP地址
	TCHAR	ibip_name[ 18 ];		//	IP 地址
	DWORD	ibip_ip_value;			//	IP 地址的 32 位数字 VALUE

}STIISBLOCKIPTABLE, *PSTIISBLOCKIPTABLE;


//	[iis_security_table]
typedef struct tagIisSecurityTable
{
	tagIisSecurityTable()
	{
		memset( this, 0, sizeof(tagIisSecurityTable) );
	}
	LONG	isc_id;
	TCHAR	iis_siteid[32];				//	关联 iis_table 的字段
	LONG	isc_enable_iis_disguise;		//	是否开启 IIS 伪装功能
	LONG	isc_iis_disguise_cfg;			//	IIS 伪装的配置
	LONG	isc_enable_antatt;			//	是否开启防 CC 攻击功能
	LONG	isc_antatt_maxvisit;			//	防 CC 攻击：一分钟多少个请求就拒绝请求
	LONG	isc_antatt_sleep;			//	防 CC 攻击：遇到攻击停歇多久，单位分钟
	LONG	isc_antatt_disabled_proxy;		//	防 CC 攻击：proxy 禁止功能
							//	防 CC 攻击：白名单
							//	防 CC 攻击：黑名单
	LONG	isc_enable_url_filter;			//	是否开启 URL 过滤
	LONG	isc_enable_content_filter;		//	是否开始内容过滤
	LONG	isc_enable_lmt_urllen;			//	是否限制 URL 的长度
	LONG	isc_lmt_urllen_cfg;			//	限制 URL 长度的数值（单位是字符数字）
	LONG	isc_enable_lmt_urlchars;		//	是否限制 URL 中不允许有特殊字符出现
	TCHAR	isc_lmt_urlchars_cfg[ MAX_PATH ];	//	不允许出现的特殊字符配置
	LONG	isc_enable_lmt_contentlen;		//	是否开启限制内容长度
	LONG	isc_lmt_contentlen_sendbyserver;	//	限制服务器发送出的大小
	LONG	isc_lmt_contentlen_postbyclient;	//	限制从客户端 POST 给服务器的大小
	LONG	isc_enable_lmt_dayip;			//	是否开启日下载次数限制
	LONG	isc_lmt_dayip;				//	日 IP 下载次数限制
#ifdef RUN_ENV_SERVICE
	TCHAR	szUrlCharsCfg[32][MAX_SINGLEURLCHARSLEN];	//	isc_lmt_urlchars_cfg 的数组展开
	DWORD	dwUrlCharsCfgCount;				//	UrlCharsCfg 有效个数
#endif

}STIISSECURITYTABLE, *PSTIISSECURITYTABLE;

//	[iis_log_table]
typedef struct tagIisLogable
{
	tagIisLogable()
	{
		memset( this, 0, sizeof(tagIisLogable) );
	}
	LONG	ilg_id;
	TCHAR	iis_siteid[32];				//	关联 iis_table 的字段
	LONG	ilg_logtimeperiod;			//	日志格式：0 每小时，1每天，2每周，3每月
	LONG	ilg_logsize;				//	日志文件大小：单位 MB

}STIISLOGABLE, *PSTIISLOGABLE;

//	[iis_advanced_table]
typedef struct tagIisAdvancedTable
{
	tagIisAdvancedTable()
	{
		memset( this, 0, sizeof(tagIisAdvancedTable) );
	}
	LONG	iav_id;
	TCHAR	iis_siteid[32];				//	关联 iis_table 的字段
	LONG	iav_wm_show_vwtitle;			//	警告消息：是否显示VW标题
	LONG	iav_wm_model;				//	警告消息：模式，1-normal, 2-expert, 3-htmlfile
	LONG	iav_wm_http_status;			//	警告消息：http status
	LONG	iav_wm_delay_time;			//	警告消息：停留时间
	TCHAR	iav_wm_sitename[ MAX_PATH ];		//	警告消息：网站名称
	TCHAR	iav_wm_redirect_url[ MAX_PATH ];	//	警告消息：转向 URL
	TCHAR	iav_wm_sitelogo_url[ MAX_PATH ];	//	警告消息：站点LOGO
	TCHAR	iav_wm_topinfo[ 1024 ];			//	警告消息：topinfo
	TCHAR	iav_wm_info[ 1024 ];			//	警告消息：用户自定义的警告信息
	TCHAR	iav_wm_htmlfile[ MAX_PATH ];		//	警告消息：用户自定义的 HTML 文件

}STIISADVANCEDTABLE, *PSTIISADVANCEDTABLE;








class CProcCfg : public CProcDb
{
public:
	CProcCfg( BOOL bShowMessageBox = TRUE );
	virtual ~CProcCfg();

	//	更新数据库
	BOOL	UpdateDatabase();


	BOOL	GetMainTableInfoById( UINT m_id, PSTMAINTABLE pstMainTable );
	BOOL	SaveMainTableInfoById( UINT m_id, LPSTR lpszField, LPSTR lpcszValue );
	BOOL	SaveMainTableInfoById( UINT m_id, LPSTR lpszField, LONG lnValue );

	BOOL	GetAllHostInfo( vector<STHOSTINFO> & vcHostInfoAll );
	BOOL	SaveAllHostInfo( vector<STHOSTINFO> & vcHostInfoAll );

	BOOL	TryToCreateNewSiteCfg( LPCSTR lpcszSiteId );

	//	..
	BOOL	GetAllIisTableInfo( vector<STIISTABLE> & vcIis );
	BOOL	GetIisTableInfoBySiteId( LPCSTR lpcszSiteId, PSTIISTABLE pstSiteInfo );
	BOOL	SaveIisTableInfoBySiteId( LPCSTR lpcszSiteId, PSTIISTABLE pstSiteInfo );
	BOOL	UpdateIisTableUseById( LONG iis_id, LONG iis_use );
	BOOL	UpdateIisTableUseBySiteId( LPCSTR lpcszSiteId, LONG iis_use );
	BOOL	UpdateIisTableRootPathBySiteId( LPCSTR lpcszSiteId, LPCSTR lpcszRootPath );
	BOOL	UpdateIisTableAppPoolIdBySiteId( LPCSTR lpcszSiteId, LPCSTR lpcszAppPoolId );

	BOOL	CopyGeneralSettingTo( LPSTR lpszSiteId, LPSTR lpszSiteIdCopyTo );


	//	..
	BOOL	QueryValueForIisBlockExtTable( _RecordsetPtr rs, STIISBLOCKEXTTABLE * pstIisBlockExtTable );
	UINT	GetIisBlockExtTableCountBySiteId( LPSTR lpszSiteId );
	BOOL	DeleteAllIisBlockExtTableInfoBySiteId( LPSTR lpszSiteId );
	BOOL	GetIisBlockExtTableInfoById( LONG ibe_id, STIISBLOCKEXTTABLE * pstIisBlockExtTable );

	BOOL	GetAllIisBlockExtTableInfoBySiteIdEx( LPSTR lpszSiteId, vector<STIISBLOCKEXTTABLE> & vcBlockExts );
	BOOL	DeleteIisBlockExtTableInfoById( LONG ibe_id );
	BOOL	SaveIisBlockExtTableInfoById( LONG ibe_id, STIISBLOCKEXTTABLE * pstIisBlockExtTable );

	BOOL	GetAllIisBlockDirTableInfoBySiteId( LPSTR lpszSiteId, vector<STIISBLOCKDIRSTABLE> & vcBlockDirs );
	BOOL	SaveIisBlockDirTableInfoById( LONG ibd_id, STIISBLOCKDIRSTABLE * pstIisBlockDirTable );
	BOOL	DeleteIisBlockDirTableInfoById( LONG ibd_id );
	BOOL	DeleteAllIisBlockDirTableInfoBySiteId( LPSTR lpszSiteId );

	BOOL	CopyProtectedSettingTo( LPSTR lpszSiteId, LPSTR lpszSiteIdCopyTo );


	//
	//	for exceptions
	//
	BOOL	GetAllIisAllowDirTableInfoBySiteId( LPSTR lpszSiteId, vector<STIISALLOWDIRSTABLE> & vcAllowDirs );
	BOOL	SaveIisAllowDirTableInfoById( LONG iad_id, STIISALLOWDIRSTABLE * pstIisAllowDirTable );
	BOOL	DeleteIisAllowDirTableInfoById( LONG iad_id );
	BOOL	DeleteAllIisAllowDirTableInfoBySiteId( LPSTR lpszSiteId );

	//	
	BOOL	GetAllIisFriendHostsTableInfoBySiteId( LPSTR lpszSiteId, vector<STIISFRIENDHOSTSTABLE> & vcFriendHosts );
	BOOL	SaveIisFriendHostsTableInfoById( LONG ifh_id, STIISFRIENDHOSTSTABLE * pstIisFriendHostsTable );
	BOOL	DeleteIisFriendHostsTableInfoById( LONG ifh_id );
	BOOL	DeleteAllIisFriendHostsTableInfoBySiteId( LPSTR lpszSiteId );

	BOOL	CopyExceptionsSettingTo( LPSTR lpszSiteId, LPSTR lpszSiteIdCopyTo );


	//
	//	for limit
	//
	//	[iis_blockhosts_table]
	BOOL	GetAllIisBlockHostsTableInfoBySiteId( LPSTR lpszSiteId, vector<STIISBLOCKHOSTSTABLE> & vcBlockHosts );
	BOOL	SaveIisBlockHostsTableInfoById( LONG ibh_id, STIISBLOCKHOSTSTABLE * pstIisBlockHostsTable );
	BOOL	DeleteIisBlockHostsTableInfoById( LONG ibh_id );
	BOOL	DeleteAllIisBlockHostsTableInfoBySiteId( LPSTR lpszSiteId );

	//	[iis_blockip_table]
	BOOL	GetAllIisBlockIPTableInfoBySiteId( LPSTR lpszSiteId, vector<STIISBLOCKIPTABLE> & vcBlockIP );
	BOOL	SaveIisBlockIPTableInfoById( LONG ibip_id, STIISBLOCKIPTABLE * pstIisBlockIPTable );
	BOOL	DeleteIisBlockIPTableInfoById( LONG ibip_id );
	BOOL	DeleteAllIisBlockIPTableInfoBySiteId( LPSTR lpszSiteId );

	BOOL	CopyLimitSettingTo( LPSTR lpszSiteId, LPSTR lpszSiteIdCopyTo );



	//
	//	for security
	//
	BOOL	GetIisSecurityTableInfoBySiteId( LPSTR lpszSiteId, STIISSECURITYTABLE * pstIisSecurity );
	BOOL	SaveIisSecurityTableInfoById( LONG isc_id, STIISSECURITYTABLE * pstIisSecurity );
	BOOL	DeleteIisSecurityTableInfoById( LONG isc_id );
	BOOL	DeleteAllIisSecurityTableInfoBySiteId( LPSTR lpszSiteId );

	BOOL	CopySecuritySettingTo( LPSTR lpszSiteId, LPSTR lpszSiteIdCopyTo );


	//
	//	for log
	//
	BOOL	GetIisLogTableInfoBySiteId( LPSTR lpszSiteId, STIISLOGABLE * pstIisLog );
	BOOL	SaveIisLogTableInfoById( LONG ilg_id, STIISLOGABLE * pstIisLog );
	BOOL	DeleteIisLogTableInfoById( LONG ilg_id );
	BOOL	DeleteAllIisLogTableInfoBySiteId( LPSTR lpszSiteId );

	BOOL	CopyLogSettingTo( LPSTR lpszSiteId, LPSTR lpszSiteIdCopyTo );


	//
	//	for advanced
	//
	BOOL	GetIisAdvancedTableInfoBySiteId( LPSTR lpszSiteId, STIISADVANCEDTABLE * pstIisAdvanced );
	BOOL	SaveIisAdvancedTableInfoById( LONG iav_id, STIISADVANCEDTABLE * pstIisAdvanced );
	BOOL	DeleteIisAdvancedTableInfoById( LONG iav_id );
	BOOL	DeleteAllIisAdvancedTableInfoBySiteId( LPSTR lpszSiteId );

	BOOL	CopyAdvancedSettingTo( LPSTR lpszSiteId, LPSTR lpszSiteIdCopyTo );

};




#endif // __PROCCFG_HEADER__
