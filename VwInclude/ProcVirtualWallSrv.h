// ProcVirtualWallSrv.h: interface for the CProcVirtualWallSrv class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __PROCVIRTUALWALLSRV_HEADER__
#define __PROCVIRTUALWALLSRV_HEADER__

#include "VwStruct.h"
#include "DeThreadSleep.h"
#include "Crc32.h"

#include <vector>
using namespace std;


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define PROCVIRTUALWALLSRV_SHARE_MEMORY_NAME	_T("Global\\VirtualWallSrvShareMemory")

//	vector 最大记录数
#define PROCVIRTUALWALLSRV_MAX_VECTOR		50000



/**
 *	class of CProcVirtualWallSrv
 */
class CProcVirtualWallSrv
{
public:
	CProcVirtualWallSrv();
	virtual ~CProcVirtualWallSrv();

	BOOL alloc_share_memory( IN DWORD dwSize, IN LPCTSTR lpcszMemName, OUT VOID ** lppvBuffer, OUT HANDLE * phFileMap, OUT BOOL * lpbCreate );
	BOOL open_share_memory( IN LPCTSTR lpcszMemName, OUT VOID ** lppvBuffer, OUT HANDLE * phFileMap );
	VOID close_share_memory( IN OUT VOID ** lppvBuffer, IN OUT HANDLE * phFileMap );

	BOOL CreateAllThreads();
	VOID EndAllThreads();

	BOOL CreateShareMemory();
	BOOL OpenShareMemory();
	VOID CloseShareMemory();

	VOID ActivityQueue_Lock();
	VOID ActivityQueue_Unlock();

	VOID ActivitySetStatus_Lock();
	VOID ActivitySetStatus_Unlock();

	VOID CoreWorkRecord_Lock();
	VOID CoreWorkRecord_Unlock();

	BOOL ActivityQueue_SetReport( STPACKETACTIVITYQUEUEREPORT * pstActReport );
	LONG ActivityQueue_GetOperateIndex( DWORD dwAppPoolNameCrc32, LONG lnCyclePoolIdx );

	BOOL ActivityQueue_TestReport();

private:
	//	for Activity Queue Reporter
	static unsigned __stdcall _threadActivityQueueListCleanUp( PVOID arglist );
	VOID ActivityQueueListCleanUpProc();
	VOID ActivityQueueListCleanUpWorker();

	static unsigned __stdcall _threadTopUrlListDump( PVOID arglist );
	VOID TopUrlListDumpProc();

	VOID RecordTopList( STPACKETACTIVITYQUEUEREPORTDATA * pstData );
	VOID DumpTopList();

public:
	STVIRTUALWALLSRVSHAREMEMORY * m_lpvShareMemoryBuffer;
	HANDLE m_hShareMemoryFileMap;

private:
	CCrc32 m_cCrc32;
	CDeThreadSleep m_cThSleepActivityQueueListCleanUp;
	CDeThreadSleep m_cThSleepTopUrlListDump;

	vector<STVIRTUALWALLSRVTOPURL> m_vcRequestList;	//	记录所有的请求信息
	vector<STVIRTUALWALLSRVTOPURL> m_vcRefererList;	//	记录所有的来源信息
	SYSTEMTIME m_stLastVectorRecord;		//	最后记录的时间

};


#endif // __PROCVIRTUALWALLSRV_HEADER__