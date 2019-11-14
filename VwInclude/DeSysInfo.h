// DeSysInfo.h: interface for the CDeSysInfo class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __DESYSINFO_HEADER__
#define __DESYSINFO_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ntdll.h"






/**
 *	struct of system performance
 */
typedef struct _DE_SYSTEM_PERFORMANCE_INFORMATION
{
	LARGE_INTEGER liIdleTime;			//	CPU空闲时间；
	LARGE_INTEGER liReadTransferCount;		//	I/O读操作数目；
	LARGE_INTEGER liWriteTransferCount;		//	I/O写操作数目；
	LARGE_INTEGER liOtherTransferCount;		//	I/O其他操作数目；
	ULONG ulReadOperationCount;			//	I/O读数据数目；
	ULONG ulWriteOperationCount;			//	I/O写数据数目；
	ULONG ulOtherOperationCount;			//	I/O其他操作数据数目；
	ULONG ulAvailablePages;				//	可获得的页数目；
	ULONG ulTotalCommittedPages;			//	总共提交页数目；
	ULONG ulTotalCommitLimit;			//	已提交页数目；
	ULONG ulPeakCommitment;				//	页提交峰值；
	ULONG ulPageFaults;				//	页故障数目；
	ULONG ulWriteCopyFaults;			//	Copy-On-Write故障数目；
	ULONG ulTransitionFaults;			//	软页故障数目；
	ULONG ulReserved1;
	ULONG ulDemandZeroFaults;			//	需求0故障数；
	ULONG ulPagesRead;				//	读页数目；
	ULONG ulPageReadIos;				//	读页I/O操作数；
	ULONG ulReserved2[2];
	ULONG ulPagefilePagesWritten;			//	已写页文件页数；
	ULONG ulPagefilePageWriteIos;			//	已写页文件操作数；
	ULONG ulMappedFilePagesWritten;			//	已写映射文件页数；
	ULONG ulMappedFileWriteIos;			//	已写映射文件操作数；
	ULONG ulPagedPoolUsage;				//	分页池使用；
	ULONG ulNonPagedPoolUsage;			//	非分页池使用；
	ULONG ulPagedPoolAllocs;			//	分页池分配情况；
	ULONG ulPagedPoolFrees;				//	分页池释放情况；
	ULONG ulNonPagedPoolAllocs;			//	非分页池分配情况；
	ULONG ulNonPagedPoolFress;			//	非分页池释放情况；
	ULONG ulTotalFreeSystemPtes;			//	系统页表项释放总数；
	ULONG ulSystemCodePage;				//	操作系统代码页数；
	ULONG ulTotalSystemDriverPages;			//	可分页驱动程序页数；
	ULONG ulTotalSystemCodePages;			//	操作系统代码页总数；
	ULONG ulSmallNonPagedLookasideListAllocateHits;	//	小非分页侧视列表分配次数；
	ULONG ulSmallPagedLookasideListAllocateHits;	//	小分页侧视列表分配次数；
	ULONG ulReserved3;
	ULONG ulMmSystemCachePage;			//	系统缓存页数；
	ULONG ulPagedPoolPage;				//	分页池页数；
	ULONG ulSystemDriverPage;			//	可分页驱动页数；
	ULONG ulFastReadNoWait;				//	异步快速读数目；
	ULONG ulFastReadWait;				//	同步快速读数目；
	ULONG ulFastReadResourceMiss;			//	快速读资源冲突数；
	ULONG ulFastReadNotPossible;			//	快速读失败数；
	ULONG ulFastMdlReadNoWait;			//	异步MDL快速读数目；
	ULONG ulFastMdlReadWait;			//	同步MDL快速读数目；
	ULONG ulFastMdlReadResourceMiss;		//	MDL读资源冲突数；
	ULONG ulFastMdlReadNotPossible;			//	MDL读失败数；
	ULONG ulMapDataNoWait;				//	异步映射数据次数；
	ULONG ulMapDataWait;				//	同步映射数据次数；
	ULONG ulMapDataNoWaitMiss;			//	异步映射数据冲突次数；
	ULONG ulMapDataWaitMiss;			//	同步映射数据冲突次数；
	ULONG ulPinMappedDataCount;			//	牵制映射数据数目；
	ULONG ulPinReadNoWait;				//	牵制异步读数目；
	ULONG ulPinReadWait;				//	牵制同步读数目；
	ULONG ulPinReadNoWaitMiss;			//	牵制异步读冲突数目；
	ULONG ulPinReadWaitMiss;			//	牵制同步读冲突数目；
	ULONG ulCopyReadNoWait;				//	异步拷贝读次数；
	ULONG ulCopyReadWait;				//	同步拷贝读次数；
	ULONG ulCopyReadNoWaitMiss;			//	异步拷贝读故障次数；
	ULONG ulCopyReadWaitMiss;			//	同步拷贝读故障次数；
	ULONG ulMdlReadNoWait;				//	异步MDL读次数；
	ULONG ulMdlReadWait;				//	同步MDL读次数；
	ULONG ulMdlReadNoWaitMiss;			//	异步MDL读故障次数；

	ULONG ulMdlReadWaitMiss;			//	同步MDL读故障次数；
	ULONG ulReadAheadIos;				//	向前读操作数目；
	ULONG ulLazyWriteIos;				//	LAZY写操作数目；
	ULONG ulLazyWritePages;				//	LAZY写页文件数目；
	ULONG ulDataFlushes;				//	缓存刷新次数；
	ULONG ulDataPages;				//	缓存刷新页数；
	ULONG ulContextSwitches;			//	# 环境切换数目；
	ULONG ulFirstLevelTbFills;			//	第一层缓冲区填充次数；
	ULONG ulSecondLevelTbFills;			//	第二层缓冲区填充次数；
	ULONG ulSystemCall;				//	系统调用次数；

}DE_SYSTEM_PERFORMANCE_INFORMATION, *PDE_SYSTEM_PERFORMANCE_INFORMATION;

typedef struct tagDeSysInfo
{
	tagDeSysInfo()
	{
		memset( this, 0, sizeof(tagDeSysInfo) );
	}

	//	memory info
	DWORDLONG ullMemPhyTotal;		//	总共物理内存(单位:MB)
	DWORDLONG ullMemPhyAvail;		//	可用物理内存(单位:MB)
	DWORDLONG ullMemVirTotal;		//	总共页面文件(单位:MB)
	DWORDLONG ullMemVirAvail;		//	可用页面文件(单位:MB)
	
	//	kernel memory info
	DWORDLONG ullMemKernelPaged;		//	paged
	DWORDLONG ullMemKernelNP;		//	nonpaged
	DWORDLONG ullMemKernelTotal;		//	total

	//	I/O
	LARGE_INTEGER liReadTransferCount;
	LARGE_INTEGER liWriteTransferCount;
	LARGE_INTEGER liOtherTransferCount;

}STDESYSINFO, *PSTDESYSINFO;

/**
 *	class of CDeSysInfo
 */
class CDeSysInfo
{
public:
	CDeSysInfo();
	virtual ~CDeSysInfo();

	BOOL Query( STDESYSINFO * pstSysInfo );

private:
	BOOL IsWow64();

private:
	PROCNTQSI m_pfnNtQuerySystemInformation;
};

#endif // __DESYSINFO_HEADER__
