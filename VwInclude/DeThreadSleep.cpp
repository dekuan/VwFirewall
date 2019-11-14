// DeThreadSleep.cpp: implementation of the CDeThreadSleep class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeThreadSleep.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeThreadSleep::CDeThreadSleep()
{
	//	初始化 public 线程句柄
	m_hThread	= NULL;
	m_uThreadId	= 0;

	//	创建 event
	m_hSleepEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

	//	设置等待状态
	SetThreadWait( TRUE );

	//	是否继续执行线程
	m_bIsContinue	= TRUE;
}

CDeThreadSleep::~CDeThreadSleep()
{
	if ( m_hSleepEvent )
	{
		CloseHandle( m_hSleepEvent );
		m_hSleepEvent = NULL;
	}
}

/**
 *	@ public
 *	设置让线程等待
 */
VOID CDeThreadSleep::SetThreadWait( BOOL bWait )
{
	if ( bWait )
	{
		//	sets to nonsignaled.
		ResetEvent( m_hSleepEvent );
	}
	else
	{
		//	sets to signaled.
		SetEvent( m_hSleepEvent );
	}
}

/**
 *	@ public
 *	结束等待
 */
VOID CDeThreadSleep::EndSleep()
{
	//	停止等待
	SetThreadWait( FALSE );

	//	设置结束标志
	m_bIsContinue = FALSE;
}

/**
 *	@ public
 *	Event 等待
 */
BOOL CDeThreadSleep::EventSleep( DWORD dwMilliseconds )
{
	//
	//	dwMilliseconds	- [in] 要 Sleep 的时间，单位毫秒
	//			       如果设置 INFINITE ，则一直等待
	//	RETURN		- TRUE  完整地 Sleep 了 dwMilliseconds 长的时间
	//			  FALSE 可能是因为 m_hSleepEvent 被设置为 signaled 状态，或者其他原因
	//

	if ( WAIT_TIMEOUT == WaitForSingleObject( m_hSleepEvent, dwMilliseconds ) )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/**
 *	@ public
 *	结束线程
 */
BOOL CDeThreadSleep::EndThread( HANDLE * phThread /*= NULL*/ )
{
	BOOL bRet		= FALSE;
	HANDLE * phThisThread	= NULL;


	__try
	{
		if ( phThread )
		{
			phThisThread = phThread;
		}
		else
		{
			phThisThread = &m_hThread;
		}

		if ( phThisThread && (*phThisThread) )
		{
			TerminateThread( (*phThisThread), 0 );
			
			while ( WAIT_TIMEOUT == WaitForSingleObject( (*phThisThread), 1000 ) )
			{
				TerminateThread( (*phThisThread), 0 );
			}

			CloseHandle( (*phThisThread) );
			(*phThisThread) = NULL;

			bRet = TRUE;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return bRet;
}

/**
 *	@ public
 *	查询是否继续线程
 */
BOOL CDeThreadSleep::IsContinue()
{
	return m_bIsContinue;
}

/**
 *	@ public
 *	查询线程是否正在运行
 */
BOOL CDeThreadSleep::IsRunning()
{
	BOOL  bRet;
	DWORD dwCode;

	if ( NULL == m_hThread )
	{
		return FALSE;
	}

	bRet	= FALSE;
	dwCode	= 0;

	//	...
	::GetExitCodeThread( m_hThread, &dwCode );

	if ( STILL_ACTIVE == dwCode )
	{
		bRet = TRUE;
	}

	return bRet;
}



//////////////////////////////////////////////////////////////////////////
//	Private
