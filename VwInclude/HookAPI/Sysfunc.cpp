// sysfunc.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "sysfunc.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <tlhelp32.h>
#endif


#define DEF_DBGINFO_FILENAME "c:\\DfDgInfo.txt"                              //LSM{1,409}
#define DEF_DBGWND_CLASSNAME "TFormMultiDebug"                               //LSM{17,409}

#ifndef min
#define min(a, b)  (((a) < (b)) ? (a) : (b))  
#endif

#ifndef max
#define max(a, b)  (((a) > (b)) ? (a) : (b))  
#endif



SYSTEM_VER g_systemver = svWin9x;
CHAR g_DbgInfoFileName[256] = DEF_DBGINFO_FILENAME;
BOOL g_DbgCanUsed = FALSE; // can use debug mode
BOOL g_DbgIsFirst = TRUE; // Was it called debug function first?
HWND g_DbgInfoWnd = NULL; // Window handle receive debug information



VOID WINAPI ComSysInit()
{
	InitSystemInfo();
}


VOID WINAPI GetWorkPath( HINSTANCE h, LPTSTR lpszWorkPath )
{
	GetModuleFileName( h, lpszWorkPath, 255 );
	TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
	_tsplitpath( lpszWorkPath, drive, dir, fname, ext );
	_tmakepath( lpszWorkPath, drive, dir, _T(""), _T("") );                           //LSM{33,409}{49,409}
}

VOID WINAPI WriteDbgInfo( DBGINFO_TYPE InfoType, LPCSTR lpszInfo )
{
}

VOID WINAPI InitDbgInfo()
{
  strcpy( g_DbgInfoFileName, DEF_DBGINFO_FILENAME );
  g_DbgCanUsed = FALSE;
  g_DbgIsFirst = TRUE;
  g_DbgInfoWnd = NULL;
}

SYSTEM_VER WINAPI GetSystemVer()
{
	return ( g_systemver );
}

VOID WINAPI InitSystemInfo()
{
	LONG v;
	v = GetVersion();
	BYTE Major, Minor;
	Major = LOBYTE( LOWORD( v ) );
	Minor = HIBYTE( LOWORD( v ) );
	if ( Major >= 5 ) {
		g_systemver = svWin2000;
	} else if ( Major == 4 ) {
		if ( Minor == 0 )
			g_systemver = svWinNT;
		else
			g_systemver = svWin9x;
	} else {
		g_systemver = svWin3x;
	}
}

#ifdef _WIN32
LPVOID WINAPI AddFarPtr( LPVOID lpData, ULONG Len )
{
	return (LPVOID)( (ULONG)lpData + Len );
}
#else //_WIN32
LPVOID WINAPI AddFarPtr( LPVOID lpData, ULONG Len )
{
	LPVOID lpr;
	_asm {
		mov ax,word ptr [Len]
		mov dx,word ptr [Len+2]
		add ax,word ptr [lpData]
		adc dx,0
		shl dx,3
		add dx,word ptr [lpData+2]
		mov word ptr [lpr], ax
		mov word ptr [lpr+2], dx
	}
	return ( lpr );
}
#endif //_WIN32

HANDLE WINAPI Power_GetWindowTask( HWND hWnd )
{
	HANDLE hr;
	if ( IsWindow( hWnd ) )
#ifdef _WIN32
		GetWindowThreadProcessId( hWnd, (LPDWORD)&hr );
#else //_WIN32
		hr = GetWindowTask( hWnd );
#endif
	else
		hr = Power_GetCurrentTask();

	return ( hr );
}

HANDLE WINAPI Power_GetCurrentTask()
{
	HANDLE hr;
#ifdef _WIN32
	hr = (HANDLE)GetCurrentProcessId();
#else //_WIN32
	hr = GetCurrentTask();
#endif //_WIN32
	return ( hr );
}

