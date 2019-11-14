//
// HookCode.cpp
//


#include "stdafx.h"
#include "HookCode.h"
#include "PowerMem.h"
#include "disasm.h"





#ifdef _WIN32
#else //_WIN32
LPVOID GetAPIProc( LPCSTR lpszModName, LPCSTR lpszAPIName );
VOID ModifyAPIProc( LPVOID lpOrnProc, LPVOID lpNewProc, LPPROCWRITE lpOrnProcBuf );
VOID RestoreAPIProc( LPVOID lpOrnProc, LPPROCWRITE lpOrnProcBuf );
WORD MyAllocSelector( INT Num );
VOID MyFreeSelector( WORD Sel );
VOID MySetSelectorBase( WORD Sel, LONG BaseLiner );
VOID MySetSelectorLimit( WORD Sel, LONG Limit );
#endif //_WIN32


PUBLIC ULONG WINAPI SearchInnerFunc( LPVOID lpStart, ULONG StartPos, ULONG EndPos, LPBYTE lpIDs, ULONG Count )
{
	BOOL bFind;
	while ( StartPos < EndPos-4 ) {
		bFind = TRUE;
		for ( ULONG i = 0; i < Count; i++ ) {
			if ( *((LPBYTE)lpStart+i) != lpIDs[i] ) {
				bFind = FALSE;
				break;
			}
		}
		if ( bFind )
			return ( StartPos );
		StartPos++;
		lpStart = (LPVOID)((LPSTR)lpStart + 1);
	}
	return ( 0xffffffff );
}

PUBLIC ULONG WINAPI SearchInnerFuncEx( LPVOID lpStart, ULONG nStartPos, ULONG nEndPos, LPCODE_SEARCH_ITEM lpIDItems, ULONG Count )
{
	for ( ULONG i = 0; i < Count; i++ ) {
		LPBYTE lpIDs = lpIDItems[i].Buf;
		int IDLen = lpIDItems[i].Len;
		ULONG nFindPos = SearchInnerFunc( lpStart, nStartPos, nEndPos, lpIDs, IDLen );
		if ( nFindPos != 0xffffffff )
			return ( nFindPos );
	}
	return ( 0xffffffff );
}


#ifdef _WIN32
PUBLIC VOID WINAPI SaveStubFunc( LPVOID lpOrnFunc, LPVOID lpJumpFunc, ULONG SkipCount )
{
	PROCWRITE WriteBuf;
	LONG JumpOffset;
	Power_CopyHighMem( lpJumpFunc, lpOrnFunc, SkipCount );
	lpJumpFunc = (LPVOID)((ULONG)lpJumpFunc + SkipCount);
	JumpOffset = (LONG)lpOrnFunc + SkipCount - (LONG)lpJumpFunc - 6 + 1;
	WriteBuf[0] = 0xe9;
	WriteBuf[1] = LOBYTE((LOWORD(JumpOffset)));
	WriteBuf[2] = HIBYTE((LOWORD(JumpOffset)));
	WriteBuf[3] = LOBYTE((HIWORD(JumpOffset)));
	WriteBuf[4] = HIBYTE((HIWORD(JumpOffset)));
	Power_CopyHighMem( lpJumpFunc, WriteBuf, 5 );
}
#else //_WIN32
PUBLIC VOID WINAPI SaveStubFunc( LPVOID lpOrnFunc, LPVOID lpJumpFunc, ULONG SkipCount )
{
	WORD wCodeSel;
	WORD wDataSel;
	LPSTR lpCode;
	wCodeSel = HIWORD((LONG)lpJumpFunc);
	wDataSel = AllocSelector(wCodeSel);
	PrestoChangoSelector(wCodeSel, wDataSel);
	lpCode = MK_FP( wDataSel, LOWORD((LONG)lpJumpFunc));
	memmove( lpOrnFunc, lpCode, SkipCount );
	lpOrnFunc = (LPVOID)((LONG)lpOrnFunc + SkipCount);
	lpCode = lpCode + SkipCount;
	*lpCode = 0xEA; // jmp instruction
	*(lpCode+1) = LOBYTE(LOWORD((LONG)lpOrnFunc));
	*(lpCode+2) = HIBYTE(LOWORD((LONG)lpOrnFunc));
	*(lpCode+3) = LOBYTE(HIWORD((LONG)lpOrnFunc));
	*(lpCode+4) = HIBYTE(HIWORD((LONG)lpOrnFunc));
	FreeSelector(wDataSel);
}
#endif //_WIN32

