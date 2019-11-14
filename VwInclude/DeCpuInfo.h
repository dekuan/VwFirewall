// DeCpuInfo.h: interface for the CDeCpuInfo class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DECPUINFO_HEADER__
#define __DECPUINFO_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#pragma warning(disable:4786)


#include "ntdll.h"
#include "DeThreadSleep.h"

#include <vector>
#include <map>
using namespace std;


static const int PROCBUF_GROWSIZE = 4096;


/**
 *	struct
 */
typedef struct tagDeCpuInfoThread
{
	tagDeCpuInfoThread()
	{
		memset( this, 0, sizeof(tagDeCpuInfoThread) );
	}

	VOID *	pvThis;		//	类实例 this 指针
	HANDLE	hProcess;	//	指定进程的指针
	BOOL	bLive;		//	进程是否还活着
	DWORD	dwCpuUsed;	//	该进程 CPU 使用情况

}STDECPUINFOTHREAD, *LPSTDECPUINFOTHREAD;

//	用于计算指定进程名的 CPU 使用计算
typedef struct tagDeCpuInfoSpecCpuInfo
{
	BOOL     bQueryFirst;
	BOOL     bQuerySecond;

	FILETIME ftIdleTimeStart;
	FILETIME ftIdleTimeEnd;

	FILETIME ftKernelTimeStart;
	FILETIME ftKernelTimeEnd;
	
	FILETIME ftUserTimeStart;
	FILETIME ftUserTimeEnd;
	
}STDECPUINFOSPECCPUINFO, *LPSTDECPUINFOSPECCPUINFO;


/**
 *	class of CDeCpuInfo
 */
class CDeCpuInfo
{
public:
	CDeCpuInfo();
	virtual ~CDeCpuInfo();

	BOOL  SetProcessInfo( LPCTSTR lpcszProcessName, DWORD dwRefreshInterval );
	DWORD GetCpuUsedPercentSpec();
	DWORD GetCpuUsedPercentTotal();

private:
	static unsigned __stdcall _threadQueryCpuInfo( PVOID arglist );
	VOID QueryCpuInfoProc();

	static unsigned __stdcall _threadQueryCpuInfo_StablyVer( PVOID arglist );
	VOID QueryCpuInfo_StablyVerProc();

	static unsigned __stdcall _threadQuerySpecCpuInfo_StablyVer( PVOID arglist );
	VOID QuerySpecCpuInfo_StablyVerProc();

	VOID CalcCpuInfo();
	VOID CalcCpuInfo_StablyVerProc();
	VOID CalcSpecCpuInfo_StablyVerProc( LPCTSTR lpctszAppName );

	HRESULT GetProcessInfo();

private:
	vector<STDECPUINFOTHREAD> m_vcThread;
	vector<STDECPUINFOTHREAD>::iterator m_itThread;

	HANDLE m_hCalcTread;
	CDeThreadSleep	m_cThSleepCalcTread;

	HANDLE m_hCalcTread_StablyVer;
	CDeThreadSleep	m_cThSleepCalcTread_StablyVer;

	HANDLE m_hCalcSpecTread_StablyVer;
	CDeThreadSleep	m_cThSleepCalcSpecTread_StablyVer;

	TCHAR  m_szProcessName[ 128 ];
	DWORD  m_dwProcessNameLength;
	DWORD  m_dwProcessNameLengthInByte;
	DWORD  m_dwRefreshInterval;

	DWORD  m_dwCpuUsedSpec;
	DWORD  m_dwCpuUsedTotal;

	CRITICAL_SECTION m_stCriticalSection;
	PROCNTQSI m_pfnNtQuerySystemInformation;

	LPVOID	m_pvSystemProcessInfoBuffer;		//	Buffer for NtQuerySystemInfo
	DWORD	m_dwSystemProcessInfoBufferSize;	//	Size of the above buffer, in bytes

	ULONGLONG m_ullLastKernelTimeSpec;
	ULONGLONG m_ullLastUserTimeSpec;

	ULONGLONG m_ullLastKernelTimeTotal;
	ULONGLONG m_ullLastUserTimeTotal;
};


#endif // __DECPUINFO_HEADER__
