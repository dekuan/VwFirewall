#include "StdAfx.h"
#include "HashPool.h"




CHashPool::CHashPool( unsigned int lnPoolDepth /* CONST_MAX_HASHPOOL_DEPTH*3 */ )
{
	BOOL bFileMapCreate	= FALSE;
	LPVOID lpvBuffer	= NULL;
	HANDLE hFileMap		= NULL;
	UINT i;


	//
	//	Is init successfully
	//
	m_bInitSucc		= FALSE;
	m_dwThreadRecordTimeout	= 5*60*1000;	//	线程计数超时 5*60 秒
	m_pstPoolListHeader	= NULL;
	m_pstPoolList		= NULL;

	//
	//	Special the depth of the pool by user
	//
	m_uPoolDepth	= lnPoolDepth;

	if ( m_uPoolDepth > 0 )
	{
		//	进程内分配内存的方式
		bFileMapCreate	= TRUE;
		m_pstPoolListHeader = new STHASHPOOLLISTHEADER;
		if ( m_pstPoolListHeader )
		{
			//	初始化 CyclePool Header 信息
			//	Free 指针指向 0 索引处
			m_pstPoolListHeader->hFileMapHeader	= NULL;
			m_pstPoolListHeader->uRefCount		= 0;
			InitializeCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

			m_pstPoolList = new STHASHPOOLLIST[ m_uPoolDepth ];
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
		if ( _vwfunc_malloc_share_memory( sizeof(STHASHPOOLLISTHEADER), CONST_FILEMAP_VW_HASHPOOL_HEADER, &lpvBuffer, hFileMap, & bFileMapCreate ) )
		{
			m_pstPoolListHeader = (STHASHPOOLLISTHEADER*)lpvBuffer;
		}
		if ( m_pstPoolListHeader )
		{
			//	为新创建的共享内存初始化
			if ( bFileMapCreate )
			{
				m_pstPoolListHeader->hFileMapHeader	= hFileMap;
				m_pstPoolListHeader->uRefCount		= 0;
				InitializeCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );
			}
		
			//
			//	申请共享内存 :: m_pstPoolList
			//
			lpvBuffer	= NULL;
			hFileMap	= NULL;
			bFileMapCreate	= FALSE;
			if ( _vwfunc_malloc_share_memory( sizeof(STHASHPOOLLIST)*m_uPoolDepth, CONST_FILEMAP_VW_HASHPOOL, &lpvBuffer, hFileMap, & bFileMapCreate ) )
			{
				m_pstPoolList = (STHASHPOOLLIST*)lpvBuffer;
				if ( m_pstPoolList )
				{
					m_pstPoolListHeader->hFileMap	= hFileMap;
					m_bInitSucc = TRUE;
				}
			}
		}
		*/

		//
		//	初始化结构体信息
		//	注意：初始化成功，并且是第一次创建的时候才需要初始化，否则可能会干掉其他进程正在使用的这块共享内容
		//
		if ( m_bInitSucc )
		{
			//	增加使用进程引用计数，也就是有多少个进程在使用这个共享内存
			m_pstPoolListHeader->uRefCount ++;

			//	为第一次创建出来的共享内存初始化
			if ( bFileMapCreate )
			{
				for ( i = 0; i < m_uPoolDepth; i ++ )
				{
					//	初始化节点
					InitPoolNode( & m_pstPoolList[ i ] );
				}
			}

			//
			//	开启防攻击后台线程
			//	注意：
			//		2007-10-17 By LQX :: 3.0.9.1048 版本以后就暂时放弃该功能，所以无需启动该线程
			//
			//if ( ! _vwfunc_is_debug() )
			//{
			//	_beginthread( _threadAntiAttackBgWorker, 0, (void*)this );
			//}
		}
	}
}

