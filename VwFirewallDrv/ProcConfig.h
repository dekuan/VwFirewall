// ProcConfig.h: interface for the CProcConfig class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __PROCCONFIG_HEADER__
#define __PROCCONFIG_HEADER__

#include "stdafx.h"
#include "ProcessStruct.h"
#include "ProcProcess.h"


/**
 *	宏常量定义
 */
#define PROCCONFIG_MAX_EXT_LENGTH		6		//	例外扩展名的长度必须小于 6
#define PROCCONFIG_MAX_EXT_RANGE		0x100		//	例外扩展名控制字符的范围
#define PROCCONFIG_MAX_CONFIGFILESIZE		102400		//	配置文件的大小必须小于 100K
#define PROCCONFIG_MAX_PROTECTEDDIR_CACHE_SIZE	50000		//	最大缓存的项个数

#define PROCCONFIG_CFGKEY_START			"start="
#define PROCCONFIG_CFGKEY_START_PRDP		"startprdp="
#define PROCCONFIG_CFGKEY_START_PDOMAIN		"startpdomain="
#define PROCCONFIG_CFGKEY_START_FILESAFE	"startfilesafe="	//	文件安全
#define PROCCONFIG_CFGKEY_DOMAIN		"domain="
#define PROCCONFIG_CFGKEY_SYSHOST		"syshost="
#define PROCCONFIG_CFGKEY_RDP_CLIENTNAME	"rdpclientname="
#define PROCCONFIG_CFGKEY_PROTECTEDDIR		"protecteddir="
#define PROCCONFIG_CFGKEY_EXCEPTEDPROCESS	"exceptedprocess="
#define PROCCONFIG_CFGKEY_APP_INSDIR		"app_insdir="
#define PROCCONFIG_CFGKEY_APP_LOGDIR		"app_logdir="

#define PROCCONFIG_CFGKEY_REGIP			"reg_ip="
#define PROCCONFIG_CFGKEY_REGKEY		"reg_key="
#define PROCCONFIG_CFGKEY_REGPRTYPE		"reg_prtype="
#define PROCCONFIG_CFGKEY_REGPRTYPECS		"reg_prtypecs="

#define PROCCONFIG_CFGVALNAME_USE		"use:"
#define PROCCONFIG_CFGVALNAME_ITEM		"item:"
#define PROCCONFIG_CFGVALNAME_DM		"dm:"
#define PROCCONFIG_CFGVALNAME_WCD		"wcd:"		//	Wildcard

#define PROCCONFIG_CFGVALNAME_DIR		"dir:"		//	for [ProtectedDir]
#define PROCCONFIG_CFGVALNAME_TREE		"tree:"		//	for [ProtectedDir]
#define PROCCONFIG_CFGVALNAME_ALLEXT		"allext:"	//	for [ProtectedDir]
#define PROCCONFIG_CFGVALNAME_PROTEXT		"protext:"	//	for [ProtectedDir]
#define PROCCONFIG_CFGVALNAME_ECPEXT		"ecpext:"	//	for [ProtectedDir]


//	例外的子域名 名称
typedef struct tagExceptedSubDomain
{
	CHAR    szName[ 32 ];
	USHORT  uNameLength;

}STEXCEPTEDSUBDOMAIN, *LPSTEXCEPTEDSUBDOMAIN;

//	待处理的 域名白名单
typedef struct tagDomain
{
	SINGLE_LIST_ENTRY ListEntry;

	BOOLEAN bUse;						//	是否启用
	CHAR    szDomain[ MAX_PATH ];				//	需要处理的域名，例如：.abc.com
	USHORT  uDomainLength;					//	需要处理的域名的长度
	BOOLEAN bWildcard;					//	是否是泛域名

	STEXCEPTEDSUBDOMAIN stArrExceptedSubDomain[ 32 ];	//	例外子域名
	//CHAR    szExceptedSubDomain[ 32 ][ 32 ];		//	例外子域名
	USHORT  uExceptedSubDomainCount;			//	例外子域名的个数

}STDOMAIN, *LPSTDOMAIN;

typedef struct tagSysHost
{
	SINGLE_LIST_ENTRY ListEntry;

	CHAR    szHost[ 128 ];
	USHORT  uHostLength;

}STSYSHOST, *LPSTSYSHOST;

