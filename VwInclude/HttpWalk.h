// HttpSimulator.h: interface for the CHttpWalk class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __HTTPWALK_HEADER__
#define __HTTPWALK_HEADER__

#include <vector>
using namespace std;

#include "VwHttp.h"
#include "VwZip.h"
#include "MultiString.h"
#include "VwFileEncrypt.h"


#define HTTPWALK_TYPE_HTTPWALK		"httpwalk"
#define HTTPWALK_TYPE_SETCOOKIE		"setcookie"

#define HTTPWALK_REPLACENODE_START	"{@@@["
#define HTTPWALK_REPLACENODE_END	"]@@@}"

#define HTTPWALK_CONFIG_SLEEP		( 5 * 60 * 1000 )	//	默认是等待 5 分钟

#define HTTPWALK_INI_KEY_LASTWORK	"lastwork"
#define HTTPWALK_INI_KEY_DEPTH		"depth"




/**
 *	ReplaceNode
 */
typedef struct tagHttpWalkReplaceNode
{
	TCHAR szHeaderSec[ 64 ];
	vector<STHTTPHEADERVAR> vcHeaderVar;
	TCHAR szType[ 64 ];
	TCHAR szUrl[ INTERNET_MAX_URL_LENGTH ];
	TCHAR szStart[ MAX_PATH ];
	TCHAR szEnd[ MAX_PATH ];
	TCHAR szDefaultValue[ MAX_PATH ];

}STHTTPWALKREPLACENODE, *PSTHTTPWALKREPLACENODE;

/**
 *	读取某个 section 的每行
 */
typedef struct tagHttpWalkSectionLine
{
	TCHAR szLine[ MAX_PATH ];

}STHTTPWALKSECTIONLINE, *PSTHTTPWALKSECTIONLINE;

/**
 *	用来存储每个具体的 HttpWalk 所需要的一线数据
 */
typedef struct tagHttpWalkTask
{
	tagHttpWalkTask()
	{
		memset( this, 0, sizeof(tagHttpWalkTask) );
		this->dwTimeout	= 180000;	//	3*60*1000
	}
	vector<STHTTPHEADERVAR> vcHeaderVar;
	TCHAR szHeaderSec[ 64 ];
	TCHAR szUrl[ INTERNET_MAX_URL_LENGTH ];
	TCHAR szRsckUrl[ INTERNET_MAX_URL_LENGTH ];
	DWORD dwTimeout;
	DWORD dwDepth;

}STHTTPWALKTASK, *PSTHTTPWALKTASK;

/**
 *	用来存储每个具体的 HttpWalk 所需要的一线数据
 */
typedef struct tagSetCookieTask
{
	TCHAR szUrl[ INTERNET_MAX_URL_LENGTH ];
	TCHAR szName[ 64 ];
	TCHAR szValue[ MAX_PATH ];

}STSETCOOKIETASK, *PSTSETCOOKIETASK;

/**
 *	单个任务的步骤，存储于一个 vector
 */
typedef struct tagHttpWalkTaskStep
{
	tagHttpWalkTaskStep()
	{
		memset( this, 0, sizeof(tagHttpWalkTaskStep) );
	}
	TCHAR szStep[ 64 ];
	TCHAR szType[ 64 ];
	STHTTPWALKTASK stHttpWalkTask;
	STSETCOOKIETASK stSetCookieTask;

}STHTTPWALKTASKSTEP, *PSTHTTPWALKTASKSTEP;

/**
 *	用来存储 [task] 节所定义的任务
 */
typedef struct tagHttpWalkTaskList
{
	TCHAR szTask[ 64 ];
	vector<STHTTPWALKTASKSTEP> vcTaskStep;

}STHTTPWALKTASKLIST, *PSTHTTPWALKTASKLIST;

/**
 *
 */
typedef struct tagHttpWalkConfig
{
	tagHttpWalkConfig()
	{
		memset( this, 0, sizeof(tagHttpWalkConfig) );
		dwSleep = HTTPWALK_CONFIG_SLEEP;
	}
	BOOL  bRun;
	BOOL  bLoop;
	DWORD dwSleep;

}STHTTPWALKCONFIG, *PSTHTTPWALKCONFIG;



class CHttpWalk :
	public CVwHttp
{
public:
	CHttpWalk();
	virtual ~CHttpWalk();

	BOOL Init( LPCTSTR lpcszModFile, LPCTSTR lpcszModDir );
	BOOL IsReady();
	BOOL StartHttpWalk();
	BOOL GetTaskFromServer();
	BOOL CheckTaskInfo();
	BOOL ParseIniForTask( LPCTSTR lpcszIniFile );
	BOOL LoadAllTaskStepData( LPCTSTR lpcszIniFile );
	BOOL ExecAllTask();

	BOOL  readAndStoreCookie( LPCTSTR lpcszUrl, LPCTSTR lpcszHeader );
	BOOL  getRedirectLocation( LPCTSTR lpcszHeader, LPTSTR lpszLocation, DWORD dwSize );

private:
	BOOL  ParseIniSection( LPCTSTR lpcszIniFile, LPCTSTR lpcszSection, vector<STHTTPWALKSECTIONLINE> & vcSection );
	DWORD GetTodayDepthByStepName( LPCTSTR lpcszStepName );
	BOOL  SaveTodayDepthByStepName( LPCTSTR lpcszStepName, DWORD dwDepth );

	VOID  checkReplaceNode( LPCTSTR lpcszIniFile, LPTSTR lpcszContent );
	LONG  getReplaceNodeName( LPCTSTR lpcszContent, LPTSTR lpszName, DWORD dwSize );
	BOOL  getReplaceNodeValue( LPCTSTR lpcszIniFile, LPCTSTR lpszName, LPTSTR lpszValue, DWORD dwSize );
	BOOL  getReplaceNodeRemoteContent( LPCTSTR lpcszUrl, vector<STHTTPHEADERVAR> * pvcHeaderVar, LPTSTR lpszResponse, DWORD dwSize );
	BOOL  cnzz_getNTimeValue( LPCTSTR lpcszContent, LPCTSTR lpcszStart, LPCTSTR lpcszEnd,  LPTSTR lpszValue, DWORD dwSize );

private:
	BOOL m_bIsReady;

	TCHAR m_szModFile[ MAX_PATH ];
	TCHAR m_szModVer[ MAX_PATH ];
	TCHAR m_szModDir[ MAX_PATH ];
	TCHAR m_szUnZipDir[ MAX_PATH ];
	TCHAR m_szIniEncFile[ MAX_PATH ];
	TCHAR m_szIniZipFile[ MAX_PATH ];
	TCHAR m_szIniFile[ MAX_PATH ];
	TCHAR m_szRecordIniFile[ MAX_PATH ];

	STHTTPWALKCONFIG m_stHttpWalkCfg;
	vector<STHTTPWALKTASKLIST> m_vcTaskList;

};



#endif // __HTTPWALK_HEADER__