FARPROC WINAPI LoadDllFunc( HINSTANCE hInst, LPCSTR lpszProcName )
{
	if ( hInst == NULL )
		return ( NULL );
	FARPROC fp;
	fp = GetProcAddress( hInst, lpszProcName );
	if ( fp == NULL ) {
		TCHAR Buf[256];
		_sntprintf( Buf, sizeof(Buf)/sizeof(TCHAR)-1, _T("Cannot import function:%s"), lpszProcName );           //LSM{65,409}
		MessageBox( NULL, Buf, _T("Import Error"), MB_OK || MB_ICONINFORMATION );//LSM{81,409}
	}
	return ( fp );
}

#ifdef _WIN32
INT WINAPI GetProcessCount()
{
	HANDLE hSnapShot;
	PROCESSENTRY32 pe;
	BOOL ok;
	INT Count = 0;
	hSnapShot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	pe.dwSize = sizeof( PROCESSENTRY32 );
	ok = Process32First( hSnapShot, &pe );
	while ( ok ) {
		Count++;
		ok = Process32Next( hSnapShot, &pe );
	}
	CloseHandle( hSnapShot );
	return ( Count );
}
#endif //_WIN32

HWND WINAPI GetParentWnd( HWND hWnd )
{
	HWND pw = hWnd;
	while ( hWnd != NULL ) {
		pw = hWnd;
		hWnd = GetParent( pw );
		//hWnd = GetWindow( pw, GW_OWNER);
	}
	return ( pw );
}

VOID WINAPI CopyFixLenText( LPCSTR lpszSrc, LPSTR lpszDest, INT Len )
{
	INT rlen;
	rlen = min( (INT)strlen(lpszSrc), Len );
	if ( rlen <= 0 )
		return;
	strncpy( lpszDest, lpszSrc, rlen );
	lpszDest[rlen] = 0;
}

VOID WINAPI MyTrim( LPCSTR lpszSrc, LPSTR lpszDest )
{
	INT len;
	INT lpos, rpos;
	len = strlen( lpszSrc );
	lpos = 0;
	while ( lpos < len ) {
		if ( lpszSrc[lpos] != ' ' )
			break;
		lpos++;
	}
	rpos = len-1;
	while ( rpos > 0 ) {
		if ( lpszSrc[rpos] != ' ' )
			break;
		rpos--;
	}
	lpszDest[0] = 0;
	if ( rpos >= lpos ) {
		int NewLen = rpos-lpos+1;
		memmove( lpszDest, lpszSrc+lpos, NewLen );
		lpszDest[NewLen] = 0;
	}
}

INT WINAPI CodeFunc_GetWideTextLen( LPCWSTR lpwSrc )
{
	INT len = 0;
	while ( TRUE ) {
		if ( lpwSrc[0] == 0 )
			break;
		lpwSrc++;
		len++;
	}
	return ( len );
}

VOID WINAPI CodeFunc_AnsiToUnicode( LPCSTR lpszSrc, LPWSTR lpwDest )
{
	INT len;
	len = strlen( lpszSrc );
#ifdef _WIN32
	len = MultiByteToWideChar( CP_ACP, 0, lpszSrc, len, lpwDest, len * 2 );
	lpwDest[len] = 0;
#else //_WIN
	memmove( lpwDest, lpszSrc, len );
	lpwDest[len] = 0;
#endif //_WIN32
}

VOID WINAPI CodeFunc_UnicodeToAnsi( LPCWSTR lpwSrc, LPSTR lpszDest )
{
	INT len;
	len = CodeFunc_GetWideTextLen( lpwSrc );
#ifdef _WIN32
	len = WideCharToMultiByte( CP_ACP, 0, lpwSrc, len, lpszDest, len * 2, NULL, NULL );
	lpszDest[len] = 0;
#else //_WIN
	memmove( lpszDest, lpwSrc, len );
	lpszDest[len] = 0;
#endif //_WIN32
}