CHashPool::~CHashPool()
{
	//	因为是共享内存，所以不用释放
/*
	//	减少使用进程引用计数
	m_pstPoolListHeader->uRefCount --;

	if ( 0 == m_pstPoolListHeader->uRefCount )
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
UINT CHashPool::GetPoolDepth()
{
	return m_uPoolDepth;
}

/**
 *	@ Public 
 *	是否初始化成功，主要是内存是否分配成功
 */
BOOL CHashPool::IsInitSucc()
{
	return m_bInitSucc;
}


/**
 *	@ Public 
 *	[string hash]
 *	Get hashed pool index	
 */
LONG CHashPool::GetHashPoolIndex( const DWORD dwIpAddr )
{
	//
	//	dwIpAddr	- [in] ip address
	//
	//	RETURN
	//			- LONG type hash value if successfully
	//			  otherwise -1
	//

	if ( ! m_bInitSucc )
	{
		return -1;
	}

	return ( dwIpAddr % ( m_uPoolDepth + 1 ) );
}

/**
 *	@ Public 
 *	Get special item count	
 */
LONG CHashPool::GetCount( LONG lnIndex )
{
	//
	//	lnIndex		- [in] hash pool index
	//
	//	RETURN		-
	//			  the count of special pool indexed by uIndex if successfulyy
	//			  otherwise -1
	//

	if ( ! m_bInitSucc )
	{
		return -1;
	}
	if ( ! IsInIndexRange( lnIndex ) )
	{
		return -1;
	}

	if ( m_pstPoolList[ lnIndex ].dwQueryTick > 0 &&
		GetTickCount() - m_pstPoolList[ lnIndex ].dwQueryTick > m_dwThreadRecordTimeout )
	{
		//
		//	超时了，将计数器设置为0
		//
		EnterCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );
		m_pstPoolList[ lnIndex ].usThreadCount = 0;
		LeaveCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

		return 0;
	}

	//	记录 QueryTick
	EnterCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );
	m_pstPoolList[ lnIndex ].dwQueryTick = GetTickCount();
	LeaveCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

	//	..
	return m_pstPoolList[ lnIndex ].usThreadCount;
}

/**
 *	@ Public 
 *	Increase the count and Save the data
 */
BOOL CHashPool::IncCount( LONG lnIndex, const DWORD dwIpAddr )
{
	//
	//	lnIndex		- [in] hash pool index
	//	dwIpAddr	- [in] Ip Address
	//
	//	RETURN		- TRUE/FALSE
	//
	
	if ( ! m_bInitSucc )
	{
		return FALSE;
	}
	if ( ! IsInIndexRange( lnIndex ) )
	{
		return FALSE;
	}

	if ( 0 == m_pstPoolList[ lnIndex ].usThreadCount )
	{
		//	新的项

		EnterCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

		m_pstPoolList[ lnIndex ].dwIpAddr	= dwIpAddr;
		m_pstPoolList[ lnIndex ].usThreadCount	= 1;
		m_pstPoolList[ lnIndex ].dwQueryTick	= GetTickCount();

		LeaveCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

		return TRUE;
	}
	else if ( dwIpAddr == m_pstPoolList[ lnIndex ].dwIpAddr )
	{
		//	已经存在的项

		EnterCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

		m_pstPoolList[ lnIndex ].usThreadCount ++;
		m_pstPoolList[ lnIndex ].dwQueryTick = GetTickCount();

		LeaveCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

		return TRUE;
	}
	else
	{
		//	冲突产生，解决之
		//	..

		return TRUE;
	}
}

/**
 *	@ Public 
 *	Decrease the count
 */
BOOL CHashPool::DecCount( LONG lnIndex, const DWORD dwIpAddr )
{
	//
	//	uIndex		- [in] hash pool index
	//	dwIpAddr	- [in] Ip Address 
	//
	//	RETURN		-
	//			  the count of special pool indexed by uIndex
	//

	if ( ! m_bInitSucc )
	{
		return FALSE;
	}
	if ( ! IsInIndexRange( lnIndex ) )
	{
		return FALSE;
	}

	BOOL bRet	= FALSE;

	if ( dwIpAddr == m_pstPoolList[ lnIndex ].dwIpAddr )
	{
		//	查找到的项
		if ( m_pstPoolList[ lnIndex ].usThreadCount > 0 )
		{
			bRet = TRUE;

			EnterCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

			m_pstPoolList[ lnIndex ].usThreadCount --;
			m_pstPoolList[ lnIndex ].dwQueryTick = GetTickCount();

			LeaveCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );
		}
	}
	else
	{
		//	可能是冲突的项，检查二级缓冲，解决之
		//	..

	}

	return bRet;
}


/**
 *	@ Public 
 *	Decrease the count
 */
DWORD CHashPool::GetIpAddrFromString( LPCSTR lpcszIpAddr )
{
	if ( NULL == lpcszIpAddr || 0 == lpcszIpAddr[ 0 ] )
		return 0;

	//	INADDR_NONE 如果不是一个合法的 IP 地址
	return inet_addr( lpcszIpAddr );
}






/**
 *	@ Public 
 *	日保护文件访问次数限制 :: 获取计数	
 */
