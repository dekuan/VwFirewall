/*
 ************************************************************
 *
 *	TCyclePool.h
 *
 *	CLASS NAME:
 *		Desafe Ascii Code Hash Tree
 *		提供内存池模板类
 *		* 支持后台线程自动释放
 *
 *	AUTHOR:
 *		刘其星 liuqixing@gmail.com
 *
 *	HISTORY:
 *		2008-10-15 刘其星 liuqixing@gmail.com 完成
 *
 ************************************************************
 */

#ifndef __TCYCLEPOOL_HEADER__
#define __TCYCLEPOOL_HEADER__


#include "process.h"
#include "DeThreadSleep.h"


#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )



/**
 *	const define
 */


/**
 *	定义 CyclePool 标记常量
 */
#define CTCYCLEPOOL_MAX_POOL_DEPTH	2000		//	池的默认深度

#define CTCYCLEPOOL_POOLFLAG_FREE	0		//	该节点是 Free 的
#define CTCYCLEPOOL_POOLFLAG_MEMPOOL	1		//	该节点是 Pool 中的
#define CTCYCLEPOOL_POOLFLAG_MEMNEW	2		//	该节点是 new 申请来的
#define CTCYCLEPOOL_POOLFLAG_DIRTYPOOL	3		//	该节点是已经用完了，POOL 类型待处理成 feee 的


/**
 *	结构体定义
 */
typedef struct tagTCyclePool
{
	//
	//	这个结构体不会有外部的 memset 之类的初始化动作
	//
	tagTCyclePool()
	{
	}

	DWORD	dwFreeNodeCallTick;	//	调用 FreeNode 的 Tick
	LONG	lnPoolIdx;		//	循环池中的 index 值
	LONG	lnPoolFlag;		//	是否是因为循环池满而临时 new 出来的，如果是的话，用完要 free

	LPVOID	lpvData;		//	数据内存指针

	tagTCyclePool * pNext;		//	指向下一个节点，从而构成一个环状
	BOOL	bLastNode;		//	是否是最后一个节点

}STTCYCLEPOOL, *LPSTTCYCLEPOOL;



//////////////////////////////////////////////////////////////////////////
//	CTCyclePool

template< class _T, UINT t_uPoolDepth = CTCYCLEPOOL_MAX_POOL_DEPTH >
	class CTCyclePool
{
public:
	CTCyclePool()
	{
		InitializeCriticalSection( & m_oCrSec );
		m_cThSleepBgWorker.m_hThread	= NULL;

		//
		//	init
		//
		m_uPoolDepth	= 0;
		m_bInitSucc	= InitTemplate( t_uPoolDepth );

		if ( m_bInitSucc && m_cThSleepBgWorker.m_hThread )
		{
			ResumeThread( m_cThSleepBgWorker.m_hThread );
		}
	}
	~CTCyclePool()
	{
		DeleteCriticalSection( & m_oCrSec );

		//
		//	等待线程结束
		//
		StopBgWorkerThread();
	}

	VOID Lock()
	{
		EnterCriticalSection( & m_oCrSec );
	}
	VOID Unlock()
	{
		LeaveCriticalSection( & m_oCrSec );
	}

	//	Get IpList pool depth
	UINT GetPoolDepth()
	{
		return m_uPoolDepth;
	}

	//	是否初始化成功
	BOOL IsInitSucc()
	{
		return m_bInitSucc;
	}

	//	从系统中申请新内存
	BOOL GetNewNodeFromOS( STTCYCLEPOOL ** ppstNode )
	{
		if ( NULL == ppstNode )
		{
			return FALSE;
		}

		try
		{
			(*ppstNode) = new STTCYCLEPOOL;
			if ( (*ppstNode) )
			{
				(*ppstNode)->lpvData	= new _T;
				if ( (*ppstNode)->lpvData )
				{
					//	初始化并标记为 FREE 节点
					InitPoolNode( (_T*)( (*ppstNode)->lpvData ) );

					//	标记为 MEM NEW 节点
					(*ppstNode)->lnPoolFlag = CTCYCLEPOOL_POOLFLAG_MEMNEW;

					//	..
					return TRUE;
				}
			}
		}
		catch( ... )
		{
		}
		
		return FALSE;
	}

	BOOL GetNodeByPoolIdx( LONG lnPoolIdx, STTCYCLEPOOL ** ppstNode )
	{
		if ( lnPoolIdx < 0 || lnPoolIdx > m_uPoolDepth )
		{
			return FALSE;
		}
		if ( NULL == ppstNode )
		{
			return FALSE;
		}

		BOOL bRet;

		//	...
		bRet = FALSE;

		if ( m_pstPoolList )
		{
			bRet = TRUE;
			(*ppstNode) = & ( m_pstPoolList[ lnPoolIdx ] );
		}

		return bRet;
	}

