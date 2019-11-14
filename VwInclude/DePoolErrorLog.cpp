// DePoolErrorLog.cpp: implementation of the DePoolErrorLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DePoolErrorLog.h"
#include "process.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDePoolErrorLog::CDePoolErrorLog()
{
	InitializeCriticalSection( & m_oCrSecPool );

	//
	//	删除队列中的所有元素
	//
	m_arrDataPool.RemoveAll();


	//
	//	开启 池监控工作线程
	//
	_beginthreadex
	(
		NULL,
		0,
		&_threadPoolWorker,
		(void*)this,
		0,
		NULL
	);
}

CDePoolErrorLog::~CDePoolErrorLog()
{
	DeleteCriticalSection( & m_oCrSecPool );
}


/**
 *	@ Public
 *	格式化写日志
 */
BOOL CDePoolErrorLog::AsyncWriteErrorLog( DWORD dwErrorCode, LPCTSTR lpszFmt, ... )
{
	BOOL  bRet			= FALSE;
	INT   nSize			= 0;
	STDEPOOLERRORLOGDATA stData;
	va_list args;

	__try
	{
		//	dwErrorCode
		stData.dwErrorCode	= dwErrorCode;

		//	build error desc
		va_start( args, lpszFmt );
		nSize = _vsnprintf( stData.szErrorDesc, sizeof(stData.szErrorDesc)-sizeof(TCHAR), lpszFmt, args );
		va_end( args );

		//
		//	进入红灯区
		//
		EnterCriticalSection( & m_oCrSecPool );

		//
		//	将数据保存到队列
		//
		bRet = m_arrDataPool.Add( stData );

		//
		//	离开红灯区
		//
		LeaveCriticalSection( & m_oCrSecPool );

	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		bRet = FALSE;
	}

	return bRet;
}



/**
 *	文件变化监控
 */
unsigned __stdcall CDePoolErrorLog::_threadPoolWorker( PVOID arglist )
{
	if ( NULL == arglist )
	{
		return 1;
	}

	__try
	{
		CDePoolErrorLog * pThis = (CDePoolErrorLog*)(arglist);
		if ( pThis )
		{
			pThis->PoolWorkerProc();
		}

		_endthreadex( 0 );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
	
	return 0;

}
VOID CDePoolErrorLog::PoolWorkerProc()
{
	INT i;

	while( TRUE )
	{
		//
		//	进入红灯区
		//
		EnterCriticalSection( & m_oCrSecPool );


		//
		//	循环队列，取出日志并开始写入磁盘
		//
		for ( i = 0; i < m_arrDataPool.GetSize(); i++ )
		{
			CDeErrorLog::WriteErrorLog( m_arrDataPool[ i ].dwErrorCode, m_arrDataPool[ i ].szErrorDesc );
		}

		//
		//	删除队列中的所有元素
		//
		m_arrDataPool.RemoveAll();


		//
		//	离开红灯区
		//
		LeaveCriticalSection( & m_oCrSecPool );


		//
		//	休息一会儿
		//
		Sleep( 10*1000 );
	}
}