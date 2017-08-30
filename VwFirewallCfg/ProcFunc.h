// ProcFunc.h: interface for the CProcFunc class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __PROCFUNC_HEADER__
#define __PROCFUNC_HEADER__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HyperLink.h"
#include "VwFirewallConfigFile.h"


/**
 *	class of CProcFunc
 */
class CProcFunc
{
public:
	CProcFunc();
	virtual ~CProcFunc();

	BOOL GetToDomain( LPCTSTR lpcszPanDomain, LPTSTR lpszToDomain, DWORD dwSize );
	
	BOOL IsValidPanDomain( LPCTSTR lpcszString );
	BOOL IsValidDomain( LPCTSTR lpcszString );
	BOOL IsValidComputerName( LPCTSTR lpcszString );
	BOOL IsValidPortNumber( LPCTSTR lpcszString );

	BOOL IsValidSubDomain( LPCTSTR lpcszString );
	BOOL IsToDomainExist( LPCTSTR lpcszToDomain );

	BOOL IsValidProtectedDir( LPCTSTR lpcszString );
	BOOL IsValidExceptedProcessFile( LPCTSTR lpcszString );
	BOOL IsValidFileExtName( LPCTSTR lpcszString );
	BOOL IsValidExceptedExt( LPCTSTR lpcszExceptedExt );

	INT  GetListIndexByString( CListCtrl * pListObject, INT nSubItem, LPCTSTR lpcszString, INT nExceptedIndex = -1 );
	INT  GetListIndexByPanDomain( CListCtrl * pListDomain, LPCTSTR lpcszPanDomain, INT nExceptedIndex = -1 );

	BOOL OpenChmHelpPage( WPARAM wParam );

	BOOL MakeAlphaDlg( HWND hWnd, INT nAlphaPercent );
	BOOL BrowseForFolder( LPCTSTR lpcszTitle, LPTSTR lpszPath, DWORD dwSize );

public:
	TCHAR m_szLangCode[ 32 ];
	TCHAR m_szChmFile[ MAX_PATH ];

	CVwFirewallConfigFile m_cVwFirewallConfigFile;

};

#endif // __PROCFUNC_HEADER__