	//	获取一个节点
	BOOL GetNode( STTCYCLEPOOL ** ppstNode )
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
			return GetNewNodeFromOS( ppstNode );
		}


		//
		//	尝试使用循环池
		//


		//	进入红绿灯区域
		Lock();


		//
		//	获取一个节点的内存
		//
		if ( CTCYCLEPOOL_POOLFLAG_FREE == m_pstPoolListFreeNode->lnPoolFlag )
		{
			(*ppstNode) = m_pstPoolListFreeNode;
			(*ppstNode)->lnPoolFlag = CTCYCLEPOOL_POOLFLAG_MEMPOOL;
		}
		else
		{
			//
			//	从当前 FreeNode 节点开始转一圈
			//
			for ( i = 0; i < m_uPoolDepth; i ++ )
			{
				m_pstPoolListFreeNode	= m_pstPoolListFreeNode->pNext;

				if ( CTCYCLEPOOL_POOLFLAG_FREE == m_pstPoolListFreeNode->lnPoolFlag )
				{
					//
					//	好不容易找到一个 FREE 的节点
					//
					(*ppstNode) = m_pstPoolListFreeNode;
					(*ppstNode)->lnPoolFlag = CTCYCLEPOOL_POOLFLAG_MEMPOOL;

					//	下面会进一步 ++ 操作
					m_pstPoolListFreeNode	= m_pstPoolListFreeNode->pNext;
					break;
				}
			}
		}

		//	离开红绿灯区域
		Unlock();

		//
		//	从循环池中获取节点失败
		//
		if ( NULL == (*ppstNode) )
		{
			return GetNewNodeFromOS( ppstNode );
		}
		else
		{
			return TRUE;
		}
	}
	
	//	释放一个节点
	VOID FreeNode( STTCYCLEPOOL * pstNode )
	{
		if ( pstNode )
		{
			if ( m_bInitSucc && CTCYCLEPOOL_POOLFLAG_MEMPOOL == pstNode->lnPoolFlag )
			{
				//	进入红绿灯区域
				Lock();

				//
				//	是在池中的内存
				//	标记一下这个内存已经用完了
				//
				pstNode->dwFreeNodeCallTick = GetTickCount();
				pstNode->lnPoolFlag = CTCYCLEPOOL_POOLFLAG_DIRTYPOOL;

				//	离开红绿灯区域
				Unlock();
			}
			else if ( CTCYCLEPOOL_POOLFLAG_MEMNEW == pstNode->lnPoolFlag )
			{
				//
				//	内存是 new 临时申请的			
				//	需要用 delete 销毁
				//
				__try
				{
					delete pstNode->lpvData;
					pstNode->lpvData = NULL;

					delete pstNode;
					pstNode = NULL;
				}
				__except( EXCEPTION_EXECUTE_HANDLER )
				{
				}
			}
		}
	}


