//
// isysfunc.h
//

#ifndef __SYSFUNC_H__
#define __SYSFUNC_H__


#include <windows.h>
#include "sysdef.h"


typedef unsigned long SYSRESULT;


#define SYSFUNC_API

//#ifdef SYSFUNC_EXPORTS
//#define SYSFUNC_API __declspec(dllexport)
//#else
//#define SYSFUNC_API __declspec(dllimport)
//#endif

// myself defined variable style

#ifndef __cplusplus

typedef unsigned int bool;
#define true -1
#define false 0

#endif // bool

typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned char byte;
typedef unsigned long dword;
typedef unsigned short word;

#ifndef null
#define null 0
#endif



/*******************************************************************************
用于不同的编译器的设置定义
*******************************************************************************/

#ifdef _MSC_VER

// 用于Microsoft Visual C++ 6.0
// Unicode比较函数
#define WCSICMP(s1,s2) wcsicmp(s1,s2)
#define WCSNICMP(s1,s2,c) wcsnicmp(s1,s2,c)

#define ASM _asm

#else

// 用于Borland C++ Builder
#define WCSICMP(s1,s2) wcscmpi(s1,s2)
#define WCSNICMP(s1,s2,c) wcsncmpi(s1,s2,c)

#define ASM asm

#endif

// 用于调试定断点
#ifdef _SYSTEM_WIN9X

#define DEBUG_BREAK() ASM out 0ffh, al

#else

#define DEBUG_BREAK() MessageBeep(0)

#endif //_SYSTEM_WIN9X

#ifndef __cplusplus

typedef unsigned int bool;
#define true -1
#define false 0

#endif // myself define symbol




// enum all windows callback function
typedef BOOL ( CALLBACK * WALKAROUND_WINDOWPROC ) ( HWND hWnd, LPARAM lParam );




#ifdef __cplusplus
extern "C" {
#endif

VOID WINAPI InitSystemInfo();
SYSTEM_VER WINAPI GetSystemVer();
VOID WINAPI GetWorkPath( HINSTANCE h, LPSTR lpszWorkPath );
#ifdef _WIN32
INT WINAPI GetProcessCount();
#endif //_WIN32
HANDLE WINAPI Power_GetWindowTask( HWND hWnd );
HANDLE WINAPI Power_GetCurrentTask();
BOOL WINAPI Power_GetProcessName( HANDLE hTask, LPSTR lpszProcName );
BOOL WINAPI Power_GetModuleScope( HANDLE hTask, LPCSTR lpszModuleName, LPVOID& lpAddr, ULONG& uSize );
FARPROC WINAPI LoadDllFunc( HINSTANCE hInst, LPCSTR lpszProcName );
LPVOID WINAPI AddFarPtr( LPVOID lpData, ULONG Len );
HWND WINAPI GetParentWnd( HWND hWnd );
VOID WINAPI CopyFixLenText( LPCSTR lpszSrc, LPSTR lpszDest, INT Len );
VOID WINAPI MyTrim( LPCSTR lpszSrc, LPSTR lpszDest );
INT WINAPI CodeFunc_GetWideTextLen( LPCWSTR lpwSrc );
VOID WINAPI CodeFunc_ConvertCode( CONVCODE_TYPE SrcType, LPVOID lpSrc, CONVCODE_TYPE DestType, LPVOID lpDest );
BOOL WINAPI IsAscText( LPCSTR lpSrc, INT Len );
// Output debug information, maybe file or window
VOID WINAPI WriteDbgInfo( DBGINFO_TYPE InfoType, LPCSTR lpszInfo );
VOID WINAPI InitDbgInfo();
// system initialize
VOID WINAPI ComSysInit();
VOID WINAPI ExtractFileName( LPCSTR lpszFullName, LPSTR lpszFileName );
VOID WINAPI ExtractFileExt( LPCSTR lpszFullName, LPSTR lpszExtName );
VOID WINAPI ChangeFileExt( LPCSTR lpszSrcFileName, LPSTR lpszDestFileName, LPCSTR lpszExtName );
//BOOL WINAPI FileExists( LPCSTR fn );
#ifdef _WIN32
VOID WINAPI SendKey( WORD Code, BOOL IsDown );
#endif
HWND WINAPI FindIEControl( HWND hWnd, BOOL IsIncludeChild );
VOID WINAPI ProcessMessages();
BOOL WINAPI WaitToContinue( LPDWORD lpWaitFlag, DWORD TimeCount );
HWND WINAPI FindSpecialControl( HWND hWnd, BOOL IsIncludeChild, LPCSTR lpszClassName );
BOOL WINAPI GetPartText( LPSTR lpszString, LPSTR lpParts[], INT& nPartCount );
BOOL WINAPI WalkAroundWindow( HWND hTopWnd, WALKAROUND_WINDOWPROC lpWalkProc, LPARAM lParam );



INT StrToHex( LPCSTR lpszHex );
VOID HexToStr( INT nHexValue, LPSTR lpszHex, INT nBytes );
bool ReplaceHighText( char* Src );

#ifdef __cplusplus
}
#endif



#endif //__SYSFUNC_H__








