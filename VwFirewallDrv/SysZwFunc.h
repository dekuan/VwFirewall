#ifndef KFZWFUNC_H
#define KFZWFUNC_H
/////////////////////////////////////////////////////////////////
//  文件作用：声明系统未定义的函数及定义相关结构
//  作用范围：在所有模块均可见
/////////////////////////////////////////////////////////////////
//  修改历史:
//         V3.00 黄森堂(vcmfc) 2004-1-15
/////////////////////////////////////////////////////////////////
//  版权 黄森堂 (C) 2003-2005   所有.
///////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
// 检查当前IRP是文件/目录等常量
////////////////////////////////////////////////////////
typedef CSHORT NODE_TYPE_CODE;
typedef NODE_TYPE_CODE *PNODE_TYPE_CODE;

#define FAT_NTC_FCB                      ((NODE_TYPE_CODE)0x0502)
#define NTFS_NTC_FCB                     ((NODE_TYPE_CODE)0x0705)
#define FAT_NTC_DCB                      ((NODE_TYPE_CODE)0x0503)
#define FAT_NTC_ROOT_DCB                 ((NODE_TYPE_CODE)0x0504)

#ifndef NodeType
	#define NodeType(Ptr) (*((PNODE_TYPE_CODE)(Ptr)))
#endif	//NodeType

typedef struct _SRVTABLE {
	PVOID           *ServiceTable;
	ULONG           LowCall;        
	ULONG           HiCall;
	PVOID		    *ArgTable;
} SRVTABLE, *PSRVTABLE;


#if defined(_ALPHA_)
	#define SYSCALL(_function)  KeServiceDescriptorTable->ServiceTable[ (*(PULONG)_function)  & 0x0000FFFF ]
#else
	#define SYSCALL(_function)  KeServiceDescriptorTable->ServiceTable[ *(PULONG)((PUCHAR)_function+1)]
#endif

#if WINVER < 0x0501

#define RtlInitEmptyUnicodeString(_ucStr,_buf,_bufSize) \
    ((_ucStr)->Buffer = (_buf), \
     (_ucStr)->Length = 0, \
     (_ucStr)->MaximumLength = (USHORT)(_bufSize))

