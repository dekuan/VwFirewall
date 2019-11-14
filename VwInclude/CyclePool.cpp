// CyclePool.cpp: implementation of the CCyclePool class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CyclePool.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCyclePool::CCyclePool( UINT lnPoolDepth )
{
	BOOL bFileMapCreate	= FALSE;
	LPVOID lpvBuffer	= NULL;
	HANDLE hFileMap		= NULL;
	UINT i;


	//
	//	Is init successfully
	//
	m_bInitSucc		= FALSE;
	m_pstPoolListHeader	= NULL;
	m_pstPoolList		= NULL;

	//
	//	指定当前在线用户池的深度
	//	默认是 3000
	//
	m_uPoolDepth	= lnPoolDepth;


	if ( m_uPoolDepth > 0 )
	{
		//	进程内分配内存的方式
		bFileMapCreate	= TRUE;
		m_pstPoolListHeader = new STCYCLEPOOLHEADER;
		if ( m_pstPoolListHeader )
		{
			//	初始化 CyclePool Header 信息
			//	Free 指针指向 0 索引处
			m_pstPoolListHeader->hFileMapHeader	= NULL;
			m_pstPoolListHeader->lnRefCount		= 0;
			m_pstPoolListHeader->uFreePos		= 0;
			InitializeCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

			m_pstPoolList = new STCYCLEPOOL[ m_uPoolDepth ];
			if ( m_pstPoolList )
			{
				m_pstPoolListHeader->hFileMap	= NULL;
				m_bInitSucc = TRUE;
			}
		}

		/*
		//
		//	申请共享内存 :: Header 部分 m_pstPoolListHeader
		//
		lpvBuffer	= NULL;
		hFileMap	= NULL;
		bFileMapCreate	= FALSE;
		if ( _vwfunc_malloc_share_memory( sizeof(STCYCLEPOOLHEADER), CONST_FILEMAP_VW_CYCLEPOOL_HEADER, &lpvBuffer, hFileMap, & bFileMapCreate ) )
		{
			m_pstPoolListHeader = (STCYCLEPOOLHEADER*)lpvBuffer;
		}
		if ( m_pstPoolListHeader )
		{
			//	为新创建的共享内存初始化
			if ( bFileMapCreate )
			{
				//	初始化 CyclePool Header 信息
				//	Free 指针指向 0 索引处
				m_pstPoolListHeader->hFileMapHeader	= hFileMap;
				m_pstPoolListHeader->lnRefCount		= 0;
				m_pstPoolListHeader->uFreePos		= 0;
				InitializeCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );
			}

			//
			//	申请共享内存 :: Data 部分 m_pstPoolList
			//
			lpvBuffer	= NULL;
			hFileMap	= NULL;
			bFileMapCreate	= FALSE;
			if ( _vwfunc_malloc_share_memory( sizeof(STCYCLEPOOL)*m_uPoolDepth, CONST_FILEMAP_VW_CYCLEPOOL, &lpvBuffer, hFileMap, & bFileMapCreate ) )
			{
				m_pstPoolList = (STCYCLEPOOL*)lpvBuffer;
				if ( m_pstPoolList )
				{
					m_pstPoolListHeader->hFileMap	= hFileMap;
					m_bInitSucc = TRUE;
				}
			}
		}*/

		//
		//	初始化结构体信息
		//	注意：初始化成功，并且是第一次创建的时候才需要初始化，否则可能会干掉其他进程正在使用的这块共享内容
		//
		if ( m_bInitSucc )
		{
			//	增加使用进程引用计数，也就是有多少个进程在使用这个共享内存
			m_pstPoolListHeader->lnRefCount ++;

			//	为新创建的共享内存初始化
			if ( bFileMapCreate )
			{
				for ( i = 0; i < m_uPoolDepth; i ++ )
				{
					//	第一，先分配 PoolIdx。一定要先分配再 InitPoolNode 
					m_pstPoolList[ i ].lnCyclePoolIdx	= i;

					//	初始化内存，并标记该节点当前空闲
					InitPoolNode( & m_pstPoolList[ i ] );
				}
			}

			//
			//	最近来访 池的当前指针
			//
			m_uVisitRecentlyCurrPos		= 0;

			//	最近来访 池的长度
			m_uVisitRecentlyPoolDepth	= ( sizeof( m_stVisitRecently ) / sizeof( m_stVisitRecently[0] ) );

			//
			//	创建后台处理线程
			//
			if ( ! _vwfunc_is_debug() )
			{
				_beginthreadex
				(
					NULL,
					0,
					&_threadBgWorker,
					(void*)this,
					0,
					NULL
				);
			}
		}
	}
}