LONG CHashPool::LmtDayIpGetCount( LONG lnIndex )
{
	//
	//	lnIndex		- [in] hash pool index
	//
	//	RETURN		-
	//			  the count of special pool indexed by uIndex if successfulyy
	//			  otherwise -1
	//
	
	if ( ! m_bInitSucc )
	{
		return -1;
	}
	if ( ! IsInIndexRange( lnIndex ) )
	{
		return -1;
	}
	
	//	..
	return m_pstPoolList[ lnIndex ].usLmtDayIpTimes;
}

/**
 *	@ Public 
 *	日保护文件访问次数限制 :: 保存数据并增加计数	
 */
BOOL CHashPool::LmtDayIpIncCount( LONG lnIndex, const DWORD dwIpAddr )
{
	//
	//	lnIndex		- [in] hash pool index
	//	dwIpAddr	- [in] Ip Address
	//
	//	RETURN		- TRUE/FALSE
	//
	
	if ( ! m_bInitSucc )
	{
		return FALSE;
	}
	if ( ! IsInIndexRange( lnIndex ) )
	{
		return FALSE;
	}

	SYSTEMTIME st;
	USHORT usLastDate;
	
	GetLocalTime( & st );
	usLastDate = ( st.wMonth * 100 + st.wDay );

	if ( 0 == m_pstPoolList[ lnIndex ].usLmtDayIpTimes )
	{
		//	新的项

		EnterCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

		m_pstPoolList[ lnIndex ].dwIpAddr		= dwIpAddr;
		m_pstPoolList[ lnIndex ].usLmtDayIpLastDate	= usLastDate;
		m_pstPoolList[ lnIndex ].usLmtDayIpTimes	= 1;

		LeaveCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

		return TRUE;
	}
	else if ( dwIpAddr == m_pstPoolList[ lnIndex ].dwIpAddr )
	{
		//	已经存在的项

		EnterCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

		if ( usLastDate != m_pstPoolList[ lnIndex ].usLmtDayIpLastDate )
		{
			//	又过了一天
			m_pstPoolList[ lnIndex ].usLmtDayIpLastDate	= usLastDate;
			m_pstPoolList[ lnIndex ].usLmtDayIpTimes	= 1;
		}
		else
		{
			m_pstPoolList[ lnIndex ].usLmtDayIpTimes ++;
		}

		LeaveCriticalSection( & m_pstPoolListHeader->oCriticalSectionModify );

		return TRUE;
	}
	else
	{
		//	冲突产生，解决之
		//	..

		return TRUE;
	}
}






/**
 *	@ Public 
 *	Get special item count	
 */
LONG CHashPool::AntiAttackGetCount( LONG lnIndex )
{
	//
	//	lnIndex		- [in] hash pool index
	//
	//	RETURN		-
	//			  the count of special pool indexed by uIndex if successfulyy
	//			  otherwise -1
	//

	if ( ! m_bInitSucc )
	{
		return -1;
	}
	if ( ! IsInIndexRange( lnIndex ) )
	{
		return -1;
	}

	//	..
	return m_pstPoolList[ lnIndex ].usVisitCount;
}

/**
 *	@ Public 
 *	Increase the count and Save the data
 */
BOOL CHashPool::AntiAttackIncCount( LONG lnIndex, const DWORD dwIpAddr )
{
	//
	//	lnIndex		- [in] hash pool index
	//	dwIpAddr	- [in] Ip Address
	//
	//	RETURN		- TRUE/FALSE
	//
	
	if ( ! m_bInitSucc )
	{
		return FALSE;
	}
	if ( ! IsInIndexRange( lnIndex ) )
	{
		return FALSE;
	}

	if ( 0 == m_pstPoolList[ lnIndex ].usVisitCount )
	{
		//	新的项
		if ( CONST_IPADDR_NULL == m_pstPoolList[ lnIndex ].dwIpAddr )
		{
			m_pstPoolList[ lnIndex ].dwIpAddr = dwIpAddr;
		}
		m_pstPoolList[ lnIndex ].usVisitCount = 1;

		return TRUE;
	}
	else if ( dwIpAddr == m_pstPoolList[ lnIndex ].dwIpAddr )
	{
		//	已经存在的项
		m_pstPoolList[ lnIndex ].usVisitCount ++;

		return TRUE;
	}
	else
	{
		//	冲突产生，解决之
		//	..

		return TRUE;
	}
}

/**
 *	获取 被限制客户端醒来的时间点
 */