typedef struct _PROCESS_BASIC_INFORMATION {
    NTSTATUS ExitStatus;
    PPEB PebBaseAddress;
    ULONG_PTR AffinityMask;
    KPRIORITY BasePriority;
    ULONG_PTR UniqueProcessId;
    ULONG_PTR InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION;
typedef PROCESS_BASIC_INFORMATION *PPROCESS_BASIC_INFORMATION;

//
// Process Information Classes
//

typedef enum _PROCESSINFOCLASS {
    ProcessBasicInformation,
    ProcessQuotaLimits,
    ProcessIoCounters,
    ProcessVmCounters,
    ProcessTimes,
    ProcessBasePriority,
    ProcessRaisePriority,
    ProcessDebugPort,
    ProcessExceptionPort,
    ProcessAccessToken,
    ProcessLdtInformation,
    ProcessLdtSize,
    ProcessDefaultHardErrorMode,
    ProcessIoPortHandlers,          // Note: this is kernel mode only
    ProcessPooledUsageAndLimits,
    ProcessWorkingSetWatch,
    ProcessUserModeIOPL,
    ProcessEnableAlignmentFaultFixup,
    ProcessPriorityClass,
    ProcessWx86Information,
    ProcessHandleCount,
    ProcessAffinityMask,
    ProcessPriorityBoost,
    ProcessDeviceMap,
    ProcessSessionInformation,
    ProcessForegroundInformation,
    ProcessWow64Information,
    ProcessImageFileName,
    ProcessLUIDDeviceMapsEnabled,
    ProcessBreakOnTermination,
    ProcessDebugObjectHandle,
    ProcessDebugFlags,
    ProcessHandleTracing,
    MaxProcessInfoClass             // MaxProcessInfoClass should always be the last enum
    } PROCESSINFOCLASS;

//
// Process I/O Counters
//  NtQueryInformationProcess using ProcessIoCounters
//

// begin_winnt
typedef struct _IO_COUNTERS {
    ULONGLONG  ReadOperationCount;
    ULONGLONG  WriteOperationCount;
    ULONGLONG  OtherOperationCount;
    ULONGLONG ReadTransferCount;
    ULONGLONG WriteTransferCount;
    ULONGLONG OtherTransferCount;
} IO_COUNTERS;
typedef IO_COUNTERS *PIO_COUNTERS;
// end_winnt

//
// Process Virtual Memory Counters
//  NtQueryInformationProcess using ProcessVmCounters
//

typedef struct _VM_COUNTERS {
    SIZE_T PeakVirtualSize;
    SIZE_T VirtualSize;
    ULONG PageFaultCount;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage;
    SIZE_T QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
} VM_COUNTERS;
typedef VM_COUNTERS *PVM_COUNTERS;

#endif
struct _SYSTEM_THREADS
{
	LARGE_INTEGER KernelTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER CreateTime;
	ULONG WaitTime;
	PVOID StartAddress;
	CLIENT_ID ClientIs;
	KPRIORITY Priority;
	KPRIORITY BasePriority;
	ULONG ContextSwitchCount;
	ULONG ThreadState;
	KWAIT_REASON WaitReason;
};

struct _SYSTEM_PROCESSES
{
	ULONG NextEntryDelta;
	ULONG ThreadCount;
	ULONG Reserved[6];
	LARGE_INTEGER CreateTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER KernelTime;
	UNICODE_STRING ProcessName;
	KPRIORITY BasePriority;
	ULONG ProcessId;
	ULONG InheritedFromProcessId;
	ULONG HandleCount;
	ULONG Reserved2[2];
	VM_COUNTERS VmCounters;
	IO_COUNTERS IoCounters;
	struct _SYSTEM_THREADS Threads[1];
};

//extern "C"
//{
	NTSYSAPI
	NTSTATUS
	NTAPI
	ZwOpenSymbolicLinkObject(
		OUT PHANDLE LinkHandle,
		IN ACCESS_MASK DesiredAccess,
		IN POBJECT_ATTRIBUTES ObjectAttributes
		);

	NTSYSAPI
	NTSTATUS
	NTAPI
	ZwQuerySymbolicLinkObject(
		IN HANDLE LinkHandle,
		IN OUT PUNICODE_STRING LinkTarget,
		OUT PULONG ReturnedLength OPTIONAL
		);

	NTSYSAPI
	NTSTATUS
	NTAPI
	ZwDeleteFile(
	    IN POBJECT_ATTRIBUTES ObjectAttributes
	    );
	
	
	NTSYSAPI
	NTSTATUS
	NTAPI
	ZwQueryAttributesFile(
	    IN POBJECT_ATTRIBUTES ObjectAttributes,
	    OUT PFILE_BASIC_INFORMATION FileInformation
	    );

	NTSYSAPI
	NTSTATUS
	NTAPI ZwDeleteValueKey( IN HANDLE, IN PUNICODE_STRING );
	
	NTKERNELAPI
	NTSTATUS
	ObQueryNameString(
	    IN PVOID Object,
	    OUT POBJECT_NAME_INFORMATION ObjectNameInfo,
	    IN ULONG Length,
	    OUT PULONG ReturnLength
	    );
	    
	NTSYSAPI
	NTSTATUS
	NTAPI
	ZwQueryInformationProcess (
	    IN HANDLE           ProcessHandle,
	    IN PROCESSINFOCLASS ProcessInformationClass,
	    OUT PVOID           ProcessInformation,
	    IN ULONG            ProcessInformationLength,
	    OUT PULONG          ReturnLength OPTIONAL
	);
	
	NTSYSAPI
	NTSTATUS
	NTAPI
	ZwTerminateProcess (
	    IN HANDLE   ProcessHandle OPTIONAL,
	    IN NTSTATUS ExitStatus
	);

/*	NTSYSAPI
	NTSTATUS
	NTAPI ZwQuerySystemInformation(
	            IN ULONG SystemInformationClass,
				IN PVOID SystemInformation,
				IN ULONG SystemInformationLength,
				OUT PULONG ReturnLength);
*/
/*
	NTSTATUS PsLookupProcessByProcessId(
	     IN ULONG ulProcId,
	     OUT PEPROCESS * pEProcess
	     );
*/
//}

#endif //KFZWFUNC_H
