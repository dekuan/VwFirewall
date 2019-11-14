// CVwInfoMonitorConfigFile.h: interface for the CVwInfoMonitorConfigFile class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWINFOMONITORCONFIGFILE_HEADER__
#define __VWINFOMONITORCONFIGFILE_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "VwConst.h"
#include "ModuleInfo.h"
#include <vector>
using namespace std;

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )


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
#define CVWINFOMONITORCONFIGFILE_FILE_FILTER			"VwInfoMonitor.dll"
#define CVWINFOMONITORCONFIGFILE_FILE_DELIB			"DeLib.dll"

#define CVWINFOMONITORCONFIGFILE_CONFFILE_CFG			"cfg.ini"
#define CVWINFOMONITORCONFIGFILE_CONFFILE_OUT_REP_WORDS		"out_rep_words.ini"
#define CVWINFOMONITORCONFIGFILE_CONFFILE_OUT_REP_WORDS_UTF8	"out_rep_words_utf8.ini"
#define CVWINFOMONITORCONFIGFILE_CONFFILE_OUT_BLOCK_WORDS	"out_block_words.ini"
#define CVWINFOMONITORCONFIGFILE_CONFFILE_IN_REP_WORDS		"in_rep_words.ini"
#define CVWINFOMONITORCONFIGFILE_CONFFILE_IN_BLOCK_WORDS	"in_block_words.ini"
#define CVWINFOMONITORCONFIGFILE_CONFFILE_IN_BLOCK_WORDS_ESCAPE	"in_block_words_escape.ini"
#define CVWINFOMONITORCONFIGFILE_CONFFILE_IN_BLOCK_WORDS_UTF8	"in_block_words_utf8.ini"
#define CVWINFOMONITORCONFIGFILE_CONFFILE_IN_ANTISQL_WORDS	"in_antisql_words.ini"
#define CVWINFOMONITORCONFIGFILE_CONFFILE_EXCEPTION_HOSTS	"exception_hosts.ini"
#define CVWINFOMONITORCONFIGFILE_CONFFILE_IN_BLOCK_ALERT	"in_block_alert.html"
#define CVWINFOMONITORCONFIGFILE_CONFFILE_IN_ANTISQL_ALERT	"in_antisql_alert.html"

//	词典文件
#define CVWINFOMONITORCONFIGFILE_DICT_OUT_REP_WORDS		"out_rep_words.dct"
#define CVWINFOMONITORCONFIGFILE_DICT_OUT_REP_WORDS_UTF8	"out_rep_words_utf8.dct"
#define CVWINFOMONITORCONFIGFILE_DICT_OUT_BLOCK_WORDS		"out_block_words.dct"
#define CVWINFOMONITORCONFIGFILE_DICT_IN_REP_WORDS		"in_rep_words.dct"
#define CVWINFOMONITORCONFIGFILE_DICT_IN_BLOCK_WORDS		"in_block_words.dct"
#define CVWINFOMONITORCONFIGFILE_DICT_IN_BLOCK_WORDS_ESCAPE	"in_block_words_escape.dct"
#define CVWINFOMONITORCONFIGFILE_DICT_IN_BLOCK_WORDS_UTF8	"in_block_words_utf8.dct"
#define CVWINFOMONITORCONFIGFILE_DICT_EXCEPTION_HOSTS		"exception_hosts.dct"

#define CVWINFOMONITORCONFIGFILE_MAX_WORDLOADCOUNT		5000	//	从一个文件中最大装载词汇数
#define CVWINFOMONITORCONFIGFILE_MAX_SQLMPLOADCOUNT		500	//	最大SQL匹配规则个数

#define CVWINFOMONITORCONFIGFILE_MIN_WORDLEN			4	//	词的最小长度
#define CVWINFOMONITORCONFIGFILE_MAX_WORDLEN			80	//	词的最大长度
#define CVWINFOMONITORCONFIGFILE_MAX_SQLMPLEN			100	//	SQL匹配规则的最大长度

#define CVWINFOMONITORCONFIGFILE_INI_DOMAIN_MAIN		"Main"
#define CVWINFOMONITORCONFIGFILE_INI_DOMAIN_LICENSE		"License"
#define CVWINFOMONITORCONFIGFILE_INI_DOMAIN_BLOCK		"Block"
#define CVWINFOMONITORCONFIGFILE_INI_DOMAIN_TICKINFO		"TickInfo"

#define CVWINFOMONITORCONFIGFILE_INI_KEY_START			"start"
#define CVWINFOMONITORCONFIGFILE_INI_KEY_START_WORDBLOCK	"start_wordblock"
#define CVWINFOMONITORCONFIGFILE_INI_KEY_START_ANTISQL		"start_antisql"
#define CVWINFOMONITORCONFIGFILE_INI_KEY_START_WORDFILTER	"start_wordfilter"