PUBLIC VOID WINAPI HookAPI_Init( LPHOOKAPI_INFO lpHookInfo, void* lpOrnFunc, void* lpNewFunc )
{

	LONG JumpOffset;
	memset( lpHookInfo, 0, sizeof(HOOKAPI_INFO));
	lpHookInfo->lpOrnFunc = lpOrnFunc;
	lpHookInfo->lpNewFunc = lpNewFunc;

#ifdef _WIN32

	JumpOffset = (LONG)lpNewFunc - (LONG)lpOrnFunc - 6 + 1;
	lpHookInfo->WriteBuf[0] = 0xe9;
	lpHookInfo->WriteBuf[1] = LOBYTE(LOWORD(JumpOffset));
	lpHookInfo->WriteBuf[2] = HIBYTE(LOWORD(JumpOffset));
	lpHookInfo->WriteBuf[3] = LOBYTE(HIWORD(JumpOffset));
	lpHookInfo->WriteBuf[4] = HIBYTE(HIWORD(JumpOffset));

#else //_WIN32

	lpHookInfo->WriteBuf[0] = 0xea;
	lpHookInfo->WriteBuf[1] = LOBYTE(LOWORD((LONG)lpNewFunc));
	lpHookInfo->WriteBuf[2] = HIBYTE(LOWORD((LONG)lpNewFunc));
	lpHookInfo->WriteBuf[3] = LOBYTE(HIWORD((LONG)lpNewFunc));
	lpHookInfo->WriteBuf[4] = HIBYTE(HIWORD((LONG)lpNewFunc));

#endif //_WIN32

	lpHookInfo->SkipCount = 0;
}

PUBLIC VOID WINAPI HookAPI_Enable( LPHOOKAPI_INFO lpHookInfo )
{
	if( lpHookInfo->lpOrnFunc == NULL )
		return;
	// 保存要修改代码的原有数据
	memmove( lpHookInfo->OrnBuf, lpHookInfo->lpOrnFunc, 5 );

	// 确定调用函数需要在第几个字节位置跳转
	LPBYTE lpSrc;
	LPBYTE lpStubBuf = (LPBYTE)lpHookInfo->StubBuf;
	LPBYTE lpOrnFunc = (LPBYTE)lpHookInfo->lpOrnFunc;

#ifdef WIN32
	int AllCount = 0;
	while ( true ) {
		int InsCount;
		lpSrc = DetourCopyInstruction( lpStubBuf, lpOrnFunc, NULL );
		InsCount = lpSrc - lpOrnFunc;
		lpStubBuf += InsCount;
		lpOrnFunc += InsCount;
		AllCount += InsCount;
		if ( AllCount >= 5 )
			break;
	}
	lpHookInfo->SkipCount = AllCount;
	
#else
	
	AllCount = lpHookInfo->SkipCount;
#endif
	
	// 填写跳转代码
	LONG JumpOffset;
	JumpOffset = (LONG)lpHookInfo->lpOrnFunc + AllCount - (LONG)lpStubBuf - 6 + 1;
	PROCWRITE WriteBuf;
	WriteBuf[0] = 0xe9;
	WriteBuf[1] = LOBYTE((LOWORD(JumpOffset)));
	WriteBuf[2] = HIBYTE((LOWORD(JumpOffset)));
	WriteBuf[3] = LOBYTE((HIWORD(JumpOffset)));
	WriteBuf[4] = HIBYTE((HIWORD(JumpOffset)));
	Power_CopyHighMem( lpStubBuf, WriteBuf, 5 );

	Power_CopyHighMem( lpHookInfo->lpOrnFunc, lpHookInfo->WriteBuf, 5 );
}

PUBLIC VOID WINAPI HookAPI_Disable( LPHOOKAPI_INFO lpHookInfo )
{
	if( lpHookInfo->lpOrnFunc && lpHookInfo->OrnBuf )
		Power_CopyHighMem( lpHookInfo->lpOrnFunc, lpHookInfo->OrnBuf, 5 );
}

