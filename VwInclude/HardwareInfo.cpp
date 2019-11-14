// HardwareInfo.cpp: implementation of the CHardwareInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HardwareInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHardwareInfo::CHardwareInfo()
{

}

CHardwareInfo::~CHardwareInfo()
{

}

/**
 *	获取 cpuid
 */
BOOL CHardwareInfo::GetCpuId( LPTSTR lpszCPUID, DWORD dwSize )
{
	//
	//	lpszCPUID	- [out] cpuid 返回值
	//	dwSize		- [in]  返回值缓冲区大小
	//	RETURN		- TRUE / FALSE
	//
	//	备注：
	//		_asm cpuid 指令可以被一下 CPU 识别：
	//			- Intel 486 以上的 CPU
	//			- Cyrix M1 以上的 CPU
	//			- AMD Am486 以上的 CPU
	//

	if ( NULL == lpszCPUID || 0 == dwSize )
	{
		return FALSE;
	}

	BOOL  bException	= FALSE;
	TCHAR szOEMInfo[ 13 ]	= {0};
	INT nFamily;
	INT nEAXValue, nEBXValue, nECXValue, nEDXValue;
	INT nALValue;
	ULONG ulIdPart1	= 0;
	ULONG ulIdPart2	= 0;
	ULONG ulIdPart3	= 0;
	ULONG ulIdPart4	= 0;

#ifdef _M_IX86
	__try
	{
		//	get OEM info
		//
		//	(1)判断CPU厂商，先让 EAX = 0，再调用 cpuid
		//
		_asm
		{
			mov	eax,	0
			cpuid
			mov	DWORD PTR szOEMInfo[0],	ebx
			mov	DWORD PTR szOEMInfo[4],	edx
			mov	DWORD PTR szOEMInfo[8],	ecx
			mov	BYTE PTR szOEMInfo[12],	0
		}

		//
		//	get family number
		//
		//	(2)CPU 到底是几 86，是否支持 MMX
		//	先让 EAX = 1，再调用 cpuid
		//	EAX 的 8 到 11 位就表明是几 86
		//	3 - 386
		//	4 - i486
		//	5 - Pentium
		//	6 - Pentium Pro Pentium II
		//	2 - Dual Processors
		//	EDX 的第 0 位: 有无 FPU
		//	EDX 的第 23 位: CPU 是否支持 IA MMX，很重要啊！如果你想用那 57 条新增的指令，先 
		//	检查这一位吧，否则就等着看Windows的“该程序执行了非法指令，将被关闭”吧 。
		//
		//	(3)专门检测是否 P6 架构
		//	先让 EAX = 1，再调用 CPUID
		//	如果 AL = 1，就是 Pentium Pro 或 Pentium II
		//
		//	(4)专门检测 AMD 的 CPU 信息
		//	先让 EAX = 80000001H，再调用 CPUID
		//	如果 EAX = 51H，是 AMD K5
		//	如果 EAX = 66H，是 K6
		//	K7 是什么标志，只有等大家拿到 K7 的芯再说了。
		//	EDX 第 0 位: 是否有 FPU(多余的！谁用过没 FPU 的 K5,K6?)
		//	EDX 第 23 位，CPU 是否支持 MMX
		//
		_asm
		{
			mov	eax,	1
			cpuid
			mov	nEAXValue,	eax
			mov	nEBXValue,	ebx
			mov	nECXValue,	ecx
			mov	nEDXValue,	edx
		}
		nFamily	= ( 0xf00 & nEAXValue ) >> 8;

		_asm
		{
			mov	eax,	2
			cpuid
			mov	nALValue,	eax
		}
		
		//
		//	get cpu id
		//
		//	CPU 的序列号用一个 96bit 的串表示，格式是连续的 6 个 WORD 值：XXXX-XXXX-XXXX-XXX-XXXX-XXXX。
		//	WORD是16个bit长的数据，可以用unsigned short模拟：
		//	typedef unsigned short WORD;
		//	获得序列号需要两个步骤，首先用 eax = 1 做参数，返回的 eax 中存储序列号的高两个 WORD。
		//	用 eax = 3 做参数，返回 ecx 和 edx 按从低位到高位的顺序存储前 4 个 WORD 即可得到 cpuid
		//
		_asm
		{
			mov	eax,	01h
			xor	edx,	edx
			cpuid
			mov	ulIdPart1,	edx
			mov	ulIdPart2,	eax
		}	
		_asm
		{
			mov	eax,	03h
			xor	ecx,	ecx
			xor	edx,	edx
			cpuid
			mov	ulIdPart3,	edx
			mov	ulIdPart4,	ecx
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		bException = TRUE;
	}
#endif

	_sntprintf
	(
		lpszCPUID, dwSize-sizeof(TCHAR),
		"%s-%d-%08x%08x%08x%08x",
		szOEMInfo, nFamily,
		ulIdPart1, ulIdPart2, ulIdPart3, ulIdPart4
	);
	_tcslwr( lpszCPUID );

	return bException ? FALSE : TRUE;
}



/**
*	获取硬盘序列号
*/
BOOL CHardwareInfo::GetHDiskSerialNumber( LPTSTR lpszSerialNumber, DWORD dwSize )
{
	BOOL  bRet	= FALSE;
	TCHAR szNameBuf[ 12 ];
	DWORD dwSerialNumber;
	DWORD dwMaxLen; 
	DWORD dwFileFlag;
	TCHAR szSysNameBuf[ 10 ];
	
	bRet = ::GetVolumeInformation
		(
			"c:\\",
			szNameBuf,
			sizeof(szNameBuf), 
			& dwSerialNumber, 
			& dwMaxLen,
			& dwFileFlag,
			szSysNameBuf,
			sizeof(szSysNameBuf)
		);
	if ( bRet )
	{
		_sntprintf( lpszSerialNumber, dwSize-sizeof(TCHAR), _T("%x"), dwSerialNumber );
	}
	
	return bRet;
}