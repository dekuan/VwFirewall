//
// HookCode.h
//
#ifndef __HOOKCODE_H__
#define __HOOKCODE_H__


#include "SysFunc.h"

typedef BYTE PROCWRITE[5];
typedef PROCWRITE* LPPROCWRITE;

typedef struct _HOOKAPI_INFO {
	ULONG Size;
	LPVOID lpOrnFunc;
	BYTE OrnBuf[16];
	BYTE WriteBuf[16];
	LPVOID lpNewFunc;
	LPVOID lpStubFunc;
	ULONG SkipCount;
	BYTE StubBuf[256];
} HOOKAPI_INFO, FAR* LPHOOKAPI_INFO;

// 搜索内存字节的结构
typedef struct _CODE_SEARCH_ITEM {
	int Len; // 要搜索的字节的个数
	BYTE Buf[16]; // 搜索最长不超过32个字节
} CODE_SEARCH_ITEM, FAR* LPCODE_SEARCH_ITEM;

#define CALLORNFUNC(phai) (&phai->StubBuf[0])

// 存放寄存器的结构
typedef struct _HAI_REGISTER {
	DWORD r_eax;
	DWORD r_ebx;
	DWORD r_ecx;
	DWORD r_edx;
	DWORD r_esi;
	DWORD r_edi;
} HAI_REGISTER, FAR* LPHAI_REGISTER;

#ifdef _MSC_VER

// 用于Visual C++6.0
#define HAI_SAVEREG(s) \
	_asm mov dword ptr s.r_eax, eax \
	_asm mov dword ptr s.r_ebx, ebx \
	_asm mov dword ptr s.r_ecx, ecx \
	_asm mov dword ptr s.r_edx, edx \
	_asm mov dword ptr s.r_esi, esi \
	_asm mov dword ptr s.r_edi, edi \

#define HAI_RESTOREREG(s) \
	_asm mov eax, dword ptr s.r_eax \
	_asm mov ebx, dword ptr s.r_ebx \
	_asm mov ecx, dword ptr s.r_ecx \
	_asm mov edx, dword ptr s.r_edx \
	_asm mov esi, dword ptr s.r_esi \
	_asm mov edi, dword ptr s.r_edi \

#else

// 用于C++Builder
#define HAI_SAVEREG(s) \
	_asm mov dword ptr s.r_eax, eax; \
	_asm mov dword ptr s.r_ebx, ebx; \
	_asm mov dword ptr s.r_ecx, ecx; \
	_asm mov dword ptr s.r_edx, edx; \
	_asm mov dword ptr s.r_esi, esi; \
	_asm mov dword ptr s.r_edi, edi; \

#define HAI_RESTOREREG(s) \
	_asm mov eax, dword ptr s.r_eax; \
	_asm mov ebx, dword ptr s.r_ebx; \
	_asm mov ecx, dword ptr s.r_ecx; \
	_asm mov edx, dword ptr s.r_edx; \
	_asm mov esi, dword ptr s.r_esi; \
	_asm mov edi, dword ptr s.r_edi; \

#endif //_MSC_VER



#ifdef __cplusplus
extern "C" {
#endif

PUBLIC VOID WINAPI HookAPI_Init( LPHOOKAPI_INFO lpHookInfo, void* lpOrnFunc, void* lpNewFunc );
PUBLIC VOID WINAPI HookAPI_Enable( LPHOOKAPI_INFO lpHookInfo );
PUBLIC VOID WINAPI HookAPI_Disable( LPHOOKAPI_INFO lpHookInfo );
PUBLIC VOID WINAPI HookAPI_InitModule( LPHOOKAPI_INFO lpHookInfo, LPCTSTR lpszModuleName, LPCSTR lpszProcName, LPVOID lpNewFunc);

PUBLIC ULONG WINAPI SearchInnerFunc( LPVOID lpStart, ULONG StartPos, ULONG EndPos, LPBYTE lpIDs, ULONG Count );
PUBLIC VOID WINAPI SaveStubFunc( LPVOID lpOrnFunc, LPVOID lpJumpFunc, ULONG SkipCount );

PUBLIC ULONG WINAPI SearchInnerFuncEx( LPVOID lpStart, ULONG nStartPos, ULONG nEndPos, LPCODE_SEARCH_ITEM lpIDItems, ULONG Count );

		  
#ifdef __cplusplus
}
#endif



#endif //__HOOKCODE_H__









