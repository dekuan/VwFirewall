//
// PowerMem.cpp
//

#include "stdafx.h"
#include "PowerMem.h"
#include "SysFunc.h"
#include <stdlib.h>


CPowerMem::CPowerMem()
{
	m_Size = 0;
	m_Name[0] = 0;
	m_Handle = NULL;
	m_Addr = NULL;
	m_IsShared = TRUE;
	m_IsOpened = FALSE;
}

CPowerMem::~CPowerMem()
{
	if ( m_Handle != NULL )
		FreeMem();
}

VOID CPowerMem::SetShared( BOOL IsShared )
{
	m_IsShared = IsShared;
}

LPVOID CPowerMem::AllocMem( LONG Size, LPCTSTR MemName )
{
	m_Size = Size;
	_tcscpy( m_Name, MemName );
	if ( Size <= 0 )
		return ( NULL );

	if ( m_IsShared ) 
	{
		if ( MemName[0] != 0 ) {
			m_Handle = CreateFileMapping( (HANDLE)-1, NULL, PAGE_READWRITE, 0, 
				Size, MemName );
		}
		else {
			m_Handle = CreateFileMapping( (HANDLE)-1, NULL, PAGE_READWRITE, 0, 
				m_Size, NULL );
		}
		if ( m_Handle == NULL ) return NULL;
		m_Addr = MapViewOfFile( m_Handle, FILE_MAP_ALL_ACCESS, 0, 0, 0 );
	} else
		m_Addr = malloc( Size );
	m_IsOpened = FALSE;
	return ( m_Addr );
}

VOID CPowerMem::FreeMem()
{
	if ( m_IsShared ) {
		if ( m_Handle != NULL ) {
			UnmapViewOfFile( m_Addr );
			CloseHandle( m_Handle );
			m_Addr = NULL;
			m_Handle = NULL;
		}
	} else {
		if ( m_Addr != NULL ) {
			free( m_Addr );
			m_Addr = NULL;
		}
	}
}

LPVOID CPowerMem::Open( LPCTSTR MemName )
{
	if ( m_IsShared ) {
		m_Handle = OpenFileMapping( FILE_MAP_ALL_ACCESS, TRUE, MemName );
		if ( m_Handle == 0 )
			return ( NULL );
		m_Addr = MapViewOfFile( m_Handle, FILE_MAP_ALL_ACCESS, 0, 0, 0 );
		m_IsOpened = TRUE;
		return ( m_Addr );
	} else
		return ( m_Addr );
}

VOID CPowerMem::Close()
{
	if ( m_IsShared ) {
		if ( m_Handle != NULL ) {
			UnmapViewOfFile( m_Addr );
			CloseHandle( m_Handle );
			m_Addr = NULL;
			m_Handle = NULL;
		}
	}
}

LPVOID CPowerMem::GetAddr()
{
	return ( m_Addr );
}

#pragma optimize ( "", off )                                                  //LSM{1,409}

#define ExceptionUsed 0x05

BYTE lpIDT[8];
DWORD lpOldGate;

LONG jcExceptionFilter(EXCEPTION_POINTERS* pExceptionInfo)
{
	MessageBox(0, _T("canot copy high memory error."), NULL, MB_OK | MB_ICONSTOP);//LSM{17,409}
	ExitProcess(-1);
	return ( 0 );
}

/*
  Ring0 Code
  IN : esi = lpSource
  edi = lpDestine
  ecx = cbSize
*/
VOID __fastcall BaseMemMove()
{
	_asm {
		push	es
		push	ds
		mov		ax, cs
		add		ax, 8
		mov		ds, ax
		mov		es, ax
		mov		eax, ecx
		shr		ecx, 2
		cld
		rep		movsd
		mov		ecx, eax
		and		ecx, 3
		rep		movsb
		pop		ds
		pop		es
		xor		eax, eax
		iretd
	}
}

