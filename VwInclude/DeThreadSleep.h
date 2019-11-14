// DeThreadSleep.h: interface for the CDeThreadSleep class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DETHREADSLEEP_HEADER__
#define __DETHREADSLEEP_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>



/**
 *	class of CDeThreadSleep
 */
class CDeThreadSleep
{
public:
	CDeThreadSleep();
	virtual ~CDeThreadSleep();

	VOID SetThreadWait( BOOL bWait );
	BOOL EventSleep( DWORD dwMilliseconds );
	VOID EndSleep();
	BOOL EndThread( HANDLE * phThread = NULL );
	BOOL IsContinue();
	BOOL IsRunning();

public:
	HANDLE m_hThread;
	UINT   m_uThreadId;


private:
	HANDLE m_hSleepEvent;
	BOOL   m_bIsContinue;
};




#endif // __DETHREADSLEEP_HEADER__