VOID WINAPI CodeFunc_ConvertCode( CONVCODE_TYPE SrcType, LPVOID lpSrc, CONVCODE_TYPE DestType, LPVOID lpDest )
{
	if ( ( SrcType == cctAnsi ) && ( DestType == cctUnicode ) )
		CodeFunc_AnsiToUnicode( (LPCSTR)lpSrc, (LPWSTR)lpDest );
	else if ( ( SrcType == cctUnicode ) && ( DestType == cctAnsi ) )
		CodeFunc_UnicodeToAnsi( (LPCWSTR)lpSrc, (LPSTR)lpDest );
}

BOOL WINAPI IsAscText( LPCSTR lpSrc, INT len )
{
	UINT ch;
	while ( len > 0 ) {
		ch = *(LPBYTE)lpSrc;
		if ( ch == 0 )
			break;
		lpSrc++;
		len--;
		if ( ( ch >= 0x81 ) && ( ch <= 0xfe ) ) {
			ch = *(LPBYTE)lpSrc;
			if ( ( ch >= 0x40 ) && ( ch <= 0xfe ) )
				// It's Chinese text.
				return ( FALSE );
		}
	}
	return ( TRUE );
}

VOID WINAPI ExtractFileName( LPCSTR lpszFullName, LPSTR lpszFileName )
{
	CHAR drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
	_splitpath( lpszFullName, drive, dir, fname, ext );
	_makepath( lpszFileName, "", "", fname, ext );                           //LSM{97,409}{113,409}
}

VOID WINAPI ExtractFileExt( LPCSTR lpszFullName, LPSTR lpszExtName )
{
	CHAR drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME];
	_splitpath( lpszFullName, drive, dir, fname, lpszExtName );
}

VOID WINAPI ChangeFileExt( LPCSTR lpszSrcFileName, LPSTR lpszDestFileName, LPCSTR lpszExtName )
{
	CHAR drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
	_splitpath( lpszSrcFileName, drive, dir, fname, ext );
	_makepath( lpszDestFileName, drive, dir, fname, lpszExtName );
}

BOOL WINAPI Power_GetProcessName( HANDLE hTask, LPTSTR lpszProcName )
{
#ifdef _WIN32
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hSnap;
	hSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	BOOL ok, IsFind = FALSE;
	ok = Process32First( hSnap, &pe );
	while ( ok ) {
		if ( pe.th32ProcessID == (DWORD)hTask ) {
			IsFind = TRUE;
			break;
		}
		ok = Process32Next( hSnap, &pe );
	}
	CloseHandle( hSnap );
	if ( IsFind ) {
		_tcscpy( lpszProcName, pe.szExeFile );
		return ( TRUE );
	} else {
		lpszProcName[0] = 0;
		return ( FALSE );
	}

#else //_WIN32

	lpszProcName[0] = 0;
	return ( FALSE );
#endif
}

BOOL WINAPI Power_GetModuleScope( HANDLE hTask, LPCTSTR lpszModuleName, LPVOID& lpAddr, ULONG& uSize )
{
#ifdef _WIN32

	HANDLE hSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, (DWORD)hTask );
	MODULEENTRY32 me;
	me.dwSize = sizeof(MODULEENTRY32);
	BOOL ok, IsFind = FALSE;
	ok = Module32First( hSnap, &me );
	while ( ok ) {
		if ( _tcsicmp( lpszModuleName, me.szModule ) == 0 ) {
			
			//CHAR Msg[256];
			//sprintf( Msg, "base:%x, size:%x", me.modBaseAddr, me.modBaseSize );
			//MessageBox( NULL, Msg, "", MB_OK );

			lpAddr = (LPVOID)me.modBaseAddr;
			uSize = me.modBaseSize;
			
			IsFind = TRUE;
			break;
		}
		ok = Module32Next( hSnap, &me );
	}
	CloseHandle( hSnap );
	return ( IsFind );

