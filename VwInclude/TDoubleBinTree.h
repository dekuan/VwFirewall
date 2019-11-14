// TProcCacheBase.h: interface for the CTDoubleBinTree class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __TDOUBLEBINTREE_HEADER__
#define __TDOUBLEBINTREE_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//////////////////////////////////////////////////////////////////////////
//	CTDoubleBinTree

template< class _T >
	class CTDoubleBinTree
{
public:
	CTDoubleBinTree()
	{
		InitializeCriticalSection( & m_oCrSec );

		m_pstTree[ 0 ]	= NULL;
		m_pstTree[ 1 ]	= NULL;
		m_dwCurrent	= 0;
		m_dwCount[ 0 ]	= 0;
		m_dwCount[ 1 ]	= 0;

		m_bCleaning	= FALSE;
	}
	~CTDoubleBinTree()
	{
	}

	VOID Lock()
	{
		EnterCriticalSection( & m_oCrSec );
	}

	VOID Unlock()
	{
		LeaveCriticalSection( & m_oCrSec );
	}

	//	获取树根节点指针
	_T * GetTree()
	{
		return m_pstTree[ m_dwCurrent ];
	}

	//	设置第一个节点的数据
	VOID SetTreeHead( _T * pstNode )
	{
		m_pstTree[ m_dwCurrent ] = pstNode;
	}

	//	插入新节点
	BOOL InsertNode( _T * pstLstNode, _T * pstNewNode, INT nCmp )
	{
		if ( NULL == pstNewNode )
		{
			return FALSE;
		}
		if ( 0 == nCmp )
		{
			return FALSE;
		}

		BOOL bRet	= FALSE;

		if ( NULL == GetTree() )
		{
			//	创建第一个结点
			SetTreeHead( pstNewNode );

			bRet = TRUE;
		}
		else
		{
			//	创建后续结点
			if ( pstLstNode )
			{
				if ( nCmp > 0 )
				{
					pstLstNode->pLeft = pstNewNode;
				}
				else if ( nCmp < 0 )
				{
					pstLstNode->pRight = pstNewNode;
				}

				bRet = TRUE;
			}
		}

		if ( bRet )
		{
			//	增加 树上的元素个数
			IncreaseCount();
		}

		return bRet;
	}

	//	是否正在清理缓存中
	BOOL IsCleaning()
	{
		return m_bCleaning;
	}

	//	获取元素个数
	DWORD GetCount()
	{
		return m_dwCount[ m_dwCurrent ];
	}

	//	增加元素个数
	VOID IncreaseCount( DWORD dwStep = 1 )
	{
		//	树上的元素个数 ++
		m_dwCount[ m_dwCurrent ] += dwStep;
	}

	//
	//	清理树 for ProtRes
	//
	//	清理步骤是:
	//	1，将线下的树指针保存到临时变量中
	//	2，线下树指针设置为 NULL 并且计数器清零
	//	3，将线下树推上线
	//	4，开一个线程，慢慢清理线下树的数据(指针保存在临时变量 pstTreeOff 中 )
	BOOL CleanTree()
	{
		//
		//	正在清理
		//
		m_bCleaning	= TRUE;

		DWORD dwOffTreeIndex	= GetOfflineTreeIndex( m_dwCurrent );
		_T * pstTreeOff		= m_pstTree[ dwOffTreeIndex ];

		//
		//	先将线下的树头指针赋为 NULL，计数器清零
		//
		m_pstTree[ dwOffTreeIndex ]	= NULL;
		m_dwCount[ dwOffTreeIndex ]	= 0;

		//
		//	将离线树推上线
		//
		m_dwCurrent	= dwOffTreeIndex;

		//
		//	开启一个线程，慢慢清理线下树曾经使用的内存
		//
		_beginthread
		(
			_threadCleanTree,
			0,
			(VOID*)pstTreeOff
		);
		
		//
		//	清理完毕
		//
		m_bCleaning	= FALSE;

		return TRUE;
	}

private:

	DWORD GetOnlineTreeIndex( DWORD dwIndex )
	{
		return dwIndex;
	}

	DWORD GetOfflineTreeIndex( DWORD dwIndex )
	{
		return ( ( dwIndex + 1 ) % 2 );
	}

	static VOID _threadCleanTree( PVOID arglist )
	{
		if ( NULL == arglist )
		{
			return;
		}

		__try
		{
			_T * pstTree = (_T*)arglist;
			if ( pstTree )
			{
				_threadCleanTree( pstTree->pLeft );
				_threadCleanTree( pstTree->pRight );
				
				delete pstTree;
				pstTree = NULL;
			}
		}
		__except( EXCEPTION_EXECUTE_HANDLER )
		{
		}
	}

public:

private:
	CRITICAL_SECTION m_oCrSec;
	_T * m_pstTree[ 2 ];
	DWORD m_dwCurrent;
	DWORD m_dwCount[ 2 ];
	BOOL  m_bCleaning;
};





#endif // __TPROCCACHEBASE_HEADER__