PUBLIC VOID WINAPI HookAPI_InitModule( LPHOOKAPI_INFO lpHookInfo, LPCTSTR lpszModuleName, 
									  LPCSTR lpszProcName, LPVOID lpNewFunc )
{
	HINSTANCE hModule;
	LPVOID lpApiFunc;
	
	hModule = GetModuleHandle( lpszModuleName );
	if ( hModule == NULL )
		return;
	lpApiFunc = GetProcAddress( hModule, lpszProcName );
	if ( lpApiFunc == NULL )
		return;
	HookAPI_Init( lpHookInfo, lpApiFunc, lpNewFunc );
}

#ifdef _WIN32

#else //_WIN32
LPVOID GetAPIProc( LPCSTR lpszModName, LPCSTR lpszAPIName )
{
	HANDLE hModuleGDI;
	hModuleGDI = LoadLibrary( lpszModName );
	if ( hModuleGDI > HINSTANCE_ERROR ) {
		LPVOID lp;
		lp = GetProcAddress( hModuleGDI, lpszAPIName );
		FreeLibrary( hModuleGDI );
    } else
		return (NULL);
}

VOID ModifyAPIProc( LPVOID lpOrnProc, LPVOID lpNewProc, PROCWRITE& OrnProcBuf )
{
	WORD wCodeSel;
	WORD wDataSel;
	LPSTR lpCode;
	PROCWRITE NewBuf;
	LPSTR lpop = (LPSTR)lpOrnProc;
	OrnProcBuf[0] = *lpop;
	OrnProcBuf[1] = *(lpop+1);
	OrnProcBuf[2] = *(lpop+2);
	OrnProcBuf[3] = *(lpop+3);
	OrnProcBuf[4] = *(lpop+4);
	NewBuf[0] = 0xea;
	NewBuf[1] = LOBYTE(LOWORD((LONG)lpNewProc));
	NewBuf[2] = HIBYTE(LOWORD((LONG)lpNewProc));
	NewBuf[3] = LOBYTE(HIWORD((LONG)lpNewProc));
	NewBuf[4] = HIBYTE(HIWORD((LONG)lpNewProc));
	wCodeSel = HIWORD((LONG)lpOrnProc);
	wDataSel = AllocSelector( wCodeSel );
	PrestoChangoSelector( wCodeSel, wDataSel );
	wCodePtr = MK_FP( wDataSel, LOWORD((LONG)lpOrnProc));
	wCodePtr[0] = NewBuf[0];
	wCodePtr[1] = NewBuf[1];
	wCodePtr[2] = NewBuf[2];
	wCodePtr[3] = NewBuf[3];
	wCodePtr[4] = NewBuf[4];
	FreeSelector( wDataSel );
}

VOID RestoreAPIProc( LPVOID lpOrnProc, PROCWRITE& OrnProcBuf )
{
	WORD wCodeSel;
	WORD wDataSel;
	LPSTR lpCode;
	
	wCodeSel = HIWORD((LONG)lpOrnProc);
	wDataSel = AllocSelector( wCodeSel );
	PrestoChangoSelector( wCodeSel, wDataSel );
	lpCode = MK_FP( wDataSel, LOWORD((LONG)lpOrnProc));
	lpCode[0] = OrnProcBuf[0];
	lpCode[1] = OrnProcBuf[1];
	lpCode[2] = OrnProcBuf[2];
	lpCode[3] = OrnProcBuf[3];
	lpCode[4] = OrnProcBuf[4];
	FreeSelector( wDataSel );
}

WORD MyAllocSelector( INT Num )
{
	_asm {
		xor ax, ax
		mov cx, Num
		int 31h
	}
}

VOID MyFreeSelector( WORD Sel )
{
	_asm {
		mov ax, 1
		mov bx, Sel
		int 31h
	}
}

VOID MySetSelectorBase( WORD Sel, LONG BaseLiner )
{
	_asm {
		mov ax, 7
		mov bx, Sel
		mov dx, word ptr [BaseLiner]
		mov cx, word ptr [BaseLiner+2]
		int 31h
	}
}

VOID MySetSelectorLimit( WORD Sel, LONG Limit )
{
	_asm {
		mov ax, 8
		mov bx, Sel
		mov dx, word ptr [Limit]
		mov cx, word ptr [Limit+2]
		int 31h
	}
}

#endif