typedef struct tagRdpClientName
{
	SINGLE_LIST_ENTRY ListEntry;

	BOOLEAN bUse;
	WCHAR   wszClientName[ 16 ];
	USHORT  uLength;

}STRDPCLIENTNAME, *LPSTRDPCLIENTNAME;

//
//	被保护目录配置(for 链表)
//
typedef struct tagProtectedDir
{
	SINGLE_LIST_ENTRY ListEntry;

	BOOL   bUse;			//	是否启用
//	UCHAR  uszDir[ MAX_PATH ];	//	被保护目录a
	WCHAR  wszDir[ MAX_PATH ];	//	被保护目录w
	USHORT uDirLength;		//	被保护目录的长度
	ULONG  ulDirCrc32;		//	目录的 Crc32 值

	BOOL   bProtSubDir;		//	是否保护其子目录

	BOOL   bProtAllExt;		//	是否要保护所有的扩展名，TRUE 是，FALSE 不是，仅仅保护 uszProtectedExt 里指定的

	UCHAR  uszProtectedExt[ 128 ][ PROCCONFIG_MAX_EXT_LENGTH ];	//	被保护的扩展名A
	WCHAR  wszProtectedExt[ 128 ][ PROCCONFIG_MAX_EXT_LENGTH ];	//	被保护的扩展名W
	UINT   uProtectedExtCount;					//	被保护的扩展名的个数

	UCHAR  uszExceptedExt[ 128 ][ PROCCONFIG_MAX_EXT_LENGTH ];	//	例外扩展名A
	WCHAR  wszExceptedExt[ 128 ][ PROCCONFIG_MAX_EXT_LENGTH ];	//	例外扩展名W
	UINT   uExceptedExtCount;					//	例外扩展名的个数

}STPROTECTEDDIR, *LPSTPROTECTEDDIR;

struct tagProtectedDirCache
{
	ULONG   ulCrc32;
	BOOLEAN bProtectedDir;

	struct tagProtectedDirCache * pstLeft;
	struct tagProtectedDirCache * pstRight;

};
typedef struct tagProtectedDirCache STPROTECTEDDIRCACHE, *LPSTPROTECTEDDIRCACHE;


//
//	例外的进程
//
typedef struct tagExceptedProcess
{
	SINGLE_LIST_ENTRY ListEntry;

	BOOL   bUse;				//	是否启用
	UCHAR  uszDosPath[ MAX_PATH ];		//	例外进程的路径
	USHORT uDosPathLength;			//	被保护目录的长度
	ULONG  ulDosPathCrc32;			//	目录的 Crc32 值

	UCHAR  uszDevicePath[ MAX_PATH ];
	USHORT uDevicePathLengthInBytesU;	//	length of uszDevicePath in bytes
	ULONG  ulDevicePathCrc32U;		//	Crc32 value of uszDevicePath

	WCHAR  wszDevicePath[ MAX_PATH ];	//	设备名称，例如“\Device\HarddiskVolume1\Windows\explorer.exe”
	USHORT uDevicePathLengthInBytesW;	//	length of wszDevicePath in bytes

}STEXCEPTEDPROCESS, *LPSTEXCEPTEDPROCESS;


typedef struct tagConfigData
{
	FAST_MUTEX muxLock;				//	互斥锁
	
	SINGLE_LIST_ENTRY linkDomainSingle;		//	需要处理的域名(single)链表
	UINT   uDomainSingleCount;			//	需要处理的域名(single)个数

	SINGLE_LIST_ENTRY linkSysHostSingle;		//	IIS Host(single)链表
	UINT   uSysHostSingleCount;			//	IIS Host(single)个数

	SINGLE_LIST_ENTRY linkRdpClientNameSingle;	//	RDP ClientName(single)链表
	UINT   uRdpClientNameSingleCount;		//	RDP ClientName(single)个数

	WCHAR  wszProtedDisk[ 0xFF ];			//	记录被保护的磁盘符号，如 INDEX 'C' 位置 = 1 表示有保护，= 0 表示无保护
	UINT   uProtedDirTotalCount;			//	被保护目录(tree+single)个数
	SINGLE_LIST_ENTRY linkProtedDirTree;		//	被保护目录(tree)链表
	UINT   uProtedDirTreeCount;			//	被保护目录(tree)个数

	//	被保护目录缓存/个数
	STPROTECTEDDIRCACHE * pstProtectedDirCacheHeader;
	UINT   uProtectedDirCacheCount;

	SINGLE_LIST_ENTRY linkProtedDirSingle;		//	被保护目录(single)链表
	UINT   uProtedDirSingleCount;			//	被保护目录(single)个数

	SINGLE_LIST_ENTRY linkExceptedProcessSingle;	//	例外的进程(single)链表
	UINT   uExceptedProcessSingleCount;		//	例外的进程(single)个数

}STCONFIGDATA, *LPSTCONFIGDATA;

