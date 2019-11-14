// ProcHotKey.h: interface for the CProcHotKey class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __PROCHOTKEY_HEADER__
#define __PROCHOTKEY_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CProcHotKey
{
public:
	CProcHotKey();
	virtual ~CProcHotKey();

	VOID AppInit( LPCTSTR lpString );
	VOID AppUninit( HWND hWnd );

	INT  GetHotKeyId();
	VOID SetHotKeyId( INT nId );

	BOOL RegisterSpecHotKey( HWND hWnd, DWORD dwHotKey );
	BOOL UnregisterSpecHotKey( HWND hWnd = NULL );

private:
	INT  m_nHotKeyId;
	HWND m_hRegedWnd;

	BOOL m_bGlobalAddAtom;
};

#endif // __PROCHOTKEY_HEADER__


