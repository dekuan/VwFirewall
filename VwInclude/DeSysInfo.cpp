// DeSysInfo.cpp: implementation of the CDeSysInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DeSysInfo.h"

#include "DeLib.h"
#pragma comment( lib, "DeLib.lib" )

#include "Wincrypt.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDeSysInfo::CDeSysInfo()
{
	//m_pfnNtQuerySystemInformation = (PROCNTQSI)GetProcAddress( GetModuleHandleA("ntdll"), "NtQuerySystemInformation" );
	m_pfnNtQuerySystemInformation = NULL;
}

CDeSysInfo::~CDeSysInfo()
{

}

BOOL CDeSysInfo::Query( STDESYSINFO * pstSysInfo )
{
	if ( NULL == pstSysInfo )
	{
		return FALSE;
	}

	BOOL bRet	= FALSE;
//	LONG lnStatus;
//	SYSTEM_BASIC_INFORMATION stBasicInfo;
//	DE_SYSTEM_PERFORMANCE_INFORMATION stPerfInfo;
//	MEMORYSTATUS memStatus;
	MEMORYSTATUSEX memStatusEx;

	bRet = TRUE;
	
	memset( &memStatusEx, 0, sizeof(memStatusEx) );
	memStatusEx.dwLength = sizeof( memStatusEx );
	GlobalMemoryStatusEx( &memStatusEx );
	
	pstSysInfo->ullMemPhyTotal	= memStatusEx.ullTotalPhys;	//	总共物理内存
	pstSysInfo->ullMemPhyAvail	= memStatusEx.ullAvailPhys;	//	可用物理内存
	
	pstSysInfo->ullMemVirTotal	= memStatusEx.ullTotalPageFile;	//	总共页面文件
	pstSysInfo->ullMemVirAvail	= memStatusEx.ullAvailPageFile;	//	可用页面文件

	return bRet;

/*
	typedef struct _SYSTEM_INFO {
		union {
			DWORD dwOemId;          // Obsolete field...do not use
			struct {
				WORD wProcessorArchitecture;
				WORD wReserved;
			};
		};
		DWORD dwPageSize;
		LPVOID lpMinimumApplicationAddress;
		LPVOID lpMaximumApplicationAddress;
		DWORD_PTR dwActiveProcessorMask;
		DWORD dwNumberOfProcessors;
		DWORD dwProcessorType;
		DWORD dwAllocationGranularity;
		WORD wProcessorLevel;
		WORD wProcessorRevision;
} SYSTEM_INFO, *LPSYSTEM_INFO;

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
*/
/*
	SYSTEM_INFO siSysBaseInfo;
	HCRYPTPROV hCryptProv		= NULL;
	TCHAR * pszContainerName	= NULL;
	DWORD dwProvType		= PROV_RSA_FULL;


	GetSystemInfo( &siSysBaseInfo );

	if ( CryptAcquireContext( &hCryptProv, NULL, pszContainerName, dwProvType, 0 ) )
	{
		if ( CryptGenRandom( hCryptProv, sizeof(stPerfInfo), (BYTE*)( &stPerfInfo ) ) )
		{
			bRet = TRUE;
			
			//
			//	内存信息
			//	单位: 字节
			//
			pstSysInfo->dwMemPhyTotal	= (ULONG)siSysBaseInfo.lpMinimumApplicationAddress * ( siSysBaseInfo.dwPageSize / 1024 ) * 1024;
			pstSysInfo->dwMemPhyAvail	= stPerfInfo.ulAvailablePages * ( siSysBaseInfo.dwPageSize / 1024 ) * 1024;
			pstSysInfo->dwMemVirTotal	= stPerfInfo.ulTotalCommitLimit * ( siSysBaseInfo.dwPageSize / 1024 ) * 1024;
			pstSysInfo->dwMemVirAvail	= ( stPerfInfo.ulTotalCommitLimit - stPerfInfo.ulTotalCommittedPages ) * ( siSysBaseInfo.dwPageSize / 1024 ) * 1024;
		
			//	Kernel memory : Paged
			pstSysInfo->dwMemKernelPaged	= stPerfInfo.ulPagedPoolPage * ( siSysBaseInfo.dwPageSize / 1024 ) * 1024;
			
			//	Kernel memory : Nonpaged
			pstSysInfo->dwMemKernelNP	= stPerfInfo.ulNonPagedPoolUsage * ( siSysBaseInfo.dwPageSize / 1024 ) * 1024;
			
			//	Kernel memory : total
			pstSysInfo->dwMemKernelTotal	= pstSysInfo->dwMemKernelNP + pstSysInfo->dwMemKernelPaged;
			
			
			//
			//	I/O status (硬盘 IO 信息)
			//	单位: 字节
			//
			pstSysInfo->liReadTransferCount		= stPerfInfo.liReadTransferCount;
			pstSysInfo->liWriteTransferCount	= stPerfInfo.liWriteTransferCount;
			pstSysInfo->liOtherTransferCount	= stPerfInfo.liOtherTransferCount;
		}

		CryptReleaseContext( hCryptProv, 0 );
	}

	return bRet;
*/



	__try
	{
		//if ( IsWow64() )
		{
			bRet = TRUE;
			
			memset( &memStatusEx, 0, sizeof(memStatusEx) );
			memStatusEx.dwLength = sizeof( memStatusEx );
			GlobalMemoryStatusEx( &memStatusEx );
			
			pstSysInfo->ullMemPhyTotal	= memStatusEx.ullTotalPhys;	//	总共物理内存
			pstSysInfo->ullMemPhyAvail	= memStatusEx.ullAvailPhys;	//	可用物理内存
			
			pstSysInfo->ullMemVirTotal	= memStatusEx.ullTotalPageFile;	//	总共页面文件
			pstSysInfo->ullMemVirAvail	= memStatusEx.ullAvailPageFile;	//	可用页面文件
		}

		/*
		if ( ! bRet && m_pfnNtQuerySystemInformation )
		{
			lnStatus = m_pfnNtQuerySystemInformation( SystemBasicInformation, &stBasicInfo, sizeof(stBasicInfo), NULL );
			if ( NO_ERROR == lnStatus )
			{
				lnStatus = m_pfnNtQuerySystemInformation( SystemPerformanceInformation, &stPerfInfo, sizeof(stPerfInfo), NULL );
				if ( NO_ERROR == lnStatus )
				{
					bRet = TRUE;

					//
					//	内存信息
					//	单位: 字节
					//
					pstSysInfo->ullMemPhyTotal	= stBasicInfo.uMmNumberOfPhysicalPages * ( stBasicInfo.uPageSize / 1024 ) * 1024;
					pstSysInfo->ullMemPhyAvail	= stPerfInfo.ulAvailablePages * ( stBasicInfo.uPageSize / 1024 ) * 1024;
					pstSysInfo->ullMemVirTotal	= stPerfInfo.ulTotalCommitLimit * ( stBasicInfo.uPageSize / 1024 ) * 1024;
					pstSysInfo->ullMemVirAvail	= ( stPerfInfo.ulTotalCommitLimit - stPerfInfo.ulTotalCommittedPages ) * ( stBasicInfo.uPageSize / 1024 ) * 1024;

					//	Kernel memory : Paged
					pstSysInfo->ullMemKernelPaged	= stPerfInfo.ulPagedPoolPage * ( stBasicInfo.uPageSize / 1024 ) * 1024;

					//	Kernel memory : Nonpaged
					pstSysInfo->ullMemKernelNP	= stPerfInfo.ulNonPagedPoolUsage * ( stBasicInfo.uPageSize / 1024 ) * 1024;

					//	Kernel memory : total
					pstSysInfo->ullMemKernelTotal	= pstSysInfo->ullMemKernelNP + pstSysInfo->ullMemKernelPaged;

					//
					//	I/O status (硬盘 IO 信息)
					//	单位: 字节
					//
					pstSysInfo->liReadTransferCount		= stPerfInfo.liReadTransferCount;
					pstSysInfo->liWriteTransferCount	= stPerfInfo.liWriteTransferCount;
					pstSysInfo->liOtherTransferCount	= stPerfInfo.liOtherTransferCount;
				}
			}
		}

		if ( ! bRet )
		{
			//if ( _DEOS_WINDOWS_2000 != delib_get_shellsystype() && _DEOS_WINDOWS_XP != delib_get_shellsystype() )
			{
				bRet = TRUE;

				memset( &memStatusEx, 0, sizeof(memStatusEx) );
				memStatusEx.dwLength = sizeof( memStatusEx );
				GlobalMemoryStatusEx( &memStatusEx );

				pstSysInfo->ullMemPhyTotal	= memStatusEx.ullTotalPhys;	//	总共物理内存
				pstSysInfo->ullMemPhyAvail	= memStatusEx.ullAvailPhys;	//	可用物理内存

				pstSysInfo->ullMemVirTotal	= memStatusEx.ullTotalPageFile;	//	总共页面文件
				pstSysInfo->ullMemVirAvail	= memStatusEx.ullAvailPageFile;	//	可用页面文件
			}
		}
		*/
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}	

	return bRet;
}

BOOL CDeSysInfo::IsWow64()
{
	BOOL bIsWow64 = FALSE;
	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE hProcess,PBOOL Wow64Process);
	LPFN_ISWOW64PROCESS fnIsWow64Process;

	__try
	{
		fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress( GetModuleHandle(_T("kernel32")), "IsWow64Process" );
		if (NULL != fnIsWow64Process)
		{
			if ( ! fnIsWow64Process( GetCurrentProcess(), &bIsWow64 ) )
			{
				// handle error
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return bIsWow64;
}
