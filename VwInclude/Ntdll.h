#ifndef _NTDLL_H__
#define _NTDLL_H__

#include <tlhelp32.h>

//Windows NT/2000中获取CUP使用率的方法与Windows 9x系统中所使用的方法不同，
//Windows NT/2000中获取CUP使用率不用"性能计数器"。而是使用一个NTDLL.DLL中
//未公开的API函数--NtQuerySystemInformation
// ntdll!NtQuerySystemInformation (NT specific!)
//
// The function copies the system information of the
// specified type into a buffer
//
// NTSYSAPI
// NTSTATUS
// NTAPI
// NtQuerySystemInformation(
//    IN UINT SystemInformationClass,    // information type
//    OUT PVOID SystemInformation,       // pointer to buffer
//    IN ULONG SystemInformationLength,  // buffer size in bytes
//    OUT PULONG ReturnLength OPTIONAL   // pointer to a 32-bit
//                                       // variable that receives
//                                       // the number of bytes
//                                       // written to the buffer 
// );


#ifndef WIN32_NO_STATUS // winnt
	#define STATUS_INFO_LENGTH_MISMATCH      (0xC0000004L)
#endif


#define SystemBasicInformation		0
#define SystemPerformanceInformation	2
#define SystemTimeInformation		3
#define SystemProcessInformation	5

#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))

typedef struct
{
    DWORD   dwUnknown1;
    ULONG   uKeMaximumIncrement;
    ULONG   uPageSize;
    ULONG   uMmNumberOfPhysicalPages;
    ULONG   uMmLowestPhysicalPage;
    ULONG   uMmHighestPhysicalPage;
    ULONG   uAllocationGranularity;
    PVOID   pLowestUserAddress;
    PVOID   pMmHighestUserAddress;
    ULONG   uKeActiveProcessors;	
    BYTE    bKeNumberProcessors;				//CPU数目
    BYTE    bUnknown2;
    WORD    wUnknown3;
} SYSTEM_BASIC_INFORMATION;

typedef struct
{
    LARGE_INTEGER   liIdleTime;
    DWORD           dwSpare[76];
} SYSTEM_PERFORMANCE_INFORMATION;

typedef struct
{
    LARGE_INTEGER liKeBootTime;
    LARGE_INTEGER liKeSystemTime;
    LARGE_INTEGER liExpTimeZoneBias;
    ULONG         uCurrentTimeZoneId;
    DWORD         dwReserved;
} SYSTEM_TIME_INFORMATION;



#if 0
typedef struct _TIME {
         DWORD LowPart;
         LONG HighPart;
} TIME, *PTIME;
#endif
 
typedef ULARGE_INTEGER  TIME, *PTIME;
 
/* typedef WCHAR                        UNICODE_STRING; */
typedef struct _UNICODE_STRING {
	USHORT      Length;
	USHORT      MaximumLength;
	PWSTR       Buffer;
} UNICODE_STRING, *PUNICODE_STRING;



typedef struct _CLIENT_ID
{
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
} CLIENT_ID, *PCLIENT_ID;
	
typedef enum _KWAIT_REASON
{
	Executive,
	FreePage,
	PageIn,
	PoolAllocation,
	DelayExecution,
	Suspended,
	UserRequest,
	WrExecutive,
	WrFreePage,
	WrPageIn,
	WrDelayExecution,
	WrSuspended,
	WrUserRequest,
	WrQueue,
	WrLpcReceive,
	WrLpcReply,
	WrVirtualMemory,
	WrPageOut,
	WrRendezvous,
	Spare2,
	Spare3,
	Spare4,
	Spare5,
	Spare6,
	WrKernel,
	MaximumWaitReason,
} KWAIT_REASON;
	

typedef struct _SYSTEM_THREAD_INFORMATION
{
         TIME            KernelTime;
         TIME            UserTime;
         TIME            CreateTime;
         ULONG           TickCount;
         ULONG           StartEIP;
         CLIENT_ID       ClientId;
         ULONG           DynamicPriority;
         ULONG           BasePriority;
         ULONG           nSwitches;
         DWORD           State;
         KWAIT_REASON    WaitReason;

} SYSTEM_THREAD_INFORMATION, *PSYSTEM_THREAD_INFORMATION;

typedef struct SYSTEM_PROCESS_INFORMATION
{
         ULONG                           NextEntryOffset;
         ULONG                           ThreadCount;
         ULONG                           Unused1 [6];
         TIME                            CreateTime;
         TIME                            UserTime;
         TIME                            KernelTime;
         UNICODE_STRING          Name;
         ULONG                           BasePriority;
         ULONG                           ProcessId;
         ULONG                           ParentProcessId;
         ULONG                           HandleCount;
         ULONG                           SessionId;
         ULONG                           Unused2;
         ULONG                           PeakVirtualSizeBytes;
         ULONG                           TotalVirtualSizeBytes;
         ULONG                           PageFaultCount;
         ULONG                           PeakWorkingSetSizeBytes;
         ULONG                           TotalWorkingSetSizeBytes;
         ULONG                           PeakPagedPoolUsagePages;
         ULONG                           TotalPagedPoolUsagePages;
         ULONG                           PeakNonPagedPoolUsagePages;
         ULONG                           TotalNonPagedPoolUsagePages;
         ULONG                           TotalPageFileUsageBytes;
         ULONG                           PeakPageFileUsageBytes;
         ULONG                           TotalPrivateBytes;
         SYSTEM_THREAD_INFORMATION       ThreadSysInfo [1];
         
} SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;


typedef LONG (WINAPI *PROCNTQSI)(UINT,PVOID,ULONG,PULONG);


//
//以下使用TlHelp32获取进程句柄，计算进程的CPU时间
//
//

//计算CPU用量百分比
__int64 CalcCpuUsingPercent(HANDLE hSpecProcess);

//获取指定的进程句柄
HANDLE GetSpecProcessHandle( LPCTSTR lpctszAppName );

//将时间转换成LARGE_INTEGER
__int64 FileTimeToQuadWord( PFILETIME pft );









#endif