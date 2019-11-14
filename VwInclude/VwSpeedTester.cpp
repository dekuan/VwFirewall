// VwSpeedTester.cpp: implementation of the CVwSpeedTester class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VwSpeedTester.h"

#include "DeHttp.h"

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )

#ifndef FD_SET
#include "Winsock2.h"
#endif
#pragma comment( lib, "Ws2_32.lib" )


/**
 *	加密字符串
 */

//	"http://rd.xingworld.net/?2053"
static TCHAR g_szCVwSpeedTester_UrlConfigInfo[] =
{
	-105, -117, -117, -113, -59, -48, -48, -115, -101, -47, -121, -106, -111, -104, -120, -112, -115, -109, -101, -47, -111,
	-102, -117, -48, -64, -51, -49, -54, -52, 0
};

//	"VWSpeed-200903"
static TCHAR g_szCVwSpeedTester_SendKey[] =
{
	-87, -88, -84, -113, -102, -102, -101, -46, -51, -49, -49, -58, -49, -52, 0
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVwSpeedTester::CVwSpeedTester()
{
	memset( &m_stConfig, 0, sizeof(m_stConfig) );

	m_bRegUser		= FALSE;
	m_hVwVwSpeedTester	= NULL;

	//	解密客户端加密 KEY
	memcpy( m_stConfig.szSendKey, g_szCVwSpeedTester_SendKey, min( sizeof(g_szCVwSpeedTester_SendKey), sizeof(m_stConfig.szSendKey) ) );
	delib_xorenc( m_stConfig.szSendKey );

	//	解密配置文件 URL
	memcpy( m_stConfig.szUrlConfig, g_szCVwSpeedTester_UrlConfigInfo, min( sizeof(g_szCVwSpeedTester_UrlConfigInfo), sizeof(m_stConfig.szUrlConfig) ) );
	delib_xorenc( m_stConfig.szUrlConfig );
}

CVwSpeedTester::~CVwSpeedTester()
{
	if ( m_hVwVwSpeedTester )
	{
		m_cThSleepVwSpeedTester.EndSleep();
		m_cThSleepVwSpeedTester.EndThread( &m_hVwVwSpeedTester );
	}
}

/**
 *	@ public
 *	设置威盾软件的注册状态
 */
VOID CVwSpeedTester::SetRegStatus( BOOL bReged )
{
	m_bRegUser = bReged;
}

/**
 *	@ public
 *	创建测速模块工作线程
 */
BOOL CVwSpeedTester::CreateVwSpeedTesterThread()
{
	//
	//	开启速度检测模块
	//
	m_hVwVwSpeedTester = (HANDLE)_beginthreadex
	(
		NULL,
		0,
		&_threadVwSpeedTester,
		(void*)this,
		0,
		NULL
	);

	return TRUE;
}

//
//	速度测试模块
//
unsigned __stdcall CVwSpeedTester::_threadVwSpeedTester( PVOID arglist )
{
	if ( NULL == arglist )
	{
		return 1;
	}
	
	__try
	{
		CVwSpeedTester * pThis = (CVwSpeedTester*)arglist;
		if ( pThis )
		{
			pThis->VwSpeedTesterProc();
		}
		
		_endthreadex( 0 );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
	
	return 0;
}
VOID CVwSpeedTester::VwSpeedTesterProc()
{
	__try
	{
		while( m_cThSleepVwSpeedTester.IsContinue() )
		{
			if ( GetConfigInfoFromServer() )
			{
				if ( IsStart() )
				{
					//	干活
					DoWorks();
				}

				//	休息
				Sleep( GetTaskSpanTime() );
			}

			//	休息 1 分钟
			Sleep( 60*1000 );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}


/**
 *	@ public
 *	从服务器获取初始配置参数
 *	使用前必须调用的方法
 */
BOOL CVwSpeedTester::GetConfigInfoFromServer()
{

	BOOL bRet	= FALSE;
	CDeHttp cDeHttp;
	TCHAR szVerUrl[ 1024 ];
	TCHAR szResponse[ MAX_PATH ];
	TCHAR szError[ MAX_PATH ];
	TCHAR szTemp[ MAX_PATH ];
	

	__try
	{
		memset( szResponse, 0, sizeof(szResponse) );
		memset( szError, 0, sizeof(szError) );

		GetUrlWithClientInfo( m_stConfig.szUrlConfig, szVerUrl, sizeof(szVerUrl) );
		if ( cDeHttp.GetResponse( szVerUrl, szResponse, sizeof(szResponse), szError, 60*1000 ) )
		{
			//	是否开启模块，如果不开启可能要等到下一次服务器重启后才能再次跟服务器联系
			memset( szTemp, 0, sizeof(szTemp) );
			delib_get_cookie_value( szResponse, "start=", szTemp, sizeof(szTemp) );
			if ( _tcslen( szTemp ) )
			{
				m_stConfig.dwStart = (BOOL)( atol(szTemp) );
			}
			else
			{
				m_stConfig.dwStart = CVWSPEEDTESTER_START_CLOSED;
			}

			//	工作超时
			memset( szTemp, 0, sizeof(szTemp) );
			delib_get_cookie_value( szResponse, "timeout=", szTemp, sizeof(szTemp) );
			if ( _tcslen( szTemp ) )
			{
				m_stConfig.dwTimeout = (DWORD)( atol(szTemp) );
			}
			else
			{
				m_stConfig.dwTimeout = 60*1000;
			}

			//	每个URL测试中间的休息时间
			memset( szTemp, 0, sizeof(szTemp) );
			delib_get_cookie_value( szResponse, "testspantime=", szTemp, sizeof(szTemp) );
			if ( _tcslen( szTemp ) )
			{
				m_stConfig.dwTestSpanTime = (DWORD)( atol(szTemp) );
			}
			else
			{
				m_stConfig.dwTestSpanTime = CVWSPEEDTESTER_DEFAULT_TESTSPANTIME;
			}

			//	一次任务完成后休息时间
			memset( szTemp, 0, sizeof(szTemp) );
			delib_get_cookie_value( szResponse, "taskspantime=", szTemp, sizeof(szTemp) );
			if ( _tcslen( szTemp ) )
			{
				m_stConfig.dwTaskSpanTime = (DWORD)( atol(szTemp) );
			}
			else
			{
				m_stConfig.dwTaskSpanTime = CVWSPEEDTESTER_DEFAULT_TASKSPANTIME;
			}

			delib_get_cookie_value( szResponse, "urlgt=", m_stConfig.szUrlGetTask, sizeof(m_stConfig.szUrlGetTask) );
			delib_get_cookie_value( szResponse, "urlsr=", m_stConfig.szUrlSendResult, sizeof(m_stConfig.szUrlSendResult) );

			//
			//	判断是否有效
			//
			if ( _tcslen( m_stConfig.szUrlGetTask ) && _tcslen( m_stConfig.szUrlSendResult ) )
			{
				bRet = TRUE;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return bRet;
}


/**
 *	@ public
 *	设置配置
 */
BOOL CVwSpeedTester::SetConfig( STVWSPEEDTESTERCONFIG * pstConfig )
{
	if ( NULL == pstConfig )
	{
		return FALSE;
	}

	BOOL bRet	= FALSE;

	__try
	{
		if ( pstConfig->dwTimeout >= 1000 )
		{
			if ( _tcslen( pstConfig->szUrlGetTask ) && _tcslen( pstConfig->szUrlSendResult ) )
			{
				bRet = TRUE;

				//	拷贝配置参数
				m_stConfig = ( * pstConfig );
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return bRet;
}

/**
 *	@ public
 *	开始工作
 */
BOOL CVwSpeedTester::IsStart()
{
	BOOL bRet	= FALSE;

	switch( m_stConfig.dwStart )
	{
	case CVWSPEEDTESTER_START_CLOSED:
		{
			//	关闭
			bRet = FALSE;
		}
		break;
	case CVWSPEEDTESTER_START_ALL:
		{
			//	所有版本启动
			bRet = TRUE;
		}
		break;
	case CVWSPEEDTESTER_START_TRIAL:
		{
			//	非正式版本启动
			if ( ! m_bRegUser )
			{
				bRet = TRUE;
			}
		}
		break;
	case CVWSPEEDTESTER_START_REGED:
		{
			//	正式版本启动
			if ( m_bRegUser )
			{
				bRet = TRUE;
			}
		}
		break;
	}

	return bRet;
}

/**
 *	@ public
 *	当所有任务完成后的休息时间
 */
DWORD CVwSpeedTester::GetTaskSpanTime()
{
	return ( m_stConfig.dwTaskSpanTime ? m_stConfig.dwTaskSpanTime : CVWSPEEDTESTER_DEFAULT_TASKSPANTIME );
}

/**
 *	@ public
 *	开始工作
 */
BOOL CVwSpeedTester::DoWorks()
{
	BOOL bRet	= FALSE;

	__try
	{
		if ( GetClientIpAddrFromServer() )
		{
			if ( GetTaskFromServer() )
			{
				if ( TestAllTask() )
				{
					bRet = TRUE;
					
					//if ( SendAllResultToServer() )
					//{
					//	bRet = TRUE;
					//}
				}
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return bRet;
}





//////////////////////////////////////////////////////////////////////////
//	private
//




/**
 *	@ Private
 *	从服务器获取本地出口 IP 地址
 */
BOOL CVwSpeedTester::GetClientIpAddrFromServer()
{
	BOOL  bRet	= FALSE;
	CDeHttp cDeHttp;
	TCHAR szVerUrl[ 1024 ];
	TCHAR szResponse[ MAX_PATH ];
	TCHAR szError[ MAX_PATH ];
	
	__try
	{
		memset( m_stConfig.szClientIp, 0, sizeof(m_stConfig.szClientIp) );
		memset( szResponse, 0, sizeof(szResponse) );
		memset( szError, 0, sizeof(szError) );
		
		GetUrlWithClientInfo( m_stConfig.szUrlGetTask, szVerUrl, sizeof(szVerUrl) );
		if ( cDeHttp.GetResponse( szVerUrl, szResponse, sizeof(szResponse), szError, 60*1000 ) )
		{
			delib_get_cookie_value( szResponse, _T("syscip="), m_stConfig.szClientIp, sizeof(m_stConfig.szClientIp) );
			if ( _tcslen( m_stConfig.szClientIp ) && delib_is_valid_ipaddr( m_stConfig.szClientIp ) )
			{
				bRet = TRUE;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return bRet;
}

/**
 *	@ Private
 *	从服务器获取任务
 */
BOOL CVwSpeedTester::GetTaskFromServer()
{
	BOOL bRet	= FALSE;
	STVWSPEEDTESTERTASKITEM stItem;
	CDeHttp cDeHttp;
	TCHAR szVerUrl[ 1024 ];
	TCHAR szTaskFilename[ MAX_PATH ]	= {0};
	TCHAR szResponse[ 40960 ]		= {0};
	TCHAR szError[ MAX_PATH ]		= {0};
	TCHAR szTemp[ MAX_PATH ]		= {0};
	TCHAR szLine[ 2048 ];
	FILE * fp;

	DWORD dwVwCenterCntLen;		//	访问威盾中心的内容长度
	DWORD dwTickStart;
	DWORD dwVwCenterTimeUsed;	//	访问威盾中心使用的时间

	__try
	{
		m_vcAllTask.clear();

		if ( GetTempDownloadedTaskFileName( szTaskFilename, sizeof(szTaskFilename) ) )
		{
			//	删除临时文件
			if ( delib_file_exists( szTaskFilename ) )
			{
				DeleteFile( szTaskFilename );
			}

			//	下载任务列表
			GetUrlWithClientInfo( m_stConfig.szUrlGetTask, szVerUrl, sizeof(szVerUrl) );
			dwTickStart	= GetTickCount();

			if ( cDeHttp.DownloadFile( szVerUrl, szTaskFilename, NULL, TRUE, szError, 60*1000 ) &&
				delib_get_file_size( szTaskFilename ) )
			{
				bRet	= TRUE;

				//
				//	计算从威盾中心下载配置文件所用的时间
				//	此时间用以计算本地网络能力
				//
				dwVwCenterCntLen	= delib_get_file_size( szTaskFilename );
				dwVwCenterTimeUsed	= GetTickCount() - dwTickStart;

				//
				//	从文件中逐行扫描，解析任务
				//
				fp = fopen( szTaskFilename, _T("r") );
				if ( fp )
				{
					while( ! feof( fp ) )
					{
						memset( szLine, 0, sizeof(szLine) );
						if ( fgets( szLine, sizeof(szLine), fp ) && _tcslen( szLine ) )
						{
							memset( &stItem, 0, sizeof(stItem) );

							stItem.dwVwCenterCntLen		= dwVwCenterCntLen;
							stItem.dwVwCenterTimeUsed	= dwVwCenterTimeUsed;

							memset( szTemp, 0, sizeof(szTemp) );
							delib_get_cookie_value( szLine, _T("timeout="), szTemp, sizeof(szTemp) );
							if ( _tcslen( szTemp ) )
							{
								stItem.dwTimeout = (DWORD)( atol(szTemp) );
							}

							//	for dwReadSleep
							memset( szTemp, 0, sizeof(szTemp) );
							delib_get_cookie_value( szLine, _T("rslp="), szTemp, sizeof(szTemp) );
							if ( _tcslen( szTemp ) )
							{
								stItem.dwReadSleep = (DWORD)( atol(szTemp) );
							}
							if ( 0 == stItem.dwReadSleep || stItem.dwReadSleep > CVWSPEEDTESTER_MAX_READSLEEP )
							{
								stItem.dwReadSleep = CVWSPEEDTESTER_DEFAULT_READSLEEP;
							}

							delib_get_cookie_value( szLine, _T("md5="), stItem.szMd5, sizeof(stItem.szMd5) );
							delib_get_cookie_value( szLine, _T("url="), stItem.szUrl, sizeof(stItem.szUrl) );
							delib_get_cookie_value( szLine, _T("host="), stItem.szHost, sizeof(stItem.szHost) );

							//	for uPort
							memset( szTemp, 0, sizeof(szTemp) );
							delib_get_cookie_value( szLine, _T("port="), szTemp, sizeof(szTemp) );
							if ( _tcslen( szTemp ) )
							{
								stItem.uPort = (UINT)( atol(szTemp) );
							}
							else
							{
								stItem.uPort = 80;
							}

							//	插入 vector
							if ( _tcslen( stItem.szMd5 ) && _tcslen( stItem.szUrl ) && _tcslen( stItem.szHost ) && stItem.uPort )
							{
								if ( ParseIpAddrByHostName( stItem.szHost, stItem.szParsedIpAddr, sizeof(stItem.szParsedIpAddr) ) )
								{
									m_vcAllTask.push_back( stItem );
								}
							}
						}
					}

					fclose( fp );
					fp = NULL;
				}

				//	删除临时文件
				DeleteFile( szTaskFilename );
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return bRet;
}

/**
 *	@ Private
 *	测试所有 URL
 */
BOOL CVwSpeedTester::TestAllTask()
{
	BOOL bRet	= FALSE;
	CDeHttp cDeHttp;
	TCHAR szResponse[ 40960 ];
	TCHAR szError[ MAX_PATH ];
	DWORD dwTimeStart;


	try
	{
		if ( m_vcAllTask.size() )
		{
			bRet = TRUE;

			for ( m_it = m_vcAllTask.begin(); m_it != m_vcAllTask.end(); m_it ++ )
			{
				//
				//	[1] 实际测试
				//
				memset( szResponse, 0, sizeof(szResponse) );
				memset( szError, 0, sizeof(szError) );

				(*m_it).dwRealTimeUsed	= 0;
				(*m_it).dwRealCntLen	= 0;

				dwTimeStart		= GetTickCount();

				if ( cDeHttp.GetResponse
					(
						(*m_it).szUrl,
						szResponse,
						sizeof(szResponse),
						szError,
						( (*m_it).dwTimeout ? (*m_it).dwTimeout : m_stConfig.dwTimeout )
					)
				)
				{
					//	计算内容长度
					(*m_it).dwRealCntLen = _tcslen( szResponse );
				}
				
				//	计算使用时间
				(*m_it).dwRealTimeUsed	= GetTickCount() - dwTimeStart;


				//
				//	[2] 拖延测试
				//
				(*m_it).dwDelayTimeUsed	= 0;
				(*m_it).dwDelayCntLen	= 0;

				/*
				dwTimeStart		= GetTickCount();

				//	计算内容长度
				(*m_it).dwDelayCntLen = HttpGetResponseTest
					(
						(*m_it).szUrl,		//	url
						128,			//	buffer size
						(*m_it).dwReadSleep,	//	read sleep
						( (*m_it).dwTimeout ? (*m_it).dwTimeout : m_stConfig.dwTimeout ),
						szError
					);

				//	计算使用时间
				(*m_it).dwDelayTimeUsed	= GetTickCount() - dwTimeStart;
				*/


				//
				//	向服务器发送报告
				//
				SendResult( m_it );

				//
				//	休息
				//
				if ( m_stConfig.dwTestSpanTime )
				{
					Sleep( m_stConfig.dwTestSpanTime );
				}
			}
		}
	}
	catch( ... )
	{
	}

	return bRet;
}

/**
 *	@ Private
 *	将结果返回给服务器
 */
BOOL CVwSpeedTester::SendResult( STVWSPEEDTESTERTASKITEM * pstItem )
{
	//
	//	每个单元
	//	md5[]=xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx&pip[]=222.222.222.222&ctm[]=1000000&ccl[]=1000000&rtm[]=1000000&rcl[]=40960&dtm[]=1000000&dcl[]=4096022
	//
	
	BOOL bRet;
	TCHAR szServerUrl[ 1024 ];
	TCHAR szSendUrl[ 2048 ];
	TCHAR szParamArray[ MAX_PATH ]	= {0};

	CDeHttp cDeHttp;
	TCHAR szResponse[ MAX_PATH ]	= {0};
	TCHAR szError[ MAX_PATH ]	= {0};

	if ( NULL == pstItem )
	{
		return FALSE;
	}

	//	...
	bRet = FALSE;

	__try
	{
		memset( szServerUrl, 0, sizeof(szServerUrl) );
		if ( GetUrlWithClientInfo( m_stConfig.szUrlSendResult, szServerUrl, sizeof(szServerUrl) ) )
		{
			_sntprintf
			(
				szParamArray, sizeof(szParamArray)-sizeof(TCHAR),
				_T("md5[]=%s&pip[]=%s&ctm[]=%u&ccl[]=%u&rtm[]=%u&rcl[]=%u&dtm[]=%u&dcl[]=%u"),
				pstItem->szMd5,
				pstItem->szParsedIpAddr,
				pstItem->dwVwCenterTimeUsed, pstItem->dwVwCenterCntLen,
				pstItem->dwRealTimeUsed, pstItem->dwRealCntLen,
				pstItem->dwDelayTimeUsed, pstItem->dwDelayCntLen
			);
			_sntprintf( szSendUrl, sizeof(szSendUrl)/sizeof(TCHAR)-1, _T("%s&%s"), szServerUrl, szParamArray );

			//
			//	发送给服务器
			//
			memset( szResponse, 0, sizeof(szResponse) );
			memset( szError, 0, sizeof(szError) );
			if ( cDeHttp.GetResponse( szSendUrl, szResponse, sizeof(szResponse), szError, m_stConfig.dwTimeout ) )
			{
				if ( _tcsstr( szResponse, CVWSPEEDTESTER_RESULT_SUCC ) )
				{
					bRet = TRUE;
				}
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
	
	return bRet;
}


/**
 *	@ Private
 *	将结果返回给服务器
 */
BOOL CVwSpeedTester::SendAllResultToServer()
{
	//
	//	每个单元
	//	md5[]=xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx&pip[]=222.222.222.222&ctm[]=1000000&ccl[]=1000000&rtm[]=1000000&rcl[]=40960&dtm[]=1000000&dcl[]=4096022
	//

	BOOL bRet			= FALSE;
	DWORD dwCount			= 0;
	TCHAR szServerUrl[ 1024 ];
	TCHAR szSendUrl[ 2048 ];
	TCHAR szTemp[ MAX_PATH ]	= {0};
	STVWSPEEDTESTERTASKITEM * pstItem;

	CDeHttp cDeHttp;
	TCHAR szResponse[ MAX_PATH ]	= {0};
	TCHAR szError[ MAX_PATH ]	= {0};
	
	__try
	{
		if ( m_vcAllTask.size() )
		{
			memset( szServerUrl, 0, sizeof(szServerUrl) );
			GetUrlWithClientInfo( m_stConfig.szUrlSendResult, szServerUrl, sizeof(szServerUrl) );

			for ( m_it = m_vcAllTask.begin(); m_it != m_vcAllTask.end(); m_it ++ )
			{
				pstItem = (STVWSPEEDTESTERTASKITEM*)(m_it);

				_sntprintf
				(
					szTemp, sizeof(szTemp)-sizeof(TCHAR),
					_T("md5[]=%s&pip[]=%s&ctm[]=%u&ccl[]=%u&rtm[]=%u&rcl[]=%u&dtm[]=%u&dcl[]=%u"),
					pstItem->szMd5,
					pstItem->szParsedIpAddr,
					pstItem->dwVwCenterTimeUsed, pstItem->dwVwCenterCntLen,
					pstItem->dwRealTimeUsed, pstItem->dwRealCntLen,
					pstItem->dwDelayTimeUsed, pstItem->dwDelayCntLen
				);
				_sntprintf( szSendUrl, sizeof(szSendUrl)/sizeof(TCHAR)-1, _T("%s&%s"), szServerUrl, szTemp );

				//
				//	发送给服务器
				//
				memset( szResponse, 0, sizeof(szResponse) );
				memset( szError, 0, sizeof(szError) );
				if ( cDeHttp.GetResponse( szSendUrl, szResponse, sizeof(szResponse), szError, m_stConfig.dwTimeout ) )
				{
					if ( _tcsstr( szResponse, CVWSPEEDTESTER_RESULT_SUCC ) )
					{
						bRet = TRUE;
					}
				}

				Sleep( 3 * 1000 );
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
	
	return bRet;
}

BOOL CVwSpeedTester::GetTempDownloadedTaskFileName( LPTSTR lpszFilename, DWORD dwSize )
{
	if ( NULL == lpszFilename || 0 == dwSize )
	{
		return FALSE;
	}

	BOOL bRet	= FALSE;
	TCHAR szTempDir[ MAX_PATH ]	= {0};

	__try
	{
		if ( delib_get_window_tempdir( szTempDir, sizeof(szTempDir) ) )
		{
			bRet = TRUE;
			_sntprintf( lpszFilename, dwSize-sizeof(TCHAR), _T("%s\\vwsptt.tmp"), szTempDir );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return bRet;
}

BOOL CVwSpeedTester::GetUrlWithClientInfo( LPCTSTR lpcszUrl, LPTSTR lpszVerUrl, DWORD dwSize )
{
	if ( NULL == lpcszUrl )
	{
		return FALSE;
	}
	if ( NULL == lpszVerUrl || 0 == dwSize )
	{
		return FALSE;
	}

	TCHAR szCheckSum[ 64 ]		= {0};

	//	获取通讯钥匙
	GetCheckSum( szCheckSum, sizeof(szCheckSum) );

	_sntprintf
	(
		lpszVerUrl, dwSize-sizeof(TCHAR),
		_T("%s%scver=%d&reg=%d&checksum=%s"),
		lpcszUrl,
		( _tcschr( lpcszUrl, '?' ) ? "&" : "?" ),
		CVWSPEEDTESTER_VERSION,
		( m_bRegUser ? 1 : 0 ),
		szCheckSum
	);
	OutputDebugString( lpszVerUrl );

	return TRUE;
}


/**
 *	测试网站速度
 */
DWORD CVwSpeedTester::HttpGetResponseTest( LPCTSTR lpcszUrl, DWORD dwReadBufferSize, DWORD dwReadSleep, DWORD dwTimeout, LPTSTR lpszError )
{
	DWORD dwRet	= 0;
	STVWSPEEDTESTERHTTPTHREAD stHttpThread;

	stHttpThread.pvThisCls		= this;
	stHttpThread.bRet		= FALSE;
	stHttpThread.lpcszUrl		= lpcszUrl;
	stHttpThread.dwReadBufferSize	= dwReadBufferSize;
	stHttpThread.dwReadSleep	= dwReadSleep;
	stHttpThread.dwTimeout		= dwTimeout;
	stHttpThread.lpszError		= lpszError;
	stHttpThread.pdwTotalRead	= &dwRet;

	__try
	{
		stHttpThread.hThread = (HANDLE)_beginthreadex
		(
			NULL,
			0,
			&_threadHttpGetResponseTest,
			&stHttpThread,
			0,
			NULL
		);
		if ( NULL == stHttpThread.hThread )
		{
			return 0;
		}

		if ( WAIT_OBJECT_0 != WaitForSingleObject( stHttpThread.hThread, dwTimeout ) )
		{
			TerminateThread( stHttpThread.hThread, 0 );
			while ( WAIT_TIMEOUT == WaitForSingleObject( stHttpThread.hThread, 1000 ) )
			{
				TerminateThread( stHttpThread.hThread, 0 );
			}

			if ( lpszError )
			{
				_tcscpy( lpszError, _T("network connection timeout!") );
			}
		}

		if ( stHttpThread.hThread )
		{
			CloseHandle( stHttpThread.hThread );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( lpszError )
		{
			_tcscpy( lpszError, _T("EXCEPTION_EXECUTE_HANDLER") );
		}
	}

	return dwRet;
}

unsigned __stdcall CVwSpeedTester::_threadHttpGetResponseTest( PVOID arglist )
{
	STVWSPEEDTESTERHTTPTHREAD * pstThread = (STVWSPEEDTESTERHTTPTHREAD*)arglist;

	pstThread->bRet = FALSE;

	if ( NULL == pstThread->lpcszUrl )
	{
		if ( pstThread->lpszError )
		{
			_tcscpy( pstThread->lpszError, _T("Error in param.") );
		}
		return HTTP_THREAD_FAIL;
	}
	if ( 0 == _tcslen(pstThread->lpcszUrl) )
	{
		if ( pstThread->lpszError )
		{
			_tcscpy( pstThread->lpszError, _T("Error in param, empty value.") );
		}
		return HTTP_THREAD_FAIL;
	}
	if ( 0 == pstThread->dwReadBufferSize )
	{
		if ( pstThread->lpszError )
		{
			_tcscpy( pstThread->lpszError, _T("Error in param, zero read buffer size.") );
		}
		return HTTP_THREAD_FAIL;
	}

	HINTERNET hInternet	= NULL;
	HINTERNET hHttpConnect	= NULL;

	DWORD dwBytesRead	= 0;
	DWORD dwTotalRead	= 0;
	LPVOID pBuffer		= NULL;

	TCHAR szAgent[ MAX_PATH ]	= {0};
	TCHAR szHeader[ 1024 ]		= {0};
	TCHAR szSubHeader[ MAX_PATH ]	= {0};

	__try
	{
		_sntprintf( szAgent, sizeof(szAgent)-sizeof(TCHAR), _T("%s"), _T(DEFAULT_AGENT_NAME) );
		_sntprintf( szHeader, sizeof(szHeader)-sizeof(TCHAR), _T("%s"), _T(HTTP_ACCEPT) );

		hInternet = InternetOpen( _T(szAgent), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
		if ( hInternet )
		{
			DWORD dwFlag = INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_PRAGMA_NOCACHE|INTERNET_FLAG_RELOAD;
			hHttpConnect = InternetOpenUrl
				(
					hInternet,
					pstThread->lpcszUrl, 
					szHeader, 
					(DWORD)_tcslen(szHeader), 
					dwFlag,
					0
				);

			dwTotalRead = 0;
			if ( hHttpConnect )
			{
				dwBytesRead	= 0;
				dwTotalRead	= 0;
				pBuffer		= malloc( pstThread->dwReadBufferSize );

				if ( pBuffer )
				{
					//	..
					pstThread->bRet = TRUE;

					while( InternetReadFile( hHttpConnect, pBuffer, pstThread->dwReadBufferSize, &dwBytesRead ) )
					{
						if ( 0 == dwBytesRead )
						{
							break;
						}
						if ( dwTotalRead > CVWSPEEDTESTER_MAX_READBYTES )
						{
							break;
						}

						if ( pstThread->dwReadSleep )
						{
							Sleep( pstThread->dwReadSleep );
						}

						dwTotalRead += dwBytesRead;
					}

					if ( pstThread->pdwTotalRead )
					{
						*pstThread->pdwTotalRead = dwTotalRead;
					}

					if ( 0 == dwTotalRead )
					{
						if ( pstThread->lpszError )
						{
							_tcscpy( pstThread->lpszError, _T("empty webpage.") );
						}
					}

					free( pBuffer );
					pBuffer = NULL;
				}

				::InternetCloseHandle( hHttpConnect );
			}
			else
			{
				if ( pstThread->lpszError )
					_tcscpy( pstThread->lpszError, _T("Error in InternetOpenUrl") );
			}

			::InternetCloseHandle( hInternet );
		}
		else
		{
			if ( pstThread->lpszError )
				_tcscpy( pstThread->lpszError, _T("Error in InternetOpen") );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if ( pstThread->lpszError )
		{
			_tcscpy( pstThread->lpszError, _T("EXCEPTION_EXECUTE_HANDLER") );
		}
		if ( hHttpConnect )
		{
			::InternetCloseHandle( hHttpConnect );
		}
		if ( hInternet )
		{
			::InternetCloseHandle( hInternet );
		}
	}

	return ( TRUE == pstThread->bRet ) ? HTTP_THREAD_SUCC : HTTP_THREAD_FAIL;
}

/**
 *	@ Private
 *	获取验证 MD5 值
 */
BOOL CVwSpeedTester::GetCheckSum( LPTSTR lpszCheckSum, DWORD dwSize )
{
	if ( NULL == lpszCheckSum || 0 == dwSize )
	{
		return FALSE;
	}
	if ( 0 == _tcslen( m_stConfig.szSendKey ) || 0 == _tcslen( m_stConfig.szClientIp ) )
	{
		return FALSE;
	}

	BOOL  bRet			= FALSE;
	TCHAR szString[ MAX_PATH ]	= {0};

	_sntprintf( szString, sizeof(szString)-sizeof(TCHAR), _T("%s-%s"), m_stConfig.szSendKey, m_stConfig.szClientIp );
	if ( delib_get_string_md5( szString, lpszCheckSum, dwSize, (DWORD)_tcslen( szString ) ) )
	{
		bRet = TRUE;
	}

	return bRet;
}

/**
 *	@ Private
 *	解析指定域名的 IP 地址
 */
BOOL CVwSpeedTester::ParseIpAddrByHostName( LPCTSTR lpcszHostnName, LPTSTR lpszLocalAddr, DWORD dwSize )
{
	if ( NULL == lpcszHostnName || 0 == _tcslen( lpcszHostnName ) )
	{
		return FALSE;
	}
	if ( NULL == lpszLocalAddr || 0 == dwSize )
	{
		return FALSE;
	}

	BOOL  bRet			= FALSE;
	WSADATA WSData;
	HOSTENT * lpstHost		= NULL;
	struct sockaddr_in saDest;
	
	__try
	{
		if ( 0 == WSAStartup( MAKEWORD(1,1), &WSData ) )
		{
			lpstHost = gethostbyname( lpcszHostnName );

			if ( lpstHost )
			{
				//	Setup destination socket address
				saDest.sin_addr.s_addr	= *((u_long FAR *)(lpstHost->h_addr));
				saDest.sin_family	= AF_INET;
				saDest.sin_port		= 0;

				//	Get IP address
				_sntprintf( lpszLocalAddr, dwSize-sizeof(TCHAR),  _T("%s"), inet_ntoa(saDest.sin_addr) );
				if ( _tcslen( lpszLocalAddr ) > 0 && _tcsstr( lpszLocalAddr, "." ) )
				{
					bRet = TRUE;
				}
				else
				{
					//	Failed to get ip address
				}
			}
			else
			{
				//	Host not found
			}

			WSACleanup();
		}
		else
		{
			//	Cannot find Winsock.
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		// 线程异常错误退出
	}

	return bRet;
}