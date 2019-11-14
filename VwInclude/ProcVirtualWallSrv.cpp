// ProcVirtualWallSrv.cpp: implementation of the CProcVirtualWallSrv class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProcVirtualWallSrv.h"

#include "process.h"
#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )

#include <algorithm>
#include <functional>      // For greater<int>( )
#include <iostream>

/**
 *	compare functions
 */
bool __vector_cmpfunc_greater_TopUrlList_Passed( STVIRTUALWALLSRVTOPURL & l, STVIRTUALWALLSRVTOPURL & r )
{
	return ( l.dwCountPassed > r.dwCountPassed );
}
bool __vector_cmpfunc_greater_TopUrlList_Denied( STVIRTUALWALLSRVTOPURL & l, STVIRTUALWALLSRVTOPURL & r )
{
	return ( l.dwCountDenied > r.dwCountDenied );
}
bool __vector_cmpfunc_greater_TopUrlList_Blocked( STVIRTUALWALLSRVTOPURL & l, STVIRTUALWALLSRVTOPURL & r )
{
	return ( l.dwCountBlocked > r.dwCountBlocked );
}
INT __CProcVirtualWallSrv_qsort_ActivityList_compare( const void *arg1, const void *arg2 )
{
	return ( ( (STPACKETACTIVITYQUEUEREPORTLISTITEM*)arg2 )->stItem.stData.lnStartTickCount - ( (STPACKETACTIVITYQUEUEREPORTLISTITEM*)arg1 )->stItem.stData.lnStartTickCount );
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcVirtualWallSrv::CProcVirtualWallSrv()
{
	m_lpvShareMemoryBuffer	= NULL;
	m_hShareMemoryFileMap	= NULL;

	m_vcRequestList.clear();
	m_vcRefererList.clear();
	GetLocalTime( &m_stLastVectorRecord );
}
CProcVirtualWallSrv::~CProcVirtualWallSrv()
{

}

/**
*	分配一块共享内存
*/
BOOL CProcVirtualWallSrv::alloc_share_memory( IN DWORD dwSize, IN LPCTSTR lpcszMemName, OUT VOID ** lppvBuffer, OUT HANDLE * phFileMap, OUT BOOL * lpbCreate )
{
	//
	//	dwSize		- [in]  要分配的内存大小
	//	lpcszMemName	- [in]  共享内存名字
	//	lpvBuffer	- [out] 返回共享内存区地址
	//	hFileMap	- [out] 文件句柄
	//	lpbCreate	- [out] 是否是新创建的
	//	RETURN		- TRUE / FALSE
	//
	if ( 0 == dwSize )
	{
		return FALSE;
	}
	if ( NULL == lpcszMemName || NULL == lppvBuffer || NULL == phFileMap || NULL == lpbCreate )
	{
		return FALSE;
	}
	
	BOOL bRet;
	PSECURITY_DESCRIPTOR pSD;
	PSECURITY_ATTRIBUTES pSA;


	//	...
	bRet = FALSE;

	__try
	{
		//
		//	创建内存映射
		//
		*lppvBuffer	= NULL;
		*lpbCreate	= FALSE;

		*phFileMap = OpenFileMapping( FILE_MAP_READ|FILE_MAP_WRITE, FALSE, lpcszMemName );
		if ( ! (*phFileMap) )
		{
			//	FileMap 不存在，要创建之
			pSD	= (PSECURITY_DESCRIPTOR)malloc( SECURITY_DESCRIPTOR_MIN_LENGTH ); 
			pSA	= (PSECURITY_ATTRIBUTES)malloc( sizeof(SECURITY_ATTRIBUTES) ); 
			if ( pSD && pSA && 
				InitializeSecurityDescriptor( pSD, SECURITY_DESCRIPTOR_REVISION ) &&
				SetSecurityDescriptorDacl( pSD, TRUE, (PACL)NULL, FALSE ) ) 
			{
				pSA->nLength			= sizeof( SECURITY_ATTRIBUTES );
				pSA->lpSecurityDescriptor	= pSD;
				pSA->bInheritHandle		= TRUE;

				//	...
				*lpbCreate = TRUE;
				(*phFileMap) = CreateFileMapping( INVALID_HANDLE_VALUE, pSA, PAGE_READWRITE, 0, dwSize, lpcszMemName );
			}
		}
		if ( (*phFileMap) )
		{
			(*lppvBuffer) = (LPVOID)MapViewOfFile( (*phFileMap), FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, 0 );
			if ( (*lppvBuffer) )
			{
				if ( (*lpbCreate) )
				{
					memset( (*lppvBuffer), 0, dwSize );
				}

				bRet = TRUE;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
	
	//	..
	return bRet;
}

/**
 *	打开一块共享内存
 */
BOOL CProcVirtualWallSrv::open_share_memory( IN LPCTSTR lpcszMemName, OUT VOID ** lppvBuffer, OUT HANDLE * phFileMap )
{
	//
	//	lpcszMemName	- [in]  共享内存名字
	//	lpvBuffer	- [out] 返回共享内存区地址
	//	phFileMap	- [out] 文件句柄
	//	RETURN		- TRUE / FALSE
	//
	if ( NULL == lpcszMemName || NULL == lppvBuffer || NULL == phFileMap )
	{
		return FALSE;
	}
	
	BOOL bRet;

	//	...
	bRet = FALSE;

	__try
	{
		//
		//	创建内存映射
		//
		*lppvBuffer	= NULL;
		
		*phFileMap = OpenFileMapping( FILE_MAP_READ|FILE_MAP_WRITE, FALSE, lpcszMemName );
		if ( (*phFileMap) )
		{
			(*lppvBuffer) = (LPVOID)MapViewOfFile( (*phFileMap), FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, 0 );
			if ( (*lppvBuffer) )
			{
				bRet = TRUE;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
	
	//	..
	return bRet;
}

/**
 *	关闭一块共享内存
 */
VOID CProcVirtualWallSrv::close_share_memory( IN OUT VOID ** lppvBuffer, IN OUT HANDLE * phFileMap )
{
	if ( NULL == lppvBuffer || NULL == phFileMap )
	{
		return;
	}

	__try
	{
		if ( *lppvBuffer )
		{
			UnmapViewOfFile( *lppvBuffer );
			*lppvBuffer = NULL;
		}
		if ( *phFileMap )
		{
			CloseHandle( *phFileMap );
			*phFileMap = NULL;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}



BOOL CProcVirtualWallSrv::CreateShareMemory()
{
	BOOL  bRet;
	BOOL  bCreate;

	//	...
	bRet	= FALSE;


	__try
	{
		bCreate	= FALSE;

		m_lpvShareMemoryBuffer	= NULL;
		m_hShareMemoryFileMap	= NULL;

		if ( alloc_share_memory
			(
				sizeof( STVIRTUALWALLSRVSHAREMEMORY ),
				PROCVIRTUALWALLSRV_SHARE_MEMORY_NAME,
				(VOID **)( &m_lpvShareMemoryBuffer ),
				&m_hShareMemoryFileMap,
				&bCreate
			)
		)
		{
			bRet = TRUE;

			//	...
			memset( m_lpvShareMemoryBuffer->ArrActivityList, 0, sizeof(m_lpvShareMemoryBuffer->ArrActivityList) );
			m_lpvShareMemoryBuffer->dwVersion			= 1;
			m_lpvShareMemoryBuffer->dwActivityListCount		= sizeof( m_lpvShareMemoryBuffer->ArrActivityList ) / sizeof( m_lpvShareMemoryBuffer->ArrActivityList[0] );
			m_lpvShareMemoryBuffer->bSendActivityReportToService	= FALSE;
			m_lpvShareMemoryBuffer->dwVwCfgNewActivitySetStatusTick	= 0;

			//	..
			InitializeCriticalSection( & m_lpvShareMemoryBuffer->oCrSecActivityListLock );
			InitializeCriticalSection( & m_lpvShareMemoryBuffer->oCrSecActivitySetStatus );
			InitializeCriticalSection( & m_lpvShareMemoryBuffer->oCrSecCoreWorkRecordLock );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return bRet;
}
BOOL CProcVirtualWallSrv::OpenShareMemory()
{
	BOOL  bRet;

	//	...
	bRet = FALSE;

	__try
	{
		if ( open_share_memory
			(
				PROCVIRTUALWALLSRV_SHARE_MEMORY_NAME,
				(VOID **)( &m_lpvShareMemoryBuffer ),
				&m_hShareMemoryFileMap
			)
		)
		{
			bRet = TRUE;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return bRet;
}
VOID CProcVirtualWallSrv::CloseShareMemory()
{
	__try
	{
		if ( m_lpvShareMemoryBuffer && m_hShareMemoryFileMap )
		{
			close_share_memory( (VOID **)( &m_lpvShareMemoryBuffer ), &m_hShareMemoryFileMap );
			m_lpvShareMemoryBuffer	= NULL;
			m_hShareMemoryFileMap	= NULL;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}

BOOL CProcVirtualWallSrv::CreateAllThreads()
{
	__try
	{
		m_cThSleepActivityQueueListCleanUp.m_hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				&_threadActivityQueueListCleanUp,
				(void*)this,
				0,
				&m_cThSleepActivityQueueListCleanUp.m_uThreadId
			);

		m_cThSleepTopUrlListDump.m_hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				&_threadTopUrlListDump,
				(void*)this,
				0,
				&m_cThSleepTopUrlListDump.m_uThreadId
			);
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return TRUE;
}
VOID CProcVirtualWallSrv::EndAllThreads()
{
	__try
	{
		if ( m_cThSleepActivityQueueListCleanUp.m_hThread )
		{
			m_cThSleepActivityQueueListCleanUp.EndSleep();
			m_cThSleepActivityQueueListCleanUp.EndThread( &m_cThSleepActivityQueueListCleanUp.m_hThread );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}


VOID CProcVirtualWallSrv::ActivityQueue_Lock()
{
	__try
	{
		if ( m_lpvShareMemoryBuffer )
		{
			EnterCriticalSection( & m_lpvShareMemoryBuffer->oCrSecActivityListLock );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}
VOID CProcVirtualWallSrv::ActivityQueue_Unlock()
{
	__try
	{
		if ( m_lpvShareMemoryBuffer )
		{
			LeaveCriticalSection( & m_lpvShareMemoryBuffer->oCrSecActivityListLock );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}


VOID CProcVirtualWallSrv::ActivitySetStatus_Lock()
{
	__try
	{
		if ( m_lpvShareMemoryBuffer )
		{
			LeaveCriticalSection( & m_lpvShareMemoryBuffer->oCrSecActivitySetStatus );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}
VOID CProcVirtualWallSrv::ActivitySetStatus_Unlock()
{
	__try
	{
		if ( m_lpvShareMemoryBuffer )
		{
			LeaveCriticalSection( & m_lpvShareMemoryBuffer->oCrSecActivitySetStatus );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}


VOID CProcVirtualWallSrv::CoreWorkRecord_Lock()
{
	__try
	{
		if ( m_lpvShareMemoryBuffer )
		{
			EnterCriticalSection( & m_lpvShareMemoryBuffer->oCrSecCoreWorkRecordLock );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}
VOID CProcVirtualWallSrv::CoreWorkRecord_Unlock()
{
	__try
	{
		if ( m_lpvShareMemoryBuffer )
		{
			LeaveCriticalSection( & m_lpvShareMemoryBuffer->oCrSecCoreWorkRecordLock );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}




LONG CProcVirtualWallSrv::ActivityQueue_GetOperateIndex( DWORD dwAppPoolNameCrc32, LONG lnCyclePoolIdx )
{
	LONG lnRet;
	BOOL bExistItem;
	BOOL bInserted;
	INT  i;

	if ( NULL == m_lpvShareMemoryBuffer || 0 == m_lpvShareMemoryBuffer->dwActivityListCount )
	{
		return -1;
	}

	//	...
	lnRet = -1;


	__try
	{
		ActivityQueue_Lock();
		{
			//	查找正在使用的有效项
			bExistItem = FALSE;
			for ( i = 0; i < m_lpvShareMemoryBuffer->dwActivityListCount; i ++ )
			{
				if ( m_lpvShareMemoryBuffer->ArrActivityList[ i ].bUse )
				{
					if ( m_lpvShareMemoryBuffer->ArrActivityList[ i ].stItem.stData.dwAppPoolNameCrc32 == dwAppPoolNameCrc32 &&
						m_lpvShareMemoryBuffer->ArrActivityList[ i ].stItem.stData.lnCyclePoolIdx == lnCyclePoolIdx )
					{
						//	成功得到
						lnRet = i;
						bExistItem = TRUE;				
						break;
					}
				}
			}

			if ( ! bExistItem )
			{
				//	插入
				bInserted = FALSE;
				for ( i = 0; i < m_lpvShareMemoryBuffer->dwActivityListCount; i ++ )
				{
					if ( FALSE == m_lpvShareMemoryBuffer->ArrActivityList[ i ].bUse )
					{
						//	成功得到
						lnRet = i;
						bInserted = TRUE;				
						break;
					}
				}

				if ( ! bInserted )
				{
					//	强行插入，更新到最后一个
					lnRet = m_lpvShareMemoryBuffer->dwActivityListCount - 1;
				}
			}
		}
		ActivityQueue_Unlock();
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	//	...
	return lnRet;
}


/**
 *	@ Public
 *	ActivityQueueReport 写入信息
 */
BOOL CProcVirtualWallSrv::ActivityQueue_SetReport( STPACKETACTIVITYQUEUEREPORT * pstActReport )
{
	//
	//	pstActReport	- [in] 信息
	//
	//	Remark		- 调用该函数前必须先调用 OpenShareMemory() 并成功才可以
	//
	//

	BOOL bRet;
	BOOL bInserted;
	BOOL bExistItem;
	INT  i;
	TCHAR szTemp[ MAX_PATH ];

	if ( NULL == pstActReport )
	{
		return FALSE;
	}
	if ( NULL == m_lpvShareMemoryBuffer || 0 == m_lpvShareMemoryBuffer->dwActivityListCount )
	{
		return FALSE;
	}

	//	...
	bRet = FALSE;

	__try
	{
		//OutputDebugString( "$$$$$$$$$$ Activity 22222222" );
		if ( pstActReport->stData.uServerPort > 0 &&
			0 != pstActReport->stData.szHost[ 0 ] && 0 != pstActReport->stData.szFullUri[0] )
		{
			//OutputDebugString( "$$$$$$$$$$ Activity 33333333" );
			//	...
			//_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1,
			//	"###ActivityQueueReporterServerAnswerFunc - pid=%d,app=%s - action=%d[%02d:%02d:%02d] | http://%s:%d%s",
			//	pstActReport->stData.dwProcessId, pstActReport->stData.szAppPoolName,
			//	pstActReport->stData.dwAction,
			//	pstActReport->stData.stActionTime.stTime[ pstActReport->stData.dwAction ].wHour,
			//	pstActReport->stData.stActionTime.stTime[ pstActReport->stData.dwAction ].wMinute,
			//	pstActReport->stData.stActionTime.stTime[ pstActReport->stData.dwAction ].wSecond,
			//	pstActReport->stData.szHost, pstActReport->stData.uServerPort, pstActReport->stData.szFullUri );
			//OutputDebugString( szTemp );

			ActivityQueue_Lock();

			bExistItem = FALSE;

			//	查找并可能更新
			for ( i = 0; i < m_lpvShareMemoryBuffer->dwActivityListCount; i ++ )
			{
				if ( m_lpvShareMemoryBuffer->ArrActivityList[ i ].bUse )
				{
					if ( m_lpvShareMemoryBuffer->ArrActivityList[ i ].stItem.stData.dwAppPoolNameCrc32 == pstActReport->stData.dwAppPoolNameCrc32 &&
						m_lpvShareMemoryBuffer->ArrActivityList[ i ].stItem.stData.lnCyclePoolIdx == pstActReport->stData.lnCyclePoolIdx )
					{
						//	成功更新
						bRet = TRUE;

						bExistItem = TRUE;
						m_lpvShareMemoryBuffer->ArrActivityList[ i ].stItem = *pstActReport;

						_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1,
							"SRV-$$$$$$$$$$ Activity update! lnCyclePoolIdx=%d",
							pstActReport->stData.lnCyclePoolIdx );
						OutputDebugString( szTemp );

						break;
					}
				}
			}

			if ( ! bExistItem )
			{
				//	插入
				bInserted = FALSE;
				for ( i = 0; i < m_lpvShareMemoryBuffer->dwActivityListCount; i ++ )
				{
					if ( FALSE == m_lpvShareMemoryBuffer->ArrActivityList[ i ].bUse )
					{
						//	成功插入
						bRet = TRUE;

						bInserted = TRUE;

						m_lpvShareMemoryBuffer->ArrActivityList[ i ].bUse	= TRUE;
						m_lpvShareMemoryBuffer->ArrActivityList[ i ].stItem	= *pstActReport;

						_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1, _T("SRV- inserted[lnCyclePoolIdx=%d]"), pstActReport->stData.lnCyclePoolIdx );
						OutputDebugString( szTemp );

						break;
					}
				}
				if ( ! bInserted )
				{
					//	成功插入
					bRet = TRUE;

					//	强行插入，更新到最后一个
					m_lpvShareMemoryBuffer->ArrActivityList[ m_lpvShareMemoryBuffer->dwActivityListCount - 1 ].stItem = *pstActReport;

					_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1, _T("SRV- force to inserted to last") );
					OutputDebugString( szTemp );
				}
			}

			//	排序
			//qsort
			//(
			//	(void *)m_lpvShareMemoryBuffer->ArrActivityList,
			//	(size_t)m_lpvShareMemoryBuffer->dwActivityListCount,
			//	sizeof(STPACKETACTIVITYQUEUEREPORTLISTITEM),
			//	__CProcVirtualWallSrv_qsort_ActivityList_compare
			//);

			ActivityQueue_Unlock();
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return bRet;
}

/**
 *	@ Public
 *	ActivityQueueReport 写入信息
 */
BOOL CProcVirtualWallSrv::ActivityQueue_TestReport()
{
	INT  i;
	TCHAR szTemp[ MAX_PATH ];
	STPACKETACTIVITYQUEUEREPORT * pstTmp;

	if ( NULL == m_lpvShareMemoryBuffer || 0 == m_lpvShareMemoryBuffer->dwActivityListCount )
	{
		return FALSE;
	}

	__try
	{
		for ( i = 0; i < m_lpvShareMemoryBuffer->dwActivityListCount; i ++ )
		{
			if ( FALSE == m_lpvShareMemoryBuffer->ArrActivityList[ i ].bUse )
			{
				continue;
			}

			pstTmp = & m_lpvShareMemoryBuffer->ArrActivityList[ i ].stItem;

			_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1, 
				_T("SRV- Test_GetActivityQueueReport - [ci=%d]http://%s:%d%s"), 
				pstTmp->stData.lnCyclePoolIdx,
				pstTmp->stData.szHost, pstTmp->stData.uServerPort, pstTmp->stData.szFullUri );
			OutputDebugString( szTemp );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return TRUE;
}





//////////////////////////////////////////////////////////////////////////
//	Private
//
unsigned __stdcall CProcVirtualWallSrv::_threadActivityQueueListCleanUp( PVOID arglist )
{
	if ( NULL == arglist )
	{
		return 1;
	}
	
	__try
	{
		CProcVirtualWallSrv * pThis = (CProcVirtualWallSrv*)arglist;
		if ( pThis )
		{
			pThis->ActivityQueueListCleanUpProc();
		}
		
		_endthreadex( 0 );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
	
	return 0;
}
VOID CProcVirtualWallSrv::ActivityQueueListCleanUpProc()
{
	while( m_cThSleepActivityQueueListCleanUp.IsContinue() )
	{
		ActivityQueueListCleanUpWorker();

		//	休息
		m_cThSleepActivityQueueListCleanUp.EventSleep( 1000 );
	}
}
VOID CProcVirtualWallSrv::ActivityQueueListCleanUpWorker()
{
	INT i;

	__try
	{
		if ( m_lpvShareMemoryBuffer )
		{
			ActivityQueue_Lock();

			//	删除
			for ( i = 0; i < m_lpvShareMemoryBuffer->dwActivityListCount; i ++ )
			{
				if ( m_lpvShareMemoryBuffer->ArrActivityList[ i ].bUse )
				{
					if ( NOTIFY_ACTION_END_OF_REQUEST_END == m_lpvShareMemoryBuffer->ArrActivityList[ i ].stItem.stData.dwAction )
					{
						//RecordTopList( & ( m_lpvShareMemoryBuffer->ArrActivityList[ i ].stItem.stData ) );
						
						//
						m_lpvShareMemoryBuffer->ArrActivityList[ i ].bUse = FALSE;
					}
				}
			}
			
			ActivityQueue_Unlock();
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}

unsigned __stdcall CProcVirtualWallSrv::_threadTopUrlListDump( PVOID arglist )
{
	if ( NULL == arglist )
	{
		return 1;
	}
	
	__try
	{
		CProcVirtualWallSrv * pThis = (CProcVirtualWallSrv*)arglist;
		if ( pThis )
		{
			pThis->TopUrlListDumpProc();
		}
		
		_endthreadex( 0 );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
	
	return 0;
}
VOID CProcVirtualWallSrv::TopUrlListDumpProc()
{
	return;

	while( m_cThSleepTopUrlListDump.IsContinue() )
	{
		DumpTopList();

		//	休息
		m_cThSleepTopUrlListDump.EventSleep( 60 * 1000 );
	}
}


VOID CProcVirtualWallSrv::RecordTopList( STPACKETACTIVITYQUEUEREPORTDATA * pstData )
{
	SYSTEMTIME st;
	STVIRTUALWALLSRVTOPURL stUrl;
	vector<STVIRTUALWALLSRVTOPURL>::iterator it;
	BOOL bExistItem;
	

	if ( NULL == pstData )
	{
		return;
	}

	__try
	{
		//
		//	删除前记录一下
		//
		GetLocalTime( &st );

		if ( m_stLastVectorRecord.wDay == st.wDay )
		{
			//
			//	[1] for top request
			//
			memset( &stUrl, 0, sizeof(stUrl) );

			if ( 80 == pstData->uServerPort )
			{
				_sntprintf( stUrl.szUrl, sizeof(stUrl.szUrl)/sizeof(TCHAR)-1, 
					_T("http://%s%s"), pstData->szHost, pstData->szFullUri );
			}
			else
			{
				_sntprintf( stUrl.szUrl, sizeof(stUrl.szUrl)/sizeof(TCHAR)-1, 
					_T("http://%s:%d%s"), pstData->szHost, pstData->uServerPort, pstData->szFullUri );
			}
			stUrl.dwLength	= _tcslen(stUrl.szUrl);
			stUrl.dwCrc32	= m_cCrc32.GetCrc32( (BYTE*)stUrl.szUrl, stUrl.dwLength );

			if ( REQUEST_VW_DENY == ( pstData->dwRequestFlag & REQUEST_VW_DENY ) )
			{
				stUrl.dwCountDenied = 1;
			}
			else if ( REQUEST_VW_BLOCK == ( pstData->dwRequestFlag & REQUEST_VW_BLOCK ) ||
				REQUEST_VW_LIMITTHREAD == ( pstData->dwRequestFlag & REQUEST_VW_LIMITTHREAD ) ||
				REQUEST_VW_LIMITPLAYONLY == ( pstData->dwRequestFlag & REQUEST_VW_LIMITPLAYONLY ) ||
				REQUEST_VW_LIMITURLCHRS == ( pstData->dwRequestFlag & REQUEST_VW_LIMITURLCHRS ) ||
				REQUEST_VW_LIMITCONTENTPOST == ( pstData->dwRequestFlag & REQUEST_VW_LIMITCONTENTPOST ) ||
				REQUEST_VW_LIMITCONTENTSEND == ( pstData->dwRequestFlag & REQUEST_VW_LIMITCONTENTSEND ) ||
				REQUEST_VW_ANTIATTACK == ( pstData->dwRequestFlag & REQUEST_VW_ANTIATTACK ) ||
				REQUEST_VW_LIMITDAYIP == ( pstData->dwRequestFlag & REQUEST_VW_LIMITDAYIP ) ||
				REQUEST_VW_LIMITHOST == ( pstData->dwRequestFlag & REQUEST_VW_LIMITHOST ) ||
				REQUEST_VW_LIMITIP == ( pstData->dwRequestFlag & REQUEST_VW_LIMITIP )
				)
			{
				stUrl.dwCountBlocked = 1;
			}
			else
			{
				stUrl.dwCountPassed = 1;
			}

			bExistItem	= FALSE;
			for ( it = m_vcRequestList.begin(); it != m_vcRequestList.end(); it ++ )
			{
				if ( stUrl.dwCrc32 == (*it).dwCrc32 )
				{
					bExistItem = TRUE;

					(*it).dwCountDenied += stUrl.dwCountDenied;
					(*it).dwCountBlocked += stUrl.dwCountBlocked;
					(*it).dwCountPassed += stUrl.dwCountPassed;

					//	..
					break;
				}
			}
			if ( ! bExistItem )
			{
				if ( m_vcRequestList.size() < PROCVIRTUALWALLSRV_MAX_VECTOR )
				{
					m_vcRequestList.push_back( stUrl );
				}
			}


			//
			//	[2] for top referer
			//
			memset( &stUrl, 0, sizeof(stUrl) );

			_sntprintf( stUrl.szUrl, sizeof(stUrl.szUrl)/sizeof(TCHAR)-1, _T("%s"), pstData->szReferer+6 );
			stUrl.dwLength	= _tcslen(stUrl.szUrl);
			stUrl.dwCrc32	= m_cCrc32.GetCrc32( (BYTE*)stUrl.szUrl, stUrl.dwLength );

			bExistItem	= FALSE;
			for ( it = m_vcRefererList.begin(); it != m_vcRefererList.end(); it ++ )
			{
				if ( stUrl.dwCrc32 == (*it).dwCrc32 )
				{
					bExistItem = TRUE;

					(*it).dwCountPassed ++;

					//	..
					break;
				}
			}
			if ( ! bExistItem )
			{
				stUrl.dwCountPassed = 1;
				if ( m_vcRefererList.size() < PROCVIRTUALWALLSRV_MAX_VECTOR )
				{
					m_vcRefererList.push_back( stUrl );
				}
			}
		}
		else
		{
			m_stLastVectorRecord.wDay = st.wDay;

			//	TODO:
			//	1 保存到共享内存的 TOP 列表
			//	2 保存到文件，以便总结月TOP，年TOP
			DumpTopList();


			//	清理掉记录
			m_vcRequestList.clear();
			m_vcRefererList.clear();
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}

VOID CProcVirtualWallSrv::DumpTopList()
{
	vector<STVIRTUALWALLSRVTOPURL>::iterator it;
	INT  nCount;

	__try
	{
		if ( m_lpvShareMemoryBuffer )
		{
			//	输入出 TOP 100
			memset( m_lpvShareMemoryBuffer->ArrTopRequestPassed, 0, sizeof(m_lpvShareMemoryBuffer->ArrTopRequestPassed) );
			sort( m_vcRequestList.begin(), m_vcRequestList.end(), __vector_cmpfunc_greater_TopUrlList_Passed );
			for ( it = m_vcRequestList.begin(), nCount = 0;
				it != m_vcRequestList.end() && nCount < VWANITILEECHS_MAX_TOPURL;
				it ++, nCount ++ )
			{
				m_lpvShareMemoryBuffer->ArrTopRequestPassed[ nCount ] = (*it);
			}
			m_lpvShareMemoryBuffer->dwTopRequestPassedCount = nCount;

			memset( m_lpvShareMemoryBuffer->ArrTopRequestDenied, 0, sizeof(m_lpvShareMemoryBuffer->ArrTopRequestDenied) );
			sort( m_vcRequestList.begin(), m_vcRequestList.end(), __vector_cmpfunc_greater_TopUrlList_Denied );
			for ( it = m_vcRequestList.begin(), nCount = 0;
				it != m_vcRequestList.end() && nCount < VWANITILEECHS_MAX_TOPURL;
				it ++, nCount ++ )
			{
				m_lpvShareMemoryBuffer->ArrTopRequestDenied[ nCount ] = (*it);
			}
			m_lpvShareMemoryBuffer->dwTopRequestDeniedCount = nCount;
			
			memset( m_lpvShareMemoryBuffer->ArrTopRequestBlocked, 0, sizeof(m_lpvShareMemoryBuffer->ArrTopRequestBlocked) );
			sort( m_vcRequestList.begin(), m_vcRequestList.end(), __vector_cmpfunc_greater_TopUrlList_Blocked );
			for ( it = m_vcRequestList.begin(), nCount = 0;
				it != m_vcRequestList.end() && nCount < VWANITILEECHS_MAX_TOPURL;
				it ++, nCount ++ )
			{
				m_lpvShareMemoryBuffer->ArrTopRequestBlocked[ nCount ] = (*it);
			}
			m_lpvShareMemoryBuffer->dwTopRequestBlockedCount = nCount;

			//	...
			memset( m_lpvShareMemoryBuffer->ArrTopReferer, 0, sizeof(m_lpvShareMemoryBuffer->ArrTopReferer) );
			sort( m_vcRefererList.begin(), m_vcRefererList.end(), __vector_cmpfunc_greater_TopUrlList_Passed );
			for ( it = m_vcRefererList.begin(), nCount = 0;
				it != m_vcRefererList.end() && nCount < VWANITILEECHS_MAX_TOPURL;
				it ++, nCount ++ )
			{
				m_lpvShareMemoryBuffer->ArrTopReferer[ nCount ] = (*it);
			}
			m_lpvShareMemoryBuffer->dwTopRefererCount = nCount;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

}