#else //_WIN32

	return ( TRUE );
#endif
}

/*
BOOL WINAPI FileExists( LPCSTR fn )
{
	//HANDLE f = CreateFile( fn, 0, 0, NULL, OPEN_EXISTING, 0, NULL );
	//if ( f == INVALID_HANDLE_VALUE )
	//	return ( FALSE );
	//CloseHandle( f );
	
	CHAR szFileName[512];
	strcpy( szFileName, fn );
	HFILE f = _lopen( szFileName, OF_READ | OF_SHARE_DENY_NONE );
	if ( f == HFILE_ERROR )
		return ( FALSE );
	_lclose(f);
	return ( TRUE );
}
*/
#ifdef _WIN32
VOID WINAPI SendKey( WORD Code, BOOL IsDown )
{
	// 仿真按键盘按钮!
	WORD okc;
	DWORD Flag = 0;
	if ( !IsDown )
		Flag = KEYEVENTF_KEYUP;
	okc = MapVirtualKey( Code, 0 );
	keybd_event( (BYTE)Code, (BYTE)okc, Flag, 0 ); 
}
#endif

HWND WINAPI FindSpecialControl( HWND hWnd, BOOL IsIncludeChild, LPCTSTR lpszClassName )
{
	// 找到窗口句柄
	// 从自身窗口开始
	if ( hWnd == NULL )
		return ( NULL );
	if ( IsIncludeChild ) {
		hWnd = GetWindow( hWnd, GW_HWNDFIRST );

		while ( hWnd != NULL ) {
			TCHAR ClassName[256];
			GetClassName( hWnd, ClassName, 255 );
			if ( _tcsicmp( ClassName, lpszClassName ) == 0 ) {
				// 找到了 IE 控件
				return ( hWnd );
			}
			HWND ChildWnd;
			ChildWnd = GetWindow( hWnd, GW_CHILD );
			if ( ChildWnd != NULL ) {
				ChildWnd = FindSpecialControl( ChildWnd, IsIncludeChild, lpszClassName );
				if ( ChildWnd != NULL ) {
					// 找到了子控件
					return ( ChildWnd );
				}
			}
			// 找下一个
			hWnd = GetWindow( hWnd, GW_HWNDNEXT );
		}
		// 一直没找到
	} else {

		// 只要判断当前窗口就行了
		TCHAR ClassName[256];
		GetClassName( hWnd, ClassName, 255 );
		if ( _tcsicmp( ClassName, lpszClassName ) == 0 ) {
			// 找到了 IE 控件
			return ( hWnd );
		}
	}
	return ( NULL );
}

HWND WINAPI FindIEControl( HWND hWnd, BOOL IsIncludeChild )
{
	// 找到IE控件的窗口句柄
	// 从自身窗口开始
	if ( hWnd == NULL )
		return ( NULL );
	if ( IsIncludeChild ) {
		hWnd = GetWindow( hWnd, GW_HWNDFIRST );

		while ( hWnd != NULL ) {
			TCHAR ClassName[256];
			GetClassName( hWnd, ClassName, 255 );
			if ( _tcsicmp( ClassName, _T("Internet Explorer_Server") ) == 0 ) {   //LSM{177,409}
				// 找到了 IE 控件
				return ( hWnd );
			}
			HWND ChildWnd;
			ChildWnd = GetWindow( hWnd, GW_CHILD );
			if ( ChildWnd != NULL ) {
				ChildWnd = FindIEControl( ChildWnd, IsIncludeChild );
				if ( ChildWnd != NULL ) {
					// 找到了子控件
					return ( ChildWnd );
				}
			}
			// 找下一个
			hWnd = GetWindow( hWnd, GW_HWNDNEXT );
		}
		// 一直没找到
	} else {

		// 只要判断当前窗口就行了
		TCHAR ClassName[256];
		GetClassName( hWnd, ClassName, 255 );
		if ( _tcsicmp( ClassName, _T("Internet Explorer_Server") ) == 0 ) {       //LSM{193,409}
			// 找到了 IE 控件
			return ( hWnd );
		}
	}
	return ( NULL );
}