private:

	//
	//	@ private
	//	初始化模板
	//
	BOOL InitTemplate( UINT uPoolDepth )
	{
		if ( uPoolDepth < 3 )
		{
			return FALSE;
		}

		BOOL bRet	= FALSE;
		UINT i;

		//
		//
		//
		bRet			= FALSE;
		m_ptDataList		= NULL;
		m_pstPoolList		= NULL;

		//
		//	指定当前在线用户池的深度
		//	默认是 2000
		//
		m_uPoolDepth	= uPoolDepth;

		//
		//	先申请数据的内存
		//
		try
		{
			m_ptDataList = new _T[ m_uPoolDepth ];
			if ( m_ptDataList )
			{
				//	初始化内存
				memset( m_ptDataList, 0, m_uPoolDepth*sizeof(_T) );

				//
				//	再申请对应数量的管理节点
				//
				m_pstPoolList = new STTCYCLEPOOL[ m_uPoolDepth ];
				if ( m_pstPoolList )
				{
					bRet	= TRUE;

					//	默认将 free node 指向第一个节点
					m_pstPoolListFreeNode	= m_pstPoolList;


					//	初始化内存
					memset( m_pstPoolList, 0, m_uPoolDepth*sizeof(STTCYCLEPOOL) );

					//
					//	将数据节点 link 到管理节点
					//
					for ( i = 0; i < m_uPoolDepth; i ++ )
					{
						//	数据节点连接到管理节点上
						m_pstPoolList[ i ].lpvData	= &m_ptDataList[ i ];
						m_pstPoolList[ i ].lnPoolIdx	= i;

						//	构成环
						if ( i < ( m_uPoolDepth - 1 ) )
						{
							//	直接指向下一个管理
							m_pstPoolList[ i ].bLastNode	= FALSE;
							m_pstPoolList[ i ].pNext	= & m_pstPoolList[ i + 1 ];
						}
						else
						{
							//	将最后一个节点的 next 连接到第一个，从而构成闭合的环
							m_pstPoolList[ i ].bLastNode	= TRUE;
							m_pstPoolList[ i ].pNext	= & m_pstPoolList[ 0 ];
						}
					}

					//
					//	创建后台处理线程
					//
					//if ( ! delib_is_debug() )
					{
						CreateBgWorkerThread();
					}
				}
			}
		}
		catch ( ... )
		{
		}

		return bRet;
	}

	VOID InitPoolNode( _T * pstPoolNode, LONG lnFlag = CTCYCLEPOOL_POOLFLAG_FREE )
	{
		if ( pstPoolNode )
		{
			memset( pstPoolNode, 0, sizeof(_T) );
		}
	}

	//
	//	创建后台工作线程
	//
	VOID CreateBgWorkerThread()
	{
		m_cThSleepBgWorker.m_hThread = (HANDLE)_beginthreadex
			(
				NULL,
				0,
				&_threadBgWorker,
				(void*)this,
				CREATE_SUSPENDED,
				&m_cThSleepBgWorker.m_uThreadId
			);
	}

	//
	//	停止线程
	//
	VOID StopBgWorkerThread()
	{
		__try
		{
			m_cThSleepBgWorker.EndSleep();
			m_cThSleepBgWorker.EndThread( &m_cThSleepBgWorker.m_hThread );
		}
		__except( EXCEPTION_EXECUTE_HANDLER )
		{
		}
	}

	//
	//	后台工作线程
	//
	static unsigned __stdcall _threadBgWorker( PVOID arglist )
	{
		if ( NULL == arglist )
		{
			return 1;
		}
		
		__try
		{
			CTCyclePool * pThis = (CTCyclePool*)arglist;
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
	VOID BgWorkerProc()
	{
		//
		//	这个线程需要处理内存的分配
		//	1，释放已经标记为用完的内存，主要是 memset 一下，并设置 lnPoolFlag = CTCYCLEPOOL_POOLFLAG_FREE
		//	2，处理两个指针 m_dwCurrPos 和 m_dwFreePos
		//

		STTCYCLEPOOL * pstNode;
		STMYHTTPFILTERCONTEXT * pMyHfc;

		while ( m_cThSleepBgWorker.IsContinue() )
		{
			//
			//	为了缓解 CPU 的压力
			//
			m_cThSleepBgWorker.EventSleep( 5*1000 );

			if ( ! m_bInitSucc )
			{
				continue;
			}

			//	...
			Lock();

			//
			//	在 POOL 中的节点，用 memset 处理一下即可
			//
			pstNode	= m_pstPoolList;
			while( pstNode )
			{
				//
				//	将用完的内存初始化一下
				//
				if ( CTCYCLEPOOL_POOLFLAG_DIRTYPOOL == pstNode->lnPoolFlag )
				{
					pMyHfc = (STMYHTTPFILTERCONTEXT*)pstNode->lpvData;
					if ( pMyHfc->bSentLastReport ||
						GetTickCount() - pMyHfc->stActionTime.dwTick > 3000 )
					{
						//
						//	已经发送报告，或者超时 5 秒的统统清理
						//

						//TCHAR szTemp[ MAX_PATH ];
						//_sntprintf( szTemp, sizeof(szTemp)/sizeof(TCHAR)-1, "@@@Free Node[%d] memory!", pstNode->lnPoolIdx );
						//OutputDebugString( szTemp );

						//	zero memory
						memset( pstNode->lpvData, 0, sizeof(_T) );

						//	set flag to free
						pstNode->lnPoolFlag = CTCYCLEPOOL_POOLFLAG_FREE;
					}
				}

				//
				//	如果是最后一个节点，则直接跳出循环
				//
				if ( pstNode->bLastNode )
				{
					break;
				}

				pstNode = pstNode->pNext;
			}

			//	...
			Unlock();
		}
	}


public:
	//	数据池的指针
	_T * m_ptDataList;

	//	管理池的指针
	STTCYCLEPOOL * m_pstPoolList;
	STTCYCLEPOOL * m_pstPoolListFreeNode;

	//	处理线程等待
	CDeThreadSleep	m_cThSleepBgWorker;

private:
	//	循环池的长度
	UINT	m_uPoolDepth;

	//	标识是否初始化成功
	BOOL	m_bInitSucc;

	UINT m_uFreePos;		//	FreePos index of pool
	CRITICAL_SECTION m_oCrSec;
};







#endif	// __TCYCLEPOOL_HEADER__


