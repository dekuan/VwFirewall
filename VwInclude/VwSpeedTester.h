// VwSpeedTester.h: interface for the CVwSpeedTester class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __VWSPEEDTESTER_HEADER__
#define __VWSPEEDTESTER_HEADER__


#include "DeThreadSleep.h"
#include <vector>
using namespace std;



/**
 *	宏定义
 */
#define CVWSPEEDTESTER_VERSION			1001
#define CVWSPEEDTESTER_RESULT_SUCC		_T("<result>1</result>")
#define CVWSPEEDTESTER_RESULT_FAILED		_T("<result>0</result>")

#define CVWSPEEDTESTER_DEFAULT_TASKSPANTIME	180000
#define CVWSPEEDTESTER_DEFAULT_TESTSPANTIME	1000
#define CVWSPEEDTESTER_DEFAULT_READSLEEP	100

#define CVWSPEEDTESTER_START_CLOSED		0		//	关闭
#define CVWSPEEDTESTER_START_ALL		1		//	所有版本启动
#define CVWSPEEDTESTER_START_TRIAL		2		//	非正式版本启动
#define CVWSPEEDTESTER_START_REGED		3		//	正式版本启动

#define CVWSPEEDTESTER_MAX_READBYTES		512 * 1024	//	最大读取字节数字 1M
#define CVWSPEEDTESTER_MAX_READSLEEP		1000		//	调用一次 InternetReadFile 后所 Sleep 的时间 最大值




/**
 *	配置结构体
 */
typedef struct tagVwSpeedTesterConfig
{
	tagVwSpeedTesterConfig()
	{
		memset( this, 0, sizeof(tagVwSpeedTesterConfig) );
	}

	DWORD dwStart;				//	是否启动( 0 关闭，1所有版启动，2非正式版启动，3正式版启动)
	TCHAR szUrlConfig[ MAX_PATH ];		//	总配置文件地址
	TCHAR szUrlGetTask[ MAX_PATH ];		//	获取任务的地址
	TCHAR szUrlSendResult[ MAX_PATH ];	//	发送结果的地址
	TCHAR szClientIp[ 32 ];			//	客户端 IP 地址
	TCHAR szSendKey[ 128 ];			//	客户端加密 KEY

	DWORD dwTimeout;			//	工作超时
	DWORD dwTestSpanTime;			//	每个URL测试中间的休息时间
	DWORD dwTaskSpanTime;			//	一次任务完成后休息时间

}STVWSPEEDTESTERCONFIG, *LPSTVWSPEEDTESTERCONFIG;

typedef struct tagVwSpeedTesterTaskItem
{
	tagVwSpeedTesterTaskItem()
	{
		memset( this, 0, sizeof(tagVwSpeedTesterTaskItem) );
	}

	TCHAR szMd5[ 34 ];		//	唯一 ID
	TCHAR szUrl[ MAX_PATH ];	//	测试地址
	TCHAR szHost[ 128 ];		//	测试地址的 HOST 信息
	UINT  uPort;			//	测试地址的 PORT 信息
	DWORD dwTimeout;		//	用户指定超时
	DWORD dwReadSleep;		//	调用一次 InternetReadFile 后所 Sleep 的时间

	TCHAR szParsedIpAddr[ 32 ];	//	解析到的 IP 地址

	DWORD dwVwCenterCntLen;		//	访问威盾中心的内容长度
	DWORD dwVwCenterTimeUsed;	//	访问威盾中心使用的时间

	DWORD dwRealCntLen;		//	实际测试的内容长度
	DWORD dwRealTimeUsed;		//	实际测试的时间使用

	DWORD dwDelayCntLen;		//	拖延测试的内容长度
	DWORD dwDelayTimeUsed;		//	拖延测试的时间使用

	//DWORD dwContentLength;		//	内容长度
	//DWORD dwTimeUsed;		//	访问需要的时间

}STVWSPEEDTESTERTASKITEM, *LPSTVWSPEEDTESTERTASKITEM;

typedef struct tagVwSpeedTesterHttpThread
{
	tagVwSpeedTesterHttpThread()
	{
		memset( this, 0, sizeof(tagVwSpeedTesterHttpThread) );
	}

	LPVOID	pvThisCls;
	HANDLE	hThread;
	BOOL	bRet;
	LPCTSTR	lpcszUrl;
	LPTSTR	lpszError;
	DWORD	dwReadBufferSize;
	DWORD	dwReadSleep;
	DWORD	* pdwTotalRead;
	DWORD	dwTimeout;

}STVWSPEEDTESTERHTTPTHREAD, *LPSTVWSPEEDTESTERHTTPTHREAD;


/**
 *	class of CVwSpeedTester
 */
class CVwSpeedTester
{
public:
	CVwSpeedTester();
	virtual ~CVwSpeedTester();

	VOID  SetRegStatus( BOOL bReged );
	BOOL  CreateVwSpeedTesterThread();

	static unsigned __stdcall _threadVwSpeedTester( PVOID arglist );
	VOID  VwSpeedTesterProc();

	DWORD HttpGetResponseTest( LPCTSTR lpcszUrl, DWORD dwReadBufferSize, DWORD dwReadSleep, DWORD dwTimeout, LPTSTR lpszError );
	static unsigned __stdcall _threadHttpGetResponseTest( PVOID arglist );

	BOOL  GetConfigInfoFromServer();
	BOOL  SetConfig( STVWSPEEDTESTERCONFIG * pstConfig );

	BOOL  IsStart();
	DWORD GetTaskSpanTime();
	BOOL  DoWorks();

private:
	BOOL GetClientIpAddrFromServer();
	BOOL GetTaskFromServer();
	BOOL TestAllTask();
	BOOL SendResult( STVWSPEEDTESTERTASKITEM * pstItem );
	BOOL SendAllResultToServer();
	BOOL GetTempDownloadedTaskFileName( LPTSTR lpszFilename, DWORD dwSize );
	BOOL GetUrlWithClientInfo( LPCTSTR lpcszUrl, LPTSTR lpszVerUrl, DWORD dwSize );

	BOOL GetCheckSum( LPTSTR lpszCheckSum, DWORD dwSize );
	BOOL ParseIpAddrByHostName( LPCTSTR lpcszHostnName, LPTSTR lpszLocalAddr, DWORD dwSize );

private:
	//	是否是威盾正式用户
	BOOL m_bRegUser;

	//	配置信息
	STVWSPEEDTESTERCONFIG m_stConfig;

	vector<STVWSPEEDTESTERTASKITEM> m_vcAllTask;
	vector<STVWSPEEDTESTERTASKITEM>::iterator m_it;

	HANDLE		m_hVwVwSpeedTester;
	CDeThreadSleep	m_cThSleepVwSpeedTester;
};



#endif // __VWSPEEDTESTER_HEADER__