LONG CHashPool::AntiAttackGetWakeUpTime( LONG lnIndex )
{
	//
	//	lnIndex		- [in] hash pool index
	//	RETURN		- 返回受限制客户端醒来的时间点
	//			  如果这个时间点不为 0 的话那么就认为该客户端是被限制的
	//			  otherwise -1
	//

	if ( ! m_bInitSucc )
	{
		return -1;
	}
	if ( ! IsInIndexRange( lnIndex ) )
	{
		return -1;
	}

	//	..
	return m_pstPoolList[ lnIndex ].dwWakeUpTime;
}

/**
 *	设置 被限制客户端醒来的时间点
 */
VOID CHashPool::AntiAttackSetWakeUpTime( LONG lnIndex, LONG lnWakeUpTime )
{
	//
	//	lnIndex		- [in] hash pool index
	//	lnWakeUpTime	- [in] 指定被限制客户端醒来的时间点，一般是 GetTickCount() + X毫秒
	//	RETURN		- VOID
	//

	if ( ! m_bInitSucc )
	{
		return;
	}
	if ( ! IsInIndexRange( lnIndex ) )
	{
		return;
	}

	//	..
	m_pstPoolList[ lnIndex ].dwWakeUpTime = lnWakeUpTime;
}

/**
 *	防攻击后台工作线程
 */
VOID CHashPool::_threadAntiAttackBgWorker( PVOID pvArg )
{
	if ( NULL == pvArg )
		return;

	//	..
	CHashPool * pThis = (CHashPool*)pvArg;
	if ( pThis )
	{
		pThis->AntiAttackBgWorkerProc();
	}
}
VOID CHashPool::AntiAttackBgWorkerProc()
{
	//
	//	实际测试清理整个池的 usVisitCount = 0; 大概需要 0.031 ~ 0.062 秒
	//

	UINT	i;
	
	while ( TRUE )
	{
		//
		//	1 分钟做一次
		//
		Sleep( 60*1000 );

		//
		//	释放所有一分钟内的计时为 0
		//	为了速度和效率暂时不考虑互斥问题
		//
		for ( i = 0; i < m_uPoolDepth; i ++ )
		{
			m_pstPoolList[ i ].usVisitCount	= 0;
		}

		if ( _vwfunc_is_debug() )
		{
			break;
		}
	}
}





////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////





/**
 *	@ Public 
 *	Check if the index of hash pool is in range	
 */
BOOL CHashPool::IsInIndexRange( UINT uIndex )
{
	//
	//	uIndex		- [in] hash pool index
	//	RETURN		- TRUE/FALSE

	if ( uIndex >= 0 && uIndex < m_uPoolDepth )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}



/**
 *	@ Private
 *	初始化节点
 */
VOID CHashPool::InitPoolNode( STHASHPOOLLIST * pstPoolNode )
{
	if ( pstPoolNode )
	{
		pstPoolNode->dwQueryTick	= 0;
		pstPoolNode->dwIpAddr		= CONST_IPADDR_NULL;
		pstPoolNode->usThreadCount	= 0;
		pstPoolNode->usVisitCount	= 0;
		pstPoolNode->dwWakeUpTime	= 0;
	}
}



/**
 *	@ Private 暂时未使用的函数
 *	[string hash]
 *	32 version for zero terminated string
 */
UINT CHashPool::GetStringHashValue32( const char * s, UINT lnInit /*CONST_FNV_32_PRIME*/ )
{
	//
	//	s	- [in] pointer to a null-terminated string 
	//	lnInit	- [in] init frime value
	//
	//	RETURN
	//		UINT type hash value
	//

	UINT hval;

	for ( hval = lnInit; *s; s++ )
	{
		//	multiply by the 32 bit FNV magic prime mod 2^32
		hval *= CONST_FNV_32_PRIME;

		//	xor the bottom with the current octet
		hval ^= (UINT)*s;
	}

	return hval;
}

/**
 *	@ Private 暂时未使用的函数
 *	[string hash]
 *	32 version for buffer
 */
UINT CHashPool::GetStringHashValue32ByBuffer( const char * buf, UINT len, UINT lnInit /*CONST_FNV_32_PRIME*/ )
{
	//
	//	buf	- [in] pointer to a buffer
	//	len	- [in] length of buffer that need to hash
	//	lnInit	- [in] init frime value
	//
	//	RETURN
	//		UINT type hash value
	//


	//	initial hash value
	UINT hval;

	for ( hval = CONST_FNV_32_PRIME; len > 0; --len )
	{
		hval *= CONST_FNV_32_PRIME;
		hval ^= (UINT)(*buf);
		++ buf;
	}

	return hval;
}





