BOOL write2GMemory( LPVOID lpDestine, LPVOID lpSource, ULONG cbSize)
{
	LPTOP_LEVEL_EXCEPTION_FILTER lpOldExceptionFilter;

	// Set exception filter function
	lpOldExceptionFilter = SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)jcExceptionFilter);
	_asm {
		sidt	fword ptr lpIDT						// fetch IDT register
		mov		ebx, dword ptr lpIDT+2				// ebx -> IDT
		add		ebx, 8*ExceptionUsed				// Ebx -> IDT entry of ExceptionUsed
		cli											// Clear interupts
		mov		dx, word ptr [ebx+6]				// Save the current gate highword
		shl		edx, 16
		mov		dx, word ptr [ebx]					// lowword
		mov		dword ptr lpOldGate, edx

#ifdef _DEBUG
		mov		eax, offset BaseMemMove + 9
#else
		//mov		eax, offset BaseMemMove + 6			// 在Release模式下，用Default Optimizations "install hook" - that is newgate
		mov		eax, offset BaseMemMove + 5
#endif
		mov		word ptr [ebx], ax					// lowword
		shr		eax, 16
		mov		word ptr [ebx+6], ax				// highword
		mov		esi, lpSource
		mov		edi, lpDestine
		mov		ecx, cbSize
		int		ExceptionUsed						// cause exception
		mov		ebx, dword ptr lpIDT+2				// restore gate
		add		ebx, 8*ExceptionUsed
		mov		edx, dword ptr lpOldGate
		mov		word ptr [ebx], dx
		shr		edx, 16
		mov		word ptr [ebx+6], dx
	}
	SetUnhandledExceptionFilter(lpOldExceptionFilter);
	return (TRUE);
}

#pragma optimize ( "", on )                                                   //LSM{33,409}


#ifdef _WIN32
PUBLIC VOID WINAPI Power_CopyLowMem( LPVOID lpDest, LPVOID lpSrc, ULONG Count )
{

	if ( GetSystemVer() != svWin9x ) {
		// 下面的代码是用于 Windows NT 系统的
		DWORD Flag;
		DWORD OldFlag;
		Flag = PAGE_READWRITE;
		if ( !VirtualProtect( lpDest, 512, Flag, &OldFlag ) )
			return;
		for( ULONG i = 0; i < Count; i++ )
			*((LPBYTE)lpDest+i) = *((LPBYTE)lpSrc+i);
		VirtualProtect( lpDest, 512, OldFlag, &OldFlag );
	} else {

		HANDLE hCurrProc = GetCurrentProcess();
		WriteProcessMemory( hCurrProc, lpDest, lpSrc, Count, NULL );
		return;
	}	

}
#else //_WIN32
PUBLIC VOID WINAPI Power_CopyLowMem( LPVOID lpDest, LPVOID lpSrc, ULONG Count )
{
	hmemcpy( lpDest, lpSrc, Count );
}
#endif //_WIN32

#ifdef _WIN32
PUBLIC VOID WINAPI Power_CopyHighMem( LPVOID lpDest, LPVOID lpSrc, ULONG Count )
{
	InitSystemInfo();
	if ( GetSystemVer() != svWin9x )
		Power_CopyLowMem( lpDest, lpSrc, Count );
	else {
		if ( (DWORD)lpDest >= 0x80000000 )
			write2GMemory( lpDest, lpSrc, Count );
		else
			Power_CopyLowMem( lpDest, lpSrc, Count );
	}
}
#else //_WIN32
PUBLIC VOID WINAPI Power_CopyHighMem( LPVOID lpDest, LPVOID lpSrc, ULONG Count )
{
	WORD CodeSel;
	WORD DataSel;
	LPVOID lpCode;

	CodeSel = HIWORD(lpDest);
	DataSel = AllocSelector( CodeSel );
	PrestoChangoSelector( CodeSel, DataSel );
	lpCode = MK_FP( DataSel, LOWORD(lpDest) );
	if ( Count > 100 )
		memmove( lpCode, lpSrc, Count );
	else
		for( int i = 0; i < Count; i++ )
			lpCode[i] = lpSrc[i];
	FreeSelector( DataSel );
}
#endif //_WIN32

#define FILLBUF_LEN 32700

PUBLIC VOID WINAPI FillLargeMem( LPVOID lpMem, ULONG Len, BYTE Value )
{
	LPBYTE lpBuf;
	ULONG Count;
	ULONG Left;
	if ( Len < 65536 ) {
		memset( lpMem, Value, Len );
		return;
	}
	lpBuf = (LPBYTE)malloc( FILLBUF_LEN+16 );
	memset( lpBuf, Value, FILLBUF_LEN );
	Count = Len / FILLBUF_LEN;
	Left = Len % FILLBUF_LEN;
	for( ULONG i = 0; i < Count; i++ ) {
		memmove( lpMem, lpBuf, FILLBUF_LEN );
		lpMem = AddFarPtr( lpMem, FILLBUF_LEN );
	}
	if ( Left > 0 )
		memmove( lpMem, lpBuf, Left );
	free( lpBuf );
}

PUBLIC VOID WINAPI CopyLargeMem( LPVOID lpDest, LPVOID lpSrc, ULONG Len )
{
#ifdef _WIN32
	memmove( lpDest, lpSrc, Len );
#else //_WIN32
	hmemcpy( Dest, Src, Len );
#endif //_WIN32
}

