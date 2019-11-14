// ProcHotKey.cpp: implementation of the CProcHotKey class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "lashou.h"
#include "ProcHotKey.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcHotKey::CProcHotKey()
{
	SYSTEMTIME st;

	GetLocalTime( &st );
	m_nHotKeyId = st.wYear + st.wMonth + st.wDay;

	m_bGlobalAddAtom = FALSE;
}
CProcHotKey::~CProcHotKey()
{
}

VOID CProcHotKey::AppInit( LPCTSTR lpString )
{
	__try
	{
		//	如果调用过，则清除之
		if ( m_bGlobalAddAtom )
		{
			GlobalDeleteAtom( m_nHotKeyId );
		}

		if ( lpString )
		{
			m_nHotKeyId		= GlobalAddAtom( lpString );
			
			//	标记该 ID 是通过函数 GlobalAddAtom 获得，
			//	之后需要调用 GlobalDeleteAtom 来清理
			m_bGlobalAddAtom	= TRUE;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}
VOID CProcHotKey::AppUninit( HWND hWnd )
{
	//	...
	UnregisterSpecHotKey( hWnd );

	//	...
	if ( m_bGlobalAddAtom )
	{
		GlobalDeleteAtom( m_nHotKeyId );
	}
}


INT CProcHotKey::GetHotKeyId()
{
	return m_nHotKeyId;
}
VOID CProcHotKey::SetHotKeyId( INT nId )
{
	m_nHotKeyId = nId;
}

BOOL CProcHotKey::RegisterSpecHotKey( HWND hWnd, DWORD dwHotKey )
{
	BOOL bRet;
	WORD wVirtualKeyCode;
	WORD wModifiers;
	UINT fsModifiers;

	if ( NULL == hWnd )
	{
		return FALSE;
	}
	if ( 0 == dwHotKey )
	{
		return FALSE;
	}

	//	...
	bRet = FALSE;

	__try
	{
		//	...
		m_hRegedWnd = hWnd;
		//UnregisterSpecHotKey( hWnd );

		if ( 0 != dwHotKey )
		{
			//	The low-order word is the virtual key code, 
			//	and the high-order word is the modifier flags
			wVirtualKeyCode	= LOBYTE(dwHotKey);
			wModifiers	= HIBYTE(dwHotKey);
			fsModifiers	= 0;

			if ( wModifiers & HOTKEYF_ALT )
			{
				fsModifiers = MOD_ALT;
			}
			if ( wModifiers & HOTKEYF_CONTROL )
			{
				fsModifiers |= MOD_CONTROL;
			}
			if ( wModifiers & HOTKEYF_SHIFT )
			{
				fsModifiers |= MOD_SHIFT;
			}

			bRet = RegisterHotKey( hWnd, m_nHotKeyId, fsModifiers, wVirtualKeyCode );
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return bRet;
}

BOOL CProcHotKey::UnregisterSpecHotKey( HWND hWnd /*=NULL*/ )
{
	BOOL bRet;

	if ( NULL == hWnd && NULL == m_hRegedWnd )
	{
		return FALSE;
	}

	//	...
	bRet = FALSE;

	__try
	{
		bRet = UnregisterHotKey( hWnd ? hWnd : m_hRegedWnd, m_nHotKeyId );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}

	return bRet;
}