#define CVWINFOMONITORCONFIGFILE_INI_KEY_LOG			"log"

#define CVWINFOMONITORCONFIGFILE_INI_KEY_REGIP			"RegIp"
#define CVWINFOMONITORCONFIGFILE_INI_KEY_REGHOST		"RegHost"
#define CVWINFOMONITORCONFIGFILE_INI_KEY_REGKEY			"RegKey"
#define CVWINFOMONITORCONFIGFILE_INI_KEY_REGEXPIRETYPE		"RegExpireType"
#define CVWINFOMONITORCONFIGFILE_INI_KEY_REGCREATEDATE		"RegCreateDate"
#define CVWINFOMONITORCONFIGFILE_INI_KEY_REGEXPIREDATE		"RegExpireDate"

#define CVWINFOMONITORCONFIGFILE_INI_KEY_BLOCKPOST		"BlockPost"
#define CVWINFOMONITORCONFIGFILE_INI_KEY_BLOCKPOSTSIZE		"BlockPostSize"
#define CVWINFOMONITORCONFIGFILE_INI_KEY_BLOCKPOSTSIZECFG	"BlockPostSizeCfg"

#define CVWINFOMONITORCONFIGFILE_INI_KEY_TICKINFO_CTWT		"CTWT"		//	continue to work today
#define CVWINFOMONITORCONFIGFILE_INI_KEY_TICKINFO_TICK		"TICK"		//	tickcount


/**
 *	action
 *	注意: 为了节约内存，所有 action 的值加起来，总和不能超过 255
 */
#define VWINFOM_ACTION_NONE			0	//	无任何操作
#define VWINFOM_ACTION_OUT_REPLACE		1	//	流出: 替换过滤
#define VWINFOM_ACTION_OUT_BLOCK		2	//	流出: 阻止提示(暂时没用)
#define VWINFOM_ACTION_IN_REPLACE		4	//	流入: 替换过滤(暂时没用)
#define VWINFOM_ACTION_IN_BLOCK			8	//	流入: 阻止提示
#define VWINFOM_ACTION_IN_BLOCK_POST		16	//	超大文件上传阻止提示
#define VWINFOM_ACTION_IN_ANTISQL		32	//	内容中含有危险 SQL 语句
#define VWINFOM_ACTION_ALERT_OUT_REPLACE	64	//	流出过滤提示
#define VWINFOM_ACTION_ALL			( VWINFOM_ACTION_OUT_REPLACE | VWINFOM_ACTION_IN_BLOCK )



/**
 *	struct for word
 */
typedef struct tagVwInfoMWord
{
	tagVwInfoMWord()
	{
		memset( this, 0, sizeof(tagVwInfoMWord) );
	}
	BOOL bWildcard;
	UINT uLen;
	CHAR szWord[ MAX_PATH ];

}STVWINFOMWORD, *LPSTVWINFOMWORD;


/**
 *	struct for configuration
 */
typedef struct tagVwInfoMConfigEcpHost
{
	tagVwInfoMConfigEcpHost()
	{
		memset( this, 0, sizeof(tagVwInfoMConfigEcpHost) );
	}

	TCHAR szHost[ 128 ];

}STVWINFOMCONFIGECPHOST, *LPSTVWINFOMCONFIGECPHOST;

typedef struct tagVwInfoMConfigTicketInfo
{
	BOOL bContinueWork;
	DWORD dwStartTicket;
	DWORD dwDay;
	__int64 _n64LimitedDataTransfer;	//	个人版本限制大小
	__int64 _n64DataTransfer;		//	9223372036854775807 字节 = 8589934591 G字节

}STVWINFOMCONFIGTICKETINFO, *LPSTVWINFOMCONFIGTICKETINFO;

