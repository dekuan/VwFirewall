//
// PowerMem.h
//
// Power Memory Manager
//
#ifndef __POWERMEM_H__
#define __POWERMEM_H__


#include "SysFunc.h"


/******************************************************************************
  高级内存处理函数
*******************************************************************************/
class CPowerMem
{
private:
	LONG m_Size;
	TCHAR m_Name[256];
	HANDLE m_Handle;
	LPVOID m_Addr;
	BOOL m_IsShared;
	BOOL m_IsOpened;

public:
	CPowerMem();
	~CPowerMem();
	
	LPVOID AllocMem( LONG Size, LPCTSTR MemName );
	VOID FreeMem();
	LPVOID Open( LPCTSTR MemName );
	VOID Close();
	VOID SetShared( BOOL IsShared );
	LPVOID GetAddr();
};



#ifdef __cplusplus
extern "C" {
#endif


	
	
VOID WINAPI Power_CopyLowMem( LPVOID lpDest, LPVOID lpSrc, ULONG Count );
VOID WINAPI Power_CopyHighMem( LPVOID lpDest, LPVOID lpSrc, ULONG Count );
VOID WINAPI FillLargeMem( LPVOID lpMem, ULONG Len, BYTE Value );
VOID WINAPI CopyLargeMem( LPVOID lpDest, LPVOID lpSrc, ULONG Len );


#ifdef __cplusplus
}
#endif



#endif // __POWERMEM_H__