typedef struct tagRegDate
{
	INT nRegYear;
	INT nRegMonth;
	INT nRegDay;
	
	INT nExpireYear;			//	到期的年
	INT nExpireMonth;			//	到期的月
	INT nExpireDay;				//	到期的日
	
}STREGDATE, *LPSTREGDATE;

typedef struct tagConfig
{
	BOOL   bInited;				//	是否被初始化过
	BOOL   bStart;				//	是否开始干活
	BOOL   bStartProtectRdp;		//	开启保护 RDP
	BOOL   bStartProtectDomain;		//	开启域名白名单
	BOOL   bStartFileSafe;			//	开启文件安全
	BOOL   bLoading;			//	是否正在装载配置信息
	
	//
	BOOL   bReged;				//	是否是注册版本
	BOOL   bValidPrType;			//	是否是有效的 PrType
	CHAR   szRegIp[ 32 ];			//	注册 IP 地址
	CHAR   szRegKey[ 64 ];			//	注册码
	CHAR   szRegPrType[ 32 ];		//	产品类型
	CHAR   szRegPrTypeChecksum[ 64 ];	//	产品类型的校验值

	//	AppInfo
	WCHAR  wszAppDir[ MAX_PATH ];		//	应用程序所在目录，一般是 "C:\VirtualWall\VwFirewall"
	UINT   uAppDirLength;
	WCHAR  wszLogDir[ MAX_PATH ];		//	日志文件所在目录，一般是 "C:\VirtualWall\VwFirewall\Logs"
	UINT   uLogDirLength;

	//
	UINT   uCurrIndex;			//	当前使用的配置数据索引 = 0/1
	STCONFIGDATA stCfgData[ 2 ];		//	由于为了能动态装载，所以有两个配置，一个 online，另一个 offline

	//
	DWORD  dwRtspNowDay;			//	拦截今日日期记录，用以每日统计
	LONGLONG llProtectFileBlockCount;	//	今日拦截统计：文件锁
	LONGLONG llProtectDomainBlockCount;	//	今日拦截统计：域名白名单
	LONGLONG llProtectRdpBlockCount;	//	今日拦截统计：远程桌面

}STCONFIG, *LPSTCONFIG;


/**
 *	extern 全局变量
 */
extern STCONFIG g_stCfg;
extern NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolDomain;			//	需要处理的域名 池
extern NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolSysHost;		//	需要处理的域名 池
extern NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolRdpClientName;		//	RDP 客户端计算机名 池
extern NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolProtectedDir;		//	被保护目录池
extern NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolProtectedDirCache;	//	被保护目录Cache池
extern NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolExceptedProcess;	//	例外的进程
extern NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolFilename;		//	文件名内存池
extern NPAGED_LOOKASIDE_LIST g_npgProcConfigPoolFilenameW;		//	文件名内存池W

extern CHAR g_szDecryptedString_GET[ 16 ];
extern CHAR g_szDecryptedString_POST[ 16 ];
extern CHAR g_szDecryptedString_Host1[ 16 ];
extern CHAR g_szDecryptedString_Host2[ 16 ];
extern CHAR g_szDecryptedString_UserAgent1[ 16 ];
extern CHAR g_szDecryptedString_UserAgent2[ 16 ];
extern CHAR g_szDecryptedString_VwPanDmNewCfg[ 16 ];
extern CHAR g_szDecryptedString_reg_ip_[ 16 ];
extern CHAR g_szDecryptedString_reg_key_[ 16 ];
extern CHAR g_szDecryptedString_reg_prtype_[ 16 ];
extern CHAR g_szDecryptedString_reg_prtypecs_[ 16 ];