BOOL ProcessMessage( MSG& MyMsg )
{
	BOOL bResult = FALSE;
	//BOOL Handled;
	if ( PeekMessage( &MyMsg, 0, 0, 0, PM_REMOVE) ) {
		bResult = TRUE;
		if (MyMsg.message != WM_QUIT ) {
			//Handled = FALSE;
			TranslateMessage(&MyMsg);
			DispatchMessage(&MyMsg);
		} else
			bResult = FALSE;
	}
	return ( bResult );
}

VOID WINAPI ProcessMessages()
{
	// 等特系统可用的消息
	MSG MyMsg;
	while ( ProcessMessage( MyMsg ) ) {
		// 等着，什么也不干
	}
}

BOOL WINAPI WaitToContinue( LPDWORD lpWaitFlag, DWORD TimeCount )
{
	// 根据一个标志等待，直到标志有效或者超时
	DWORD TickCount = GetTickCount();
	while ( ( GetTickCount() - TickCount ) < TimeCount ) {
		if ( !(*lpWaitFlag) )
			return ( TRUE );
		ProcessMessages();
	}
	// 等了足够长的时间了，该退出了
	// 是不是要将标志设为FALSE呢？
	// 还是不要设的好
	
	// 现在已经超时了
	return ( FALSE );
}

BOOL WINAPI GetPartText( LPSTR lpszString, LPSTR lpParts[], INT& nPartCount )
{
    // 得到一个用;分隔的文本的多个片段
	INT nLen = strlen( lpszString );
	if ( nLen <= 0 ) {
		return ( FALSE );
	}
	nPartCount = 0;
	LPSTR lpGapPtr = lpszString;
	lpParts[0] = lpGapPtr;
	nPartCount = 1;
	while ( nLen > 0 ) {
		CHAR ch = *lpGapPtr;
		if ( ch == ';' ) {
			*lpGapPtr = 0; // 将当前;写为空，这样可以有一个独立的字符串
			lpGapPtr++;
			nLen--;
			lpParts[nPartCount] = lpGapPtr;
			nPartCount++;
		} else {
			lpGapPtr++;
			nLen--;
		}
	}
	return ( TRUE );
}

BOOL WalkChildWindow( HWND hTopWnd, WALKAROUND_WINDOWPROC lpWalkProc, LPARAM lParam )
{
	HWND hWnd = GetWindow( hTopWnd, GW_CHILD );
	hWnd = GetWindow( hWnd, GW_HWNDFIRST );
	while ( hWnd != NULL ) {
		if ( !lpWalkProc( hWnd, lParam ) )
			return ( FALSE );
		if ( !WalkChildWindow( hWnd, lpWalkProc, lParam ) )
			return ( FALSE );
		hWnd = GetWindow( hWnd, GW_HWNDNEXT );
	}
	return ( TRUE );
}

BOOL WINAPI WalkAroundWindow( HWND hTopWnd, WALKAROUND_WINDOWPROC lpWalkProc, LPARAM lParam )
{
	// walk around all windows in system
	if ( lpWalkProc == NULL )
		return ( FALSE );
	// get system top window 
	WalkChildWindow( hTopWnd, lpWalkProc, lParam );
	return ( TRUE );
}