typedef struct tagVwInfoMConfig
{
	tagVwInfoMConfig()
	{
		memset( this, 0, sizeof(tagVwInfoMConfig) );
	}

	STVWINFOMCONFIGTICKETINFO stTickInfo;

	BOOL  bReged;						//	是否注册版本
	BOOL  bStart;						//	是否启动监控
	BOOL  bStartWordBlock;					//	开启词汇拦截
	BOOL  bStartAntiSQL;					//	开启 SQL 拦截
	BOOL  bStartWordFilter;					//	开启词汇过滤
	BOOL  bLog;						//	是否记录日志

	DWORD dwFilterFlags;					//	Filter 部分的 Flag 重要参数

	TCHAR szRegIp[ 64 ];					//	注册 IP 地址
	TCHAR szRegHost[ MAX_PATH ];				//	注册 HOST
	TCHAR szRegKey[ MAX_PATH ];				//	注册码
	DWORD dwRegExpireType;					//	过期类型
	TCHAR szRegCreateDate[ MAX_PATH ];			//	注册时间
	TCHAR szRegExpireDate[ MAX_PATH ];			//	过期时间

	BOOL  bBlockPost;					//	是否开启文件上传监控
	BOOL  bBlockPostSize;					//	是否监控文件上床大小
	DWORD dwBlockPostSizeCfg;				//	监控文件上传大小配置，单位是：兆字节

	LPTSTR lpszInBlockAlert_Html;				//	html in block alert 文件内存映射
	LPTSTR lpszInBlockAlert_ResHtml;			//	系统资源自带的 html in block alert 文件内存映射
	LPTSTR lpszInBlockAlert_UserHtml;			//	用户指定的 html in block alert 文件内存映射

	LPTSTR lpszInAntiSqlAlert_Html;				//	html in block sql alert 文件内存映射
	LPTSTR lpszInAntiSqlAlert_ResHtml;			//	系统资源自带的 html in block alert 文件内存映射
	LPTSTR lpszInAntiSqlAlert_UserHtml;			//	用户指定的 html in block alert 文件内存映射

	DWORD dwEcpHostWithPatternCount;			//	有通配符号的例外域名的个数
	STVWINFOMCONFIGECPHOST * pstEcpHostWithPattern;		//	指向一个例外域名数组

	DWORD dwAntiSqlWordsCount;				//	危险 SQL 语句（可能带有通配符）个数
	STVWINFOMWORD * pstAntiSqlWords;			//	危险 SQL 语句（可能带有通配符）数组

} STVWINFOMCONFIG, *LPSTVWINFOMCONFIG;

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




/**
 *	class of CVwInfoMonitorConfigFile
 */
class CVwInfoMonitorConfigFile :
	public CModuleInfo
{
public:
	CVwInfoMonitorConfigFile();
	virtual ~CVwInfoMonitorConfigFile();

public:
	BOOL LoadConfig( STVWINFOMCONFIG * pstConfig );
	UINT LoadConfigUInt( LPCTSTR lpcszDomain, LPCTSTR lpcszKey );
	BOOL LoadConfigString( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPTSTR lpszString, DWORD dwSize );
	BOOL SaveConfig( STVWINFOMCONFIG * pstConfig );
	BOOL SaveConfig( LPCTSTR lpcszDomain, LPCTSTR lpcszKey, LPCTSTR lpcszValue );

	BOOL LoadInBlockWords( vector<STVWINFOMWORD> & vcWordList, UINT * puWordMaxLen );
	BOOL SaveInBlockWords( vector<STVWINFOMWORD> & vcWordList, UINT uWordMaxLen );
	BOOL LoadInBlockWords_escape( vector<STVWINFOMWORD> & vcWordList, UINT * puWordMaxLen );
	BOOL SaveInBlockWords_escape( vector<STVWINFOMWORD> & vcWordList, UINT uWordMaxLen );
	BOOL LoadInBlockWords_utf8( vector<STVWINFOMWORD> & vcWordList, UINT * puWordMaxLen );
	BOOL SaveInBlockWords_utf8( vector<STVWINFOMWORD> & vcWordList, UINT uWordMaxLen );

	BOOL LoadInAntiSqlWords( vector<STVWINFOMWORD> & vcWordList, UINT * puWordMaxLen );
	BOOL SaveInAntiSqlWords( vector<STVWINFOMWORD> & vcWordList, UINT uWordMaxLen );


	BOOL LoadOutRepWords( vector<STVWINFOMWORD> & vcWordList, UINT * puWordMaxLen );
	BOOL SaveOutRepWords( vector<STVWINFOMWORD> & vcWordList, UINT uWordMaxLen );

	BOOL LoadOutRepWords_utf8( vector<STVWINFOMWORD> & vcWordList, UINT * puWordMaxLen );
	BOOL SaveOutRepWords_utf8( vector<STVWINFOMWORD> & vcWordList, UINT uWordMaxLen );


	BOOL LoadExceptionHosts( vector<STVWINFOMWORD> & vcWordList, UINT * puWordMaxLen );
	BOOL SaveExceptionHosts( vector<STVWINFOMWORD> & vcWordList, UINT uWordMaxLen );

	BOOL LoadWordsFromFile( LPCTSTR lpcszFilename, UINT uMaxWordLoadCount, vector<STVWINFOMWORD> & vcWordList, UINT * puWordMaxLen );
	BOOL LoadWildcardWordsFromFile( LPCTSTR lpcszFilename, DWORD dwMaxWordLen, BOOL bOnlyPatternWords, vector<STVWINFOMWORD> & vcWordList );
	BOOL SaveWordsFromFile( LPCTSTR lpcszFilename, vector<STVWINFOMWORD> & vcWordList, UINT uWordMaxLen );

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



#endif	//	__VWINFOMONITORCONFIGFILE_HEADER__


