// DePoolErrorLog.h: interface for the DePoolErrorLog class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DEPOOLERRORLOG_HEADER__
#define __DEPOOLERRORLOG_HEADER__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "DeErrorLog.h"
#include <atlbase.h>


/**
 *	struct
 */
typedef struct tagDePoolErrorLogData
{
	tagDePoolErrorLogData()
	{
		//memset( this, 0, sizeof(tagDePoolErrorLogData) );
	}
	DWORD dwErrorCode;
	TCHAR szErrorDesc[ MAX_PATH ];

}STDEPOOLERRORLOGDATA, *LPSTDEPOOLERRORLOGDATA;


/**
 *	class of DePoolErrorLog
 */
class CDePoolErrorLog :
	public CDeErrorLog
{
public:
	CDePoolErrorLog();
	virtual ~CDePoolErrorLog();

public:
	BOOL AsyncWriteErrorLog( DWORD dwErrorCode, LPCTSTR lpszFmt, ... );

private:
	static unsigned __stdcall _threadPoolWorker( PVOID arglist );
	VOID PoolWorkerProc();

private:
	CRITICAL_SECTION m_oCrSecPool;
	CSimpleArray<STDEPOOLERRORLOGDATA> m_arrDataPool;
};




#endif // __DEPOOLERRORLOG_HEADER__