INT StrToHex( LPCSTR lpszHex )
{
	// 将十六进制数字符串转换到变量中
	
	CHAR szValidHex[256];
	LPSTR lpszValidHex = &szValidHex[0];
	
	// 得到有效的十六进制数
	
	while ( TRUE ) {
		INT ch = *(BYTE*)lpszHex++;
		if ( ch == 0 )
			break;
		ch = toupper(ch);
		if ( ( ( ch >= '0' ) && ( ch <= '9' ) ) || 
			( ( ch >= 'A' ) && ( ch <= 'F' ) ) ) {
	
			// 这个字符算是十六进制数
			
			*lpszValidHex++ = ch;
		}
	}
	*lpszValidHex = 0;

	// 现在可以转换到变量中了
	// 转换方法是从后往前转换字符，然后换算到变量中

	lpszValidHex = &szValidHex[0];
	INT nHexValue = 0; // 存放转换出的十六进制数
	INT nHexLen = strlen( lpszValidHex )-1;
	INT nMultiple = 0;
	while ( nHexLen >= 0 ) {
		INT ch = *(BYTE*)(lpszValidHex+nHexLen);
		if ( ( ch >= '0' ) && ( ch <= '9' ) ) {
			ch = ch - '0';
		} else {
			
			// 这是A-F

			ch = ch - 'A';
		}
		INT nMultiValue = 1;
		for ( INT i = 0; i < nMultiple; i++ )
			nMultiValue *= 16;
		nHexValue += ch * nMultiValue;
		nHexLen--;
		nMultiple++;
	}
	return ( nHexValue );
}

VOID HexToStr( INT nHexValue, LPSTR lpszHex, INT nBytes )
{
	// 将变量转换为16进制数的字符串，nBytes是字符个数
	// 数字前面的不满的用0填充

	INT nMultiple = nBytes;
	while ( nMultiple >= 0 ) {
		DWORD nMultiValue = 1;
		for ( INT i = 0; i < nMultiple; i++ )
			nMultiValue *= 16;
		INT ch = ( nHexValue / nMultiValue ) & 0x0f;
		if ( ch <= 9 )
			ch = ch + '0';
		else
			ch = ch - 0x0a + 'a';
		*lpszHex++ = ch;
		nMultiple--;
	}
	*lpszHex = 0;
}

char* g_RepCharset = 
	//0123456789ABCDEF0123456789ABCDEF
	"  ,f. ^|^% <        *--~  >     " 
	//0123456789ABCDEF0123456789ABCDEF
	"    & |$   <-- - +              "
	//0123456789ABCDEF0123456789ABCDEF
	"                       x       B"
	//0123456789ABCDEF0123456789ABCDEF
	"                       /        ";



bool ReplaceHighText( char* Src )
{
	int i;
  int TCCount;
  int iLeftCount;
  int iRightCount;
  int iLastPos;
  int len;
	
  len = strlen( Src );
	if ( len < 4 ) {
		return ( false );
	}
	TCCount = 0;
	for ( i = 0; i < len; i++ ) {
    if ( (BYTE)Src[i] > (BYTE)'~' ) {
      TCCount = TCCount + 1;
      if ( i > 0  ) {
        if ( (BYTE)Src[i - 1] > (BYTE)'~' ) {
					return ( false );
        }
      }
    }
    if ( TCCount > 2 ) {
			return ( false );
//      if ( ( len / TCCount ) < 10 ) {
//				return ( false );
//      }
//      if ( TCCount > 4 ) {
//				return ( false );
//      }
    }
  }
  
  iLeftCount = 0;
  iRightCount = 0;
  iLastPos = -1;
  for ( i = 0; i < len; i++ ) {
    if ( iLastPos >= 0 ) {
      if ( ( iLeftCount + iRightCount ) > 0 ) {
        Src[iLastPos] =  g_RepCharset[(BYTE)(Src[iLastPos]) - 0x7f];
        iLastPos = -1;
      }
    }
    if ( (BYTE)Src[i] > (BYTE)'~' ) {
      if ( ( i>0 ) && (iRightCount == 0) ) {
				return ( false );
      }
      iLeftCount = iRightCount;
      iRightCount =0;
      iLastPos = i;
    } else {
			iRightCount = iRightCount + 1;
    }
	}
	return ( true );
}