CCyclePool::~CCyclePool()
{
	/*
	if ( m_pstPoolList )
	{
		delete [] m_pstPoolList;
		m_pstPoolList = NULL;
	}
	if ( m_pstPoolListHeader )
	{
		DeleteCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );
		
		delete [] m_pstPoolListHeader;
		m_pstPoolListHeader = NULL;
	}
	*/

	/*
	//	减少使用进程引用计数
	m_pstPoolListHeader->lnRefCount --;

	if ( 0 == m_pstPoolListHeader->lnRefCount )
	{
		if ( m_pstPoolList )
		{
			//delete [] m_pstPoolList;
			//m_pstPoolList = NULL;

			UnmapViewOfFile( m_pstPoolList );

			if ( m_pstPoolListHeader->hFileMap )
			{
				CloseHandle( m_pstPoolListHeader->hFileMap );
			}
		}
		
		//
		//	releases all resources used by a critical section object that is not owned
		//
		if ( m_pstPoolListHeader )
		{
			DeleteCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

			//delete [] m_pstPoolListHeader;
			//m_pstPoolListHeader = NULL;

			UnmapViewOfFile( m_pstPoolListHeader );
			
			if ( m_pstPoolListHeader->hFileMapHeader )
			{
				CloseHandle( m_pstPoolListHeader->hFileMapHeader );
			}
		}
	}
	*/
}


/**
 *	@ Public 
 *	Get IpList pool depth
 */
UINT CCyclePool::GetPoolDepth()
{
	return m_uPoolDepth;
}

/**
 *	@ Public 
 *	是否初始化成功，主要是内存是否分配成功
 */
BOOL CCyclePool::IsInitSucc()
{
	return m_bInitSucc;
}

/**
 *	@ Public 
 *	获取一个节点
 */