//	for prtypefmt
extern CHAR g_szDecryptedString_PrTypeChecksumFmt[ 32 ];

//	for prtype
extern CHAR g_szDecryptedString_PrType_comadv[ 32 ];
extern CHAR g_szDecryptedString_PrType_comnormal[ 32 ];
extern CHAR g_szDecryptedString_PrType_pslunlmt[ 32 ];
extern CHAR g_szDecryptedString_PrType_psladv[ 32 ];
extern CHAR g_szDecryptedString_PrType_pslnormal[ 32 ];


//
//	public
//
BOOLEAN procconfig_init_config();
VOID	procconfig_unload_config();
BOOLEAN procconfig_is_valid_key( LPCWSTR lpcwszKeyFile );
BOOLEAN procconfig_is_valid_prtype_checksum( LPCTSTR lpcszPrTypeChecksum );
BOOLEAN procconfig_load_config( LPCWSTR lpcwszConfigFile );

BOOLEAN procconfig_start();
BOOLEAN procconfig_stop();
BOOLEAN procconfig_is_start();
BOOLEAN procconfig_is_loading();
BOOLEAN procconfig_lock( INT nCurrIndex );
BOOLEAN procconfig_unlock( INT nCurrIndex );

VOID    procconfig_add_block_count( LONGLONG * pllCounter );
BOOLEAN procconfig_is_matched_domain( LPCSTR lpcszDomain, UINT uDomainLength );
BOOLEAN procconfig_clean_all_offline_config();

//
//	private
//
VOID    procconfig_decrypt_string();
UINT	procconfig_stconfig_get_online_index();
UINT	procconfig_stconfig_get_offline_index();
VOID	procconfig_stconfig_switch_index();

UINT    procconfig_get_max_parsed_count();
BOOLEAN procconfig_load_allconfig_from_file( LPCWSTR lpcwszConfigFile );
BOOLEAN procconfig_addnew_domain( LPSTDOMAIN pstDomain );
BOOLEAN procconfig_addnew_rdp_clientname( LPSTRDPCLIENTNAME pstRdpClientName );
LPSTDOMAIN procconfig_get_matched_domain( LPCSTR lpcszOrgDomain, UINT uOrgDomainLength );
BOOLEAN procconfig_is_exist_rdp_clientname( LPCWSTR lpcwszClientName, UINT uLength );

UINT	procconfig_split_string_to_array( IN LPCSTR lpcszStringList, IN UCHAR uChrSpliter, OUT BYTE pbtArray[][PROCCONFIG_MAX_EXT_LENGTH], IN DWORD dwArrayCount );



BOOLEAN	procconfig_addnew_protected_dir( LPSTPROTECTEDDIR pstProtectedDir );

BOOLEAN	procconfig_is_protected_dir( IN WCHAR * lpwszFilePath, IN UINT uFilePathLength, IN BOOLEAN bCheckAllParentDirs, IN HANDLE hFileHandle );

BOOLEAN	procconfig_is_protected_disk( WCHAR wszDisk );
BOOLEAN	procconfig_is_protected_ext( STPROTECTEDDIR * pstProtectedDir, LPCWSTR lpcwszFilePath, UINT uFilePathLength );
BOOLEAN	procconfig_is_excepted_ext( STPROTECTEDDIR * pstProtectedDir, LPCWSTR lpcwszFilePath, UINT uFilePathLength );


BOOLEAN procconfig_addnew_protected_dir_cache_crc32( STCONFIGDATA * pstCurrCfgData, ULONG ulCrc32, BOOLEAN bProtectedDir );
BOOLEAN procconfig_is_protected_dir_by_cache_crc32( STCONFIGDATA * pstCurrCfgData, ULONG ulCrc32 );
BOOLEAN procconfig_clean_protected_dir_cache_crc32( STCONFIGDATA * pstCurrCfgData, STPROTECTEDDIRCACHE * pstProtectedDirCache );



BOOLEAN procconfig_addnew_excepted_process( LPSTEXCEPTEDPROCESS pstExceptedProcess );
BOOLEAN procconfig_is_excepted_process();
BOOLEAN procconfig_is_excepted_process_by_devicepath( PWSTR pwszDevicePath, USHORT uLengthInBytes );




#endif // __PROCCONFIG_HEADER__