BOOL CCyclePool::GetNode( STCYCLEPOOL ** ppstNode )
{
	if ( NULL == ppstNode )
	{
		return FALSE;
	}

	//STCYCLEPOOL * pstRet	= NULL;
	UINT	i;

	//	初始化一个值
	(*ppstNode)	= NULL;

	//
	//	如果循环池初始化失败的话，那么就直接用 new 来搞把
	//
	if ( ! m_bInitSucc )
	{
		try
		{
			(*ppstNode) = new STCYCLEPOOL;
			if ( (*ppstNode) )
			{
				//	初始化并标记为 FREE 节点
				InitPoolNode( (*ppstNode) );

				//	标记为 MEM NEW 节点
				(*ppstNode)->lnCyclePoolFlag = CONST_CYCLEPOOLFLAG_MEMNEW;

				//	..
				return TRUE;
			}
		}
		catch( ... )
		{
		}

		return FALSE;
	}


	//
	//	尝试使用循环池
	//


	//	进入红绿灯区域
	EnterCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );


	//
	//	获取一个节点的内存
	//
	if ( CONST_CYCLEPOOLFLAG_FREE == m_pstPoolList[ m_pstPoolListHeader->uFreePos ].lnCyclePoolFlag )
	{
		(*ppstNode) = & m_pstPoolList[ m_pstPoolListHeader->uFreePos ];
		(*ppstNode)->lnCyclePoolFlag = CONST_CYCLEPOOLFLAG_MEMPOOL;
	}
	else
	{
		for ( i = 0; i < m_uPoolDepth; i ++ )
		{
			if ( CONST_CYCLEPOOLFLAG_FREE == m_pstPoolList[ i ].lnCyclePoolFlag )
			{
				//
				//	好不容易找到一个 FREE 的节点
				//
				(*ppstNode) = & m_pstPoolList[ i ];
				(*ppstNode)->lnCyclePoolFlag = CONST_CYCLEPOOLFLAG_MEMPOOL;

				//	下面会进一步 ++ 操作
				m_pstPoolListHeader->uFreePos = i;
				break;
			}
		}
	}

	//
	//	这是一个循环的池
	//
	//	5 [0-4]
	//	0,1,2,3,4,5
	m_pstPoolListHeader->uFreePos ++;
	if ( m_pstPoolListHeader->uFreePos >= m_uPoolDepth )
	{
		m_pstPoolListHeader->uFreePos = 0;
	}


	//	离开红绿灯区域
	LeaveCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

	//
	//	从循环池中获取节点失败
	//
	if ( NULL == (*ppstNode) )
	{
		try
		{
			(*ppstNode) = new STCYCLEPOOL;
			if ( (*ppstNode) )
			{
				//	初始化并标记为 FREE 节点
				InitPoolNode( (*ppstNode) );

				//	标记为 MEM NEW 节点
				(*ppstNode)->lnCyclePoolFlag = CONST_CYCLEPOOLFLAG_MEMNEW;

				return TRUE;
			}
		}
		catch ( ... )
		{
		}

		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

/**
 *	@ Public
 *	释放一个节点
 */
VOID CCyclePool::FreeNode( STCYCLEPOOL * pstCyclePool )
{
	if ( pstCyclePool )
	{
		if ( m_bInitSucc && 
			CONST_CYCLEPOOLFLAG_MEMPOOL == pstCyclePool->lnCyclePoolFlag )
		{
			//	进入红绿灯区域
			EnterCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

			//
			//	是在池中的内存
			//	标记一下这个内存已经用完了
			//
			pstCyclePool->lnCyclePoolFlag = CONST_CYCLEPOOLFLAG_DIRTYPOOL;

			//	离开红绿灯区域
			LeaveCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

		}
		else if ( CONST_CYCLEPOOLFLAG_MEMNEW == pstCyclePool->lnCyclePoolFlag )
		{
			//
			//	内存是 new 临时申请的			
			//	需要用 delete 销毁
			//
			__try
			{
				delete pstCyclePool;
				pstCyclePool = NULL;
			}
			__except( EXCEPTION_EXECUTE_HANDLER )
			{
			}
		}
	}
}

/**
 *	初始化节点
 */
VOID CCyclePool::InitPoolNode( STCYCLEPOOL * pstPoolNode, LONG lnFlag /*CONST_CYCLEPOOLFLAG_FREE*/ )
{
	if ( pstPoolNode )
	{
		LONG lnIdxOrg = pstPoolNode->lnCyclePoolIdx;
		memset( pstPoolNode, 0, sizeof(STCYCLEPOOL) );

		pstPoolNode->pstCfgData		= NULL;
		pstPoolNode->lnCyclePoolIdx	= lnIdxOrg;	//	保存 POOL IDX 值，暂时还无用
		pstPoolNode->lnCyclePoolFlag	= lnFlag;
		pstPoolNode->lnHaspPoolIdx	= -1;
		pstPoolNode->dwRequestFlag	= REQUEST_VW_OKPASS;
	}
}

/**
 *	后台工作线程
 */
unsigned __stdcall CCyclePool::_threadBgWorker( PVOID arglist )
{
	if ( NULL == arglist )
	{
		return 1;
	}
	
	__try
	{
		//	..
		CCyclePool * pThis = (CCyclePool*)arglist;
		if ( pThis )
		{
			pThis->BgWorkerProc();
		}
		
		_endthreadex( 0 );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
	
	return 0;
}
VOID CCyclePool::BgWorkerProc()
{
	//
	//	这个线程需要处理内存的分配
	//	1，释放已经标记为用完的内存，主要是 memset 一下
	//	2，处理两个指针 m_dwCurrPos 和 m_dwFreePos
	//	3，统计最近访问来源
	//	4，统计排行榜
	//
	/*
	#define CONST_CYCLEPOOLFLAG_FREE	0		//	该节点是 Free 的
	#define CONST_CYCLEPOOLFLAG_POOLM	1		//	该节点是 Pool 中的
	#define CONST_CYCLEPOOLFLAG_NEWM	2		//	该节点是 new 申请来的
	#define CONST_CYCLEPOOLFLAG_DIRTY	3		//	该节点是已经用完了的待处理成 feee 的
	*/
	if ( ! m_bInitSucc )
	{
		return;
	}

	UINT	i;
	//STCYCLEPOOL * pstPoolNode;
	//STVISITRECENTLY * pstVRNode;

	
	while ( m_cThSleepBgWorker.IsContinue() )
	{
		//
		//	为了缓解 CPU 的压力
		//
		//Sleep( 5*1000 );
		m_cThSleepBgWorker.EventSleep( 5*1000 );


		//
		//	统计 当前在线、访问排行、最近来访 的数据
		//
		/*
		for ( i = 0; i < m_uPoolDepth; i ++ )
		{
			pstPoolNode	= & m_pstPoolList[ i ];
			if ( pstPoolNode && NULL != pstPoolNode->stHhInfo.szReferer[ 0 ] )
			{
				//	判断 REFERER 不是站内请求
				if ( 0 != stricmp( pstPoolNode->stHhInfo.szHost, pstPoolNode->stHhInfo.szRefererHost ) )
				{
					//
					//	保存信息到 最近来访 池
					//	m_stVisitRecently 是一个长度为 100 的数组，用来保存最近 100 个访问者
					//
					pstVRNode	= & m_stVisitRecently[ m_uVisitRecentlyCurrPos ];
					if ( pstVRNode &&
						pstVRNode->lnTickCount < pstPoolNode->lnTickCount )
					{
						//	pstPoolNode 指向的内容比 pstVRNode 指向的要新
						//	用新的信息替换原来此位置的信息
						pstVRNode->lnTickCount = GetTickCount();
						GetLocalTime( & pstVRNode->stStart );
						_sntprintf( pstVRNode->szAgent, sizeof(pstVRNode->szAgent)-sizeof(TCHAR), "%s", pstPoolNode->stHhInfo.szAgent );
						_sntprintf( pstVRNode->szReferer, sizeof(pstVRNode->szReferer)-sizeof(TCHAR), "%s", pstPoolNode->stHhInfo.szReferer );
						_sntprintf( pstVRNode->szRemoteAddr, sizeof(pstVRNode->szRemoteAddr)-sizeof(TCHAR), "%s", pstPoolNode->stHhInfo.szRemoteAddr );
						_sntprintf( pstVRNode->szUrl, sizeof(pstVRNode->szUrl)-sizeof(TCHAR), "http://%s%s", pstPoolNode->stHhInfo.szHost, pstPoolNode->stHhInfo.szFullUri );
					}

					//	最近来访 池是个循环池
					m_uVisitRecentlyCurrPos ++;
					if ( m_uVisitRecentlyCurrPos >= m_uVisitRecentlyPoolDepth )
					{
						m_uVisitRecentlyCurrPos = 0;
					}
				}
			}
		}
		*/


		//	进入红绿灯区域
		EnterCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

		//
		//	在 POOL 中的节点，用 memset 处理一下即可
		//
		for ( i = 0; i < m_uPoolDepth; i ++ )
		{
			if ( CONST_CYCLEPOOLFLAG_DIRTYPOOL == m_pstPoolList[ i ].lnCyclePoolFlag )
			{
				//	初始化内存，并标记该节点当前空闲
				InitPoolNode( & m_pstPoolList[ i ] );

				//	修改全局指针
				m_pstPoolListHeader->uFreePos = i;
			}
		}

		//	离开红绿灯区域
		LeaveCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );
	}

}

/**
 *	获取最近 100 个访问者列表的 JSON 缓冲区大概大小
 */
DWORD CCyclePool::GetVisitRecentlyJsonStringLength()
{
	return ( m_uVisitRecentlyPoolDepth * ( sizeof(STVISITRECENTLY) + MAX_PATH ) );
}

/**
 *	获取最近 100 个访问者列表
 */
BOOL CCyclePool::GetVisitRecentlyJsonString( LPTSTR lpszVRListJson, DWORD dwSize )
{
	if ( NULL == lpszVRListJson || 0 == dwSize )
	{
		return FALSE;
	}

	INT i;
	TCHAR szTemp[ sizeof(STVISITRECENTLY) + MAX_PATH ]	= {0};
	STVISITRECENTLY * pstCurr;

	for ( i = 0; i < m_uVisitRecentlyPoolDepth; i ++ )
	{
		pstCurr = & m_stVisitRecently[ i ];
		_sntprintf
		(
			szTemp, sizeof(szTemp)-sizeof(TCHAR),
			"{\"st\":\"%04d-%02d-%02d %02d:%02d:%02d\",\"ag\":\"%s\",\"url\":\"%s\",\"rma\":\"%s\",\"rf\":\"%s\"}",
			pstCurr->stStart.wYear, pstCurr->stStart.wMonth, pstCurr->stStart.wDay,
			pstCurr->stStart.wHour, pstCurr->stStart.wMinute, pstCurr->stStart.wSecond,
			pstCurr->szAgent,
			pstCurr->szUrl,
			pstCurr->szRemoteAddr,
			pstCurr->szReferer
		);
		if ( strlen( lpszVRListJson ) + sizeof(szTemp) < dwSize )
		{
			strcat( lpszVRListJson, szTemp );
			if ( i < ( m_uVisitRecentlyPoolDepth - 1 ) )
			{
				strcat( lpszVRListJson, "," );
			}
		}
		else
		{
			break;
		}
	}


	return TRUE;
}